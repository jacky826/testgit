/*
 * audio.c
 *
 * This source file has the implementations for the Audio Thread implemented 
 * for the decode demo on DM6467 platform.
 *
 * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/ 
 * 
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

#include <xdc/std.h>

#include <ti/sdo/ce/Engine.h>

#include <ti/sdo/dmai/Pause.h>
#include <ti/sdo/dmai/Sound.h>
#include <ti/sdo/dmai/Buffer.h>
#include <ti/sdo/dmai/Loader.h>
#include <ti/sdo/dmai/Rendezvous.h>
#include <ti/sdo/dmai/ce/Adec1.h>

#include "audio.h"
#include "../../../pvr_debug.h"
#include "../../include/pvr_types.h"
#include "../../include/config.h"


typedef struct tag_fifo_loader_private{
    Buffer_Handle       hBuf;
    Buffer_Handle       hReadBuffer;
    Int32               readBufSize;
    Int32               readSize;
    Int32               readAhead;
    Int8               *w;          /* Where new data is written */
    Int8               *r1;         /* Start of data to be read */
    Memory_AllocParams  mParams;	
    Int8                bendoffile;
    Fifo_Handle         hOutFifo;
    Fifo_Handle         hInFifo;     
}FIFO_LOADER_PRIVATE,*PFIFO_LOADER_PRIVATE;
PFIFO_LOADER_PRIVATE Loader_create_fifo(Loader_Attrs *attrs);
int Loader_prime_fifo(PFIFO_LOADER_PRIVATE pparser, Buffer_Handle *hBufPtr);
int Loader_getFrame_fifo(PFIFO_LOADER_PRIVATE pparser,Buffer_Handle hBuf);
void Loader_flush_fifo(PFIFO_LOADER_PRIVATE pparser);
int Loader_delete_fifo(PFIFO_LOADER_PRIVATE pparser);


#ifdef APPEND_AVI_IDX
int frm_audio_idx;
#define AUDIO_FRM_COUNT 50
struct {
    struct{
        int frm_len;
        int frm_idx;
    }frm_array[AUDIO_FRM_COUNT];
    int idx_add;
    int idx_dec;
}audio_frm_info;

static void add_audio_frm_info(int frm_len,int frm_idx,int fifo_left)
{
    audio_frm_info.frm_array[audio_frm_info.idx_add].frm_len = frm_len;
    audio_frm_info.frm_array[audio_frm_info.idx_add].frm_idx = frm_idx;
    if(++audio_frm_info.idx_add>=AUDIO_FRM_COUNT)
        audio_frm_info.idx_add = 0;
    if(fifo_left==0) {
        //LOGT("got audio frm[idx=0x%x] len=0x%x,fifo left %d\n",frm_idx,frm_len,fifo_left);
    }        
}
static void update_audio_frm_idx(int exhaused_len,int fifo_left)
{
    int next_idx_dec,exhaused_len_copy;
    exhaused_len_copy = exhaused_len;
    while(exhaused_len>0) {
        frm_audio_idx = audio_frm_info.frm_array[audio_frm_info.idx_dec].frm_idx;
        if(audio_frm_info.frm_array[audio_frm_info.idx_dec].frm_len>exhaused_len) {
            audio_frm_info.frm_array[audio_frm_info.idx_dec].frm_len-=exhaused_len;
            exhaused_len = 0;
        }else {
            exhaused_len-=audio_frm_info.frm_array[audio_frm_info.idx_dec].frm_len;     

            audio_frm_info.frm_array[audio_frm_info.idx_dec].frm_len = 0;
            audio_frm_info.frm_array[audio_frm_info.idx_dec].frm_idx = 0;
            
            if(++audio_frm_info.idx_dec>=AUDIO_FRM_COUNT)
                audio_frm_info.idx_dec = 0;
        }
    }
    next_idx_dec = audio_frm_info.idx_dec+1;
    if(next_idx_dec>=AUDIO_FRM_COUNT)
        next_idx_dec = 0;
    if(audio_frm_info.frm_array[next_idx_dec].frm_idx!=0)
        frm_audio_idx = audio_frm_info.frm_array[next_idx_dec].frm_idx;
      
    if(fifo_left==0) {
        LOGT("exhaused audio[idx=0x%x] len=0x%x,fifo left %d\n",frm_audio_idx,exhaused_len_copy,fifo_left);
    }        
}
#endif

