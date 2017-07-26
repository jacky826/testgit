#include "encoder.h"
#include "ui_avi_io.h"
#include "../../pvr_debug.h"
#include <config.h>
#include <File_manager.h>
#include "pvr_error.h"
#include <UI_Message.h>
#include <all_data.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> 
#include <sys/time.h>
#include <ti/sdo/dmai/VideoStd.h>



extern void init_encode_thread_group(void);
extern int start_encoder_thread_group(int argc, char *argv[]);
extern int stop_encoder_thread_group(void);
extern int gblSetQuit_enc(void);

/**************************************file synthesize definition*****************************/
typedef struct tag_FILE_SYNTH{
	int (*Initial)(struct tag_FILE_SYNTH *,int file,const SaveAndPlayPara *para);
	int (*BeginFlush)(struct tag_FILE_SYNTH *);
	int (*Add_Aframe)(struct tag_FILE_SYNTH *,unsigned char *pbuf,int buf_len);
	int (*Add_Vframe)(struct tag_FILE_SYNTH *,unsigned char *pbuf,int buf_len,int keyframe);
	int (*FlushTailer)(struct tag_FILE_SYNTH *);
	int (*Pause)(struct tag_FILE_SYNTH *);
	int (*Restart)(struct tag_FILE_SYNTH *);
	int (*DeInitial)(struct tag_FILE_SYNTH *);
	void *pspec;
}FILE_SYNTH,*PFILE_SYNTH;

struct ENCODER_MODULE{
	PFILE_SYNTH pfile_synth;
	char enFilepath[MAX_FS_PATH];
	SaveAndPlayPara enPara;
	
	int running;

	int got_key_vframe;
	unsigned long discard_none_keyframe;
	int keyframecount;
}en_module;


static int rec_restart(void);
static int rec_stop(void);
static int rec_pause(void);

//#define USE_AUDIO_SEQUENCER

#ifdef USE_AUDIO_SEQUENCER
#define AUDIO_FILTER_SLOT_NUM 50
static struct ENCODER_SEQUENCER{
    struct {
        int frm_len;
        int buf_len;
        unsigned char *slot_buf;
    }slot_info[AUDIO_FILTER_SLOT_NUM];
    int head_slot_pos;
    int tail_slot_pos;
    int valid_slot_num;
}en_seq;
#endif

#define USE_VIDEO_SEQUENCER

#ifdef USE_VIDEO_SEQUENCER
#define VIDEO_FILTER_SLOT_NUM 50
#define VIDEO_FILTER_FRM_LEN 0xc000
static struct ENCODER_SEQUENCER{
    struct {
        int frm_len;
        int buf_len;
        int flag;
        unsigned char *slot_buf;
    }slot_info[VIDEO_FILTER_SLOT_NUM];
    int head_slot_pos;
    int tail_slot_pos;
    int valid_slot_num;
}en_seq;
#endif


struct AVI_SSYNC_INFO{
	int hdestfile;

	M_MPEGLAYER3WAVEFORMAT Mp3Format;
	M_BITMAPINFOHEADER BitmapInfo;
	
	double video_time_total;
	double last_time;
	
	#define AVI_INITIALED 0x1
	#define AVI_RUN 0x2
	#define AVI_CAN_FLUSH_DATA (AVI_INITIALED|AVI_RUN)
	int status; // run or pause
	int err;
#ifdef SUPPORT_AUDIO_ENC
    int get_first_audio;
#endif
}avi_info;

static double get_current_msec(void)
{
	double ms_time;
	struct timeval  tv;
	gettimeofday(&tv, NULL);

	ms_time = (double)tv.tv_sec * 1000 +(double) tv.tv_usec / 1000;
	return ms_time;
}

