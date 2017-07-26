/*
 * capture.c
 *
 * This source file implements the video capture for the encode demo 
 * for DM365 platform.
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
#include <string.h>

#include <ti/sdo/dmai/Fifo.h>
#include <ti/sdo/dmai/Pause.h>
#include <ti/sdo/dmai/BufTab.h>
#include <ti/sdo/dmai/Capture.h>
#include <ti/sdo/dmai/Display.h>
#include <ti/sdo/dmai/VideoStd.h>
#include <ti/sdo/dmai/BufferGfx.h>
#include <ti/sdo/dmai/Rendezvous.h>

#include "capture.h"
#include "../../../pvr_debug.h"
#include "../../include/pvr_types.h"
#include "../../include/config.h"


/* Buffering for the display driver */
#define NUM_DISPLAY_BUFS         3

/* Buffering for the capture driver */
#define NUM_CAPTURE_BUFS         3

/* Number of buffers in the pipe to the capture thread */
/* Note: It needs to match capture.c pipe size */
#define VIDEO_PIPE_SIZE          4 

#define NUM_BUFS (NUM_CAPTURE_BUFS + NUM_DISPLAY_BUFS + VIDEO_PIPE_SIZE)

extern void gblSetQuit_enc(void);
extern int gblGetQuit_enc(void);
/* Black color in UYVY format */
#define UYVY_BLACK      0x10801080

/******************************************************************************
 * CapBuf_blackFill
 * Note: This function uses x, y parameters to calculate the offset for the
 * buffers.
 ******************************************************************************/

Void CapBuf_blackFill(Buffer_Handle hBuf)
{
    switch (BufferGfx_getColorSpace(hBuf)) {
        case ColorSpace_YUV422PSEMI:
        {
            Int8  *yPtr     = Buffer_getUserPtr(hBuf);
            Int32  ySize    = Buffer_getSize(hBuf) / 2;
            Int8  *cbcrPtr  = yPtr + ySize;
            Int    bpp = ColorSpace_getBpp(ColorSpace_YUV422PSEMI);
            Int    y;
            BufferGfx_Dimensions dim;
            UInt32 offset;

            BufferGfx_getDimensions(hBuf, &dim);
            offset = dim.y * dim.lineLength + dim.x * bpp / 8;
            for (y = 0; y < dim.height; y++) {
                memset(yPtr + offset, 0x0, dim.width * bpp / 8);
                yPtr += dim.lineLength;
            }

            for (y = 0; y < dim.height; y++) {
                memset(cbcrPtr + offset, 0x80, dim.width * bpp / 8);
                cbcrPtr += dim.lineLength;
            }

            break;
        }

        case ColorSpace_YUV420PSEMI:
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
            
            break;
        }

        case ColorSpace_UYVY:
        {
            Int32 *bufPtr = (Int32*)Buffer_getUserPtr(hBuf);
            Int    bpp    = ColorSpace_getBpp(ColorSpace_UYVY);
            Int    i, j;
            BufferGfx_Dimensions dim;

            BufferGfx_getDimensions(hBuf, &dim);
            bufPtr += (dim.y * dim.lineLength + dim.x * bpp / 8) / sizeof(Int32);

            /* Make sure display buffer is 4-byte aligned */
            assert((((UInt32) bufPtr) & 0x3) == 0);

            for (i = 0; i < dim.height; i++) {
                for (j = 0; j < dim.width / 2; j++) {
                    bufPtr[j] = UYVY_BLACK;
                }
                bufPtr += dim.lineLength / sizeof(Int32);
            }

            break;
        }

        case ColorSpace_RGB565:
        {
            memset(Buffer_getUserPtr(hBuf), 0, Buffer_getSize(hBuf));
            break;
        }

        default:
        {
                ERR("Unsupported color space (%d) for _Dmai_blackFill\n",
                BufferGfx_getColorSpace(hBuf));
            break;
        }
    }
}
/* Cleans up cleanly after a failure */
#define cleanup_enc(x)                                  \
    status = (x);                                   \
    gblSetQuit_enc();                                   \
    goto cleanup_out
    
/******************************************************************************
 * captureThrFxn
 ******************************************************************************/
const Capture_Attrs Capture_Attrs_EFDM365_DEFAULT = {
    3,
    Capture_Input_EFLAGFPGA,
    -1,
    -1,
    -1,
    -1,
    "/dev/video0",
    FALSE,
    VideoStd_768P_60,
    -1,
    ColorSpace_YUV420PSEMI,
    NULL,
    TRUE
};

int inter_frm_num = 0;

