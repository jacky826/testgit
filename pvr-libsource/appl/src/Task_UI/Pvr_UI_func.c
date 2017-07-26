/*
----------------------------------------------------------------------
File		:	PVR_UI_Func.c
Purpose	:	PVR_UI misc function
----------------------------------------------------------------------
Version-Date			:	2007.1.26
Author-Explanation		:	Shadow
----------------------------------------------------------------------
*/

#include <stdio.h>
#include <string.h>

#include <config.h>	//Config for platform
#include <UI_Message.h>	//UI message

#include <my_spinit.h>
#include <Task_UI/task_ui_var.h>
#include <Task_UI/pvr_ui_func.h>
#include <File_manager.h>
#include <GUI.H>

/*
   Pvr_UI_Init: Provide useful UI_System Data for use
 */

void Pvr_UI_Init(void){
	pUISystem=malloc(sizeof(UI_System_t));
	if(pUISystem==NULL) {
		LOGE("%s:malloc failed\n",__func__);
		request2reboot();
	}
}

void MenuFileBrowser(void)
{
    INT32U bmp_x0;
    INT32U bmp_y0;
    INT32U bmp_SubBG_x0;
    INT32U bmp_SubBG_y0;
    INT32U tmp_interval;
    INT32U bmp_FirstListOnSub_x0;
    INT32U bmp_FirstListOnSub_y0;
    INT32U bmp_FdBG_x0;
    INT32U bmp_FdBG_y0;
    struct timeval tv;

    GUI_SetFont(&GUI_Font20_SongTi_Unicode);
    GUI_SetColor(GUI_WHITE);
    GUI_SetBkColor(GUI_BLACK);

    //gettimeofday(&tv, NULL);
    //printf("%d,%d\r\n", tv.tv_sec, tv.tv_usec);

    GUI_BITMAP *pBmp_BG = &bmBG_DBlue_620_455;
    GUI_BITMAP *pBmp_FdBg = &bmBG_DBlue_316_156;
    GUI_BITMAP *pBmp_FLstBg = &bmBG_DBlue_241_366;

//Logo
    INT32U logo207_x;
    INT32U logo207_y;
    GUI_BITMAP *pBmp_logo_207=&bm207;
    GUI_BITMAP *pBmp_Combox_UnSel = &bmButton_Combox_DBlue_148_30;

    bmp_FdBG_x0 = AUTO_GET_X0_FROM_WIDTH(pBmp_FdBg->XSize+BUTTON_H_INTERVAL_COMMON*3+pBmp_FLstBg->XSize)+pBmp_FLstBg->XSize+BUTTON_H_INTERVAL_COMMON*3;
    bmp_FdBG_y0 = AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) +TITLE_ALL_HEIGHT + BUTTON_V_INTERVAL_COMMON;
    tmp_interval = (pBmp_FdBg->YSize - GUI_Font20_SongTi_Unicode.YSize * BUTTON_CNT_FILESUBINFO)/(BUTTON_CNT_FILESUBINFO + 1);
    bmp_FirstListOnSub_x0=bmp_FdBG_x0+BUTTON_H_INTERVAL_COMMON;
    bmp_FirstListOnSub_y0 = bmp_FdBG_y0 + tmp_interval;
//Logo
    logo207_x=bmp_FdBG_x0+40;
    logo207_y=bmp_FdBG_y0+pBmp_FdBg->YSize+BUTTON_H_INTERVAL_COMMON*3+pBmp_Combox_UnSel->YSize;

    //gettimeofday(&tv, NULL);
    //printf("%d,%d\r\n", tv.tv_sec, tv.tv_usec);

    #if 1
    //绘制背景
    GUI_DrawBitmap(pBmp_BG, AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize), AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize));

    //gettimeofday(&tv, NULL);
    //printf("%d,%d\r\n", tv.tv_sec, tv.tv_usec);
    //填写Title:FileBrowser -
    GUI_SetTextMode(GUI_TM_TRANS);
    GUI_DispStringHCenterAt(FileBrowserStorageList[0], (LCD_WIDTH / 2), \
                            (AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_BELOW_HEIGHT));
    //绘制下方提示信息
    bmp_x0 = AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize);
    bmp_y0 = AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + pBmp_BG->YSize - GUI_Font20_SongTi_Unicode.YSize - BUTTON_V_INTERVAL_COMMON;
    //zqh 2015年5月25日 add，由于英文的提示信息太长(超过字符边框)，所以将HelpTip11的字体变小。
    //gettimeofday(&tv, NULL);
    //printf("%d,%d\r\n", tv.tv_sec, tv.tv_usec);


    GUI_DispStringAt(HelpTip11[pUISystem->allData.Language], bmp_x0, bmp_y0);


    //gettimeofday(&tv, NULL);
    //printf("%d,%d\r\n", tv.tv_sec, tv.tv_usec);

    GUI_DrawBitmap(pBmp_logo_207, logo207_x, logo207_y);

    //gettimeofday(&tv, NULL);
    //printf("%d,%d\r\n", tv.tv_sec, tv.tv_usec);
    #endif
}