int avi_Initial_wrapper(struct tag_FILE_SYNTH *pfsyn,int file,const SaveAndPlayPara *para)
{
	int w,h;
	if(pfsyn!=NULL) {
		memset(&avi_info,0,sizeof(avi_info));
		VideoStd_getResolution(VideoStd_768P_60,&w,&h);
		if(w==0||h==0)
			return ERR_INVALID_ENC_PARA;

		avi_info.BitmapInfo.biWidth = w;
		avi_info.BitmapInfo.biHeight = h;

		switch(para->Vmethod) {
			case ENC_MPEG4:
				avi_info.BitmapInfo.biCompression = FCC('XVID');
				break;
			case ENC_H264:
				avi_info.BitmapInfo.biCompression = FCC('H264');
				break;
			case ENC_H263:
				avi_info.BitmapInfo.biCompression = FCC('H263');
				break;
			default:
				return ERR_INVALID_ENC_PARA;
		}
		avi_info.hdestfile=file;

		avi_info.status=AVI_INITIALED;		
		pfsyn->pspec=(void *)&avi_info;

		switch(para->Amethod){
		case ENC_MP3:
			avi_info.Mp3Format.wFormatTag = 0x55;
			avi_info.Mp3Format.nChannels = 2;
			avi_info.Mp3Format.nSamplesPerSec = 0x7d00;
			avi_info.Mp3Format.nAvgBytesPerSec = 16000;
			avi_info.Mp3Format.nBlockAlign = 1;
			avi_info.Mp3Format.wBitsPerSample = 0;
			avi_info.Mp3Format.cbsize = 14;
			avi_info.Mp3Format.reserve = (unsigned char*)malloc(14);
			avi_info.Mp3Format.reserve[0] = 1;
			avi_info.Mp3Format.reserve[1] = 0;
			avi_info.Mp3Format.reserve[2] = 2;
			avi_info.Mp3Format.reserve[3] = 0;
			avi_info.Mp3Format.reserve[4] = 0;
			avi_info.Mp3Format.reserve[5] = 0;
			avi_info.Mp3Format.reserve[6] = 0xe0;
			avi_info.Mp3Format.reserve[7] = 1;
			avi_info.Mp3Format.reserve[8] = 1;
			avi_info.Mp3Format.reserve[9] = 0;
			avi_info.Mp3Format.reserve[10] = 0x71;
			avi_info.Mp3Format.reserve[11] = 5;
			avi_info.Mp3Format.reserve[12] = 0;
			avi_info.Mp3Format.reserve[13] = 0;	
			break;
		case ENC_AAC:
			avi_info.Mp3Format.wFormatTag = 0x706d;
			avi_info.Mp3Format.nChannels = 2;
			avi_info.Mp3Format.nSamplesPerSec = 0x7d00;
			avi_info.Mp3Format.nAvgBytesPerSec = 2500;
			avi_info.Mp3Format.nBlockAlign = 1;
			avi_info.Mp3Format.wBitsPerSample = 0;
			avi_info.Mp3Format.cbsize = 0;
			avi_info.Mp3Format.reserve = NULL;
			break;			
		case ENC_AMR:
		default:
			return ERR_INVALID_ENC_PARA;
		}
        
#ifdef SUPPORT_AUDIO_ENC  
        if(IsBoardSupportAudio()) {
    		avi_info.err= Avi_Initial_Header(avi_info.Mp3Format.cbsize);
        }else {
            avi_info.err= Avi_Initial_Header(0);
        }
        
#ifdef USE_AUDIO_SEQUENCER
        memset(&en_seq,0,sizeof(en_seq));
#endif

#else
		avi_info.err= Avi_Initial_Header(0);
#endif

#ifdef USE_VIDEO_SEQUENCER
        memset(&en_seq,0,sizeof(en_seq));
        for(h=0;h<VIDEO_FILTER_SLOT_NUM;h++) {
            en_seq.slot_info[h].slot_buf=(char *)malloc(VIDEO_FILTER_FRM_LEN);
            if(en_seq.slot_info[h].slot_buf==NULL) {
                LOGE("%s:malloc failed@%d\n",__func__,h);
                return ERR_MALLOC;
            }
            en_seq.slot_info[h].buf_len = VIDEO_FILTER_FRM_LEN;            
        }
#endif

		return avi_info.err;
	}
	return ERR_INVALID_SYNTH;
}
int avi_Begin_Flush_wrapper(struct tag_FILE_SYNTH *pfsyn)
{
	FUNC_ENTER
	if(pfsyn!=NULL) {
		struct AVI_SSYNC_INFO *pinfo;
		pinfo=(struct AVI_SSYNC_INFO *)pfsyn->pspec;
		if(avi_info.err<0)
			return avi_info.err;
		pinfo->last_time=get_current_msec();
		pinfo->status|=AVI_RUN;
		avi_info.err=Avi_Before_AddData(pinfo->hdestfile);
		return avi_info.err;
	}
	return ERR_INVALID_SYNTH;
}

