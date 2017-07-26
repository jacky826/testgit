#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <unistd.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ui_avi_io.h"
#include "all_data.h"
#include "pvr_error.h"
#include "ui_fileio_tools.h"
#include "config.h"
#include "../../pvr_debug.h"


TOTAL_VA_HEADER m_aviheader;		//文件头的信息
M_RIFFLIST m_listmovi;				//数据段的信息
AVIOLDINDEX m_rindx;				//文件为索引的信息

static unsigned long long movi_end;			//movi区的末尾位置，用于寻找idx1区
static unsigned long long movi_start;
static int avitailfile = -1;
static unsigned int indexnum = 0;		//index 结点的总数


static int m_bhave_audio = 0;				//是否包括声音码流
static int m_video_count = 0;
static int m_audio_frm_count = 0;
static int m_audio_frm_len = 0;
//unsigned long long m_video_last_time = 0;


static void Avi_Fill_Mainheader(M_BITMAPINFOHEADER *pbmp,M_MPEGLAYER3WAVEFORMAT *pwave,unsigned long video_time);
static void Avi_Fill_Videoinfo(M_BITMAPINFOHEADER *pbmp,unsigned long video_time);
static void Avi_Fill_Audioinfo(M_MPEGLAYER3WAVEFORMAT *pwave,unsigned long video_time);
static int Avi_Flush_Data(int avifp);
static int Avi_Get_Riff(M_RIFFLIST *avi, int pb);
static int Avi_Get_Index(int pb);
#if 0
static aIndex* Avi_Get_Idx1(int pb,unsigned int size);
#else
static int Avi_Get_Idx1(int pb,unsigned int size);
#endif


#define USE_INDEX_EN_BUFFER

#ifdef USE_INDEX_EN_BUFFER
#define AVI_HD_WR_INDEX_COUNT 1024
static aIndex *m_index_array=NULL;
static int cur_index;

int avi_en_add_index(aIndex *pin)
{
	int ret;
	if(m_index_array==NULL)
		return -1;
	if(cur_index>=AVI_HD_WR_INDEX_COUNT) {
		ret=avi_flush_en_index();
		if(ret<0)
			return ret;
	}
	memcpy(&m_index_array[cur_index++],pin,sizeof(aIndex));
	return 1;
}
int avi_flush_en_index()
{
	DWORD WriteBytes;
	if(avitailfile!=-1) {
		if(cur_index>0) {
			WriteBytes = sizeof(aIndex)*cur_index;
			if(write(avitailfile,m_index_array,WriteBytes)!=WriteBytes) {
				LOGE("%s:write failed\n",__func__);
				return ERR_WRITEFILE;
			}
			fdatasync(avitailfile);
			cur_index=0;
		}
		return 1;	
	}
	return -1;
}
#endif

#define USE_CLUSTER_WRITE

#ifdef USE_CLUSTER_WRITE
#define AVI_CLUSTER_WRITE_LEN 0x100000
static unsigned char *cluster_buf=NULL;
static int cur_w_ptr;

int avi_cluster_write(int hf,unsigned char *pbuf,int len)
{
	int ret;
	int cplen;
	if(cluster_buf==NULL)
		return -1;
	if(cur_w_ptr>=AVI_CLUSTER_WRITE_LEN) {
		ret=avi_cluster_flush(hf);
		if(ret<0)
			return ret;
	}
//	LOGT("%s: len = %d cur_w_ptr=0x%x\n",__func__,len,cur_w_ptr);
	if(cur_w_ptr+len>AVI_CLUSTER_WRITE_LEN) {
		cplen=AVI_CLUSTER_WRITE_LEN-cur_w_ptr;
		memcpy(cluster_buf+cur_w_ptr,pbuf,cplen);
		cur_w_ptr+=cplen;
		ret=avi_cluster_flush(hf);
		if(ret<0)
			return ret;
		pbuf+=cplen;
		len-=cplen;
	}
	if(len>0) {
		memcpy(cluster_buf+cur_w_ptr,pbuf,len);
		cur_w_ptr+=len;
	}
	return 1;
}

extern int DoParseMUSBStatus(void);
int avi_cluster_flush(int hf)
{
	LOGT("%s:hf =%d\n",__func__,hf);
	if(hf!=-1) {
		if(cur_w_ptr>0) {
			// check /proc/interrupts
			if(write(hf,cluster_buf,cur_w_ptr)!=cur_w_ptr) {
				LOGE("%s write failed\n",__func__);
				return ERR_WRITEFILE;
			}
			fdatasync(hf);
			cur_w_ptr=0;
//			LOGT("check usb interrupts\r\n");
			DoParseMUSBStatus();
		}
		return 1;
	}
	return -1;
}
#endif

int Avi_Initial_Header(unsigned short cbsize)
{
	indexnum = 0;
	m_video_count = 0;
    m_audio_frm_count = 0;
    m_audio_frm_len = 0;
	memset(&m_listmovi,0,sizeof(m_listmovi));
	memset(&m_rindx,0,sizeof(m_rindx));

#ifdef SUPPORT_AUDIO_ENC
    if(cbsize!=0) {
    	m_bhave_audio = 1;
    }else
        m_bhave_audio = 0;
#endif
	avitailfile = open(AVI_SYNC_TMP_FILE,O_RDWR|O_CREAT|O_TRUNC, 0666);
	if(avitailfile<0)
	{
		LOGE("%s: open file failed\n",__func__);
		return ERR_OPENFILE;
	}
	m_aviheader.aviriff.fcc=FCC('RIFF');	
	m_aviheader.aviriff.fccListType=FCC('AVI ');

	m_aviheader.list_hdrl.fcc=FCC('LIST');
	m_aviheader.list_hdrl.fccListType=FCC('hdrl');
	if(m_bhave_audio) {
		m_aviheader.list_hdrl.cb=sizeof(TOTAL_VA_HEADER)-sizeof(M_RIFFLIST)-8;
		if(cbsize >0)
			m_aviheader.list_hdrl.cb = m_aviheader.list_hdrl.cb -8+2+cbsize;
	}
	else
		m_aviheader.list_hdrl.cb=sizeof(TOTAL_VA_HEADER)-sizeof(M_RIFFLIST)-8-sizeof(m_aviheader.u);
		
	m_aviheader.list_videostrl.fcc=FCC('LIST');
	m_aviheader.list_videostrl.fccListType=FCC('strl');
	m_aviheader.list_videostrl.cb=sizeof(M_RIFFLIST)-8+sizeof(M_AVISTREAMHEADER)+sizeof(M_RIFFCHUNK)+sizeof(M_BITMAPINFOHEADER);
		
	m_aviheader.chunk_vstrf.fcc=FCC('strf');
	m_aviheader.chunk_vstrf.cb=sizeof(M_BITMAPINFOHEADER);
	
	if(m_bhave_audio) {
		m_aviheader.u.a_info.list_audiostrl.fcc=FCC('LIST');
		m_aviheader.u.a_info.list_audiostrl.fccListType=FCC('strl');
		m_aviheader.u.a_info.list_audiostrl.cb=sizeof(m_aviheader.u)-8;
		if(cbsize >0)
			m_aviheader.u.a_info.list_audiostrl.cb = m_aviheader.u.a_info.list_audiostrl.cb-8+2+cbsize;
		m_aviheader.u.a_info.chunk_astrf.fcc=FCC('strf');
		m_aviheader.u.a_info.chunk_astrf.cb=sizeof(M_MPEGLAYER3WAVEFORMAT);
		if(cbsize >0)
			m_aviheader.u.a_info.chunk_astrf.cb = m_aviheader.u.a_info.chunk_astrf.cb-8+2+cbsize;
	}
	if(cbsize > 0)
		m_aviheader.u.a_info.astream_format.cbsize = cbsize;
	else
		m_aviheader.u.a_info.astream_format.cbsize = 0;
#ifdef USE_INDEX_EN_BUFFER
	cur_index=0;
	m_index_array=(aIndex *)malloc(sizeof(aIndex)*AVI_HD_WR_INDEX_COUNT);
	if(m_index_array==NULL) {
		LOGE("%s:malloc failed\r\n",__func__);
		request2reboot();
		return ERR_MALLOC;
	}	
#endif

#ifdef USE_CLUSTER_WRITE
	cur_w_ptr=0;
	cluster_buf=(unsigned char *)malloc(AVI_CLUSTER_WRITE_LEN);
	if(cluster_buf==NULL) {
		LOGE("%s:malloc failed\r\n",__func__);
		request2reboot();
		return ERR_MALLOC;
	}	
#endif
	LOGT("%s success\n",__func__);
	return 0;
}


