/*
 * audio.h
 *
 * ============================================================================
 * Copyright (c) Texas Instruments Inc 2009
 *
 * Use of this software is controlled by the terms and conditions found in the
 * license agreement under which this software has been supplied or provided.
 * ============================================================================
 */

#ifndef _AUDIO_H_PVR_ENC
#define _AUDIO_H_PVR_ENC

#include <xdc/std.h>
#include <ti/sdo/dmai/Fifo.h>
#include <ti/sdo/dmai/Pause.h>
#include <ti/sdo/dmai/Sound.h>
#include <ti/sdo/dmai/Rendezvous.h>

/* Environment passed when creating the thread */
typedef struct AudioEnv_enc {
    Rendezvous_Handle       hRendezvousInit;
    Rendezvous_Handle       hRendezvousCleanup;
    Pause_Handle            hPauseProcess;
    Sound_Input             soundInput;
    Char                   *audioEncoder;
    Char                   *audioFile;
    Char                   *engineName;
    Void                   *params;
    Void                   *dynParams;
    Int                     soundBitRate;
    Int                     sampleRate;
    Bool                    writeDisabled;
    Fifo_Handle             hWriterInFifo;
    Fifo_Handle             hWriterOutFifo;
} AudioEnv_enc;

/* Thread function prototype */
extern Void *audioThrFxn_enc(Void *arg);

#define AUDIO_FRAME_MASK 0xaf

#endif /* _AUDIO_H_PVR_ENC */
