#include <stdio.h>
#include <string.h>

#include <config.h>	//Config for platform
#include <UI_Message.h>	//UI message
#include "UserMsg.h"

#include <Task_UI/task_ui_var.h>
#include <Task_UI/pvr_ui_func.h>
#include <GUI.H>

//#include <decoder.h>

/*
 ----	Position for OSD 
 */
#define STATUS_PIC_SIZE			(32)
#define WORD_WIDTH				(20)
#define WORD_HEIGHT			(20)

//OSD Display Area
#define OSD_AREA_X0			(STATUS_PIC_SIZE*2)				//OSD Area Left position
#define OSD_AREA_X1			(LCD_XSIZE-STATUS_PIC_SIZE*2)	//OSD Area Right position 
#define OSD_AREA_Y0			(STATUS_PIC_SIZE)				//OSD Area Top position
#define OSD_AREA_Y1			(LCD_YSIZE-40)					//OSD Area Bottom position

//--Dynamic OSD	//Use for Channel Name & Volumn Display at Center Bottom
#define DOSD_X0					(OSD_AREA_X0+190)			//At center
#define DOSD_Y1					(OSD_AREA_Y1)	//At Bottom
//TODO: Y0 & X1 need decide by BMP'size
//#define DOSD_Y0					(OSD_AREA_Y1-STATUS_PIC_SIZE)
//#define DOSD_X1					(OSD_AREA_X0-200)

//--OSD Positon Top Right	for Notify message & state at Right Top
#define NOTIFY_X0  				(LCD_XSIZE-STATUS_PIC_SIZE*4)
#define NOTIFY_Y0				(STATUS_PIC_SIZE)
#define NOTIFY_X1				(LCD_XSIZE-STATUS_PIC_SIZE*2)
#define NOTIFY_Y1				(STATUS_PIC_SIZE*2)
#define NOTIFY_WIDTH			(NOTIFY_X1-NOTIFY_X0)
#define NOTIFY_HEIGHT			(NOTIFY_Y1-NOTIFY_Y0)

//--OSD Positon Top Right
#define STATUS_WIDTH	(200)
#define STATUS_HEIGHT	(NOTIFY_HEIGHT)
#define STATUS_X0  	(NOTIFY_X0-STATUS_WIDTH)
#define STATUS_Y0	(NOTIFY_Y0)
#define STATUS_X1	(NOTIFY_X0)
#define STATUS_Y1	(NOTIFY_Y1)


//--OSD Posion Top left
#define CUR_STATUS_WIDTH		(160)
#define CUR_STATUS_HEIGHT		(30)
#define CUR_STATUS_X0_DEFAULT   (780)//(LCD_XSIZE-CUR_STATUS_WIDTH-180)//40)
#define CUR_STATUS_INTERVAL		(8)
#define CUR_STATUS_Y0_DEFAULT   (155)//(20)//(30)
#define CUR_STATUS_X1			(CUR_STATUS_X0)
#define CUR_STATUS_Y1			(CUR_STATUS_Y0+CUR_STATUS_HEIGHT)
#define CUR_STATUS_X2			(CUR_STATUS_X0)
#define CUR_STATUS_Y2			(CUR_STATUS_Y1+CUR_STATUS_HEIGHT)
#define CUR_STATUS_X3			(CUR_STATUS_X0)
#define CUR_STATUS_Y3			(CUR_STATUS_Y2+CUR_STATUS_HEIGHT)

// 0121-2013 添加图标坐标可变功能
unsigned short CUR_STATUS_X0 = CUR_STATUS_X0_DEFAULT;
unsigned short CUR_STATUS_Y0 = CUR_STATUS_Y0_DEFAULT;

/*
 ----	Functions for OSD handle
 */
 /*OSD manage functions*/
TMsgHandleStat OSDTimeOut_Handle(TEF_TIMER_STRUCT *pTimer);
int RestoreOSDPos(int nPos);

void ReleaseOSDTimer(int nPos,TEF_OSD_STRUCT *pOSD){
	int i;
	int *pOSDIdx;
	TEF_OSD_STRUCT *pCheckOSD;
	if (pOSD){
		//Perform the Close 
		if (VALIDFUNCTION(pOSD->OSD_Close)){
			pOSD->OSD_Close(pOSD->pUser);
		}
		//Delete from OSD show queue
		pOSDIdx=&(PGETMENU(pOSD->nMenu)->nShowOSDIdx);
		for (i=0;i<UI_OSD_COUNT;i++){
			if (VALIDOSDIDX(*pOSDIdx)){
				pCheckOSD=&(pUISystem->sOSDStructStatic[*pOSDIdx]);
				if ((unsigned int)(pCheckOSD)==(unsigned int)(pOSD)){
					*pOSDIdx=pOSD->nNextOSD;
					pOSD->nNextOSD=DVR_OSD_DIS;
					RestoreOSDPos(nPos);
					return ;
				}else{
					pOSDIdx=&(pCheckOSD->nNextOSD);
				}
			}else{
				break;
			}
		}
	}	
	pUISystem->nOSDBitMap&=~(1<<nPos);
	DisableTimer(&(pUISystem->sOSDTimer[nPos]));
}