void Pvr_UI_Start(void){
	int i;
	int ret;
	
/*Init pUISystem*/
// 1.Init Some variables
	memset(pUISystem,0,sizeof(UI_System_t));

	pUISystem->max_filecount_ever=0;
	pUISystem->list_contain=0;

// 2.Fill in menu & OSD handler
	for (i=0;i<UI_MENU_COUNT;i++){
		pUISystem->Menus[i].nMenuIdx=i;
	}
	//MENUBLANK	
	pUISystem->Menus[MENUBLANK].msgHandle_fn=MenuBlank_MsgHandle;
 	pUISystem->Menus[MENUBLANK].menuInit=NULL;
	pUISystem->Menus[MENUBLANK].menuOpen=MenuBlank_Open;
	pUISystem->Menus[MENUBLANK].menuClose=MenuBlank_Close;
	pUISystem->Menus[MENUBLANK].menuRefresh=MenuBlank_Refresh;
	pUISystem->Menus[MENUBLANK].menuRefreshItem=NULL;

	//MENUFILEBROWSER		
	pUISystem->Menus[MENUFILEBROWSER].msgHandle_fn=MenuFileBrowser_MsgHandle;
 	pUISystem->Menus[MENUFILEBROWSER].menuInit=MenuFileBrowser_Init;
	pUISystem->Menus[MENUFILEBROWSER].menuOpen=NULL;
	pUISystem->Menus[MENUFILEBROWSER].menuClose=MenuFileBrowser_Close;
	pUISystem->Menus[MENUFILEBROWSER].menuRefresh=MenuFileBrowser_Refresh;
	pUISystem->Menus[MENUFILEBROWSER].menuRefreshItem=MenuFileBrowser_RefreshItem;//NULL;	
	//MENUFILEOPERATION	
	pUISystem->Menus[MENUFILEOPERATION].msgHandle_fn=MenuFileOper_MsgHandle;
 	pUISystem->Menus[MENUFILEOPERATION].menuInit=MenuFileOper_Init;
	pUISystem->Menus[MENUFILEOPERATION].menuOpen=NULL;
	pUISystem->Menus[MENUFILEOPERATION].menuClose=NULL;
	pUISystem->Menus[MENUFILEOPERATION].menuRefresh=MenuFileOper_Refresh;
	pUISystem->Menus[MENUFILEOPERATION].menuRefreshItem=MenuFileOper_RefreshItem;	
	//MENUFILECOPYTO		
	pUISystem->Menus[MENUFILECOPYTO].msgHandle_fn=MenuCopyTo_MsgHandle;
 	pUISystem->Menus[MENUFILECOPYTO].menuInit=MenuCopyTo_Init;
	pUISystem->Menus[MENUFILECOPYTO].menuOpen=NULL;
	pUISystem->Menus[MENUFILECOPYTO].menuClose=NULL;
	pUISystem->Menus[MENUFILECOPYTO].menuRefresh=MenuCopyTo_Refresh;
	pUISystem->Menus[MENUFILECOPYTO].menuRefreshItem=MenuCopyTo_RefreshItem;

	//MENUPLAYBACK
	pUISystem->Menus[MENUPLAYBACK].msgHandle_fn=MenuPlayback_MsgHandle;
 	pUISystem->Menus[MENUPLAYBACK].menuInit=MenuPlayback_Init;
	pUISystem->Menus[MENUPLAYBACK].menuOpen=NULL;
	pUISystem->Menus[MENUPLAYBACK].menuClose=MenuPlayback_Close;
	pUISystem->Menus[MENUPLAYBACK].menuRefresh=MenuPlayback_Refresh;
	pUISystem->Menus[MENUPLAYBACK].menuRefreshItem=NULL;	

	//MENUMessagebox
	pUISystem->Menus[MENUMESSAGEBOX].msgHandle_fn=MenuMessageBox_MsgHandle;
 	pUISystem->Menus[MENUMESSAGEBOX].menuInit=MenuMessageBox_Init;
	pUISystem->Menus[MENUMESSAGEBOX].menuOpen=MenuMessageBox_Open;
	pUISystem->Menus[MENUMESSAGEBOX].menuClose=MenuMessageBox_Close;
	pUISystem->Menus[MENUMESSAGEBOX].menuRefresh=MenuMessageBox_Refresh;
	pUISystem->Menus[MENUMESSAGEBOX].menuRefreshItem=NULL;	

	//MENUCODECSETTING		
	pUISystem->Menus[MENUCODECSETTING].msgHandle_fn=MenuCodecSetting_MsgHandle;
 	pUISystem->Menus[MENUCODECSETTING].menuInit=MenuCodecSetting_Init;
	pUISystem->Menus[MENUCODECSETTING].menuOpen=NULL;
	pUISystem->Menus[MENUCODECSETTING].menuClose=NULL;
	pUISystem->Menus[MENUCODECSETTING].menuRefresh=MenuCodecSetting_Refresh;
	pUISystem->Menus[MENUCODECSETTING].menuRefreshItem=MenuCodecSetting_RefreshItem;

	//MENUDEVSTATMSG
	pUISystem->Menus[MENUDEVSTATMSG].msgHandle_fn = MenuDev_Status_MSG_MsgHandle;
	pUISystem->Menus[MENUDEVSTATMSG].menuInit = MenuDev_Status_MSG_Init;
	pUISystem->Menus[MENUDEVSTATMSG].menuOpen = MenuDev_Status_MSG_Open;
	pUISystem->Menus[MENUDEVSTATMSG].menuClose = MenuDev_Status_MSG_Close;
	pUISystem->Menus[MENUDEVSTATMSG].menuRefresh = MenuDev_Status_MSG_Refresh;
	pUISystem->Menus[MENUDEVSTATMSG].menuRefreshItem = NULL;

	//MENUSYSTEMUPDATA
	pUISystem->Menus[MENUSYSTEMUPDATE].msgHandle_fn = MenuSystemUpdate_MsgHandle;
	pUISystem->Menus[MENUSYSTEMUPDATE].menuInit = MenuSystemUpdate_Init;
	pUISystem->Menus[MENUSYSTEMUPDATE].menuOpen = MenuSystemUpdate_Open;
	pUISystem->Menus[MENUSYSTEMUPDATE].menuClose = MenuSystemUpdate_Close;
	pUISystem->Menus[MENUSYSTEMUPDATE].menuRefresh = MenuSystemUpdate_Refresh;
	pUISystem->Menus[MENUSYSTEMUPDATE].menuRefreshItem = MenuSystemUpdate_Refresh_Item;

	//MENUDOFILECOPY
	pUISystem->Menus[MENUDOFILECOPY].msgHandle_fn = MenuDoFileCopy_MsgHandle;
	pUISystem->Menus[MENUDOFILECOPY].menuInit = MenuDoFileCopy_Init;
	pUISystem->Menus[MENUDOFILECOPY].menuOpen = NULL;
	pUISystem->Menus[MENUDOFILECOPY].menuClose = MenuDoFileCopy_Close;
	pUISystem->Menus[MENUDOFILECOPY].menuRefresh = MenuDoFileCopy_Refresh;
	pUISystem->Menus[MENUDOFILECOPY].menuRefreshItem = NULL;

    // Add sys func & udisk file list menu
	//MENUSYSTEMUPDATA
	pUISystem->Menus[MENUSYSTEMFUNC].msgHandle_fn = MenuSystemFunc_MsgHandle;
	pUISystem->Menus[MENUSYSTEMFUNC].menuInit = MenuSystemFunc_Init;
	pUISystem->Menus[MENUSYSTEMFUNC].menuOpen = NULL;
	pUISystem->Menus[MENUSYSTEMFUNC].menuClose = NULL;
	pUISystem->Menus[MENUSYSTEMFUNC].menuRefresh = MenuSystemFunc_Refresh;
	pUISystem->Menus[MENUSYSTEMFUNC].menuRefreshItem = MenuSystemFunc_RefreshItem;

	//MENUSYSTEMUPDATA
	pUISystem->Menus[MENUUDISKFILELIST].msgHandle_fn = MenuUDiskFileList_MsgHandle;
	pUISystem->Menus[MENUUDISKFILELIST].menuInit = MenuUDiskFileList_Init;
	pUISystem->Menus[MENUUDISKFILELIST].menuOpen = NULL;
	pUISystem->Menus[MENUUDISKFILELIST].menuClose = MenuUDiskFileList_Close;
	pUISystem->Menus[MENUUDISKFILELIST].menuRefresh = MenuUDiskFileList_Refresh;
	pUISystem->Menus[MENUUDISKFILELIST].menuRefreshItem = MenuUDiskFileList_RefreshItem;

    //zqh 2015年3月31日 add， 添加“语言”选择功能界面
    pUISystem->Menus[MENULANGUAGESEL].msgHandle_fn = MenuLanguageSel_MsgHandle;
    pUISystem->Menus[MENULANGUAGESEL].menuInit = MenuLanguageSel_Init;
    pUISystem->Menus[MENULANGUAGESEL].menuOpen = NULL;
    pUISystem->Menus[MENULANGUAGESEL].menuClose = NULL;
    pUISystem->Menus[MENULANGUAGESEL].menuRefresh = MenuLanguageSel_Refresh;
    pUISystem->Menus[MENULANGUAGESEL].menuRefreshItem = MenuLanguageSel_Refresh_Item;



    //OSD
	InitStaticOSD();

    LoadAllDataFlash();//读取配置文件初始化pUISystem变量

	SPInit_all();

	//set default record stat
	pUISystem->SysState.CurState=SYS_STAT_IDLE;
	SETBIT(pUISystem->SysState.CurState,SYS_BIT_GETTIME);

	//Set default dev stat
	pUISystem->SysState.DevState=SYS_STAT_IDLE;
	pUISystem->nDevCurStatusMsg = MESSAGESTAGE_IDLE;
	SETBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_REMOVE);
	SETBIT(pUISystem->SysState.DevState,SYS_BIT_SDCard_REMOVE);

    // set default record disk
	pUISystem->nFileSelectDisk = DISK_CF;
    // Set File select to default value
	pUISystem->FileBrowserIndex = 0;
	pUISystem->FileBrowserSelFile = 0;

	pUISystem->nMenuIdx=-1;
    LOGT("system init to menublank\r\n");
	SystemChangeWindow(MENUBLANK,0,1,0);

    // 默认设置为 初始化状态,在上电过程中第一次加载成功之前,不自动提示cf卡不在位,但仍会自动尝试加载操作

    // 加载cf卡,如果cf卡在位但是控制芯片需要复位,则在该步已经进行了复位
    // 如果启用了加载cf的提示界面,则加载过程会在一个子线程进行,不影响界面响应
	ret = SystemMountCFCard();
	if(ret<0)
	{
		LOGE("%s: Mount CFCard Failed\r\n",__func__);
		SetLvdsSwitch(0);
		pUISystem->nDevCurStatusMsg = MESSAGESTAGE_CFCARDREMOVE;
		SystemChangeWindow(MENUDEVSTATMSG,1,1,0);
	}

