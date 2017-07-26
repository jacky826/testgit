#ifndef _OSD_MISC_FUNCS_
#define _OSD_MISC_FUNCS_

//OSD API
int RestoreStaticOSD(TEF_TIMER_STRUCT *pTimer,TEF_OSD_STRUCT *pOSD);
int CloseStaticOSD(int nOSDidx);
int RegisterStaticOSD(int nOSDidx,int nPos,int nMenu,int nTimeInterval,
	OSD_Refresh_fn *Refresh,OSD_Close_fn *Close, OSD_Restore_fn *Restore);

int InitStaticOSD(void);

int RestoreAllStaticOSD(void);
int PauseAllOSD(void);

//Tmp OSD
void DisplayIgnoreKey(void);

void DisplayControl(void);
void DisplayPause(void);
void DisplayStop(void);


//Static  OSD
void DisplayCurState(void);

void DisplayGotoTime(void);

void DisplayABStatus(void);

#endif