int RestoreStaticOSD(TEF_TIMER_STRUCT *pTimer,TEF_OSD_STRUCT *pOSD){
	int retval=-1;
	if(pOSD == NULL)//SACH:W10863
	{
		return -1;
	}
	if ((pOSD->nMenu!=pUISystem->nMenuIdx)){//SACH:W10863
		pUISystem->nOSDBitMap&=~(1<<pOSD->nPos);
		return -1;
	}
	//Call Restore Function
	pUISystem->nOSDBitMap|=(1<<pOSD->nPos);
	if (VALIDFUNCTION(pOSD->OSD_Restore)){

		retval=pOSD->OSD_Restore(pOSD->pUser);
	}else if (VALIDFUNCTION(pOSD->OSD_Refresh)){

		retval=pOSD->OSD_Refresh(pOSD->pUser);
	}
	//Set Timer
	if (retval>=0){
		if (pOSD->TimerInterval>0){
			EnableTimer(pTimer,0,(TIMER_OSDDISPLAY_STATIC|((pOSD->nPos)<<16)),
				pOSD->TimerInterval,OSDTimeOut_Handle);
		}	
	}

	if (retval<0){//No osd display
		pUISystem->nOSDBitMap&=~(1<<pOSD->nPos);
	}
	return retval;
}

int RefreshStaticOSD(TEF_TIMER_STRUCT *pTimer,TEF_OSD_STRUCT *pOSD){
	//Call Restore Function
	int retval=-1;
	if (!pOSD){
		return -1;
	}
	if (VALIDFUNCTION(pOSD->OSD_Refresh)){
		retval=pOSD->OSD_Refresh(pOSD->pUser);
	}else if (VALIDFUNCTION(pOSD->OSD_Restore)){

		retval=pOSD->OSD_Restore(pOSD->pUser);
	}
	return retval;	
}

TEF_OSD_STRUCT *FindActiveOSD(int nPos){
	int i;
	int nCheckOSD;
	TEF_OSD_STRUCT *pOSD;

	nCheckOSD=PACTIVE_MENU->nShowOSDIdx;
	for (i=0;i<OSDPOS_COUNT;i++){
		if (VALIDOSDIDX(nCheckOSD)){
			pOSD=&(pUISystem->sOSDStructStatic[nCheckOSD]);
			if( pOSD->nPos==nPos){
				return pOSD;
			}
			nCheckOSD=pOSD->nNextOSD;
		}else{
			return NULL;
		}
	}
	return NULL;
}

int RefreshOSDPos(int nPos){
	TEF_OSD_STRUCT *pActiveOSD;
	TEF_TIMER_STRUCT *pTimer=&(pUISystem->sOSDTimer[nPos]);
	pActiveOSD=FindActiveOSD(nPos);
	if (pActiveOSD!=NULL){
		if (RefreshStaticOSD(pTimer,pActiveOSD)<0){
			ReleaseOSDTimer(nPos,pActiveOSD);	
			return -1;
		}
		return 1;
	}else{
		ReleaseOSDTimer(nPos,NULL);	
	}
	return -1;
}

int RestoreOSDPos(int nPos){
	TEF_OSD_STRUCT *pActiveOSD;
	TEF_TIMER_STRUCT *pTimer=&(pUISystem->sOSDTimer[nPos]);
	pActiveOSD=FindActiveOSD(nPos);
	if (pActiveOSD!=NULL){
		if (RestoreStaticOSD(pTimer, pActiveOSD)<0){
			ReleaseOSDTimer(nPos, pActiveOSD);	
			return -1;
		}
		return 1;
	}else{//Never should happen
		ReleaseOSDTimer(nPos,NULL);	
	}
	return -1;
}
 
