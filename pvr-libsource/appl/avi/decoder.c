#include "decoder.h"
#include "pvr_error.h"
#include "ui_avi_io.h"
#include "UI_Message.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> 
#include <ctype.h>
#include "../../pvr_debug.h"
#include "../include/config.h"



static PlayerFileParser* playerfileparser = NULL;
static char PlayBackFilename[MAX_PATH];
static BOOL PlayerRun = 0;

#define VIDEO_DEC_BUFFER_LEN 0x100000//800000
#define AUDIO_INBUF_LEN (100000)

#define SKIP_FRAME_COUNT 40

#ifdef SKIP_FRAME_DELAY_WHEN_SPEED_CHANGE
static int skip_count=0;
#endif

extern void init_decode_thread_group(void);
extern int start_decoder_thread_group(char *arg);
extern int stop_decoder_thread_group(void);
extern int pause_decoder_thread(int pause_action);


/*************************************Avi File Format Parser************************************/

static int Avi_GetInfo(PlayerFileParser *parser,PlayerInfo* info)
{
	int err=-1;
	if(parser->fd != -1) {
		if((err=Avi_Get_Header(parser->fd,0))==0) {
			info->xdist = Avi_Get_XDist();
			info->ydist = Avi_Get_YDist();
			info->framerate = Avi_Get_FrameRate();
			info->playtime = Avi_GetFilePlayTime();	
			info->acodec = Avi_Get_ACodecMethod();
			info->vcodec = Avi_Get_VCodecMethod();
			return ERR_OK;
		}else {
			LOGE("%s:Avi_Get_Header() failed @0x%x\n",__func__,err);
		}
	}
	return err;
}

static int Avi_BeginPrefetch(PlayerFileParser *parser)
{
	return Avi_prestart(parser->fd);
}
static int Avi_StopPrefetch(PlayerFileParser *parser)
{
	return Avi_prestop(parser->fd);
}
	
static int Avi_DoPrefetch(PlayerFileParser *parser)
{
	if(playerfileparser->playbackstatus==PLAYBACKNORMAL) {
		return Avi_prefetch(parser->fd);
	}
	return 1;
}

static int avi_fill_video(PlayerFileParser *parser)
{
	AviPrivateMember* pap;
	char* fillpt = NULL;
	int i;
	int count;
	
	pap=(AviPrivateMember*)parser->specInfo;
	parser->DecodeTotal = 0;
	parser->count=0;

	fillpt =parser->AviVFrameBuffer;

	for(i=0;i<pap->VideoDecCount;i++) {	
		pap->Rvframenum = Avi_Next_VFrame(pap->Rvframenum);
		if(pap->Rvframenum == -1) {
			LOGE("%s:Avi_Next_VFrame return -1 (%d)\n",__func__,i);
			parser->bend=1;
			return 1;
		}
		count = Avi_Get_Frame(fillpt,VIDEO_DEC_BUFFER_LEN-parser->DecodeTotal,parser->fd,pap->Rvframenum,1);	
		if(count<0) {
			LOGE("%s:Avi_Get_Frame return %d\n",__func__,count);
			return count;
		}				
		pap->Rvframenum++;
		parser->DecodeTotal+=count;
		fillpt+=count;
		parser->count+=count;
//		LOGT("%s:Avi_Get_Frame fetch frame len %d %d\n",__func__,count,parser->DecodeTotal);
	}	
	parser->DecodeFrame = i;
//	LOGT("%s:read get frame count=0x%x  frm num=%d\n",__func__,count,pap->Rvframenum);
	return 1;
}

