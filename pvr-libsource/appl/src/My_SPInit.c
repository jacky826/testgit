/*
----------------------------------------------------------------------
File		:	My_SPInit.c
Purpose	:
----------------------------------------------------------------------
Version-Date			:	2006.12.12
Author-Explanation		:	Sean.Yu
----------------------------------------------------------------------
*/
#include <config.h>
#include <Task_UI/task_ui_var.h>

//! 字符串组资源列表，在一组按钮或属性相近的字符串使用时，可以将其归到一组，在调用时根据按钮或者
//! 项目，显示相关信息

// 以下资源未使用
const char *WeekDayList[7];
const char *SimpleWeekDayList[7];
const char *InputSelectList[INPUT_SELECT_CNT];
const char *OutputSelectList[OUTPUT_SELECT_CNT];
const char *VideoModeList[VIDEO_MODE_CNT];
const char *ClockFormatList[2];
const char *DateSettingList[3];
const char *ClockSettingList[3];
const char *StorageList[BUTTON_CNT_STORAGEINFO];
const char *StorageInfoList[BUTTON_CNT_STORAGEINFO];
const char *SpaceList[3];
//const char *HelpTipList[HELP_TIP_CNT];//USE IN PVR Message Box

const char *ScheduleRecMode[SCHEDULE_RECORDMODE_CNT];
const char *ScheduleRecState[SCHEDULE_RECSTATUS_CNT];

// 文件浏览菜单使用的资源
const char *FileBrowserStorageList[BUTTON_CNT_STORAGEINFO]; /// 文件浏览窗口存储介质字符串的列表
const char *FileOperationList[BUTTON_CNT_FILEOPERATION];    /// 文件操作窗口字符串列表

//zqh 2015年3月31日 add，
const char *LanguageList[BUTTON_CNT_LANGUAGE];//语言选择窗口字符列表

const char *FormatContinue[BUTTON_CNT_STORAGEINFO];
const char *FormatSuccess[BUTTON_CNT_STORAGEINFO];
const char *FormatFailed[BUTTON_CNT_STORAGEINFO];

// 系统设置窗口（该窗口已去掉）
const char *FileBrowserSetList[BUTTON_CNT_FILEBROWSER_SET_LIST];
const char *CodecSetLeftList[BUTTON_CNT_CODECSETTING];
const char *FullDiskStrategyContent[FULLDISKSTRATEGY_CNT];

const char *CodecLevelList[BUTTON_CNT_CODECSETTING_LEFT];

// 系统升级窗口
const char *SystemUpdateList[BUTTON_CNT_SYSTEMUPDATA];

// 系统功能窗口
const char *SystemFuncList[BUTTON_CNT_SYSTEMFUNC];

//-------------------------Record Setting Description----------------------//
//Care The following should obey All_Data.h
const char AutoChapterDes_None[] = 
{
	"Disable"
};

const char AutoChapterDes_10[] = 
{
	"10 min"
};

const char AutoChapterDes_20[] = 
{
	"20 min"
};

const char AutoChapterDes_30[] = 
{
	"30 min"
};

const char *AutoChapterDes[DEFREC_AUTOMARKERTIME_CNT]={
	AutoChapterDes_None,//TODO: careful for none
	AutoChapterDes_10,
	AutoChapterDes_20,
	AutoChapterDes_30
};

const char FileFormatDes_AVI[] = 
{
	"AVI"
};

const char FileFormatrDes_3GP[] = 
{
	"3GP"
};

const char FileFormatDes_MOV[] = 
{
	"MOV"
};

const char FileFormatDes_MP4[] = 
{
	"MP4"
};

const char *FileFormatDes[DEFREC_FILEFORMAT_CNT]={
	FileFormatDes_AVI,
	FileFormatrDes_3GP,
	FileFormatDes_MOV,
	FileFormatDes_MP4
};

const char QualityDes_SP[] = 
{
	"SP"
};

const char QualityDes_LP[] = 
{
	"LP"
};

const char QualityDes_EP[] = 
{
	"EP"
};

const char QualityDes_HP[] = 
{
	"HP"
};

const char *QualityDes[DEFREC_QUALITY_CNT]={
	QualityDes_SP,
	QualityDes_LP,
	QualityDes_EP,
	QualityDes_HP
};


