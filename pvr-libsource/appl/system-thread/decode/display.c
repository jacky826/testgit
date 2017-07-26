/*
 * display.c
 *
 * This source file has the implementations for the 'display' function
 * for the DVSDK decode demos on DM365 platform.
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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <unistd.h>

#include <xdc/std.h>

#include <ti/sdo/dmai/Time.h>
#include <ti/sdo/dmai/Framecopy.h>
#include <ti/sdo/dmai/Fifo.h>
#include <ti/sdo/dmai/Pause.h>
#include <ti/sdo/dmai/Display.h>
#include <ti/sdo/dmai/BufferGfx.h>
#include <ti/sdo/dmai/Rendezvous.h>

#include "display.h"
#include "../../../pvr_debug.h"
#include "../../include/pvr_types.h"
#include "../../include/config.h"

// 当前播放状态是否是NormalPlay,如果是,返回1,否则返回0
extern int IsNormalPlaySpeed(void);

/* Display loop delay in us */
#define DISPLAYLOOPLATENCY 33332

/* Buffering for the display driver */
#define NUM_DISPLAY_BUFS         4

extern void gblSetQuit_dec(void);
extern int gblGetQuit_dec(void);
/* Cleans up cleanly after a failure */
#define cleanup_dec(x)                                  \
    status = (x);                                   \
    gblSetQuit_dec();                                   \
    goto cleanup_out

const Display_Attrs Display_Attrs_efDM365_VID_DEFAULT = {
    3,
    Display_Std_V4L2,
    VideoStd_768P_60,
    Display_Output_EFLAGLCD,
    "/dev/video2",
    0,
    ColorSpace_YUV420PSEMI
    -1,
    -1,
    FALSE,
    0,
    0,
    FALSE    
};

#if 0
void disable_cursor()
{
	struct fb_cursor cursor_info;

	cursor_info.enable = 0;
	cursor_info.image.dx = 0;
	cursor_info.image.dy = 0;
	cursor_info.image.width = 50;
	cursor_info.image.height = 50;
	cursor_info.image.depth = 1;
	cursor_info.image.fg_color = 0xf9;

	if (ioctl(fd_vid1, FBIO_SET_CURSOR, &cursor_info) < 0) {
		perror("\nFailed  FBIO_SET_CURSOR");
		return;
	}
}

#endif
#ifdef CONFGI_SAVE_DEC_IMAGE
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> 

void save_image(void *img_buf,int len_img)
{
    static int image_index=0;
    int fd_img;
    char fd_path[128];
    sprintf(fd_path,"/media/cf/dec%d.yuv",image_index++);
    fd_img=open(fd_path,O_RDWR|O_CREAT|O_TRUNC, 0666);
    if(fd_img>0) {
        write(fd_img,img_buf,len_img/*1088*(768+48)*3/2*/);
        close(fd_img);		
        LOGT("%s:save to file %s\n",__func__,fd_path);
    }else {
        LOGT("%s:create file %s failed\n",__func__,fd_path);
    }		

}
#endif

extern double Avi_Get_FrameRate(void);

#ifdef APPEND_AVI_IDX
extern int frm_audio_idx;
#define MAX_VIDEO_BEHIND_AUDIO 20//75
#endif

/******************************************************************************
 * displayThrFxn
 ******************************************************************************/
