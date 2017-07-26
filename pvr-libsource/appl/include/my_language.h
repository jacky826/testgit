/*
----------------------------------------------------------------------
File		:	My_language.h
Purpose	:
----------------------------------------------------------------------
Version-Date			:	2006.12.12
Author-Explanation		:	Sean.Yu
----------------------------------------------------------------------
*/

#ifndef  __MY_LANGUAGE_DEFINED
#define __MY_LANGUAGE_DEFINED

#include "config.h"
#include "Task_UI/task_ui_var.h"

extern const char *MainMenu[LANGUAGE_CNT];
extern const char *Schedule[LANGUAGE_CNT];
extern const char *ChannelSetting[LANGUAGE_CNT];
extern const char *ChannelScan[LANGUAGE_CNT];
extern const char *AutoScan[LANGUAGE_CNT];
extern const char *ManualScan[LANGUAGE_CNT];
extern const char *RecordSetting[LANGUAGE_CNT];

extern const char *EnterFileBrowser[LANGUAGE_CNT];

extern const char *FileBrowser[LANGUAGE_CNT];
extern const char *SystemSetting[LANGUAGE_CNT];
extern const char *TVSetting[LANGUAGE_CNT];

extern const char *Contrast[LANGUAGE_CNT];
extern const char *Brightness[LANGUAGE_CNT];

extern const char *SystemDate[LANGUAGE_CNT];
extern const char *WeekDay[LANGUAGE_CNT];
extern const char *SystemClock[LANGUAGE_CNT];
extern const char *VideoMode[LANGUAGE_CNT];
extern const char *Language[LANGUAGE_CNT];
extern const char *InputSelect[LANGUAGE_CNT];
extern const char *OutputSelect[LANGUAGE_CNT];
extern const char *Reset[LANGUAGE_CNT];
extern const char *StorageInfo[LANGUAGE_CNT];
extern const char *Format[LANGUAGE_CNT];

//zqh 2015年3月31日 add，
extern const char English_En[];               //英文
extern const char SampleChinese_SiCn[];       //简体中文
extern const char TraditionChinese_TrCn[];    //繁體中文

extern const char *SelectLanguage[LANGUAGE_CNT];

extern const char *Sunday[LANGUAGE_CNT];
extern const char *Monday[LANGUAGE_CNT];
extern const char *Tuesday[LANGUAGE_CNT];
extern const char *Wednesday[LANGUAGE_CNT];
extern const char *Thursday[LANGUAGE_CNT];
extern const char *Friday[LANGUAGE_CNT];
extern const char *Saturday[LANGUAGE_CNT];

extern const char *SUN[LANGUAGE_CNT];
extern const char *MON[LANGUAGE_CNT];
extern const char *TUE[LANGUAGE_CNT];
extern const char *WED[LANGUAGE_CNT];
extern const char *THU[LANGUAGE_CNT];
extern const char *FRI[LANGUAGE_CNT];
extern const char *SAT[LANGUAGE_CNT];

extern const char *SVideo[LANGUAGE_CNT];
extern const char *Cvbs[LANGUAGE_CNT];
extern const char *Cable[LANGUAGE_CNT];
extern const char *Component[LANGUAGE_CNT];

extern const char *PAL_DK[LANGUAGE_CNT];
extern const char *PAL_I[LANGUAGE_CNT];
extern const char *PAL_BG[LANGUAGE_CNT];
extern const char *NTSC[LANGUAGE_CNT];
extern const char *SECAM[LANGUAGE_CNT];

extern const char *Hour24[LANGUAGE_CNT];
extern const char *Hour12[LANGUAGE_CNT];

extern const char *Year[LANGUAGE_CNT];
extern const char *Month[LANGUAGE_CNT];
extern const char *Day[LANGUAGE_CNT];
extern const char *Hour[LANGUAGE_CNT];
extern const char *Minute[LANGUAGE_CNT];
extern const char *Second[LANGUAGE_CNT];

