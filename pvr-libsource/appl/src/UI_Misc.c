#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>      /*open(),flag*/
#include <errno.h>      /*errno*/
#include <unistd.h>     /*ssize_t*/
#include <dirent.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <sys/statfs.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "UserMsg.h"

#include <config.h>	//Config for platform
#include <UI_Message.h>	//UI message

#include <Task_UI/task_ui_var.h>
#include <Task_UI/pvr_ui_func.h>

#include "../avi/pvr_error.h"
#include <Key.h>

#include "../avi/encoder.h"
#include "../avi/decoder.h"

extern unsigned char ImageData[];  //ryan


/*
 *------------------DECODER /ENCODER Part ----------------------------------------------
 */
/*
called by StartRecord,try to find empty auto-increasing  file name for recording.
*/

/**
 * @brief AutoGetFilename
 * 首先，检测要录制的文件名在cf卡中是否存在。
 * 其次，假如文件存在，则将文件名更改为20100221031025 (4).avi这种格式的文件
 * @param name
 * 返回要保存在CF卡里的文件名(全路径形式的)。
 * @return
 */
static int AutoGetFilename(char *name)
{
#define MAX_RENAME_FILE	0xff
	DATE_TIME sTime = {0};
	int len = 0;
	FILE * fp = NULL;
	char tmp_name[MAX_FS_PATH];
	char tmp_full_name[MAX_FS_PATH];
	char ext_rename[EXTNAME_LEN];
	int i;

    FUNC_ENTER;

	if(TESTBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_REMOVE))
	{
		ERR("%s:CFCard removed\n",__func__);
		return -1;
	}

//文件命名规范: 年月日时分秒.avi
	memset(ext_rename,0,sizeof(ext_rename));

    GetCurrentDateTime(&sTime);//获取当前系统时间
	sprintf(tmp_name,"%s%04d%02d%02d%02d%02d%02d",
			pUISystem->Vol_Disk[pUISystem->nFileSelectDisk],
			sTime.Year,sTime.Month,sTime.Day,
            sTime.Hour,sTime.Minute,sTime.Second);//比如：合成像这样”/media/cf/年月日时分秒“字符串，例如：/meida/cf/20100221031025

    sprintf(tmp_full_name,"%s%s"".avi",tmp_name,ext_rename);//比如：/meida/cf/20100221031025.avi
	len = strlen(tmp_full_name);

#ifndef ENABLE_RENAME_FILE
	fp = fopen(tmp_full_name, "r");
	if(fp!=NULL){
		fclose(fp);//TODO 有文件存在的处理
		ERR("File exist\r\n");
		ERR("Get file name failed\r\n");
		return -1;
	}
#else
	for(i=0;i<MAX_RENAME_FILE;)
	{
		fp = fopen(tmp_full_name, "r");
        if(fp!=NULL)//fp不为空，则表明该文件已经存在，需要将视频文件名更改为20100221031025 (4).avi这种格式的。
		{
			fclose(fp);
			i++;
			if(i>=MAX_RENAME_FILE)
			{
				ERR("%s:Too many times we have tried\r\n",__func__);
				return -1;
			}
            sprintf(ext_rename," (%d)",i);//比如：ext_rename[]=" (4)"
			sprintf(tmp_full_name,"%s%s"".avi",tmp_name,ext_rename);
			len = strlen(tmp_full_name);
			LOGT("File exist,rename new file to %s\r\n",tmp_full_name);
		}
		else
		{
			break;
		}
	}
#endif
	strncpy(name, tmp_full_name, len+1);
	LOGT("%s",name);

	return 0;
}



/*StartRecord: 	1.Check system state, 
 *				2.tran TEF_RECINFO to SaveAndPlayPara, give a filename
 *				3.Issue start cmd by Invoke start_encoder
 *	return: 	ERR_SYSSTATE 	error, system in a non stop state 
 *			ERR_DISK 		error, system disk failed
 *			ERR_PARAMETER 	error, input parameter formate wrong
 *			ERR_OK		success
 */
int StartRecord(int reserved){
	SaveAndPlayPara encPara;
	int retval;

	FUNC_ENTER

	if(((pUISystem->SysState.CurState)&(SYS_BIT_RECORDING_ONE_CLICK))||\
		((pUISystem->SysState.CurState)&(SYS_BIT_PLAYBACK)))
	{
		return GUI_ERR(ERR_SYSSTATE);
	}

	if (AutoGetFilename(pUISystem->sRecFilename)<0){
		return GUI_ERR(ERR_PARAMETER);
	}

	pUISystem->nRecordPause=0;//Clear Pause Stat
	
	encPara.MFileFormat=FORMAT_AVI;
	encPara.Vmethod=ENC_H264;
	encPara.Amethod=ENC_MP3;
    retval=start_encoder(pUISystem->sRecFilename,&(encPara));//开始录制

	return retval;
}



/*
 * StopRecord: 	1.Check system state, 
 *				2.Issue stop cmd by Invoke stop_encoder
 *	return: 	ERR_SYSSTATE 	error, system in a non stop state 
 *			ERR_OK		success
 */
// User should always call this func instead of stop_encoder
// Remove DoStopRecord coz these two funcs are same
int StopRecord(void)
{
#if USE_SERIAL_MSG_422
	TUserReply tReply;
	unsigned long i;
#endif
	int ret;

	FUNC_ENTER;

    if(!TESTBIT(pUISystem->SysState.CurState,SYS_BIT_RECORDING_ONE_CLICK))//检测当前是否为录制状态
	{
#if USE_SERIAL_MSG_422
//Send Reply
		tReply.ePvrStat=ePvrStop;
		SendUserMsg(&tReply);
#endif
		LOGT("%s: not recording\n",__func__);
		return GUI_ERR(ERR_SYSSTATE);
	}

    ret=stop_encoder();//停止编码

	return ret;
}

extern int restart_record_mark;
int RestartRecord(void)
{
#if USE_SERIAL_MSG_422
	TUserReply tReply;
	unsigned long i;
#endif
	int ret;

	FUNC_ENTER;
	ret = StopRecord();
	if(ret != ERR_OK)
	{
		ClearRecordState();
		return ret;
	}

	restart_record_mark = 1;

	return ret;
}

//Junr todo: adjust return,more debug output
/**
 * @brief StartPlayFile
 * 开始播放文件
 * @param fileName
 * 要播放的文件（全路径形式）。
 * @return
 */

int StartPlayFile(char *fileName)
{
	int retval;
	if (((pUISystem->SysState.CurState)&(SYS_BIT_RECORDING_ONE_CLICK))||\
		((pUISystem->SysState.CurState)&(SYS_BIT_PLAYBACK)))
	{
		return GUI_ERR(ERR_SYSSTATE);
	}

	if ((retval=Player_GetInfo(fileName,&(pUISystem->sPlayback.sFileInfo)))!=ERR_OK){
		return GUI_ERR(ERR_PARAMETER);
	}

	if ((retval=Player_Start(fileName))!=ERR_OK){
		return GUI_ERR(ERR_PARAMETER);
	}

	SETBIT(pUISystem->SysState.CurState,SYS_BIT_PLAYBACK);
    //zqh 2015年9月24日 add，清除文件浏览状态标志位
    CLEARBIT(pUISystem->SysState.CurState, SYS_BIT_FILEBROSWER);

	StartParseMUSBStatus(10);

	return ERR_OK;

}

/**
 * @brief ClearPlayState
 * 将播放状态位清零
 */

void ClearPlayState(void)
{
	CLEARBIT(pUISystem->SysState.CurState,SYS_BIT_PLAYBACK);
	//Clear Schedule Pending Mask
}

/**
 * @brief ClearRecordState
 * 将录制状态位清零
 */

void ClearRecordState(void){
	CLEARBIT(pUISystem->SysState.CurState,SYS_BIT_RECORDING_ONE_CLICK);
	//Clear Schedule Pending Mask
}

/*!
 * \brief getReplyStatus
 * 获取当前系统状态，用于回复上位机状态查询命令
 * \return
 */
unsigned int getReplyStatus()
{
    unsigned int stat;

    // IDDLE stat
    stat = 0;

    if(TESTBIT(pUISystem->SysState.CurState,SYS_BIT_RECORDING_ONE_CLICK)){
        stat |= REPLY_STAT_BIT_RECORD;
    }else if(TESTBIT(pUISystem->SysState.CurState,SYS_BIT_PLAYBACK)){
        stat |= REPLY_STAT_BIT_PLAY;
    }else if(TESTBIT(pUISystem->SysState.CurState, SYS_BIT_FILEBROSWER)){
        stat |= REPLY_STAT_BIT_FILEBROSWER;
    }else if(TESTBIT(pUISystem->SysState.CurState, SYS_BIT_SYSTEMFUNCTION)){
        stat |= REPLY_STAT_BIT_SYSTEMFUNC;
    }else if(TESTBIT(pUISystem->SysState.CurState, SYS_BIT_SYSTEMUPDATE)){
        stat |= REPLY_STAT_BIT_SYSTEMEUPDATE;
    }else{
        stat |= REPLY_STAT_BIT_STANDBY;
    }

    if(TESTBIT(pUISystem->SysState.CurState,SYS_BIT_GETTIME)){
        stat |= REPLY_STAT_BIT_GETTIME;
    }

    if(TESTBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_Mount_Flag))    {
        stat |= REPLY_STAT_BIT_UDISK;
    }

    if(TESTBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_FreeSpaceWarning)){//硬盘空间小于500M警告。
        stat |= REPLY_STAT_BIT_CFCARDFULL;
    }

    if(!TESTBIT(pUISystem->SysState.DevState, SYS_BIT_CFCard_Mount_Flag)){
        stat |= REPLY_STAT_BIT_CFCARDERROR;//电子盘故障
    }

    /*
    if(TESTBIT(stat,REPLY_STAT_BIT_RECORD) && TESTBIT(stat,REPLY_STAT_BIT_PLAY))
    {
        LOGE("invalid reply status: 0x%02x,stop playback\r\n",stat);
        Player_Stop();
        ClearPlayState();
        stat &= ~(REPLY_STAT_BIT_PLAY);
    }
    */

    return stat;
}

/**
 * @brief ShowDecoderMessage
 * 显示系统的解码信息
 * @param nMsgData
 */

void ShowDecoderMessage(unsigned short nMsgData){
#ifdef DEBUG_MESSAGEBOX
	snprintf(pUISystem->CODEC_ERRMSG,CODEC_DEBUG_MSGLEN,"%s: \nDecoder Message 0x%04x",pUISystem->sRecFilename,nMsgData);
#else
	switch(nMsgData)
	{
	case MSG_PARA_DEC_FILEEND:
	case MSG_PARA_DEC_STOP:
		snprintf(pUISystem->CODEC_ERRMSG,CODEC_DEBUG_MSGLEN,PlayBackFinished[pUISystem->allData.Language]);
		break;
	default:
		snprintf(pUISystem->CODEC_ERRMSG,CODEC_DEBUG_MSGLEN,PlayBackErr[pUISystem->allData.Language]);
		break;
	}
#endif
	MessageBox((const char *)(pUISystem->CODEC_ERRMSG),
		CODEC_MESSAGE_DELAY,
		NULL,NULL,NULL,
		NULL,NULL,NULL);
}