Void *displayThrFxn(Void *arg)
{
    DisplayEnv             *envp       = (DisplayEnv *) arg;
    Display_Attrs           dAttrs     = Display_Attrs_efDM365_VID_DEFAULT;
    Display_Handle          hDisplay   = NULL;
    Void                   *status     = THREAD_SUCCESS;
    Uns                     frameCnt   = 0;
    BufferGfx_Dimensions    srcDim;
    Buffer_Handle           hSrcBuf, hDstBuf;
    Int                     fifoRet;
    ColorSpace_Type         colorSpace = ColorSpace_YUV420PSEMI;
    BufferGfx_Attrs         gfxAttrs = BufferGfx_Attrs_DEFAULT;
    BufTab_Handle           hBufTab  = NULL;
    Int32                   bufSize;
    Time_Attrs              tAttrs   = Time_Attrs_DEFAULT;
    Time_Handle             hTime    = NULL;
    Int32                   time, waitTime_fr, dynamicTime;
    Int                     bufCnt = 1;
    double                  frame_delay_us=0;
#ifdef APPEND_AVI_IDX
    int                     frm_video_idx,frm_delay;
    LOGT("video behind audio %d\n",MAX_VIDEO_BEHIND_AUDIO);
#endif


    FUNC_ENTER

    hTime = Time_create(&tAttrs);

    if (hTime == NULL) {
        ERR("Failed to create Time object\n");
        cleanup_dec(THREAD_FAILURE);
    }

    if(Time_reset(hTime) != Dmai_EOK) {
        ERR("Failed to reset timer\n");
        cleanup_dec(THREAD_FAILURE);
    }

    /* Signal that initialization is done and wait for other threads */
    Rendezvous_meet(envp->hRendezvousInit);

    frame_delay_us = Avi_Get_FrameRate();

    LOGT("display frame rate %d\n",(int)frame_delay_us);

    if(frame_delay_us!=0) {
        frame_delay_us = 1000000/frame_delay_us;
    }

    while (!gblGetQuit_dec()) {
        /* Pause processing? */
        Pause_test(envp->hPauseProcess);

        /* Pause for priming? */
        Pause_test(envp->hPausePrime);

//        LOGT("%s: <fifo get\n",__func__);		
        /* Get decoded video frame */
        fifoRet = Fifo_get(envp->hInFifo, &hSrcBuf);
//        LOGT("%s: fifo get>\n",__func__);		

        if (fifoRet < 0) {
            ERR("Failed to get buffer from video thread\n");
            cleanup_dec(THREAD_FAILURE);
        }

        /* Did the video thread flush the fifo? */
        if (fifoRet == Dmai_EFLUSH) {
            cleanup_dec(THREAD_SUCCESS);
        }
        
        BufferGfx_getDimensions(hSrcBuf, &srcDim);
 
        /* Prime the display driver with the first NUM_DISPLAY_BUFS buffers */
        if (bufCnt <= NUM_DISPLAY_BUFS) { 
            if (bufCnt == 1) {  // Create the Display at the first frame
                gfxAttrs.dim.width = srcDim.width;
                gfxAttrs.dim.height = srcDim.height;
                gfxAttrs.dim.lineLength = srcDim.lineLength;
                gfxAttrs.dim.x = srcDim.x;
                gfxAttrs.dim.y = srcDim.y;
                if (colorSpace ==  ColorSpace_YUV420PSEMI) {
                    bufSize = gfxAttrs.dim.lineLength * gfxAttrs.dim.height * 
                        3 / 2;
                } else {
                    bufSize = gfxAttrs.dim.lineLength * gfxAttrs.dim.height * 2;
                }

                /* Create a table of buffers to use with the device drivers */
                gfxAttrs.colorSpace = colorSpace;
                hBufTab = BufTab_create(NUM_DISPLAY_BUFS, bufSize,
                    BufferGfx_getBufferAttrs(&gfxAttrs));
                if (hBufTab == NULL) {
                    ERR("Failed to create buftab\n");
                    cleanup_dec(THREAD_FAILURE);
                }
	
                /* Create the display device instance */
                dAttrs.delayStreamon = TRUE;
                dAttrs.numBufs = NUM_DISPLAY_BUFS;
                dAttrs.videoStd = envp->videoStd;
                /* 
                 * Round down the width to a multiple of 32 as required by 
                 * display driver. Otherwise, the driver would internally round
                 * up the width, resulting in the codec padding showing up
                 * on the display when the image width is not a multiple of 32.
                 */
                dAttrs.width = ((gfxAttrs.dim.width & 0x1f) ?
                    (gfxAttrs.dim.width & ~(0x1f)) : gfxAttrs.dim.width);
                dAttrs.height = gfxAttrs.dim.height;
                dAttrs.videoOutput = envp->displayOutput;
                dAttrs.colorSpace  = colorSpace;
                hDisplay = Display_create(hBufTab, &dAttrs);

                if (hDisplay == NULL) {
                    ERR("Failed to create display device\n");
                    cleanup_dec(THREAD_FAILURE);
                }
            }

            bufCnt++;
        }
        else {
            /* Get a buffer from the display device driver */
            if (Display_get(hDisplay, &hDstBuf) < 0) {
                ERR("Failed to get display buffer\n");
                cleanup_dec(THREAD_FAILURE);
            }

            /* Send buffer back to the video thread */
            if (Fifo_put(envp->hOutFifo, hDstBuf) < 0) {
                ERR("Failed to send buffer to video thread\n");
                cleanup_dec(THREAD_FAILURE);
            }
        }
//        LOGT("%s: frame copy>\n",__func__);	
#ifdef APPEND_AVI_IDX
        memcpy(&frm_video_idx ,Buffer_getUserPtr(hSrcBuf),4);
#endif

        if((envp->videoStd == VideoStd_768P_60) && 
			(IsNormalPlaySpeed()))
		{
#ifdef APPEND_AVI_IDX
            frm_delay = 0; // max delay 100ms
            while(((frm_video_idx+MAX_VIDEO_BEHIND_AUDIO)>=frm_audio_idx)&&(frm_delay<20)) {
                usleep(5000); //5ms 
                frm_delay++;
            }
            //LOGT("video frame[idx=0x%x] wait %d ms,fifo left %d\n",frm_video_idx,frm_delay*5,Fifo_getNumEntries(envp->hInFifo));
#else
            if (Time_delta(hTime, (UInt32*)&time) < 0) {
                ERR("Failed to get timer delta\n");
                cleanup_dec(THREAD_FAILURE);
            }
            if(frame_delay_us!=0) {
                waitTime_fr = (int)frame_delay_us - time;
            }else    
                waitTime_fr = DISPLAYLOOPLATENCY - time;

            usleep(waitTime_fr-20);

            if(Time_reset(hTime) != Dmai_EOK) {
                ERR("Failed to reset timer\n");
                cleanup_dec(THREAD_FAILURE);
            }
#endif            
        }
#ifdef CONFGI_SAVE_DEC_IMAGE
        static save_count=0;
        if(save_count==0) {
           LOGT("%s:%d %d %d %d %d %d\n",__func__,srcDim.x,srcDim.y,srcDim.width,srcDim.height,srcDim.lineLength,Buffer_getSize(hSrcBuf));
//                save_count=1;			
           save_image(Buffer_getUserPtr(hSrcBuf),Buffer_getSize(hSrcBuf));
        }
#endif	
//    LOGT("%s: Display_put frameCnt=%d\n",__func__,frameCnt);		
        /* Give a filled buffer back to the display device driver */
        if (Display_put(hDisplay, hSrcBuf) < 0) {
            ERR("Failed to put display buffer\n");
            cleanup_dec(THREAD_FAILURE);
        }
        
        frameCnt++;
    }

cleanup_out:
    /* Make sure the other threads aren't waiting for us */
    Rendezvous_force(envp->hRendezvousInit);
    Pause_off(envp->hPauseProcess);
    Pause_off(envp->hPausePrime);
    Fifo_flush(envp->hOutFifo);
    LOGT("%s: begin cleanup meet\n",__func__);

    /* Meet up with other threads before cleaning up */
    Rendezvous_meet(envp->hRendezvousCleanup);

    if (hDisplay) {
        Display_delete(hDisplay);
        hDisplay = NULL;
    }

    /* Clean up the thread before exiting */
    if (hBufTab) {
        BufTab_delete(hBufTab);
        hBufTab = NULL;
    }

    if(hTime) {
        Time_delete(hTime);
        hTime = NULL;
    }

    return status;
}
