#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <strings.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>

#include <xdc/std.h>

#include <ti/sdo/ce/CERuntime.h>

#include <ti/sdo/dmai/Dmai.h>
#include <ti/sdo/dmai/Fifo.h>
#include <ti/sdo/dmai/Pause.h>
#include <ti/sdo/dmai/BufferGfx.h>
#include <ti/sdo/dmai/Rendezvous.h>
#include <ti/sdo/fc/rman/rman.h>

#include <ti/sdo/dmai/Buffer.h>
#include <ti/sdo/dmai/Display.h>

#include "pvr_debug.h"
#include "appl/include/pvr_types.h"

#include "appl/system-thread/encode/capture.h"
#include "appl/system-thread/encode/video.h"
#include "appl/system-thread/encode/writer.h"
#include "appl/system-thread/encode/audio.h"

#include "appl/system-thread/decode/display.h"
#include "appl/system-thread/decode/video.h"
#include "appl/system-thread/decode/loader.h"
#include "appl/system-thread/decode/audio.h"
#include "pvr_platform.h"

#include "appl/include/config.h"

#ifndef USE_SYSLOG
void debug_init(void)
{
    if(freopen("/opt/pvr_log.txt","a",stdout) != NULL)//SACH:W10910
    {
    	 setbuf(stdout,NULL);
    }
   
    if(freopen("/opt/pvr_log.txt","a",stderr) != NULL)
    { 
    		setbuf(stderr,NULL);
	}
    
}
#else
void debug_init(void)
{
    openlog("pvr", LOG_PID | LOG_NDELAY, LOG_DAEMON);
}
void debug_deinit(void)
{
    closelog();
}
#endif

void dump_mem(void *ptr,int len)
{
	int i,*ptr_int;
	ptr_int=(int *)ptr;
	ERR("\nbegin dump 0x%x len=%d: ",(unsigned int)ptr,len);
	for(i=0;i<len/4;i++) {
		ERR("%x ",*(ptr_int+i));
	}
	ERR("end dump\n");
}

/* Defining infinite time */
#define FOREVER         -1

/* The levels of initialization */
#define LOGSINITIALIZED         0x1
#define DISPLAYTHREADCREATED    0x20
#define CAPTURETHREADCREATED    0x40
#define WRITERTHREADCREATED     0x80
#define VIDEOTHREADCREATED      0x100
#define LOADERTHREADCREATED     0x200
#define AUDIOTHREADCREATED      0x400


/* Thread priorities */
#define WRITER_THREAD_PRIORITY  sched_get_priority_max(SCHED_FIFO) - 2
#define AUDIO_THREAD_PRIORITY   sched_get_priority_max(SCHED_FIFO) - 2
#define VIDEO_THREAD_PRIORITY   sched_get_priority_max(SCHED_FIFO) - 1
#define CAPTURE_THREAD_PRIORITY sched_get_priority_max(SCHED_FIFO)

#define LOADER_THREAD_PRIORITY  sched_get_priority_max(SCHED_FIFO) - 2
#define DISPLAY_THREAD_PRIORITY sched_get_priority_max(SCHED_FIFO) 

/*********************************************************************************
**********************************************************************************
**********************************encoder part*************************************
*************************************begin*****************************************
**********************************************************************************
*/

typedef struct tag_encoder_group_thread_status
{
    Uns                 initMask;
    Pause_Attrs         pAttrs;
    Rendezvous_Attrs    rzvAttrs;
    Fifo_Attrs          fAttrs;
    Rendezvous_Handle   hRendezvousCapStd;
    Rendezvous_Handle   hRendezvousInit;
    Rendezvous_Handle   hRendezvousWriter;
    Rendezvous_Handle   hRendezvousCleanup;
    Pause_Handle        hPauseProcess;
    struct sched_param  schedParam;
    pthread_t           captureThread;
    pthread_t           writerThread;
    pthread_t           videoThread;
    pthread_t           speechThread;
#ifdef SUPPORT_AUDIO_ENC
    pthread_t           audioThread;
    AudioEnv_enc        audioEnv;

#endif
    CaptureEnv          captureEnv;
    WriterEnv           writerEnv;
    VideoEnv_enc        videoEnv;
    Int                 numThreads;
    pthread_attr_t      attr;
    int                  quit;	
}encoder_group_thread_status,*pencoder_group_thread_status;

encoder_group_thread_status egts;

Void gblSetQuit_enc(void)
{
    egts.quit = TRUE;
}
Int gblGetQuit_enc(void)
{
    Int quit;

    quit = egts.quit;

    return quit;
}