static void Avi_Fill_Mainheader(M_BITMAPINFOHEADER *pbmp,M_MPEGLAYER3WAVEFORMAT *pwave,unsigned long video_time)
{
	M_AVIMAINHEADER *pmain=&m_aviheader.mheader;
   	double temp1,temp2,temp3;
    
	pmain->fcc=FCC('avih');
	pmain->cb=sizeof(M_AVIMAINHEADER)-8;	
//	if(m_video_count!=0) {
//		if(!m_bhave_audio) 
//			pmain->dwMicroSecPerFrame=(unsigned int)((m_video_last_time/10.0)/m_video_count);
//		else
//			pmain->dwMicroSecPerFrame=(unsigned int)(m_audio_time/(m_video_count*10));
//	}else
	pmain->dwMicroSecPerFrame=40000;
	pmain->dwMaxBytesPerSec=0;
	pmain->dwReserved1=0;
	pmain->dwFlags=AVIF_HASINDEX|AVIF_ISINTERLEAVED;
	pmain->dwTotalFrames=m_video_count;
	pmain->dwInitialFrames=0;
	if(m_bhave_audio)
		pmain->dwStreams=2;
	else
		pmain->dwStreams=1;
	pmain->dwSuggestedBufferSize=0;
//	pmain->dwWidth=pbmp->biWidth;
//	pmain->dwHeight=pbmp->biHeight>0?pbmp->biHeight:-pbmp->biHeight;
	pmain->dwWidth = pbmp->biWidth;
	pmain->dwHeight = pbmp->biHeight;
	pmain->dwReserved[0]=FCC('efpp');
	if(m_video_count!=0){
		temp1 = (double)video_time/1000;
		temp2 = (double)m_video_count/(double)video_time;
		temp3 = temp1/temp2; 
        pmain->dwReserved[1] =(unsigned int)temp3;       
	}

    LOGT("video frm count=%d,audio frm count=%d,audio frm len=0x%x\n",m_video_count,m_audio_frm_count,m_audio_frm_len);
}


static void Avi_Fill_Videoinfo(M_BITMAPINFOHEADER *pbmp,unsigned long video_time)
{
	double temp1,temp2,temp3;
	m_aviheader.vstream_header.fcc=FCC('strh');
	m_aviheader.vstream_header.cb=sizeof(M_AVISTREAMHEADER)-8;	
	m_aviheader.vstream_header.fccType=FCC('vids');
	m_aviheader.vstream_header.fccHandler=pbmp->biCompression;
	m_aviheader.vstream_header.dwFlags=0;
	m_aviheader.vstream_header.wPriority=0;
	m_aviheader.vstream_header.wLanguage=0;
	m_aviheader.vstream_header.dwInitialFrames=1;
	if(m_video_count!=0){
//        if(m_bhave_audio) {
            temp3 = (double)video_time/39.0;
//        }else {
    		temp1 = (double)video_time/1000;
    		temp2 = (double)m_video_count/(double)video_time;
    		temp3 = temp1/temp2;
//        }    
		m_aviheader.vstream_header.dwScale =(unsigned int)temp3;	
	}
	else
		m_aviheader.vstream_header.dwScale=40000;//random number	
	m_aviheader.vstream_header.dwRate=video_time;       // dwRate/dwScale is stream tick rate in ticks/sec	
	m_aviheader.vstream_header.dwStart=0;
	m_aviheader.vstream_header.dwLength=m_video_count;
	m_aviheader.vstream_header.dwSuggestedBufferSize=0;
	m_aviheader.vstream_header.dwQuality=0;
	m_aviheader.vstream_header.dwSampleSize=0;
	m_aviheader.vstream_header.rcFrame.left=0;
	m_aviheader.vstream_header.rcFrame.top=0;
	m_aviheader.vstream_header.rcFrame.right=m_aviheader.mheader.dwWidth;
	m_aviheader.vstream_header.rcFrame.bottom=m_aviheader.mheader.dwHeight;

	m_aviheader.vstream_format.biSize=sizeof(M_BITMAPINFOHEADER); 
	m_aviheader.vstream_format.biWidth=m_aviheader.mheader.dwWidth; 
	m_aviheader.vstream_format.biHeight=m_aviheader.mheader.dwHeight; 
	m_aviheader.vstream_format.biPlanes=1; 
	m_aviheader.vstream_format.biBitCount=24; 
	m_aviheader.vstream_format.biCompression=pbmp->biCompression; 
	m_aviheader.vstream_format.biSizeImage=m_aviheader.mheader.dwWidth*m_aviheader.mheader.dwHeight*3; 
	m_aviheader.vstream_format.biXPelsPerMeter=0; 
	m_aviheader.vstream_format.biYPelsPerMeter=0; 
	m_aviheader.vstream_format.biClrUsed=0; 
	m_aviheader.vstream_format.biClrImportant=0; 
}

static void Avi_Fill_Audioinfo(M_MPEGLAYER3WAVEFORMAT *pwave,unsigned long video_time)
{
	if(m_bhave_audio) {
        double temp;
		m_aviheader.u.a_info.astream_header.fcc=FCC('strh');
		m_aviheader.u.a_info.astream_header.cb=sizeof(M_AVISTREAMHEADER)-8;
		m_aviheader.u.a_info.astream_header.fccType=FCC('auds');
		m_aviheader.u.a_info.astream_header.fccHandler=pwave->wFormatTag;
		m_aviheader.u.a_info.astream_header.dwFlags=0;
		m_aviheader.u.a_info.astream_header.wPriority=0;
		m_aviheader.u.a_info.astream_header.wLanguage=0;
		m_aviheader.u.a_info.astream_header.dwInitialFrames=1;
		m_aviheader.u.a_info.astream_header.dwScale=1;//1152;
		if(video_time!=0) {
            temp = (1000.0*m_audio_frm_len)/video_time;
            m_aviheader.u.a_info.astream_header.dwRate=(int)temp;    
        }else {
            temp = 0x900/4;
            temp = (16000*0x1b0)/temp;
		    m_aviheader.u.a_info.astream_header.dwRate=temp;       // dwRate/dwScale is stream tick rate in ticks/sec
        }
		m_aviheader.u.a_info.astream_header.dwStart=0;
		m_aviheader.u.a_info.astream_header.dwLength=m_audio_frm_len;
		m_aviheader.u.a_info.astream_header.dwSuggestedBufferSize=0x300;
		m_aviheader.u.a_info.astream_header.dwQuality=0xffffffff;
		m_aviheader.u.a_info.astream_header.dwSampleSize=1;
		m_aviheader.u.a_info.astream_header.rcFrame.left=0;
		m_aviheader.u.a_info.astream_header.rcFrame.top=0;
		m_aviheader.u.a_info.astream_header.rcFrame.right=0;
		m_aviheader.u.a_info.astream_header.rcFrame.bottom=0;

		m_aviheader.u.a_info.astream_format=*pwave; 
	}else{
		m_aviheader.u.chunk_audio.fcc = FCC('JUNK');
		m_aviheader.u.chunk_audio.cb = 0;
	}
}

