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

//zqh 2015��3��31�� add��
extern const char English_En[];               //Ӣ��
extern const char SampleChinese_SiCn[];       //��������
extern const char TraditionChinese_TrCn[];    //���w����

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


//��ס�޸�HELP_TIP_CNT
extern const char *HelpTip0[LANGUAGE_CNT];
extern const char *HelpTip1[LANGUAGE_CNT];
extern const char *HelpTip2[LANGUAGE_CNT];
extern const char *HelpTip3[LANGUAGE_CNT];
extern const char *HelpTip4[LANGUAGE_CNT];
extern const char *HelpTip5[LANGUAGE_CNT];
extern const char *HelpTip6[LANGUAGE_CNT];
//zqh 2015��3��27�� add
extern const char *HelpTip7[LANGUAGE_CNT];
extern const char *HelpTip8[LANGUAGE_CNT];
extern const char *HelpTip9[LANGUAGE_CNT];
extern const char *HelpTip10[LANGUAGE_CNT];
extern const char *HelpTip11[LANGUAGE_CNT];

extern const char *MsgBox1[LANGUAGE_CNT];		//�ָ���Ĭ�����ã��Ƿ������
extern const char *MsgBox2[LANGUAGE_CNT];		//�����ļ�ϵͳʧ�ܣ�
extern const char *MsgBox3[LANGUAGE_CNT];		//��ʼ��Ӳ��ʧ�ܣ�
extern const char *MsgBox4[LANGUAGE_CNT];		//���豸�����ڣ�
extern const char *MsgBox5[LANGUAGE_CNT];		//��ʽ��Ӳ�̣��Ƿ������
extern const char *MsgBox6[LANGUAGE_CNT];		//��ʽ��SM�����Ƿ������
extern const char *MsgBox7[LANGUAGE_CNT];		//��ʽ��MS�����Ƿ������
extern const char *MsgBox8[LANGUAGE_CNT];		//��ʽ��SD�����Ƿ������
extern const char *MsgBox9[LANGUAGE_CNT];		//��ʽ��CF�����Ƿ������
//extern const char *MsgBox10[LANGUAGE_CNT];
extern const char *MsgBox11[LANGUAGE_CNT];	//��ʽ��Ӳ�̳ɹ���
extern const char *MsgBox12[LANGUAGE_CNT];	//��ʽ��SM�� �ɹ���
extern const char *MsgBox13[LANGUAGE_CNT];	//��ʽ��MS���ɹ���
extern const char *MsgBox14[LANGUAGE_CNT];	//��ʽ��SD���ɹ���
extern const char *MsgBox15[LANGUAGE_CNT];	//��ʽ��CF���ɹ���
extern const char *MsgBox16[LANGUAGE_CNT];	//��ʽ��Ӳ��ʧ�ܣ�
extern const char *MsgBox17[LANGUAGE_CNT];	//��ʽ��SM�� ʧ�ܣ�
extern const char *MsgBox18[LANGUAGE_CNT];	//��ʽ��MS��ʧ�ܣ�
extern const char *MsgBox19[LANGUAGE_CNT];	//��ʽ��SD��ʧ�ܣ�
extern const char *MsgBox20[LANGUAGE_CNT];	//��ʽ��CF��ʧ�ܣ�
extern const char *MsgBox21[LANGUAGE_CNT];	//ָ�����ļ��������ļ�������\n��ָ����һ�ļ�����
extern const char *MsgBox22[LANGUAGE_CNT];	//û����Ƶ�ļ���
extern const char *MsgBox23[LANGUAGE_CNT];	//ɾ���ļ��ɹ���
extern const char *MsgBox24[LANGUAGE_CNT];	//ɾ���ļ�ʧ�ܣ�
extern const char *MsgBox25[LANGUAGE_CNT];	//�������ļ��ɹ���
extern const char *MsgBox26[LANGUAGE_CNT];	//�������ļ�ʧ�ܣ�
extern const char *MsgBox27[LANGUAGE_CNT];	//�޷���̨���뽫�����л���Cableģʽ��
extern const char *MsgBox28[LANGUAGE_CNT];	//��̨������
extern const char *MsgBox29[LANGUAGE_CNT];	//�趨ʱ�䳬��ӰƬ���ȣ�
extern const char *MsgBox30[LANGUAGE_CNT];	//ɾ���ļ����Ƿ������
extern const char *MsgBox31[LANGUAGE_CNT];	//¼�Ƽƻ�����������2����Զ��л���¼��Ƶ����
extern const char *MsgBox32[LANGUAGE_CNT];	//¼�ƿ�ʼ��
extern const char *MsgBox33[LANGUAGE_CNT];	//¼�ƽ�����
extern const char *MsgBox34[LANGUAGE_CNT];	//����ͬ���ļ����޷����ƣ�
extern const char *MsgBox35[LANGUAGE_CNT];	//�ļ����Ƴɹ���
extern const char *MsgBox36[LANGUAGE_CNT];	//�ļ�����ʧ��
extern const char *MsgBox37[LANGUAGE_CNT];	//�ļ����ƣ��Ƿ������
extern const char * MsgBox38[LANGUAGE_CNT];	//�Զ���̨���������Ƶ����Ϣ
extern const char *MsgBox39[LANGUAGE_CNT];	//��"Enter"��ʼ��̨
extern const char *MsgBox40[LANGUAGE_CNT];	//��ǰƵ��
extern const char *MsgBox41[LANGUAGE_CNT];	//��Ƶ��
extern const char *MsgBox42[LANGUAGE_CNT];	//��������̨����
extern const char *MsgBox43[LANGUAGE_CNT];	//�ָ�Ĭ�����óɹ�
extern const char *MsgBox44[LANGUAGE_CNT];	//�ָ�Ĭ������ʧ��
extern const char *MsgBox45[LANGUAGE_CNT];
//zqh 2015��3��25�� add����ɾ���������������ļ�ʱ����������Ϣ���ڡ�
extern const char *MsgBox46[LANGUAGE_CNT];  //ɾ�������ļ����Ƿ����
extern const char *MsgBox47[LANGUAGE_CNT];  //ZQH 2015��4��20�� add, ���������ļ�ϵͳ���Ƿ������
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