/* Cleans up cleanly after a failure */
#define cleanup_enc(x)                                  \
    status = (x);                                   \
    gblSetQuit_enc();                                   \
    goto cleanup_out
    
void init_encode_thread_group(void)
{
    FUNC_ENTER
    memset(&egts,0,sizeof(egts));
    egts.pAttrs 		= Pause_Attrs_DEFAULT;
    egts.rzvAttrs            = Rendezvous_Attrs_DEFAULT;
    egts.fAttrs              = Fifo_Attrs_DEFAULT;	
}

typedef struct tag_encArgs {
    VideoStd_Type  videoStd;
    Char          *videoStdString;
    Capture_Input  videoInput;

    Char          *videoFile;
    Int32          imageWidth;
    Int32          imageHeight;

    Int            videoBitRate;
    Int            time;
    Int            osd;

#ifdef SUPPORT_AUDIO_ENC
    Sound_Input    soundInput;
    Char          *audioFile;
    Int            soundBitRate;
    Int            sampleRate;
#endif
} encArgs;

#ifdef SUPPORT_AUDIO_ENC
#define DEFAULT_encARGS \
    { VideoStd_768P_60, "D1 PAL",  Capture_Input_EFLAGFPGA, \
      "/media/ram/test.264",  0, 0, \
      -1, FOREVER, TRUE, \
      /*Sound_Input_MIC*/Sound_Input_LINE, NULL, 96000, 16000 \
    }
#else
#define DEFAULT_encARGS \
    { VideoStd_768P_60, "D1 PAL",  Capture_Input_EFLAGFPGA, \
      "/media/ram/test.264",  0, 0, \
      -1, FOREVER, TRUE}
#endif


int pause_encoder_thread(int pause_action)
{
	if(egts.hPauseProcess!=NULL) {
		if(pause_action!=0) {
			LOGT("%s: pause on\n",__func__);
			Pause_on(egts.hPauseProcess);
		}else {
			LOGT("%s: pause off\n",__func__);
			Pause_off(egts.hPauseProcess);
		}
	}else {
		LOGE("%s:hPauseProcess=NULL\n",__func__);
		request2reboot();
	}	
	return 1;
}

#define USE_MP3_ENCODER

/******************************************************************************
 * start_encoder_thread_group
 ******************************************************************************/