TMsgHandleStat OSDTimeOut_Handle(TEF_TIMER_STRUCT *pTimer){
	int nTimerStat=(pTimer->TimerStatus)&0xff;
	int nTimerUser=(pTimer->TimerStatus)>>16;
	
	if (nTimerStat==TIMER_OSDDISPLAY_TMP){
		RestoreOSDPos(nTimerUser);
	}else if(nTimerStat==TIMER_OSDDISPLAY_STATIC) {//Static
		RefreshOSDPos(nTimerUser);
	}else{//Never should happen
		DisableTimer(pTimer);
	}
	RefreshOSDDisplay();
	return MsgHandled;
}

int StopTmpOSD(void){
	FUNC_ENTER;

    // 使用清屏的方法处理，否则有一个缺角
	pUISystem->nOSDBitMap=0;
	GUI_SetBkColor(GUI_BLACK);

	GUI_Clear();
	return 1;
 }
int StartTmpOSD(int nPos,int nInterval){
	if (nInterval==0){
		pUISystem->nOSDBitMap&=~(1<<nPos);
		return -1;
	}
//Disable Timer 
//	EnableTimer(&(pUISystem->sOSDTimer[nPos]),0,(TIMER_OSDDISPLAY_TMP)|(nPos<<16),
//		nInterval,OSDTimeOut_Handle);
	RefreshOSDDisplay();
	return 1;
}

int StartStaticOSD(int nOSDidx,int nInterval,void *pUser){
	if (!VALIDOSDIDX(nOSDidx)){
		return -1;//Never should happen
	}	
    //NOTE: perform a check on Menu, seem no need and what if we open a Display in other menu
	int i;
	int nMenu=pUISystem->sOSDStructStatic[nOSDidx].nMenu;
	TEF_OSD_STRUCT *pOSD=&(pUISystem->sOSDStructStatic[nOSDidx]);
	int *pPreOSD;
	TEF_OSD_STRUCT *pCheckOSD;
	pPreOSD=&(PGETMENU(nMenu)->nShowOSDIdx);
	for (i=0;i<UI_OSD_COUNT;i++){
		if (VALIDOSDIDX(*pPreOSD)){
			pCheckOSD=&(pUISystem->sOSDStructStatic[*pPreOSD]);
			if (nOSDidx==(*pPreOSD)){
				//Find the Same delete from the Queue
				*pPreOSD=pCheckOSD->nNextOSD;
				pOSD->nNextOSD=DVR_OSD_DIS;
				break;
			}
			pPreOSD=&(pCheckOSD->nNextOSD);
		}else{//End
			break;
		}
	}

	pPreOSD=&(PGETMENU(nMenu)->nShowOSDIdx);
	//Need find if we covered 
	for (i=0;i<UI_OSD_COUNT;i++){
		if (VALIDOSDIDX(*pPreOSD)){
			pCheckOSD=&(pUISystem->sOSDStructStatic[*pPreOSD]);
			if (pCheckOSD->nPos>=pOSD->nPos){
				//OK insert Here
				break;
			}
			pPreOSD=&(pCheckOSD->nNextOSD);
		}else{
			//OK insert Here,Last One
			break;
		}
	}
	//Insert to the quene
	pOSD->nNextOSD=*pPreOSD;
	*pPreOSD=nOSDidx;
	

	pOSD->TimerInterval=nInterval;
	pOSD->pUser=pUser;

	if (nInterval>0){
		EnableTimer(&(pUISystem->sOSDTimer[pOSD->nPos]),0,(TIMER_OSDDISPLAY_STATIC|((pOSD->nPos)<<16)),
			nInterval,OSDTimeOut_Handle);//No need close 
	}
	pOSD->OSD_Restore(pUser);
	pUISystem->nOSDBitMap|=(1<<pOSD->nPos);//Enable refresh the OSD
	//Do Display Blend refresh
	RefreshOSDDisplay();
 	return 1;
}

int CloseStaticOSD(int nOSDidx){
	ReleaseOSDTimer(pUISystem->sOSDStructStatic[nOSDidx].nPos,&pUISystem->sOSDStructStatic[nOSDidx]);
	RefreshOSDDisplay();
 	return 1;
}