/**
 * @brief ShowEncoderMessage
 * 显示系统的编码信息
 * @param nMsgData
 */

void ShowEncoderMessage(unsigned short nMsgData){
#ifdef DEBUG_MESSAGEBOX
	snprintf(pUISystem->CODEC_ERRMSG,CODEC_DEBUG_MSGLEN,"%s: \nEncoder Message 0x%04x",pUISystem->sRecFilename,nMsgData);
#else
	switch(nMsgData)
	{
	case MSG_PARA_ENC_STOP:
		snprintf(pUISystem->CODEC_ERRMSG,CODEC_DEBUG_MSGLEN,MsgBox33[pUISystem->allData.Language]);
		break;
	default:
		snprintf(pUISystem->CODEC_ERRMSG,CODEC_DEBUG_MSGLEN,RecordFailed[pUISystem->allData.Language]);
		break;
	}
#endif
	MessageBox((const char*)(pUISystem->CODEC_ERRMSG),
		CODEC_MESSAGE_DELAY,
		NULL,NULL,NULL,
		NULL,NULL,NULL);
}

/**
 * @brief Pvr_AllData_Init
 * 加入没有.pvr_config.cfg这个文件，就会调用此函数，对pUISystem进行初始化，默认语言为英语
 */
void Pvr_AllData_Init(void){
	memset(&pUISystem->allData,0,sizeof(ALL_DATA));
	pUISystem->allData.MagicNum = FLASHMAGICNUMBER;

    pUISystem->allData.Language=1;//系统默认语言由英文改为中文
	pUISystem->allData.codec_info.codec_level = 1;
	pUISystem->allData.disk_full_strategy = 0;
    pUISystem->allData.encode_volume = 127;//127;
    pUISystem->allData.decode_volume = 127;//127;

}

#define FILE_READ_BUFFER_LEN 16
/**
 * @brief Parse_ConfigFiles
 * 从/opt/.pvr_config.cfg这个配置文件获得数据，将pUISystem结构体的某些变量初始化。
 * @param buf
 * 从/opt/.pvr_config.cfg这个文件获得的。
 */
void Parse_ConfigFiles(INT32U * buf)
{
	pUISystem->allData.MagicNum = buf[0];
    pUISystem->allData.Language = buf[1];//系统语言默认配置为中文
	pUISystem->allData.disk_full_strategy = buf[2];
	pUISystem->allData.codec_info.codec_level = buf[3];
	pUISystem->allData.codec_info.brightness = buf[4];
	pUISystem->allData.codec_info.chroma = buf[5];
	pUISystem->allData.codec_info.contrast = buf[6];
    pUISystem->allData.encode_volume = buf[7];
    pUISystem->allData.decode_volume = buf[8];

    //LOGT("read config file encode volume=0x%02x,decode volume=0x%02x",\
         pUISystem->allData.encode_volume,pUISystem->allData.decode_volume);
}

/**
 * @brief CheckAllDataFlash
 * 检查pUISystem的初始化变量是否在正确的范围之内
 */
void CheckAllDataFlash(void){
	int ret=0;
	if(pUISystem->allData.MagicNum!=FLASHMAGICNUMBER)
		ret = 1;
	if(pUISystem->allData.Language>=LANGUAGE_CNT)
		ret = 2;
	if(pUISystem->allData.disk_full_strategy>=FULLDISKSTRATEGY_CNT)
		ret = 3;
	if(pUISystem->allData.codec_info.codec_level>=CODECLEVEL_RANGE)
		ret = 4;
	if(pUISystem->allData.codec_info.brightness>=BRIGHTNESS_RANGE)
		ret = 5;
	if(pUISystem->allData.codec_info.chroma>=CHROMA_RANGE)
		ret = 6;
	if(pUISystem->allData.codec_info.contrast>=CONTRAST_RANGE)
		ret = 7;
	if(ret){
		LOGE("check error: %d",ret);
		Pvr_AllData_Init();
		SaveAllDataFlash();
	}
}

/*
load parameter from config file.
if the file does not exist or in wrong format,use default parameter
*/
void LoadAllDataFlash(void){
	FILE * fp;
	INT32U buf[FILE_READ_BUFFER_LEN];
	int ret = 0;

	FUNC_ENTER

	memset(&pUISystem->allData,0,sizeof(ALL_DATA));
    pUISystem->BMPStream=ImageData;
	pUISystem->allData.CurSortMethod=0;
    pUISystem->allData.Language=1;//默认使用语言为中文
    fp= fopen(CONFIG_FILE_NAME,"rb");//读/opt/.pvr_config.cfg配置文件
	if(fp!=NULL) {//JunR add note: try load config from config file first
		ret = fread(buf,sizeof(INT32U),FILE_READ_BUFFER_LEN,fp);
		if(ret<=0)
		{
			LOGE("read error: %d",ret);
			fclose(fp);
			goto use_default_config;
		}
        Parse_ConfigFiles(buf);//解析buf
        CheckAllDataFlash();//检查数据有效性
		fclose(fp);
		return;
	}
use_default_config:
    LOGE("LoadAllDataFlash Error,set to default\r\n");
    //load default config
    Pvr_AllData_Init();//无/opt/.pvr_config.cfg配置文件，就是用默认的值对变量pUISystem进行初始化
    SaveAllDataFlash();//将pUISystem的当前设置保存到配置文件中

}


/*
JunR item2:
save current setting to config file.
if the file does not exist, try to create one.
*/
void SaveAllDataFlash(void){
	int ret;
	FILE * fp;
	INT32U buf[FILE_READ_BUFFER_LEN] = {
		pUISystem->allData.MagicNum,
		pUISystem->allData.Language,
		pUISystem->allData.disk_full_strategy,
		pUISystem->allData.codec_info.codec_level,
		pUISystem->allData.codec_info.brightness,
		pUISystem->allData.codec_info.chroma,
		pUISystem->allData.codec_info.contrast,
        pUISystem->allData.encode_volume,
        pUISystem->allData.decode_volume,
		0
		};

	FUNC_ENTER

	if((fp= fopen(CONFIG_FILE_NAME,"wb"))!=NULL) {//JunR add note: try load config from config file first
		ret = fwrite(buf,sizeof(INT32U),FILE_READ_BUFFER_LEN,fp);
		if(ret<=0)
		{
			LOGE("write error: %d",ret);
//			fclose(fp);
//			return;
		}
		fclose(fp);
		
	}
}

/*
JunR item3:
get current system time and convert it to DATE_TIME format
*/
/**
 * @brief GetCurrentDateTime
 * 获取当前系统时间，如果获取localtime失败，就是用默认时间
 * @param pTime
 * 将获得的系统时间保存在此变量中
 * @return
 */
int GetCurrentDateTime(DATE_TIME *pTime){
	int cnt;
	struct tm *local;
	time_t t;

	FUNC_ENTER

	t = time(NULL);
	cnt = 0;
	while(1)
	{
		//Get local time
		local = localtime(&t);
		if(local!=NULL)
		{
			pTime->Year=local->tm_year+1900;
			pTime->Month=local->tm_mon+1;
			pTime->Day=local->tm_mday;
			pTime->Hour=local->tm_hour;
			pTime->Minute=local->tm_min;
			pTime->Second=local->tm_sec;
			get_week_from_date(pTime);
			return 0;
		}
		cnt++;
		if(cnt >= 3){
			//Get local time failed
			//set to default time
			LOGT("Get Local time failed,set to default time\r\n");
			pTime->Year=DEFAULT_YEAR;
			pTime->Month=DEFAULT_MONTH;
			pTime->Day=DEFAULT_DAY;
			pTime->Hour=DEFAULT_HOUR;
			pTime->Minute=DEFAULT_MINUTE;
			pTime->Second=DEFAULT_SECOND;
			get_week_from_date(pTime);
			return 0;
		}
	}

	return -1;
}	

/*
JunR item4:
convert and set system time
*/
/**
 * @brief SetCurrentDateTime
 * 设置当前系统时间，当与上位机校时成功时调用此函数
 * @param pTime
 * 从上位机获得的时间
 * @return
 */
int SetCurrentDateTime(DATE_TIME *pTime){
	struct tm *timeinfo;
	time_t rawtime;
	struct timeval tv;
	int ret;

	FUNC_ENTER

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	timeinfo->tm_year = pTime->Year;
	timeinfo->tm_mon = pTime->Month;
	timeinfo->tm_mday = pTime->Day;
	timeinfo->tm_hour=pTime->Hour;
	timeinfo->tm_min=pTime->Minute;
	timeinfo->tm_sec=pTime->Second;
	
	tv.tv_sec = mktime(timeinfo);
	tv.tv_usec = 0;

    // display time info
    //LOGT("get time info: year=%d mon=%d mday=%d hour=%d min=%d sec=%d\r\n",\
		pTime->Year,pTime->Month,pTime->Day,pTime->Hour,pTime->Minute,pTime->Second);

	if(tv.tv_sec<=0)
	{
		LOGE("GetUserMsgErr tv.tv_sec=%d\r\n",(int)tv.tv_sec);
		return -1;
	}

    ret = settimeofday(&tv, (struct timezone *) 0);//设置系统时间
	if(ret==0)
	{
        LOGT("set time success\r\n");
        CLEARBIT(pUISystem->SysState.CurState,SYS_BIT_GETTIME);
	}
	else
	{
		ERR("set time err:%s\r\n",strerror(errno));
	}

	return ret;
}

//Enable Auto Scan: scan SDCard UDisk etc.
/**
 * @brief CallSystemFsCheck
 * 在/proc/mounts文件中获得某一特定的字符串，比如：dev = "/dev/sda1"
 * @param dev
 * 要获得的字符串的变量。
 * @return
 */

int CallSystemFsCheck(const char *dev)
{
	FILE *fp;
	char buf[MAX_FS_PATH];
	char path[MAX_FS_PATH];
	char *str = NULL;
	char strexec[MAX_FS_PATH+20];
	int isget;

	FUNC_ENTER

	if(dev==NULL)
	{
		return -1;
	}

	memset(buf,0,sizeof(buf));
	memset(path,0,sizeof(path));

	fp = fopen(PROC_MOUNTS_FILE,"r");
	if(fp==NULL)
	{
		ERR("Open "PROC_MOUNTS_FILE" failed\n");
		return -1;
	}

	isget = 0;
	while(fgets(buf,MAX_FS_PATH,fp)!=NULL)
	{
		str = strstr(buf,dev);
		if(str!=NULL)
		{
			strcpy(path,str);
			str = strstr(path," ");
			if(str!=NULL)
			{
                memset(str,0,strlen(str));//将path字符串中以“ ”为界限后边的字符串的值变为0
				LOGT("%s:get path = %s\n",__func__,path);
				isget = 1;
				break;
			}
		}
	}

	fclose(fp);

	if(isget)
	{
		//fsck.msdos
//		sprintf(strexec,"fsck.msdos -a %s &",path);
//		system(strexec);
		return 1;
	}
	return 0;
}

