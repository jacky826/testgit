/*
 * audio.c
 *
 * ============================================================================
 * Copyright (c) Texas Instruments Inc 2009
 *
 * Use of this software is controlled by the terms and conditions found in the
 * license agreement under which this software has been supplied or provided.
 * ============================================================================
 */

#include <stdio.h>
#include <string.h>

#include <xdc/std.h>

#include <ti/sdo/ce/Engine.h>

#include <ti/sdo/dmai/Pause.h>
#include <ti/sdo/dmai/Sound.h>
#include <ti/sdo/dmai/Buffer.h>
#include <ti/sdo/dmai/Loader.h>
#include <ti/sdo/dmai/Rendezvous.h>
#include <ti/sdo/dmai/ce/Aenc1.h>

#include "audio.h"
#include "../../../pvr_debug.h"
#include "../../include/Task_UI/task_ui_var.h"
#include "../../include/config.h"
#include "../../../pvr_platform.h"



#define NUM_WRITER_BUFS_AUDIO 30
/* Cleans up cleanly after a failure */
#define cleanup_enc(x)                                  \
    status = (x);                                   \
    gblSetQuit_enc();                                   \
    goto cleanup_out

extern int inter_frm_num;

//#define AUDIO_LOOPBACK_TEST
#define SUPPORT_SINGLE_CHANNEL_LINEIN

#ifdef SUPPORT_SINGLE_CHANNEL_LINEIN
void duplicate_audio_channel(Buffer_Handle hInBuf)
{
    int i;
    short *channel_datal,*channel_datar,ch_average;

    channel_datal = (short *)Buffer_getUserPtr(hInBuf);
    channel_datar = channel_datal+1;

    for(i=0;i<Buffer_getNumBytesUsed(hInBuf)/4;i++) {
#if 0
        *channel_datar=*channel_datal;
//      *channel_datal=*channel_datar;
#else
        ch_average = (*channel_datar+*channel_datal)/2;
        *channel_datar= ch_average;
        *channel_datal= ch_average;
#endif
        channel_datar+=2;
        channel_datal+=2;
    }
}
#endif

extern Engine_Handle        hEngine;

/******************************************************************************
 * audioThrFxn
 ******************************************************************************/