int RegisterStaticOSD(int nOSDidx,int nMenu,int nPos,int nTimeInterval,
	OSD_Refresh_fn *Refresh,OSD_Close_fn *Close, OSD_Restore_fn *Restore){
	
	if (!VALIDOSDIDX(nOSDidx)){
		return -1;//Never should happen
	}	
	if ((!VALIDFUNCTION(Refresh))&&(!(VALIDFUNCTION(Restore)))){
		return -1;
	}	

	if (!VALIDMENU(nMenu)){
		return -1;
	}
	if (!VALIDPOSITION(nPos)){
		return -1;
	}
	pUISystem->sOSDStructStatic[nOSDidx].OSD_Refresh=Refresh;
	pUISystem->sOSDStructStatic[nOSDidx].OSD_Close=Close;
	pUISystem->sOSDStructStatic[nOSDidx].OSD_Restore=Restore;
	
	pUISystem->sOSDStructStatic[nOSDidx].nPos=nPos;
	pUISystem->sOSDStructStatic[nOSDidx].nMenu=nMenu;

	//For a tmp start value , can modify at run time
	pUISystem->sOSDStructStatic[nOSDidx].TimerInterval=nTimeInterval;
	pUISystem->sOSDStructStatic[nOSDidx].pUser=NULL;
	
	pUISystem->sOSDStructStatic[nOSDidx].nNextOSD=DVR_OSD_DIS;
	return 1;
}

int InitStaticOSD(void){
	int i;
	for (i=0;i<UI_MENU_COUNT;i++){
		pUISystem->Menus[i].nShowOSDIdx=DVR_OSD_DIS;
	}
	pUISystem->nOSDBitMap=0;
	
	RegisterStaticOSD(OSD_BLANK_DISPLAY,MENUBLANK,OSD_BLANK_DISPLAY_POS,OSD_BLANK_DISPLAY_INTERVAL,
		DoDisplayCurStatRefresh,NULL,DoDisplayCurStatRestore);
	
	RegisterStaticOSD(OSD_BLANK_RECORD,MENUBLANK,OSD_BLANK_RECORD_INTERVAL,OSD_BLANK_RECORD_INTERVAL,
		NULL,NULL,DoDisplayRecStaus);
	
	RegisterStaticOSD(OSD_PLAY_DISPLAY,MENUPLAYBACK,OSD_PLAY_DISPLAY_POS,OSD_PLAY_DISPLAY_INTERVAL,
		DoDisplayCurStatRefresh,NULL,DoDisplayCurStatRestore);

	return 1;	
}

int PauseAllOSD(void){
	int i;
	for (i=0;i<OSDPOS_COUNT;i++){
		DisableTimer(&(pUISystem->sOSDTimer[i]));
	}
	return 1;
}

int RestoreAllStaticOSD(void){//Must call from active menu
	int i;
	int *pPreOSD;
	TEF_OSD_STRUCT *pCheckOSD;
	int nLastPos=-1;//A  invalid postition

	pPreOSD=&(PACTIVE_MENU->nShowOSDIdx);
	//Need find if we covered 
	for (i=0;i<UI_OSD_COUNT;i++){
		if (VALIDOSDIDX(*pPreOSD)){
			pCheckOSD=&(pUISystem->sOSDStructStatic[*pPreOSD]);
			if (pCheckOSD->nPos!=nLastPos){
				if (RestoreStaticOSD(&(pUISystem->sOSDTimer[pCheckOSD->nPos]),pCheckOSD)>0){
					nLastPos=pCheckOSD->nPos;
					pPreOSD=&(pCheckOSD->nNextOSD);//Goto Next;
				}else{//restore fail delete the OSD,check again
					*pPreOSD=pCheckOSD->nNextOSD;
					pCheckOSD->nNextOSD=DVR_OSD_DIS;
				}
			}
		}else{
			break;//Final One
		}
	}
	return 1;
}

/*OSD Misc Functions*/

void DoDisplayDigitalStat(char Num1,char Num2)
{
#define DISPBUF_LEN	100
	char  DispBuf[DISPBUF_LEN];
	GUI_RECT RectBG;

	RectBG.x0 = COORDINATE_X0_PROG_NUM;
	RectBG.y0 = COORDINATE_Y0_PROG_NUM;
	RectBG.x1 = LCD_WIDTH;
	RectBG.y1 = COORDINATE_Y0_PROG_NUM+GUI_FontF72_BasicLatin.YSize;
	GUI_SetBkColor(GUI_BLACK);
	GUI_ClearRect(RectBG.x0, RectBG.y0, RectBG.x1, RectBG.y1);

	GUI_SetFont(&GUI_FontF72_BasicLatin);
	GUI_SetColor(GUI_GREEN);
	snprintf(DispBuf, DISPBUF_LEN," %c %c", Num1, Num2);
	GUI_DispStringAt((const char *)DispBuf, RectBG.x0, RectBG.y0);

	//恢复默认字体和前景色
	GUI_SetFont(&GUI_Font20_SongTi_Unicode);
	GUI_SetColor(GUI_WHITE);
	
	//设定刷新区域
	SetRefreshAreaPos(OSD_TVAV_POS, RectBG.x0, RectBG.y0, RectBG.x1-RectBG.x0, RectBG.y1-RectBG.y0, ALPHA_COMMON, TRUE);
}


