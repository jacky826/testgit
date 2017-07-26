#ifndef _ALL_DATA_HEADER_
#define _ALL_DATA_HEADER_ 
//--------------------------------------------------------------------------//
//	Purpose	:	System value in ALL_DATA structure
//--------------------------------------------------------------------------//
//
#include "config.h"
#include "pvr_types.h"
#include "RTC.h"

#define SORT_METHOD_NAME				(0)
#define SORT_METHOD_TYPE				(1)
#define SORT_METHOD_SIZE				(2)
#define SORT_METHOD_DATE				(3)
#define SORT_METHOD_CNT				(4)

#define SCHEDULE_RECSTATUS_CNT		(5)

#define SCHEDULE_ENABLE				(1)
#define SCHEDULE_DISABLE				(0)

#define IS_SCHEDULE_VALID(p)			((p)->RecIdx<REC_FORMAT_CNT)

#define RECORDMODE_WEEKLY				(0)
#define RECORDMODE_DAILY				(1)
#define RECORDMODE_ONCE				(2)

#define SCHEDULE_RECORDMODE_CNT		(3)

#define TV_BAND_CNT			(3)		//电视频段: UHF; VHF_H; VHF_L

#define VIDEOMODE_PAL_DK				(0)
#define VIDEOMODE_PAL_I				(1)
#define VIDEOMODE_PAL_BG				(2)
#define VIDEOMODE_NTSC					(3)
#define VIDEOMODE_SECAM				(4)

#define VIDEO_MODE_CNT			(5)		//支持的电视制式种类--0:PAL_DK; 1:PAL_I; 2:PAL_BG; 3:NTSC 5 SCEAM

#define VIDEOINPUT_SVIDEO				(0)
#define VIDEOINPUT_CVBS				(1)
#define VIDEOINPUT_CABLE				(2)

#define INPUT_SELECT_CNT		(3)		//视频输入源种类: S-video;CVBS;Cable

#define VIDEOOUTPUT_SVIDEO				(0)
#define VIDEOOUTPUT_CVBS				(1)
#define VIDEOOUTPUT_COMPONENT		(2)

#define OUTPUT_SELECT_CNT		(3)		//视频输入源种类: S-video;CVBS;Component
/*
 *----- From Global.h
 */
#define FRAMERATE_FULL					(3)
#define FRAMERATE_FULL_NTSC			(30)
#define FRAMERATE_FULL_PAL				(25)
#define DEFREC_FRAMERATE_CNT			(FRAMERATE_FULL+1)
extern const int SupportFrameRate[];//PVR_UI_Func.c

/*Encoder h*/

#define DEFREC_RECTO_CNT				(SUPPORT_DISK_CNT)
//Order as support 

#define DEFREC_NICAMMODE_CNT			(3)
#define NICAMMODE_A					(0)
#define NICAMMODE_B					(1)
#define NICAMMODE_DUAL				(2)

//RecAutoChapter(Clip)
#define DEFREC_AUTOMARKERTIME_CNT	(4)
#define NO_CLIP		0
#define CLIP_10MIN	1
#define CLIP_20MIN	2
#define CLIP_30MIN	3
//Resolution(Dist)
#define DEFREC_RESOLUTION_CNT			(5)
#define DIST_QCIF	0
#define DIST_CIF 	1
#define DIST_HALFD1	2
#define DIST_D1		3
#define DIST_QVGA	4
//Quality(EQuality)
#define DEFREC_QUALITY_CNT				(4)

enum VideoQuality
{
	VIDEO_SP	= 0,
	VIDEO_LP	= 1,
	VIDEO_EP	= 2,
	VIDEO_HP	= 3	
};

#define QUALITY_SP	0
#define QUALITY_LP	1
#define QUALITY_EP	2
#define QUALITY_HP	3