#if (USE_MOUNT_CF_THREAD)||(USE_MOUNT_CF_THREAD_S)
    // 延时1s用于加载cf卡,此时不响应按键信息，如果此时有按键按下，会在之后的获取键值时进行处理
    FEED_WATCHDOG();
    sleep(1);
    FEED_WATCHDOG();
#endif

	//Enable Removable Device detect,this timer will not be disabled
	EnableTimer(&(pUISystem->sDevScanTimer),0,TIMER_DEVSCAN,1,System_DevScanTimer_TO);

	// clear FileBrowserdirlist struct
	memset(&(pUISystem->FileBrowserdirlist), 0, sizeof(struct FileListBlock));

    MenuFileBrowser();

	LOGT("PVR UI Init complete\r\n");

}

void Pvr_UI_Stop(void){
	if (TESTBIT(pUISystem->SysState.CurState,SYS_BIT_PLAYBACK)){
		Player_Stop();
	}
	if (StopRecord()!=ERR_OK){
		ClearRecordState();
	}

	Pvr_Waiting(FALSE);

    LOGT("system close change to menublank\r\n");
	SystemChangeWindow(MENUBLANK,1,1,0);
	Pvr_Waiting(FALSE);	
	//sys_deinit_encoder();
	SaveAllDataFlash();
#if 0 //Junr add note : umount sd/usb
	if (pUISystem->Vol_Disk[DISK_HDD]){
		UnmountStore(dt_HDD,pUISystem->DiskNo[DISK_HDD]);
		IDE_Deinit();
	}		
	for (i=1;i<SUPPORT_DISK_CNT;i++){
		if (pUISystem->Vol_Disk[i]){
			UnmountStore(dt_USB,pUISystem->DiskNo[i]);
		}		
	}
#endif	
	SETBIT(pUISystem->SysState.CurState,SYS_BIT_POWEROFF);
}