/**
 * @brief FakeSetDevStat
 * 检测CF卡、SD卡、U盘是否在位的程序
 * 方法是：将一个文件写入到CF卡、SD卡、U盘的挂载目录，看是否成功
 * @param dev
 * 参数为：/media/cf、/media/usb、/media/sd
 * @return
 */

int FakeSetDevStat(const char *dev)
{
	FILE *fp;
	int len;
	char buf[MAX_FS_PATH+20];

//	FUNC_ENTER;

	strncpy(buf,dev,MAX_FS_PATH);

	len=strlen(buf);
	if(buf[len-1]!='/')
	{
		strcat(buf,"/");
	}

	strcat(buf,FAKE_DEV_STAT_FILE);

	fp = fopen(buf,"wr");
	if(fp==NULL)
	{
		LOGT("%s :%s open err\n",__func__,buf);
		return -1;
	}
	fclose(fp);
	return 0;
}

/**
 * @brief FakeGetDevStat
 * 看设备是否在位。
 * @param dev
 * @return
 */
int FakeGetDevStat(const char *dev)
{
	FILE *fp;
	char buf[MAX_FS_PATH+20];
	int len;

	strncpy(buf,dev,MAX_FS_PATH);

	len=strlen(buf);
	if(buf[len-1]!='/')
	{
		strcat(buf,"/");
	}

	strcat(buf,FAKE_DEV_STAT_FILE);

	fp = fopen(buf,"r");
	if(fp==NULL)
	{
//		LOGT("%s :%s open error\n",__func__,buf);
		//call system umount
//		snprintf(buf,MAX_FS_PATH,"umount %s &",dev);
//		system(buf);
		return -1;
	}
	fclose(fp);
	return 0;
}

// Remove record_path
/**
 * @brief DoDeleteOldFile
 * 当CF卡的剩余空间小于100M时，就删除最早录制的文件
 * @return
 */
int DoDeleteOldFile(void)
{
	int cnt;
	struct FileAttrib *pFsInfo = NULL;
	char tmpstr[MAX_FS_PATH];
#if USE_SERIAL_MSG_422
	TUserReply tReply;
#endif
	FUNC_ENTER;

    //Just rescan dir
	//set Scan dir
	strcpy(pUISystem->FileBrowserdirlist.directory,pUISystem->Vol_Disk[pUISystem->nFileSelectDisk]);
	pUISystem->FileBrowserdirlist.contain = CONTAIN_FILE;
	pUISystem->FileBrowserdirlist.nCount = 0;

	//Start Scan
//	pUISystem->FileBrowserIndex= 0;
	if (!Scandir(&(pUISystem->FileBrowserdirlist), VIDEO_FILES, pUISystem->allData.CurSortMethod)) 
	{
		ERR("%s: Scandir failed\r\n",__func__);
		ReleaseList(&(pUISystem->FileBrowserdirlist));
		return -1;
	}
	cnt = pUISystem->FileBrowserdirlist.nCount-1;
	if(cnt<=0)
	{
		LOGT("%s:No more file to delete,stop record\r\n",__func__);
		if (StopRecord()!=ERR_OK){
			ClearRecordState();
		}
#if USE_SERIAL_MSG_422
		//Send Reply
		tReply.ePvrStat=ePvrStop;
		SendUserMsg(&tReply);
#endif
		return -1;
	}
	pFsInfo = &(pUISystem->FileBrowserdirlist.attribute[cnt]);
	memset(tmpstr,0,sizeof(tmpstr));

	sprintf(tmpstr,"%s%s",pUISystem->Vol_Disk[pUISystem->nFileSelectDisk],pFsInfo->shortname);
	LOGT("%s:File name %s\n",__func__,tmpstr);

	if(strncmp(pFsInfo->shortname,RESERVED_HEADER_STR,RESERVED_HEADER_LEN)==0)
	{
		LOGT("do not delete reserved file %s\r\n",tmpstr);
		return 0;
	}

	if(strcmp(tmpstr,pUISystem->sRecFilename)==0)
	{
		LOGT("do not delete current record file %s!\r\n",pUISystem->sRecFilename);
		LOGT("stop record\r\n");
		if (StopRecord()!=ERR_OK){
			ClearRecordState();
		}
#if USE_SERIAL_MSG_422
		//Send Reply
		tReply.ePvrStat=ePvrStop;
		SendUserMsg(&tReply);
#endif
		return 0;
	}

	START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_DELETE_FILE);

	if(remove(tmpstr))
	{
		ERR("Delete File failed!\r\n");
		return -1;
    }

	sync();
	sync();
	STOP_WATCHDOG_TIMER_SIG();

	return 0;
}

// add disk full message on record state
/**
 * @brief DoDiskFullOperate
 * 当cf卡无剩余空间时候进行的操作，删除最早录制的文件。
 * 在这个函数中调用System_DevScanTimer_TO();
 * @return
 */
int DoDiskFullOperate(void)
{
#if USE_SERIAL_MSG_422
	TUserReply tReply;
#endif
	int ret;
	FUNC_ENTER

	if(!TESTBIT(pUISystem->SysState.DevState,SYS_BIT_Disk_Full))
	{
		return 0;
	}

	ret = TESTBIT(pUISystem->SysState.CurState,SYS_BIT_RECORDING_ONE_CLICK);

	if((pUISystem->nMenuIdx) == MENUBLANK)
	{
        //swtich lvds to dm365 mode
        //	SetLvdsSwitch(0);
	}

//	pUISystem->nDevCurStatusMsg = MESSAGESTAGE_DISKFULL;
//	SystemChangeWindow(MENUDEVSTATMSG,0,1,1);

	if(ret)
	{
		switch(pUISystem->allData.disk_full_strategy)
		{
        case 0:	// 自动删除
			//  Need to guarantee the file is removed completely
			//  careful watchdog timeout
			do
			{
				ret = DoDeleteOldFile();
				if(ret<0)
				{
					//  如果删除失败,空间无法释放,则停止录制,提示信息
					if(TESTBIT(pUISystem->SysState.CurState,SYS_BIT_RECORDING_ONE_CLICK))
						StopRecord();
					STOP_WATCHDOG_TIMER_SIG();
					return -1;
				}
				if(!TESTBIT(pUISystem->SysState.CurState,SYS_BIT_RECORDING_ONE_CLICK))
					break;

				ret = CheckFreeSpace();
				if(ret < 0)
				{
					if(TESTBIT(pUISystem->SysState.CurState,SYS_BIT_RECORDING_ONE_CLICK))
						StopRecord();
					STOP_WATCHDOG_TIMER_SIG();
					return -1;
				}

				if(!TESTBIT(pUISystem->SysState.DevState,SYS_BIT_Disk_Full))
					break;
			}while(1);

			break;
        case 1:	// 停止录制
			if(StopRecord() == ERR_OK)
				return 0;
			else
				return -1;
		default:
			ERR("Invalid disk_full_strategy\n");
			return -1;
		}
	}

	return 0;
}

/**
 * @brief CheckFreeSpace
 * 检测cf卡剩余空间
 * @return
 * -1:
 */
int CheckFreeSpace(void)
{
	int retval;
	char tmpstr[MAX_FS_PATH];
	long long block;
	long long free;
	long bsize;
	int pathlen;
	struct statfs disk_statfs;

    char *record_path = pUISystem->Vol_Disk[pUISystem->nFileSelectDisk];//获得cf卡的挂载路径

	memset(tmpstr,0,sizeof(tmpstr));

	pathlen = strlen(record_path);
	memcpy(tmpstr,record_path,pathlen);
	tmpstr[pathlen] = 0x0;
	retval = statfs(tmpstr,&disk_statfs);
	if(retval)
	{
		ERR("Statfs Failed!\n");
		return -1;
	}
	block = disk_statfs.f_blocks;
	free = disk_statfs.f_bfree;
	bsize = disk_statfs.f_bsize;

	block = block*(bsize/1024)/1024;
	free = free*(bsize/1024)/1024;

    //zqh 2015年10月18日add， 当cf卡空间可用空间小于500M时，对用户开始提示，cf卡空间快满了。
    if(free < MIN_WARNING_FREE_SPACE_MB)
    {
        LOGT("%lld MB free\r\n", free);
        SETBIT(pUISystem->SysState.DevState, SYS_BIT_CFCard_FreeSpaceWarning);
    }else{
        if(TESTBIT(pUISystem->SysState.DevState, SYS_BIT_CFCard_FreeSpaceWarning))
        {
            CLEARBIT(pUISystem->SysState.DevState, SYS_BIT_CFCard_FreeSpaceWarning);
        }
    }

    if(free < MIN_FREE_SPACE_MB)//当cf卡少于100M时，进入if，设置相关的系统状态位
	{
//		if(!TESTBIT(pUISystem->SysState.DevState,SYS_BIT_Disk_Full))
		{
			LOGT("There is %lldMB in total and %lldMB free\n",block,free);
            //LOGT("%s:Disk Full!\n",__func__);
			SETBIT(pUISystem->SysState.DevState,SYS_BIT_Disk_Full);
		}
	}
	else
	{
		if(TESTBIT(pUISystem->SysState.DevState,SYS_BIT_Disk_Full))
		{
			LOGT("%s:Disk Full recover\n",__func__);
			CLEARBIT(pUISystem->SysState.DevState,SYS_BIT_Disk_Full);
		}
	}

	return 0;
}

// Enable file switch
/**
 * @brief CheckRecordFileStat
 * 检测录制的文件要是大于1500M，则重新录制一个新文件
 * 在这个函数中被调用System_DevScanTimer_TO()
 * @return
 */