Int start_encoder_thread_group(Int argc, Char *argv[])
{
    encArgs                args                = DEFAULT_encARGS;
    int status;

	
    FUNC_ENTER

    /* Zero out the thread environments */
    Dmai_clear(egts.captureEnv);
    Dmai_clear(egts.writerEnv);
    Dmai_clear(egts.videoEnv);
#ifdef SUPPORT_AUDIO_ENC    
	Dmai_clear(egts.audioEnv);
#endif

    egts.initMask |= LOGSINITIALIZED;

    /* Create the Pause object */
    egts.hPauseProcess = Pause_create(&egts.pAttrs);

    if (egts.hPauseProcess == NULL) {
        ERR("Failed to create Pause object\n");
        cleanup_enc(-1);
    }

    /* Determine the number of threads needing synchronization */
    egts.numThreads =1;

    if (args.videoFile) {
        egts.numThreads += 3;
    }
#ifdef SUPPORT_AUDIO_ENC
    if(IsBoardSupportAudio()) {
        egts.numThreads += 1;
    }    
#endif
    /* Create the objects which synchronizes the thread init and cleanup */
    egts.hRendezvousCapStd  = Rendezvous_create(2, &egts.rzvAttrs);
    egts.hRendezvousInit = Rendezvous_create(egts.numThreads, &egts.rzvAttrs);
    egts.hRendezvousCleanup = Rendezvous_create(egts.numThreads, &egts.rzvAttrs);
    egts.hRendezvousWriter = Rendezvous_create(2, &egts.rzvAttrs);

    if (egts.hRendezvousCapStd  == NULL || egts.hRendezvousInit == NULL || 
        egts.hRendezvousCleanup == NULL || egts.hRendezvousWriter == NULL) {
        ERR("Failed to create Rendezvous objects\n");
        cleanup_enc(-2);
    }

    /* Initialize the thread attributes */
    if (pthread_attr_init(&egts.attr)) {
        ERR("Failed to initialize thread attrs\n");
        cleanup_enc(-3);
    }

    /* Force the thread to use custom scheduling attributes */
    if (pthread_attr_setinheritsched(&egts.attr, PTHREAD_EXPLICIT_SCHED)) {
        ERR("Failed to set schedule inheritance attribute\n");
        cleanup_enc(-4);
    }

    /* Set the thread to be fifo real time scheduled */
    if (pthread_attr_setschedpolicy(&egts.attr, SCHED_FIFO)) {
        ERR("Failed to set FIFO scheduling policy\n");
        cleanup_enc(-5);
    }

    /* Create the video threads if a file name is supplied */
    if (args.videoFile) {
        /* Create the capture fifos */
        egts.captureEnv.hInFifo = Fifo_create(&egts.fAttrs);
        egts.captureEnv.hOutFifo = Fifo_create(&egts.fAttrs);

        if (egts.captureEnv.hInFifo == NULL || egts.captureEnv.hOutFifo == NULL) {
            ERR("Failed to open display fifos\n");
            cleanup_enc(-6);
        }

        /* Set the capture thread priority */
        egts.schedParam.sched_priority = CAPTURE_THREAD_PRIORITY;
        if (pthread_attr_setschedparam(&egts.attr, &egts.schedParam)) {
            ERR("Failed to set scheduler parameters\n");
            cleanup_enc(-7);
        }

        /* Create the capture thread */
        egts.captureEnv.hRendezvousInit    = egts.hRendezvousInit;
        egts.captureEnv.hRendezvousCapStd  = egts.hRendezvousCapStd;
        egts.captureEnv.hRendezvousCleanup = egts.hRendezvousCleanup;
        egts.captureEnv.hPauseProcess      = egts.hPauseProcess;
        egts.captureEnv.videoStd           = args.videoStd;
        egts.captureEnv.videoInput         = args.videoInput;
        egts.captureEnv.imageWidth         = args.imageWidth;
        egts.captureEnv.imageHeight        = args.imageHeight;
        egts.captureEnv.previewDisabled  = 1;		

        if (pthread_create(&egts.captureThread, &egts.attr, captureThrFxn, &egts.captureEnv)) {
            ERR("Failed to create capture thread\n");
            cleanup_enc(-8);
        }

        egts.initMask |= CAPTURETHREADCREATED;

        /*
         * Once the capture thread has detected the video standard, make it
         * available to other threads. The capture thread will set the
         * resolution of the buffer to encode in the environment (derived
         * from the video standard if the user hasn't passed a resolution).
         */
        Rendezvous_meet(egts.hRendezvousCapStd);

        /* Create the writer fifos */
        egts.writerEnv.hInFifo = Fifo_create(&egts.fAttrs);
        egts.writerEnv.hOutFifo = Fifo_create(&egts.fAttrs);

        if (egts.writerEnv.hInFifo == NULL || egts.writerEnv.hOutFifo == NULL) {
            ERR("Failed to open display fifos\n");
            cleanup_enc(-9);
        }
#ifdef SUPPORT_AUDIO_ENC
        if(IsBoardSupportAudio()) {

            egts.writerEnv.hOutFifo_audio = Fifo_create(&egts.fAttrs);

            if (egts.writerEnv.hOutFifo_audio == NULL) {
                ERR("Failed to open audio fifos\n");
                cleanup_enc(-16);
            }

            /* Set the thread priority */
            egts.schedParam.sched_priority = AUDIO_THREAD_PRIORITY;
            if (pthread_attr_setschedparam(&egts.attr, &egts.schedParam)) {
                ERR("Failed to set scheduler parameters\n");
                cleanup_enc(-14);

            }
            
            /* Create the audio thread */
            egts.audioEnv.hRendezvousInit   = egts.hRendezvousInit;
            egts.audioEnv.hRendezvousCleanup = egts.hRendezvousCleanup;
            egts.audioEnv.hPauseProcess     = egts.hPauseProcess;
            egts.audioEnv.engineName        = "pvrcodec";//engine->engineName;
#ifdef USE_MP3_ENCODER
            egts.audioEnv.audioEncoder      = "mp3_enc";
#else
            egts.audioEnv.audioEncoder      = "aacenc";
#endif            
            egts.audioEnv.params            = NULL;
            egts.audioEnv.dynParams         = NULL;
            egts.audioEnv.audioFile         = args.audioFile;
            egts.audioEnv.soundInput        = args.soundInput;
            egts.audioEnv.soundBitRate      = args.soundBitRate;
            egts.audioEnv.sampleRate        = args.sampleRate;
            egts.audioEnv.writeDisabled     = FALSE;//args.writeDisabled;
            egts.audioEnv.hWriterOutFifo    = egts.writerEnv.hOutFifo_audio;
            egts.audioEnv.hWriterInFifo     = egts.writerEnv.hInFifo;

            if (pthread_create(&egts.audioThread, &egts.attr, audioThrFxn_enc, &egts.audioEnv)) {
                ERR("Failed to create audio thread\n");
                cleanup_enc(-15);
            }
            
            egts.initMask |= AUDIOTHREADCREATED;
        }    
        
#endif
        /* Set the video thread priority */
        egts.schedParam.sched_priority = VIDEO_THREAD_PRIORITY;
        if (pthread_attr_setschedparam(&egts.attr, &egts.schedParam)) {
            ERR("Failed to set scheduler parameters\n");
            cleanup_enc(-10);
        }

        /* Create the video thread */
        egts.videoEnv.hRendezvousInit    = egts.hRendezvousInit;
        egts.videoEnv.hRendezvousCleanup = egts.hRendezvousCleanup;
        egts.videoEnv.hRendezvousWriter  = egts.hRendezvousWriter;
        egts.videoEnv.hPauseProcess      = egts.hPauseProcess;
        egts.videoEnv.hCaptureOutFifo    = egts.captureEnv.hOutFifo;
        egts.videoEnv.hCaptureInFifo     = egts.captureEnv.hInFifo;
        egts.videoEnv.hWriterOutFifo     = egts.writerEnv.hOutFifo;
        egts.videoEnv.hWriterInFifo      = egts.writerEnv.hInFifo;
        egts.videoEnv.videoEncoder       = "h264enc";
        egts.videoEnv.params             = NULL;
        egts.videoEnv.dynParams          = NULL;
        egts.videoEnv.videoBitRate       = args.videoBitRate;
        egts.videoEnv.imageWidth         = egts.captureEnv.imageWidth;
        egts.videoEnv.imageHeight        = egts.captureEnv.imageHeight;
        egts.videoEnv.engineName         =  "pvrcodec"; //engine->engineName;
        if (args.videoStd == VideoStd_D1_PAL) {
            egts.videoEnv.videoFrameRate     = 25000;
        } else {
            egts.videoEnv.videoFrameRate     = 30000;
        }

        if (pthread_create(&egts.videoThread, &egts.attr, videoThrFxn_enc, &egts.videoEnv)) {
            ERR("Failed to create video thread\n");
            cleanup_enc(-11);
        }

        egts.initMask |= VIDEOTHREADCREATED;

        /*
         * Wait for the codec to be created in the video thread before
         * launching the writer thread (otherwise we don't know which size
         * of buffers to use).
         */
        Rendezvous_meet(egts.hRendezvousWriter);

        /* Set the writer thread priority */
        egts.schedParam.sched_priority = WRITER_THREAD_PRIORITY;
        if (pthread_attr_setschedparam(&egts.attr, &egts.schedParam)) {
            ERR("Failed to set scheduler parameters\n");
            cleanup_enc(-12);
        }

        /* Create the writer thread */
        egts.writerEnv.hRendezvousInit    = egts.hRendezvousInit;
        egts.writerEnv.hRendezvousCleanup = egts.hRendezvousCleanup;
        egts.writerEnv.hPauseProcess      = egts.hPauseProcess;
        egts.writerEnv.videoFile          = args.videoFile;
        egts.writerEnv.outBufSize         = egts.videoEnv.outBufSize;

        if (pthread_create(&egts.writerThread, &egts.attr, writerThrFxn, &egts.writerEnv)) {
            ERR("Failed to create writer thread\n");
            cleanup_enc(-13);
        }

        egts.initMask |= WRITERTHREADCREATED;
        /* Signal that initialization is done and wait for other threads */
        Rendezvous_meet(egts.hRendezvousInit);

    }
    status = 1;

cleanup_out:
	if(status<0)
		request2reboot();
	
    return status;
}
int stop_encoder_thread_group(void)
{
    int status=1,ret;

    FUNC_ENTER

    /* Make sure the other threads aren't waiting for us */
    if (egts.hRendezvousInit) Rendezvous_force(egts.hRendezvousInit);
	
    Pause_off(egts.hPauseProcess);
    /* Meet up with other threads before cleaning up */
    LOGT("%s: 0 join threads\n",__func__);	
    if(egts.hRendezvousCleanup)	
        Rendezvous_meet(egts.hRendezvousCleanup);
    LOGT("%s: 1 join threads\n",__func__);			
	

    /* Make sure the other threads aren't waiting for init to complete */
    if (egts.hRendezvousCapStd) Rendezvous_force(egts.hRendezvousCapStd);
    LOGT("%s: 2 join threads\n",__func__);			
    if (egts.hRendezvousWriter) Rendezvous_force(egts.hRendezvousWriter);
    LOGT("%s: 3 join threads\n",__func__);			
    if (egts.hRendezvousInit) Rendezvous_force(egts.hRendezvousInit);
//if (egts.hPauseProcess) Pause_off(egts.hPauseProcess);

    LOGT("%s: begin join threads\n",__func__);		
    /* Wait until the other threads terminate */
#ifdef SUPPORT_AUDIO_ENC
    if (egts.initMask & AUDIOTHREADCREATED) {
        if (pthread_join(egts.audioThread, &ret) == 0) {
    		if (ret == THREAD_FAILURE) {
    			status = EXIT_FAILURE;
    		}
    	}
        LOGT("%s: audio thread joined\n",__func__);    
    }
#endif
    if (egts.initMask & VIDEOTHREADCREATED) {
        if (pthread_join(egts.videoThread, &ret) == 0) {
            if (ret == THREAD_FAILURE) {
                status = -1;
            }
        }
    }
    LOGT("%s: video thread joined\n",__func__);
    if (egts.initMask & WRITERTHREADCREATED) {
        if (pthread_join(egts.writerThread, &ret) == 0) {
            if (ret == THREAD_FAILURE) {
                status = -2;
            }
        }
    }
    LOGT("%s: writer thread joined\n",__func__);
    if (egts.writerEnv.hOutFifo) {
        Fifo_delete(egts.writerEnv.hOutFifo);
    }

    if (egts.writerEnv.hInFifo) {
        Fifo_delete(egts.writerEnv.hInFifo);
    }
#ifdef SUPPORT_AUDIO_ENC
    if (egts.writerEnv.hOutFifo_audio) {
        Fifo_delete(egts.writerEnv.hOutFifo_audio);
    }
#endif
    if (egts.initMask & CAPTURETHREADCREATED) {
        if (pthread_join(egts.captureThread, &ret) == 0) {
            if (ret == THREAD_FAILURE) {
                status = -3;
            }
        }
    }

    LOGT("%s: all thread joined\n",__func__);
    if (egts.captureEnv.hOutFifo) {
        Fifo_delete(egts.captureEnv.hOutFifo);
    }

    if (egts.captureEnv.hInFifo) {
        Fifo_delete(egts.captureEnv.hInFifo);
    }

    if (egts.hRendezvousCleanup) {
        Rendezvous_delete(egts.hRendezvousCleanup);
    }

    if (egts.hRendezvousInit) {
        Rendezvous_delete(egts.hRendezvousInit);
    }

    if (egts.hPauseProcess) {
        Pause_delete(egts.hPauseProcess);
    }


    sync();
	sync();
    system("echo 3 > /proc/sys/vm/drop_caches");
	if(status<0)
		request2reboot();

    return status;	
}