void DoDisplayIngore(void){
	GUI_BITMAP_STREAM *pBmp_Ignore = BMPIndexSearch(IGNORE_32X32);
	GUI_SetColor(GUI_BLACK);
	GUI_FillRect(NOTIFY_X0,NOTIFY_Y0,NOTIFY_X1,NOTIFY_Y1);
	GUI_DrawStreamedBitmap(pBmp_Ignore, NOTIFY_X0, NOTIFY_Y0);
	GUI_SetColor(GUI_WHITE);
	SetRefreshAreaPos(OSD_IGNOREKEY_POS, NOTIFY_X0, NOTIFY_Y0,NOTIFY_WIDTH,NOTIFY_HEIGHT, ALPHA_COMMON, TRUE);
}	

int DoDisplayRecStaus(void *user){
	GUI_BITMAP_STREAM *pBmp_Rec = BMPIndexSearch(IGNORE_32X32);//bmRecording
	GUI_SetColor(GUI_BLACK);
	GUI_FillRect(NOTIFY_X0,NOTIFY_Y0,NOTIFY_X1,NOTIFY_Y1);
	GUI_DrawStreamedBitmap(pBmp_Rec, NOTIFY_X0, NOTIFY_Y0);
	GUI_SetColor(GUI_RED);
	GUI_SetFont(&GUI_Font20_SongTi_Unicode);
	GUI_SetTextMode(GUI_TM_TRANS);
	GUI_DispStringAt("REC",NOTIFY_X0+pBmp_Rec->XSize,NOTIFY_X0);
	
	GUI_SetColor(GUI_WHITE);
	SetRefreshAreaPos(OSD_BLANK_RECORD_POS, NOTIFY_X0, NOTIFY_Y0,NOTIFY_WIDTH,NOTIFY_HEIGHT, ALPHA_COMMON, TRUE);
	return 1;
}