int  Avi_Before_AddData(int avifp)
{	
	unsigned char * temphead;
	int wrpos=0;
	int ret;
	wrpos += sizeof(M_RIFFLIST);
	wrpos += sizeof(M_RIFFLIST);
	wrpos += sizeof(M_AVIMAINHEADER);
	wrpos += sizeof(M_RIFFLIST);
	wrpos += sizeof(M_AVISTREAMHEADER);	
	wrpos += sizeof(M_RIFFCHUNK);		
	wrpos += sizeof(M_BITMAPINFOHEADER);	
	if(m_bhave_audio)
	{
		wrpos += (sizeof(m_aviheader.u)-8);
		if(m_aviheader.u.a_info.astream_format.cbsize!=0)
			wrpos+=(m_aviheader.u.a_info.astream_format.cbsize+2);		
	}
	m_listmovi.fcc=FCC('LIST');
	m_listmovi.fccListType=FCC('movi');	
	m_listmovi.cb=4;	
	wrpos += sizeof(M_RIFFLIST);
	temphead = malloc(wrpos);
	if(temphead == NULL) {
		LOGE("%s:malloc failed\r\n",__func__);
		request2reboot();
		return ERR_MALLOC;
	}	
#ifndef USE_CLUSTER_WRITE	
	if(write(avifp,temphead,wrpos)!=wrpos) {
		ret=ERR_WRITEFILE;
	}else{
		ret=0;
	}
#else
	ret=avi_cluster_write(avifp,(unsigned char *)temphead,wrpos);
#endif

	free(temphead);
	return 0;
}
#ifndef USE_INDEX_EN_BUFFER
int Avi_Add_Aframe(int ft,const Stream_Info info)
{
	unsigned int writeerr;
	M_RIFFCHUNK audiochunk;
	unsigned char alian = 0;
	aIndex node;
	unsigned long WriteBytes;
	audiochunk.cb = info.dwSize;
	audiochunk.fcc = FCC('01wb');

	if(write(ft,&audiochunk,sizeof(M_RIFFCHUNK))!=sizeof(M_RIFFCHUNK)) {
		return ERR_WRITEFILE;
	}
	node.dwChunkId = FCC('01wb');
	node.dwFlags =  info.dwFlags;
	node.dwOffset = m_listmovi.cb;
	node.dwSize = info.dwSize;
	if(write(avitailfile,&node,16)!=16) {
		return ERR_WRITEFILE;
	}	
	indexnum++;
    m_audio_frm_count++;
    m_audio_frm_len+=info.dwSize;
    
	if(write(ft,info.buffer,info.dwSize)!=info.deSize) {
		return ERR_WRITEFILE;
	}	
	if(info.dwSize&0x1){
		m_listmovi.cb++;
		if(write(ft,&alian,1)!=1) {
			return ERR_WRITEFILE;
		}	
	}

	m_listmovi.cb+= (info.dwSize+8);
	return 0;
}

int Avi_Add_Vframe(int ft,const Stream_Info info)
{
	int err;
	M_RIFFCHUNK videochunk;
	unsigned char alian = 0;
	aIndex node;
	unsigned long WriteBytes;
	
	videochunk.cb = info.dwSize;
	videochunk.fcc = FCC('00dc');
	err = write(ft,&videochunk,sizeof(M_RIFFCHUNK));
	if(err != sizeof(M_RIFFCHUNK))
		return -1;
	node.dwChunkId = FCC('00dc');
	node.dwFlags =  info.dwFlags;
	node.dwOffset = m_listmovi.cb;
	node.dwSize = info.dwSize;
	write(avitailfile,&node,16);
	indexnum++;	
	m_video_count++;
	err = write(ft,info.buffer,info.dwSize);
	if(err != info.dwSize)
		return -1;
	if(info.dwSize&0x1){
		m_listmovi.cb++;
		err = write(ft,&alian,1);
	}
	m_listmovi.cb+= (info.dwSize+8);
	return 0;
}
#else
int Avi_Add_Aframe(int ft,const Stream_Info *pinfo)
{
	M_RIFFCHUNK audiochunk;
	aIndex node;
	int ret,wlen;

	audiochunk.cb = pinfo->dwSize;
	audiochunk.fcc = FCC('01wb');
	wlen=pinfo->dwSize&0x1?(pinfo->dwSize+1):pinfo->dwSize;
#ifndef USE_CLUSTER_WRITE
	if(write(ft,&audiochunk,sizeof(audiochunk))!=sizeof(M_RIFFCHUNK)) {
		return ERR_WRITEFILE;
	}
	if(write(ft,pinfo->buffer,wlen)!=wlen) {
		return ERR_WRITEFILE;
	}    
#else
	if((ret=avi_cluster_write(ft,(unsigned char *)&audiochunk,sizeof(M_RIFFCHUNK)))<0) {
		return ret;
	}
	if((ret=avi_cluster_write(ft,(unsigned char *)pinfo->buffer,wlen))<0) {
		return ret;
	}    
#endif
	node.dwChunkId = FCC('01wb');
	node.dwFlags =  pinfo->dwFlags;
	node.dwOffset = m_listmovi.cb;
	node.dwSize = pinfo->dwSize;
	
	m_listmovi.cb+=(wlen+8);
	
	ret=avi_en_add_index(&node);
	if(ret<0) {
        LOGE("%s: avi_en_add_index return %d\n",__func__,ret);
		return ret;
    }    

	indexnum++;
    m_audio_frm_count++;
    m_audio_frm_len+=pinfo->dwSize;
	return 0;
}

int Avi_Add_Vframe(int ft,const Stream_Info *pinfo)
{
	M_RIFFCHUNK videochunk;
	aIndex node;
	int ret,wlen;	

//	FUNC_ENTER
		
	videochunk.cb = pinfo->dwSize;
	videochunk.fcc = FCC('00dc');
	wlen=pinfo->dwSize&0x1?(pinfo->dwSize+1):pinfo->dwSize;
#ifndef USE_CLUSTER_WRITE	
	if(write(ft,&videochunk,sizeof(videochunk))!=sizeof(videochunk))
		return ERR_WRITEFILE;
	if(write(ft,pinfo->buffer,wlen)!=wlen) {
		return ERR_WRITEFILE;
	}
#else
	if((ret=avi_cluster_write(ft,(unsigned char *)&videochunk,sizeof(M_RIFFCHUNK)))<0) {
		return ret;
	}
	if((ret=avi_cluster_write(ft,(unsigned char *)pinfo->buffer,wlen))<0) {
		return ret;
	}
#endif
	
	node.dwChunkId = FCC('00dc');
	node.dwFlags =  pinfo->dwFlags;
	node.dwOffset = m_listmovi.cb;
	node.dwSize = pinfo->dwSize;
	
	m_listmovi.cb+=(wlen+8);
	ret=avi_en_add_index(&node);
	if(ret<0) {
		LOGE("%s: avi_en_add_index return %d\n",__func__,ret);
		return ret;
	}	
	
	indexnum++;	
	m_video_count++;
	return 0;
}
#endif