#define MAX_INAUDIO_BUF_FRAME_COUNT 20
#define MAX_INAUDIO_BUF_FRAME_SIZE 0x800
/* Cleans up cleanly after a failure */
#define cleanup_dec(x)                                  \
    status = (x);                                   \
    gblSetQuit_dec();                                   \
    goto cleanup_out

extern Engine_Handle        hEngine;

/******************************************************************************
 * audioThrFxn
 ******************************************************************************/
Void *audioThrFxn_dec(Void *arg)
{
    AudioEnv_dec           *envp                = (AudioEnv_dec *) arg;
    Void                   *status              = THREAD_SUCCESS;
    Sound_Attrs             sAttrs              = Sound_Attrs_STEREO_DEFAULT;
    Buffer_Attrs            bAttrs              = Buffer_Attrs_DEFAULT;
    Loader_Attrs            lAttrs              = Loader_Attrs_DEFAULT;
    AUDDEC1_Params          defaultParams       = Adec1_Params_DEFAULT;
    AUDDEC1_DynamicParams   defaultDynParams    = Adec1_DynamicParams_DEFAULT;
    Engine_Handle           hEngine_audio       = NULL;
    Sound_Handle            hSound              = NULL;
    PFIFO_LOADER_PRIVATE    hLoader             = NULL;
    Adec1_Handle            hAd1                = NULL;
    Buffer_Handle           hOutBuf             = NULL;
    Buffer_Handle           hInBuf;
    AUDDEC1_Params         *params;
    AUDDEC1_DynamicParams  *dynParams;
    Int                     ret;
//    int                     skip_count          = 0;//SACH:W10858
    char                    *pchar;
    Buffer_Handle           hPreFillBuf         = NULL;
    int                     prefill_count       = 0;
    int                     error_frm           = 0;
    BufTab_Handle           hInAudioBufTab      = NULL;
    int                     bufIdx;


#define PREFILL_BUF_LEN 32000  //aboutn 0.5s

#define SKIP_AUDIO_DATA_LEN 0//0x8000 //about 0.5s 

#define MAX_ERRFRM_COUNT   10

    //skip_count = SKIP_AUDIO_DATA_LEN;//SACH:W10858

#ifdef APPEND_AVI_IDX
    memset(&audio_frm_info,0,sizeof(audio_frm_info));
    frm_audio_idx = 0;
#endif

#if 0
    /* Open the codec engine */
    hEngine_audio = Engine_open(envp->engineName, NULL, NULL);

    if (hEngine_audio == NULL) {
        ERR("Failed to open codec engine %s\n", envp->engineName);
        cleanup_dec(THREAD_FAILURE);
    }
#else
    if(hEngine == NULL) {
        ERR("Failed to open codec engine %s\n", envp->engineName);
        cleanup_dec(THREAD_FAILURE);
    }
#endif    
    /* Use supplied params if any, otherwise use defaults */
    params = envp->params ? envp->params : &defaultParams;
    dynParams = envp->dynParams ? envp->dynParams : &defaultDynParams; 

    params->dataEndianness = XDM_LE_16;
    

    /* Create the audio decoder */
    hAd1 = Adec1_create(hEngine,/*hEngine_audio,*/ envp->audioDecoder, params, dynParams);

    if (hAd1 == NULL) {
        ERR("Failed to create audio decoder %s\n", envp->audioDecoder);
        cleanup_dec(THREAD_FAILURE);
    }

    /* Create an output buffer for decoded data */
    hOutBuf = Buffer_create(Adec1_getOutBufSize(hAd1) , &bAttrs);

    if (hOutBuf == NULL) {
        ERR("Failed to allocate output buffer\n");
        cleanup_dec(THREAD_FAILURE);
    }
    
    hPreFillBuf = Buffer_create(PREFILL_BUF_LEN*2 ,&bAttrs);

    if (hPreFillBuf == NULL) {
        ERR("Failed to allocate prefill buffer\n");
        cleanup_dec(THREAD_FAILURE);
    }    
    prefill_count = 0;
    
    /* Ask the codec how much input data it needs */
    lAttrs.readSize = Adec1_getInBufSize(hAd1);

    /* Make the total ring buffer larger */
    lAttrs.readBufSize = lAttrs.readSize * 10;

    /* Create the file loader for reading encoded data */
    hLoader = Loader_create_fifo(&lAttrs);

    if (hLoader == NULL) {
        ERR("Failed to create loader\n");
        cleanup_dec(THREAD_FAILURE);
    }
    hLoader->hInFifo = envp->hInFifo;
    hLoader->hOutFifo= envp->hOutFifo;

    hInAudioBufTab = BufTab_create(MAX_INAUDIO_BUF_FRAME_COUNT, MAX_INAUDIO_BUF_FRAME_SIZE, &bAttrs);
    
    if (hInAudioBufTab == NULL) {
        ERR("Failed to allocate audio frame buffers\n");
        cleanup_dec(THREAD_FAILURE);
    }

    /* Send all buffers to the video thread to be filled with encoded data */
    for (bufIdx = 0; bufIdx < MAX_INAUDIO_BUF_FRAME_COUNT; bufIdx++) {
        if (Fifo_put(hLoader->hOutFifo, BufTab_getBuf(hInAudioBufTab, bufIdx)) < 0) {
            ERR("Failed to send audio buffer to loader thread\n");
            cleanup_dec(THREAD_FAILURE);
        }
    }


    /* Signal that initialization is done and wait for other threads */
    Rendezvous_meet(envp->hRendezvousInit);

    /* Prime the file loader */
    ret = Loader_prime_fifo(hLoader, &hInBuf);
    if(ret == Dmai_EOK) {
        pchar=Buffer_getUserPtr(hInBuf);
        if(pchar!=NULL)
            LOGT("first audio frame len 0x%x@0x%x,%x %x %x\r\n",Buffer_getNumBytesUsed(hInBuf),Buffer_getUserPtr(hInBuf),*pchar,*(pchar+1),*(pchar+2));
    }else {
        ERR("Loader_prime_fifo failed\n");
        cleanup_dec(THREAD_FAILURE);
    }
    
    
    /* Decode the audio buffer */
    ret = Adec1_process(hAd1, hInBuf, hOutBuf);
    
    if ((ret == Dmai_EFAIL)||
        (ret == Dmai_EBITERROR && Buffer_getNumBytesUsed(hInBuf) == 0)) {
            ERR("Failed to decode audio buffer. Stream may have ended \
                with partial frame.\n");
            cleanup_dec(THREAD_FAILURE);
    }
    
    /* Create the sound device */
    sAttrs.sampleRate = Adec1_getSampleRate(hAd1);
    sAttrs.leftGain  = 127;//pUISystem->allData.decode_volume;
    sAttrs.rightGain = 127;//pUISystem->allData.decode_volume;
    sAttrs.mode      = Sound_Mode_OUTPUT;
    sAttrs.bufSize   = 4096;
    LOGT("decode audio gain 0x%x\r\n",sAttrs.leftGain);

    while (!gblGetQuit_dec()) {
//        if(skip_count>0) {//SACH:W10858
//            if(Buffer_getNumBytesUsed(hOutBuf)> skip_count) {
//                pchar = Buffer_getUserPtr(hOutBuf); 
//                memcpy(pchar,pchar + skip_count,Buffer_getNumBytesUsed(hOutBuf)-skip_count);
//                Buffer_setNumBytesUsed(hOutBuf,Buffer_getNumBytesUsed(hOutBuf)-skip_count);
//                skip_count = 0;
//            }else {
//                skip_count -= Buffer_getNumBytesUsed(hOutBuf);
//                Buffer_setNumBytesUsed(hOutBuf,0);
//            }
        }else if(hSound==NULL&&Buffer_getNumBytesUsed(hOutBuf)>0) {
            memcpy(Buffer_getUserPtr(hPreFillBuf)+prefill_count,Buffer_getUserPtr(hOutBuf),Buffer_getNumBytesUsed(hOutBuf));
            prefill_count += Buffer_getNumBytesUsed(hOutBuf);
            if(prefill_count>PREFILL_BUF_LEN) {
                hSound = Sound_create(&sAttrs);
                if (hSound == NULL) {
                    ERR("Failed to create audio device\n");
                    cleanup_dec(THREAD_FAILURE);
                }
//                        LOGT("sound create successful\n");
                Buffer_setNumBytesUsed(hPreFillBuf,prefill_count);
                if (Sound_write(hSound, hPreFillBuf) < 0) {
                    ERR("Failed to write prefill audio buffer\n");
                    cleanup_dec(THREAD_FAILURE);
                }
                LOGT("send prefilled buffer to sound 0x%x\n",prefill_count);                
            }    
        }else {    
    //           memset(Buffer_getUserPtr(hOutBuf),0,Buffer_getNumBytesUsed(hOutBuf));
            /* Write the decoded samples to the sound device */
            if (Sound_write(hSound, hOutBuf) < 0) {
                ERR("Failed to write audio buffer\n");
                cleanup_dec(THREAD_FAILURE);
            }
        }    

        /* Pause processing? */
        Pause_test(envp->hPauseProcess);

        /* Load a new frame from the file system */
        if (Loader_getFrame_fifo(hLoader, hInBuf) < 0) {
            ERR("Failed to read a frame of encoded data\n");
            cleanup_dec(THREAD_FAILURE);
        }

        /* Check if the clip has ended */
        if (Buffer_getUserPtr(hInBuf) == NULL) {
            /* Wait for the video clip to finish, if applicable */
            if(envp->hRendezvousLoop!=NULL)
                Rendezvous_meet(envp->hRendezvousLoop);

            /* If we are to loop the clip, start over */
            if (envp->loop) {
                /* Recreate the audio codec */
                Adec1_delete(hAd1);
                hAd1 = Adec1_create(hEngine_audio, envp->audioDecoder,
                                    params, dynParams);

                if (hAd1 == NULL) {
                    ERR("Failed to create audio decoder %s\n",
                        envp->audioDecoder);
                    cleanup_dec(THREAD_FAILURE);
                }

                /* Re-prime the file loader */
                Loader_prime_fifo(hLoader, &hInBuf);
            }
            else {
                LOGW("End of clip reached, exiting..\n");
                cleanup_dec(THREAD_SUCCESS);
            }
        }

        /* Decode the audio buffer */
        ret = Adec1_process(hAd1, hInBuf, hOutBuf);

        if ((ret == Dmai_EFAIL)||
            (ret == Dmai_EBITERROR && Buffer_getNumBytesUsed(hInBuf) == 0)) {
            ERR("Failed to decode audio buffer. Stream may have ended with a \
                partial frame.ret=0x%x count=%d\n",ret,error_frm);
            if(++error_frm>MAX_ERRFRM_COUNT) {
                cleanup_dec(THREAD_FAILURE);
            }else {
                memset(Buffer_getUserPtr(hOutBuf),0,0x900); //about 36ms
                Buffer_setNumBytesUsed(hOutBuf,0x900);
            }
        }else {
            //LOGT("Adec1_process get raw audio 0x%x\n",Buffer_getNumBytesUsed(hOutBuf));    
        }
    }