void Pvr_UI_DeInit(void){
	if (pUISystem){
		free(pUISystem);
		pUISystem=NULL;
	}	
}

/*
 ----	System Provided API 
 */

/*SystemChangeWindow: invoke to change active window
 *nNewMenu: new window id;
 *nNeedClose: 1-Close current menu
 *nNeedInit: 1-Open Initmenu
 *return: 0-wrong parameters 1-succes
 */
int SystemChangeWindow(int nNewMenu,int nNeedClose,int nNeedInit,int nNeedSaveFrom){
	int nOldMenu=pUISystem->nMenuIdx;
	if (!VALIDMENU(nNewMenu)){
		return 0;
	}
	LOGT("%s: change window from %d to %d\n",__func__,nOldMenu,nNewMenu);
	if (nNewMenu==pUISystem->nMenuIdx){
		//Seem no need perform a change
		if (nNewMenu==MENUBLANK){
			//Sometime this window need open to activate schedule
			if (VALIDFUNCTION(PACTIVE_MENU->menuOpen)){//Menu init already checked  
				PACTIVE_MENU->menuOpen(PACTIVE_MENU);
				//TODO:check return value
			}else {
				DefaultMenuOpen(PACTIVE_MENU);
			}
		
		}
		if(nNewMenu == MENUDEVSTATMSG)
		{
			if (VALIDFUNCTION(PACTIVE_MENU->menuOpen)){//Menu init already checked  
				PACTIVE_MENU->menuOpen(PACTIVE_MENU);
				//TODO:check return value
			}else {
				DefaultMenuOpen(PACTIVE_MENU);
			}
		}
		return 1;
	}
	if (nNeedClose){
		if (VALIDMENU(pUISystem->nMenuIdx)&&
			VALIDFUNCTION(PACTIVE_MENU->menuClose)){
			PACTIVE_MENU->menuClose(PACTIVE_MENU);
            //TODO:check return value
		}
		//TODO:perform default close 
	}
	if (nNeedInit){
		if (VALIDFUNCTION(PGETMENU(nNewMenu)->menuInit)){//Menu init already checked  
			if (PGETMENU(nNewMenu)->menuInit(PGETMENU(nNewMenu))<=0){
				MenuRefresh();
				return 0;
			}
			//TODO:check return value
		}
	}
	//Change Active Menu
	PauseAllOSD();
	pUISystem->nOSDBitMap=0;
	pUISystem->nMenuIdx=nNewMenu;

	if (nNeedSaveFrom){
		PACTIVE_MENU->nFromMenuIdx=nOldMenu;
	}

	if (VALIDFUNCTION(PACTIVE_MENU->menuOpen)){//Menu init already checked  
		PACTIVE_MENU->menuOpen(PACTIVE_MENU);
		//TODO:check return value
	}else {
		DefaultMenuOpen(PACTIVE_MENU);
	}

	return 1;
}