/*
********************************************************************************
**********************************************************************************
**********************************encoder part*************************************
*************************************end******************************************
**********************************************************************************
*/


/*
********************************************************************************
**********************************************************************************
**********************************decoder part*************************************
*************************************begin******************************************
**********************************************************************************
*/
typedef struct tag_decoder_group_thread_status {
    Uns                     initMask;
    Pause_Attrs             pAttrs;
    Rendezvous_Attrs        rzvAttrs;
    Fifo_Attrs              fAttrs;
    Rendezvous_Handle       hRendezvousInit;
    Rendezvous_Handle       hRendezvousCleanup;
    Rendezvous_Handle       hRendezvousLoader;
    Pause_Handle            hPauseProcess;
    Pause_Handle            hPausePrime;
    Int                     syncCnt;
    struct sched_param      schedParam;
    pthread_attr_t          attr;
    pthread_t               displayThread;
    pthread_t               videoThread;
    pthread_t               loaderThread;
#ifdef SUPPORT_AUDIO_ENC
    pthread_t               audioThread;
    AudioEnv_dec            audioEnv;
#endif    
    LoaderEnv               loaderEnv;
    DisplayEnv              displayEnv;
    VideoEnv_dec            videoEnv;
    Int                     numThreads;	
    int                     quit;		
}decoder_group_thread_status,*pdecoder_group_thread_status;