Void *captureThrFxn(Void *arg)
{
    CaptureEnv           *envp     = (CaptureEnv *) arg;
    Void                 *status   = THREAD_SUCCESS;
    Capture_Attrs         cAttrs   = Capture_Attrs_EFDM365_DEFAULT;
    BufferGfx_Attrs       gfxAttrs = BufferGfx_Attrs_DEFAULT;    
    Capture_Handle        hCapture = NULL;
    BufTab_Handle         hBufTab  = NULL;
//    BufTab_Handle         hDispBufTab = NULL;//SACH:W10859
    BufTab_Handle         hFifoBufTab = NULL;
    Buffer_Handle         hDstBuf, hCapBuf, hDisBuf, hBuf;
    BufferGfx_Dimensions  capDim;
    VideoStd_Type         videoStd;
    Int32                 width, height, bufSize;
    Int                   fifoRet;
    ColorSpace_Type       colorSpace = ColorSpace_YUV420PSEMI;
    Int                   bufIdx;
    Int                   numCapBufs;
    Int                   VideoSkip_count=0;

#define VIDEO_SKIP_COUNT 0
    LOGT("%s: videoStd=%d imgWidth=%d imgHeight=%d videoSkipCount=%d\n",__func__,envp->videoStd,envp->imageWidth,envp->imageHeight,VIDEO_SKIP_COUNT);
    videoStd = envp->videoStd;	
    inter_frm_num = 0;
    /* We only support 768p input */
    if (videoStd != VideoStd_768P_60) {
        ERR("Need 768P input to this demo %d\n",videoStd);
        cleanup_enc(THREAD_FAILURE);
    }

    if (envp->imageWidth > 0 && envp->imageHeight > 0) {
        if (VideoStd_getResolution(videoStd, &width, &height) < 0) {
            ERR("Failed to calculate resolution of video standard\n");
            cleanup_enc(THREAD_FAILURE);
        }

        if (width < envp->imageWidth && height < envp->imageHeight) {
            ERR("User resolution (%ldx%ld) larger than detected (%ldx%ld)\n",
                envp->imageWidth, envp->imageHeight, width, height);
            cleanup_enc(THREAD_FAILURE);
        }

       /*
        * Capture driver provides 32 byte aligned data. We 32 byte align the
        * capture and video buffers to perform zero copy encoding.
        */
        envp->imageWidth  = Dmai_roundUp(envp->imageWidth,32);
        capDim.x          = 0;
        capDim.y          = 0;
        capDim.height     = envp->imageHeight;
        capDim.width      = envp->imageWidth;
        capDim.lineLength = BufferGfx_calcLineLength(capDim.width, colorSpace);
    } else {
        /* Calculate the dimensions of a video standard given a color space */
        if (BufferGfx_calcDimensions(videoStd, colorSpace, &capDim) < 0) {
            ERR("Failed to calculate Buffer dimensions\n");
            cleanup_enc(THREAD_FAILURE);
        }
       /*
        * Capture driver provides 32 byte aligned data. We 32 byte align the
        * capture and video buffers to perform zero copy encoding.
        */
        capDim.width      = Dmai_roundUp(capDim.width,32);
        envp->imageWidth  = capDim.width;
        envp->imageHeight = capDim.height;
    }
    numCapBufs = NUM_CAPTURE_BUFS;

    gfxAttrs.dim.height = capDim.height;
    gfxAttrs.dim.width = capDim.width;
    gfxAttrs.dim.lineLength = 
        Dmai_roundUp(BufferGfx_calcLineLength(gfxAttrs.dim.width,
                     colorSpace), 32);
    gfxAttrs.dim.x = 0;
    gfxAttrs.dim.y = 0;
    if (colorSpace ==  ColorSpace_YUV420PSEMI) {
        bufSize = gfxAttrs.dim.lineLength * gfxAttrs.dim.height * 3 / 2;
    } else {
        bufSize = gfxAttrs.dim.lineLength * gfxAttrs.dim.height * 2;
    }

    /* Create a table of buffers to use with the capture driver */
    gfxAttrs.colorSpace = colorSpace;
    hBufTab = BufTab_create(numCapBufs, bufSize,
                            BufferGfx_getBufferAttrs(&gfxAttrs));
    if (hBufTab == NULL) {
        ERR("Failed to create buftab\n");
        cleanup_enc(THREAD_FAILURE);
    }
    //LOGT("%s:capbuf create succ\n",__func__);		
    /* Create a table of buffers to use to prime Fifo to video thread */
    hFifoBufTab = BufTab_create(VIDEO_PIPE_SIZE, bufSize,
                            BufferGfx_getBufferAttrs(&gfxAttrs));
    if (hFifoBufTab == NULL) {
        ERR("Failed to create fifo buftab\n");
        cleanup_enc(THREAD_FAILURE);
    }
    //LOGT("%s:fifobuf create succ\n",__func__);
    /* Report the video standard and image size back to the main thread */
    Rendezvous_meet(envp->hRendezvousCapStd);
    cAttrs.numBufs    = NUM_CAPTURE_BUFS;    
    cAttrs.colorSpace = colorSpace;
    cAttrs.captureDimension = &gfxAttrs.dim;
    /* Create the capture device driver instance */
    hCapture = Capture_create(hBufTab, &cAttrs);
    if (hCapture == NULL) {
        ERR("Failed to create capture device\n");
        cleanup_enc(THREAD_FAILURE);
    }
    //LOGT("%s:capture create succ\n",__func__);
    for (bufIdx = 0; bufIdx < VIDEO_PIPE_SIZE; bufIdx++) {
        /* Queue the video buffers for main thread processing */
        hBuf = BufTab_getFreeBuf(hFifoBufTab);
        if (hBuf == NULL) {
            ERR("Failed to fill video pipeline\n");
            cleanup_enc(THREAD_FAILURE);            
        }
        /* Fill with black the buffer */
        CapBuf_blackFill(hBuf);
        /* Send buffer to video thread for encoding */
        if (Fifo_put(envp->hOutFifo, hBuf) < 0) {
            ERR("Failed to send buffer to display thread\n");
            cleanup_enc(THREAD_FAILURE);
        }
    }
    /* Signal that initialization is done and wait for other threads */
    Rendezvous_meet(envp->hRendezvousInit);

    while (!gblGetQuit_enc()) {
        /* Pause processing? */
        Pause_test(envp->hPauseProcess);
//          LOGT("%s:before get\n",__func__);	
        /* Capture a frame */
        if (Capture_get(hCapture, &hCapBuf) < 0) {
            ERR("Failed to get capture buffer\n");
            cleanup_enc(THREAD_FAILURE);
        }
//        if(VideoSkip_count<VIDEO_SKIP_COUNT) {//SACH:W	10860	
//            VideoSkip_count++;
//            if (Capture_put(hCapture, hCapBuf) < 0) {
//                ERR("Failed to put capture buffer\n");
//                cleanup_enc(THREAD_FAILURE);
//            }            
//            continue; //disacard the frame
//        }
        //LOGT("%s:got frame %d\n",__func__,inter_frm_num++);			
        /* Send buffer to video thread for encoding */
        if (Fifo_put(envp->hOutFifo, hCapBuf) < 0) {
            ERR("Failed to send buffer to display thread\n");
            cleanup_enc(THREAD_FAILURE);
        }
//	       LOGT("%s: before fifo_get\n",__func__);
	    /* Get a buffer from the video thread */
        fifoRet = Fifo_get(envp->hInFifo, &hDstBuf);

        if (fifoRet < 0) {
            ERR("Failed to get buffer from video thread\n");
            cleanup_enc(THREAD_FAILURE);
        }

        /* Did the video thread flush the fifo? */
        if (fifoRet == Dmai_EFLUSH) {
            cleanup_enc(THREAD_SUCCESS);
        }
        
        if (envp->previewDisabled) {
            /* Return the processed buffer to the capture driver */
//          LOGT("%s: before capture_put\n",__func__);
            if (Capture_put(hCapture, hDstBuf) < 0) {
                ERR("Failed to put capture buffer\n");
                cleanup_enc(THREAD_FAILURE);
            }
        }
    }

cleanup_out:
    /* Make sure the other threads aren't waiting for us */
    Rendezvous_force(envp->hRendezvousCapStd);
    Rendezvous_force(envp->hRendezvousInit);
    Pause_off(envp->hPauseProcess);
    Fifo_flush(envp->hOutFifo);
	
    LOGT("%s: meet threads\n",__func__);	
    /* Meet up with other threads before cleaning up */
    Rendezvous_meet(envp->hRendezvousCleanup);

    if (hCapture) {
        Capture_delete(hCapture);
    }
    
    /* Clean up the thread before exiting */
    if (hBufTab) {
        BufTab_delete(hBufTab);
    }
    if (hFifoBufTab) {
        BufTab_delete(hFifoBufTab);
    }
//    if (hDispBufTab) {//SACH:W10859
//        BufTab_delete(hDispBufTab);
//    }
    LOGT("%s exit\n",__func__);
    return status;
}