/*SystemMenuRestore: close current menu and restore uplevel menu
 *return: 0-wrong parameters 1-succes
 */
int SystemMenuRestore(void){
	if (!VALIDMENU(PACTIVE_MENU->nFromMenuIdx)){
		PACTIVE_MENU->nFromMenuIdx=MENUBLANK;
	}
	return SystemChangeWindow(PACTIVE_MENU->nFromMenuIdx,1,0,0);
}


int MenuSetUsefulButton(int nMenuIdx){
	int i;
	if (!VALIDMENU(nMenuIdx)){
		return -1;
	}
	for (i=0;i<MenuButtonCnt[nMenuIdx];i++){
		if (!((1<<i)&MenuButtonMaskBitmap[nMenuIdx])){		
			return i;
		}
	}
	return 0;//No useful button
}

/*MenuChangeFocusCustom: invoke to change focus of active menu
 *nOffset: (-1)-move up,let idx-1, 1-move down,let idx+1
 *nButtonCnt	MenuButtonCnt
 *return: 0-wrong parameters 1-succes
 */
int MenuChangeFocusCustom(int nOffset,int nButtonCnt){
	int oldIdx;
	int i;
	if ((!nOffset)||(!VALIDMENU(pUISystem->nMenuIdx))){
		return 0;
	}

	if (!nButtonCnt){
		//Not support change focus on active menu
		return 0;
	}

	oldIdx=PACTIVE_MENU->nButtonIdx;

	for (i=0;i<nButtonCnt;i++){
		PACTIVE_MENU->nButtonIdx+=nOffset;
		
		(PACTIVE_MENU->nButtonIdx)%=nButtonCnt;

		if (PACTIVE_MENU->nButtonIdx<0){
			PACTIVE_MENU->nButtonIdx+=nButtonCnt;
		}
		if ((1<<PACTIVE_MENU->nButtonIdx)&MenuButtonMaskBitmap[pUISystem->nMenuIdx]){
			nOffset=(nOffset>0)?1:(-1);
		}else{
			break;
		}
	}

	if (VALIDFUNCTION(PACTIVE_MENU->menuRefreshItem)){
		if ((MenuRefreshItemOnly(oldIdx)>0)&&
			(MenuRefreshItemOnly(PACTIVE_MENU->nButtonIdx)>0)){
			RefreshOSDDisplay();
			return 1;
		}else{
			return 0;
		}
	}else {
		return MenuRefresh();
	}
}


/*MenuChangeFocus: invoke to change focus of active menu
 *nOffset: (-1)-move up,let idx-1, 1-move down,let idx+1
 *return: 0-wrong parameters 1-succes
 */
int MenuChangeFocus(int nOffset){
	if ((!nOffset)||(!VALIDMENU(pUISystem->nMenuIdx))){
		return 0;
	}

	return MenuChangeFocusCustom(nOffset,MenuButtonCnt[pUISystem->nMenuIdx]);
}

int MenuRefreshItemOnly(int nIdx){
	if (VALIDFUNCTION(PACTIVE_MENU->menuRefreshItem)){
		return PACTIVE_MENU->menuRefreshItem(PACTIVE_MENU,nIdx,
			(nIdx==PACTIVE_MENU->nButtonIdx)?1:0);//Focus
	}else if (VALIDFUNCTION(PACTIVE_MENU->menuRefresh)){
		//At least menu shoud have a handle to repaint
		return PACTIVE_MENU->menuRefresh(PACTIVE_MENU);
	}else{
		//Never should happen
		return 0;
	}
}

int MenuRefreshItem(int nIdx){
	if (MenuRefreshItemOnly(nIdx)>0){
		RefreshOSDDisplay();
		return 1;
	}
	return 0;
}

int MenuRefresh(void){
	if (VALIDMENU(pUISystem->nMenuIdx)&&(VALIDFUNCTION(PACTIVE_MENU->menuRefresh))){
		//At least menu shoud have a handle to repaint
		if (PACTIVE_MENU->menuRefresh(PACTIVE_MENU)>0){
			RefreshOSDDisplay();
			return 1;
		}
		return 0;
	}else{
		//Never should happen
		return 0;
	}
}


int MenuRefreshAllControl(void){
	int i;
	int val;
	for (i=0;i<MenuButtonCnt[pUISystem->nMenuIdx];i++){
		val=MenuRefreshItemOnly(i);
		if (val<=0){
			return val;
		}
	}
	RefreshOSDDisplay();
	return 1;
}

/**
 * @brief DisableTimer
 * 关闭定时器，将参数恢复为默认值
 * @param pTimer
 */
void DisableTimer(TEF_TIMER_STRUCT *pTimer){
	pTimer->TimerStatus=DVR_TIMER_DIS;
	pTimer->TimerCnt=0;
	pTimer->TimerDelay=0;
	pTimer->TO_fn=NULL;
}

void CloseTimer(TEF_TIMER_STRUCT *pTimer){
	if (pTimer->TimerStatus!=DVR_TIMER_DIS){
		//Close Timer
		if (VALIDHANDLE(pTimer->TO_fn)){
			pTimer->TO_fn(pTimer);
		}
	}

	DisableTimer(pTimer);
}