const char NicamModeDes_A[] = 
{
	"A"
};

const char NicamModeDes_B[] = 
{
	"B"
};

const char NicamModeDes_DUAL[] = 
{
	"Dual AB"
};


//follow the order defined in config.h
const char RecordToDes_USB[] = 
{
	"UDISK"
};

const char RecordToDes_SD[] = 
{
	"SD"
};

const char *RecordToDes[DEFREC_RECTO_CNT]={
	RecordToDes_SD,
	RecordToDes_USB

};


const char RecFrameRateFull[] = 
{
	"Full"
};


/**
 * @brief SPInit_all
 * (1)初始化显示字符串
 * (2)在"开机\切换语言\复位"时调用
 * 在各个窗口按键列表会用到相关资源
 */
void SPInit_all(void)
{
	WeekDayList[0] = Sunday[pUISystem->allData.Language];
	WeekDayList[1] = Monday[pUISystem->allData.Language];
	WeekDayList[2] = Tuesday[pUISystem->allData.Language];
	WeekDayList[3] = Wednesday[pUISystem->allData.Language];
	WeekDayList[4] = Thursday[pUISystem->allData.Language];
	WeekDayList[5] = Friday[pUISystem->allData.Language];
	WeekDayList[6] = Saturday[pUISystem->allData.Language];

	SimpleWeekDayList[0] = SUN[pUISystem->allData.Language];
	SimpleWeekDayList[1] = MON[pUISystem->allData.Language];
	SimpleWeekDayList[2] = TUE[pUISystem->allData.Language];
	SimpleWeekDayList[3] = WED[pUISystem->allData.Language];
	SimpleWeekDayList[4] = THU[pUISystem->allData.Language];
	SimpleWeekDayList[5] = FRI[pUISystem->allData.Language];
	SimpleWeekDayList[6] = SAT[pUISystem->allData.Language];

	InputSelectList[0] = SVideo[pUISystem->allData.Language];
	InputSelectList[1] = Cvbs[pUISystem->allData.Language];
	InputSelectList[2] = Cable[pUISystem->allData.Language];

	OutputSelectList[0] = SVideo[pUISystem->allData.Language];
	OutputSelectList[1] = Cvbs[pUISystem->allData.Language];
	OutputSelectList[2] = Cable[pUISystem->allData.Language];

	VideoModeList[0] = PAL_DK[pUISystem->allData.Language];
	VideoModeList[1] = PAL_I[pUISystem->allData.Language];
	VideoModeList[2] = PAL_BG[pUISystem->allData.Language];
	VideoModeList[3] = NTSC[pUISystem->allData.Language];
	VideoModeList[4] = SECAM[pUISystem->allData.Language];	

	ClockFormatList [0] = Hour24[pUISystem->allData.Language];
	ClockFormatList [1] = Hour12[pUISystem->allData.Language];

	DateSettingList[0] = Year[pUISystem->allData.Language];
	DateSettingList[1] = Month[pUISystem->allData.Language];
	DateSettingList[2] = Day[pUISystem->allData.Language];

	ClockSettingList[0] = Hour[pUISystem->allData.Language];
	ClockSettingList[1] = Minute[pUISystem->allData.Language];
	ClockSettingList[2] = Second[pUISystem->allData.Language];

	StorageList[0] = HDD[pUISystem->allData.Language];
	StorageList[1] = SM[pUISystem->allData.Language];
	StorageList[2] = MS[pUISystem->allData.Language];
	StorageList[3] = SD[pUISystem->allData.Language];
	StorageList[4] = CF[pUISystem->allData.Language];	

	StorageInfoList[0] = HDDInfo[pUISystem->allData.Language];
	StorageInfoList[1] = SMInfo[pUISystem->allData.Language];
	StorageInfoList[2] = MSInfo[pUISystem->allData.Language];
	StorageInfoList[3] = SDInfo[pUISystem->allData.Language];
    StorageInfoList[4] = CFInfo[pUISystem->allData.Language];

	SpaceList[0] = TotalSpace[pUISystem->allData.Language];
	SpaceList[1] = UsedSpace[pUISystem->allData.Language];
	SpaceList[2] = FreeSpace[pUISystem->allData.Language];

	ScheduleRecMode[0] = Weekly[pUISystem->allData.Language];
	ScheduleRecMode[1] = Daily[pUISystem->allData.Language];
	ScheduleRecMode[2] = Once[pUISystem->allData.Language];

	ScheduleRecState[0] = HelpTip0[pUISystem->allData.Language];
	ScheduleRecState[1] = StateEn[pUISystem->allData.Language];
	ScheduleRecState[2] = StateDis[pUISystem->allData.Language];
	ScheduleRecState[3] = StateDoing[pUISystem->allData.Language];
	ScheduleRecState[4] = StateDone[pUISystem->allData.Language];


    FileBrowserStorageList[0] = FileBrowser_CF[pUISystem->allData.Language];
    FileBrowserStorageList[1] = FileBrowser_HDD[pUISystem->allData.Language];
    FileBrowserStorageList[2] = FileBrowser_SD[pUISystem->allData.Language];

	FileOperationList[0] = Play[pUISystem->allData.Language];
	FileOperationList[1] = Delete[pUISystem->allData.Language];
	FileOperationList[2] = Copy[pUISystem->allData.Language];
    FileOperationList[3] = DeleteAllFile[pUISystem->allData.Language];//haiqing

	FormatContinue[DISK_USB]=MsgBox5[pUISystem->allData.Language];
	FormatContinue[DISK_SD]=MsgBox8[pUISystem->allData.Language];


	FormatSuccess[DISK_USB]=MsgBox11[pUISystem->allData.Language];
	FormatSuccess[DISK_SD]=MsgBox14[pUISystem->allData.Language];

	
	FormatFailed[DISK_USB]=MsgBox16[pUISystem->allData.Language];
	FormatFailed[DISK_SD]=MsgBox19[pUISystem->allData.Language];

	FileBrowserSetList[0] = MountUDisk[pUISystem->allData.Language];

	FullDiskStrategyContent[0] = DeleteOldFile[pUISystem->allData.Language];
	FullDiskStrategyContent[1] = StopSave[pUISystem->allData.Language];

	CodecSetLeftList[0] = CodecLevel[pUISystem->allData.Language];
	CodecSetLeftList[1] = Brightness[pUISystem->allData.Language];
	CodecSetLeftList[2] = Chroma[pUISystem->allData.Language];
	CodecSetLeftList[3] = Contrast[pUISystem->allData.Language];

	CodecLevelList[0] = LowQuality[pUISystem->allData.Language];
	CodecLevelList[1] = MediumQuality[pUISystem->allData.Language];
	CodecLevelList[2] = HighQuality[pUISystem->allData.Language];
	CodecLevelList[3] = HighestQuality[pUISystem->allData.Language];

//System update
	SystemUpdateList[0] = ubootUpdate[pUISystem->allData.Language];
	SystemUpdateList[1] = KernelUpdata[pUISystem->allData.Language];
	SystemUpdateList[2] = FileSystemUpdate[pUISystem->allData.Language];
	SystemUpdateList[3] = UpdateDaemon[pUISystem->allData.Language];
	SystemUpdateList[4] = SystemInfo[pUISystem->allData.Language];

    //zqh 2015年3月31日 add， 添加“语言选择”界面菜单
    LanguageList[0] = English_En;
    LanguageList[1] = SampleChinese_SiCn;
    LanguageList[2] = TraditionChinese_TrCn;

// System func
    //zqh 2015年3月24日 modify，将“获取系统日志”及“开启/关闭日志”功能去掉
	SystemFuncList[0] = CopyToCFCardName[pUISystem->allData.Language];
    //SystemFuncList[1] = GetSystemLog[pUISystem->allData.Language];
    SystemFuncList[1] = FormatCFCard[pUISystem->allData.Language];
    SystemFuncList[2] = CFCardFsck[pUISystem->allData.Language];
    //zqh 2015年3月31日 add，添加“语言选择”功能
    SystemFuncList[3] = Language[pUISystem->allData.Language];

#if DEBUG_V2_1
    //SystemFuncList[4] = SetSystemLog[pUISystem->allData.Language];
#else
	SystemFuncList[4] = UpdateSystemTools[pUISystem->allData.Language];
#endif
}