static int adding_vframe=0;


#ifdef SUPPORT_AUDIO_ENC
static int adding_aframe=0;
#define MUTE_MP3_HEADER_LEN 0x5c1
static char mute_mp3_stream_header[MUTE_MP3_HEADER_LEN];
void initial_mute_mp3_header(void)
{
    memset(mute_mp3_stream_header,0,MUTE_MP3_HEADER_LEN);
    mute_mp3_stream_header[0]=0xff;
    mute_mp3_stream_header[1]=0xf3;
    mute_mp3_stream_header[2]=0xa8;
    mute_mp3_stream_header[3]=0x44;
    mute_mp3_stream_header[7]=0x01;    
    mute_mp3_stream_header[8]=0xa4;    
    mute_mp3_stream_header[0xf]=0x03;    
    mute_mp3_stream_header[0x10]=0x48;    

    mute_mp3_stream_header[0x1b0]=0xff;
    mute_mp3_stream_header[0x1b1]=0xf3;
    mute_mp3_stream_header[0x1b2]=0xa8;
    mute_mp3_stream_header[0x1b3]=0x44;
    mute_mp3_stream_header[0x1b4]=0xff;
    mute_mp3_stream_header[0x1b7]=0x01;
    mute_mp3_stream_header[0x1b8]=0xa4;
    mute_mp3_stream_header[0x1bf]=0x03;    
    mute_mp3_stream_header[0x1c0]=0x48;    


    mute_mp3_stream_header[0x360]=0xff;
    mute_mp3_stream_header[0x361]=0xf3;
    mute_mp3_stream_header[0x362]=0xa8;
    mute_mp3_stream_header[0x363]=0x44;
    mute_mp3_stream_header[0x364]=0xff;
    mute_mp3_stream_header[0x367]=0x01;
    mute_mp3_stream_header[0x368]=0xa4;
    mute_mp3_stream_header[0x36f]=0x03;    
    mute_mp3_stream_header[0x370]=0x48;    
    

    mute_mp3_stream_header[0x510]=0xff;
    mute_mp3_stream_header[0x511]=0xf3;
    mute_mp3_stream_header[0x512]=0xa8;
    mute_mp3_stream_header[0x513]=0x44;
    mute_mp3_stream_header[0x514]=0xff;
    mute_mp3_stream_header[0x517]=0x01;
    mute_mp3_stream_header[0x518]=0xa4;
    mute_mp3_stream_header[0x51f]=0x03;    
    mute_mp3_stream_header[0x520]=0x48;    
   
    
}
#endif

