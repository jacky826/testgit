/*
 * video.c
 *
 * This source file has the implementations for the video thread
 * functions implemented for 'DVSDK decode demo' on DM365 platform
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
 */

#include <stdio.h>
#include <string.h>

#include <xdc/std.h>

#include <ti/sdo/ce/Engine.h>
#include <ti/sdo/ce/osal/Memory.h>

#include <ti/sdo/dmai/Fifo.h>
#include <ti/sdo/dmai/Pause.h>
#include <ti/sdo/dmai/BufTab.h>
#include <ti/sdo/dmai/Loader.h>
#include <ti/sdo/dmai/VideoStd.h>
#include <ti/sdo/dmai/ce/Vdec2.h>
#include <ti/sdo/codecs/h264dec/ih264vdec.h>
#include <ti/sdo/dmai/BufferGfx.h>
#include <ti/sdo/dmai/Rendezvous.h>

#include "video.h"
#include "../../../pvr_debug.h"
#include "../../include/pvr_types.h"
#include "../../include/config.h"
#include "../../avi/decoder.h"
#include "../../include/UI_Message.h"
#include "../../include/config.h"

/* Buffering for the display driver */
#define NUM_DISPLAY_PIPE_BUFS    15//40
#define NUM_DISPLAY_BUFS         4

/* The masks to use for knowing when a buffer is free */
#define CODEC_FREE              0x1
#define DISPLAY_FREE            0x2

#ifndef YUV_420SP
#define YUV_420SP 256
#endif 

extern void gblSetQuit_dec(void);
extern int gblGetQuit_dec(void);
/* Cleans up cleanly after a failure */
#define cleanup_dec(x)                                  \
    status = (x);                                   \
    gblSetQuit_dec();                                   \
    goto cleanup_out


PlayerFileParser * Loader_create_avi(char *fileName, Loader_Attrs *attrs);
int Loader_prime_avi(PlayerFileParser *pparser, Buffer_Handle *hBufPtr);
int Loader_getFrame_avi(PlayerFileParser *pparser,Buffer_Handle hBuf);
void Loader_flush_avi(PlayerFileParser *pparser);
int Loader_delete_avi(PlayerFileParser *pparser);

typedef struct tag_avi_loader_private{
    Buffer_Handle       hBuf;
    Buffer_Handle       hReadBuffer;
    Int32               readBufSize;
    Int32               readSize;
    Int32               readAhead;
    Int8               *w;          /* Where new data is written */
    Int8               *r1;         /* Start of data to be read */
    Memory_AllocParams  mParams;	
    Int8                bendoffile;
#ifdef SUPPORT_AUDIO_ENC
    Fifo_Handle         hAudioOutFifo;
    Fifo_Handle         hAudioInFifo;   
#endif
}AVI_LOADER_PRIVATE,*PAVI_LOADER_PRIVATE;

/******************************************************************************
 * resizeBufTab
******************************************************************************/
static Int resizeBufTab(Vdec2_Handle hVd2, Int displayBufs)
{
    BufTab_Handle hBufTab = Vdec2_getBufTab(hVd2);
    Int numBufs, numCodecBuffers, numExpBufs;
    Buffer_Handle hBuf;
    Int32 frameSize;

    /* How many buffers can the codec keep at one time? */
    numCodecBuffers = Vdec2_getMinOutBufs(hVd2);

    if (numCodecBuffers < 0) {
        ERR("Failed to get buffer requirements\n");
        return FAILURE;
    }

    /*
     * Total number of frames needed are the number of buffers the codec
     * can keep at any time, plus the number of frames in the display pipe.
     */
    numBufs = numCodecBuffers + displayBufs;
	
    /* Get the size of output buffers needed from codec */
    frameSize = Vdec2_getOutBufSize(hVd2);

    /*
     * Get the first buffer of the BufTab to determine buffer characteristics.
     * All buffers in a BufTab share the same characteristics.
     */
    hBuf = BufTab_getBuf(hBufTab, 0);

    LOGT("%s:numBufs=%d curbuf=%d framesize=0x%x curSize=0x%x\n",__func__,numBufs,BufTab_getNumBufs(hBufTab),frameSize,Buffer_getSize(hBuf));

    /* Do we need to resize the BufTab? */
    if (numBufs > BufTab_getNumBufs(hBufTab) ||
        frameSize < Buffer_getSize(hBuf)) {

        /* Should we break the current buffers in to many smaller buffers? */
        if (frameSize < Buffer_getSize(hBuf)) {

            /*
             * Chunk the larger buffers of the BufTab in to smaller buffers
             * to accomodate the codec requirements.
             */
            numExpBufs = BufTab_chunk(hBufTab, numBufs, frameSize);

            if (numExpBufs < 0) {
                ERR("Failed to chunk %d bufs size %ld to %d bufs size %ld\n",
                    BufTab_getNumBufs(hBufTab), Buffer_getSize(hBuf),
                    numBufs, frameSize);
                return FAILURE;
            }

            /*
             * Did the current BufTab fit the chunked buffers,
             * or do we need to expand the BufTab (numExpBufs > 0)?
             */
            if (BufTab_expand(hBufTab, numExpBufs) < 0) {
                ERR("Failed to expand BufTab with %d buffers after chunk\n",
                    numExpBufs);
                return FAILURE;
            }
        }
        else {
            /* Just expand the BufTab with more buffers */
            if (BufTab_expand(hBufTab, (numBufs - BufTab_getNumBufs(hBufTab))) < 0) {
                ERR("Failed to expand BufTab with %d buffers\n",
                    (numBufs - BufTab_getNumBufs(hBufTab)));
                return FAILURE;
            }
        }
    }

    return numBufs;
}