extern const char *HDD[LANGUAGE_CNT];
extern const char *SM[LANGUAGE_CNT];
extern const char *MS[LANGUAGE_CNT];
extern const char *SD[LANGUAGE_CNT];
extern const char *CF[LANGUAGE_CNT];

extern const char *HDDInfo[LANGUAGE_CNT];
extern const char *SMInfo[LANGUAGE_CNT];
extern const char *MSInfo[LANGUAGE_CNT];
extern const char *SDInfo[LANGUAGE_CNT];
extern const char *CFInfo[LANGUAGE_CNT];

extern const char *TotalSpace[LANGUAGE_CNT];
extern const char *UsedSpace[LANGUAGE_CNT];
extern const char *FreeSpace[LANGUAGE_CNT];

extern const char *Number[LANGUAGE_CNT];
extern const char *Source[LANGUAGE_CNT];
extern const char *Qual[LANGUAGE_CNT];
extern const char *Quality[LANGUAGE_CNT];
extern const char *Date[LANGUAGE_CNT];
extern const char *Start[LANGUAGE_CNT];
extern const char *End[LANGUAGE_CNT];
extern const char *RecordTo[LANGUAGE_CNT];
extern const char *Mode[LANGUAGE_CNT];
extern const char *Status[LANGUAGE_CNT];

extern const char *ScheduleDetails[LANGUAGE_CNT];
extern const char *Title[LANGUAGE_CNT];
extern const char *Enable[LANGUAGE_CNT];
extern const char *Disable[LANGUAGE_CNT];
extern const char *Delete[LANGUAGE_CNT];
extern const char *StartTime[LANGUAGE_CNT];
extern const char *EndTime[LANGUAGE_CNT];
extern const char *DeleteAllFile[LANGUAGE_CNT];//haiqing
extern const char *Weekly[LANGUAGE_CNT];
extern const char *Daily[LANGUAGE_CNT];
extern const char *Once[LANGUAGE_CNT];

extern const char *StateEn[LANGUAGE_CNT];
extern const char *StateDis[LANGUAGE_CNT];
extern const char *StateDoing[LANGUAGE_CNT];
extern const char *StateDone[LANGUAGE_CNT];

extern const char *Message[LANGUAGE_CNT];
extern const char *TextInput[LANGUAGE_CNT];
extern const char *Input[LANGUAGE_CNT];
extern const char *FreeCharacter[LANGUAGE_CNT];
extern const char *Wait[LANGUAGE_CNT];

extern const char *FileBrowser_HDD[LANGUAGE_CNT];
extern const char *FileBrowser_SM[LANGUAGE_CNT];
extern const char *FileBrowser_MS[LANGUAGE_CNT];
extern const char *FileBrowser_SD[LANGUAGE_CNT];
extern const char *FileBrowser_CF[LANGUAGE_CNT];
extern const char *FileOperation[LANGUAGE_CNT];

extern const char *Play[LANGUAGE_CNT];
extern const char *Copy[LANGUAGE_CNT];
extern const char *FileRename[LANGUAGE_CNT];
extern const char *Convert[LANGUAGE_CNT];
extern const char *Sort[LANGUAGE_CNT];

extern const char *FileName[LANGUAGE_CNT];
extern const char *FrameRate[LANGUAGE_CNT];
extern const char *RecordInfo[LANGUAGE_CNT];

extern const char *Duration[LANGUAGE_CNT];
extern const char *Size[LANGUAGE_CNT];
extern const char *Page[LANGUAGE_CNT];
extern const char *Time[LANGUAGE_CNT];
extern const char *Name[LANGUAGE_CNT];
extern const char *Type[LANGUAGE_CNT];

extern const char *Default[LANGUAGE_CNT];
extern const char *PSP[LANGUAGE_CNT];
extern const char *PMP[LANGUAGE_CNT];
extern const char *IPOD[LANGUAGE_CNT];
extern const char *PocketPC[LANGUAGE_CNT];
extern const char *DCam[LANGUAGE_CNT];
extern const char *Mobile[LANGUAGE_CNT];
extern const char *AutoMarkerTime[LANGUAGE_CNT];
extern const char *NicamMode[LANGUAGE_CNT];
extern const char *FileFormat[LANGUAGE_CNT];
extern const char *Resolution[LANGUAGE_CNT];

