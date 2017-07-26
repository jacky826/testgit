/*
----------------------------------------------------------------------
File		:	My_SPInit.h
Purpose	:
----------------------------------------------------------------------
Version-Date			:	2006.12.12
Author-Explanation		:	Sean.Yu
----------------------------------------------------------------------
*/

#ifndef  __MY_SPINIT_DEFINED
#define __MY_SPINIT_DEFINED

#include "config.h"
#include "Task_UI/task_ui_var.h"

extern const char *WeekDayList[7];
extern const char *SimpleWeekDayList[7];
extern const char *InputSelectList[INPUT_SELECT_CNT];
extern const char *OutputSelectList[OUTPUT_SELECT_CNT];
extern const char *VideoModeList[VIDEO_MODE_CNT];
extern const char *ClockFormatList[2];
extern const char *DateSettingList[3];
extern const char *ClockSettingList[3];
extern const char *StorageList[BUTTON_CNT_STORAGEINFO];
extern const char *StorageInfoList[BUTTON_CNT_STORAGEINFO];
extern const char *SpaceList[3];
//extern const char *HelpTipList[HELP_TIP_CNT];
extern const char *ScheduleRecMode[SCHEDULE_RECORDMODE_CNT];
extern const char *ScheduleRecState[SCHEDULE_RECSTATUS_CNT];
extern const char *FileBrowserStorageList[BUTTON_CNT_STORAGEINFO];
extern const char *FileOperationList[BUTTON_CNT_FILEOPERATION];

//zqh 2015年3月31日 add,添加“语言”菜单选项
extern const char *LanguageList[BUTTON_CNT_LANGUAGE];

extern const char RecFrameRateFull[];


extern const char *FormatContinue[BUTTON_CNT_STORAGEINFO];
extern const char *FormatSuccess[BUTTON_CNT_STORAGEINFO];
extern const char *FormatFailed[BUTTON_CNT_STORAGEINFO];

extern const char *FileBrowserSetList[BUTTON_CNT_FILEBROWSER_SET_LIST];
extern const char *CodecSetLeftList[BUTTON_CNT_CODECSETTING];
extern const char *FullDiskStrategyContent[FULLDISKSTRATEGY_CNT];

extern const char *CodecLevelList[BUTTON_CNT_CODECSETTING_LEFT];

extern const char *SystemUpdateList[BUTTON_CNT_SYSTEMUPDATA];

extern const char *SystemFuncList[BUTTON_CNT_SYSTEMFUNC];

void SPInit_all(void);


#endif