int Check_Video_Decoder(int vcodecmethod,int width,int height)
{
	static int dim_h264_support[][2]={{720,576},{1280,720},{1024,768}};
	int i;

	switch(vcodecmethod) {
	case ENC_H264:
		for(i=0;i<sizeof(dim_h264_support)/8;i++) {
			if(dim_h264_support[i][0]==width&&dim_h264_support[i][1]==height) {
				return 1;
			}	
		}
		break;
	default:
		return -1;
	}
	return 1;
}
#define INITIAL_GET_PARA(con_para,parser,startIn) con_para.buf_audio=parser->AudioDecInBuffer; \
                                            		con_para.buf_video=parser->AviVFrameBuffer; \
                                            		con_para.fb=parser->fd; \
                                                    con_para.start_index=startIn; \
                                                    con_para.abuf_used_len = 0; \
                                                    con_para.vbuf_used_len = 0; \
                                                    con_para.abuf_len=AUDIO_INBUF_LEN; \
                                                    con_para.vbuf_len=VIDEO_DEC_BUFFER_LEN;
#if 0
#define DUMP_CON_PARA(con_para)  LOGT("abuf_used_len=0x%x afrm_count=%d last_aindex=%d vbuf_used_len=0x%x vfrm_count=%d last_vindex=%d\n",con_para.abuf_used_len, con_para.afrm_count,con_para.last_aindex,con_para.vbuf_used_len,con_para.vfrm_count,con_para.last_vindex);
#else
#define DUMP_CON_PARA(con_para) do{}while(0);
#endif

#ifdef APPEND_AVI_IDX
#define RECOVER_PARA(con_para,parser,pap) pap->Raframenum=con_para.last_aindex; \
                                    		pap->Rvframenum=con_para.last_vindex; \
                                            if((parser->audio_count = con_para.abuf_used_len)>0) memcpy((parser->AudioDecInBuffer+con_para.abuf_used_len),&con_para.last_aindex,4); \
                                            if((parser->count = con_para.vbuf_used_len)>0) memcpy((parser->AviVFrameBuffer+con_para.vbuf_used_len),&con_para.last_vindex,4); \
                                            DUMP_CON_PARA(con_para)

#else
#define RECOVER_PARA(con_para,parser,pap) pap->Raframenum=con_para.last_aindex; \
                                    		pap->Rvframenum=con_para.last_vindex; \
                                            parser->audio_count = con_para.abuf_used_len; \
                                            parser->count = con_para.vbuf_used_len; \
                                            DUMP_CON_PARA(con_para)
#endif



static int Avi_Start(PlayerFileParser *parser)
{
	int err;
	int vcodecmethod;
	unsigned short FrameRate;
	unsigned short XDist,YDist;
	CON_GET_PARA con_para;

	AviPrivateMember* pap;

	FUNC_ENTER
		
	err = Avi_Get_Header(parser->fd,1);
	if(err){
		err= ERR_AVIGETHEADER;
		LOGE("%s:Avi_Get_Header failed @%d\n",__func__,err);
		goto errout;
	}
	pap=(AviPrivateMember*)parser->specInfo;

	XDist = Avi_Get_XDist();
	YDist = Avi_Get_YDist();
	FrameRate = (unsigned short)Avi_Get_FrameRate();

//check video format	
	vcodecmethod = Avi_Get_VCodecMethod();
	if(Check_Video_Decoder(vcodecmethod,XDist,YDist)<0) {
		LOGE("%s:Check_Video_Decoder failed\n",__func__);
		err=ERR_NOT_SUPPORT_VIDEO_FORMAT;
		goto errout;
	}
//check audio format
	if(Avi_HaveAudio()) {
		//let it be
	}	
	if(Avi_HaveAudio())
	{
#ifdef SUPPORT_AUDIO_ENC	
	    INITIAL_GET_PARA(con_para,parser,0)

		err=Avi_Get_Next_Consecutive_frame(&con_para);
		if(err<=0) {
			if(err==0) {
				parser->bend = TRUE;
				err=ERR_AVIFILEEND;
			}
			goto errout;
		}	
        RECOVER_PARA(con_para,parser,pap)
#endif            
	} else {
    	pap->VideoDecCount = 1;
    	if((err=avi_fill_video(parser))<0) {
    		goto errout;
    	}
	}	
	return ERR_OK;
	
errout:
	parser->berror = TRUE;

	Avi_FreeData();	
	return err;
}