int DoDisplayCurStatRefresh(void *user){
	char  DispBuf[DISPBUF_LEN];

	if(TESTBIT(pUISystem->SysState.CurState,SYS_BIT_PLAYBACK)){
		DATE_TIME CurPlayTime;
		GUI_RECT RectBG0;
		int CurPlayTimeInSecond;
		int playlength;
		int totalplaylength=192;
		int playbar_x0;
		int playbar_y0;
		int num;

		GUI_BITMAP_STREAM *pBmp_Player = BMPIndexSearch(PLAYER_240X40);

		RectBG0.x0 = AUTO_GET_X0_FROM_WIDTH(pBmp_Player->XSize);
		RectBG0.y1 = DOSD_Y1;
		RectBG0.y0 = RectBG0.y1-pBmp_Player->YSize;
		RectBG0.x1 = RectBG0.x0+pBmp_Player->XSize;

		//绘制播放器图片
		GUI_DrawStreamedBitmap(pBmp_Player,RectBG0.x0,RectBG0.y0);

		//填写播放剩余时间
		//del by ryan GUI_SetFont(&GUI_Font20_Bold_SongTi_Unicode_BasicLatin);  
		GUI_SetColor(GUI_WHITE);
		GUI_SetTextMode(GUI_TM_TRANS);
		
		CurPlayTimeInSecond = Player_GetPos();
		//--显示进度保护
		if(CurPlayTimeInSecond > pUISystem->sPlayback.sFileInfo.playtime){
			CurPlayTimeInSecond = pUISystem->sPlayback.sFileInfo.playtime;
		}
		CurPlayTime.Hour = (CurPlayTimeInSecond>>10)/3600;
		CurPlayTime.Minute = ((CurPlayTimeInSecond>>10) % 3600) / 60;
		CurPlayTime.Second = (CurPlayTimeInSecond>>10) % 60;

		num = sprintf(DispBuf, "%02d:%02d:%02d", CurPlayTime.Hour,CurPlayTime.Minute,CurPlayTime.Second);

		GUI_DispStringInRect((const char *)DispBuf, &RectBG0, GUI_TA_HCENTER | GUI_TA_VCENTER);

		GUI_SetFont(&GUI_Font20_SongTi_Unicode);
		//绘制播放进度
		playbar_x0 = RectBG0.x0+36;
		playbar_y0 = RectBG0.y0+6;
		GUI_SetPenSize(2);
		GUI_SetColor(GUI_YELLOW);
		GUI_DrawLine(playbar_x0,playbar_y0,playbar_x0+totalplaylength,playbar_y0);	
		GUI_SetColor(COLOR_BLUE_LOW);
		playlength=totalplaylength*CurPlayTimeInSecond/pUISystem->sPlayback.sFileInfo.playtime;
		GUI_DrawLine(playbar_x0,playbar_y0,playbar_x0+playlength,playbar_y0);	

		//设置刷新区域
		SetRefreshAreaPos(OSD_PLAY_DISPLAY_POS, RectBG0.x0, RectBG0.y0, RectBG0.x1-RectBG0.x0, RectBG0.y1-RectBG0.y0, ALPHA_COMMON, TRUE);
	}else{
		DATE_TIME CurTime;
		GUI_RECT RectBG0;
		GUI_RECT RectBG1;
		GUI_RECT RectBG2;
		GUI_RECT RectBG3;
		GUI_BITMAP_STREAM *pBmp_CurStatus = BMPIndexSearch(CURSTATUS_160_90);
		GUI_BITMAP_STREAM *pBmp_Recording = BMPIndexSearch(RECORDING);
		
		RectBG0.x0 = CUR_STATUS_X0+CUR_STATUS_INTERVAL;
		RectBG0.y0 = CUR_STATUS_Y0;
		RectBG0.x1 = RectBG0.x0+CUR_STATUS_WIDTH-2*CUR_STATUS_INTERVAL;
		RectBG0.y1 = RectBG0.y0+30;

		RectBG1.x0 = RectBG0.x0;
		RectBG1.y0 = CUR_STATUS_Y1;
		RectBG1.x1 = RectBG0.x1;
		RectBG1.y1 = RectBG1.y0+CUR_STATUS_HEIGHT;

		RectBG2.x0 = RectBG0.x0;
		RectBG2.y0 = CUR_STATUS_Y2;
		RectBG2.x1 = RectBG0.x1;
		RectBG2.y1 = RectBG2.y0+CUR_STATUS_HEIGHT;

		GUI_DrawStreamedBitmap(pBmp_CurStatus,CUR_STATUS_X0,CUR_STATUS_Y0);

		GUI_SetFont(&GUI_Font20_SongTi_Unicode);
		GUI_SetColor(GUI_WHITE);
		GUI_SetTextMode(GUI_TM_TRANS);
		GetCurrentDateTime(&CurTime);

		//显示第一行:状态图标
		if( TESTBIT(pUISystem->SysState.CurState,SYS_BIT_RECORDING_ONE_CLICK)){
			GUI_DrawStreamedBitmap(pBmp_Recording,RectBG0.x1-STATUS_PIC_SIZE,RectBG1.y0);
		}
		//显示第二行:时间
		snprintf(DispBuf, DISPBUF_LEN, "%02d:%02d:%02d",CurTime.Hour,CurTime.Minute,CurTime.Second);
		GUI_DispStringInRect((const char *)DispBuf, &RectBG1, GUI_TA_LEFT | GUI_TA_VCENTER);
		//显示第三行:星期
		sprintf(DispBuf,"%s", WeekDayList[CurTime.DayOfWeek]);
		GUI_DispStringInRect((const char *)DispBuf, &RectBG2, GUI_TA_LEFT | GUI_TA_VCENTER);
		SetRefreshAreaPos(OSD_BLANK_DISPLAY_POS, CUR_STATUS_X0, CUR_STATUS_Y0, CUR_STATUS_WIDTH, 3*CUR_STATUS_HEIGHT, ALPHA_COMMON, TRUE);
	}
	return 1;
}


int DoDisplayCurStatRestore(void *user){	
	DoDisplayCurStatRefresh(NULL);
	return 1;
}

void DisplayIgnoreKey(void){	//Display a Icon in blank and playback menu for unhandled key
	DoDisplayIngore();
 	StartTmpOSD(OSDPOS_RIGHTTOP,DELAY_BLANKOSD);
	pUISystem->nOSDBitMap&=(~(1<<OSDPOS_RIGHTTOP));//Clear on next refresh
}