static int Avi_Flush_Data(int avifp)
{
#define FLUSH_BUF_LEN 0x20000
	
	unsigned long wrpos = 0;
	int err;
	off_t err_off;	
	int ReadBytes;//SACH:W10877
	unsigned long returnpt;
	unsigned char* tempbuffer=NULL;
	unsigned char *buffer=NULL;

	err=1;
	tempbuffer= (unsigned char* )malloc(FLUSH_BUF_LEN);
	buffer = (unsigned char* )malloc(4096);

	if(tempbuffer==NULL||buffer==NULL) {
		LOGE("%s:malloc failed\r\n",__func__);
		request2reboot();
		err=ERR_MALLOC;
		goto out;
	}

#ifdef USE_CLUSTER_WRITE
	if((err=avi_cluster_flush(avifp))<0) {
		LOGE("%s error 1\n",__func__);
		goto out;
	}	
#endif

	returnpt = 0;
	err_off = lseek(avifp,returnpt,SEEK_SET);
	if(err_off != 0)
	{	
		err=ERR_SEEKFILE;
		LOGE("%s error 1\n",__func__);
		goto out;
	}
	m_rindx.fcc = FCC('idx1');
	m_rindx.cb = indexnum*16;
	if(m_bhave_audio)
	{
		m_aviheader.aviriff.cb=(sizeof(TOTAL_VA_HEADER)-8) //header part 
		+(m_listmovi.cb+8) //movie part
		+(m_rindx.cb+8); //index part
	} else {
		m_aviheader.aviriff.cb = (sizeof(TOTAL_VA_HEADER)-8) //header part 
		+(m_listmovi.cb+8) //movie part
		+(m_rindx.cb+8) //index part
		- sizeof(M_RIFFLIST)
		- sizeof(M_AVISTREAMHEADER) //chunk embedded	64bits
		- sizeof(M_MPEGLAYER3WAVEFORMAT)
		- 8;//4 //16bytes 
	}
	//FIXED 
	if(m_aviheader.u.a_info.astream_format.cbsize!=0) {	//extradata
		m_aviheader.aviriff.cb = m_aviheader.aviriff.cb-8+m_aviheader.u.a_info.astream_format.cbsize+2;
		m_aviheader.u.a_info.chunk_astrf.cb=sizeof(M_MPEGLAYER3WAVEFORMAT)-8+m_aviheader.u.a_info.astream_format.cbsize+2;
	} else {
		m_aviheader.aviriff.cb = m_aviheader.aviriff.cb-8;	
		m_aviheader.u.a_info.chunk_astrf.cb=sizeof(M_MPEGLAYER3WAVEFORMAT)-8;
	}
	
	memcpy(buffer,&m_aviheader.aviriff,sizeof(M_RIFFLIST));
	wrpos += sizeof(M_RIFFLIST);
	memcpy(&buffer[wrpos],&m_aviheader.list_hdrl,sizeof(M_RIFFLIST));
	wrpos += sizeof(M_RIFFLIST);
	memcpy(&buffer[wrpos],&m_aviheader.mheader,sizeof(M_AVIMAINHEADER));
	wrpos += sizeof(M_AVIMAINHEADER);
	memcpy(&buffer[wrpos],&m_aviheader.list_videostrl,sizeof(M_RIFFLIST));
	wrpos += sizeof(M_RIFFLIST);
	memcpy(&buffer[wrpos],&m_aviheader.vstream_header,sizeof(M_AVISTREAMHEADER));
	wrpos += sizeof(M_AVISTREAMHEADER);	
	memcpy(&buffer[wrpos],&m_aviheader.chunk_vstrf,sizeof(M_RIFFCHUNK));
	wrpos += sizeof(M_RIFFCHUNK);		
	memcpy(&buffer[wrpos],&m_aviheader.vstream_format,sizeof(M_BITMAPINFOHEADER));
	wrpos += sizeof(M_BITMAPINFOHEADER);	
	if(m_bhave_audio)
	{
		memcpy(&buffer[wrpos],&m_aviheader.u,(sizeof(m_aviheader.u)-8));
		wrpos += (sizeof(m_aviheader.u)-8);	
		if(m_aviheader.u.a_info.astream_format.cbsize!=0) {
			memcpy(&buffer[wrpos],&m_aviheader.u.a_info.astream_format.cbsize,sizeof(short));
			wrpos += sizeof(short);
			memcpy(&buffer[wrpos],m_aviheader.u.a_info.astream_format.reserve ,m_aviheader.u.a_info.astream_format.cbsize);
			wrpos += m_aviheader.u.a_info.astream_format.cbsize;
		}
	}
//	else
//	{
//		memcpy(&buffer[wrpos],&m_aviheader.u,sizeof(M_RIFFCHUNK));
//		wrpos += sizeof(M_RIFFCHUNK);	
//	}
//	printf("%d\n",wrpos);
	//fixed 


	err = write(avifp,buffer,wrpos);//write file header
	if(err != wrpos){
		err=ERR_WRITEFILE;
		LOGE("%s error 1\n",__func__);		
		goto out;
	}

	err = write(avifp,&m_listmovi,12);//write "movi" chunk
	if(err != 12) {
		err=ERR_WRITEFILE;
		LOGE("%s error 2\n",__func__);		
		goto out;
	}
	returnpt = 0;
	err_off = lseek(avifp,returnpt,SEEK_END);
	if(err_off == (off_t)(-1)) {
		err=ERR_SEEKFILE;
		LOGE("%s error 3\n",__func__);		
		goto out;
	}
	err = write(avifp,&m_rindx,sizeof(M_RIFFCHUNK));
	if(err != sizeof(M_RIFFCHUNK)){
		err=ERR_WRITEFILE;
		LOGE("%s error 4\n",__func__);		
		goto out; 
	}

//	CloseFile(avitailfile);
//	avitailfile= NULL;
//	avitailfile = OpenFile("c:\\tail.dat",NULL,O_RDONLY,FILE_ATTRIBUTE_NORMAL);
//	if(avitailfile==NULL)
//	{
//		free(buffer);
//		free(tempbuffer);
//		return -1;
//	}

#ifdef USE_INDEX_EN_BUFFER
	if((err=avi_flush_en_index())<0) {
		LOGE("%s error 5\n",__func__);		
		goto out;
	}
#endif
	wrpos = 0; 
	lseek(avitailfile,0,SEEK_SET);
	while((ReadBytes=read(avitailfile,tempbuffer,FLUSH_BUF_LEN))>0)
	{
		if(write(avifp,tempbuffer,ReadBytes)!=ReadBytes) {
			LOGE("%s error 6\n",__func__);			
			err=ERR_WRITEFILE;	
			goto out;
		}
	}
out:

#ifdef USE_INDEX_EN_BUFFER
	if(m_index_array!=NULL) {
		free(m_index_array);
		m_index_array=0;
	}
	cur_index=0;
#endif

#ifdef USE_CLUSTER_WRITE
	if(cluster_buf!=NULL) {
		free(cluster_buf);
		cluster_buf=NULL;
	}	
	cur_w_ptr=0;	
#endif
	if(avitailfile!=-1) {
		close(avitailfile);
		avitailfile=-1;
	}	
	unlink(AVI_SYNC_TMP_FILE);
	if(tempbuffer!=NULL)
		free(tempbuffer);	
	if(buffer!=NULL)
		free(buffer);
	return err;
}

int Avi_FlushTailer(int aviwritefile,M_BITMAPINFOHEADER *pbmp,M_MPEGLAYER3WAVEFORMAT *pwave,unsigned long video_time)
{
	int err;
	Avi_Fill_Mainheader(pbmp,pwave,video_time);
	if(m_bhave_audio)
		Avi_Fill_Audioinfo(pwave,video_time);
	Avi_Fill_Videoinfo(pbmp,video_time);
	err = Avi_Flush_Data(aviwritefile);	
	fdatasync(aviwritefile);
	return err;
}

#define AVI_01WB 0x62773130 //audio mark
#define AVI_00DC 0x63643030 //video mark