static void Avi_NormalPlay(PlayerFileParser *parser)
{
	int err;
	AviPrivateMember* pap;
	CON_GET_PARA con_para;

	pap=(AviPrivateMember*)playerfileparser->specInfo;
	
	if(Avi_HaveAudio()){
#ifdef SUPPORT_AUDIO_ENC        
   	    INITIAL_GET_PARA(con_para,parser,((pap->Raframenum>pap->Rvframenum?pap->Raframenum:pap->Rvframenum)+1))

		err=Avi_Get_Next_Consecutive_frame(&con_para);
		RECOVER_PARA(con_para,parser,pap)

		if(err<0) 
			goto err_out;
        else if(err==0) {
            parser->bend= TRUE;
        }                
#endif            
	}else {
    	if(avi_fill_video(parser)<0) {
    		goto err_out;
    	}
	}
	return;
err_out:
	parser->berror = TRUE;
    ERR("Avi_Get_Next_Consecutive_frame return %d\r\n",err);
}
//called when restart from pause or speed recover to normal
static void Avi_ReStart(PlayerFileParser *parser)
{
	int i,err;
	unsigned char ucerr;
	unsigned long outbufferusedbytes1;
	unsigned long outbufferusedbytes2;

	CON_GET_PARA con_para;
	AviPrivateMember* pap;
	pap=(AviPrivateMember*)parser->specInfo;	


	parser->playbackstatus = PLAYBACKNORMAL;
	pap->Rvframenum = Avi_Next_KeyVFrame(pap->Rvframenum);
	
	if(pap->Rvframenum == -1){
		parser->bend = TRUE;
		return;
	}	
	if(Avi_HaveAudio())
	{
#ifdef SUPPORT_AUDIO_ENC	
	    INITIAL_GET_PARA(con_para,parser,pap->Rvframenum)

		err=Avi_Get_Next_Consecutive_frame(&con_para);
		RECOVER_PARA(con_para,parser,pap)

		if(err<=0) 
			goto err_out;
//        else if(err==0) { //SACH:W10851
//            parser->bend= TRUE;
//        } 
#endif            
	}else {
    	if(avi_fill_video(parser)<0)
    		goto err_out;
    }
	return;

err_out:	
	parser->bend = TRUE;	
}
unsigned char parameter_set[]={
	0x00,0x00,0x00,0x01,0x27,0x64,0x00,0x28,
0xad,0x88,0x0e,0x43,0x98,0x20,0xe1,0x0c,0x29,0x0a,0x44,0x07,0x21,0xcc,0x10,0x70,
0x86,0x14,0x85,0x22,0x03,0x90,0xe6,0x08,0x38,0x43,0x0a,0x42,0x90,0xc0,0x42,0x18,
0xc2,0x1c,0x66,0x32,0x10,0x86,0x02,0x10,0xc6,0x10,0xe3,0x31,0x90,0x84,0x30,0x10,
0x86,0x30,0x87,0x19,0x8c,0x84,0x22,0x02,0x11,0x98,0xce,0x23,0xc2,0x9f,0x11,0xf8,
0x8f,0xe2,0x3f,0x11,0xf1,0x1e,0x33,0x88,0xc4,0x44,0x42,0x81,0x08,0x8c,0x47,0x11,
0xe2,0x3e,0x4f,0xc4,0x7f,0x27,0xe4,0xf8,0x8f,0x11,0xc4,0x64,0x88,0xb4,0x02,0x00,
0x30,0xc8,0x00,0x00,0x00,0x01,0x28,0xee,0x3c,0xb0
};
static void Avi_FastForward(PlayerFileParser *parser)
{
	int count;
	char *fillpt = NULL;	
	AviPrivateMember* pap;
	int fake_header=0x01000000;

	pap=(AviPrivateMember*)parser->specInfo;

//FRAME_CAPTURE_RATE frames per second in normal speed and the i frame interval is IDR_FRAME_INTERVAL
//that is to say one i frame per second in normal speed.
//when fast forward only i frame is used. the sleep time is divided by speed.
#ifdef SKIP_FRAME_DELAY_WHEN_SPEED_CHANGE
	if(skip_count>SKIP_FRAME_COUNT) {
#endif		
		switch(parser->speed)	//select stratages due to speed 
		{	
			case SPEED_2X: 
				usleep((1000000*IDR_FRAME_INTERVAL)/(FRAME_CAPTURE_RATE*2));
				break;
			case SPEED_4X:
				usleep((1000000*IDR_FRAME_INTERVAL)/(FRAME_CAPTURE_RATE*4));
				break;
			case SPEED_8X:
				usleep((1000000*IDR_FRAME_INTERVAL)/(FRAME_CAPTURE_RATE*8));
				break;
			case SPEED_16X:
			case SPEED_32X:		
			default:	
				usleep((1000000*IDR_FRAME_INTERVAL)/(FRAME_CAPTURE_RATE*16));
				break;
		} 
#ifdef SKIP_FRAME_DELAY_WHEN_SPEED_CHANGE		
	}else
		skip_count++;
#endif
	parser->count = 0;

#ifdef SUPPORT_AUDIO_ENC
    parser->audio_count = 0;
#endif
	pap->Rvframenum = Avi_Next_KeyVFrame(pap->Rvframenum);
	if(pap->Rvframenum>0) {
		fillpt = parser->AviVFrameBuffer;
#if 0
		memcpy(fillpt,parameter_set,sizeof(parameter_set));
		parser->count+=sizeof(parameter_set);
		fillpt+=sizeof(parameter_set);
#endif

		count = Avi_Get_Frame(fillpt,VIDEO_DEC_BUFFER_LEN ,parser->fd,pap->Rvframenum,0);	
		if(count<0) {
			parser->berror= TRUE;
		}else {
			parser->count +=  count;
			pap->Rvframenum++;
#if 0
			memcpy(fillpt+count,&fake_header,4);
			parser->count += 4;
#endif
#if 0
			count=Avi_Get_Frame(fillpt+count,VIDEO_DEC_BUFFER_LEN-count,parser->fd,pap->Rvframenum,0);
			if(count>0) {
				parser->count+=count;
				pap->Rvframenum++;
			}
#endif			
		}		
	} else if(pap->Rvframenum==0){
		parser->bend = TRUE;
	}else {
		parser->berror = TRUE;
	}
//	LOGT("%s:got frame %d len=0x%x\n",__func__,pap->Rvframenum,parser->count);
}