decoder_group_thread_status dgts;

typedef struct tag_decArgs {
    Display_Output displayOutput;
    VideoStd_Type  videoStd;
    Char          *videoStdString;
    Char          *speechFile;
    Char          *videoFile;
    Int            loop;
    Int            keyboard;
    Int            time;
    Int            osd;
    Int            interface;
} decArgs;
#define DEFAULT_decARGS { Display_Output_EFLAGLCD, VideoStd_768P_60, "EFLAG HD", NULL, "/mnt/ram/test.264", \
                       FALSE, FALSE, FOREVER, FALSE, FALSE }

Void gblSetQuit_dec(void)
{
    dgts.quit = TRUE;
}
Int gblGetQuit_dec(void)
{
    Int quit;

    quit = dgts.quit;

    return quit;
}

/* Cleans up cleanly after a failure */
#define cleanup_dec(x)                                  \
    status = (x);                                   \
    gblSetQuit_dec();                                   \
    goto cleanup_out

void init_decode_thread_group(void)
{
    FUNC_ENTER
    memset(&dgts,0,sizeof(dgts));
    dgts.pAttrs 		= Pause_Attrs_DEFAULT;
    dgts.rzvAttrs            = Rendezvous_Attrs_DEFAULT;
    dgts.fAttrs              = Fifo_Attrs_DEFAULT;	
}