cleanup_out:
    /* Make sure the other threads aren't waiting for us */
    Rendezvous_force(envp->hRendezvousInit);
    if(envp->hRendezvousLoop!=NULL)
        Rendezvous_force(envp->hRendezvousLoop);
    Pause_off(envp->hPauseProcess);

    LOGT("%s: begin cleanup meet\n",__func__);
    /* Meet up with other threads before cleaning up */
    Rendezvous_meet(envp->hRendezvousCleanup);

    /* Clean up the thread before exiting */
    if (hLoader) {
        Loader_delete_fifo(hLoader);
    }

    if (hAd1) {
        Adec1_delete(hAd1);
    }

    if (hSound) {
        Sound_delete(hSound);
    }

    if (hOutBuf) {
        Buffer_delete(hOutBuf);
    }
#if 0    
    if (hEngine_audio) {
        Engine_close(hEngine_audio);
    }
#endif    
    if (hPreFillBuf) {
        Buffer_delete(hPreFillBuf);
    }

    if (hInAudioBufTab)
        BufTab_delete(hInAudioBufTab);
    return status;
}

/********************************fifo mode audio loader wrapper begin**************************************/
void release_fifo_loader_priv(PFIFO_LOADER_PRIVATE hLoader)
{
    if(hLoader==NULL)
         return;
    if (hLoader->hReadBuffer) {
        Buffer_delete(hLoader->hReadBuffer);
    }
    if (hLoader->hBuf) {
        Buffer_delete(hLoader->hBuf);
    }
    free(hLoader);
}