static void Avi_SlowForward(PlayerFileParser *parser)
{
	int count;
	char *fillpt = NULL;
	
	AviPrivateMember* pap;
	pap=(AviPrivateMember*)parser->specInfo;	
	
	switch(parser->speed)	//select stratages due to speed 
	{
		case SPEED_HALF:
			usleep(2*1000000/FRAME_CAPTURE_RATE);
			break;
		case SPEED_QUATER:
			usleep(4*1000000/FRAME_CAPTURE_RATE);
			break;
		case SPEED_1IN8:
			usleep(8*1000000/FRAME_CAPTURE_RATE);
			break;
		case SPEED_1IN16:
		case SPEED_1IN32:
		default:
			usleep(16*1000000/FRAME_CAPTURE_RATE);
			break;
	}
	pap->Rvframenum = Avi_Next_VFrame(pap->Rvframenum);
	parser->count = 0;	
#ifdef SUPPORT_AUDIO_ENC
    parser->audio_count = 0;
#endif
	if(pap->Rvframenum>0) {
		fillpt = parser->AviVFrameBuffer;
		count = Avi_Get_Frame(fillpt,VIDEO_DEC_BUFFER_LEN,parser->fd,pap->Rvframenum,0);	
		if(count<0) {
			parser->berror=TRUE;
		}else {
			parser->count =  count;
			pap->Rvframenum++;
		}		
	}else if(pap->Rvframenum==0) {
		parser->bend =  TRUE;
	}else {
		parser->berror=TRUE;
	}
}

static void Avi_SlowRewind(PlayerFileParser *parser)
{
//not support yet
}