void EnableTimer(TEF_TIMER_STRUCT *pTimer,int nPerformClose,unsigned int TimerUser,int TimerTO,TimerTO_fn *fn){
	if (nPerformClose){
		CloseTimer(pTimer);
	}else{
		DisableTimer(pTimer);
	}

	pTimer->TimerStatus=TimerUser;
	pTimer->TimerDelay=TimerTO;
	pTimer->TimerInterval=TimerTO;
	pTimer->TO_fn=fn;
}

/*
 ----	Default handler for Menu set handler to NULL 
 */

 /*Default Timer Time-Out Handler for OSD & Key Timer, just close timer
  */
TMsgHandleStat DefaultTimerHandle(TEF_TIMER_STRUCT *pTimer){
	DisableTimer(pTimer);
	return MsgHandled;
}

 /* a default menu open function
  */
int DefaultMenuOpen(UI_Menu_t *pMenu){
	return MenuRefresh();
}

 /*Message Box
 */

/**
 * @brief MessageBox
 * 弹出“消息窗口“时调用此函数，比如：优盘未挂载、cf卡未插入等消息窗口
 * 主要是对pUISystem的相关变量进行初始化
 * @param pContent
 * 要显示的文本
 * @param nTimeout
 * 弹窗显示定时器
 * @param pfn
 *
 * @param pCancelfn
 * @param pUser
 * @param pOk
 * @param pFail
 * @param pDeinitfn
 * @return
 */
int MessageBox(const char *pContent, int nTimeout,
	MessageboxCB_fn *pfn,MessageboxCB_fn *pCancelfn, void *pUser,
	const char *pOk,const char *pFail,MessageboxDeinit_fn *pDeinitfn)
{
	int nSave=1;
	if (pUISystem->nMessageStage!=MESSAGESTAGE_IDLE){//Another message box is running
		nSave=0;//Don't update from window TODO; need call deinit at first?
	}

    //FUNC_ENTER

    pUISystem->pMessageDisplay=(char *)pContent;
	pUISystem->nMessageTimeout=nTimeout;
	pUISystem->pMessageUser_fn=pfn;
	pUISystem->pMessageCancel_fn=pCancelfn;
	
	pUISystem->pMessageUser=(char *)pUser;

	pUISystem->pMessageFail=(char *)pFail;
	pUISystem->pMessageOK=(char *)pOk;
	pUISystem->pMessageDeinit_fn=pDeinitfn;
	
	return SystemChangeWindow(MENUMESSAGEBOX,0,1,nSave);
}

/*
 ----	Misc Functions for UI Operation from PVR_common.c
 */
//--------------------------------------------------------------------------//
//	Name		:	SetRefreshArea
//	Purpose		:	设定需要做OSD的范围
//	Parameter	:	
//					"flag = 1" means to use DisplayBlendRefresh() to refresh the screen;
//					"flag = 0" means to use DisplayRefresh() to refresh the screen;
//					(x0, y0) is the left-upper coordinate of current background
//					(x0 + width, y0 + height) is the right-lower coordinate of current background
//--------------------------------------------------------------------------//
void SetRefreshAreaPos(int nPos, DWORD x0, DWORD y0, DWORD width, DWORD height, WORD alpha, BOOL transC)
{
	//TODO:Ignore Flag
	
	if (x0&0x1){
		x0-=1;
	}
	if (width&0x1){
		width-=1;
	}
	pUISystem->RefreshRect[nPos].Left = x0;
	pUISystem->RefreshRect[nPos].Top = y0;
	pUISystem->RefreshRect[nPos].Width = width;
	pUISystem->RefreshRect[nPos].Height = height;
	pUISystem->RefreshRect[nPos].Alpha = alpha; 			// (0:Cover -- 256:Transparent)
	pUISystem->RefreshRect[nPos].TransColor = transC; 		// Enable transparence color (black)

	pUISystem->nOSDBitMap|=(1<<nPos);
}

int RefreshOSDDisplay(void){
	OSD_ALPHA_RECT RefreshRect[OSDPOS_COUNT];	
	int i,nc;
	nc=0;
	for (i=0;i<OSDPOS_COUNT;i++){
		if (pUISystem->nOSDBitMap&(1<<i)){
			memcpy(&(RefreshRect[nc]),&(pUISystem->RefreshRect[i]),sizeof(OSD_ALPHA_RECT));
			nc++;
		}
	}
	if (nc){
		DisplayBlendRefresh(RefreshRect,nc);
	}else{
		DisableOsdDisplay();
	}
	return 1;
}


//--------------------------------------------------------------------------//
//	Name	:	BMPIndexSearch
//	Purpose	:	由图片文件名查找相应图片.dta文件存储的地址
//--------------------------------------------------------------------------//
void * BMPIndexSearch(int nImageName) 
{
	if (nImageName>=IMAGE_RESOURCE_NUM){
		return NULL;
	}else{
		return &(pUISystem->BMPStream[ImageResourceIndex[nImageName].ReservedOffset]);		
	}
}

/*
 ----	Misc Functions for UI Operation from My_tool.c
 */