//Format(MFileFormat)
#define DEFREC_FILEFORMAT_CNT			(4)
#define FORMAT_AVI	0
#define FORMAT_3GP	1
#define FORMAT_MOV	2
#define FORMAT_MP4		3	//When add format need add DEFREC_FILEFORMAT_CNT

//Format  add on
#define DEVICE_DEFAULT	(0x0)
#define DEVICE_PSP		(0x01)

//VideoCodec(Vmethod)
#define ENC_H264	0
#define ENC_MPEG4	1
#define ENC_H263	2
//AudioCodec(Amethod)
#define ENC_MP3		0
#define ENC_AAC		1
#define ENC_AMR		2


typedef struct _SaveAndPlayPara{
	unsigned char Vmethod;
	unsigned char Amethod;
	unsigned char MFileFormat;
	enum VideoQuality EQuality;
}SaveAndPlayPara;

/*Decoder Part*/

enum PlayBackSpeed
{	
	SPEED_1IN32 = -31,
	SPEED_1IN16 = -62,
	SPEED_1IN8 = -125,
	SPEED_QUATER = -250,
	SPEED_HALF = -500,
	SPEED_NORMAL = 0,
	SPEED_2X = 31,
	SPEED_4X = 62,
	SPEED_8X = 125,
	SPEED_16X = 250,
	SPEED_32X = 500
};

#define PLAYSPEEDFAST_CNT	5
extern const int PlaybackSpeedFast[];
#define PLAYSPEEDSLOW_CNT	5
extern const int PlaybackSpeedSlow[];

typedef struct {
	unsigned short xdist;
	unsigned short ydist;
	
	unsigned int nextclip;
	unsigned int playtime;
	unsigned char acodec;
	unsigned char vcodec;
	unsigned short framerate;
}PlayerInfo;

enum PlayerOrient {FOWARD,BACKWARD};

#define RECSETTING_AUTOCLIP	(0)
#define RECSETTING_FILEFORMAT	(1)
#define RECSETTING_RESOLUTION	(2)
#define RECSETTING_QUALITY		(3)
#define RECSETTING_FRAMERATE	(4)
#define RECSETTING_RECORDTO	(5)
#define RECSETTING_NICAMMODE	(6)

#define RECSETTING_VCODEC		(7)
#define RECSETTING_ACODEC		(8)

#define RECSETTING_DEVICE		(9)

typedef struct {
	//AUTO Chapter|FILE Format|Resolution|Quality|FrameRate|RecordTo|NicamMode

	unsigned char RecSetting[16];//Actuall Need 7(BUTTON_CNT_RECSETTING), for reserved & allign
	
}TEF_RECINFO;

#define DIGISTAT_SINGLE		(0)
#define DIGISTAT_DOUBLE	(1)


#define AUDIO_MUTE_DISABLE	(0)
#define AUDIO_MUTE_ENABLE		(1)
#define AUDIO_DEFAULT_VOL		(0x73)

typedef struct _CODEC_INFO {
	INT16U codec_level;		//编码质量--分为4档
	INT16U brightness;		//图像亮度
	INT16U chroma;		//图像色度
	INT16U contrast;		//图像对比度
}CODEC_INFO;

//zqh 2015年5月25日 add，
#define ENGLISH  0  //代表英文
#define SICN     1  //代表简体中文
#define TRCN     2  //代表繁体中文


typedef struct _ALL_DATA {
	int MagicNum;

	int CurSortMethod;				//当前排序方式-- 0:Name; 1:Type; 2:Size; 3:Date

	//System Setting
	int Language;						//系统语言--0:英文;1: 简体中文; 2:繁体中文
	
	//Checksum
	int Checksum;					//校验位

    int disk_full_strategy;			//盘满策略--0:自动删除;1:停止保存

	CODEC_INFO	codec_info;		//编码属性设置

    int encode_volume;          // 编码音量设置
    int decode_volume;          // 解码音量设置
} ALL_DATA;

#include "../../pvr_debug.h"
#include "../../pvr_watchdog.h"

#endif