extern const char *GotoTimePlay[LANGUAGE_CNT];
extern const char *MovieLen[LANGUAGE_CNT];
extern const char *ScanWait[LANGUAGE_CNT];

extern const char *ProgramNum[LANGUAGE_CNT];
extern const char *ChannelName[LANGUAGE_CNT];
extern const char *Band[LANGUAGE_CNT];
extern const char *Search[LANGUAGE_CNT];
extern const char *FineTuning[LANGUAGE_CNT];
extern const char *Skip[LANGUAGE_CNT];

extern const char *GotoTime[LANGUAGE_CNT];
extern const char *TotalTime[LANGUAGE_CNT];


//记住修改HELP_TIP_CNT
extern const char *HelpTip0[LANGUAGE_CNT];
extern const char *HelpTip1[LANGUAGE_CNT];
extern const char *HelpTip2[LANGUAGE_CNT];
extern const char *HelpTip3[LANGUAGE_CNT];
extern const char *HelpTip4[LANGUAGE_CNT];
extern const char *HelpTip5[LANGUAGE_CNT];
extern const char *HelpTip6[LANGUAGE_CNT];
//zqh 2015年3月27日 add
extern const char *HelpTip7[LANGUAGE_CNT];
extern const char *HelpTip8[LANGUAGE_CNT];
extern const char *HelpTip9[LANGUAGE_CNT];
extern const char *HelpTip10[LANGUAGE_CNT];
extern const char *HelpTip11[LANGUAGE_CNT];