unsigned int Avi_GetFilePlayTime(void)
{
	float fplaytime;
	unsigned int playtime;
	fplaytime = (float)m_aviheader.vstream_header.dwScale/ (float)m_aviheader.vstream_header.dwRate*m_aviheader.vstream_header.dwLength;
	playtime = (unsigned int) (fplaytime*1000);
	return playtime;
}
static int Avi_Get_Riff(M_RIFFLIST *avi,int pb)
{
	unsigned int tag; 
	off_t file_size;

	file_size=lseek(pb,0,SEEK_END);
	if(file_size==(off_t)-1) {
		return -1;
	}
	lseek(pb,0, SEEK_SET);
	/* check RIFF header */
	tag = get_le32(pb);
	if (tag != FCC('RIFF'))
		return -1;
	avi->fcc = tag;	
	tag = get_le32(pb);
	avi->cb = tag;
	tag = get_le32(pb);
	if (tag != FCC('AVI ') && tag != FCC('AVIX'))
		return -1;	
/*
       according to avi specification,file_size=avi->cb+8.
       in legacy pvr edition(no audio), there is bug in calculating the cb which will produce the result file_size=avi->cb+16
       To keep compatible with legacy version,check here is a little tricky.
*/
	if(file_size!=(avi->cb+16)&&file_size!=(avi->cb+8)) {//(file_size<avi->cb) {
		LOGE("%s:unfinished file\n",__func__);
		return -1;
	}
	avi->fccListType = tag;
	return 0;
}
int Avi_Get_Header(int pb,int getindex)
{
	unsigned int tag, tag1, handler;
	int codec_type;
	unsigned int size;
	unsigned long returnpt;
	M_AVISTREAMHEADER stream_header; //chunk embedded	68bits
	M_RIFFLIST	list_strl;
	off_t cur_fpos;
	if (Avi_Get_Riff(&m_aviheader.aviriff, pb) < 0)
		return ERR_UNKNOWN_FILEFORAMT;

#ifdef SUPPORT_AUDIO_ENC
	m_bhave_audio = 0; //recover this variable to its default state to support legacy dumb file
#endif
	/* first list tag */

	for(;;) {
		tag = get_le32(pb);
		size = get_le32(pb);
		if(size == 0) {
			LOGE("%s:zero size tag\n",__func__);
			return ERR_NOT_COMPLETE_FILE;
		}		
		switch(tag) {
		case FCC('LIST'):
			/* ignored, except when start of video packets */
			tag1 = get_le32(pb);
			switch(tag1) {
			case FCC('movi'):
				m_listmovi.fcc = tag;
				m_listmovi.cb = size;
				m_listmovi.fccListType = tag1;
				cur_fpos= lseek(pb,0,SEEK_CUR);
				movi_start = cur_fpos - 4;
				if(size) 
					movi_end = cur_fpos + size - 4;
//				else    //SACH:W10857
//					movi_end = cur_fpos;				
				goto end_of_header;
			case FCC('hdrl'):
				m_aviheader.list_hdrl.fcc = tag;
				m_aviheader.list_hdrl.cb = size;
				m_aviheader.list_hdrl.fccListType = tag1;
				break;
			case FCC('strl'):
				list_strl.fcc = tag;
				list_strl.cb = size;
				list_strl.fccListType =tag1;
				break;
			case FCC('odml'):
				if(size){
					returnpt = size-4;
					if(lseek(pb,returnpt,SEEK_CUR)==-1)//SACH:W10842
					{
						return -1;
					}
				}
				break;
			default:
				return -1;
			}			

			break;
		case FCC('dmlh'):
			returnpt = size + (size & 1);
			if(lseek(pb,returnpt,SEEK_CUR)==-1)//SACH:W10842
			{
				return -1;
			}			
			break;
		case FCC('avih'):
			/* avi header */
			/* using frame_period is bad idea */
			m_aviheader.mheader.fcc = tag;
			m_aviheader.mheader.cb = size;
			m_aviheader.mheader.dwMicroSecPerFrame = get_le32(pb);
			m_aviheader.mheader.dwMaxBytesPerSec = get_le32(pb);
			m_aviheader.mheader.dwReserved1 = get_le32(pb);
			m_aviheader.mheader.dwFlags = get_le32(pb);
			m_aviheader.mheader.dwTotalFrames = get_le32(pb);
			m_aviheader.mheader.dwInitialFrames = get_le32(pb);	
			m_aviheader.mheader.dwStreams = get_le32(pb);
			m_aviheader.mheader.dwSuggestedBufferSize = get_le32(pb);
			m_aviheader.mheader.dwWidth = get_le32(pb);
			m_aviheader.mheader.dwHeight = get_le32(pb);
			m_aviheader.mheader.dwReserved[0] = get_le32(pb);
			m_aviheader.mheader.dwReserved[1] = get_le32(pb);
			m_aviheader.mheader.dwReserved[2] = get_le32(pb);
			m_aviheader.mheader.dwReserved[3] = get_le32(pb);			
			break;
		case FCC('strh'):
			/* stream header */
			tag1 = get_le32(pb);
			handler = get_le32(pb); /* codec tag */
			if(tag1 == FCC('iavs') || tag1 == FCC('ivas')){
				/* 
				* After some consideration -- I don't think we 
				* have to support anything but DV in a type1 AVIs.
				*/
				if (handler != FCC('dvsd') && 
					handler != FCC('dvhd') &&
					handler != FCC('dvsl'))
					return -1;
				returnpt = size-8;
				if(lseek(pb,returnpt,SEEK_CUR)==-1)//SACH:W10842
				{
					return -1;
				}
				break;
			}
			stream_header.fcc = tag;
			stream_header.cb = size;
			stream_header.fccType = tag1;
			stream_header.fccHandler = handler;
			stream_header.dwFlags =  get_le32(pb); /* flags */
			stream_header.wPriority = get_le16(pb); /* priority */
			stream_header.wLanguage = get_le16(pb); /* priority */
			stream_header.dwInitialFrames = get_le32(pb); /* initial frame */
			stream_header.dwScale = get_le32(pb);
			stream_header.dwRate = get_le32(pb);
			stream_header.dwStart = get_le32(pb);
			stream_header.dwLength = get_le32(pb);
			stream_header.dwSuggestedBufferSize = get_le32(pb);
			stream_header.dwQuality = get_le32(pb);
			stream_header.dwSampleSize = get_le32(pb);
			stream_header.rcFrame.left = get_le16(pb);
			stream_header.rcFrame.top = get_le16(pb);
			stream_header.rcFrame.right = get_le16(pb);
			stream_header.rcFrame.bottom = get_le16(pb);
			if(stream_header.dwScale && stream_header.dwRate){
				//??empty statement
			}else if(m_aviheader.mheader.dwMicroSecPerFrame){
				stream_header.dwRate = 1000000;
				stream_header.dwScale = m_aviheader.mheader.dwMicroSecPerFrame;
			}else{
				stream_header.dwRate = 25;
				stream_header.dwScale = 1;
			}
			switch(tag1) {
			case FCC('vids'):
				codec_type = CODEC_TYPE_VIDEO;
				stream_header.dwSampleSize = 0;
				memcpy(&m_aviheader.list_videostrl,&list_strl,sizeof(M_RIFFLIST));
				break;
			case FCC('auds'):
				codec_type = CODEC_TYPE_AUDIO;
				memcpy(&m_aviheader.u.a_info.list_audiostrl,&list_strl,sizeof(M_RIFFLIST));
				break;
			case FCC('txts'):
				//FIXME 
				codec_type = CODEC_TYPE_DATA; //CODEC_TYPE_SUB ?  FIXME
				break;
			case FCC('pads'):
				codec_type = CODEC_TYPE_UNKNOWN;
				break;
			default:
				return -1;
			}
			break;
		case FCC('strf'):
			/* stream header */
			switch(codec_type) {
			case CODEC_TYPE_VIDEO:
				memcpy(&m_aviheader.vstream_header,&stream_header,sizeof(M_AVISTREAMHEADER));
				m_aviheader.chunk_vstrf.fcc = tag;
				m_aviheader.chunk_vstrf.cb = size;
				m_aviheader.vstream_format.biSize = get_le32(pb); /* size */
				m_aviheader.vstream_format.biWidth = get_le32(pb);
				m_aviheader.vstream_format.biHeight = get_le32(pb);
				m_aviheader.vstream_format.biPlanes  = get_le16(pb); /* panes */
				m_aviheader.vstream_format.biBitCount = get_le16(pb); /* depth */
				m_aviheader.vstream_format.biCompression = get_le32(pb);
				m_aviheader.vstream_format.biSizeImage = get_le32(pb); /* ImageSize */
				m_aviheader.vstream_format.biXPelsPerMeter = get_le32(pb); /* XPelsPerMeter */
				m_aviheader.vstream_format.biYPelsPerMeter = get_le32(pb); /* YPelsPerMeter */
				m_aviheader.vstream_format.biClrUsed = get_le32(pb); /* ClrUsed */
				m_aviheader.vstream_format.biClrImportant = get_le32(pb); /* ClrImportant */
				returnpt = size-40;
				if(lseek(pb,returnpt,SEEK_CUR)==-1)//SACH:W10842
				{
					return -1;
				}
				break;
			case CODEC_TYPE_AUDIO:
				memcpy(&m_aviheader.u.a_info.astream_header,&stream_header,sizeof(M_AVISTREAMHEADER));
				m_aviheader.u.a_info.chunk_astrf.fcc = tag;
				m_aviheader.u.a_info.chunk_astrf.cb = size;
				m_aviheader.u.a_info.astream_format.wFormatTag = get_le16(pb);
				m_aviheader.u.a_info.astream_format.nChannels = get_le16(pb);
				m_aviheader.u.a_info.astream_format.nSamplesPerSec = get_le32(pb);
 				m_aviheader.u.a_info.astream_format.nBlockAlign = get_le32(pb);
 				m_aviheader.u.a_info.astream_format.nAvgBytesPerSec = get_le16(pb);
 				m_aviheader.u.a_info.astream_format.wBitsPerSample = get_le16(pb);
				returnpt = size-16;
				if(lseek(pb,returnpt,SEEK_CUR)==-1)//SACH:W10842
				{
					return -1;
				}						
				m_bhave_audio  = 1;
				break;
			default:
				returnpt = size;	
				if(lseek(pb,returnpt,SEEK_CUR)==-1)//SACH:W10842
				{
					return -1;
				}
				break;
			}
			break;
		default:
			/* skip tag */
			size += (size & 1);
			returnpt = size;
			if(lseek(pb,returnpt,SEEK_CUR)==-1)//SACH:W10842
			{
				return -1;
			}
//			LOGT("%s:skip unknown tag 0x%x: 0x%x\n",__func__,tag,size);		
			break;
		}
	}
end_of_header:
	if(getindex)
		return Avi_Get_Index(pb);	
	return 0;
}
int Avi_HaveAudio(void)
{
#ifdef SUPPORT_AUDIO_ENC
	return m_bhave_audio;
#else
    return 0;
#endif
}
unsigned short Avi_Get_XDist(void)
{
	return  m_aviheader.mheader.dwWidth;
}