int avi_Add_Aframe_wrapper(struct tag_FILE_SYNTH *pfsyn,unsigned char *pbuf,int buf_len)
{
#ifdef SUPPORT_AUDIO_ENC
	Stream_Info aviindexinfo;
	if(pfsyn!=NULL) {
		struct AVI_SSYNC_INFO *pinfo;
		pinfo=(struct AVI_SSYNC_INFO *)pfsyn->pspec;
		if(pinfo->status==AVI_CAN_FLUSH_DATA) {
			if(avi_info.err<0)
				return avi_info.err;
            if(avi_info.get_first_audio==0) {
                pinfo->last_time=get_current_msec();
                avi_info.get_first_audio = 1;
                if(buf_len<(MUTE_MP3_HEADER_LEN-0x50)) {
                    /*this branch will be triggered when switch the file name,add the fake audio data to the header of the new avi file*/
                    LOGT("add fake audio header,first audio frame len 0x%x\n",buf_len);
                    aviindexinfo.buffer = mute_mp3_stream_header;
                    aviindexinfo.dwFlags = 0;
                    aviindexinfo.dwSize = MUTE_MP3_HEADER_LEN;
                    adding_aframe = 1;
                    avi_info.err=Avi_Add_Aframe(pinfo->hdestfile,&aviindexinfo);
                    adding_aframe = 0;                     
                }
            }
#ifdef USE_AUDIO_SEQUENCER
            if(en_seq.valid_slot_num>=AUDIO_FILTER_SLOT_NUM) {
                //too many audio frm in sequencer,write the header one to disk
                aviindexinfo.buffer = en_seq.slot_info[en_seq.head_slot_pos].slot_buf;
                aviindexinfo.dwFlags = 0;
			    aviindexinfo.dwSize = en_seq.slot_info[en_seq.head_slot_pos].frm_len;
                adding_aframe = 1;
                avi_info.err=Avi_Add_Aframe(pinfo->hdestfile,&aviindexinfo);
                adding_aframe = 0;  
                if(++en_seq.head_slot_pos>=AUDIO_FILTER_SLOT_NUM) {
                    en_seq.head_slot_pos=0;
                }                
                en_seq.valid_slot_num--;
                LOGT("too many audio frame in sequencer 0x%x vs 0x%x\n",en_seq.head_slot_pos,en_seq.tail_slot_pos);
            }
            //add new audio frame to tail of sequencer buffer
            if(en_seq.slot_info[en_seq.tail_slot_pos].slot_buf!=NULL&&en_seq.slot_info[en_seq.tail_slot_pos].buf_len<buf_len) {
               free(en_seq.slot_info[en_seq.tail_slot_pos].slot_buf);
               en_seq.slot_info[en_seq.tail_slot_pos].slot_buf = NULL;
               LOGT("slot buf reallocate 0x%x vs 0x%x\n",en_seq.slot_info[en_seq.tail_slot_pos].buf_len,buf_len);
            }   
            if(en_seq.slot_info[en_seq.tail_slot_pos].slot_buf==NULL) {
                en_seq.slot_info[en_seq.tail_slot_pos].slot_buf=(char *)malloc(buf_len+0x100);
                en_seq.slot_info[en_seq.tail_slot_pos].buf_len = buf_len+0x100;
            }
            memcpy(en_seq.slot_info[en_seq.tail_slot_pos].slot_buf,pbuf,buf_len);
            en_seq.slot_info[en_seq.tail_slot_pos].frm_len=buf_len;
            if(++en_seq.tail_slot_pos>=AUDIO_FILTER_SLOT_NUM)
                en_seq.tail_slot_pos=0;
            en_seq.valid_slot_num++;
            return 0;
#elif defined USE_VIDEO_SEQUENCER
			aviindexinfo.buffer = pbuf;	
			aviindexinfo.dwFlags = 0;
			aviindexinfo.dwSize = buf_len;
			adding_aframe = 1;
			avi_info.err=Avi_Add_Aframe(pinfo->hdestfile,&aviindexinfo);
			adding_aframe = 0;	

            if(en_seq.valid_slot_num>0) {
                /*
                                there is at least one audio frame in the encoder sequencer,add the audio first and the video frame
                              */
                aviindexinfo.buffer = en_seq.slot_info[en_seq.head_slot_pos].slot_buf;
                aviindexinfo.dwFlags = en_seq.slot_info[en_seq.head_slot_pos].flag;
                aviindexinfo.dwSize = en_seq.slot_info[en_seq.head_slot_pos].frm_len;
                adding_vframe = 1;
                avi_info.err=Avi_Add_Vframe(pinfo->hdestfile,&aviindexinfo);
                adding_vframe = 0;  
                if(avi_info.err<0) {
                    return avi_info.err;
                }
                if(++en_seq.head_slot_pos>=VIDEO_FILTER_SLOT_NUM) {
                    en_seq.head_slot_pos=0;
                }
                en_seq.valid_slot_num--;
            }
//            LOGT("%s:add audio frame 0x%x,video in fifo %d\n",buf_len,en_seq.valid_slot_num);
    		return avi_info.err;
#endif            
		}else
			return 1;
	}
#endif	
	return ERR_INVALID_SYNTH;
}