extern const char *MsgBox1[LANGUAGE_CNT];		//恢复到默认设置，是否继续？
extern const char *MsgBox2[LANGUAGE_CNT];		//挂载文件系统失败！
extern const char *MsgBox3[LANGUAGE_CNT];		//初始化硬盘失败！
extern const char *MsgBox4[LANGUAGE_CNT];		//该设备不存在！
extern const char *MsgBox5[LANGUAGE_CNT];		//格式化硬盘，是否继续？
extern const char *MsgBox6[LANGUAGE_CNT];		//格式化SM卡，是否继续？
extern const char *MsgBox7[LANGUAGE_CNT];		//格式化MS卡，是否继续？
extern const char *MsgBox8[LANGUAGE_CNT];		//格式化SD卡，是否继续？
extern const char *MsgBox9[LANGUAGE_CNT];		//格式化CF卡，是否继续？
//extern const char *MsgBox10[LANGUAGE_CNT];
extern const char *MsgBox11[LANGUAGE_CNT];	//格式化硬盘成功！
extern const char *MsgBox12[LANGUAGE_CNT];	//格式化SM卡 成功！
extern const char *MsgBox13[LANGUAGE_CNT];	//格式化MS卡成功！
extern const char *MsgBox14[LANGUAGE_CNT];	//格式化SD卡成功！
extern const char *MsgBox15[LANGUAGE_CNT];	//格式化CF卡成功！
extern const char *MsgBox16[LANGUAGE_CNT];	//格式化硬盘失败！
extern const char *MsgBox17[LANGUAGE_CNT];	//格式化SM卡 失败！
extern const char *MsgBox18[LANGUAGE_CNT];	//格式化MS卡失败！
extern const char *MsgBox19[LANGUAGE_CNT];	//格式化SD卡失败！
extern const char *MsgBox20[LANGUAGE_CNT];	//格式化CF卡失败！
extern const char *MsgBox21[LANGUAGE_CNT];	//指定的文件与现有文件重名。\n请指定另一文件名。
extern const char *MsgBox22[LANGUAGE_CNT];	//没有视频文件！
extern const char *MsgBox23[LANGUAGE_CNT];	//删除文件成功！
extern const char *MsgBox24[LANGUAGE_CNT];	//删除文件失败！
extern const char *MsgBox25[LANGUAGE_CNT];	//重命名文件成功！
extern const char *MsgBox26[LANGUAGE_CNT];	//重命名文件失败！
extern const char *MsgBox27[LANGUAGE_CNT];	//无法搜台，请将输入切换至Cable模式！
extern const char *MsgBox28[LANGUAGE_CNT];	//搜台结束！
extern const char *MsgBox29[LANGUAGE_CNT];	//设定时间超出影片长度！
extern const char *MsgBox30[LANGUAGE_CNT];	//删除文件，是否继续？
extern const char *MsgBox31[LANGUAGE_CNT];	//录制计划即将启动，2秒后自动切换至录制频道！
extern const char *MsgBox32[LANGUAGE_CNT];	//录制开始！
extern const char *MsgBox33[LANGUAGE_CNT];	//录制结束！
extern const char *MsgBox34[LANGUAGE_CNT];	//存在同名文件，无法复制！
extern const char *MsgBox35[LANGUAGE_CNT];	//文件复制成功！
extern const char *MsgBox36[LANGUAGE_CNT];	//文件复制失败
extern const char *MsgBox37[LANGUAGE_CNT];	//文件复制，是否继续？
extern const char * MsgBox38[LANGUAGE_CNT];	//自动搜台将首先清除频道信息
extern const char *MsgBox39[LANGUAGE_CNT];	//按"Enter"开始搜台
extern const char *MsgBox40[LANGUAGE_CNT];	//当前频段
extern const char *MsgBox41[LANGUAGE_CNT];	//个频道
extern const char *MsgBox42[LANGUAGE_CNT];	//搜索到的台数：
extern const char *MsgBox43[LANGUAGE_CNT];	//恢复默认设置成功
extern const char *MsgBox44[LANGUAGE_CNT];	//恢复默认设置失败
extern const char *MsgBox45[LANGUAGE_CNT];
//zqh 2015年3月25日 add，当删除电子盘上所有文件时，弹出此信息窗口。
extern const char *MsgBox46[LANGUAGE_CNT];  //删除所有文件，是否继续
extern const char *MsgBox47[LANGUAGE_CNT];  //ZQH 2015年4月20日 add, 检查电子盘文件系统，是否继续？
extern const char *MsgBox48[LANGUAGE_CNT];  //zqh add

extern const char *CodecSetting[LANGUAGE_CNT];
extern const char *FullDiskStrategy[LANGUAGE_CNT];
extern const char *MountUDisk[LANGUAGE_CNT];
extern const char *UMountUDisk[LANGUAGE_CNT];
extern const char *FormatSDCard[LANGUAGE_CNT];
extern const char *RestoreSetting[LANGUAGE_CNT];
extern const char *SystemUpdate[LANGUAGE_CNT];
extern const char *CodecLevel[LANGUAGE_CNT];
extern const char *Chroma[LANGUAGE_CNT];
extern const char *DeleteOldFile[LANGUAGE_CNT];
extern const char *StopSave[LANGUAGE_CNT];
extern const char *HighestQuality[LANGUAGE_CNT];
extern const char *HighQuality[LANGUAGE_CNT];
extern const char *MediumQuality[LANGUAGE_CNT];
extern const char *LowQuality[LANGUAGE_CNT];
extern const char *MsgBoxFmtUDisk[LANGUAGE_CNT];
extern const char *MsgBoxFmtUDiskSuc[LANGUAGE_CNT];
extern const char *SDRemoved[LANGUAGE_CNT];
extern const char *SDDetected[LANGUAGE_CNT];
extern const char *UDiskRemoved[LANGUAGE_CNT];
extern const char *UDiskDetected[LANGUAGE_CNT];
extern const char *HelpSDRemove[LANGUAGE_CNT];
extern const char *SDCardFull[LANGUAGE_CNT];
extern const char *UpdataSuc[LANGUAGE_CNT];
extern const char *UpdataFailed[LANGUAGE_CNT];
extern const char *HelpMountUDisk[LANGUAGE_CNT];
extern const char *FileCopy[LANGUAGE_CNT];