int CheckRecordFileStat(void)
{
	struct stat rec_stat;
	unsigned long long sizeinbyte;
	int ret;
#ifdef ENABLE_FILE_SWITCH
	SaveAndPlayPara encPara;
#endif
#if USE_SERIAL_MSG_422
	TUserReply tReply;
#endif

	ret = stat(pUISystem->sRecFilename,&rec_stat);
	if(ret < 0)//SACH:10841
	{
		return -1;
	}
	sizeinbyte = rec_stat.st_size/1024/1024;

	if(sizeinbyte > MAX_RECORDFILE_SIZE_MB)
	{
		LOGT("%s: File size larger than max file size: %ld\r\n",__func__,sizeinbyte);
#ifdef ENABLE_FILE_SWITCH
		//Switch newfile

		encPara.MFileFormat=FORMAT_AVI;
		encPara.Vmethod=ENC_H264;
		encPara.Amethod=ENC_MP3;

        ret=AutoGetFilename(pUISystem->sRecFilename);//获取新的文件名
		if(ret==0)
		{
			if((ret=change_dest_filename(pUISystem->sRecFilename,&encPara))==ERR_OK)
			{
				SETBIT(pUISystem->SysState.CurState,SYS_BIT_RECORDING_ONE_CLICK);
				return 0;
			}
			else
			{
				ERR("%s:change_dest_filename return 0x%x\n",__func__,ret);
				Post_UI_Message(MSG_ENCODER_EVENT,ret);
			}
		}
#if USE_SERIAL_MSG_422
		//Send Reply
		tReply.ePvrStat=ePvrStop;
		SendUserMsg(&tReply);
#endif
		return -1;
#else
		//Stop rec oldfile,start rec newfile
		// Set restart mark to avoid close pvr
		if(RestartRecord()!= ERR_OK)
			return -1;
#endif
	}
	return 0;
}

/**
 * @brief Blink_FPGA_LED
 * LED灯亮灭的控制
 * 会在System_DevScanTimer_TO()函数中调用
 * @param leddev
 * 对应的led灯
 * @param blink
 * @return
 */

int Blink_FPGA_LED(int leddev,int blink)
{
	int fd = -1;
	int ret;
	char *path=NULL;
	static int led2_stat = FPGA_LED_ON;
	static int led3_stat = FPGA_LED_ON;
	int ledstat;

	switch(leddev)
	{
	case FPGA_LED2:
		path=FPGA_PATH_LED2;
		led2_stat = (!led2_stat)&blink;
		ledstat = led2_stat;
		break;
	case FPGA_LED3:
		path=FPGA_PATH_LED3;
		led3_stat = (!led3_stat)&blink;
		ledstat = led3_stat;
		break;
	default:
		ERR("invalid led dev\n");
		return -1;
	}

	fd = open(path,O_WRONLY);
	if(fd<0)
	{
		ERR("get fpga led failed\npath=%s\n",path);
		return -1;
	}

	ret = write(fd,&ledstat,sizeof(ledstat));
	if(ret<=0)
	{
		ERR("set fpga led failed\n");
		close(fd);
		return -1;
	}

	close(fd);

	return 0;
}

// 从字符串中查找第column列字符串,间隔符为空格或制表符,忽略连续间隔符
// 如果字符串列数少于column,返回空指针
char *GetColumnStr(char *buff,int column)
{
	char * str=NULL;
	char * str2=NULL;
	int cnt = 0;

	str2=buff;
	do
	{
		str=str2;
		while((*str==' ')||(*str == '\t'))
		{
			str++;
		}

		cnt++;

		str2=strstr(str," ");
		if(str2 == NULL)
		{
			str2=strstr(str,"\t");
			if(str2==NULL)
				break;
		}
	}while(cnt < column);

	if(str2!=NULL)
		*str2='\0';

	if(cnt < column)
	{
		ERR("column out of scope\r\n");
		str=NULL;
	}
	return str;
}

#define INTERRUPT_FILE			"/proc/interrupts"
#define INTERRUPT_NAME			"musb-hdrc"
static int parse_musb_tick_cnt=0;
static int parse_musb_tick_max=10;
static int parse_musb_tick_en=0;
int DoParseMUSBStatus(void)
{
#if CHECK_USB_EN
	static int last_enum=0;

	int ret;
	FILE *fp=NULL;
	char buff[MAX_FS_PATH];
	char *str=NULL;
	char *str2=NULL;
	int i;

    LOGT("check usb interrupts\r\n");

	fp=fopen(INTERRUPT_FILE,"r");
	if(fp==NULL)
	{
		LOGE("open "INTERRUPT_FILE"error\r\n");
		return -1;
	}

	while(fgets(buff,MAX_FS_PATH,fp)!=NULL)
	{
		str = strstr(buff,"musb-hdrc");
		if(str!=NULL)
		{
			str=GetColumnStr(buff,2);
			if(str==NULL)
				break;
//			LOGT("%s\r\n",str);
			i=atoi(str);
//					LOGT("/proc/interrupts i=%d\r\n",i);
		}
	}

	fclose(fp);

	if(i == last_enum)
	{
		LOGT("interrupts flag not change\r\n");
		return 0;
	}

	last_enum = i;

#endif
	return 1;
}

int ParseMUSBStatus(void)
{
#if CHECK_USB_EN
	if(parse_musb_tick_en)
	{
		parse_musb_tick_cnt++;
		if(parse_musb_tick_cnt>=parse_musb_tick_max)
		{
			parse_musb_tick_cnt = 0;
			return DoParseMUSBStatus();
		}
	}
#endif
	return 1;
}


void StartParseMUSBStatus(int time_out)
{
#if CHECK_USB_EN
	FUNC_ENTER;

	parse_musb_tick_en = 1;
	parse_musb_tick_cnt = 0;
	parse_musb_tick_max = time_out;
	
#endif
}

void StopParseMUSBStatus(void)
{
#if CHECK_USB_EN
	FUNC_ENTER;

	parse_musb_tick_en = 0;
#endif
}

// 使用状态机进行cf卡在位检测状态判断
int CFCardDetectStatus = ERR_CF_MOUNT_OK;
#define CF_STAT_MACHINE_INIT            (-1)    //!<上电默认状态，尝试加载cf卡>
#define CF_STAT_MACHINE_NO_ERR          (0)     //!<检测到cf卡在位>
#define CF_STAT_MACHINE_ERR1            (1)     //!<检测到cf卡在位后，出现检测失败1>
#define CF_STAT_MACHINE_ERR2            (2)     //!<连续检测失败2>
#define CF_STAT_MACHINE_ERR3            (3)     //!<连续检测失败3，显示提示信息（无操作时不显示）>
#define CF_STAT_MACHINE_NEED_MOUNT      (4)     //!<检测到cf卡在位后，尝试加载cf卡>

/**
 * @brief System_DevScanTimer_TO
 * 扫描系统设备状态函数，一秒钟调用一次。在这个函数中调用Handle_Timer_Tick().
 * 在这个函数中Pvr_UI_Start()调用EnableTimer()函数进行初始化。
 * @param pTimer
 *
 * @return
 */
TMsgHandleStat System_DevScanTimer_TO(TEF_TIMER_STRUCT *pTimer)
{
    TMsgHandleStat retval = MsgHandled;
    int ret;
    int blink_led2;
    int blink_led3;
#if USE_SERIAL_MSG_422
    TUserReply tReply;
#endif
    int cf_stat;
    static int cf_stat_machine=0;// cf 在位状态机

    if(pTimer->TimerStatus != TIMER_DEVSCAN)
    {
        ERR("Timer Status Error\n");
        retval = MsgNeedDispatch;//Disable Timer,//Should not Happen
        goto Dev_TO_Return;
    }

    // 使用子线程检查cf卡在位状态时,通过设置系统状态标志位表示当前cf卡状态,
    // 不使用子线程检查cf卡在位状态时,每次主线程1s Timer超时,检查一次在位状态
    // 需要注意的是,使用子线程检查在位状态时,将自动进行复位、加载等操作,
    // 不使用加载子线程时需要在这里进行复位或加载操作(在调用)
#if USE_MOUNT_CF_THREAD
    cf_stat = CFCardDetectStatus;
    if(cf_stat < 0)
#else
    cf_stat = DoCFCardDetect();
    if(cf_stat < 0)
#endif
    {
        switch(cf_stat_machine)
        {
        // cf没有加载,并且上次加载成功过,才会进行未加载计数
        case CF_STAT_MACHINE_NO_ERR:
        case CF_STAT_MACHINE_ERR1:
        case CF_STAT_MACHINE_ERR2:
            cf_stat_machine++;
            break;
        // 计数到3次连续未加载,显示提示界面(文件浏览或录制)
        case CF_STAT_MACHINE_ERR3:
            // 日志信息重定向
            redirect_log(LOG_PATH_TMP,LOG_FILE_LOG);
            if((pUISystem->nMenuIdx != MENUBLANK)
                    || TESTBIT(pUISystem->SysState.CurState,SYS_BIT_RECORDING_ONE_CLICK))
            {
                LOGE("CFCard detect failed ret=%d\r\n",cf_stat);
                SystemUmountDev(CF_MOUNT_DIR);
                SetLvdsSwitch(0);

                if(TESTBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_NEEDFORMAT))
                {
                    LOGE("CFCard need format\r\n");
                    MessageBox(CFCardNeedFormat[pUISystem->allData.Language],
                            DEV_MESSAGE_DELAY,NULL,NULL,NULL,NULL,NULL,
                            NeedFormatCallback);
                }
                else
                {
                    pUISystem->nDevCurStatusMsg = MESSAGESTAGE_CFCARDREMOVE;
                    SystemChangeWindow(MENUDEVSTATMSG,1,1,0);
                }
            }
            cf_stat_machine++;
        case CF_STAT_MACHINE_INIT:
        case CF_STAT_MACHINE_NEED_MOUNT:
            if(cf_stat == ERR_CF_NEED_MOUNT)
            {
                // 尝试进行加载操作,如果当前正在加载,则不会重新启动加载线程
                SystemMountCFCard();
            }
            break;
        default:
            LOGE("invalid cf_stat_machine stat=%d\r\n",cf_stat_machine);
            break;
        }
    }
    else
    {
        cf_stat_machine = CF_STAT_MACHINE_NO_ERR;

        check_log_size();

        //only record state
        if(TESTBIT(pUISystem->SysState.CurState,SYS_BIT_RECORDING_ONE_CLICK))
        {
            ret = CheckFreeSpace();
            if(ret<0)
            {
    #ifdef EXIT_WHEN_FINISH
                request_exit_main_task = 1;
                exit_code = EXIT_CODE_PVR_ERROR;
    #endif
                pUISystem->SysState.CurState = SYS_STAT_IDLE;
            }
            else
            {
                if(TESTBIT(pUISystem->SysState.DevState,SYS_BIT_Disk_Full))//假如磁盘已满，则进入if，执行删除文件的操作
                {
                    DoDiskFullOperate();
                }
                // Record File Size Monitoring
                CheckRecordFileStat();
            }
        }
    }