#ifdef APPEND_AVI_IDX
#define VIDEO_FRM_COUNT 50
struct {
    struct{
        int frm_idx;
    }frm_array[VIDEO_FRM_COUNT];
    int idx_add;
    int idx_dec;
}video_frm_info;

void add_video_idx(PlayerFileParser *pparser)
{
    if(pparser!=NULL) {
        if(pparser->count>0) {
            memcpy(&video_frm_info.frm_array[video_frm_info.idx_add].frm_idx,(pparser->AviVFrameBuffer+pparser->count),4);
//                  LOGT("idx[0x%x] from parser,len[0x%x]\n",video_frm_info.frm_array[video_frm_info.idx_add].frm_idx,pparser->count);
            if(++video_frm_info.idx_add>=VIDEO_FRM_COUNT)
                video_frm_info.idx_add = 0;            
        }
    }
}
int get_video_idx_history(void)
{
    int legacy_idx;

    if(video_frm_info.idx_dec==video_frm_info.idx_add) {
/*Video frame and audio frame 's rate is not 1 to 1,video frame is one frame more than audio frame every 1 second
   There will be consecutive two video frames be send to "add_video_idx" and been decoded to 2 frame.
   The second frame's idx will be same with the first one.
   */        
        legacy_idx=video_frm_info.frm_array[video_frm_info.idx_dec].frm_idx;
    }else {
        legacy_idx=video_frm_info.frm_array[video_frm_info.idx_dec].frm_idx;
        video_frm_info.frm_array[video_frm_info.idx_dec].frm_idx = 0;
        if(++video_frm_info.idx_dec>=VIDEO_FRM_COUNT)
            video_frm_info.idx_dec=0;

    }
    return legacy_idx;
}
#endif

/******************************************************************************
 * handleCodecBufs
******************************************************************************/
static Int handleCodecBufs(Vdec2_Handle hVd2, Fifo_Handle hFifo)
{
    Buffer_Handle hOutBuf, hFreeBuf;
    Int numDisplayBufs = 0;
    UInt16 useMask;
//    LOGT("<%s\n",__func__);	
    /* Get a buffer for display from the codec */
    hOutBuf = Vdec2_getDisplayBuf(hVd2);
    while (hOutBuf) {
        /* Mark the buffer as being used by display thread */
        useMask = Buffer_getUseMask(hOutBuf);
        Buffer_setUseMask(hOutBuf, useMask | DISPLAY_FREE);
#ifdef APPEND_AVI_IDX
        int cur_video_idx;
        cur_video_idx = get_video_idx_history();
        memcpy(Buffer_getUserPtr(hOutBuf),(void *)&cur_video_idx,4);
//        LOGT("send to display thread,idx = 0x%x\n",cur_video_idx);
#endif
        /* Send buffer to display thread */
        if (Fifo_put(hFifo, hOutBuf) < 0) {
            ERR("Failed to send buffer to display thread\n");
            return FAILURE;
        }

        numDisplayBufs++;
        
        /* Get another buffer for display from the codec */
        hOutBuf = Vdec2_getDisplayBuf(hVd2);
    }

    /* Get a buffer to free from the codec */
    hFreeBuf = Vdec2_getFreeBuf(hVd2);
    while (hFreeBuf) {
        /* The codec is no longer using the buffer */
        Buffer_freeUseMask(hFreeBuf, CODEC_FREE);
        hFreeBuf = Vdec2_getFreeBuf(hVd2);
    }
// LOGT("%s: numDisplayBufs = %d>\n",__func__, numDisplayBufs);
    return numDisplayBufs;
}

/******************************************************************************
 * blackFill
 ******************************************************************************/
static Void blackFill(Buffer_Handle hBuf)
{
    Int8  *yPtr     = Buffer_getUserPtr(hBuf);
    Int32  ySize    = Buffer_getSize(hBuf) * 2 / 3;
    Int8  *cbcrPtr  = yPtr + ySize;
    Int    bpp = ColorSpace_getBpp(ColorSpace_YUV420PSEMI);
    Int    y;
    BufferGfx_Dimensions dim;

    BufferGfx_getDimensions(hBuf, &dim); 

    yPtr += dim.y * dim.lineLength + dim.x * bpp / 8;
    for (y = 0; y < dim.height; y++) {
        memset(yPtr, 0x0, dim.width * bpp / 8);
        yPtr += dim.lineLength;
    }

    cbcrPtr += dim.y * dim.lineLength / 2 + dim.x * bpp / 8;
    for (y = 0; y < dim.height / 2; y++) {
        memset(cbcrPtr, 0x80, dim.width * bpp / 8);
        cbcrPtr += dim.lineLength;
    }
}