unsigned short Avi_Get_YDist(void)
{
	return  m_aviheader.mheader.dwHeight;
}
unsigned long Avi_Get_IndexCount(void)
{
	return (m_rindx.cb>>4);
}

double Avi_Get_FrameRate(void)
{
    if(m_bhave_audio&&m_aviheader.mheader.dwReserved[1]!=0)
        return ((double)m_aviheader.vstream_header.dwRate/(double)m_aviheader.mheader.dwReserved[1]);
    else
    	return ((double)m_aviheader.vstream_header.dwRate/(double)m_aviheader.vstream_header.dwScale);
}

int Avi_Get_ACodecMethod(void)
{
	switch(m_aviheader.u.a_info.astream_header.fccHandler)
	{
		case 0x55:
			return ENC_MP3;
		default:
			return -1;
	}
}

int Avi_Get_VCodecMethod(void)
{
	switch(m_aviheader.vstream_header.fccHandler)
	{
		case FCC('XVID'):
			return ENC_MPEG4;
		case FCC('H264'):
			return ENC_H264;
		default:
			return -1;
	}
}

//#define USE_PREFETCH

#ifdef USE_PREFETCH

static unsigned char *ppre_buf=NULL;
static unsigned long pre_offset;
static unsigned long pre_len;

static int bcan_prefetch=0;


#define PREFETCH_LEN 0x300000

int avi_tryprefetch(int hfile,DWORD offset,unsigned char *buffer,int len)
{
	DWORD sector_offset;
	DWORD real_read;
	if(bcan_prefetch!=1) 
		return -1;
	
	if(pre_len>0) {
		if((offset>=pre_offset)&&((offset+len)<=(pre_offset+pre_len))) { //hit
			memcpy(buffer,ppre_buf+offset-pre_offset,len);
			return len;
		}
	}
	sector_offset=offset&0xfffffe00;
	
	if(lseek(hfile,sector_offset,SEEK_SET)!=sector_offset)
		return ERR_SEEKFILE;	
	if((real_read=read(hfile,ppre_buf,PREFETCH_LEN))<0) {
		return ERR_READFILE;
	}else {
		pre_len=real_read;	
		pre_offset=sector_offset;
		if(offset-pre_offset+len<=pre_len) {
			memcpy(buffer,ppre_buf+offset-pre_offset,len);		
			return len;
		}else
			return -1;
	}
}
#endif

int Avi_prefetch(int hfile)
{
	return 1;
}

int Avi_prestart(int hfile)
{
#ifdef USE_PREFETCH
	bcan_prefetch=0;
	if(ppre_buf==NULL) {
		ppre_buf=(unsigned char *)malloc(PREFETCH_LEN);
	}
	if(ppre_buf!=NULL) {
		bcan_prefetch=1;
		pre_offset=0;
		pre_len=0;
		return 1;
	}
	return -1;
#else
	return 1;
#endif
}

int Avi_prestop(int hfile)
{
#ifdef USE_PREFETCH
	if(ppre_buf!=NULL) {
		free(ppre_buf);
		ppre_buf=NULL;
	}
	bcan_prefetch=0;
#endif	
	return 1;
}


#define BUFFERED_INDEX
#ifndef BUFFERED_INDEX

list_avioldindex_entry * tailentry = NULL;	//IDX1 区的双向链表的指针
aIndex* Idx1buf = NULL;//读索引时的索引buffer

int Avi_FreeData(void)
{
//	list_avioldindex_entry * tailentrytmp;	
	if(Idx1buf != NULL)
	{
		free(Idx1buf);
		Idx1buf = NULL;
	}	
/*	if(tailentry != NULL)
	{
		while(tailentry->next != NULL)
			tailentry = tailentry->next ;
	}else
	{
		return -1;
	}
	while(tailentry != NULL)
	{
		tailentrytmp = tailentry->priv;
		free(tailentry);
//		printf("free %p\n",tailentry);
		tailentry = tailentrytmp;
	}
*/

	return 0;
}

unsigned int Avi_GetVFrameCount(void)
{
	int i,count=0;
	for(i=0;i<(m_rindx.cb>>4);i++)
	{
		if(Idx1buf[i].dwChunkId == AVI_00DC)
			count++;
	}
	return count;
}

unsigned int Avi_GetAFrameCount(void)
{
	int i,count=0;
	for(i=0;i<(m_rindx.cb>>4);i++)
	{
		if(Idx1buf[i].dwChunkId == AVI_01WB)
			count++;
	}
	return count;
}

unsigned int Avi_GetKeyFrameCount(void)
{
	int i,count=0;
	for(i=0;i<(m_rindx.cb>>4);i++)
	{
		if((Idx1buf[i].dwFlags== 0x10) && (Idx1buf[i].dwChunkId== AVI_00DC))
			count++;
	}
	return count;
}

int Avi_GotoSpecialTime(unsigned int time)
{
	int frames,i,j;
	frames = (unsigned int)(time/1000*(float)m_aviheader.vstream_header.dwRate/ (float)m_aviheader.vstream_header.dwScale);
	for(i=0,j=0;(i<frames)&&(j<(m_rindx.cb>>4));j++)
	{
		if(Idx1buf[j].dwChunkId == AVI_00DC)
			i++;
	}	
	frames = Avi_Next_KeyVFrame(i);
	return frames;
}

int Avi_GotoNextClip(unsigned int alreadyframes,unsigned int inteval)
{
	int frames,i,j;
	frames = alreadyframes+(unsigned int)(inteval*(float)m_aviheader.vstream_header.dwRate/ (float)m_aviheader.vstream_header.dwScale);
	for(i=alreadyframes,j=alreadyframes;(i<frames)&&(j<(m_rindx.cb>>4));j++)
	{
		if(Idx1buf[j].dwChunkId == AVI_00DC)
			i++;
	}
	if(frames>(m_rindx.cb>>4))
		return alreadyframes;
	frames = Avi_Next_KeyVFrame(i);
	return frames;
}

int Avi_GotoPrevClip(unsigned int alreadyframes,unsigned int inteval)
{
	int frames,i,j;
	frames = alreadyframes-(unsigned int)(inteval*(float)m_aviheader.vstream_header.dwRate/ (float)m_aviheader.vstream_header.dwScale);
	for(i=alreadyframes,j=alreadyframes;(i>frames)&&(j>=0);j--)
	{
		if(Idx1buf[j].dwChunkId == AVI_00DC)
			i--;
	}
	if(j==-1)
		return 0;	
	frames = Avi_Next_KeyVFrame(i);
	return frames;
}	

static aIndex* Avi_Get_Idx1(int pb,unsigned int size)
{
	unsigned int readerr;
	unsigned long ReadBytes;
	Idx1buf = (aIndex*)malloc(size);
	if(Idx1buf == NULL) {
		LOGE("%s:malloc failed\r\n",__func__);
		request2reboot();
		return NULL;
	}	
	readerr = ReadFile(pb,Idx1buf,sizeof(aIndex)*size/16,&ReadBytes);
	if(readerr == 0)
	{
		free(Idx1buf);
		Idx1buf = NULL;
		return NULL;
	}
	return Idx1buf;
}

static int Avi_Get_Index(int pb)
{
	unsigned int tag,size;
	unsigned long returnpt;
	returnpt = movi_end;
	if(!SetFilePointer(pb,&returnpt,SEEK_SET))
		return ERR_SEEKFILE;		
	for(;;) {
		tag = get_le32(pb);
		size = get_le32(pb);
		if(size == 0)
			return -1;
#ifdef DEBUG_SEEK
	printf("tag=%c%c%c%c size=0x%x\n",tag & 0xff,(tag >> 8) & 0xff,(tag >> 16) & 0xff,(tag >> 24) & 0xff,size);
#endif
        	switch(tag) {
			case FCC('idx1'):
				m_rindx.fcc = tag;
				m_rindx.cb = size;
				if (Avi_Get_Idx1(pb, size) == NULL)
					goto skip;
				else
					goto the_end;
			default:
		skip:
				size += (size & 1);
				returnpt = size;
				SetFilePointer(pb,&returnpt,SEEK_CUR);	
				break;
		}
	}
the_end:
	return 0;
}