Dev_TO_Return:

    //playback or OSD menu
    if((VALIDMENU(pUISystem->nMenuIdx))&&((pUISystem->nMenuIdx)!=MENUBLANK))
    {
        blink_led2 = 1;
        blink_led3 = 0;
#if USE_SERIAL_MSG_422
        tReply.ePvrStat = ePvrStop;
#endif
    }
    // blank menu
    else
    {
        blink_led3 = 1;
        if(TESTBIT(pUISystem->SysState.CurState,SYS_BIT_RECORDING_ONE_CLICK))
        {
            blink_led2 = 0;
#if USE_SERIAL_MSG_422
            tReply.ePvrStat=ePvrRec;
#endif
        }
        else
        {
            blink_led2 = 1;
#if USE_SERIAL_MSG_422
            tReply.ePvrStat=ePvrStop;
#endif
        }
    }

#if USE_SERIAL_MSG_422
    SendUserMsg(&tReply);

#if !(ENABLE_422_KEY_MSG)
    if(TESTBIT(pUISystem->SysState.CurState,SYS_BIT_GETTIME))
    {
        tReply.ePvrStat=ePvrTime;
        SendUserMsg(&tReply);//发送当前系统状态
    }
#endif

#endif
    //设置led亮灭状态
    Blink_FPGA_LED(FPGA_LED2,blink_led2);
    Blink_FPGA_LED(FPGA_LED3,blink_led3);

    return retval;
}
/**
 * @brief SystemUmountDev
 * 卸载设备
 * @param path
 * 需要卸载设备的路径
 * @return
 */
int SystemUmountDev(const char *path)
{
	int ret;
	FUNC_ENTER;

	START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_MOUNT);

	//umount
    ret = umount(path);

	STOP_WATCHDOG_TIMER_SIG();
	
	if(ret == 0)
	{
		return 0;
	}

    // 这里仅仅是警告,因为有些情况下cf卡并未加载,就尝试卸载
	LOGW("unmount failed: %s\n",strerror(errno));

	return -1;
}

/*
device path for cf card
/sys/devices/platform/musb_hdrc/usb1/1-1/1-1.2/1-1.2:1.0/host0/target0:0:0/0:0:0:0/block/sda

device path for usb disk
/sys/devices/platform/musb_hdrc/usb1/1-1/1-1.1/1-1.1:1.0/

device path for sd card
/sys/devices/platform/davinci_mmc.0/mmc_host/mmc0/  mmc0:0002/block/mmcblk0/mmcblk0p1
*/
#if 0 //for kernel 2.6.32-rc1
const char cf_path_pattern[]="/sys/devices/platform/musb_hdrc/usb1/1-1/1-1.2/1-1.2:1.0/host%d/target%d:%d:%d/%d:%d:%d:%d/block/sd%c/sd%c%d/dev";
const char usbdisk_path_pattern[]="/sys/devices/platform/musb_hdrc/usb1/1-1/1-1.1/1-1.1:1.0/host%d/target%d:%d:%d/%d:%d:%d:%d/block/sd%c/sd%c%d/dev";
#else //for kernel 2.6.32-17
const char cf_path_pattern[]="/sys/devices/platform/musb-davinci/musb-hdrc/usb1/1-1/1-1.2/1-1.2:1.0/host%d/target%d:%d:%d/%d:%d:%d:%d/block/sd%c/sd%c%d/dev";
const char cf_path_pattern_nopartition[]="/sys/devices/platform/musb-davinci/musb-hdrc/usb1/1-1/1-1.2/1-1.2:1.0/host%d/target%d:%d:%d/%d:%d:%d:%d/block/sd%c/dev";
const char usbdisk_path_pattern[]="/sys/devices/platform/musb-davinci/musb-hdrc/usb1/1-1/1-1.1/1-1.1:1.0/host%d/target%d:%d:%d/%d:%d:%d:%d/block/sd%c/sd%c%d/dev";
const char usbdisk_path_pattern_nopartition[]="/sys/devices/platform/musb-davinci/musb-hdrc/usb1/1-1/1-1.1/1-1.1:1.0/host%d/target%d:%d:%d/%d:%d:%d:%d/block/sd%c/dev";
const char usbdisk_path_pattern_usb2[]="/sys/devices/platform/musb-davinci/musb-hdrc/usb1/1-1/1-1.4/1-1.4:1.0/host%d/target%d:%d:%d/%d:%d:%d:%d/block/sd%c/sd%c%d/dev";
const char usbdisk_path_pattern_usb2_plus[]="/sys/devices/platform/musb-davinci/musb-hdrc/usb1/1-1/1-1.4/1-1.4:1.0/host%d/target%d:%d:%d/%d:%d:%d:%d/block/sd%c/dev";
#endif
const char sd_path_pattern[]="/sys/devices/platform/davinci_mmc.0/mmc_host/mmc0/mmc0:000%d/block/mmcblk%d/mmcblk%dp%d/dev";
const char sd_path_pattern_nopartition[]="/sys/devices/platform/davinci_mmc.0/mmc_host/mmc0/mmc0:000%d/block/mmcblk%d/dev";

//const char usbdisk_path_pattern[]="/sys/devices/pci0000:00/0000:00:12.0/usb1/1-1/1-%d:1.0/host%d/target%d:%d:%d/%d:%d:%d:%d/block/sd%c/sd%c%d/dev";


#define INVALID_DEVICE_NUM 0xFFFFFFFF
#define MAX_DIR_SIZE_PVR 160
#define MAX_DIR_ENTRY_SIZE_PVR 32

/**
 * @brief get_device_num_interator
 * 通过设备路径获得设备的主次设备号
 * @param path_pattern
 * 设备在sys目录下的路径
 * @param begin_pos
 * 其实位置
 * @return
 * 返回设备的主次设备号
 */
int get_device_num_interator(const char path_pattern[],int begin_pos)
{
	char parent_dir[MAX_DIR_SIZE_PVR],find_entry_name[MAX_DIR_ENTRY_SIZE_PVR];
	int pos_solidus_end,i;
	DIR *dir_par=NULL;
	struct dirent *dir_ent=NULL;
	int tmp[8],use_scanf;

	memset(parent_dir,0,MAX_DIR_SIZE_PVR);
	memset(find_entry_name,0,MAX_DIR_ENTRY_SIZE_PVR);
	if(path_pattern[begin_pos]=='/') { 
		strncpy(parent_dir,path_pattern,begin_pos+1);  //copy include the '/'
		begin_pos+=1; //begin_pos point to the next valid entry
	}else {
		strncpy(parent_dir,path_pattern,begin_pos+2); //suppose the '/' is next to the begin_pos
		begin_pos+=2; //begin_pos point to the next valid entry
	}
	parent_dir[begin_pos]=0;//append zero to the end of parent_dir

	//find the end of entry
	use_scanf = 0;
	pos_solidus_end = -1;
	for(i=0;i<MAX_DIR_ENTRY_SIZE_PVR;i++) {
		if(path_pattern[begin_pos+i]=='/') {
			pos_solidus_end=begin_pos+i;
			break;
		}	
		if(path_pattern[begin_pos+i]==0)
			break;
		if(path_pattern[begin_pos+i]=='%') 
			use_scanf = 1;
		find_entry_name[i]=path_pattern[begin_pos+i];
	}
	if(!strlen(find_entry_name)) {
		return INVALID_DEVICE_NUM;
	}
	//time to scan the parent_dir to find matching find_entry_name
//	printf("opendir %s looking for %s\n",parent_dir,find_entry_name);
	
	dir_par = opendir((const char *)parent_dir);
	if(dir_par==NULL) {
//		perror("open dir failed");
		return INVALID_DEVICE_NUM;
	}	
	dir_ent=readdir(dir_par);
	while(dir_ent!=NULL) {
//		printf("dir entry: %s\n",dir_ent->d_name);
		if(use_scanf) {
			if(sscanf(dir_ent->d_name,find_entry_name,&tmp[0],&tmp[1],&tmp[2],&tmp[3],&tmp[4],&tmp[5],&tmp[6],&tmp[7])!=0)
				break;
		}else {
			if(strcmp(dir_ent->d_name,find_entry_name)==0)
				break;
		}
		dir_ent=readdir(dir_par);
	}
	if(dir_ent==NULL) {
//		printf("no maching dir entry\n");
		goto error_out;
	}
	if(dir_ent->d_type&DT_REG) {
		closedir(dir_par);
		strcat(parent_dir,dir_ent->d_name);
		int fd_dev,major_num,minor_num;
		int dev_num=INVALID_DEVICE_NUM;
		
		fd_dev=open(parent_dir,O_RDONLY);
		if(fd_dev>0) {
			memset(find_entry_name,0,MAX_DIR_ENTRY_SIZE_PVR);
			if((i= read(fd_dev,find_entry_name,MAX_DIR_ENTRY_SIZE_PVR))>0) {//SACH:W10842
				if(i>1 && (sscanf(find_entry_name,"%d:%d",&major_num,&minor_num)==2))//SACH:W10842
					dev_num = major_num<<16|minor_num;
			}
			close(fd_dev);
		}
		return dev_num;
	}else {
		if(pos_solidus_end<begin_pos)
			goto error_out;
		strcat(parent_dir,dir_ent->d_name);
		begin_pos = strlen(parent_dir);
		strcat(parent_dir,path_pattern+pos_solidus_end);
		closedir(dir_par);
//		printf("next dir scan is %s\n",parent_dir);
		return get_device_num_interator(parent_dir,begin_pos);
	}	
error_out:
	if(dir_par!=NULL) {
		closedir(dir_par);
	}
	return INVALID_DEVICE_NUM;
}

/**
 * @brief convert_to_device_name
 * 根据设备的主次设备号，获得设备在dev目录下生成的设备名字
 * @param device_num
 * 主次设备号
 * @return
 */
const char *convert_to_device_name(int device_num)
{
	static const struct {
		int major;
		int minor;
		const char *dev_path;
	}device_table[]={{8,0,"sda"},
					{8,1,"sda1"},
					{8,2,"sda2"},
					{8,16,"sdb"},
					{8,17,"sdb1"},
					{8,18,"sdb2"},
					{8,32,"sdc"},
					{8,33,"sdc1"},
					{8,34,"sdc2"},
					{179,0,"mmcblk0"},
					{179,1,"mmcblk0p1"},
					{179,2,"mmcblk0p2"},
					{179,32,"mmcblk1"},
					{179,33,"mmcblk1p1"},
					{179,34,"mmcblk1p2"},					
		};
	int i;
//	printf("device_num is 0x%x\n",device_num);
	for(i=0;i<sizeof(device_table)/sizeof(device_table[0]);i++) {
		if((device_table[i].major==((device_num>>16)&0xffff))
			&&(device_table[i].minor==(device_num&0xffff)))
			return device_table[i].dev_path;
	}

	ERR("can't find device.device_num_major:%d device_num_minor:%d\r\n",((device_num>>16)&0xffff),(device_num&0xffff));

	return NULL;
}

static char DevPath[SUPPORT_DISK_CNT][DEV_PATH_LEN];