static void Avi_FastRewind(PlayerFileParser *parser)
{
	int count;
	char *fillpt = NULL;
	
	AviPrivateMember* pap;
	pap=(AviPrivateMember*)parser->specInfo;	
#ifdef FLUSH_LOADERCACHE_WHEN_SPEED_CHANGE	
	if(skip_count>SKIP_FRAME_COUNT) {
#endif		
		switch(parser->speed)	//select stratages due to speed 
		{	
			case SPEED_2X:
				usleep(80000);
				break;
			case SPEED_4X:
				usleep(40000);
				break;
			case SPEED_8X:
				usleep(20000);
				break;
			case SPEED_16X:
			case SPEED_32X:		
			default:	
				usleep(10000);
				break;
		} 
#ifdef FLUSH_LOADERCACHE_WHEN_SPEED_CHANGE		
	}else
		skip_count++;
#endif
	pap->Rvframenum = Avi_Back_KeyVFrame(pap->Rvframenum);
	parser->count = 0;
#ifdef SUPPORT_AUDIO_ENC
    parser->audio_count = 0;
#endif
	if(pap->Rvframenum!=-1) {
		fillpt = parser->AviVFrameBuffer;
		count = Avi_Get_Frame(fillpt,VIDEO_DEC_BUFFER_LEN,parser->fd,pap->Rvframenum,0);	
		if(count<0) {
			parser->berror=TRUE;
		}else {
			if(pap->Rvframenum == 0) { //return to normal play
                Post_UI_Message(MSG_DECODER_EVENT,MSG_PARA_DEC_FILEBEGIN);
			}else
				pap->Rvframenum--;
			parser->count =  count;
		}	
	}else {
		parser->berror = TRUE;
	}
}

static void Avi_Pause(PlayerFileParser *parser)
{
#if 0
	unsigned char err;
//JunR remark	
	while(1) {
		OSSemPend(PauseAviSem, 10, &err);	
		if(err==OS_TIMEOUT) {
			if(playerfileparser->playbackstatus!=PLAYBACKPAUSE) {// we lost a post?
				break;
			}
		}
		OSSemSet(PauseAviSem, 0, &err);		
	}	
	OSSemSet(AudioPlaySem, 0, &err);	
#endif	
	Avi_ReStart(parser);
}

static void Avi_Deinit(PlayerFileParser *parser)
{
	if(Avi_HaveAudio()) {
		
	}
	Avi_FreeData();
}

static int Avi_GetPosition(PlayerFileParser *parser)
{
#if 0
	double pos,framerate;
	int vcount;
	framerate = Avi_Get_FrameRate();
	vcount = avi_viframe(((AviPrivateMember*)(playerfileparser->specInfo))->Rvframenum);
	if(vcount<0)
		return Avi_GetFilePlayTime();
	pos = vcount/framerate;
	pos*=1000;
	return (int)pos;
#else
	double pos;
	unsigned long frame_total;
	frame_total=Avi_Get_IndexCount();
	pos=Avi_GetFilePlayTime();
	if(frame_total!=0)
		pos=(((AviPrivateMember*)(parser->specInfo))->Rvframenum*pos)/frame_total;
	else
		pos=0;
	return (int)pos;
#endif	
}
static void Avi_Goto(PlayerFileParser *parser,unsigned int time)
{
	unsigned int tempframe;
	tempframe = Avi_GotoSpecialTime(time);
	((AviPrivateMember*)(parser->specInfo))->Rvframenum = Avi_Next_KeyVFrame(tempframe);
	Avi_ReStart(parser);
}
	
/*******************************************************************/
static PlayerFileParser Avi_Parser = {
	PLAYBACKNORMAL,//playbackstatus
	FALSE,//bstop
	FOWARD,//orient
	SPEED_NORMAL,//speed
	FALSE,//berror
	FALSE,//bend
	0,	//decodetotal
	0,	//decodeFrame
	0,	//gototime
	NULL,  //specInfo
	NULL, //AviVFrameBuffer
	0,  //count
#ifdef SUPPORT_AUDIO_ENC
    NULL, //AudioDecInBuffer
    0,  //audio_count
#endif
	-1, //fd
	Avi_Start,
	Avi_GetInfo,
	Avi_NormalPlay,
	Avi_FastForward,
	Avi_FastRewind,
	Avi_SlowForward,
	Avi_SlowRewind,
	Avi_ReStart,
	Avi_Pause,
	Avi_GetPosition,
	Avi_Goto,
	Avi_BeginPrefetch,
	Avi_DoPrefetch,
	Avi_StopPrefetch,
	Avi_Deinit,
};
/*******************************************************************/