int pause_decoder_thread(int pause_action)
{
	if(dgts.hPauseProcess!=NULL) {
		if(pause_action!=0) {
			LOGT("%s: pause on\n",__func__);
			Pause_on(dgts.hPauseProcess);
		}else {
			LOGT("%s: pause off\n",__func__);
			Pause_off(dgts.hPauseProcess);
		}
	}else {
		LOGE("%s:hPauseProcess=NULL\n",__func__);
		request2reboot();
	}	
	return 1;
}

/******************************************************************************
 * start_decoder_thread_group
 ******************************************************************************/
Int start_decoder_thread_group(char *media_file)
{
    decArgs                args                = DEFAULT_decARGS;
    int status;
    Void               *ret;
	
    /* Zero out the thread environments */
    Dmai_clear(dgts.loaderEnv);
    Dmai_clear(dgts.displayEnv);
    Dmai_clear(dgts.videoEnv);
#ifdef SUPPORT_AUDIO_ENC    
	Dmai_clear(dgts.audioEnv);	
#endif

    LOGT("Decode demo started %s.\n",media_file);

    args.videoFile = media_file;
	
    dgts.initMask |= LOGSINITIALIZED;

    /* Create the Pause objects */
    dgts.hPauseProcess = Pause_create(&dgts.pAttrs);
    dgts.hPausePrime = Pause_create(&dgts.pAttrs);

    if (dgts.hPauseProcess == NULL || dgts.hPausePrime == NULL) {
        ERR("Failed to create Pause objects\n");
        cleanup_dec(EXIT_FAILURE);
    }

    /* Determine the number of threads needing synchronization */
    dgts.numThreads = 1;

    if (args.videoFile) {
        dgts.numThreads+=2;
        dgts.syncCnt++;
    }

#ifdef SUPPORT_AUDIO_ENC
    if(IsBoardSupportAudio()) {
        dgts.numThreads += 1;
    }    
#endif

    /* Create the objects which synchronizes the thread init and cleanup */
    dgts.hRendezvousInit = Rendezvous_create(dgts.numThreads, &dgts.rzvAttrs);
    dgts.hRendezvousCleanup = Rendezvous_create(dgts.numThreads, &dgts.rzvAttrs);


    if (dgts.hRendezvousInit == NULL || dgts.hRendezvousCleanup == NULL) {

        ERR("Failed to create Rendezvous objects\n");
        cleanup_dec(-2);
    }

    /* Initialize the thread attributes */
    if (pthread_attr_init(&dgts.attr)) {
        ERR("Failed to initialize thread attrs\n");
        cleanup_dec(-3);
    }

    /* Force the thread to use custom scheduling attributes */
    if (pthread_attr_setinheritsched(&dgts.attr, PTHREAD_EXPLICIT_SCHED)) {
        ERR("Failed to set schedule inheritance attribute\n");
        cleanup_dec(-4);
    }

    /* Set the thread to be fifo real time scheduled */
    if (pthread_attr_setschedpolicy(&dgts.attr, SCHED_FIFO)) {
        ERR("Failed to set FIFO scheduling policy\n");
        cleanup_dec(-5);
    }

    /* Create the video threads if a file name is supplied */
    if (args.videoFile) {
        LOGT("will decode %s.\n",args.videoFile);		
        /* Create the display fifos */
        dgts.displayEnv.hInFifo = Fifo_create(&dgts.fAttrs);
        dgts.displayEnv.hOutFifo = Fifo_create(&dgts.fAttrs);

        if (dgts.displayEnv.hInFifo == NULL || dgts.displayEnv.hOutFifo == NULL) {
            ERR("Failed to create display fifos\n");
            cleanup_dec(-6);
        }
#ifdef SUPPORT_AUDIO_ENC
        /* Create the audio fifos */
        dgts.audioEnv.hInFifo = Fifo_create(&dgts.fAttrs);
        dgts.audioEnv.hOutFifo = Fifo_create(&dgts.fAttrs);

        if (dgts.audioEnv.hInFifo == NULL || dgts.audioEnv.hOutFifo == NULL) {
            ERR("Failed to create audio dec fifos\n");
            cleanup_dec(-15);
        }
#endif
        /* Set the display thread priority */
        dgts.schedParam.sched_priority = DISPLAY_THREAD_PRIORITY;
        if (pthread_attr_setschedparam(&dgts.attr, &dgts.schedParam)) {
            ERR("Failed to set scheduler parameters\n");
            cleanup_dec(-7);
        }

        /* Create the display thread */
        dgts.displayEnv.displayOutput      = args.displayOutput;        
        dgts.displayEnv.videoStd           = args.videoStd;
        dgts.displayEnv.hRendezvousInit    = dgts.hRendezvousInit;
        dgts.displayEnv.hRendezvousCleanup = dgts.hRendezvousCleanup;
        dgts.displayEnv.hPauseProcess      = dgts.hPauseProcess;
        dgts.displayEnv.hPausePrime        = dgts.hPausePrime;
        dgts.displayEnv.osd                = args.osd;

        if (pthread_create(&dgts.displayThread, &dgts.attr, displayThrFxn, &dgts.displayEnv)) {
            ERR("Failed to create display thread\n");
            cleanup_dec(-8);
        }

        dgts.initMask |= DISPLAYTHREADCREATED;

        /* Set the video thread priority */
        dgts.schedParam.sched_priority = VIDEO_THREAD_PRIORITY;
        if (pthread_attr_setschedparam(&dgts.attr, &dgts.schedParam)) {
            ERR("Failed to set scheduler parameters\n");
            cleanup_dec(-9);
        }

        /* Create the video thread */
        dgts.videoEnv.hRendezvousInit    = dgts.hRendezvousInit;
        dgts.videoEnv.hRendezvousCleanup = dgts.hRendezvousCleanup;
        dgts.videoEnv.hRendezvousLoader  = dgts.hRendezvousLoader;
        dgts.videoEnv.hPauseProcess      = dgts.hPauseProcess;
        dgts.videoEnv.hPausePrime        = dgts.hPausePrime;
        dgts.videoEnv.hDisplayInFifo     = dgts.displayEnv.hInFifo;
        dgts.videoEnv.hDisplayOutFifo    = dgts.displayEnv.hOutFifo;
#ifdef SUPPORT_AUDIO_ENC
        dgts.videoEnv.hAudioInFifo       = dgts.audioEnv.hInFifo;
        dgts.videoEnv.hAudioOutFifo      = dgts.audioEnv.hOutFifo;
#endif
        dgts.videoEnv.videoFile          = args.videoFile;
        dgts.videoEnv.videoDecoder       = "h264dec";
        dgts.videoEnv.params             = NULL;
        dgts.videoEnv.dynParams          = NULL;
        dgts.videoEnv.loop               = args.loop;
        dgts.videoEnv.engineName         = "pvrcodec";
        dgts.videoEnv.videoStd           = args.videoStd;        

        if (pthread_create(&dgts.videoThread, &dgts.attr, videoThrFxn_dec, &dgts.videoEnv)) {
            ERR("Failed to create video thread\n");
            cleanup_dec(-10);
        }

        dgts.initMask |= VIDEOTHREADCREATED;

#ifdef SUPPORT_AUDIO_ENC
        if(IsBoardSupportAudio()) {
    		/* Set the thread priority */
    		dgts.schedParam.sched_priority = AUDIO_THREAD_PRIORITY;
    		if (pthread_attr_setschedparam(&dgts.attr, &dgts.schedParam)) {
    			ERR("Failed to set scheduler parameters\n");
    			return -13;
    		}

    		/* Create the audio thread */
    		dgts.audioEnv.hRendezvousInit	= dgts.hRendezvousInit;
    		dgts.audioEnv.hRendezvousCleanup = dgts.hRendezvousCleanup;
    		dgts.audioEnv.hRendezvousLoop	= NULL;//dgts.hRendezvousLoop;
    		dgts.audioEnv.hPauseProcess		= dgts.hPauseProcess;
    		dgts.audioEnv.audioFile			= NULL;//args.audioFile;
#ifdef USE_MP3_ENCODER    		
    		dgts.audioEnv.audioDecoder		= "mp3_dec";
#else
            dgts.audioEnv.audioDecoder      = "aacdec";//args.audioDecoder->codecName;
#endif
    		dgts.audioEnv.params 			= NULL;//args.audioDecoder->params;
    		dgts.audioEnv.dynParams			= NULL;//args.audioDecoder->dynParams;
    		dgts.audioEnv.loop				= FALSE;//args.loop;
    		dgts.audioEnv.engineName 		= "pvrcodec";//engine->engineName;

    		if (pthread_create(&dgts.audioThread, &dgts.attr, audioThrFxn_dec, &dgts.audioEnv)) {
    			ERR("Failed to create audio thread\n");
    			cleanup_dec(-14);
    		}

    		dgts.initMask |= AUDIOTHREADCREATED;
        }    
#endif
        /* Signal that initialization is done and wait for other threads */
        Rendezvous_meet(dgts.hRendezvousInit);

    }
    status = 1;	
cleanup_out:
	if(status<0)
		request2reboot();
    return status;		
}

