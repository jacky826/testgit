/*
 * writer.c
 *
 * This source file has the implementations for the writer thread
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

#include <stdio.h>

#include <xdc/std.h>

#include <ti/sdo/dmai/Fifo.h>
#include <ti/sdo/dmai/BufTab.h>
#include <ti/sdo/dmai/BufferGfx.h>
#include <ti/sdo/dmai/Rendezvous.h>

#include "writer.h"
#include "../../../pvr_debug.h"
#include "../../include/pvr_types.h"
#include "../../avi/encoder.h"
#include "audio.h"
#include <ti/xdais/dm/ividenc.h>

extern int gblSetQuit_enc(void);

/* Cleans up cleanly after a failure */
#define cleanup_enc(x)                                  \
    status = (x);                                   \
    gblSetQuit_enc();                                   \
    goto cleanup_out
    
/* Number of buffers in writer pipe */
#define NUM_WRITER_BUFS         10

//#define SAVE2RAWDATA
/******************************************************************************
 * writerThrFxn
 ******************************************************************************/
Void *writerThrFxn(Void *arg)
{
    WriterEnv          *envp            = (WriterEnv *) arg;
    Void               *status          = THREAD_SUCCESS;
#ifdef SAVE2RAWDATA
    FILE               *outFile         = NULL;
#endif
    Buffer_Attrs        bAttrs          = Buffer_Attrs_DEFAULT;
    BufTab_Handle       hBufTab         = NULL;
    Buffer_Handle       hOutBuf;
    Int                 fifoRet;
    Int                 bufIdx;
    char                frame_type;
    Fifo_Handle         hOutFifo;    

#ifdef SAVE2RAWDATA
    /* Open the output video file */
    outFile = fopen(envp->videoFile, "w");

    if (outFile == NULL) {
        ERR("Failed to open %s for writing\n", envp->videoFile);
        cleanup_enc(THREAD_FAILURE);
    }
#endif
    /*
     * Create a table of buffers for communicating buffers to
     * and from the video thread.
     */
    hBufTab = BufTab_create(NUM_WRITER_BUFS, envp->outBufSize, &bAttrs);

    if (hBufTab == NULL) {
        ERR("Failed to allocate contiguous buffers\n");
        cleanup_enc(THREAD_FAILURE);
    }

    /* Send all buffers to the video thread to be filled with encoded data */
    for (bufIdx = 0; bufIdx < NUM_WRITER_BUFS; bufIdx++) {
        if (Fifo_put(envp->hOutFifo, BufTab_getBuf(hBufTab, bufIdx)) < 0) {
            ERR("Failed to send buffer to display thread\n");
            cleanup_enc(THREAD_FAILURE);
        }
    }

    /* Signal that initialization is done and wait for other threads */
    Rendezvous_meet(envp->hRendezvousInit);

    while (TRUE) {
        /* Get an encoded buffer from the video thread */
        fifoRet = Fifo_get(envp->hInFifo, &hOutBuf);

        if (fifoRet < 0) {
            ERR("Failed to get buffer from video thread\n");
            cleanup_enc(THREAD_FAILURE);
        }

        /* Did the video thread flush the fifo? */
        if (fifoRet == Dmai_EFLUSH) {
            cleanup_enc(THREAD_SUCCESS);
        }

        frame_type = *((char *)Buffer_getUserPtr(hOutBuf)+Buffer_getNumBytesUsed(hOutBuf));         
        *((char *)Buffer_getUserPtr(hOutBuf)+Buffer_getNumBytesUsed(hOutBuf)) = 0;

        if (!envp->writeDisabled) {
            /* Store the encoded frame to disk */
            if (Buffer_getNumBytesUsed(hOutBuf)) {
                if(frame_type == AUDIO_FRAME_MASK) {
                    rec_add_audio(Buffer_getUserPtr(hOutBuf),Buffer_getNumBytesUsed(hOutBuf));
                    //LOGT("%s:get audio frame,len 0x%x@0x%x\n",__func__,Buffer_getNumBytesUsed(hOutBuf),Buffer_getUserPtr(hOutBuf));
                }else {    
#ifdef SAVE2RAWDATA			
                    if (fwrite(Buffer_getUserPtr(hOutBuf),
                               Buffer_getNumBytesUsed(hOutBuf), 1, outFile) != 1) {
                        ERR("Error writing the encoded data to video file\n");
                        cleanup_enc(THREAD_FAILURE);
                    }
#endif			
                    rec_add_video(Buffer_getUserPtr(hOutBuf),
                        Buffer_getNumBytesUsed(hOutBuf),frame_type==IVIDEO_I_FRAME||frame_type==IVIDEO_IDR_FRAME);			
                    //LOGT("%s:get video frame,len 0x%x\n",__func__,Buffer_getNumBytesUsed(hOutBuf));                
                }
//              		LOGT("%s frametype=%d len=%d\n",__func__,frame_type,Buffer_getNumBytesUsed(hOutBuf));			
            }else {
                ERR("Warning, writer received 0 byte encoded frame\n");
            }
        }

        /* Return buffer to capture thread */
#ifdef SUPPORT_AUDIO_ENC        
        if(frame_type == AUDIO_FRAME_MASK) {
            hOutFifo = envp->hOutFifo_audio;
        }else
#endif        
            hOutFifo = envp->hOutFifo;
        
        if (Fifo_put(hOutFifo, hOutBuf) < 0) {
            ERR("Failed to send buffer to display thread\n");
            cleanup_enc(THREAD_FAILURE);
        }
    }

cleanup_out:
    /* Make sure the other threads aren't waiting for us */
    Rendezvous_force(envp->hRendezvousInit);
    Pause_off(envp->hPauseProcess);
    Fifo_flush(envp->hOutFifo);
#ifdef SUPPORT_AUDIO_ENC
    Fifo_flush(envp->hOutFifo_audio);
#endif    

    LOGT("%s: meet threads\n",__func__);	
    /* Meet up with other threads before cleaning up */
    Rendezvous_meet(envp->hRendezvousCleanup);

#ifdef SAVE2RAWDATA
    /* Clean up the thread before exiting */
    if (outFile) {
        fclose(outFile);
    }
#endif

    if (hBufTab) {
        BufTab_delete(hBufTab);
    }
    LOGT("%s exit\n",__func__);
    return status;
}