/******************************************************************************
 * flushDisplayPipe
******************************************************************************/
static Int flushDisplayPipe(Fifo_Handle hFifo, BufTab_Handle hBufTab,
    BufferGfx_Dimensions * dim)
{
   /* Flush the Display pipe with blank buffers */
   Buffer_Handle hBuf;
   Int i;

   for (i = 0; i < NUM_DISPLAY_PIPE_BUFS; i++) {
       hBuf = BufTab_getFreeBuf(hBufTab);
       if (hBuf == NULL) {
           ERR("No free buffer found for flushing display pipe.\n");
           return FAILURE;
       }

       BufferGfx_setDimensions(hBuf, dim);

       blackFill(hBuf);

       /* Send buffer to display thread */
       if (Fifo_put(hFifo, hBuf) < 0) {
           ERR("Failed to send buffer to display thread\n");
           return FAILURE;
       }
   }

   return SUCCESS;
}

extern Engine_Handle        hEngine;

const VIDDEC2_Params Vdec2_efParams_DEFAULT = {
    sizeof(VIDDEC2_Params),             /* size */
    768, 						/* maxHeight */
    1024,                               /* maxWidth */
    30000,                              /* maxFrameRate */
    6000000,                            /* maxBitRate */
    XDM_BYTE,                           /* dataEndianess */
    XDM_YUV_420P,                       /* forceChromaFormat */
};



/******************************************************************************
 * videoThrFxn
 ******************************************************************************/