int stop_decoder_thread_group(void)
{
    int status=0,ret;

    FUNC_ENTER

//  gblSetQuit_dec();

    /* Make sure the other threads aren't waiting for us */
    if (dgts.hRendezvousInit) Rendezvous_force(dgts.hRendezvousInit);
	
    Pause_off(dgts.hPauseProcess);

    /* Meet up with other threads before cleaning up */
    Rendezvous_meet(dgts.hRendezvousCleanup);
	
    /* Make sure the other threads aren't waiting for us */
//if (dgts.hRendezvousInit) Rendezvous_force(dgts.hRendezvousInit);


//    if (dgts.hPauseProcess) Pause_off(dgts.hPauseProcess);
    if (dgts.hPausePrime) Pause_off(dgts.hPausePrime);
	
#ifdef SUPPORT_AUDIO_ENC
    if (dgts.initMask & AUDIOTHREADCREATED) {
		LOGT("%s: wait AudioThread\n",__func__);
		if (pthread_join(dgts.audioThread, &ret) == 0) {
            if (ret == THREAD_FAILURE) {
                status = -4;
            }
        }
    }
#endif	

    if (dgts.initMask & LOADERTHREADCREATED) {
        LOGT("%s: wait LoaderThread\n",__func__);
        if (pthread_join(dgts.loaderThread, &ret) == 0) {
            if (ret == THREAD_FAILURE) {
                status = -1;
            }
        }
    }

    if (dgts.initMask & VIDEOTHREADCREATED) {
        LOGT("%s: wait videoThread\n",__func__);		
        if (pthread_join(dgts.videoThread, &ret) == 0) {
            if (ret == THREAD_FAILURE) {
                ERR("%s:join videoThread failed\r\n",__func__);
                status = -2;
            }
        }
    }

    if (dgts.initMask & DISPLAYTHREADCREATED) {
        LOGT("%s: wait displayThread\n",__func__);		
        if (pthread_join(dgts.displayThread, &ret) == 0) {
            if (ret == THREAD_FAILURE) {
                status = -3;
            }
        }
    }

    if (dgts.displayEnv.hOutFifo) {
        Fifo_delete(dgts.displayEnv.hOutFifo);
    }

    if (dgts.displayEnv.hInFifo) {
        Fifo_delete(dgts.displayEnv.hInFifo);
    }
#ifdef SUPPORT_AUDIO_ENC
    if (dgts.audioEnv.hOutFifo) {
        Fifo_delete(dgts.audioEnv.hOutFifo);
    }

    if (dgts.audioEnv.hInFifo) {
        Fifo_delete(dgts.audioEnv.hInFifo);
    }
#endif    

    if (dgts.hRendezvousCleanup) {
        Rendezvous_delete(dgts.hRendezvousCleanup);
    }

    if (dgts.hRendezvousInit) {
        Rendezvous_delete(dgts.hRendezvousInit);
    }

    if (dgts.hPauseProcess) {
        Pause_delete(dgts.hPauseProcess);
    }

    if (dgts.hPausePrime) {
        Pause_delete(dgts.hPausePrime);
    }

    sync();
	sync();
    system("echo 3 > /proc/sys/vm/drop_caches");

	if(status<0) {
        ERR("%s:request reboot,status=%d\r\n",__func__,status);
		request2reboot();
    }    
    return 1;
}

/*
********************************************************************************
**********************************************************************************
**********************************decoder part*************************************
*************************************end******************************************
**********************************************************************************
*/