int Avi_Get_Frame(unsigned char *buffer,int buf_size,int fb,unsigned int index,int from_prefetch)
{	
	unsigned int count = Idx1buf[index].dwSize;	
	unsigned int pos = Idx1buf[index].dwOffset;
	unsigned int readerr;
	unsigned long ReadBytes;
	unsigned long returnpt;
	if(index > (m_rindx.cb/4))
		return -1;
	buf_size=buf_size;
	returnpt = pos+movi_start+8;
	SetFilePointer(fb,&returnpt,SEEK_SET);	
	readerr = ReadFile(fb,buffer,count,&ReadBytes);
	if(readerr == 0)
		return -1;	
	from_prefetch=from_prefetch;
	return count;
}


int Avi_Next_AFrame(int index)
{
	unsigned int type;
	int i;
	if(index >= (m_rindx.cb>>4))
		return -1;

	for(i=index;i<(m_rindx.cb>>4);i++)
	{
		if(Idx1buf[i].dwChunkId == AVI_01WB) //"01wb"
			break;
	}
	if(i == (m_rindx.cb>>4)){
		return -1;
	}
	return i;
}

int Avi_Next_VFrame(int index)
{
	unsigned int type;
	int i;
	if(index >= (m_rindx.cb>>4))
		return -1;
	
	for(i=index;i<(m_rindx.cb>>4);i++)
	{
		if(Idx1buf[i].dwChunkId == AVI_00DC) //"00dc"
			break;
	}
	
	if(i == (m_rindx.cb>>4)){
		return -1;
	}
	return i;
}

int Avi_Next_KeyVFrame(int index)
{
	unsigned int type;
	int i;
	if(index >= (m_rindx.cb>>4))
		return -1;
	for(i=index;i<(m_rindx.cb>>4);i++)
	{
		if((Idx1buf[i].dwChunkId == AVI_00DC)&& (Idx1buf[i].dwFlags == 0x10))
			break;
	}
	
	if(i == (m_rindx.cb>>4)){
		return -1;
	}
	return i;
}

int Avi_Back_KeyVFrame(int index)
{
	unsigned int type;
	int i;
	if(index >= (m_rindx.cb>>4))
		return -1;
	for(i=index;i>=0;i--)
	{
		if((Idx1buf[i].dwChunkId == AVI_00DC)&& (Idx1buf[i].dwFlags == 0x10))
			break;
	}	
	if(i <0) {
		return 0;
	}
	return i;
}

#else

#define AVI_RD_INDEX_COUNT 4096
#define AVI_RD_INDEX_BIT 0xfffff000

static aIndex *m_index_array_rd=NULL;
static int index_total;
static int cur_index_grd=0; //0-index_total
static int fp_avi=-1;

int Avi_FreeData(void)
{
	if(m_index_array_rd!=NULL) {
		free(m_index_array_rd);
		m_index_array_rd=NULL;
	}
	fp_avi=-1;
	cur_index_grd=0;	
	return 0;
}

static int Avi_Get_Idx1(int pb,unsigned int size)
{
	int i;
	i=sizeof(aIndex)*AVI_RD_INDEX_COUNT;
	m_index_array_rd=(aIndex *)malloc(i);
	if(m_index_array_rd == NULL) {
		LOGE("%s:malloc failed %d\n",__func__,i);
		request2reboot();
		return ERR_MALLOC;
	}	
	i=size>i?i:size;
	if(read(pb,m_index_array_rd,i)!=i) {
		free(m_index_array_rd);
		m_index_array_rd = NULL;
		LOGE("%s:read failed %d\n",__func__,i);
		return ERR_READFILE;
	}
	fp_avi=pb;
	cur_index_grd=0;
	index_total=size/sizeof(aIndex);
	LOGT("%s:get index_total = %d\n",__func__,index_total);
	return 0;
}

static int Avi_Get_Index(int pb)
{
	unsigned int tag,size;
	unsigned long returnpt;

	FUNC_ENTER
	returnpt = movi_end;
	if(lseek(pb,returnpt,SEEK_SET)!=returnpt) {
		LOGE("%s: lseek set failed 0x%x\n",__func__,(int)returnpt);
		return ERR_SEEKFILE;		
	}	
	while(1) {
		tag = get_le32(pb);
		size = get_le32(pb);
		if(size == 0) {
			LOGE("%s:zero size\n",__func__);
			return ERR_NOT_COMPLETE_FILE;
		}	
		switch(tag) {
		case FCC('idx1'):
			m_rindx.fcc = tag;
			m_rindx.cb = size;
			return Avi_Get_Idx1(pb,size);
		default:
#if 1				
			return ERR_UNKNOWN_FILEFORAMT;
#else
			size += (size & 1);
			returnpt = size;
			if(lseek(pb,returnpt,SEEK_CUR)==(off_t)-1) {//skip thisblock
				LOGE("%s:lseek cur failed 0x%x\n",__func__,(int)returnpt);
				return ERR_SEEKFILE;	
			}	
			break;
#endif								
		}
	}
}

static aIndex *avi_get_index(int i)
{
	unsigned long cur_pos;
	unsigned long dest_pos;
	int j,t;
	
	if(fp_avi==-1) {
		LOGE("%s: fp_avi=-1\n",__func__);
		return NULL;
	}	
	if(i<0||i>index_total) {
		LOGE("%s:invalid i %d(0-%d)\n",__func__,i,index_total);
		return NULL;
	}	
	if((i>=cur_index_grd)&&(i<cur_index_grd+AVI_RD_INDEX_COUNT)) { // in local cache
		return m_index_array_rd+i-cur_index_grd;
	}
	j=i&AVI_RD_INDEX_BIT;
	dest_pos=movi_end+8+(j<<4); //fuck,god dumn adi's compiler

	cur_pos=0;
	if(lseek(fp_avi,cur_pos,SEEK_CUR)==(off_t)(-1)) {
		LOGE("%s:lseek failed cur 0x%x\n",__func__,cur_pos);
		return NULL;
	}		
	if(lseek(fp_avi,dest_pos,SEEK_SET)==(off_t)(-1)) {
		LOGE("%s:lseek failed set 0x%x\n",__func__,dest_pos);
		return NULL;
	}		
	t=((index_total-j)>AVI_RD_INDEX_COUNT)?AVI_RD_INDEX_COUNT:(index_total-j);

	if(read(fp_avi,m_index_array_rd,t<<4)!=(t<<4)) {
		LOGE("%s: read failed\n",__func__);
		return NULL;
	}	
	cur_index_grd=j;
	if(lseek(fp_avi,cur_pos,SEEK_SET)!=cur_pos) {
		LOGE("%s: lseek failed befor return 0x%x\n",__func__,(int)cur_pos);
		return NULL;
	}	
	return m_index_array_rd+i-cur_index_grd;
}

unsigned int Avi_GetVFrameCount(void)
{
	int i,count=0;
	aIndex *pindex;
	
	for(i=0;i<index_total;i++)
	{
		pindex=avi_get_index(i);
		if(pindex==NULL)
			break;
		if(pindex->dwChunkId == AVI_00DC)
			count++;
	}
	return count;
}

unsigned int Avi_GetAFrameCount(void)
{
	int i,count=0;
	aIndex *pindex;
	
	for(i=0;i<index_total;i++)
	{
		pindex=avi_get_index(i);
		if(pindex==NULL)
			break;
		if(pindex->dwChunkId == AVI_01WB)
			count++;
	}
	return count;
}

unsigned int Avi_GetKeyFrameCount(void)
{
	int i,count=0;
	aIndex *pindex;
	
	for(i=0;i<index_total;i++)
	{
		pindex=avi_get_index(i);
		if(pindex==NULL)
			break;
		if((pindex->dwFlags== 0x10) && (pindex->dwChunkId== AVI_00DC))
			count++;
	}
	return count;
}

int avi_vframe(int vfrm)
{
	int i,j;
	aIndex *pindex;	
	for(i=0,j=0;(i<vfrm)&&(j<index_total);j++)
	{
		pindex=avi_get_index(j);
		if(pindex==NULL)
			return -1;
		if(pindex->dwChunkId == AVI_00DC)
			i++;
	}
	return Avi_Next_KeyVFrame(j);
}

int avi_viframe(int vfrm)
{
	int i,j;
	aIndex *pindex;	
	if(vfrm>=index_total)
		return -1;
	for(i=0,j=0;j<vfrm;j++)
	{
		pindex=avi_get_index(j);
		if(pindex==NULL)
			return -1;
		if(pindex->dwChunkId == AVI_00DC)
			i++;
	}	
	return i;
}