/*
called in decoder thread
*/
static void DeInitialize_Parser(PlayerFileParser* parser)
{
//avi parser is the only one in our system,no need to free it's memory
	if(parser!=&Avi_Parser) {
		LOGE("%s:unknown parser!!\n",__func__);		
		if(parser->specInfo) {
			free(parser->specInfo);
			parser->specInfo = NULL;
		}	
		if(parser->AviVFrameBuffer !=NULL) {
			free(parser->AviVFrameBuffer);
			parser->AviVFrameBuffer = NULL;
		}
#ifdef SUPPORT_AUDIO_ENC
		if(parser->AudioDecInBuffer !=NULL) {
			free(parser->AudioDecInBuffer);
			parser->AudioDecInBuffer = NULL;
		}
#endif        
	}

	if(parser->fd!=-1) {
		close(parser->fd);
		parser->fd =-1;
	}
}

/*
called in  decoder thread and ui thread
*/
static int Initialize_Parser(PlayerFileParser** parser ,const char *filename)
{
	int len = strlen(filename);
	int i,ret;
	char extend[4];
	
	strncpy(extend,&filename[len-3],3);
	extend[3] = 0;
	for(i=0;i<4;i++)
		extend[i] = tolower((int)extend[i]);
	if(strcmp(extend,"avi")) {
		LOGE("%s: not support file %s\n",__func__,filename);
		return -1;
	}
	i = open(filename,O_RDONLY);
	if(i < 0) {
		LOGE("%s: open %s failed for %d\n",__func__,filename,i);
		return -1;
	}		
	*parser  = &Avi_Parser;
	(*parser)->fd = i;
	if(NULL == Avi_Parser.specInfo) {
		Avi_Parser.specInfo = malloc(sizeof(AviPrivateMember)); 
		if(Avi_Parser.specInfo == NULL) {
			ret = -1;
			goto errout;
		}
	}
	memset(Avi_Parser.specInfo,0,sizeof(AviPrivateMember));
	if(NULL==Avi_Parser.AviVFrameBuffer) {
		Avi_Parser.AviVFrameBuffer = (char*)malloc(VIDEO_DEC_BUFFER_LEN);
		if(Avi_Parser.AviVFrameBuffer == NULL) {
			ret = -2;
			goto errout;
		}
        //LOGT("%s:Avi_Parser.AviVFrameBuffer=0x%x\n",__func__,Avi_Parser.AviVFrameBuffer);
	}	
#ifdef SUPPORT_AUDIO_ENC
	if(NULL==Avi_Parser.AudioDecInBuffer) {
		Avi_Parser.AudioDecInBuffer = (char*)malloc(AUDIO_INBUF_LEN);
		if(Avi_Parser.AudioDecInBuffer == NULL) {
			ret = -3;
			goto errout;
		}
        //LOGT("%s:Avi_Parser.AudioDecInBuffer=0x%x\n",__func__,Avi_Parser.AudioDecInBuffer);
	}	    
#endif    
	(*parser)->playbackstatus = PLAYBACKNORMAL;
	(*parser)->bend = 0;
	(*parser)->berror = 0;
	(*parser)->bstop = 0;
	(*parser)->DecodeFrame = 0;
	(*parser)->DecodeTotal = 0;
	(*parser)->count = 0;
#ifdef FLUSH_LOADERCACHE_WHEN_SPEED_CHANGE
	(*parser)->flush_cache=0;	
#endif
	(*parser)->speed = SPEED_NORMAL;
	return 0;	

errout:		
	if(*parser!=NULL)
		DeInitialize_Parser(*parser);
	request2reboot();
	return ret;		
}


/****************************************************************************************************************/