Void *audioThrFxn_enc(Void *arg)
{
    AudioEnv_enc           *envp                = (AudioEnv_enc *) arg;
    Void                   *status              = THREAD_SUCCESS;
    Sound_Attrs             sAttrs              = Sound_Attrs_STEREO_DEFAULT;
    Buffer_Attrs            bAttrs              = Buffer_Attrs_DEFAULT;
    AUDENC1_Params          defaultParams       = Aenc1_Params_DEFAULT;
    AUDENC1_DynamicParams   defaultDynParams    = Aenc1_DynamicParams_DEFAULT;
    Engine_Handle           hEngine_audio       = NULL;
    Sound_Handle            hSound              = NULL;
    Aenc1_Handle            hAe1                = NULL;
    BufTab_Handle           hBufTab             = NULL;
    Buffer_Handle           hInBuf              = NULL;
    Buffer_Handle           hEncInBuf           = NULL;
    Buffer_Handle           hOutBuf             = NULL;
    Buffer_Handle           hOutBuf_head        = NULL;
    Int                     head_frm            = 0;
    FILE                   *outFile             = NULL;
    AUDENC1_Params         *params;
    AUDENC1_DynamicParams  *dynParams;
    Int                     bufIdx;
    Int                     fifoRet;
    Int                     skip_audio_count    = 0;
    static Int offset = 0;
    static Int wmacodec = 0;

#define SKIP_AUDIO_FRAME   20//about 1s
#define HEADER_AUDIO_FRAME 1  

    /* Open the output file for writing */
    if(envp->audioFile!=NULL) {
        outFile = fopen(envp->audioFile, "w");

        if (outFile == NULL) {
            ERR("Failed to open %s for writing\n", envp->audioFile);
            cleanup_enc(THREAD_FAILURE);
        }
    }  
#if 0    
    /* Open the codec engine */
    hEngine_audio = Engine_open(envp->engineName, NULL, NULL);

    if (hEngine_audio == NULL) {
        ERR("Failed to open codec engine %s\n", envp->engineName);
        cleanup_enc(THREAD_FAILURE);
    }
#else 
    if(hEngine == NULL) {
        ERR("Failed to open codec engine %s\n", envp->engineName);
        cleanup_enc(THREAD_FAILURE);
    }
#endif    
    /* Use supplied params if any, otherwise use defaults */
    params = envp->params ? envp->params : &defaultParams;
    dynParams = envp->dynParams ? envp->dynParams : &defaultDynParams;

    params->sampleRate = dynParams->sampleRate = envp->sampleRate;
    params->bitRate = dynParams->bitRate = envp->soundBitRate;
    
    /* Create the audio encoder */
    hAe1 = Aenc1_create(hEngine/*hEngine_audio*/, envp->audioEncoder, params, dynParams);

    if (hAe1 == NULL) {
        ERR("Failed to create audio encoder: %s\n", envp->audioEncoder);
        cleanup_enc(THREAD_FAILURE);
    }
//reserve 2 bytes for frame type
    LOGT("%s:create audio enc out buffer len 0x%x,skip audio count %d\n",__func__,Aenc1_getOutBufSize(hAe1),SKIP_AUDIO_FRAME);
    hBufTab = BufTab_create(NUM_WRITER_BUFS_AUDIO, Aenc1_getOutBufSize(hAe1)+2, &bAttrs);
    
    hOutBuf_head = Buffer_create(Aenc1_getOutBufSize(hAe1), &bAttrs);

    if (hBufTab == NULL || hOutBuf_head == NULL) {
        ERR("Failed to allocate contiguous audio buffers\n");
        cleanup_enc(THREAD_FAILURE);
    }

    /* Send all buffers to the video thread to be filled with encoded data */
    for (bufIdx = 0; bufIdx < NUM_WRITER_BUFS_AUDIO; bufIdx++) {
        if (Fifo_put(envp->hWriterOutFifo, BufTab_getBuf(hBufTab, bufIdx)) < 0) {
            ERR("Failed to send audio buffer to writer thread\n");
            cleanup_enc(THREAD_FAILURE);
        }
    }

    /* Ask the codec how much input data it needs */
    hInBuf = Buffer_create(Aenc1_getInBufSize(hAe1), &bAttrs);

    if (hInBuf == NULL) {
        ERR("Failed to allocate audio buffers\n");
        cleanup_enc(THREAD_FAILURE);
    }
    
    /* Buffer specifically for WMA to read 8KBytes of data*/
    if(!strcmp(envp->audioEncoder,"wmaenc")) {
        hEncInBuf = Buffer_create(Aenc1_getInBufSize(hAe1)/4, &bAttrs);
        if (hEncInBuf == NULL) {
            ERR("Failed to allocate wma audio buffers\n");
            cleanup_enc(THREAD_FAILURE);
        }
    }

    /* Create the sound device */
    sAttrs.sampleRate = envp->sampleRate;
    sAttrs.soundInput = IsAudioInputMic()?Sound_Input_MIC:Sound_Input_LINE;//envp->soundInput;
    sAttrs.leftGain=127;//pUISystem->allData.encode_volume;
    sAttrs.rightGain=127;//pUISystem->allData.encode_volume;
#ifdef AUDIO_LOOPBACK_TEST
    sAttrs.mode = Sound_Mode_FULLDUPLEX;
#else
    sAttrs.mode = Sound_Mode_INPUT;
#endif
    hSound = Sound_create(&sAttrs);
    LOGT("encode audio gain 0x%x\r\n",sAttrs.leftGain);
    LOGT("audio encode input %d\r\n",sAttrs.soundInput);
    if (hSound == NULL) {
        ERR("Failed to create audio device\n");
        cleanup_enc(THREAD_FAILURE);
    }

    /* Signal that initialization is done and wait for other threads */
    Rendezvous_meet(envp->hRendezvousInit);


    while (!gblGetQuit_enc()) {
#ifdef AUDIO_LOOPBACK_TEST
        /* Read samples from the Sound device */
        if (Sound_read(hSound, hInBuf) < 0) {
            ERR("Failed to write audio buffer\n");
            cleanup_enc(THREAD_FAILURE);
        }
        usleep(100000);// AUDIO_TEST_TAG
        if (Sound_write(hSound, hInBuf) < 0) {
            ERR("Failed to write audio buffer\n");
            cleanup_enc(THREAD_FAILURE);
        }        
#else
        /* Pause processing? */
        Pause_test(envp->hPauseProcess);

        if(skip_audio_count< SKIP_AUDIO_FRAME) {
            if (Sound_read(hSound, hInBuf) < 0) {
                ERR("Failed to write audio buffer\n");
                cleanup_enc(THREAD_FAILURE);
            } 
            skip_audio_count++;       
//            LOGT("audio skip %d\n",skip_audio_count);
            continue; //discard the audio data
        }

        if(Fifo_getNumEntries(envp->hWriterOutFifo)==0) {
            LOGT("%s:sample fifo empty\n",__func__);
        }
        /* Get a buffer to encode to from the writer thread */
        fifoRet = Fifo_get(envp->hWriterOutFifo, &hOutBuf);

        if (fifoRet < 0) {
            ERR("Failed to get audio buffer from writer thread\n");
            cleanup_enc(THREAD_FAILURE);
        }

        /* Did the writer thread flush the fifo? */
        if (fifoRet == Dmai_EFLUSH) {
            cleanup_enc(THREAD_SUCCESS);
        }  
        
        if(!wmacodec)
        {
            if(head_frm < HEADER_AUDIO_FRAME) {
                int i = 0,head_frm_count=0;
              
                for(i=0;i<4;i++) {
                    /* Read samples from the Sound device */
                    if (Sound_read(hSound, hInBuf) < 0) {
                        ERR("Failed to write audio buffer\n");
                        cleanup_enc(THREAD_FAILURE);
                    }
#ifdef SUPPORT_SINGLE_CHANNEL_LINEIN
                    duplicate_audio_channel(hInBuf);
#endif                    
                    /* Encode the audio buffer */
                    if (Aenc1_process(hAe1, hInBuf, hOutBuf_head) < 0) {
                        ERR("Failed to encode audio buffer\n");
                        cleanup_enc(THREAD_FAILURE);
                    }
                    memcpy(Buffer_getUserPtr(hOutBuf)+ head_frm_count,Buffer_getUserPtr(hOutBuf_head),Buffer_getNumBytesUsed(hOutBuf_head));
                    head_frm_count+=Buffer_getNumBytesUsed(hOutBuf_head);
                }    
                Buffer_setNumBytesUsed(hOutBuf,head_frm_count);
                head_frm++;                
            }else {
                /* Read samples from the Sound device */
                if (Sound_read(hSound, hInBuf) < 0) {
                    ERR("Failed to write audio buffer\n");
                    cleanup_enc(THREAD_FAILURE);
                }
#ifdef SUPPORT_SINGLE_CHANNEL_LINEIN
                duplicate_audio_channel(hInBuf);
#endif
                //LOGT("got one audio frm %d\n",inter_frm_num++);
                /* Encode the audio buffer */
                if (Aenc1_process(hAe1, hInBuf, hOutBuf) < 0) {
                    ERR("Failed to encode audio buffer\n");
                    cleanup_enc(THREAD_FAILURE);
                }
            }
            #if 0
            char *pchar = (char *)Buffer_getUserPtr(hOutBuf);
            LOGT("%s:in len 0x%x,enc len 0x%x@0x%x:%x %x %x\n",__func__,Buffer_getNumBytesUsed(hInBuf),Buffer_getNumBytesUsed(hOutBuf),Buffer_getUserPtr(hOutBuf),*pchar,*(pchar+1),*(pchar+2));
            #endif
            if(!strcmp(envp->audioEncoder,"wmaenc"))
            {
                if (Buffer_getNumBytesUsed(hInBuf)) {
                    memcpy(Buffer_getUserPtr(hInBuf),Buffer_getUserPtr(hInBuf)+ 
                        Buffer_getNumBytesUsed(hInBuf), 
                        Buffer_getSize(hInBuf)-Buffer_getNumBytesUsed(hInBuf));
                    offset = Buffer_getSize(hInBuf)-Buffer_getNumBytesUsed(hInBuf);
                }
                else
                    offset = 0;
          
                wmacodec = 1;
            }
        }
        else
        {
            /* For the wma encoder each time 8 Kbytes will be read and appended to the input */

            /* Read samples from the Sound device */
            if (Sound_read(hSound, hEncInBuf) < 0) {
                ERR("Failed to write audio buffer\n");
                cleanup_enc(THREAD_FAILURE);
            }

            memcpy(Buffer_getUserPtr(hInBuf) + offset, Buffer_getUserPtr(hEncInBuf),Buffer_getNumBytesUsed(hEncInBuf));
            Buffer_setNumBytesUsed(hInBuf,Buffer_getSize(hInBuf));

            /* Encode the audio buffer */
            if (Aenc1_process(hAe1, hInBuf, hOutBuf) < 0) {
                ERR("Failed to encode audio buffer\n");
                cleanup_enc(THREAD_FAILURE);
            }
            if (Buffer_getNumBytesUsed(hEncInBuf)) {
                memcpy(Buffer_getUserPtr(hEncInBuf),Buffer_getUserPtr(hEncInBuf) + Buffer_getNumBytesUsed(hEncInBuf), Buffer_getSize(hEncInBuf)  - Buffer_getNumBytesUsed(hEncInBuf));
                offset = Buffer_getSize(hEncInBuf)  - Buffer_getNumBytesUsed(hEncInBuf); 
            }
            else
                offset = 0;
        }
         
        /* Write encoded buffer to the speech file */
        if (!envp->writeDisabled) {
            if (Buffer_getNumBytesUsed(hOutBuf)) {
                *((char *)Buffer_getUserPtr(hOutBuf)+Buffer_getNumBytesUsed(hOutBuf))=AUDIO_FRAME_MASK;
                if(outFile!=NULL) {
                    if (fwrite(Buffer_getUserPtr(hOutBuf),
                           Buffer_getNumBytesUsed(hOutBuf), 1, outFile) != Buffer_getNumBytesUsed(hOutBuf)) {
                        ERR("Error writing the encoded data to speech file.\n");
                        cleanup_enc(THREAD_FAILURE);
                    }
                }
                if(envp->hWriterInFifo){
                    /* Send encoded buffer to writer thread for filesystem output */
                    if (Fifo_put(envp->hWriterInFifo, hOutBuf) < 0) {
                        ERR("Failed to send buffer to writer thread\n");
                        cleanup_enc(THREAD_FAILURE);
                    }
                }
            }
            else {
                LOGW("Warning, zero bytes audio encoded\n");
            }
        }
#endif
    }

cleanup_out:
    /* Make sure the other threads aren't waiting for us */
    Rendezvous_force(envp->hRendezvousInit);
    Pause_off(envp->hPauseProcess);
		
		Fifo_flush(envp->hWriterInFifo);
		LOGT("%s: meet threads\n",__func__);	
    /* Meet up with other threads before cleaning up */
    Rendezvous_meet(envp->hRendezvousCleanup);

    /* Clean up the thread before exiting */
    if (hAe1) {
        Aenc1_delete(hAe1);
    }

    if (hSound) {
        Sound_delete(hSound);
    }

    if (hInBuf) {
        Buffer_delete(hInBuf);
    }
    if (hEncInBuf) {
        Buffer_delete(hEncInBuf);
    }

    if (hBufTab) {
        BufTab_delete(hBufTab);
    }

    if (hOutBuf_head) {
        Buffer_delete(hOutBuf_head);
    } 
#if 0    
    if (hEngine_audio) {
        Engine_close(hEngine_audio);
    }
#endif    
    if (outFile) {
        fclose(outFile);
    }
    LOGT("%s exit\n",__func__);
    return status;
}