//zqh 2015��3��30�� add
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

//zqh 2015��3��30�� add
extern const char *SystemFunction[LANGUAGE_CNT];
extern const char *SoftwareVersion[LANGUAGE_CNT];

extern const char *SpecificFile[LANGUAGE_CNT];	// �ض��ļ�����ɾ��

extern const char *UpdateDaemon[LANGUAGE_CNT];	// ����daemon����

//zqh 2015��3��25�� modify
extern const char *NoUpdateFileuImage[LANGUAGE_CNT];// û�������ļ�: uImage
//zqh 2015��3��25�� add
extern const char *NoUpdateFilePvr[LANGUAGE_CNT];//�]�������ļ���pvr
extern const char *NoUpdateFileDaemon[LANGUAGE_CNT];//û�������ļ���daemon
extern const char *DoKernelUpdate[LANGUAGE_CNT]; // ȷ�������ں�?

extern const char *FileBackUp[LANGUAGE_CNT];
extern const char *FileCopyBack[LANGUAGE_CNT];

extern const char *FileBackUpSuc[LANGUAGE_CNT];
extern const char *FileBackUpFail[LANGUAGE_CNT];

extern const char *FileCopyBackSuc[LANGUAGE_CNT];
extern const char *FileCopyBackFail[LANGUAGE_CNT];

extern const char *FileUpdate[LANGUAGE_CNT];

extern const char *SystemReboot[LANGUAGE_CNT];	// ϵͳ����

extern const char *CFCardFull[LANGUAGE_CNT];
extern const char *CFCardERR[LANGUAGE_CNT];
extern const char *CFCardOK[LANGUAGE_CNT];

//zqh 2015��3��26�� modify
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

