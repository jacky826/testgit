/*
 * video.c
 *
 * This source file has the implementations for the video thread
 * functions implemented for 'DVSDK encode demo' on DM365 platform
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

#include <xdc/std.h>

#include <ti/sdo/ce/Engine.h>

#include <ti/sdo/dmai/Fifo.h>
#include <ti/sdo/dmai/Pause.h>
#include <ti/sdo/dmai/BufTab.h>
#include <ti/sdo/dmai/VideoStd.h>
#include <ti/sdo/dmai/BufferGfx.h>
#include <ti/sdo/dmai/Rendezvous.h>
#include <ti/sdo/dmai/ce/Venc1.h>
#include <ti/sdo/codecs/h264enc/ih264venc.h>

#include "video.h"
#include "../../../pvr_debug.h"
#include "../../include/pvr_types.h"
#include "../../include/UI_Message.h"
#include "../../include/config.h"

extern int gblSetQuit_enc(void);
extern int gblGetQuit_enc(void);

/* Cleans up cleanly after a failure */
#define cleanup_enc(x)                                  \
    status = (x);                                   \
    gblSetQuit_enc();                                   \
    goto cleanup_out
    
#ifndef YUV_420SP
#define YUV_420SP 256
#endif 

/* Number of buffers in the pipe to the capture thread */
/* Note: It needs to match capture.c pipe size */
#define VIDEO_PIPE_SIZE           4

extern Engine_Handle        hEngine;

#define USE_H264_PARAMETER

const VIDENC1_Params Venc1_efParams_DEFAULT = {
   sizeof(VIDENC1_Params),           /* size */
    XDM_DEFAULT,                      /* encodingPreset */
    IVIDEO_LOW_DELAY,                 /* rateControlPreset */
    768,                              /* maxHeight */
    1024,                             /* maxWidth */
    30000,                            /* maxFrameRate */
    10000000,                          /* maxBitRate */
    XDM_BYTE,                         /* dataEndianness */
    1,                                /* maxInterFrameInterval */
    XDM_YUV_420P,                     /* inputChromaFormat */
    IVIDEO_PROGRESSIVE,               /* inputContentType */
    XDM_CHROMA_NA                     /* reconChromaFormat */
};

const VIDENC1_DynamicParams Venc1_efDynamicParams_DEFAULT = {
    sizeof(IVIDENC1_DynamicParams),   /* size */
    768,                              /* maxHeight */
    1024,                             /* inputWidth */
    30000,                            /* refFrameRate */
    30000,                            /* targetFrameRate */
    9000000,                          /* targetBitRate */
    IDR_FRAME_INTERVAL,                               /* intraFrameInterval */
    XDM_ENCODE_AU,                    /* generateHeader */
    0,                                /* captureWidth */
    IVIDEO_NA_FRAME,                  /* forceFrame */
    0,                                /* interFrameInterval */
    0                                 /* mbDataFlag */
};
/******************************************************************************
 * videoThrFxn
 ******************************************************************************/
