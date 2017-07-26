#ifndef MOVCOMMON_H_
#define MOVCOMMON_H_

#define MODE_MP4 0
#define MODE_MOV 1
#define MODE_3GP 2
#define MODE_PSP 3 // example working PSP command line: 
// ffmpeg -i testinput.avi  -f psp -r 14.985 -s 320x240 -b 768 -ar 24000 -ab 32 M4V00001.MP4
#define MODE_3G2 4
#define MAX_STREAMS 2
#define FFMAX(a,b) ((a) > (b) ? (a) : (b))
#define FFMIN(a,b) ((a) > (b) ? (b) : (a))
#define MKTAG(a,b,c,d) (a | (b << 8) | (c << 16) | (d << 24))

enum CodecType {
	CODEC_TYPE_UNKNOWN = -1,
	CODEC_TYPE_VIDEO,
	CODEC_TYPE_AUDIO,
	CODEC_TYPE_DATA,
	CODEC_TYPE_SUBTITLE,
};

typedef struct _M_MPEGLAYER3WAVEFORMAT
{
	unsigned short  wFormatTag;      
	unsigned short  nChannels;       
	unsigned int    nSamplesPerSec;  
	unsigned int    nAvgBytesPerSec; 
	unsigned short  nBlockAlign;     
	unsigned short  wBitsPerSample;
	unsigned short  cbsize;
	unsigned char   *reserve;
}M_MPEGLAYER3WAVEFORMAT;

typedef struct _M_BITMAPINFOHEADER {
	unsigned int  biSize;
	long biWidth;
	long biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned int biCompression;
	unsigned int biSizeImage;
	long biXPelsPerMeter;
	long biYPelsPerMeter;
	unsigned int biClrUsed;
	unsigned int biClrImportant;
} M_BITMAPINFOHEADER;

typedef struct _Stream_Info{
	unsigned char *buffer;
	unsigned int dwFlags;
	unsigned int dwSize;      // size of the data (excluding riff header size)
}Stream_Info;


/**
 * Rational number num/den.
 */
typedef struct AVRational{
    int num; ///< numerator
    int den; ///< denominator
} AVRational;

typedef struct _CodecTag {
    int id;
    unsigned int tag;
    unsigned int invalid_asf : 1;
} CodecTag;

typedef struct tag_con_get_para{
	int fb;
	unsigned long start_index;
	
	unsigned char *buf_video;
	unsigned long vbuf_len;
	unsigned long vbuf_used_len;
	int vfrm_count;
	
	unsigned char *buf_audio;
	unsigned long abuf_len;
	unsigned long abuf_used_len;
	int afrm_count;
	
	int last_vindex;
	int last_aindex;
}CON_GET_PARA,*PCON_GET_PARA;

#endif