//zqh 2015年3月30日 add
extern const char *FileCopyHelpInfo[LANGUAGE_CNT];
extern const char *SystemInfo[LANGUAGE_CNT];
extern const char *KernelUpdata[LANGUAGE_CNT];
extern const char *ubootUpdate[LANGUAGE_CNT];
extern const char *FileSystemUpdate[LANGUAGE_CNT];
extern const char *NoNeedUpdate[LANGUAGE_CNT];

extern const char *PlayBackFinished[LANGUAGE_CNT];
extern const char *PlayBackErr[LANGUAGE_CNT];
extern const char *RecordFailed[LANGUAGE_CNT];

extern const char *UDiskLinkFail[LANGUAGE_CNT];
extern const char *UDiskBreakFail[LANGUAGE_CNT];

extern const char *CFRemoved[LANGUAGE_CNT];
extern const char *CFDetected[LANGUAGE_CNT];

//zqh 2015年3月30日 add
extern const char *SystemFunction[LANGUAGE_CNT];
extern const char *SoftwareVersion[LANGUAGE_CNT];

extern const char *SpecificFile[LANGUAGE_CNT];	// 特定文件不能删除

extern const char *UpdateDaemon[LANGUAGE_CNT];	// 升级daemon程序

//zqh 2015年3月25日 modify
extern const char *NoUpdateFileuImage[LANGUAGE_CNT];// 没有升级文件: uImage
//zqh 2015年3月25日 add
extern const char *NoUpdateFilePvr[LANGUAGE_CNT];//沒有升級文件：pvr
extern const char *NoUpdateFileDaemon[LANGUAGE_CNT];//没有升级文件：daemon
extern const char *DoKernelUpdate[LANGUAGE_CNT]; // 确定升级内核?

extern const char *FileBackUp[LANGUAGE_CNT];
extern const char *FileCopyBack[LANGUAGE_CNT];

extern const char *FileBackUpSuc[LANGUAGE_CNT];
extern const char *FileBackUpFail[LANGUAGE_CNT];

extern const char *FileCopyBackSuc[LANGUAGE_CNT];
extern const char *FileCopyBackFail[LANGUAGE_CNT];

extern const char *FileUpdate[LANGUAGE_CNT];

extern const char *SystemReboot[LANGUAGE_CNT];	// 系统重启

extern const char *CFCardFull[LANGUAGE_CNT];
extern const char *CFCardERR[LANGUAGE_CNT];
extern const char *CFCardOK[LANGUAGE_CNT];

//zqh 2015年3月26日 modify
extern const char *CopyToCFCardName[LANGUAGE_CNT];
extern const char *CopyToCFCard[LANGUAGE_CNT];
extern const char *FormatCFCard[LANGUAGE_CNT];

extern const char *UDiskFileList[LANGUAGE_CNT];

extern const char *CFCardInsert[LANGUAGE_CNT];
extern const char *CFCardNeedFormat[LANGUAGE_CNT];

extern const char *CFCardFsck[LANGUAGE_CNT];
extern const char *CFCardFsckOK[LANGUAGE_CNT];
extern const char *CFCardFsckFail[LANGUAGE_CNT];

extern const char *UpdateSystemTools[LANGUAGE_CNT];
extern const char *UpdateSystemToolsOK[LANGUAGE_CNT];
extern const char *UpdateSystemToolsFail[LANGUAGE_CNT];

extern const char *GetSystemLog[LANGUAGE_CNT];

extern const char *UDiskReadOnly[LANGUAGE_CNT];

extern const char *EnableSystemLog[LANGUAGE_CNT];
extern const char *DisableSystemLog[LANGUAGE_CNT];
extern const char *SetSystemLog[LANGUAGE_CNT];

extern const char *CheckingCFCard[LANGUAGE_CNT];
extern const char *FormattingCFCard[LANGUAGE_CNT];

#endif