/**
 * @brief CF卡在位时的复位操作,如果复位不正确(复位后30s内,仍无法找到设备),进行系统复位
 * @return 检测到device返回0,如果在复位过程中cf卡不在位,返回ERR_CF_NOT_IN,
 *          如果复位之后(30s)仍检测不到cf卡,返回ERR_CF_RESET
 */
int ResetCFDevice(void)
{
    static int reset_in_progress=0;
    static int reset_cnt=0;
    int tmp_num;

    FUNC_ENTER;
    if(TESTBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_RESET) == 0)
    {
        LOGT("reset in progress\r\n");
        return 0;
    }

    if(reset_in_progress == 0)
    {
        redirect_log(LOG_PATH_TMP,LOG_FILE_LOG);
        SystemUmountDev(CF_MOUNT_DIR);

        reset_in_progress = 30;
        START_WATCHDOG_TIMER_SIG(60);
        ResetCFCard();
        ResetHUB();
        //等待复位完成/
        while(reset_in_progress > 0)
        {
            if(IsCFCardInSlot()==0)
            {
                SETBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_RESET);
                return ERR_CF_NOT_IN;
            }
            tmp_num = get_device_num_interator(cf_path_pattern,0);
        	if(tmp_num != INVALID_DEVICE_NUM)
                break;
            tmp_num = get_device_num_interator(cf_path_pattern_nopartition,0);
            if(tmp_num != INVALID_DEVICE_NUM)
                break;
            sleep(1);
            reset_in_progress--;
        }
        STOP_WATCHDOG_TIMER_SIG();

        // 复位之后等待30s仍没有检测到cf卡,并且当前cf卡在位,需要进行整板复位
        if(reset_in_progress == 0)
        {
            // 不管任意位置,调用过两次复位hub和cf卡控制芯片,,仍检测不到cf卡信息,都认为需要整板复位
            reset_cnt++;
            if(reset_cnt >= 2)
            {
                LOGE("reset hub&cf controller does not take effact.need system reboot\r\n");
                exit_code = EXIT_CODE_REBOOT;
                request2reboot();
                LOGE("wait to reboot\r\n");
                while(1)
                {
                    sleep(1);
                }
            }
        }
        reset_in_progress = 0;
    }

    CLEARBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_RESET);

    return 0;
}

/**
 * @brief DoCFCardDetect
 * 首先检测CF卡是否在位，
 * 假如CF卡已插入的话，查找该设备的主次设备号，找到设备号后，转化为在dev目录下的设备名
 * @return
 */
int DoCFCardDetect(void)
{
    FILE *fp;
    char buf[MAX_FS_PATH];
    char *str=NULL;
    int len=0;
    char buf2[DEV_PATH_LEN];

    int device_num;
    const char *dev_path;

    int cf_last_remove_flag=0;

    static int check_remount_cnt=0;
    char current_dev_name[DEV_PATH_LEN];// 读到的device name 临时变量,在检测到device name 变化时,需要连续判断3次才认为需要重新加载
                                        // 在这个判断过程中,不修改已经保存的全局DevPath>

    cf_last_remove_flag = TESTBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_REMOVE);
    //! 通过读CF卡在位寄存器（FPGA寄存器）状态，获取CF卡在位状态，并设置系统标志位
    if(IsCFCardInSlot()==0)
    {
        SETBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_REMOVE);
        return ERR_CF_NOT_IN;
    }

    //! 获取CF卡的主次设备号
    device_num = get_device_num_interator(cf_path_pattern,0);
    if(device_num==INVALID_DEVICE_NUM) {
        // 如果未获取到sd*1的设备名，尝试获取sd*设备名
        device_num = get_device_num_interator(cf_path_pattern_nopartition,0);
        if(device_num==INVALID_DEVICE_NUM) {
            // 尝试卸载CF卡,如果cf卡未加载会提示警告
            SystemUmountDev(CF_MOUNT_DIR);
#if DEBUG_V2_1
            // CF卡在位,但cf控制器需要复位,返回复位错误码
            SETBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_RESET);
            return ERR_CF_RESET;
#else
            // 旧版板卡没有fpga复位功能
            SETBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_REMOVE);
            return ERR_CF_NOT_IN;
#endif
        }
    }

    // 获取/dev/sd* 设备名
    dev_path=convert_to_device_name(device_num);
    if(dev_path!=NULL)
    {
        strcpy(current_dev_name,dev_path);
    }
    else
    {
        // 获取设备名失败,不应该发生该情况,应重新检查cf卡在位状态
        ERR("get device name failed\r\n");
        return ERR_CF_MOUNT_ERR;
    }

    CLEARBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_REMOVE);

    //! 通过读/proc/mounts 文件,检测cf卡是否已经加载
    fp = fopen(PROC_MOUNTS_FILE,"r");
    if(fp==NULL)
    {
        // 读加载信息失败,不应该发生该情况,应重新检查cf卡在位状态
        ERR("Open "PROC_MOUNTS_FILE" failed\r\n");
        return ERR_CF_MOUNT_ERR;
    }

    while(fgets(buf,MAX_FS_PATH,fp)!=NULL)
    {
        str = strstr(buf,CF_MOUNT_DIR);
        // 检测到cf卡已经加载到"/media/cf"目录
        if(str != NULL)
        {
            // 上一次检测到CF卡不在位(通过FPGA寄存器),需要重新加载
            if(cf_last_remove_flag)
            {
                LOGT("found cf card for first time\r\n");
                LOGT("need remount\r\n");
                // 尝试卸载CF卡,如果cf卡未加载会提示警告
                SystemUmountDev(CF_MOUNT_DIR);
                fclose(fp);
                return ERR_CF_NEED_MOUNT;
            }
            // 检查CF卡device name
            sprintf(buf2,"/dev/%s ",current_dev_name);
            str = strstr(buf,buf2);
            // 上次加载的device name 与本次检测到的device name不同
            if(str == NULL)
            {
                LOGT("mount message:%s\r\n",buf);
                LOGT("current_dev_name: %s\r\n",current_dev_name);

                // device name不同的情况下,需要连续3次判断,以防误判
                check_remount_cnt++;
                if(check_remount_cnt >= 3)
                {
                    check_remount_cnt = 0;
                    strcpy(DevPath[DISK_CF],current_dev_name);

                    LOGT("check device name incompatible over 3 times,need remount\r\n");
                    // 尝试卸载CF卡,如果cf卡未加载会提示警告
                    SystemUmountDev(CF_MOUNT_DIR);
                    fclose(fp);

                    return ERR_CF_NEED_MOUNT;
                }
                else
                {
                    fclose(fp);

                    return ERR_CF_MOUNT_OK;
                }
            }
            // 只要不是连续检测到device name不同,就认为是误判.
            check_remount_cnt = 0;
            strcpy(DevPath[DISK_CF],current_dev_name);

            // 仅在最终判断确定了cf卡device name
            strcpy(pUISystem->Vol_Disk[DISK_CF],CF_MOUNT_DIR);
            len = strlen(pUISystem->Vol_Disk[DISK_CF]);
            if(pUISystem->Vol_Disk[DISK_CF][len-1]!='/')
            {
                strcat(pUISystem->Vol_Disk[DISK_CF],"/");
            }

            fclose(fp);

            // 对于已经在位的cf卡,通过写入测试检测cf卡读写是否正常.
            if(FakeSetDevStat(CF_MOUNT_DIR))
            {
                LOGE("test cf card write failed,need remount\r\n");
                // 尝试卸载CF卡,如果cf卡未加载会提示警告
                SystemUmountDev(CF_MOUNT_DIR);

                return ERR_CF_NEED_MOUNT;
            }

            return ERR_CF_MOUNT_OK;
        }
    }

    // 未检测到cf卡的加载信息,需要进行加载
    strcpy(DevPath[DISK_CF],current_dev_name);
    fclose(fp);

    return ERR_CF_NEED_MOUNT;
}

/**
 * @brief DoUDiskDetect
 * 检测优盘是否挂载。
 * @return
 */

int DoUDiskDetect(void)
{
    FILE *fp=NULL;
    char buf[MAX_FS_PATH];
    char *str=NULL;
    char tmp_dev_str[DEV_PATH_LEN];
    int len=0;

    int device_num;
    const char *dev_path;

    SETBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_REMOVE);

    //! 检测U盘device num
    /// 检测顺序:
    /// 外接(由接插件引出)sdb1->外接sdb->板上预留(测试/升级新版预留)sdb1->板上预留sdb
    device_num = get_device_num_interator(usbdisk_path_pattern,0);
    if(device_num==INVALID_DEVICE_NUM) {
        device_num = get_device_num_interator(usbdisk_path_pattern_nopartition,0);
        if(device_num==INVALID_DEVICE_NUM) {
            device_num = get_device_num_interator(usbdisk_path_pattern_usb2,0);
            if(device_num==INVALID_DEVICE_NUM) {
                device_num = get_device_num_interator(usbdisk_path_pattern_usb2_plus,0);
                if(device_num==INVALID_DEVICE_NUM) {
                    ERR("No udisk partitions msg\r\n");
                    return -1;
                }
            }
        }
    }
    dev_path=convert_to_device_name(device_num);
    if(dev_path!=NULL) {
        LOGT("dev_path: %s\r\n",dev_path);
        strcpy(DevPath[DISK_USB],dev_path);
    }else {
        //JunR@201111 call mknod to create the node manually
        LOGE("get usb device name failed,dev_num=0x%8x\r\n",device_num);
        return -1;
    }

    fp = fopen(PROC_MOUNTS_FILE,"r");
    if(fp==NULL)
    {
        ERR("Open "PROC_MOUNTS_FILE" failed\n");
        return -1;
    }

    while(fgets(buf,MAX_FS_PATH,fp)!=NULL)
    {
        str = strstr(buf,USB_MOUNT_DIR);
        if(str != NULL)
        {
            //! DevPath 判断
            // 如果在/proc/mounts 中有 /media/usb,但是已经mount的devpath与当前的
            // devpath不同,则认为是上一次没有umount的情况下拔掉了U盘,执行umount
            // 并设置当前状态为U盘卸载状态
            sprintf(tmp_dev_str,"/dev/%s ",DevPath[DISK_USB]);
            str = strstr(buf,tmp_dev_str);
            if(str == NULL)
            {
                LOGT("mount message:%s\r\n",buf);
                LOGT("DevPath[DISK_USB]: %s\r\n",DevPath[DISK_USB]);
                LOGT("%s:need remount\r\n",__func__);
                SystemUmountDev(USB_MOUNT_DIR);
                fclose(fp);
                return 0;
            }

            LOGT("udisk is already mounted\r\n");
            fclose(fp);

            strcpy(pUISystem->Vol_Disk[DISK_USB],USB_MOUNT_DIR);

            len = strlen(pUISystem->Vol_Disk[DISK_USB]);
            if(pUISystem->Vol_Disk[DISK_USB][len-1]!='/')
            {
                strcat(pUISystem->Vol_Disk[DISK_USB],"/");
            }

            CLEARBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_REMOVE);

            return 0;
        }
    }
    fclose(fp);

    return 0;
}