int avi_Add_Vframe_wrapper(struct tag_FILE_SYNTH *pfsyn,unsigned char *pbuf,int buf_len,int keyframe)
{
	Stream_Info aviindexinfo;

	if(pfsyn!=NULL) {
		struct AVI_SSYNC_INFO *pinfo;
		pinfo=(struct AVI_SSYNC_INFO *)pfsyn->pspec;
		if(pinfo->status==AVI_CAN_FLUSH_DATA) {
			if(avi_info.err<0)
				return avi_info.err;				
//			FUNC_ENTER
#ifdef USE_AUDIO_SEQUENCER
            if(en_seq.valid_slot_num>0) {
                /*
                                there is at least one audio frame in the encoder sequencer,add the audio first and the video frame
                              */
                aviindexinfo.buffer = en_seq.slot_info[en_seq.head_slot_pos].slot_buf;
                aviindexinfo.dwFlags = 0;
			    aviindexinfo.dwSize = en_seq.slot_info[en_seq.head_slot_pos].frm_len;
			    adding_aframe = 1;
			    avi_info.err=Avi_Add_Aframe(pinfo->hdestfile,&aviindexinfo);
			    adding_aframe = 0;	
                if(avi_info.err<0) {
                    return avi_info.err;
                }
                if(++en_seq.head_slot_pos>=AUDIO_FILTER_SLOT_NUM) {
                    en_seq.head_slot_pos=0;
                }
                en_seq.valid_slot_num--;
            }
#elif defined USE_VIDEO_SEQUENCER
            if(en_seq.valid_slot_num>=VIDEO_FILTER_SLOT_NUM) {
                //too many video frm in sequencer,write the header one to disk
                aviindexinfo.buffer = en_seq.slot_info[en_seq.head_slot_pos].slot_buf;
                aviindexinfo.dwFlags = en_seq.slot_info[en_seq.head_slot_pos].flag;
                aviindexinfo.dwSize = en_seq.slot_info[en_seq.head_slot_pos].frm_len;
                adding_vframe = 1;
                avi_info.err=Avi_Add_Vframe(pinfo->hdestfile,&aviindexinfo);
                adding_vframe = 0;  
                if(++en_seq.head_slot_pos>=VIDEO_FILTER_SLOT_NUM) {
                    en_seq.head_slot_pos=0;
                }                
                en_seq.valid_slot_num--;
                LOGT("too many video frame in sequencer 0x%x vs 0x%x\n",en_seq.head_slot_pos,en_seq.tail_slot_pos);
            }
            //add new audio frame to tail of sequencer buffer
            if(en_seq.slot_info[en_seq.tail_slot_pos].slot_buf!=NULL&&en_seq.slot_info[en_seq.tail_slot_pos].buf_len<buf_len) {
               free(en_seq.slot_info[en_seq.tail_slot_pos].slot_buf);
               en_seq.slot_info[en_seq.tail_slot_pos].slot_buf = NULL;
               LOGT("slot buf reallocate 0x%x vs 0x%x\n",en_seq.slot_info[en_seq.tail_slot_pos].buf_len,buf_len);
            }   
            if(en_seq.slot_info[en_seq.tail_slot_pos].slot_buf==NULL) {
                en_seq.slot_info[en_seq.tail_slot_pos].slot_buf=(char *)malloc(buf_len+0x100);
                en_seq.slot_info[en_seq.tail_slot_pos].buf_len = buf_len+0x100;
            }
            memcpy(en_seq.slot_info[en_seq.tail_slot_pos].slot_buf,pbuf,buf_len);
            en_seq.slot_info[en_seq.tail_slot_pos].frm_len=buf_len;
            en_seq.slot_info[en_seq.tail_slot_pos].flag=(keyframe!=0?0x10:0);
            if(++en_seq.tail_slot_pos>=VIDEO_FILTER_SLOT_NUM)
                en_seq.tail_slot_pos=0;
            en_seq.valid_slot_num++;
            return 0;
#else //SACH:W10906
			aviindexinfo.buffer = pbuf;	
			aviindexinfo.dwFlags = (keyframe!=0?0x10:0);
			aviindexinfo.dwSize = buf_len;
			adding_vframe = 1;
			avi_info.err=Avi_Add_Vframe(pinfo->hdestfile,&aviindexinfo);
			adding_vframe = 0;

			return avi_info.err;
#endif //SACH:W10906
		}else
			return 1;
	}
	return ERR_INVALID_SYNTH;
}