//Current Stat
void DisplayCurState(void){//Disp
	int i;
	int nOSDon=0;
	int nCheckOSD;
	int nOSDIdx;

	if(TESTBIT(pUISystem->SysState.CurState,SYS_BIT_PLAYBACK)){
		nOSDIdx=OSD_PLAY_DISPLAY;
	}else{
		nOSDIdx=OSD_BLANK_DISPLAY;
	}
	nCheckOSD=PGETMENU(pUISystem->sOSDStructStatic[nOSDIdx].nMenu)->nShowOSDIdx;
	for (i=0;i<UI_OSD_COUNT;i++){
		if (VALIDOSDIDX(nCheckOSD)){
			if (nCheckOSD==nOSDIdx){
				nOSDon=1;
				break;
			}
			nCheckOSD=pUISystem->sOSDStructStatic[nCheckOSD].nNextOSD;
		}else{
			break;
		}
	}
	
	if (nOSDon){//OSD already open
		CloseStaticOSD(nOSDIdx);
		return ;
	}
	//OSD open
	StartStaticOSD(nOSDIdx,OSD_BLANK_DISPLAY_INTERVAL,NULL);
}

void DoDisplayControl(void){
	char  DispBuf[DISPBUF_LEN];
	GUI_RECT RectText;
	GUI_BITMAP_STREAM *pBmp;
	GUI_BITMAP_STREAM *pBmp_FF = BMPIndexSearch(FF_32X32);
	GUI_BITMAP_STREAM *pBmp_FR = BMPIndexSearch(FR_32X32);
	GUI_BITMAP_STREAM *pBmp_SlowFW = BMPIndexSearch(SLOW_FW_32X32);
	GUI_BITMAP_STREAM *pBmp_SlowREW = BMPIndexSearch(SLOW_REW_32X32);
	GUI_BITMAP_STREAM *pBmp_Play = BMPIndexSearch(SLOW_FW_32X32);

	FUNC_ENTER
	//绘制控制图标
	if(pUISystem->sPlayback.PlayOrient == FOWARD){ 			//前进
		if(pUISystem->sPlayback.PlayFastorSlow == 1){				//快速
			pBmp = pBmp_FF;
		}else if(pUISystem->sPlayback.PlayFastorSlow == -1){		//慢速
			pBmp = pBmp_SlowFW;
		}else{													//正常
			pBmp = pBmp_Play;
		}
	}else{													//后退
		if(pUISystem->sPlayback.PlayFastorSlow == 1){				//快速
			pBmp = pBmp_FR;
		}else if(pUISystem->sPlayback.PlayFastorSlow == -1){		//慢速
			pBmp = pBmp_SlowREW;
		}else{													//正常
			pBmp = pBmp_Play;
		}
	}
	GUI_DrawStreamedBitmap(pBmp, CUR_STATUS_X0, CUR_STATUS_Y0);

	if(pUISystem->sPlayback.PlaySpeed != 0){
		//填写操作倍数
		RectText.x0 = CUR_STATUS_X0+STATUS_PIC_SIZE;
		RectText.y0 = CUR_STATUS_Y0;
		RectText.x1 = CUR_STATUS_X0+STATUS_PIC_SIZE*3;
		RectText.y1 = CUR_STATUS_Y0+STATUS_PIC_SIZE;
		GUI_SetColor(COLOR_BLUE_LOW);
		GUI_FillRect(RectText.x0, RectText.y0, RectText.x1, RectText.y1);
		//del by ryan GUI_SetFont(&GUI_Font20_Bold_SongTi_Unicode_BasicLatin);

		GUI_SetColor(GUI_WHITE);
		GUI_SetTextMode(GUI_TM_TRANS);
		switch(pUISystem->sPlayback.PlaySpeed)
		{	
			case 1:
				if(pUISystem->sPlayback.PlayFastorSlow==1)
					sprintf(DispBuf, "%s", "x2");
				else if(pUISystem->sPlayback.PlayFastorSlow==-1)
					sprintf(DispBuf, "%s", "x1/2");
				break;
			case 2:
				if(pUISystem->sPlayback.PlayFastorSlow==1)
					sprintf(DispBuf, "%s", "x4");
				else if(pUISystem->sPlayback.PlayFastorSlow==-1)
					sprintf(DispBuf, "%s", "x1/4");
				break;
			case 3:
				if(pUISystem->sPlayback.PlayFastorSlow==1)
					sprintf(DispBuf, "%s", "x8");
				else if(pUISystem->sPlayback.PlayFastorSlow==-1)
					sprintf(DispBuf, "%s", "x1/8");
				break;
			case 4:
				if(pUISystem->sPlayback.PlayFastorSlow==1)
					sprintf(DispBuf, "%s", "x16");
				else if(pUISystem->sPlayback.PlayFastorSlow==-1)
					sprintf(DispBuf, "%s", "x1/16");
				break;
			case 5:
				if(pUISystem->sPlayback.PlayFastorSlow==1)
					sprintf(DispBuf, "%s", "x32");
				else if(pUISystem->sPlayback.PlayFastorSlow==-1)
					sprintf(DispBuf, "%s", "x1/32");
				break;
			default:
				break;
		}
		GUI_DispStringInRect((const char *)DispBuf, &RectText, GUI_TA_HCENTER | GUI_TA_VCENTER);
		GUI_SetFont(&GUI_Font20_SongTi_Unicode);
	}else{
		
		RectText.x0 = CUR_STATUS_X0+STATUS_PIC_SIZE;
		RectText.y0 = CUR_STATUS_Y0;
		RectText.x1 = RectText.x0;
		RectText.y1 = CUR_STATUS_Y0+STATUS_PIC_SIZE;
	}

	//设置刷新区域
	SetRefreshAreaPos(OSD_PLAY_CONTROL_POS, CUR_STATUS_X0, CUR_STATUS_Y0, RectText.x1-CUR_STATUS_X0, RectText.y1-RectText.y0, ALPHA_COMMON, TRUE);
 }