/**
 * @brief DoSDCardDetect
 * 检测sd卡是否挂载
 * @return
 */

int DoSDCardDetect(void)
{
	FILE *fp=NULL;
	char buf[MAX_FS_PATH];
	char *str=NULL;
	char tmp_dev_str[DEV_PATH_LEN];

	int len=0;

	int device_num;
	const char *dev_path;

	SETBIT(pUISystem->SysState.DevState,SYS_BIT_SDCard_REMOVE);

	device_num = get_device_num_interator(sd_path_pattern,0);
	if(device_num==INVALID_DEVICE_NUM) {
		device_num = get_device_num_interator(sd_path_pattern_nopartition,0);
		if(device_num==INVALID_DEVICE_NUM) {
			ERR("No sd partitions msg\r\n");
			return -1;		
		}	
	}
	dev_path=convert_to_device_name(device_num);
	if(dev_path!=NULL)
		strcpy(DevPath[DISK_SD],dev_path);
	else {
		//JunR@201111 call mknod to create the node manually

	}

	fp = fopen(PROC_MOUNTS_FILE,"r");
	if(fp==NULL)
	{
		ERR("Open "PROC_MOUNTS_FILE" failed\n");
		return -1;
	}

	while(fgets(buf,MAX_FS_PATH,fp)!=NULL)
	{
        str = strstr(buf,SD_MOUNT_DIR);//查找/proc/mounts文件里是否包含/media/sd类型的字符串
		if(str != NULL)
		{
            sprintf(tmp_dev_str,"/dev/%s ",DevPath[DISK_SD]);//查找/proc/mounts文件里是否包含/dev/mmc*类型的字符串
			str = strstr(buf,tmp_dev_str);
			if(str==NULL)
			{
				LOGT("mount message:%s\r\n",buf);
				LOGT("DevPath[DISK_SD]: %s\r\n",DevPath[DISK_SD]);
				LOGT("%s:need remount\r\n",__func__);
				SystemUmountDev(SD_MOUNT_DIR);
				fclose(fp);
				return 0;
			}

			LOGT("SDCard is already mounted\r\n");

			strcpy(pUISystem->Vol_Disk[DISK_SD],SD_MOUNT_DIR);

			len = strlen(pUISystem->Vol_Disk[DISK_SD]);
			if(pUISystem->Vol_Disk[DISK_SD][len-1]!='/')
			{
				strcat(pUISystem->Vol_Disk[DISK_SD],"/");
			}

			CLEARBIT(pUISystem->SysState.DevState,SYS_BIT_SDCard_REMOVE);

			fclose(fp);
			return 0;
		}
	}
	fclose(fp);

	return 0;
}

/**
 * @brief check_e2fs_lib
 * 检测e2fs库是否有效函数,通过检查mkfs.ext3是否有效确定
 * @return
 */
int check_e2fs_lib()
{
    FILE *fp;
	int len;
	char buf[MAX_FS_PATH+20];

	sprintf(buf,"/sbin/mkfs.ext3");

	fp = fopen(buf,"r");
	if(fp==NULL)
	{
		LOGT("%s :%s open err\n",__func__,buf);
		return -1;
	}
	fclose(fp);

	return 0;
}

extern int DoCFCardFsck(void *pUser);

/**
 * @brief DoMountCFCard
 * 检测cf卡在位正常,需要进行加载时调用
 * @return
 */
int DoMountCFCard(void)
{
    char tmp_dev_path[64];
    int len;
    int ret;


    // 设置当前为加载中状态,在加载中状态按任意键,显示正在加载中的提示信息
    SETBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_MOUNTING);

    // 获取当前设备名
    sprintf(tmp_dev_path,"/dev/%s",DevPath[DISK_CF]);

    // 不使用单独线程加载时，需要设置看门狗超时时间
#if !(USE_MOUNT_CF_THREAD) && !(USE_MOUNT_CF_THREAD_S)
    START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_MOUNT);
#endif

    // 检查是否有e2fs 库,如果没有,则无法mount成ext3文件系统,使用系统调用自动指定文件系统
    ret = check_e2fs_lib();
    if(ret < 0)
    {
        // 使用系统调用,不指定mount type
        sprintf(tmp_dev_path,"mount /dev/%s "CF_MOUNT_DIR,DevPath[DISK_CF]);
        system(tmp_dev_path);
        ret = 0;
    }
    else
    {
        ret = mount( tmp_dev_path , CF_MOUNT_DIR , MOUNT_TYPE_CF , 0 /*MS_SYNCHRONOUS*/ , MOUNT_DATA_CF );
    }
#if !(USE_MOUNT_CF_THREAD) && !(USE_MOUNT_CF_THREAD_S)
    STOP_WATCHDOG_TIMER_SIG();
#endif

#if (USE_MOUNT_CF_THREAD) || (USE_MOUNT_CF_THREAD_S)
    // 使用子线程加载时,第一次加载失败后,进行文件系统检查,重新加载一次
    // 如果不使用子线程加载,文件系统检查时间过长,因此不自动进行文件系统检查
    if(ret)
    {
        LOGE("Mount CFCard Failed,check & retry\r\n");

        // 检查是否有e2fs 库,如果没有,则无法mount成ext文件系统,使用系统调用自动指定文件系统
        ret = check_e2fs_lib();
        if(ret < 0)
        {
            // 直接mount 使用系统调用,不指定mount type
            sprintf(tmp_dev_path,"mount /dev/%s "CF_MOUNT_DIR,DevPath[DISK_CF]);
            system(tmp_dev_path);
            ret = 0;
        }
        else
        {
            // 文件系统检查
            SETBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_FSCK);
            ret = DoCFCardFsck(0);
            if(ret >= 0)
            {
                // 重新mount
                ret = mount( tmp_dev_path , CF_MOUNT_DIR , MOUNT_TYPE_CF , 0/*MS_SYNCHRONOUS*/ , MOUNT_DATA_CF );
            }
        }
    }
#endif
    //! CFCard mount 失败处理
    if(ret)
    {
        ERR( "mount %s to "CF_MOUNT_DIR" failed!%s %d\n" , tmp_dev_path , strerror(errno) , errno );
#if DEBUG_V2_1
        // No such device or address,错误代码为6,重新复位,检测cf卡
        if(errno == 6)
        {
            ret = ResetCFDevice();
            // 复位后仍检测不到cf卡,说明在加载过程中,cf卡不在位
            // 退出本次加载,并进行整板复位
            // 否则,认为是cf卡格式不正确,需要格式化,在之后的逻辑中再尝试加载
            if(ret < 0)
            {
                return ret;
            }
        }
#endif
        // 加载失败,cf卡有可能需要格式化
        SETBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_NEEDFORMAT);

        return ERR_CF_FORMAT;
    }

#if (!DEBUG_V2_1)
    if(FakeSetDevStat(CF_MOUNT_DIR))
    {
        ERR("test cf card error\r\n");
        return ERR_CF_MOUNT_ERR;
    }
#endif

    strcpy(pUISystem->Vol_Disk[DISK_CF],CF_MOUNT_DIR);
    len = strlen(pUISystem->Vol_Disk[DISK_CF]);
    if(pUISystem->Vol_Disk[DISK_CF][len-1]!='/')
    {
        strcat(pUISystem->Vol_Disk[DISK_CF],"/");
    }

    CLEARBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_NEEDFORMAT);
    CLEARBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_MOUNTING);
    CLEARBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_REMOVE);

    LOGT("Get CFCard Info suc:\r\n%s\r\n",DevPath[DISK_CF]);
    LOGT("Mount CFCard suc:\r\n%s\r\n",pUISystem->Vol_Disk[DISK_CF]);


    //! redirect log to cfcard log file if mount cfcard success
    // get startup log
    move_log(LOG_PATH_TMP,LOG_PATH_CFCARD,LOG_FILE_STARTUP);

    // get fsck log
    move_log(LOG_PATH_TMP,LOG_PATH_CFCARD,LOG_FILE_FSCK);

    // get format log
    move_log(LOG_PATH_TMP,LOG_PATH_CFCARD,LOG_FILE_FMT);

    // get last kernel msg
    move_log(LOG_PATH_TMP,LOG_PATH_CFCARD,LOG_FILE_KERNEL_LAST);

    // get startup kernel msg
    move_log(LOG_PATH_TMP,LOG_PATH_CFCARD,LOG_FILE_KERNEL_START);

    // copy old log
    move_log(LOG_PATH_TMP,LOG_PATH_CFCARD,LOG_FILE_LOG_OLD);
    // cat current log
    cat_log(LOG_PATH_TMP,LOG_PATH_CFCARD,LOG_FILE_LOG);
    // redirect log output
    redirect_log(LOG_PATH_CFCARD,LOG_FILE_LOG);

    return 0;
}

/**
 * @brief SystemMountCFCard
 * 执行加载cf卡的函数入口,如果使用子线程进行加载,则开始创建子线程,否则进行cf卡检测、加载操作
 * @return
 */
int SystemMountCFCard(void)
{
    int ret = 0;
    if(CF_Mount_Runing==0)
    {
#if (USE_MOUNT_CF_THREAD) || (USE_MOUNT_CF_THREAD_S)
        // 使用检测CF在位子线程进行加载,如果当前未启动,将启动cf检测子线程
        StartMountCF_Thread();
#else
        CF_Mount_Runing = 1;
        // 检查cf卡在位状态,并设置系统状态
        // 在系统主流程的timer中,根据异常系统状态,显示不同的界面信息
        ret = DoCFCardDetect();
        switch(ret)
        {
        case ERR_CF_MOUNT_OK:
            ret = 0;
            break;
        case ERR_CF_NOT_IN:
            ret = -1;
            break;
        case ERR_CF_RESET:
            // 复位cf卡控制器与hub,需要注意的是,连续两次复位之后仍检测不到cf卡,就会进行系统复位
            // 设置了系统复位之后,将等待系统复位完成,而不再进行cf在位检查
            ret = ResetCFDevice();
            break;
        case ERR_CF_NEED_MOUNT:
            if((TESTBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_FORMAT) == 0)
                &&(TESTBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_FSCK) == 0))
            {
                ret = DoMountCFCard();
                if(ret == ERR_CF_FORMAT)
                {
                    LOGE("CFCard need format");
                    SystemUmountDev(CF_MOUNT_DIR);
                    SetLvdsSwitch(0);
                    MessageBox(CFCardNeedFormat[pUISystem->allData.Language],
                            DEV_MESSAGE_DELAY,NULL,NULL,
                            NULL,NULL,NULL,NeedFormatCallback);

                    CLEARBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_NEEDFORMAT);
                }
            }
            break;
        case ERR_CF_FORMAT:
        case ERR_CF_MOUNT_ERR:
        default:
            break;
        }
        CF_Mount_Runing = 0;