int Avi_GotoSpecialTime(unsigned int time)
{
	int v_frames;
	int returnframe;
	v_frames = (unsigned int)(time/1000*(float)m_aviheader.vstream_header.dwRate/ (float)m_aviheader.vstream_header.dwScale);
	returnframe = avi_vframe(v_frames);
	return (returnframe>0)?returnframe:0;
}

int Avi_GotoNextClip(unsigned int alreadyframes,unsigned int inteval)
{
	int frames;
	frames = alreadyframes+(unsigned int)(inteval*(float)m_aviheader.vstream_header.dwRate/ (float)m_aviheader.vstream_header.dwScale);
	return avi_vframe(frames);
}

int Avi_GotoPrevClip(unsigned int alreadyframes,unsigned int inteval)
{
	int frames;
	frames = alreadyframes-(unsigned int)(inteval*(float)m_aviheader.vstream_header.dwRate/ (float)m_aviheader.vstream_header.dwScale);
	return avi_vframe(frames);
}	

int Avi_Get_Frame(unsigned char *buffer,int buf_size,int fb,unsigned int index,int from_prefetch)
{	
	unsigned int count;	
	unsigned int pos;
	unsigned long returnpt;
	aIndex *pindex;

//	LOGT("%s:buf_size 0x%x index=%d\n",__func__,buf_size,index);
	pindex=avi_get_index(index);
	if(pindex==NULL) {
		return -1;
	}	
	if(buf_size<pindex->dwSize)
		return 0;
	count=pindex->dwSize;
	pos=pindex->dwOffset;
	
	returnpt = pos+movi_start+8;

#ifdef USE_PREFETCH
	if(from_prefetch!=0) {
		if(avi_tryprefetch(fb,returnpt,buffer, count)>0)
			return count;
	}
#endif

	if(lseek(fb,returnpt,SEEK_SET)!=returnpt)
		return ERR_SEEKFILE;	
	if(read(fb,buffer,count)!=count)
		return ERR_READFILE;
	else
		return count;
}


int Avi_Next_AFrame(int index)
{
	int i;
	aIndex *pindex;
	
	for(i=index;i<index_total;i++)
	{
		pindex=avi_get_index(i);
		if(pindex!=NULL) {	
			if(pindex->dwChunkId == AVI_01WB) //"00dc"
				return i;
		}else
			return -1;	
	}
	return -1;
}

int Avi_Next_VFrame(int index)
{
	int i;
	aIndex *pindex;
	
	for(i=index;i<index_total;i++)
	{
		pindex=avi_get_index(i);
		if(pindex!=NULL) {	
			if(pindex->dwChunkId == AVI_00DC) //"00dc"
				return i;
		}else
			return -1;
	}
	LOGE("%s: index between %d %d\n",__func__,i,index_total);
	return -1;
}

static int avi_get_consecutive_frame(int fb,unsigned long frm_type,unsigned long start_index,unsigned long *pend_index,unsigned char *dest_buf,unsigned long *pbuf_len)
{
	unsigned long len_top,len_used;
	int i;
	aIndex *pindex;
	static unsigned long bufer_full=0;
	unsigned long count,pos,returnpt;


	len_top=*pbuf_len;
	len_used=0;
	*pend_index=start_index;

	for(i=start_index;i<index_total;i++) {
		pindex=avi_get_index(i);
		if(pindex==NULL) {
            LOGT("pindex NULL(%d)\r\n",i);
			return -1;
        }    
		if(pindex->dwChunkId!=frm_type) {
			*pbuf_len=len_used;
			return (i-start_index);
		}
		if(len_top-len_used>=pindex->dwSize) {
			count=pindex->dwSize;
			pos=pindex->dwOffset;
			returnpt = pos+movi_start+8;

#ifdef USE_PREFETCH
			if(avi_tryprefetch(fb,returnpt,dest_buf+len_used, count)<0)
				return -1;
#else
			if(lseek(fb,returnpt,SEEK_SET)!=returnpt) {
                LOGT("seek failed 0x%x\r\n",returnpt);
				return ERR_SEEKFILE;	
            }    
			if(read(fb,dest_buf+len_used,count)!=count) {
                LOGT("read failed count=0x%x\r\n",count);
				return ERR_READFILE;
            }    
#endif
            len_used+=pindex->dwSize; //we must sure the use of pindex,because schedule may happened in avi_tryprefetch 
		}else {
		 	bufer_full++;
            //return current result if buffer full
            LOGT("frame buffer full:0x%x 0x%x 0x%x\r\n",len_top,len_used,pindex->dwSize);
            *pbuf_len=len_used;
            return (i-start_index);
		}
		*pend_index=i;
	}
//same frm_type until index_total  
    *pbuf_len=len_used;
	return index_total-start_index;
}
/*
fetch continuous audio&video frame begin from start_index,
pvbuf_len & pabuf_len is corresponding buffer len when input and the real len of each buffer when
output.
last_vindex & last_aindex is the index number when the fetch end
return: <0 for error; 0 for end of file; 1 for success
*/
int Avi_Get_Next_Consecutive_frame(PCON_GET_PARA ppara)
{
	aIndex *pindex;
	unsigned long end_index;
	unsigned long used_len;
	int frm;

	if(ppara->start_index>=index_total) {
        LOGT("end of file %d vs %d\r\n",ppara->start_index,index_total);
		return 0;
    }    
	pindex=avi_get_index(ppara->start_index);
	if(pindex!=NULL) {
		if(pindex->dwChunkId==AVI_00DC) { //first frame is video frame
			used_len=ppara->vbuf_len;
			if((frm=avi_get_consecutive_frame(ppara->fb,AVI_00DC,ppara->start_index,&end_index,ppara->buf_video,&used_len))>0) {
				ppara->vbuf_used_len=used_len;
				ppara->vfrm_count=frm;
                ppara->last_vindex=end_index;
				if(end_index>=(index_total-1)) {
                    LOGT("end of file %d vs %d,vbuf_len 0x%x\r\n",end_index,index_total,used_len);
					return 0;
				}	

				used_len=ppara->abuf_len;
				if((frm=avi_get_consecutive_frame(ppara->fb,AVI_01WB,end_index+1,&end_index,ppara->buf_audio,&used_len))>0) {
					ppara->afrm_count=frm;
					ppara->abuf_used_len=used_len;
                    ppara->last_aindex=end_index;
					if(end_index>=(index_total-1)) {
                        LOGT("end of file %d vs %d\r\n",end_index,index_total);
						return 0;
					}else{					
						return 1;
					}	
				}
			}
		}else { // first frame is audio frame
			used_len=ppara->abuf_len;		
			if((frm=avi_get_consecutive_frame(ppara->fb,AVI_01WB,ppara->start_index,&end_index,ppara->buf_audio,&used_len))>0) {
				ppara->afrm_count=frm;
				ppara->abuf_used_len=used_len;
 				ppara->last_aindex=end_index;               
				if(end_index>=(index_total-1)) {
                    LOGT("end of file %d vs %d\r\n",end_index,index_total);
					return 0;
				}
				used_len=ppara->vbuf_len;
				if((frm=avi_get_consecutive_frame(ppara->fb,AVI_00DC,end_index+1,&end_index,ppara->buf_video,&used_len))>0) {
					ppara->vfrm_count=frm;
					ppara->vbuf_used_len=used_len;
                    ppara->last_vindex=end_index;
					if(end_index>=(index_total-1)) {
                        LOGT("end of file %d vs %d\r\n",end_index,index_total);
						return 0;
					}else {
						return 1;
					}	
				}
			}
		}
	}
    ERR("current index=%d,total=%d,pindex=0x%x\r\n",ppara->start_index,index_total,pindex);
	return -1;
}

int Avi_Next_KeyVFrame(int index)
{
	int i;
	aIndex *pindex;
	
	for(i=index;i<index_total;i++)
	{
		pindex=avi_get_index(i);
		if(pindex!=NULL) {
			if((pindex->dwChunkId == AVI_00DC)&& (pindex->dwFlags == 0x10))
				return i;
		}else
			return -1;	
	}
	return 0;
}

int Avi_Back_KeyVFrame(int index)
{
	int i;
	aIndex *pindex;
	
	if(index >= index_total)
		return -1;
	for(i=index;i>=0;i--)
	{
		pindex=avi_get_index(i);
		if(pindex!=NULL) {	
			if((pindex->dwChunkId == AVI_00DC)&& (pindex->dwFlags == 0x10))
				return i;
		}else
			return -1;	
	}
	return -1;
}
#endif