Void *videoThrFxn_dec(Void *arg)
{
    VideoEnv_dec*           envp                = (VideoEnv_dec *) arg;
    Void                   *status              = THREAD_SUCCESS;
    VIDDEC2_Params          defaultParams       = Vdec2_efParams_DEFAULT;
    VIDDEC2_DynamicParams   defaultDynParams    = Vdec2_DynamicParams_DEFAULT;
    BufferGfx_Attrs         gfxAttrs            = BufferGfx_Attrs_DEFAULT;
    Loader_Attrs            lAttrs              = Loader_Attrs_DEFAULT;
    Vdec2_Handle            hVd2                = NULL;
    void *                  hLoader             = NULL;
    BufTab_Handle           hBufTab             = NULL;
//    BufTab_Handle           hBufTabFlush        = NULL;//SACH:W10853
    Buffer_Handle           hDstBuf, hInBuf, hDispBuf, hBuf;
    Int                     bufIdx, bufsSent, numDisplayBufs, numBufs;
    Int                     totalNumBufs;
    Int                     fifoRet, ret, frameNbr;
    IH264VDEC_Params        extnParams;
    VIDDEC2_Params         *params;
    VIDDEC2_DynamicParams  *dynParams;
    Int32                   bufSize;
    ColorSpace_Type         colorSpace = ColorSpace_YUV420PSEMI;    
    Int                     numFlushBufsSent    = 0;
    Int                     idx;
    BufferGfx_Dimensions    dim;
    Int                     try_count;

    FUNC_ENTER
		
    if(hEngine == NULL) {
        ERR("Failed to open codec engine %s\n", envp->engineName);
        cleanup_dec(THREAD_FAILURE);
    }


    /* Use supplied params if any, otherwise use defaults */
    params = envp->params ? envp->params : &defaultParams;
    dynParams = envp->dynParams ? envp->dynParams : &defaultDynParams;
//    lAttrs.readSize = 1024*768*2;
    lAttrs.readSize = 1024*300;
//    defaultDynParams.frameOrder = IVIDDEC2_DECODE_ORDER;
    
    if (colorSpace ==  ColorSpace_YUV420PSEMI) { 
        params->forceChromaFormat = XDM_YUV_420SP;
    }
//	else {//SACH:W10855
//        params->forceChromaFormat = XDM_YUV_422ILE;
//    }

    /* Create the video decoder */
    hVd2 = Vdec2_create(hEngine, envp->videoDecoder, params, dynParams);

    if (hVd2 == NULL) {
        ERR("Failed to create video decoder: %s\n", envp->videoDecoder);
        cleanup_dec(THREAD_FAILURE);
    }

    /* Which output buffer size does the codec require? */
    bufSize = Vdec2_getOutBufSize(hVd2);

    /* Both the codec and the display thread can own a buffer */
    gfxAttrs.bAttrs.useMask = CODEC_FREE;

    /* Color space */
    gfxAttrs.colorSpace = colorSpace;

    /* Set the original dimensions of the Buffers to the max */
    gfxAttrs.dim.width = params->maxWidth;
    gfxAttrs.dim.height = params->maxHeight;
    gfxAttrs.dim.lineLength = BufferGfx_calcLineLength(gfxAttrs.dim.width,
                                                       colorSpace);

    /* Create a table of buffers for decoded data */
    hBufTab = BufTab_create(NUM_DISPLAY_PIPE_BUFS+12, bufSize,
                            BufferGfx_getBufferAttrs(&gfxAttrs));

    if (hBufTab == NULL) {
        ERR("Failed to create BufTab for display pipe\n");
        cleanup_dec(THREAD_FAILURE);
    }

    /* The codec is going to use this BufTab for output buffers */
    Vdec2_setBufTab(hVd2, hBufTab);

    /* Ask the codec how much input data it needs */
    lAttrs.readSize = Vdec2_getInBufSize(hVd2);
	
    LOGT("%s: readsize = %d\n",__func__,(int)lAttrs.readSize);
	
    /* Let the loader thread read 300000 bytes extra */
    lAttrs.readAhead = 300000;

    /* Make the total ring buffer larger */
    lAttrs.readBufSize = (lAttrs.readSize + lAttrs.readAhead) * 2;
    lAttrs.async = FALSE;

	

    /* Create the file loader for reading encoded data */
    hLoader = Loader_create_avi(envp->videoFile,&lAttrs);
	if (hLoader == NULL) {//SACH:W10890
				ERR("Failed to create loader for file %s\n", envp->videoFile);
				cleanup_dec(THREAD_FAILURE);
	}

#ifdef SUPPORT_AUDIO_ENC
    ((PAVI_LOADER_PRIVATE)(((PlayerFileParser *)hLoader)->preserved))->hAudioOutFifo = envp->hAudioOutFifo;
    ((PAVI_LOADER_PRIVATE)(((PlayerFileParser *)hLoader)->preserved))->hAudioInFifo = envp->hAudioInFifo;
#endif

    

    /* The environment copy will be shared with the loader thread */
    envp->hLoader = hLoader;
	
    /* Make sure the display thread is stopped when it's unlocked */
    Pause_on(envp->hPausePrime);

    /* Signal that initialization is done and wait for other threads */
    Rendezvous_meet(envp->hRendezvousInit);

    frameNbr = 0;
    numDisplayBufs = 0;
prime: 
    /* Initialize the state of the decode */
    ret = Dmai_EOK;

//    if (frameNbr == 0) {//SACH:W10907
//        numBufs = NUM_DISPLAY_PIPE_BUFS;
//    }
//    else {
        /* when looping, the display thread was previously primed */
        numBufs = totalNumBufs - NUM_DISPLAY_PIPE_BUFS;
//    }

    /* Prime the file loader */
    if(Loader_prime_avi(hLoader,&hInBuf)<0) {
        ERR("Failed to prime loader for file %s\n", envp->videoFile);
        cleanup_dec(THREAD_FAILURE);
    }

    /* Prime the display thread with video buffers */
    for (bufIdx=0; bufIdx < NUM_DISPLAY_BUFS; ) {
        if (ret != Dmai_EFIRSTFIELD) {
            /* Get a free buffer from the BufTab */
            hDstBuf = BufTab_getFreeBuf(hBufTab);

            if (hDstBuf == NULL) {
                ERR("Failed to get free buffer from display pipe BufTab\n");
                BufTab_print(hBufTab);
                cleanup_dec(THREAD_FAILURE);
            }
            bufIdx++;
        }

        /* Make sure the whole buffer is used for output */
        BufferGfx_resetDimensions(hDstBuf);
        
        //LOGE("before video dec:0x%x@0x%x--0x%x\n",Buffer_getUserPtr(hInBuf),Buffer_getNumBytesUsed(hInBuf),*(int *)Buffer_getUserPtr(hInBuf));
        /* Decode the video buffer */
        ret = Vdec2_process(hVd2, hInBuf, hDstBuf);
        //LOGE("after video dec:0x%x\n",Buffer_getNumBytesUsed(hInBuf));

        if (ret < 0) {
            ERR("Failed to decode video buffer\n");
            cleanup_dec(THREAD_FAILURE);
        }
//        LOGT("%s: buffer used %d\n",__func__,(int)Buffer_getNumBytesUsed(hInBuf));
        /* If no encoded data was used we cannot find the next frame */
        if (ret == Dmai_EBITERROR && Buffer_getNumBytesUsed(hInBuf) == 0) {
            ERR("Fatal bit error\n");
            cleanup_dec(THREAD_FAILURE);
        }

        /* Increment statistics for the user interface */
//     gblIncVideoBytesProcessed(Buffer_getNumBytesUsed(hInBuf));

        /* Send frames to display thread */
        bufsSent = handleCodecBufs(hVd2, envp->hDisplayInFifo);

        if (bufsSent < 0) {
            cleanup_dec(THREAD_FAILURE);
        }

        /* Keep track of the number of buffers sent to the display thread */
        numDisplayBufs += bufsSent;

        if (frameNbr == 0) {
            /*
             * Resize the BufTab after the first frame has been processed.
             * This because the codec may not know it's buffer requirements
             * before the first frame has been decoded.
             */
            numBufs = resizeBufTab(hVd2, NUM_DISPLAY_PIPE_BUFS);

            if (numBufs < 0) {
                cleanup_dec(THREAD_FAILURE);
            }

            /* 
             * Set the total number of buffers used between video and display 
             * threads.
             */
            totalNumBufs = numBufs;
        }

        /* Load a new encoded frame from the file system */
        if (Loader_getFrame_avi(hLoader , hInBuf) <0) {
            ERR("Failed to get frame of encoded data\n");
            cleanup_dec(THREAD_FAILURE);
        }

        /* End of clip? */
        if (Buffer_getUserPtr(hInBuf) == NULL) {
            LOGT("Clip ended, exiting demo..\n");
            cleanup_dec(THREAD_SUCCESS);
        }

        frameNbr++;
    } /*for (bufIdx=0; bufIdx < numBufs; ) */

    /* Release the display thread, it is now fully primed */
    Pause_off(envp->hPausePrime);

    /* Main loop */
    while (!gblGetQuit_dec()) {
        if (ret != Dmai_EFIRSTFIELD) {
            while(Fifo_getNumEntries(envp->hDisplayOutFifo)>0) {
    //            LOGT("before Fifo_get(),left %d\n",Fifo_getNumEntries(envp->hDisplayOutFifo));            

                /* Get a displayed frame from the display thread */
                fifoRet = Fifo_get(envp->hDisplayOutFifo, &hDispBuf);
    //            LOGT("after Fifo_get()\n");			
    			
                if (fifoRet != Dmai_EOK) {
                    cleanup_dec(THREAD_FAILURE);
                }

                /* Did the display thread flush the fifo? */
                if (fifoRet == Dmai_EFLUSH) {
                    cleanup_dec(THREAD_SUCCESS);
                }

                /* The display thread is no longer using the buffer */
                Buffer_freeUseMask(hDispBuf, DISPLAY_FREE);

                /* Keep track of the number of buffers sent to the display thread */
                numDisplayBufs--;
            }
            try_count = 0;
            /* Get a free buffer from the BufTab to give to the codec */
            while((hDstBuf = BufTab_getFreeBuf(hBufTab))==NULL) {
                if(try_count>0) {
                    /* the code should not arrive here*/
                    ERR("Failed to get free buffer from BufTab,%d vs %d\n",Fifo_getNumEntries(envp->hDisplayOutFifo),Fifo_getNumEntries(envp->hDisplayInFifo));
                    BufTab_print(hBufTab);
                    cleanup_dec(THREAD_FAILURE);
                }
                fifoRet = Fifo_get(envp->hDisplayOutFifo, &hDispBuf);
                if (fifoRet != Dmai_EOK) {
                    cleanup_dec(THREAD_FAILURE);
                }

                /* Did the display thread flush the fifo? */
                if (fifoRet == Dmai_EFLUSH) {
                    cleanup_dec(THREAD_SUCCESS);
                }

                Buffer_freeUseMask(hDispBuf, DISPLAY_FREE);
                numDisplayBufs--;
                try_count++;
            }
        }
        /* Make sure the whole buffer is used for output */
        BufferGfx_resetDimensions(hDstBuf);

//        LOGT("%s:<Vdec2_process\n",__func__);       	
        /* Decode the video buffer */
        ret = Vdec2_process(hVd2, hInBuf, hDstBuf);
//        LOGT("%s:Vdec2_process>\n",__func__);       	
        if (ret < 0) {
            ERR("Failed to decode video buffer\n");
            cleanup_dec(THREAD_FAILURE);
        }

        /* If no encoded data was used we cannot find the next frame */
        if (ret == Dmai_EBITERROR && Buffer_getNumBytesUsed(hInBuf) == 0) {
            ERR("Fatal bit error\n");
            cleanup_dec(THREAD_FAILURE);
        }

        /* Send frames to display thread */
        bufsSent = handleCodecBufs(hVd2, envp->hDisplayInFifo);

        if (bufsSent < 0) {
            cleanup_dec(THREAD_FAILURE);
        }

        /* Keep track of the number of buffers sent to the display thread */
        numDisplayBufs += bufsSent;

        /* Load a new encoded frame from the file system */
        if (Loader_getFrame_avi(hLoader, hInBuf) < 0) {
            ERR("Failed to get frame of encoded data\n");
            cleanup_dec(THREAD_FAILURE);
        }

        frameNbr++;

        /* End of clip? */
        if (Buffer_getUserPtr(hInBuf) == NULL) {
                
            /* Flush the codec for display frames */
            Vdec2_flush(hVd2);
            bufsSent = 0;
            do {
                /*
                 * Temporarily create a dummy buffer for the process call.
                 * After a flush the codec ignores the input buffer, but since 
                 * Codec Engine still address translates the buffer, it needs 
                 * to exist.
                 */
                hInBuf = Buffer_create(1, BufferGfx_getBufferAttrs(&gfxAttrs));

                if (hInBuf == NULL) {
                    ERR("Failed to allocate dummy buffer\n");
                    cleanup_dec(THREAD_FAILURE);
                }

                Buffer_setNumBytesUsed(hInBuf, 1);

                ret = Vdec2_process(hVd2, hInBuf, hDstBuf);

                if (ret < 0) {
                    ERR("Failed to decode video buffer\n");
                    cleanup_dec(THREAD_FAILURE);
                }

                Buffer_delete(hInBuf);
            
                /* Keep track of the number of buffers sent to the display thread */
                numDisplayBufs += bufsSent;

                /* Send frames to display thread */
                bufsSent = handleCodecBufs(hVd2, envp->hDisplayInFifo);

            }while(bufsSent > 0);
#if 0 
            /* Flush the display pipe if not looping */
            if (!envp->loop) {
                /* Create a table of buffers for decoded data */
                gfxAttrs.dim = dim;

                hBufTabFlush = BufTab_create(NUM_DISPLAY_PIPE_BUFS, bufSize,
                            BufferGfx_getBufferAttrs(&gfxAttrs));

                if (hBufTabFlush == NULL) {
                    ERR("Failed to create BufTab for flushing\n");
                    cleanup_dec(THREAD_FAILURE);
                }

                if (flushDisplayPipe(envp->hDisplayInFifo, hBufTabFlush, &dim) 
                    == FAILURE) {
                    cleanup_dec(THREAD_FAILURE);
                }

                numFlushBufsSent = NUM_DISPLAY_PIPE_BUFS;
            }

            /* Drain the display thread making sure all frames are displayed */
            while (numDisplayBufs > (NUM_DISPLAY_PIPE_BUFS - numFlushBufsSent)) {
#else
            while (numDisplayBufs > NUM_DISPLAY_BUFS) {
                LOGT("numDisplayBufs=%d\n",numDisplayBufs);
#endif
                /* Get a displayed frame from the display thread */
                fifoRet = Fifo_get(envp->hDisplayOutFifo, &hDispBuf);

                if (fifoRet != Dmai_EOK) {
                    cleanup_dec(THREAD_FAILURE);
                }

                /* Did the display thread flush the fifo? */
                if (fifoRet == Dmai_EFLUSH) {
                    cleanup_dec(THREAD_SUCCESS);
                }

                /* The display thread is no longer using the buffer */
                Buffer_freeUseMask(hDispBuf, DISPLAY_FREE);

                /* Keep track of number of buffers sent to the display thread */
                numDisplayBufs--;
            }


            /* Loop the clip or quit? */
            if (envp->loop) {
                /* Make sure the decoder has no state by recreating it */
                Vdec2_delete(hVd2);

                /* Make sure any buffers kept by the codec are freed */
                for (idx = 0; idx < BufTab_getNumBufs(hBufTab); idx++) {
                    hBuf = BufTab_getBuf(hBufTab, idx);
                    Buffer_freeUseMask(hBuf, CODEC_FREE);
                }
                hVd2 = Vdec2_create(hEngine, envp->videoDecoder,
                                    params, dynParams);

                if (hVd2 == NULL) {
                    ERR("Failed to create video decoder: %s\n",
                        envp->videoDecoder);
                    cleanup_dec(THREAD_FAILURE);
                }

                /* The codec is going to use this BufTab for output buffers */
                Vdec2_setBufTab(hVd2, hBufTab);

                /* Halt the display thread for priming */
                Pause_on(envp->hPausePrime);

                goto prime;
            }else {
                LOGT("Clip ended, exiting demo..\n");
                gblSetQuit_dec();
            }
        } /* End of clip? */				
    } /* Main loop */

cleanup_out:
//while(1) sleep(20);	
    /* Make sure the other threads aren't waiting for us */
    Rendezvous_force(envp->hRendezvousInit);

    Pause_off(envp->hPauseProcess);
    Pause_off(envp->hPausePrime);
    Fifo_flush(envp->hDisplayInFifo);
    
#ifdef SUPPORT_AUDIO_ENC
    if(IsBoardSupportAudio()) {
        Fifo_flush(envp->hAudioInFifo);
    }    
#endif

    if (hLoader) {
        Loader_flush_avi(hLoader);
    }
    
    Post_UI_Message(MSG_DECODER_EVENT,MSG_PARA_DEC_FILEEND);
    LOGT("%s: begin cleanup meet\n",__func__);	
    /* Meet up with other threads before cleaning up */
    Rendezvous_meet(envp->hRendezvousCleanup);

    /* Clean up the thread before exiting */
    if (hLoader) {
        Loader_delete_avi(hLoader);
        hLoader = NULL;
    }

    if (hVd2) {
        LOGT("%s: hVd2=0x%x\n",__func__,hVd2);
        Vdec2_delete(hVd2);
        hVd2 = NULL;
    }
/*   
at least NUM_DISPLAY_BUFS buffers are left in display device.
compitition between BufTab_delete(hBufTab) and Display_delete(hDisplay) in display thread
*/
    if (hBufTab) {
        BufTab_delete(hBufTab);
        hBufTab = NULL;		
    }
//    if (hBufTabFlush) {//SACH:W10853
//        BufTab_delete(hBufTabFlush);
//        hBufTabFlush = NULL;
//    }	
    LOGT("%s exit status=%d\n",__func__,status);
    return 0;//status; //return value here will reboot the system
}

void release_aviloader_priv(PAVI_LOADER_PRIVATE hLoader)
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

PAVI_LOADER_PRIVATE create_aviload_priv(Loader_Attrs *attrs)
{
    Buffer_Attrs        bAttrs        = Buffer_Attrs_DEFAULT;
    PAVI_LOADER_PRIVATE hLoader;
    hLoader = (PAVI_LOADER_PRIVATE)malloc(sizeof(AVI_LOADER_PRIVATE));

    if (hLoader == NULL) {
        LOGE("%s:Failed to allocate avi Loader Object\n",__func__);
		request2reboot();
        return NULL;
    }
    memset(hLoader,0,sizeof(AVI_LOADER_PRIVATE));	
    
    bAttrs.memParams = attrs->mParams;

    LOGT("Creating Loader buffer size 0x%x, readSize 0x%x, readahead 0x%x alignment %d\n", 
		(int)attrs->readBufSize, (int)attrs->readSize, (int)attrs->readAhead,(int)attrs->mParams.align);

    /* Create the ring buffer which will hold the data */
    hLoader->hReadBuffer = Buffer_create(attrs->readBufSize, &bAttrs);

    if (hLoader->hReadBuffer == NULL) {
        LOGT("%s:Failed to allocate ring buffer\n",__func__);
        release_aviloader_priv(hLoader);
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
        release_aviloader_priv(hLoader);
        return NULL;
    }
    hLoader->readBufSize = attrs->readBufSize;	
    hLoader->readSize = attrs->readSize;
    hLoader->readAhead	 = attrs->readAhead;
    return hLoader;	
}


PlayerFileParser * Loader_create_avi(char *fileName, Loader_Attrs *attrs)
{
    PlayerFileParser *pparser;	
    PAVI_LOADER_PRIVATE hLoader;	

    hLoader = create_aviload_priv(attrs);
    if(hLoader==NULL) {
        LOGE("%s:create_aviload_priv failed\n",__func__);
		request2reboot();
        return NULL;
    }		

    pparser = Player_Init_w(fileName);    	
    if(pparser !=NULL) {
        pparser->preserved = (void *)hLoader;
    }else {
        release_aviloader_priv(hLoader);
    }
    return pparser;	
}
static int Loader_avi_send_audio(PlayerFileParser *pparser)
{
#ifdef SUPPORT_AUDIO_ENC
    if(pparser->audio_count>0&&IsBoardSupportAudio()) {
        Buffer_Handle           hAudioBuf;
        Int                     fifoRet,cp_size;
        PAVI_LOADER_PRIVATE     hLoader;

        hLoader = (PAVI_LOADER_PRIVATE)pparser->preserved;
        
        fifoRet = Fifo_get(hLoader->hAudioOutFifo, &hAudioBuf);
		
        if (fifoRet != Dmai_EOK) {
            LOGT("%s:get audio fifo failed\n",__func__);
            return Dmai_EEOF;
        }
//        LOGT("after Fifo_get():mem@0x%x@0x%xbyte,audio len0x%x,fifo left %d\n",Buffer_getUserPtr(hAudioBuf),Buffer_getSize(hAudioBuf),pparser->audio_count,Fifo_getNumEntries(hLoader->hAudioOutFifo));
#ifdef APPEND_AVI_IDX
        if(Buffer_getSize(hAudioBuf)<(pparser->audio_count+4)) {
		   cp_size = Buffer_getSize(hAudioBuf)-sizeof(int);//SACH:10899
            LOGT("%s:audio frame too long %d vs %d\n",__func__,cp_size,pparser->audio_count);
            
        }else {
            cp_size = pparser->audio_count;
        }    
        //LOGT("%s:after memcpy\n",__func__);
        memcpy(Buffer_getUserPtr(hAudioBuf),pparser->AudioDecInBuffer,cp_size); //copy data payload
        memcpy(Buffer_getUserPtr(hAudioBuf)+cp_size,pparser->AudioDecInBuffer+pparser->audio_count,sizeof(int));//copy avi idx
        
        //cp_size not include the avi idx
#else
        if(Buffer_getSize(hAudioBuf)<pparser->audio_count) {
			cp_size = Buffer_getSize(hAudioBuf);//SACH:10899
			LOGT("%s:audio frame too long %d vs %d\n",__func__,cp_size,pparser->audio_count);
        }else
            cp_size = pparser->audio_count;
        //LOGT("%s:after memcpy\n",__func__);
        memcpy(Buffer_getUserPtr(hAudioBuf),pparser->AudioDecInBuffer,cp_size);
#endif               
        Buffer_setNumBytesUsed(hAudioBuf,cp_size);

        if(Fifo_put(hLoader->hAudioInFifo, hAudioBuf)<0) {
            LOGT("%s:send audio fifo failed\n",__func__);
            return Dmai_EEOF;
        }               
    }    
#endif
    return Dmai_EOK;
}

int Loader_prime_avi(PlayerFileParser *pparser,Buffer_Handle *hBufPtr)
{
    int err;	
    PAVI_LOADER_PRIVATE hLoader;	
	
    FUNC_ENTER
    if(pparser->BeginPrefetch!=NULL) {
        if((err=pparser->BeginPrefetch(pparser)<0)) {
            return Dmai_EIO;		
        }		
    }	
#ifdef APPEND_AVI_IDX
    memset(&video_frm_info,0,sizeof(video_frm_info));
#endif

    if(pparser->Start != NULL) {
        err = pparser->Start(pparser);
        if(err!=ERR_OK) {
            return Dmai_EIO;		
        }else {
            LOGT("pparser->count=0x%x\n",pparser->count);        
            hLoader = (PAVI_LOADER_PRIVATE)pparser->preserved;
            if(pparser->count==0) {
                LOGT("%s:count = 0\n",__func__);				
                return Dmai_EEOF;
            }
            if((err = Loader_avi_send_audio(pparser))!=Dmai_EOK)
                return err;
            *hBufPtr = hLoader->hBuf;
		    if(pparser->count>0) {
                memcpy(Buffer_getUserPtr(hLoader->hReadBuffer),pparser->AviVFrameBuffer,pparser->count);
#ifdef APPEND_AVI_IDX
                add_video_idx(pparser);
#endif
            }    
			
            /* Initialize state */
            hLoader->r1 = Buffer_getUserPtr(hLoader->hReadBuffer);
            hLoader->w =  Buffer_getUserPtr(hLoader->hReadBuffer) + pparser->count;
            Buffer_setNumBytesUsed(*hBufPtr, 0);		
            return Loader_getFrame_avi(pparser,*hBufPtr);
        }
    }
    return Dmai_EEOF;
}

static int Loader_readData_avi(PlayerFileParser *pparser)
{
    int delta, toRead, numBytes,ret,ret_avi;
    unsigned int ringBufferEnd;
    char *ptr;
    PAVI_LOADER_PRIVATE hLoader;
	
    hLoader = (PAVI_LOADER_PRIVATE)pparser->preserved;	

    ringBufferEnd = (unsigned int ) Buffer_getUserPtr(hLoader->hReadBuffer) +
                    (unsigned int ) Buffer_getSize(hLoader->hReadBuffer);

    delta = hLoader->w - hLoader->r1;
    ret = Dmai_EOK;	

#define MIN_SIZE_LEFT_IN_BUFFER 6
    if(delta<MIN_SIZE_LEFT_IN_BUFFER) {
        /* Read more data from file */
        ret_avi = Player_get_data_w(pparser);
        if(ret_avi == 0) {
            if(pparser->count>0) {
                memcpy(hLoader->w,pparser->AviVFrameBuffer,pparser->count);
#ifdef APPEND_AVI_IDX
                add_video_idx(pparser);
#endif       
                hLoader->w+=pparser->count;					
            }
            LOGE("Player_get_data_w return 0,tail video frame len 0x%x\n",pparser->count);				
            return Dmai_EEOF;
        }else if(ret_avi<0) {
            LOGE("Player_get_data_w return %d\n",ret_avi);				
            return Dmai_EIO;
        }
        if((ret = Loader_avi_send_audio(pparser))!=Dmai_EOK)
            return ret;
           
        ptr = hLoader->r1;
        delta = hLoader->w - hLoader->r1;
        memcpy(Buffer_getUserPtr(hLoader->hReadBuffer), ptr, delta);
        /* Reset ring buffer pointers to beginning of buffer */
        hLoader->w = Buffer_getUserPtr(hLoader->hReadBuffer) + delta;
        hLoader->r1 = Buffer_getUserPtr(hLoader->hReadBuffer) ;
        memcpy(hLoader->w,pparser->AviVFrameBuffer,pparser->count);
        hLoader->w+=pparser->count;  
#ifdef APPEND_AVI_IDX
        add_video_idx(pparser);
#endif
    }
    return ret;		
}

int Loader_getFrame_avi(PlayerFileParser *pparser,Buffer_Handle hBuf)
{
    int ret;
    int delta;	
    PAVI_LOADER_PRIVATE hLoader;
	

    hLoader = (PAVI_LOADER_PRIVATE)pparser->preserved;
    /* Increment the read pointers from current buffer */
    hLoader->r1 += Buffer_getNumBytesUsed(hBuf);

//    LOGT("%s:buffer used 0x%x,left 0x%x\n",__func__,Buffer_getNumBytesUsed(hBuf),hLoader->w-hLoader->r1);
    if(pparser->bstop!=0) {
        Buffer_setUserPtr(hBuf, NULL);
        Buffer_setSize(hBuf, 0);
        Buffer_setNumBytesUsed(hBuf, 0);
        LOGT("%s:receive stop command\n",__func__);		
        return Dmai_EEOF;
    }		
    if(hLoader->bendoffile!=0) {
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
        return Dmai_EOK;	 	
    }
    ret = Loader_readData_avi(pparser);

    if(ret == Dmai_EEOF||ret==Dmai_EOK) {
        delta = hLoader->w - hLoader->r1;
        if(ret==Dmai_EEOF) {
            LOGT("%s:endof of file,begin flush left 0x%x\n",__func__,delta);
	        hLoader->bendoffile = 1; 		
        }	
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
	        hLoader->bendoffile = 1;
            LOGT("%s:delta zero,ret=%d\r\n",__func__,ret);
            return Dmai_EEOF;
        }			
        return Dmai_EOK;	 	
    }else {
        LOGT("%s:Loader_readData_avi return %d\r\n",__func__,ret);
    }
    return ret;	
}
void Loader_flush_avi(PlayerFileParser *pparser)
{

}

int Loader_delete_avi(PlayerFileParser *pparser)
{
    FUNC_ENTER
    if(pparser->preserved!=NULL) {
        release_aviloader_priv((PAVI_LOADER_PRIVATE)pparser->preserved);		
        pparser->preserved = NULL;
    }
    Player_Deinit_w(pparser);
    FUNC_EXIT
    return 1;	
}