PFIFO_LOADER_PRIVATE Loader_create_fifo(Loader_Attrs *attrs)
{
    Buffer_Attrs               bAttrs        = Buffer_Attrs_DEFAULT;
    PFIFO_LOADER_PRIVATE       hLoader;
    hLoader = (PFIFO_LOADER_PRIVATE)malloc(sizeof(FIFO_LOADER_PRIVATE));

    if (hLoader == NULL) {
        LOGE("%s:Failed to allocate avi Loader Object\n",__func__);
		request2reboot();
        return NULL;
    }
    memset(hLoader,0,sizeof(FIFO_LOADER_PRIVATE));	
    
    bAttrs.memParams = attrs->mParams;

    LOGT("Creating audio Loader buffer size 0x%x, readSize 0x%x, readahead 0x%x alignment %d\n", 
		    (int)attrs->readBufSize, (int)attrs->readSize, (int)attrs->readAhead,(int)attrs->mParams.align);

    /* Create the ring buffer which will hold the data */
    hLoader->hReadBuffer = Buffer_create(attrs->readBufSize, &bAttrs);

    if (hLoader->hReadBuffer == NULL) {
        LOGT("%s:Failed to allocate ring buffer\n",__func__);
        release_fifo_loader_priv(hLoader);
        return NULL;
    }

    /* Invalidate the created pool in case it is cached */
    if (attrs->mParams.flags == Memory_CACHED)
    {
        Memory_cacheInv(Buffer_getUserPtr(hLoader->hReadBuffer), 
            attrs->readBufSize);
    }

    bAttrs.reference = TRUE;
    hLoader->hBuf = Buffer_create(attrs->readSize, &bAttrs);

    if (hLoader->hBuf == NULL) {
        LOGT("%s:Failed to allocate window buffer object\n",__func__);
        release_fifo_loader_priv(hLoader);
        return NULL;
    }
    hLoader->readBufSize = attrs->readBufSize;	
    hLoader->readSize = attrs->readSize;
    hLoader->readAhead	 = attrs->readAhead;
    return hLoader;	

}