void DisplayControl(void){
	DoDisplayControl();
 	StartTmpOSD(OSDPOS_RIGHTTOP,DELAY_BLANKOSD);
}

void DoDisplayStop(void){
	GUI_BITMAP_STREAM *pBmp_Stop = BMPIndexSearch(STOP_32X32);

	GUI_SetBkColor(GUI_BLACK);
	GUI_ClearRect(CUR_STATUS_X0+STATUS_PIC_SIZE, 
				CUR_STATUS_Y0, 
				CUR_STATUS_X0+STATUS_PIC_SIZE*3, 
				CUR_STATUS_Y0+STATUS_PIC_SIZE);

	GUI_DrawStreamedBitmap(pBmp_Stop, CUR_STATUS_X0, CUR_STATUS_Y0);

	//设置刷新区域
	SetRefreshAreaPos(OSD_PLAY_STOP_POS, CUR_STATUS_X0, CUR_STATUS_Y0,STATUS_PIC_SIZE*3,STATUS_PIC_SIZE, ALPHA_COMMON, TRUE);
 }

void DisplayStop(void){
	DoDisplayStop();
 	StartTmpOSD(OSDPOS_RIGHTTOP,DELAY_BLANKOSD);
 }

void DoDisplayPause(void){
	GUI_BITMAP_STREAM *pBmp_Pause = BMPIndexSearch(PAUSE_32X32);

	GUI_SetBkColor(GUI_BLACK);
	GUI_ClearRect(CUR_STATUS_X0+STATUS_PIC_SIZE, 
				CUR_STATUS_Y0, 
				CUR_STATUS_X0+STATUS_PIC_SIZE*3, 
				CUR_STATUS_Y0+STATUS_PIC_SIZE);

	GUI_DrawStreamedBitmap(pBmp_Pause, CUR_STATUS_X0, CUR_STATUS_Y0);

	//设置刷新区域
	SetRefreshAreaPos(OSD_PLAY_PAUSE_POS, CUR_STATUS_X0, CUR_STATUS_Y0,STATUS_PIC_SIZE,STATUS_PIC_SIZE, ALPHA_COMMON, TRUE);
 }

void DisplayPause(void){
	DoDisplayPause();
 	StartTmpOSD(OSDPOS_RIGHTTOP,DELAY_BLANKOSD);
}

extern void enable_osd_dis(OSD_ALPHA_RECT *pRect, DWORD nRect);
extern void disable_osd_dis(void);

void DisplayBlendRefresh(OSD_ALPHA_RECT *pRect, DWORD nRect)
{
	int n;
	OSD_ALPHA_RECT	*rect;
	
	for (n = 0; n < nRect; n++)
	{
		rect = &pRect[n];
		if ((rect->Left >= LCD_XSIZE) || ((rect->Left + rect->Width) > LCD_XSIZE))
		{
			ERR("refresh area x size overflow\r\n");
			return;
		}
		if ((rect->Top >= LCD_YSIZE) || ((rect->Top + rect->Height) > LCD_YSIZE))
		{
			ERR("refresh area y size overflow\r\n");
			return;
		}
		if (((rect->Left & 0x01) != 0) || ((rect->Width & 0x01) != 0))
		{
			ERR("refresh area left:%d , width %d not fit\r\n",rect->Left,rect->Width);
			return;
		}
		if (rect->Alpha > 256) // The alpha max value is 256
			return;
	}
	
	enable_osd_dis(pRect,nRect);
}

void DisableOsdDisplay(void)
{
	disable_osd_dis();
}