void Player_Deinit_w(PlayerFileParser *parser)
{
	FUNC_ENTER
	if(playerfileparser!=NULL) {
		if(playerfileparser->StopPrefetch!=NULL)
			playerfileparser->StopPrefetch(parser);
		if(playerfileparser->Deinit)
			playerfileparser->Deinit(parser);
		DeInitialize_Parser(playerfileparser);
		playerfileparser = NULL;
	}	
	PlayerRun = 0;	
	FUNC_EXIT
}

PlayerFileParser *Player_Init_w(char* filename)
{
	int err;
	PlayerFileParser* pparser = NULL;

    //FUNC_ENTER
	if(playerfileparser!=NULL) {
		Player_Deinit_w(playerfileparser);
		playerfileparser = NULL;
		LOGE("%s: playerfileparser not empty\n",__func__);
	}	
	
	err = Initialize_Parser(&pparser,filename);//init parser
	if(err!=0) {
		LOGE("%s: Initialize_Parser failed for %d\n",__func__,err);
		return NULL;
	}
	playerfileparser=pparser;
	playerfileparser->playbackstatus = PLAYBACKNORMAL;
	PlayerRun =1;
	
	return playerfileparser;
}

int Player_get_data_w(PlayerFileParser *parser)
{
	int msgcode,err,ret;
	switch(playerfileparser->playbackstatus) {
		case PLAYBACKNORMAL:
			if(playerfileparser->NormalPlay)
				playerfileparser->NormalPlay(parser);
			break;
		case PLAYBACKPAUSE:
			if(playerfileparser->Pause)
				playerfileparser->Pause(parser);
			break;
		case PLAYBACKFASTFORWARD:
			if(playerfileparser->FastForward)
				playerfileparser->FastForward(parser);
			break;
		case PLAYBACKFASTREWIND:	
			if(playerfileparser->FastRewind)
				playerfileparser->FastRewind(parser);
			break;
		case PLAYBACKRECOVER:				
			if(playerfileparser->ReStart)
				playerfileparser->ReStart(parser);
			break;
		case PLAYBACKGOTO:				
			if(playerfileparser->Goto)
				playerfileparser->Goto(parser,playerfileparser->gototime);
			break;
		case PLAYBACKSLOWFORWARD:
			if(playerfileparser->SlowForward)
				playerfileparser->SlowForward(parser);
			break;
		case PLAYBACKSLOWREWIND:
			if(playerfileparser->SlowRewind)
				playerfileparser->SlowRewind(parser);
			break;
		default:
			break;
	}
	ret = 1;				
	if(playerfileparser->berror) { 
		LOGT("%s:error\n",__func__);
		msgcode = ERR_MP3DECODER;
		ret = -1;
		goto MSG_HANDLER;
	}
			
	if(playerfileparser->bend) {	
		LOGT("%s:end\n",__func__);		
		ret = 0;
		msgcode = MSG_PARA_DEC_FILEEND;
		goto MSG_HANDLER;				
	}
			
	if(playerfileparser->bstop) {
		LOGT("%s:stop\n",__func__);		
		ret = 0;
		msgcode =  MSG_PARA_DEC_STOP;
		goto MSG_HANDLER;	
	} 
	if(playerfileparser->DoPrefetch!=NULL&&ret==1) {
		if((err=playerfileparser->DoPrefetch(parser))<0) {
			msgcode=err;
			ret = -1;
			goto MSG_HANDLER;
		}
	}
	
MSG_HANDLER:
	//to do:notify ui thread
	return ret;
}



/*************************** interface to ui thread******************************************/


int Player_Start(char *filename)
{
	int len;
	FUNC_ENTER	
	if(PlayerRun!=0) {
		LOGE("%s: PlayerRun not zero\n",__func__);
		return ERR_INVALID_DEC_STATE;
	}	
	len=strlen(filename);
	if(len>255)
		return -1;
	strncpy(PlayBackFilename,filename,len);
	PlayBackFilename[len] = 0;
	init_decode_thread_group();
	if((len=start_decoder_thread_group(PlayBackFilename))>0)
		return ERR_OK;
	else {
		LOGE("%s: start_decoder_thread_group ret %d\n",__func__,len);
		request2reboot();
		return len;
	}	
}