#endif
    }

    return ret;
}

/*
 * mount usb disk function
 * parse /proc/partitions to get dev_path
 * disgard cf card dev_path
 * mount usb disk to /media/usb/
 * clear system dev-remove flag
 * Note: mount the first usb disk (found in /proc/partitions) ONLY
*/
int SystemMountUDisk(void)
{
	char buf[DEV_PATH_LEN];
	int len;
	int ret;

	FUNC_ENTER;

	sprintf(buf,"/dev/%s",DevPath[DISK_USB]);
	LOGT("mount %s to %s\r\n",buf,USB_MOUNT_DIR);

// 0802-2012 add mount timeout
//	if(mount(buf,USB_MOUNT_DIR,MOUNT_TYPE_UDISK,0,0))
	START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_MOUNT);
	ret = mount(buf,USB_MOUNT_DIR,MOUNT_TYPE_UDISK,0/*MS_SYNCHRONOUS*/,0);
	STOP_WATCHDOG_TIMER_SIG();

	if(ret)
	{
		ERR("UDisk mount %s failed!%s %d\r\n",buf,strerror(errno),errno);
		return -1;
	}
	else
	{
#if (!DEBUG_V2_1)
		if(FakeSetDevStat(USB_MOUNT_DIR))
		{
			ERR("test USB error!\r\n");
			return -1;
		}
#endif
		CLEARBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_REMOVE);

		strcpy(pUISystem->Vol_Disk[DISK_USB],USB_MOUNT_DIR);
		len = strlen(pUISystem->Vol_Disk[DISK_USB]);
		if(pUISystem->Vol_Disk[DISK_USB][len-1]!='/')
		{
			strcat(pUISystem->Vol_Disk[DISK_USB],"/");
		}

		LOGT("UDisk mount success!\r\n");
		return 0;
	}
}

/*
 * Do not need create dir
*/
int SystemMountSDcard(void)
{
	char buf[DEV_PATH_LEN];
	int len;

	FUNC_ENTER;

	sprintf(buf,"/dev/%s",DevPath[DISK_SD]);
	LOGT("mount %s to %s\r\n",buf,SD_MOUNT_DIR);

	if(mount(buf,SD_MOUNT_DIR,MOUNT_TYPE_UDISK,0/*MS_SYNCHRONOUS*/,0))
	{
		ERR("SDCard mount %s failed!%s %d\r\n",buf,strerror(errno),errno);
		return -1;
	}
	else
	{
#if (!DEBUG_V2_1)
		if(FakeSetDevStat(SD_MOUNT_DIR))
		{
			ERR("test SDCard error!\r\n");
			return -1;
		}
#endif
		strcpy(pUISystem->Vol_Disk[DISK_SD],SD_MOUNT_DIR);
		len = strlen(pUISystem->Vol_Disk[DISK_SD]);
		if(pUISystem->Vol_Disk[DISK_SD][len-1]!='/')
		{
			strcat(pUISystem->Vol_Disk[DISK_SD],"/");
		}

		LOGT("SDCard mount success!\r\n");
		CLEARBIT(pUISystem->SysState.DevState,SYS_BIT_SDCard_REMOVE);

		return 0;
	}
}

int SystemGetMountDevPath(char *dev_path,int dev_type)
{
	FUNC_ENTER;

	if(dev_path == NULL)
	{
		LOGE("invalid param\r\n");
		return -1;
	}

	switch(dev_type)
	{
		case DISK_CF:
#if USE_MOUNT_CF_THREAD
            if(TESTBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_REMOVE))
#else
			if(DoCFCardDetect()<0)
#endif
				return -1;
			break;
		case DISK_USB:
			if(DoUDiskDetect()<0)
				return -1;
			break;
		case DISK_SD:
			if(DoSDCardDetect()<0)
				return -1;
			break;
		default:
			ERR("%s: device type error. not support.type_id: %d\r\n",__func__,dev_type);
			return -1;
	}

	sprintf(dev_path,"/dev/%s",DevPath[dev_type]);

	return 0;
}

/********************************************************************************
*wrapper function for access fpga
**********************************************************************************/
#include <sys/mman.h>

//! FPGA 映射之后的基地址指针
char *fpga_base = NULL;

//! FPGA 物理基地址
#define FPGA_BASE                       0X4000000

//! FPGA 地址映射大小
#define FPGA_MAP_SIZE                   0x100000


//! FPGA 视频信号不在位寄存器偏移量
#define VIDEO_LOST_REG_OFFSET           (0x2200<<2)

//! FPGA CF卡不在位寄存器偏移量
#define CF_STATUS_REG_OFFSET            (0x2400<<2)

//! FPGA SD卡不在位寄存器偏移量
#define SD_STATUS_REG_OFFSET            (0x2600<<2)

//! FPGA CF卡复位寄存器偏移量
#define CF_RESET_REG_OFFSET             (0x2800<<2)

//! FPGA HUB复位寄存器偏移量
#define HUB_RESET_REG_OFFSET            (0x2a00<<2)

//! FPGA 看门狗使能寄存器偏移量
#define WATCHDOG_SET2_REG_OFFSET        (0x2c00<<2)

//! FPGA 看门狗喂狗寄存器偏移量
#define WATCHDOG_WDI_REG_OFFSET         (0x2e00<<2)

//! FPGA 视频输出信号切换寄存器偏移量
#define LVDS_SWITCH_REG_OFFSET          (0x3200<<2)

#define pVIDEO_LOST_REG                 (volatile char *)(fpga_base+VIDEO_LOST_REG_OFFSET)
#define pCF_STATUS_REG                  (volatile char *)(fpga_base+CF_STATUS_REG_OFFSET)
#define pSD_STATUS_REG                  (volatile char *)(fpga_base+SD_STATUS_REG_OFFSET)

// #define pFPAG_LED2_REG                  (volatile char *)(fpga_base+FPGA_LED2_REG_OFFSET)
#define pCF_RESET_REG                   (volatile char *)(fpga_base+CF_RESET_REG_OFFSET)
#define pHUB_RESET_REG                  (volatile char *)(fpga_base+HUB_RESET_REG_OFFSET)
#define pWATCHDOG_SET2_REG              (volatile char *)(fpga_base+WATCHDOG_SET2_REG_OFFSET)
#define pWATCHDOG_WDI_REG               (volatile char *)(fpga_base+WATCHDOG_WDI_REG_OFFSET)

#define pLVDS_SWITCH_REG                (volatile char *)(fpga_base+LVDS_SWITCH_REG_OFFSET)

//! 视频信号bypass输出设置
#define LVDS_SW_BYPASS                  0x02
#define LVDS_SW_OUTPUT                  0x01

/**
 * @brief Initial_map_fpga
 * 将FPGA的物理寄存器映射到ARM内存
 * @return
 */
int Initial_map_fpga(void)
{
#if DEBUG_V2_1
	int fd;
	fd = open("/dev/mem",O_RDWR|O_SYNC,0);
	if(fd>0) {
        //将fpga的物理寄存器映射到内存。
		fpga_base = mmap(0,FPGA_MAP_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,
							fd,FPGA_BASE);
		if(((int)fpga_base) == -1) {
			LOGE("failed to map fpga base\r\n");
			fpga_base = NULL;
			return -1;
		}
	}else {
		LOGE("failed to open /dev/mem \r\n");
		return -1;
	}
#endif
	return 1;
}

void Deinital_map_fpga(void)
{
#if DEBUG_V2_1
	if(fpga_base!=NULL) {
		munmap((void *)fpga_base, FPGA_MAP_SIZE);
	}
#endif
}

/**
 * @brief SetLvdsSwitch
 * @param stat 1: switch to bypass mode; 0: switch to dm365 output mode
 * @return
 */
int SetLvdsSwitch(int stat)
{
#if (!DEBUG_V2_1)
	if(stat)
	{
		system("echo 1 > /sys/class/davinci_display/ch0/lvdsswitch");
	}
	else
	{
		system("echo 0 > /sys/class/davinci_display/ch0/lvdsswitch");
	}
#else
	if(stat)
	{
		*pLVDS_SWITCH_REG = LVDS_SW_BYPASS;
	}
	else
	{
		*pLVDS_SWITCH_REG = LVDS_SW_OUTPUT;
	}
#endif
	GUI_SetBkColor(GUI_BLACK);
	GUI_Clear();
	SetRefreshAreaPos(OSDPOS_BASEMENU,0,0,LCD_XSIZE-1,LCD_YSIZE-1,ALPHA_COMMON,TRUE);
	RefreshOSDDisplay();
	pUISystem->nOSDBitMap = 0;

	return stat;
}


//return 1 for in slot,0 for not in slot
int IsCFCardInSlot(void)
{
#if (!DEBUG_V2_1)
    return 1;
#else
	return (int)*pCF_STATUS_REG;
#endif
}

//return 1 for in slot,0 for not in slot
int IsSDCardInSlot(void)
{
#if (!DEBUG_V2_1)
	return 1;
#else
	return (int)*pSD_STATUS_REG;
#endif
}

//check if video input is ok
//return 1 to indicate ok
int IsVideoInputOK(void)
{
#if (!DEBUG_V2_1)
	return 1;
#else
	return (int)*pVIDEO_LOST_REG;
#endif
}

// reset cf
void ResetCFCard(void)
{
#if DEBUG_V2_1
	LOGT("reset cfcard\r\n");
	*pCF_RESET_REG = 1;
	usleep(10000);
	*pCF_RESET_REG = 0;
	usleep(10000);
	*pCF_RESET_REG = 1;
#endif
}

// reset hub
void ResetHUB(void)
{
#if DEBUG_V2_1
	LOGT("reset hub\r\n");
	*pHUB_RESET_REG = 1;
	usleep(10000);
	*pHUB_RESET_REG = 0;
	usleep(10000);
	*pHUB_RESET_REG = 1;
#endif
}

void EnableHardwareWDT(int enable)
{
#ifdef ENABLE_HARDWARE_WATCHDOG
	*pWATCHDOG_SET2_REG = enable;
#endif
}

void FeedHardwareWDT(void)
{
#ifdef ENABLE_HARDWARE_WATCHDOG
	static int stat = 0;
	stat = !stat;
	*pWATCHDOG_WDI_REG = stat;
#endif
}

int IsBoardSupportAudio(void)
{
//use fpga register to get exact information about the board
    return 1;
}

/*
return 0:use mic
          1:use linein
*/ 
int IsAudioInputMic(void)
{
//use fpga register to get exact information about the board
#if 1// AUDIO_TEST_TAG
    return 0;
#endif
}

