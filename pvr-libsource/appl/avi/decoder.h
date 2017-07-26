#ifndef _DECODER_H_
#define _DECODER_H_

#include <all_data.h>
#include "pvr_error.h"

#define PLAYBACKNORMAL			0
#define PLAYBACKFASTFORWARD	1
#define PLAYBACKFASTREWIND	2
#define PLAYBACKSLOWFORWARD	3
#define PLAYBACKSLOWREWIND	4
#define PLAYBACKRECOVER		5
#define PLAYBACKPAUSE			6
#define PLAYBACKGOTO			7


typedef struct tagPlayerFileParser{	
	int playbackstatus;	
	BOOL bstop;
	enum PlayerOrient orient;
	enum PlayBackSpeed speed;
	BOOL berror;
	BOOL bend;
	unsigned int DecodeTotal;
	unsigned int DecodeFrame;
	unsigned int gototime;
	void* specInfo;//与文件类型有关的信息
	char *AviVFrameBuffer;//buffer for current video frame
	int count; //valid data len in AviVFrameBuffer
#ifdef SUPPORT_AUDIO_ENC
	char *AudioDecInBuffer; //buffer for current audio frame
	int audio_count; //valid data len in AudioDecInBuffer
#endif    
	int fd; //file descriptor to media file
/*********************************************/	
	int (*Start) (struct tagPlayerFileParser *pparser);
	int (*GetInfo) (struct tagPlayerFileParser *pparser,PlayerInfo*);
	void (*NormalPlay) (struct tagPlayerFileParser *pparser);
	void (*FastForward) (struct tagPlayerFileParser *pparser);
	void (*FastRewind) (struct tagPlayerFileParser *pparser);
	void (*SlowForward) (struct tagPlayerFileParser *pparser);
	void (*SlowRewind) (struct tagPlayerFileParser *pparser);	
	void (*ReStart) (struct tagPlayerFileParser *pparser);
	void (*Pause) (struct tagPlayerFileParser *pparser);
	int (*GetPosition) (struct tagPlayerFileParser *pparser);
	void (*Goto) (struct tagPlayerFileParser *pparser ,unsigned int);
	int (*BeginPrefetch)(struct tagPlayerFileParser *pparser);
	int (*DoPrefetch)(struct tagPlayerFileParser *pparser);
	int (*StopPrefetch)(struct tagPlayerFileParser *pparser);
	void (*Deinit) (struct tagPlayerFileParser *pparser);
/*<2010-12-5 renjun add begin: flush loader's cache if play speed or play orient changed*/	
#ifdef FLUSH_LOADERCACHE_WHEN_SPEED_CHANGE
	int flush_cache;
#endif
/*2010-12-5 renjun add end>*/
///////////////////////////////////////////////////////
	void *preserved;// used by decoder worker thread	
}PlayerFileParser;

//following function is called in ui thread

int Player_Start(char *filename);
void Player_Stop(void);
int Player_SetSpeed(enum PlayerOrient orient,enum PlayBackSpeed speed);
int Player_GetPos(void);
int Player_GotoPos(int timesec);
void Player_Pause(BOOL bpause);
int Player_GetInfo(const char *filename,PlayerInfo* info);


//following function is called in real working decode  thead
PlayerFileParser *Player_Init_w(char* filename);
void Player_Deinit_w(PlayerFileParser *parser);
int Player_get_data_w(PlayerFileParser *parser);

#endif
