#ifndef _AVI_IO_H
#define _AVI_IO_H

#include "movcommon.h"

#define AVISF_DISABLED          0x00000001
#define AVISF_VIDEO_PALCHANGES  0x00010000
#define AVIF_HASINDEX        0x00000010 // Index at end of file?
#define AVIF_MUSTUSEINDEX    0x00000020
#define AVIF_ISINTERLEAVED   0x00000100
#define AVIF_TRUSTCKTYPE     0x00000800 // Use CKType to find key frames
#define AVIF_WASCAPTUREFILE  0x00010000
#define AVIF_COPYRIGHTED     0x00020000
#define FCC(ch4) ((((unsigned int)(ch4) & 0xFF) << 24) |     \
                  (((unsigned int)(ch4) & 0xFF00) << 8) |    \
                  (((unsigned int)(ch4) & 0xFF0000) >> 8) |  \
                  (((unsigned int)(ch4) & 0xFF000000) >> 24))

#define ckidAVIOLDINDEX FCC('idx1')

#ifndef AVIIF_LIST
#define AVIIF_LIST       0x00000001
#define AVIIF_KEYFRAME   0x00000010
#endif
#define AVIIF_NO_TIME    0x00000100
#define AVIIF_COMPRESSOR 0x0FFF0000  // unused?

typedef struct _M_RIFFLIST
{
	unsigned int fcc;
	unsigned int cb;
	unsigned int fccListType;
}M_RIFFLIST;

typedef struct _M_RIFFCHUNK {
	unsigned int fcc;
	unsigned int  cb;
}M_RIFFCHUNK, * LPM_RIFFCHUNK;

typedef struct _M_AVIMAINHEADER
{
	unsigned int fcc;
	unsigned int cb;
	unsigned int dwMicroSecPerFrame;
	unsigned int dwMaxBytesPerSec;
	unsigned int dwReserved1;
	unsigned int dwFlags;
	unsigned int dwTotalFrames;
	unsigned int dwInitialFrames;
	unsigned int dwStreams;
	unsigned int dwSuggestedBufferSize;
	unsigned int dwWidth;
	unsigned int dwHeight;
	unsigned int dwReserved[4];
}M_AVIMAINHEADER;





typedef struct _M_AVISTREAMHEADER{
	unsigned int fcc;          // 'strh'
	unsigned int cb;           // size of this structure - 8

	unsigned int fccType;      // stream type codes

	unsigned int fccHandler;

	unsigned int  dwFlags;
	unsigned short  wPriority;
	unsigned short  wLanguage;
	unsigned int  dwInitialFrames;
	unsigned int  dwScale;
	unsigned int  dwRate;       // dwRate/dwScale is stream tick rate in ticks/sec
	unsigned int  dwStart;
	unsigned int  dwLength;
	unsigned int  dwSuggestedBufferSize;
	unsigned int  dwQuality;
	unsigned int  dwSampleSize;
	struct {
		short  left;
		short  top;
		short  right;
		short  bottom;
	} rcFrame;
} M_AVISTREAMHEADER;



typedef	struct _avioldindex_entry {
	unsigned int dwChunkId;
	unsigned int dwFlags;
	unsigned int dwOffset;    // offset of riff chunk header for the data
	unsigned int dwSize;      // size of the data (excluding riff header size)
} aIndex;          // size of this array




typedef struct _avioldindex {
	unsigned int fcc;        // 'idx1'
	unsigned int cb;         // size of this structure -8
} AVIOLDINDEX;

typedef	struct _list_avioldindex_entry{
	aIndex node;
	int index;
	void* next;
	void* priv;
}list_avioldindex_entry;

typedef struct tag_TOTAL_VA_HEADER{
	M_RIFFLIST aviriff;	//12bytes

	M_RIFFLIST list_hdrl;	//12bytes

	M_AVIMAINHEADER mheader;//chunk embedded		64bytes

	M_RIFFLIST list_videostrl;	//12bytes
	M_AVISTREAMHEADER vstream_header; //chunk embedded	68bits
	M_RIFFCHUNK chunk_vstrf;	//8bytes 
	M_BITMAPINFOHEADER vstream_format;//40 bytes
	union {
		struct {
			M_RIFFLIST list_audiostrl;//12bytes
			M_AVISTREAMHEADER astream_header; //chunk embedded	64bits
			M_RIFFCHUNK chunk_astrf;//8 bits
			M_MPEGLAYER3WAVEFORMAT astream_format;//4 //16bytes
//			unsigned int unused;//4 
		}a_info;
		M_RIFFCHUNK chunk_audio;//8bytes
	}u;
}TOTAL_VA_HEADER,*PTOTAL_VA_HEADER;

int Avi_Get_ACodecMethod(void);
int Avi_Get_VCodecMethod(void);
int Avi_Initial_Header(unsigned short cbsize);
int  Avi_Before_AddData(int avifp);
int Avi_Add_Aframe(int ft,const Stream_Info *info);
int Avi_Add_Vframe(int ft,const Stream_Info *pinfo);
int Avi_FlushTailer(int aviwritefile,M_BITMAPINFOHEADER *pbmp,M_MPEGLAYER3WAVEFORMAT *pwave,unsigned long video_time);
int Avi_FreeData(void);
int avi_cluster_flush(int hf);
int Avi_Get_Header(int pb,int getindex);
unsigned short Avi_Get_XDist(void);
unsigned short Avi_Get_YDist(void);
unsigned long Avi_Get_IndexCount(void);
double Avi_Get_FrameRate(void);
unsigned int Avi_GetFilePlayTime(void);
int Avi_GotoSpecialTime(unsigned int time);
int Avi_GotoNextClip(unsigned int alreadyframes,unsigned int inteval);
int Avi_GotoPrevClip(unsigned int alreadyframes, unsigned int inteval);
unsigned int Avi_GetVFrameCount(void);
unsigned int Avi_GetAFrameCount(void);
unsigned int Avi_GetKeyFrameCount(void);
int Avi_Get_Frame(unsigned char *buffer,int buf_size,int fb,unsigned int index,int from_prefetch);
int Avi_Get_Next_Consecutive_frame(PCON_GET_PARA ppara);
int Avi_prestart(int hfile);
int Avi_prestop(int hfile);
int Avi_prefetch(int hfile);

int Avi_Next_AFrame(int index);
int Avi_Next_VFrame(int index);
int Avi_Next_KeyVFrame(int index);
int Avi_Back_KeyVFrame(int index);
int Avi_HaveAudio(void);
int avi_flush_en_index(void);
int avi_vframe(int vfrm);
extern TOTAL_VA_HEADER m_aviheader;		//文件头的信息
extern M_RIFFLIST m_listmovi;				//数据段的信息
extern AVIOLDINDEX m_rindx;				//文件为索引的信息

typedef struct _AviPrivateMember{
	long LastRaframenum;
	long Raframenum;
	long Rvframenum;
	int VideoDecCount;
}AviPrivateMember;
#endif