//--------------------------------------------------------------------------//
//	Name	:	get_week_from_date
//	Purpose	:	计算"*年*月*日"是星期几
//--------------------------------------------------------------------------//
//
void get_week_from_date(DATE_TIME *date)
{
	//1900年1月1日是星期一
	WORD temp_year = 1900;
	WORD temp_month = 1;
	WORD temp_day = 1;
	WORD large_years = 0;
	WORD small_years = 0;
	WORD large_months = 0;
	WORD small_months = 0;
	WORD large_feb = 0;
	WORD small_feb = 0;
	DWORD total_days;

	for(temp_year=1900; temp_year < date->Year ; temp_year++)
	{
		if(((temp_year % 4) == 0 &&( temp_year % 100) != 0) || (temp_year % 400) == 0)
		{
			large_years++;
		}
		else
		{	
			small_years++;
		}
	}

	for(temp_month = 1; temp_month < date->Month; temp_month++)
	{
		if(temp_month == 1 || temp_month == 3 || temp_month == 5 || temp_month == 7 ||temp_month == 8 || temp_month==10 || temp_month==12)
		{
			large_months++;
		}
		if( temp_month == 4 || temp_month == 6 || temp_month == 9 || temp_month == 11)
		{
			small_months++;
		}
		else
		{
			if(((date->Year % 4) == 0 && (date->Year % 100) !=0) || (date->Year % 400) == 0)
			{
				large_feb++;
			}
			else
			{
				small_feb++;
			}
		}
	}

	total_days = 366 * large_years + 365 * small_years + 31 * large_months + 30 * small_months + date->Day;

	date->DayOfWeek=(DWORD)(total_days%7);
	
	return;
	
}	

WORD max_days_in_month(DATE_TIME *tmp_date)
{
	switch (tmp_date->Month)
	{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			return 31;
		case 4:
		case 6:
		case 9:
		case 11:
			return 30;
		case 2:
			if ((!(tmp_date->Year % 4) && (tmp_date->Year% 100)) || !(tmp_date->Year% 400))
				return 29;
			else
				return 28;
		default:
			return 30;
	}
}

void AdjustValue(int *val,int adjval,int min,int max){
	int tmpval=*val;
	tmpval+=adjval;
	if (min==0){
		tmpval%=max;
		if (tmpval<0){
			tmpval+=max;
		}
	}else{
		if (tmpval>max){
			tmpval=max;
		}
		if (tmpval<min){
			tmpval=min;
		}
	}
	*val=tmpval;
}

void CheckValue(int *oldVal,int newVal,int min,int max){
	int value;
	if((newVal >= min) &&(newVal <= max)){
		value = newVal;
	}else{
		if(min == 2000){
			value = 2000 + newVal % 100;
		}else{
			value = newVal % 10;
		}
		if(value < min){
			value = *oldVal;
		}
	}
	*oldVal = value;
}

void check_days(DATE_TIME *tmp_date){
	WORD Days = 0;
	Days = max_days_in_month(tmp_date);
	if (tmp_date->Day> Days)	{
		tmp_date->Day = Days;
	}
}

void datetime_change(int value, INT8S ListIndex, DATE_TIME *tmp_date){
	int tmpOldVal;
	int tmpNewVal;

	switch (ListIndex)
	{
		case 0:	//year
			tmpOldVal = tmp_date->Year;
			tmpNewVal = (tmp_date->Year % 1000) * 10 + value;
			CheckValue(&tmpOldVal,tmpNewVal,2000,MAX_YEAR);
			tmp_date->Year = tmpOldVal;
			check_days(tmp_date);
			get_week_from_date(tmp_date);	
			break;
		case 1:	//month
			tmpOldVal = tmp_date->Month;
			tmpNewVal = (tmp_date->Month % 10) * 10 + value;
			CheckValue(&tmpOldVal,tmpNewVal,1,12);
			tmp_date->Month = tmpOldVal;
			check_days(tmp_date);
			get_week_from_date(tmp_date);	
			break;
		case 2:	//day
			tmpOldVal = tmp_date->Day;
			tmpNewVal = (tmp_date->Day % 10) * 10 + value;
			CheckValue(&tmpOldVal,tmpNewVal,1,max_days_in_month(tmp_date));
			tmp_date->Day = tmpOldVal;
			get_week_from_date(tmp_date);	
			break;
		case 3:	//hour
			tmpOldVal = tmp_date->Hour;
			tmpNewVal = (tmp_date->Hour % 10) * 10 + value;
			CheckValue(&tmpOldVal,tmpNewVal,0,23);
			tmp_date->Hour = tmpOldVal;
			break;
		case 4:	//minute
			tmpOldVal = tmp_date->Minute;
			tmpNewVal = (tmp_date->Minute % 10) * 10 + value;
			CheckValue(&tmpOldVal,tmpNewVal,0,59);
			tmp_date->Minute = tmpOldVal;
			break;
		case 5:	//second
			tmpOldVal = tmp_date->Second;
			tmpNewVal = (tmp_date->Second % 10) * 10 + value;
			CheckValue(&tmpOldVal,tmpNewVal,0,59);
			tmp_date->Second = tmpOldVal;
			break;
		default:
			break;
	}
}