int avi_FlushTailer_wrapper(struct tag_FILE_SYNTH *pfsyn)
{
	if(pfsyn!=NULL) {
		struct AVI_SSYNC_INFO *pinfo;
		double cur,last;
		int err;

		FUNC_ENTER
		pinfo=(struct AVI_SSYNC_INFO *)pfsyn->pspec;
		if(avi_info.err<0)
			return avi_info.err;	
		if(pinfo->status&AVI_INITIALED) {
			if(pinfo->status&AVI_RUN) {
				cur = get_current_msec();
				last=cur-pinfo->last_time;
				pinfo->video_time_total+=last;
			} //if paused ,use pinfo->video_time_total directly	
			err=Avi_FlushTailer(pinfo->hdestfile,&pinfo->BitmapInfo,&pinfo->Mp3Format,pinfo->video_time_total);
			avi_info.err=err<0?err:0;
			return avi_info.err; 
		}	
	}
	return ERR_INVALID_SYNTH;
}
int avi_Pause_wrapper(struct tag_FILE_SYNTH *pfsyn)
{
	if(pfsyn!=NULL) {
		struct AVI_SSYNC_INFO *pinfo;
		double cur,last;
		pinfo=(struct AVI_SSYNC_INFO *)pfsyn->pspec;
		if(pinfo->status==AVI_CAN_FLUSH_DATA) {
			pinfo->status&=~AVI_RUN;
			cur = get_current_msec();
			last = cur-pinfo->last_time;
			pinfo->video_time_total+=last;
			return 1;
		}
	}
	return ERR_INVALID_SYNTH;	
}
int avi_Restart_wrapper(struct tag_FILE_SYNTH *pfsyn)
{
	if(pfsyn!=NULL) {
		struct AVI_SSYNC_INFO *pinfo;
		pinfo=(struct AVI_SSYNC_INFO *)pfsyn->pspec;
		if(pinfo->status==AVI_INITIALED) {
			pinfo->status|=AVI_RUN;
			pinfo->last_time = get_current_msec();
			return 1;
		}	
	}
	return ERR_INVALID_SYNTH;	
}
int avi_DeInitial_wrapper(struct tag_FILE_SYNTH *pfsyn)
{	
	int err=1;
	if(pfsyn!=NULL) {
		struct AVI_SSYNC_INFO *pinfo;
		pinfo=(struct AVI_SSYNC_INFO *)pfsyn->pspec;
		if(pinfo!=NULL) {
			if(pinfo->status&AVI_INITIALED) {
				if(pinfo->hdestfile!=-1) {
					close(pinfo->hdestfile);
					pinfo->hdestfile=-1;
				}
			}
			err=pinfo->err;
			if(pinfo->Mp3Format.reserve!=NULL) {
				free(pinfo->Mp3Format.reserve);
				pinfo->Mp3Format.reserve = NULL;
			}
		}
		pfsyn->pspec=NULL;
#ifdef USE_AUDIO_SEQUENCER
        int i;
        for(i=0;i<AUDIO_FILTER_SLOT_NUM;i++) {
            if(en_seq.slot_info[i].slot_buf!=NULL) {
                free(en_seq.slot_info[i].slot_buf);
                en_seq.slot_info[i].slot_buf = NULL;
                en_seq.slot_info[i].buf_len = 0;
            }                
        }
#endif   
#ifdef USE_VIDEO_SEQUENCER
        int i;
        for(i=0;i<VIDEO_FILTER_SLOT_NUM;i++) {
            if(en_seq.slot_info[i].slot_buf!=NULL) {
                free(en_seq.slot_info[i].slot_buf);
                en_seq.slot_info[i].slot_buf = NULL;
                en_seq.slot_info[i].buf_len = 0;
            }                
        }
#endif 

    }
	return err;
}


FILE_SYNTH avi_synth={
	avi_Initial_wrapper,
	avi_Begin_Flush_wrapper,
	avi_Add_Aframe_wrapper,
	avi_Add_Vframe_wrapper,
	avi_FlushTailer_wrapper,
	avi_Pause_wrapper,
	avi_Restart_wrapper,
	avi_DeInitial_wrapper,
	NULL,
};

