/*
 * loader.c
 *
 * This source file has the implementations for the 'loader' on DM365 platform
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

#include "../../include/config.h"

#ifndef DECODER_USE_SYNCHRONOUS_LOAD
#include <xdc/std.h>

#include <ti/sdo/dmai/Loader.h>

#include "loader.h"
#include "../../../pvr_debug.h"
#include "../../include/pvr_types.h"
#include "../../avi/decoder.h"

extern void gblSetQuit_dec(void);

/* Cleans up cleanly after a failure */
#define cleanup_dec(x)                                  \
    status = (x);                                   \
    gblSetQuit_dec();                                   \
    goto cleanup_out
    
/******************************************************************************
 * loaderThrFxn
 ******************************************************************************/
Void *loaderThrFxn(Void *arg)
{
    LoaderEnv              *envp        = (LoaderEnv *) arg;
    Void                   *status      = THREAD_SUCCESS;
    Int                     ret;

    /* Meet up with other threads before cleaning up */
    Rendezvous_meet(envp->hRendezvousInit);

    if (envp->hLoader == NULL) {
        cleanup_dec(THREAD_FAILURE);
    }

    while (1) {
        /* Load a new frame from the file system */
        ret = Loader_readData(envp->hLoader);

        if (ret < 0) {
            ERR("Failed to load data from video file\n");
            cleanup_dec(THREAD_FAILURE);
        }

        /* Was the end of file encountered */
        if (ret == Dmai_EEOF) {
            if (!envp->loop) {
                break;
            }
        }
        else if (ret == Dmai_EFLUSH) {
            break;
        }
    }

cleanup_out:
    /* Meet up with other threads before cleaning up */
    Rendezvous_meet(envp->hRendezvousCleanup);

    return status;
}
#endif