void datetime_setting(int adjustval,INT8S ListIndex, DATE_TIME *tmp_date){
	int tmpval;
	switch (ListIndex)
	{
		case 0:	//year
			tmpval=tmp_date->Year;
			AdjustValue(&tmpval,adjustval,2000,MAX_YEAR);
			tmp_date->Year=tmpval;
			check_days(tmp_date);
			get_week_from_date(tmp_date);	
			break;
		case 1:	//month
			tmpval=tmp_date->Month-1;
			AdjustValue(&tmpval,adjustval,0,12);
			tmp_date->Month=tmpval+1;
			check_days(tmp_date);
			get_week_from_date(tmp_date);	
			break;
		case 2:	//day
			tmpval=tmp_date->Day-1;
			AdjustValue(&tmpval,adjustval,0,max_days_in_month(tmp_date));
			tmp_date->Day=tmpval+1;
			get_week_from_date(tmp_date);	
			break;
		case 3:	//hour
			tmpval=tmp_date->Hour;
			AdjustValue(&tmpval,adjustval,0,24);
			tmp_date->Hour=tmpval;		
			break;
		case 4:	//minute
			tmpval=tmp_date->Minute;
			AdjustValue(&tmpval,adjustval,0,60);
			tmp_date->Minute=tmpval;		
			break;
		case 5:	//second
			tmpval=tmp_date->Second;
			AdjustValue(&tmpval,adjustval,0,60);
			tmp_date->Second=tmpval;		
			break;
	}
}


//--------------------------------------------------------------------------//
//	Name	:	Pvr_Waiting
//	Purpose	:	等待提示控件
//				使用后，需要将该区域重新绘制，不建议使用
//	Parameter	:	@blend_or_not - True:调用DisplayBlendRefresh;	False:调用DisplayRefresh
//--------------------------------------------------------------------------//
void Pvr_Waiting(BOOL blend_or_not)
{
	GUI_BITMAP *pBmp_BG = (&bmMessageRect_310_130);
	//--------------------------------------------------------------------------//
	//	Purpose	:	绘制背景
	//--------------------------------------------------------------------------//
	GUI_DrawBitmap(pBmp_BG, 
							AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize), 
							AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize));

	//--------------------------------------------------------------------------//
	//	Purpose	:	填写显示内容
	//--------------------------------------------------------------------------//
	GUI_RECT Rect;
	Rect.x0 = AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize);
	Rect.y0 = AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize);
	Rect.x1 = AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize) + pBmp_BG->XSize;
	Rect.y1 = AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + pBmp_BG->YSize;
	GUI_SetFont(&GUI_Font20_SongTi_Unicode);
	GUI_SetColor(GUI_WHITE);
	GUI_SetBkColor(GUI_BLACK);
	GUI_SetTextMode(GUI_TM_TRANS);
	GUI_DispStringInRect(Wait[pUISystem->allData.Language], &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
	if(!blend_or_not){
		SetRefreshAreaPos(OSDPOS_BASEMENU, 
				AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize), 
				AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize), 
				pBmp_BG->XSize, 
				pBmp_BG->YSize, 
				ALPHA_COMMON,TRUE);	
	}
	RefreshOSDDisplay();
}

/*
* set_filelist_and_IconMask - 设置filelist以及IconMask
*
* @*dirlist - FileListBlock结构体指针，存储当前路径文件系统信息。
* @*filelist[] - FILES_PER_BLOCK个成员的指针数组，用于显示一屏。
* @Index - 当前高亮在整个列表中的索引1 ~ TotalNum。
* @TotalNum - 整个列表总个数。
* @*IconMask - 显示一屏列表的图标位图，
*
*	|*IconMask:
*	|
*	|	 b  r  r  r  r  r  9  8  7  6  5  4  3  2  1  0
*	|	10 00 00 00 00 00 xx xx xx xx xx xx xx xx xx xx
*	|
*	|	-- b:		标志位，0x10 - 位图有效; 0x00 - 位图无效。
*	|	-- r:			保留区。
*	|	-- 0 ~ 9:		列表行数所对应的值， 1 - dir; 2 - file。
*	----------------------------------------------------------------------
* 该函数通过Index以及TotalNum获取整屏信息，
* 填充*filelist[]指针数组，并且将*IconMask填充。
*
* return value:
*	void
*/
void set_filelist_and_IconMask(struct FileListBlock *dirlist, 
		char *filelist[], int Index, int TotalNum, INT32U *IconMask)
{
	INT32U NumPerPage;
	int i;

	if (dirlist == NULL)
		return;
	if (filelist == NULL)
		return;
	if (IconMask == NULL)
		return;
	
	memset(filelist, 0, FILES_PER_BLOCK * sizeof(char*));
	
	NumPerPage = (Index - 1) / FILES_PER_BLOCK;
	NumPerPage = TotalNum - (NumPerPage * FILES_PER_BLOCK);
	if (NumPerPage > FILES_PER_BLOCK)
		NumPerPage = FILES_PER_BLOCK;
	
	(*IconMask) = 0x80000000;
	for (i = 0; i != NumPerPage; i++) {
		filelist[i] = (dirlist->attribute + (((Index- 1) / FILES_PER_BLOCK) * FILES_PER_BLOCK + i))->longname;
		if (((dirlist->attribute + (((Index- 1) / FILES_PER_BLOCK) * FILES_PER_BLOCK + i))->flag) & FILE_DIR_MASK)
			(*IconMask) |= DIR_ICON(i);
		else
			(*IconMask) |= FILE_ICON(i);
	}
}