int Loader_prime_fifo(PFIFO_LOADER_PRIVATE hLoader,Buffer_Handle *hBufPtr)
{
    FUNC_ENTER

    *hBufPtr = hLoader->hBuf;
		
    /* Initialize state */
    hLoader->r1 = Buffer_getUserPtr(hLoader->hReadBuffer);
    hLoader->w =  Buffer_getUserPtr(hLoader->hReadBuffer);
    Buffer_setNumBytesUsed(*hBufPtr, 0);		
    return Loader_getFrame_fifo(hLoader,*hBufPtr);
}

#define USE_INDIVIDUAL_AUDIO_BUFTAB

static int Loader_readData_fifo(PFIFO_LOADER_PRIVATE hLoader)
{
    int delta, toRead, numBytes,ret,ret_avi;
    unsigned int ringBufferEnd;
    char *ptr;
    Int   fifoRet;
    BufTab_Handle  hBufTab_ret;
#ifdef APPEND_AVI_IDX
    int audio_idx;
#endif
	
    ringBufferEnd = (unsigned int ) Buffer_getUserPtr(hLoader->hReadBuffer) +
                    (unsigned int ) Buffer_getSize(hLoader->hReadBuffer);

    delta = hLoader->w - hLoader->r1;
    ret = Dmai_EOK;	
#define AUDIO_FRAME_SIZE 0x1b0    
    if(delta<=AUDIO_FRAME_SIZE/*hLoader->readSize*/) {
        memcpy(Buffer_getUserPtr(hLoader->hReadBuffer), hLoader->r1, delta);
        /* Reset ring buffer pointers to beginning of buffer */
        hLoader->w = Buffer_getUserPtr(hLoader->hReadBuffer) + delta;
        hLoader->r1 = Buffer_getUserPtr(hLoader->hReadBuffer);
        
#ifndef USE_INDIVIDUAL_AUDIO_BUFTAB
        //initial the buffer tab to hLoader's internal buffer
        Buffer_setUserPtr(hLoader->hBuf, hLoader->w);
        Buffer_setSize(hLoader->hBuf, ringBufferEnd-(unsigned int)hLoader->w);
        Buffer_setNumBytesUsed(hLoader->hBuf, 0);

        /* Send buffer to video thread */
        if (Fifo_put(hLoader->hOutFifo, hLoader->hBuf) < 0) {
            LOGE("Failed to send buffer to video thread\n");
            return Dmai_EEOF;
        }        
#endif        
        /* Read more data from fifo */
        fifoRet = Fifo_get(hLoader->hInFifo, &hBufTab_ret);
//           LOGT("after Fifo_get()\n");			
			
        if (fifoRet != Dmai_EOK) {
            LOGE("Failed to get buffer from video thread\n");
            return Dmai_EEOF;
        }
#ifdef APPEND_AVI_IDX
        memcpy(&audio_idx,(Buffer_getUserPtr(hBufTab_ret)+Buffer_getNumBytesUsed(hBufTab_ret)),4);
        add_audio_frm_info(Buffer_getNumBytesUsed(hBufTab_ret),audio_idx,Fifo_getNumEntries(hLoader->hInFifo));
#endif
        if(hLoader->w+Buffer_getNumBytesUsed(hBufTab_ret)<ringBufferEnd) {
//                 LOGT("%s:fill frame data 0x%x delta=0x%x\n",__func__,pparser->count,delta);		   	
            //hBufTab_ret has pointed to hLoader->w
#ifdef USE_INDIVIDUAL_AUDIO_BUFTAB            
            memcpy(hLoader->w,Buffer_getUserPtr(hBufTab_ret),Buffer_getNumBytesUsed(hBufTab_ret));
#endif
            hLoader->w+=Buffer_getNumBytesUsed(hBufTab_ret);
        }else {
            LOGT("%s:should not enter[move write pointer:w=0x%x count=0x%x]\n",__func__,hLoader->w-Buffer_getUserPtr(hLoader->hReadBuffer),Buffer_getNumBytesUsed(hBufTab_ret));           
        }
#ifdef USE_INDIVIDUAL_AUDIO_BUFTAB
        /* Send buffer to video thread */
        if (Fifo_put(hLoader->hOutFifo, hBufTab_ret) < 0) {
            LOGE("Failed to send buffer to video thread\n");
            return Dmai_EEOF;
        }                
#endif
    }
    return ret;		
}