Void *videoThrFxn_enc(Void *arg)
{
    VideoEnv_enc               *envp                = (VideoEnv_enc *) arg;
    Void                   *status              = THREAD_SUCCESS;

#ifndef USE_H264_PARAMETER
    VIDENC1_Params          defaultParams       = Venc1_efParams_DEFAULT;
    VIDENC1_DynamicParams   defaultDynParams    = Venc1_efDynamicParams_DEFAULT;
#else
    IH264VENC_Params          defaultParams       = IH264VENC_PARAMS;
    IH264VENC_DynamicParams defaultDynParams = H264VENC_TI_IH264VENC_DYNAMICPARAMS;

#ifdef USE_ENCODELIB_VER02300001
    VUIParamBuffer vuip_buf;
    vuip_buf = H264VENC_TI_VUIPARAMBUFFER;
    if(defaultDynParams.VUI_Buffer !=NULL) {
        LOGT("not empty VUI_Buffer 0x%x vs 0x%x\n",defaultDynParams.VUI_Buffer,&H264VENC_TI_VUIPARAMBUFFER);
        LOGT("pixelRange is %d\n",defaultDynParams.VUI_Buffer->pixelRange);	
        defaultDynParams.VUI_Buffer = &vuip_buf;
    }
    vuip_buf.pixelRange = vuip_buf.pixelRange?0:1;	
#endif

#endif

    BufferGfx_Attrs         gfxAttrs            = BufferGfx_Attrs_DEFAULT;
    Venc1_Handle            hVe1                = NULL;

    BufTab_Handle           hBufTab             = NULL;
    Int                     frameCnt            = 0;
    Buffer_Handle           hCapBuf, hDstBuf;
    VIDENC1_Params         *params;
    VIDENC1_DynamicParams  *dynParams;
    Int                     fifoRet;
    Int                     bufIdx;
    ColorSpace_Type         colorSpace = ColorSpace_YUV420PSEMI;
    Bool                    localBufferAlloc = TRUE;
    BufferGfx_Dimensions    dim;
    Int32                   bufSize;
    int frame_type;
	int max_frame_len = 0;

    if (hEngine == NULL) {
        ERR("Failed to open codec engine %s\n", envp->engineName);
        cleanup_enc(THREAD_FAILURE);
    }

    /* In case of HD resolutions the video buffer will be allocated
       by capture thread. */
    if((envp->imageWidth == 1024) && 
        (envp->imageHeight == 768)) {
        localBufferAlloc = FALSE;
    }
//  LOGT("%s:localBufferAlloc = %d\n",__func__,localBufferAlloc);	
	
#ifdef USE_H264_PARAMETER
    defaultParams.videncParams=Venc1_efParams_DEFAULT;
    defaultParams.videncParams.size = sizeof(IH264VENC_Params);
//  defaultParams.profileIdc=66;	
    defaultDynParams.videncDynamicParams=Venc1_efDynamicParams_DEFAULT;
    defaultDynParams.videncDynamicParams.size=sizeof(IH264VENC_DynamicParams);
    defaultDynParams.idrFrameInterval = IDR_FRAME_INTERVAL;
#endif


    /* Use supplied params if any, otherwise use defaults */
    params = envp->params ? envp->params : (VIDENC1_Params *)&defaultParams;
    dynParams = envp->dynParams ? envp->dynParams : &defaultDynParams;

    /* 
     * Set up codec parameters. We round up the height to accomodate for 
     * alignment restrictions from codecs 
     */
    params->maxWidth          = envp->imageWidth;	
    params->maxHeight       = Dmai_roundUp(envp->imageHeight, CODECHEIGHTALIGN);
    params->encodingPreset    = XDM_HIGH_SPEED;
    if (colorSpace ==  ColorSpace_YUV420PSEMI) { 
        params->inputChromaFormat = XDM_YUV_420SP;
    } else {
        params->inputChromaFormat = XDM_YUV_422ILE;
    }
    params->reconChromaFormat = XDM_YUV_420SP;
    params->maxFrameRate      = envp->videoFrameRate;

    /* Set up codec parameters depending on bit rate */
    if (envp->videoBitRate < 0) {
        /* Variable bit rate */
        params->rateControlPreset = IVIDEO_NONE;

        /*
         * If variable bit rate use a bogus bit rate value (> 0)
         * since it will be ignored.
         */
        params->maxBitRate        = 2000000;
    }
    else {
        /* Constant bit rate */
        params->rateControlPreset = IVIDEO_STORAGE;
        params->maxBitRate        = envp->videoBitRate;
    }

    dynParams->targetBitRate   = params->maxBitRate;
    dynParams->inputWidth      = params->maxWidth;
    dynParams->inputHeight     = params->maxHeight;    
    dynParams->refFrameRate    = params->maxFrameRate;
    dynParams->targetFrameRate = params->maxFrameRate;
    dynParams->interFrameInterval = 0;


    /* Create the video encoder */
    hVe1 = Venc1_create(hEngine, envp->videoEncoder, params, dynParams);

    if (hVe1 == NULL) {
        ERR("Failed to create video encoder: %s\n", envp->videoEncoder);
        cleanup_enc(THREAD_FAILURE);
    }

    /* Store the output buffer size in the environment */
    envp->outBufSize = Venc1_getOutBufSize(hVe1);

    LOGT("%s: outBufSize=%d\n",__func__,envp->outBufSize);	

    /* Signal that the codec is created and output buffer size available */
    Rendezvous_meet(envp->hRendezvousWriter);
    if (localBufferAlloc == TRUE) {
        gfxAttrs.colorSpace = colorSpace;
        gfxAttrs.dim.width  = envp->imageWidth;
        gfxAttrs.dim.height = envp->imageHeight;
        gfxAttrs.dim.lineLength = 
            Dmai_roundUp(BufferGfx_calcLineLength(gfxAttrs.dim.width,
                                   gfxAttrs.colorSpace), 32);
        
        /*
         *Calculate size of codec buffers based on rounded LineLength 
         *This will allow to share the buffers with Capture thread. If the
         *size of the buffers was obtained from the codec through 
         *Venc1_getInBufSize() there would be a mismatch with the size of
         * Capture thread buffers when the width is not 32 byte aligned.
         */
        if (colorSpace ==  ColorSpace_YUV420PSEMI) {
            bufSize = gfxAttrs.dim.lineLength * gfxAttrs.dim.height * 3 / 2;
        } 
//        else {//SACH:W10862
//            bufSize = gfxAttrs.dim.lineLength * gfxAttrs.dim.height * 2;
//        } 

        /* Create a table of buffers with size based on rounded LineLength */
        hBufTab = BufTab_create(VIDEO_PIPE_SIZE, bufSize,
                                BufferGfx_getBufferAttrs(&gfxAttrs));

        if (hBufTab == NULL) {
            ERR("Failed to allocate contiguous buffers\n");
            cleanup_enc(THREAD_FAILURE);
        }

        /* Send buffers to the capture thread to be ready for main loop */
        for (bufIdx = 0; bufIdx < VIDEO_PIPE_SIZE; bufIdx++) {
            if (Fifo_put(envp->hCaptureInFifo,
                         BufTab_getBuf(hBufTab, bufIdx)) < 0) {
                ERR("Failed to send buffer to display thread\n");
                cleanup_enc(THREAD_FAILURE);
            }
        }
    } else {
        /* Send buffers to the capture thread to be ready for main loop */
        for (bufIdx = 0; bufIdx < VIDEO_PIPE_SIZE; bufIdx++) {
            fifoRet = Fifo_get(envp->hCaptureOutFifo, &hCapBuf);

            if (fifoRet < 0) {
                ERR("Failed to get buffer from capture thread\n");
                cleanup_enc(THREAD_FAILURE);
            }
            /* Did the capture thread flush the fifo? */
            if (fifoRet == Dmai_EFLUSH) {
                cleanup_enc(THREAD_SUCCESS);
            }
            /* Return buffer to capture thread */
            if (Fifo_put(envp->hCaptureInFifo, hCapBuf) < 0) {
                ERR("Failed to send buffer to display thread\n");
                cleanup_enc(THREAD_FAILURE);
            }
        }
    }
    /* Signal that initialization is done and wait for other threads */
    Rendezvous_meet(envp->hRendezvousInit);

    while (!gblGetQuit_enc()) {
          /* Pause processing? */
        Pause_test(envp->hPauseProcess);

        /* Get a buffer to encode from the capture thread */
        fifoRet = Fifo_get(envp->hCaptureOutFifo, &hCapBuf);
//      LOGT("%s: get frame\n",__func__);		

        if (fifoRet < 0) {
            ERR("Failed to get buffer from video thread\n");
            cleanup_enc(THREAD_FAILURE);
        }

        /* Did the capture thread flush the fifo? */
        if (fifoRet == Dmai_EFLUSH) {
            cleanup_enc(THREAD_SUCCESS);
        }
//        LOGT("%s: Fifo_get\n",__func__);		
        /* Get a buffer to encode to from the writer thread */
        fifoRet = Fifo_get(envp->hWriterOutFifo, &hDstBuf);

        if (fifoRet < 0) {
            ERR("Failed to get buffer from video thread\n");
            cleanup_enc(THREAD_FAILURE);
        }

        /* Did the writer thread flush the fifo? */
        if (fifoRet == Dmai_EFLUSH) {
            cleanup_enc(THREAD_SUCCESS);
        }

        /* Make sure the whole buffer is used for input */
        BufferGfx_resetDimensions(hCapBuf);

        /* Ensure that the video buffer has dimensions accepted by codec */
        BufferGfx_getDimensions(hCapBuf, &dim);
        dim.height = Dmai_roundUp(dim.height, CODECHEIGHTALIGN);
        BufferGfx_setDimensions(hCapBuf, &dim);
		
//      LOGT("%s: Venc1_process\n",__func__);	
        /* Encode the video buffer */
        if (Venc1_process(hVe1, hCapBuf, hDstBuf) < 0) {
            ERR("Failed to encode video buffer\n");
            cleanup_enc(THREAD_FAILURE);
        }
        frame_type = BufferGfx_getFrameType(hCapBuf);
        *((char *)Buffer_getUserPtr(hDstBuf)+Buffer_getNumBytesUsed(hDstBuf))=frame_type;

		if(max_frame_len<Buffer_getNumBytesUsed(hDstBuf)) {
			max_frame_len = Buffer_getNumBytesUsed(hDstBuf);
			LOGT("%s: max_frame_len=%d\n",__func__,max_frame_len);
		}
        /* Send encoded buffer to writer thread for filesystem output */
//        LOGT("%s: hWriterInFifo\n",__func__);	

        /* Reset the dimensions to what they were originally */
        BufferGfx_resetDimensions(hCapBuf);

        /* Send encoded buffer to writer thread for filesystem output */
        if (Fifo_put(envp->hWriterInFifo, hDstBuf) < 0) {
            ERR("Failed to send buffer to writer thread\n");
            cleanup_enc(THREAD_FAILURE);
        }
//        LOGT("%s: Fifo_put\n",__func__);
        /* Return buffer to capture thread */
        if (Fifo_put(envp->hCaptureInFifo, hCapBuf) < 0) {
            ERR("Failed to send buffer to display thread\n");
            cleanup_enc(THREAD_FAILURE);
        }

        frameCnt++;
    }

cleanup_out:
    /* Make sure the other threads aren't waiting for us */
    Rendezvous_force(envp->hRendezvousInit);
    Rendezvous_force(envp->hRendezvousWriter);
    Pause_off(envp->hPauseProcess);
    Fifo_flush(envp->hWriterInFifo);
    Fifo_flush(envp->hCaptureInFifo);

    LOGT("%s: meet threads\n",__func__);	
    /* Make sure the other threads aren't waiting for init to complete */
    Rendezvous_meet(envp->hRendezvousCleanup);

    /* Clean up the thread before exiting */
    if (hBufTab) {
        BufTab_delete(hBufTab);
    }

    if (hVe1) {
        Venc1_delete(hVe1);
    }
    Post_UI_Message(MSG_ENCODER_EVENT,MSG_PARA_ENC_STOP);	
    LOGT("%s exit\n",__func__);
    return status;
}