static int judge_codec(SaveAndPlayPara *para)
{
	switch(para->MFileFormat){
		case FORMAT_AVI:
			en_module.pfile_synth=&avi_synth;
			return 1;
		case FORMAT_3GP:
		case FORMAT_MOV:
		case FORMAT_MP4:
		default:
			en_module.pfile_synth = NULL;
			break;
	}
	en_module.pfile_synth=NULL;
	return ERR_NO_SUPPORT_ENCODER;
}
static int rec_restart()
{
	if(en_module.pfile_synth==NULL||en_module.running==0)
		return ERR_INVALID_ENC_STATE;
	en_module.got_key_vframe=0; // start from next key video frame
	return en_module.pfile_synth->Restart(en_module.pfile_synth);
}
static int rec_pause()
{
	if(en_module.pfile_synth==NULL||en_module.running==0)
		return ERR_INVALID_ENC_STATE;
	return en_module.pfile_synth->Pause(en_module.pfile_synth);
}

int rec_add_audio(char *pbuf,int buf_len)
{
#ifdef SUPPORT_AUDIO_ENC
	int err,aacsize,i;

	if(en_module.pfile_synth==NULL||en_module.running==0) {
		err=ERR_INVALID_ENC_STATE;
		goto out;
	}
	err = en_module.pfile_synth->Add_Aframe(en_module.pfile_synth,pbuf,buf_len);
out:	
	return err;
#else
    return 0;
#endif    
}



int rec_add_video(char *pbuf,int buf_len,int keyframe)
{
	int err;

//	LOGT("%s: keyframe=%d len=%d\n",__func__,keyframe,buf_len);	
	if(en_module.pfile_synth==NULL||en_module.running==0) {
		err=ERR_INVALID_ENC_STATE;
		goto out;
	}
	if(en_module.got_key_vframe==0) {
		en_module.got_key_vframe=(keyframe!=0);
	}
	if(en_module.got_key_vframe!=0)
		err = en_module.pfile_synth->Add_Vframe(en_module.pfile_synth,pbuf,buf_len,keyframe);
	else
		en_module.discard_none_keyframe++;
out:
	return err;
}

static int rec_stop()
{
	FUNC_ENTER
	if(en_module.running!=0) {
		if(en_module.pfile_synth!=NULL) {
			en_module.pfile_synth->FlushTailer(en_module.pfile_synth);
			if(en_module.pfile_synth->DeInitial(en_module.pfile_synth)<0) {
				unlink(en_module.enFilepath);
				LOGE("%s:DeInitial failed %d\n",__func__,avi_info.err);
			}
		}
		en_module.pfile_synth=NULL;		
		en_module.running=0;	
	}	
	return 1;
}

static int rec_start(char *filename,SaveAndPlayPara *para)
{
	int err = 0;
	int aviwritefile = -1;

	FUNC_ENTER
		
	if(en_module.running!=0)
		return ERR_INVALID_ENC_STATE;

	en_module.pfile_synth=NULL;

	err = judge_codec(para);
	if(err < 0 || en_module.pfile_synth == NULL) {//SACH:W10864
		LOGE("%s:judge_codec failed for %d\n",__func__,err);
		return err;	
	}	

	aviwritefile = open(filename,O_RDWR|O_CREAT|O_TRUNC, 0666);
	if(aviwritefile == -1) {
		err=ERR_OPENFILE;
		goto stop;
	}
	if((err=en_module.pfile_synth->Initial(en_module.pfile_synth,aviwritefile,para))<0) {
		goto stop;
	}
	
	if(en_module.pfile_synth->BeginFlush(en_module.pfile_synth)<0) {
		err=ERR_SYNC_FLUSH;
		goto stop;
	}

	en_module.got_key_vframe=0;
	en_module.keyframecount = 0;
	en_module.running=1;

	LOGT("%s:encode to %s\n",__func__,filename);
	return 1;

stop:
	en_module.running=0;

	if(en_module.pfile_synth!=NULL) {
		en_module.pfile_synth->DeInitial(en_module.pfile_synth);
	}else {
		if(aviwritefile!=-1) {
			close(aviwritefile);	
			aviwritefile=-1;
		}
	}	
	unlink(filename);
	
	en_module.pfile_synth=NULL;
	LOGE("%s error=0x%x\n",__func__,err);
	return err;
}