int Loader_getFrame_fifo(PFIFO_LOADER_PRIVATE hLoader,Buffer_Handle hBuf)
{
    int ret = Dmai_EEOF;
    int delta;	
	
    /* Increment the read pointers from current buffer */
    hLoader->r1 += Buffer_getNumBytesUsed(hBuf);
#ifdef APPEND_AVI_IDX
    update_audio_frm_idx(Buffer_getNumBytesUsed(hBuf),Fifo_getNumEntries(hLoader->hInFifo));
#endif
//    LOGT("%s:hBuf used len 0x%x\n",__func__,Buffer_getNumBytesUsed(hBuf));	
//    LOGT("%s:buffer used 0x%x,left 0x%x\n",__func__,Buffer_getNumBytesUsed(hBuf),hLoader->w-hLoader->r1);
    if(hLoader->bendoffile==0) {
        ret = Loader_readData_fifo(hLoader);
        if(ret==Dmai_EEOF) {
            LOGT("%s:endof of file,begin flush left 0x%x\n",__func__,hLoader->w - hLoader->r1);	
	        hLoader->bendoffile = 1; 		
        }
        if(ret!=Dmai_EEOF&&ret!=Dmai_EOK)
            return ret;
    }
    delta = hLoader->w - hLoader->r1;
    Buffer_setUserPtr(hBuf, hLoader->r1);
    
    if (delta > hLoader->readSize) {
        Buffer_setSize(hBuf, hLoader->readSize);
        Buffer_setNumBytesUsed(hBuf, hLoader->readSize);
    }else if(delta>0) {
        Buffer_setSize(hBuf, delta);
        Buffer_setNumBytesUsed(hBuf, delta);
    }else {
        Buffer_setUserPtr(hBuf, NULL);
        Buffer_setSize(hBuf, 0);
        Buffer_setNumBytesUsed(hBuf, 0);
        LOGT("%s:endof flush\n",__func__);			
        return Dmai_EEOF;
    }			
    return ret;	 	

}
void Loader_flush_fifo(PFIFO_LOADER_PRIVATE hLoader)
{

}

int Loader_delete_fifo(PFIFO_LOADER_PRIVATE hLoader)
{
    FUNC_ENTER
    release_aviloader_priv(hLoader);		
    FUNC_EXIT
    return 1;	
}


/********************************fifo mode audio loader wrapper end  **************************************/
