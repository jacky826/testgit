#ifndef _UI_MISC_FUNC_HEAD_
#define _UI_MISC_FUNC_HEAD_

/*
 *Functions including:
 * Record, change channel, change volum
 * change video/auido input, change video/auido output
 */
#include "../../avi/pvr_error.h"
#include "../pvr_types.h"
#include "../RTC.h"
#include "task_ui_var.h"
#include "../../../pvr_platform.h"


#define ERR_SYSSTATE	(1)
#define ERR_DISK			(2)
#define ERR_PARAMETER	(3)

/*Decoder /Encoder Part*/

int StartRecord(int idx);
int StopRecord(void);
void ClearRecordState(void);

int StartPlayFile(char *fileName);
void ClearPlayState(void);
void ShowDecoderMessage(unsigned short nMsgData);
void ShowEncoderMessage(unsigned short nMsgData);


//int MenuKEY2Num(unsigned short nMsgData);


void Pvr_AllData_Init(void);

void LoadAllDataFlash(void);
void SaveAllDataFlash(void);

int GetCurrentDateTime(DATE_TIME *pTime);
int SetCurrentDateTime(DATE_TIME *pTime);

int DoParseMUSBStatus(void);
int ParseMUSBStatus(void);
void StartParseMUSBStatus(int time_out);
void StopParseMUSBStatus(void);

#define ERR_CF_MOUNT_OK     (0)
#define ERR_CF_MOUNT_ERR    (-1)
#define ERR_CF_NOT_IN       (-2)
#define ERR_CF_RESET        (-3)
#define ERR_CF_FORMAT       (-4)
#define ERR_CF_NEED_MOUNT   (-5)
int ResetCFDevice(void);

int DoCFCardDetect(void);
int DoUDiskDetect(void);
int DoSDCardDetect(void);

int SystemMountSDcard(void);
int SystemMountUDisk(void);

int DoMountCFCard(void);
int SystemMountCFCard(void);

//int DoUpdataFile(char *filename);
//int CheckUpdateFile(char *filename);
TMsgHandleStat System_DevScanTimer_TO(TEF_TIMER_STRUCT *pTimer);

//Schedule Part
int GetResolutionDesAppix(int nResX,int nResY);
int GetResolutionDes(char *Buf,int nBuflen,int nRes,int nVideoMode);

#endif