// called in the context of ui and schedule record
int start_encoder(const char *pdestfile,SaveAndPlayPara *penpara)
{
	FUNC_ENTER
	if(pdestfile==NULL||penpara==NULL)
		return ERR_INVALID_ENC_PARA;
	if(en_module.pfile_synth!=NULL||en_module.running!=0) {
		LOGE("%s:pfile_synth=0x%x running=%d\n",__func__,(int)en_module.pfile_synth,en_module.running);
		return ERR_INVALID_ENC_STATE;
	}	
	strcpy(en_module.enFilepath,pdestfile);
	memcpy(&en_module.enPara,penpara,sizeof(SaveAndPlayPara));

	rec_start(en_module.enFilepath,penpara);
	init_encode_thread_group();
	start_encoder_thread_group(0,NULL);	

	return ERR_OK;
}

int stop_encoder()
{
	FUNC_ENTER
	if(en_module.pfile_synth==NULL||en_module.running==0) {
		LOGE("%s:pfile_synth=0x%x running=%d\n",__func__,(int)en_module.pfile_synth,en_module.running);
		return ERR_INVALID_ENC_STATE;
	}	
	gblSetQuit_enc();
	stop_encoder_thread_group();	
	rec_stop();

	FEED_WATCHDOG();
	sync();
	sync();

	return ERR_OK;
}

int change_dest_filename(const char *pdestfile,SaveAndPlayPara *penpara)
{
	FUNC_ENTER
	int i=0;
	if(en_module.pfile_synth==NULL||en_module.running==0) {
		LOGE("%s:pfile_synth=0x%x running=%d\n",__func__,(int)en_module.pfile_synth,en_module.running);
		return ERR_INVALID_ENC_STATE;
	}	
	if(pdestfile==NULL||penpara==NULL)
		return ERR_INVALID_ENC_PARA;

	//set flag to pause background encoder thread and wait for a while
	pause_encoder_thread(1);

	rec_pause();//prevent writer thread to add video frame to file
	while(adding_vframe!=0&&i<50) { //wait write thread to finish last frame
		usleep(200000); //wait 200ms 
		i++;
		LOGE("%s wait writer thead finished last video frame %d\n",__func__,i);
	}	

#ifdef SUPPORT_AUDIO_ENC
	i = 0;
	while(adding_aframe!=0&&i<50) { //wait write thread to finish last frame
		usleep(200000); //wait 200ms 
		i++;
		LOGE("%s wait writer thead finished last audio frame %d\n",__func__,i);
	}	
#endif

	rec_stop();
	if(en_module.pfile_synth!=NULL||en_module.running!=0) {
		LOGE("%s:pfile_synth=0x%x running=%d\n",__func__,(int)en_module.pfile_synth,en_module.running);
		return ERR_INVALID_ENC_STATE;
	}	
	strcpy(en_module.enFilepath,pdestfile);
	memcpy(&en_module.enPara,penpara,sizeof(SaveAndPlayPara));

	rec_start(en_module.enFilepath,penpara);
	pause_encoder_thread(0);	
	return ERR_OK;
}

extern int pause_encoder_thread(int pause_action);
int pause_encoder()
{
	if(en_module.pfile_synth==NULL||en_module.running==0)
		return ERR_INVALID_ENC_STATE;
	pause_encoder_thread(1);
	rec_pause();
	return ERR_OK;	
}

int restart_encoder()
{
	if(en_module.pfile_synth==NULL||en_module.running==0)
		return ERR_INVALID_ENC_STATE;
	rec_restart();
	pause_encoder_thread(0);	
	return ERR_OK;
}

void sys_init_encoder()
{
	memset(&en_module,0,sizeof(en_module));
#ifdef SUPPORT_AUDIO_ENC
    initial_mute_mp3_header();
#endif    
}

void sys_deinit_encoder()
{

}