void Player_Stop(void)
{
	LOGT("%s\n",__func__);
	if(PlayerRun&&playerfileparser!=NULL){
		playerfileparser ->bstop=TRUE;
		stop_decoder_thread_group();
	}
}
#ifdef FLUSH_LOADERCACHE_WHEN_SPEED_CHANGE
static int Player_FlushCacheCheck(enum PlayerOrient orient,enum PlayBackSpeed speed)
{
/*
  flush cache needed in following situation: x means flush
  
  normal play --c1-> fast forwardx1 --->fast forwardx2---->normal play
  normal play --c2-> fast rewindx1  ---->fast rewindx2 --c3->normal play 
  normal play ----> slow forwardx1----->slow forwardx2-->normal play
  normal play --c1-> fast forwardx1 --c4-->fast rewindx1 --c5-> normal play

*/  
	if(playerfileparser->orient!=orient)  //c2 c3 c5 c4
		return 1;
	else if(playerfileparser->speed == SPEED_NORMAL) {
		if(speed==SPEED_2X||speed==SPEED_4X||speed==SPEED_8X||speed==SPEED_16X||speed==SPEED_32X) { //c1
			return 1;
		}
	}
	return 0;
}
#endif

int Player_SetSpeed(enum PlayerOrient orient,enum PlayBackSpeed speed) 
{
	LOGT("%s:orient=%d speed=%d\n",__func__,orient,speed);
	if(PlayerRun==0||playerfileparser==NULL)
		return ERR_INVALID_DEC_STATE;
#ifdef SKIP_FRAME_DELAY_WHEN_SPEED_CHANGE
	if(playerfileparser->orient!=orient)
		skip_count = 0;
#endif
#ifdef FLUSH_LOADERCACHE_WHEN_SPEED_CHANGE
	playerfileparser->flush_cache = Player_FlushCacheCheck(orient,speed);	
#endif
	playerfileparser->orient = orient;
	playerfileparser->speed = speed;	
	if(speed == SPEED_NORMAL){
		playerfileparser->playbackstatus=PLAYBACKRECOVER;
	} else if(speed>0) {
		if(orient == FOWARD) {			
			playerfileparser->playbackstatus=PLAYBACKFASTFORWARD;
		} else {
			playerfileparser->playbackstatus=PLAYBACKFASTREWIND;
		}
	} else {
		if(orient == FOWARD) {
			playerfileparser->playbackstatus=PLAYBACKSLOWFORWARD;
		} else {
		#if 0
			playerfileparser->playbackstatus=PLAYBACKSLOWREWIND;
		#else
			return -1; // no slowrewind support
		#endif
		}	
	}	
	return 0;
}

int Player_GetPos(void)
{
	if(PlayerRun==0||playerfileparser==NULL)
		return ERR_INVALID_DEC_STATE;
	return playerfileparser->GetPosition(playerfileparser);
}

int Player_GotoPos(int timesec)
{
	if(PlayerRun==0||playerfileparser==NULL)
		return ERR_INVALID_DEC_STATE;
	playerfileparser->gototime=timesec;
	playerfileparser->playbackstatus=PLAYBACKGOTO;
	return 0;
}

void Player_Pause(BOOL bpause) //Pause 1:pause 0:
{
	if(playerfileparser != NULL && PlayerRun) {
		if(bpause) {
			playerfileparser->playbackstatus=PLAYBACKPAUSE;
			pause_decoder_thread(1);
		} else {
			pause_decoder_thread(0);
			playerfileparser->playbackstatus=PLAYBACKNORMAL;
		}
	}
}

int Player_GetInfo(const char *filename,PlayerInfo* info)
{
	int ret;
	PlayerFileParser *parser = NULL;

	if(Initialize_Parser(&parser,filename)<0) {
		return ERR_INVALID_PARSER;	
	}	
	ret=parser->GetInfo(parser,info);
	DeInitialize_Parser(parser);
	return ret;
}		

int Player_GetStatus(void){
	if(playerfileparser!=NULL)
		return playerfileparser->playbackstatus;
	return 0;
}

