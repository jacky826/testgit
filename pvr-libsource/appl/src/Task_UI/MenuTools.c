#include <string.h>
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h> 
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <sys/utsname.h> //uname()

#include <config.h>	//Config for platform
#include <UI_Message.h>	//UI message
#include <Key.h>
#include <Task_UI/task_ui_var.h>
#include <Task_UI/pvr_ui_func.h>
#include <GUI.H>

extern int rs422fd; //haiqing
extern void Handle_Timer_Tick(TEF_TIMER_STRUCT *pTimer);//为了调用此函数

/*Message Box*/
/**
 * @brief MessageBox_Deinit
 * 消息窗口的反初始化
 * @return
 */
int MessageBox_Deinit(void)
{
    DisableTimer(&pUISystem->sKeyTimer);//关闭定时器

	if (VALIDFUNCTION(pUISystem->pMessageDeinit_fn)){
		return pUISystem->pMessageDeinit_fn(pUISystem->nMessageStage);
	}else{
	    LOGT("MenuRestore\r\n");
		return SystemMenuRestore();
	}
}

/**
 * @brief MessageBox_Cancel
 * 调用MessageBox_Deinit()函数进行反初始化
 * 并设置消息状态，为MenuMessageBox_Refresh()显示做准备
 * @return
 */
int MessageBox_Cancel(void)
{
	if (VALIDFUNCTION(pUISystem->pMessageCancel_fn)){
		pUISystem->pMessageCancel_fn(pUISystem->pMessageUser);
		pUISystem->nMessageStage=MESSAGESTAGE_CANCEL;
	}else if (!VALIDFUNCTION(pUISystem->pMessageCancel_fn)){
		pUISystem->nMessageStage=MESSAGESTAGE_OK;
	}else{
		pUISystem->nMessageStage=MESSAGESTAGE_CANCEL;
	}
	MessageBox_Deinit();
	return 1;
}

/**
 * @brief MenuMessageBox_KeyTimer_TO
 * 消息弹窗，显示超时，处理函数
 * 在MenuMessageBox_Open()的EnableTimer()的函数中进行初始化
 * @param pTimer
 * @return
 */
TMsgHandleStat MenuMessageBox_KeyTimer_TO(TEF_TIMER_STRUCT *pTimer){
	if (pUISystem->nMessageStage!=MESSAGESTAGE_RUN){//User confirm the notify message, any key quit 
        //disable timer,call deinit_fn or menurestore
		MessageBox_Deinit();
	}else{
	    // call cancel_fn and deinit
		MessageBox_Cancel();
	}
	return MsgHandled;
}

/**
 * @brief MenuMessageBox_Init
 * 消息弹窗初始化。
 * @param pMenu
 * 当前活动窗口
 * @return
 */
int MenuMessageBox_Init(UI_Menu_t *pMenu){
	//Do some check
    //FUNC_ENTER
    if((pUISystem->pMessageDisplay== NULL)||(strlen(pUISystem->pMessageDisplay)==0)) {//检测是否有需要显示的内容，无则直接返回
		return 0;
	}//Nothing to Display
	DisableTimer(&pUISystem->sKeyTimer);
    pUISystem->nMessageStage=MESSAGESTAGE_RUN;//将信息状态设置成”正在运行“状态
	return 1;
}

/**
 * @brief MenuMessageBox_MsgHandle
 * 在消息弹窗界面下，响应某些按键的处理函数
 * @param pMenu
 * 当前活动窗口
 * @param nMsgType
 * @param nMsgData
 * @return
 */
TMsgHandleStat MenuMessageBox_MsgHandle(UI_Menu_t *pMenu,unsigned short nMsgType,unsigned short nMsgData){
	TMsgHandleStat retval=MsgHandled;
	int val;

	if (nMsgType!=MSG_KEY_EVENT){
		return MsgNocare;
	}

	if (pUISystem->nMessageStage!=MESSAGESTAGE_RUN){//User confirm the notify message, any key quit 
		MessageBox_Deinit();//Will disable timer in Close
		return MsgHandled;
	}

	switch(nMsgData){
        case KEY_ENTER://确认键
			//OK
            if (VALIDFUNCTION(pUISystem->pMessageUser_fn)){//假设：是删除文件的操作。则pUISystem->pMessageUser_fn =  Delete_Callback()函数
				val=pUISystem->pMessageUser_fn(pUISystem->pMessageUser);
				if (val>=0){
					pUISystem->nMessageStage=MESSAGESTAGE_OK;
				}else{
					pUISystem->nMessageStage=MESSAGESTAGE_FAIL;
				}
				if ((pUISystem->pMessageOK)&&(pUISystem->pMessageFail)){//Display the notify 
					if (pUISystem->nMessageTimeout>0){
                        //设置消息弹窗的显示时间为(pUISystem->nMessageTimeout)秒
						EnableTimer(&(pUISystem->sKeyTimer),0,TIMER_MESSAGEBOX,pUISystem->nMessageTimeout,MenuMessageBox_KeyTimer_TO);
					}//Must not call TO function
					MenuRefresh();
				}else{//No care exceute result,
					MessageBox_Deinit();
				}
			}else{
				pUISystem->nMessageStage=MESSAGESTAGE_OK;
				MessageBox_Deinit();
			}
			break;
        case KEY_ESC:
        default://default cancel
			MessageBox_Cancel();
			break;
	}
	return retval;
}

/**
 * @brief MenuMessageBox_Open
 * 消息弹窗的初始化，使能定时器
 * 初始化，消息弹窗超时后的回调函数：MenuMessageBox_KeyTimer_TO()
 * @param pMenu
 * @return
 */
int MenuMessageBox_Open(UI_Menu_t *pMenu){//Over write default close timer
    //FUNC_ENTER
    MenuRefresh();//绘制弹窗界面，通过调用MenuMessageBox_Refresh()实现绘制
	if (pUISystem->nMessageTimeout>0){
		EnableTimer(&(pUISystem->sKeyTimer),1,TIMER_MESSAGEBOX,pUISystem->nMessageTimeout,MenuMessageBox_KeyTimer_TO);
	}
	return 1;
}

/**
 * @brief MenuMessageBox_Close
 * 关闭消息弹窗，对变量pUISystem->nMessageStage恢复默认值、关闭定时器
 * @param pMenu
 * 当前活动窗口
 * @return
 */
int MenuMessageBox_Close(UI_Menu_t *pMenu){
	pUISystem->nMessageStage=MESSAGESTAGE_IDLE;
	DisableTimer(&pUISystem->sKeyTimer);
	return 1;
}

/**
 * @brief MenuMessageBox_Refresh
 * 消息弹窗的界面刷新函数
 * @param pMenu
 * 当前活动窗口
 * @return
 */

int MenuMessageBox_Refresh(UI_Menu_t *pMenu){
	GUI_BITMAP *pBmp_BG = (&bmMessageBoxBG_360_240);
	GUI_BITMAP *pBmp_SubBG = (&bmMessageRect_310_130);
	GUI_RECT Rect;
	const char *MessageDisp=NULL;
	const char *MessageHelp=NULL;

    //FUNC_ENTER

	GUI_SetFont(&GUI_Font20_SongTi_Unicode);
	GUI_SetColor(GUI_WHITE);
	GUI_SetBkColor(GUI_BLACK);
	//--------------------------------------------------------------------------//
	//	Purpose	:	绘制背景
	//--------------------------------------------------------------------------//
	GUI_DrawBitmap(pBmp_BG, 
							AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize), 
							AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize));
	GUI_DrawBitmap(pBmp_SubBG, 
							AUTO_GET_X0_FROM_WIDTH(pBmp_SubBG->XSize), 
							(AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_ALL_HEIGHT + BUTTON_V_INTERVAL_COMMON));

	//--------------------------------------------------------------------------//
	//	Purpose	:	填写Title
	//--------------------------------------------------------------------------//
    //FUNC_ENTER
    GUI_SetTextMode(GUI_TM_TRANS);
    //zqh 2015年3月30日 modify，当进入系统升级菜单时，将Title的“提示信息”改为“软件版本”
    if(memcmp(pUISystem->pMessageDisplay, "Ver:", 4) == 0)//zqh add
        GUI_DispStringHCenterAt(SoftwareVersion[pUISystem->allData.Language],
                            (LCD_WIDTH / 2),
                            (AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_BELOW_HEIGHT));

    else
        //zqh 2015年4月15日 add，在提示“系统信息”菜单的title，将“提示信息”改为“系统信息”
        if(memcmp(pUISystem->pMessageDisplay, "Linux", 5) == 0)//zqh add
            GUI_DispStringHCenterAt(SystemInfo[pUISystem->allData.Language],
                                (LCD_WIDTH / 2),
                                (AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_BELOW_HEIGHT));
    else
        GUI_DispStringHCenterAt(Message[pUISystem->allData.Language],
							(LCD_WIDTH / 2), 
							(AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_BELOW_HEIGHT));

	//--------------------------------------------------------------------------//
	//	Purpose	:	填写显示内容
	//	Purpose	:	绘制下方提示信息
	//--------------------------------------------------------------------------//
    //FUNC_ENTER
	Rect.x0 = AUTO_GET_X0_FROM_WIDTH(pBmp_SubBG->XSize);
	Rect.y0 = AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize);
	Rect.x1 = AUTO_GET_X0_FROM_WIDTH(pBmp_SubBG->XSize) + pBmp_SubBG->XSize;
	Rect.y1 = AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + pBmp_BG->YSize;
    //FUNC_ENTER
	switch (pUISystem->nMessageStage){
		case MESSAGESTAGE_OK:
			MessageDisp=pUISystem->pMessageOK;
			//zqh 2015年3月27日modify，将HelpTip2改为HelpTip7
            MessageHelp=HelpTip7[pUISystem->allData.Language];
			break;
		case MESSAGESTAGE_FAIL:	
			MessageDisp=pUISystem->pMessageFail;	
			//zqh 2015年3月27日modify，将HelpTip2改为HelpTip7
            MessageHelp=HelpTip7[pUISystem->allData.Language];
			break;
		case MESSAGESTAGE_RUN:
		default:
            //FUNC_ENTER
			MessageDisp=pUISystem->pMessageDisplay;	
			if (VALIDFUNCTION(pUISystem->pMessageUser_fn)){
                //FUNC_ENTER
				//zqh 2015年3月27日modify，将HelpTip1改为HelpTip8
				MessageHelp=HelpTip8[pUISystem->allData.Language];
			}else{
                //FUNC_ENTER
                //zqh 2015年3月27日modify，将HelpTip2改为HelpTip7,
                //当进入系统升级菜单时,提示不同的帮助信息,提示帮助信息为："按任意键进入系统升级菜单"
                if(memcmp(pUISystem->pMessageDisplay, "Ver:", 4) == 0)//zqh add
                    MessageHelp=HelpTip9[pUISystem->allData.Language];//zqh add
                else
                    MessageHelp=HelpTip7[pUISystem->allData.Language];
			}
			break;
	}
	if (MessageDisp==NULL){
		MessageDisp=pUISystem->pMessageDisplay;		
	}

	GUI_DispStringInRect(MessageDisp, &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);

    //zqh 2015年5月25日 add，将提示信息的字体都变小。
    if(pUISystem->allData.Language == ENGLISH)
        GUI_SetFont(&GUI_Font8x16);

	GUI_DispStringAt(MessageHelp, 
					AUTO_GET_X0_FROM_WIDTH(pBmp_SubBG->XSize), 
					(AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + pBmp_BG->YSize - GUI_Font20_SongTi_Unicode.YSize - PROCESS_V_OFFSET_BG_BOTTON));

    //temp--设定刷新区域
	SetRefreshAreaPos(OSDPOS_BASEMENU, 
					AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize), 
					AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize), 
					pBmp_BG->XSize, 
					pBmp_BG->YSize, 
					ALPHA_COMMON,
					TRUE);
    return 1;
}

/*
* Codec Setting Menu
*/

/**
 * @brief ChangeCodecSettingContent
 * 修改色度、对比度、亮度的调节。
 * @param pData
 * 上次的数值
 * @param nOffset
 * 步进值，一般为1或者-1
 * @param nRange
 * 色度、亮度、对比度的范围
 * @return
 */
int ChangeCodecSettingContent(INT16U * pData,int nOffset,int nRange)
{
	int tmpval = *pData;
	tmpval+=nOffset;
	if(tmpval<0)
		tmpval = nRange-1;
	if(tmpval>=nRange)
		tmpval = 0;
	*pData = tmpval;
	return 1;
}

/**
 * @brief ChangeCodecSetting
 * 对系统的亮度、色度、对比度、编码质量的设定及界面的显示
 * @param pMenu
 * 当前窗口
 * @param nOffset
 * 偏移量，一般为1或-1
 * @return
 */

int ChangeCodecSetting(UI_Menu_t *pMenu,int nOffset)
{
	switch(pMenu->nButtonIdx)
	{
    case MenuCodecSetting_Button_ID_CodecLevel://编码质量的设定
		ChangeCodecSettingContent(&pUISystem->allData.codec_info.codec_level,
			nOffset,CODECLEVEL_RANGE);
		break;
    case MenuCodecSetting_Button_ID_Brightness://亮度的设定
		ChangeCodecSettingContent(&pUISystem->allData.codec_info.brightness,
			nOffset,BRIGHTNESS_RANGE);
		break;
    case MenuCodecSetting_Button_ID_Chroma://色度的设定
		ChangeCodecSettingContent(&pUISystem->allData.codec_info.chroma,
			nOffset,CHROMA_RANGE);
		break;
    case MenuCodecSetting_Button_ID_Contrast://对比度的设定
		ChangeCodecSettingContent(&pUISystem->allData.codec_info.contrast,
			nOffset,CONTRAST_RANGE);
		break;
	default://never should happen
		break;
	}

	if (VALIDFUNCTION(PACTIVE_MENU->menuRefreshItem)){
        if (MenuRefreshItemOnly(PACTIVE_MENU->nButtonIdx)>0){//刷新显示界面
			RefreshOSDDisplay();
			return 1;
		}else{
			return 0;
		}
	}else {
		return MenuRefresh();
	}

	return 1;
}

/**
 * @brief MenuCodecSetting_MsgHandle
 * 亮度、对比度、灰度等界面的按键消息处理函数
 * @param pMenu
 * 当前活动窗口
 * @param nMsgType
 * 消息类型
 * @param nMsgData
 * 消息数据
 * @return
 */
TMsgHandleStat MenuCodecSetting_MsgHandle(UI_Menu_t *pMenu,unsigned short nMsgType,unsigned short nMsgData)
{
	TMsgHandleStat retval=MsgHandled;

	if (nMsgType!=MSG_KEY_EVENT){
		return MsgNocare;
	}
	switch(nMsgData){
        case KEY_UP://光标上移
			MenuChangeFocus(-1);
			break;
        case KEY_DOWN://光标下移
			MenuChangeFocus(1);
			break;
		case KEY_LEFT:
        case KEY_RIGHT://调节灰度、亮度、对比度、编码质量值的变化
			ChangeCodecSetting(pMenu,(nMsgData == KEY_LEFT?-1:1));
			break;
//		case KEY_MENU:
//			SaveAllDataFlash();
//			SystemChangeWindow(MENUBLANK, 1, 1,0);
//			break;
		case KEY_ESC:
            SaveAllDataFlash();//将对比度、亮度、灰度、编码质量数据保存在/opt/.pvr_config.cfg文件中
            LOGT("MenuRestore\r\n");
			SystemMenuRestore();
			break;
		default:
			retval=MsgNeedDispatch;
			break;
	}
	return retval;

}

/**
 * @brief MenuCodecSetting_Init
 * 编码设置窗口的初始化，比如：初次显示时，光标的位置。
 * @param pMenu
 * 当前活动窗口
 * @return
 */

int MenuCodecSetting_Init(UI_Menu_t *pMenu)
{
	int i;
	GUI_SetBkColor(GUI_BLACK);
	MenuButtonMaskBitmap[pMenu->nMenuIdx]=0;
	for (i=0;i<BUTTON_CNT_CODECSETTING_LEFT;i++){
		MenuButtonMaskBitmap[pMenu->nMenuIdx]|=(1<<i);
	}

	pMenu->nButtonIdx=MenuSetUsefulButton(pMenu->nMenuIdx);
	return 1;
}

/**
 * @brief MenuCodecSetting_RefreshItem
 * 绘制编码参数界面，包括：显示字符，如对比度、亮度、灰度等。
 * @param pMenu
 * 当前活动窗口
 * @param nIdx
 * 选中的button值
 * @param nFocus
 * 绘制选中button
 * @return
 */

int MenuCodecSetting_RefreshItem(struct UI_Menu_tag *pMenu,int nIdx,int nFocus)
{
	INT32U bmp_x0;
	INT32U bmp_y0;
	INT32U bmp_FirstList_x0;
	INT32U bmp_FirstList_y0;
	INT32U tmp_interval;
	INT16U Disp[BUTTON_CNT_CODECSETTING_LEFT];

	GUI_BITMAP *pBmp_BG = &bmMainMenuBG_360_320;
	GUI_BITMAP *pBmp_Sel = &bmButton_Combox_Orange_148_30;
	GUI_BITMAP *pBmp_UnSel = &bmButton_Combox_DBlue_148_30;
	GUI_BITMAP *pBmp_Left = &bmButton_Flat_DBlue_148_30;

	tmp_interval = (pBmp_BG->YSize - TITLE_ALL_HEIGHT - pBmp_Sel->YSize * BUTTON_CNT_CODECSETTING_LEFT) / (BUTTON_CNT_CODECSETTING_LEFT + 1);
	bmp_FirstList_x0 = AUTO_GET_X0_FROM_WIDTH(pBmp_Sel->XSize+BUTTON_H_INTERVAL_COMMON +pBmp_Left->XSize);
	bmp_FirstList_y0 = AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_ALL_HEIGHT + tmp_interval;

	bmp_x0 = bmp_FirstList_x0+(BUTTON_H_INTERVAL_COMMON + pBmp_Left->XSize) * (nIdx/BUTTON_CNT_CODECSETTING_LEFT);
	bmp_y0 =bmp_FirstList_y0 + (pBmp_Sel->YSize + tmp_interval) * (nIdx%BUTTON_CNT_CODECSETTING_LEFT);

	//画Button
	if(nIdx<BUTTON_CNT_CODECSETTING_LEFT)
	{
		GUI_DrawBitmap(pBmp_Left, bmp_x0, bmp_y0);
		//写Content
		GUI_DispStringHCenterAt(CodecSetLeftList[nIdx], 
							(bmp_x0 + pBmp_UnSel->XSize / 2),
							(bmp_y0 + BUTTON_V_OFFSET_DISPLAY));

	}else{
		if(nFocus){	//绘制高亮选中条目
			GUI_DrawBitmap(pBmp_Sel, bmp_x0, bmp_y0);
		}else{			//绘制非高亮条目
			GUI_DrawBitmap(pBmp_UnSel, bmp_x0, bmp_y0);
		}
		if(nIdx == MenuCodecSetting_Button_ID_CodecLevel)
		{
			GUI_DispStringHCenterAt(CodecLevelList[pUISystem->allData.codec_info.codec_level],
							(bmp_x0 + pBmp_UnSel->XSize / 2),
							(bmp_y0 + BUTTON_V_OFFSET_DISPLAY));
		}
		else
		{
			memcpy(Disp,&pUISystem->allData.codec_info,sizeof(CODEC_INFO));
			GUI_DispDecAt(Disp[nIdx%BUTTON_CNT_CODECSETTING_LEFT], 
							(bmp_x0 + pBmp_UnSel->XSize / 2-15),
							(bmp_y0 + BUTTON_V_OFFSET_DISPLAY),3);
		}
	}

    //NOTE: Do not SetRefreshArea should notify
	return 1;
}

//--------------------------------------------------------------------------//
//	Name		:	Pvr_File_Button_SetFocus
//	Purpose		:	绘制磁盘选择界面
//	Parameter	:	@Index - 所选中的项目索引。0 ~ (BUTTON_CNT_STORAGEINFO - 1)
//--------------------------------------------------------------------------//
//
void Pvr_CodecSetting_SetFocus(INT8S Index)
{
	INT8U i;
	GUI_BITMAP *pBmp_BG = &bmMainMenuBG_360_320;
	
	//变量保护
	Index = (Index > (BUTTON_CNT_CODECSETTING- 1)) ? 0 : Index;

	//绘制背景
	GUI_DrawBitmap(pBmp_BG, 
						AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize), 
						AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize));

	//填写Title:FileBrowser
	GUI_SetTextMode(GUI_TM_TRANS);
	GUI_DispStringHCenterAt(CodecSetting[pUISystem->allData.Language], 
							(LCD_WIDTH / 2), 
							(AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_BELOW_HEIGHT));

	//绘制Button(共BUTTON_CNT_STORAGEINFO个,索引为0 ~ (BUTTON_CNT_STORAGEINFO - 1))
	for(i = 0; i < BUTTON_CNT_CODECSETTING; i++)
	{
		MenuRefreshItemOnly(i);
	}

	//设定刷新区域
	SetRefreshAreaPos(OSDPOS_BASEMENU, 
					AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize), 
					AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize), 
					pBmp_BG->XSize,
					pBmp_BG->YSize,
					ALPHA_COMMON,
					TRUE);
}

/**
 * @brief MenuCodecSetting_Refresh
 * 编码参数界面的刷新
 * @param pMenu
 * @return
 */
int MenuCodecSetting_Refresh(UI_Menu_t *pMenu)
{
	Pvr_CodecSetting_SetFocus(pMenu->nButtonIdx);
	return 1;
}


/*
*device scan msg Menu
*/
/**
 * @brief MenuDev_Status_MSG_TO
 * 将变量pUISystem->SysState.CurState重新设置成MESSAGESTAGE_IDLE，
 * 并假如为系统启动时cf卡未插入，调用窗口切换函数.
 * 调用此函数是发生在这种情况下：应用程序启动之前，CF卡挂载在板子上，但是应用程序启动后，cf卡被拔出，会调用此函数
 * @param pTimer
 * @return
 */
TMsgHandleStat MenuDev_Status_MSG_TO(TEF_TIMER_STRUCT *pTimer)
{
	unsigned int stat = pUISystem->nDevCurStatusMsg;
	FUNC_ENTER;

	pUISystem->nDevCurStatusMsg = MESSAGESTAGE_IDLE;
	DisableTimer(pTimer);
// switch lvds while recording
	if(TESTBIT(pUISystem->SysState.CurState,SYS_BIT_RECORDING_ONE_CLICK))
	{
		SetLvdsSwitch(1);
	}

    if(stat == MESSAGESTAGE_CFCARDREMOVE)//系统启动时，cf卡未插入，则进入if里边
    {
        LOGT("change to blank\r\n");
        SystemChangeWindow(MENUBLANK,1,1,0);//切换到blank窗口
    }
	else
    {
        LOGT("menu restore\r\n");
		SystemMenuRestore();
    }
	MenuRefresh();
	return MsgHandled;
}

/**
 * @brief MenuDev_Status_MSG_Init
 * 设备状态信息初始化，pUISystem->nDevCurStatusMsg这个变量的初始值为MESSAGESTAGE_IDLE，
 * 在这个函数void Pvr_UI_Start(void)中进行的初始化。
 * @param pMenu
 * 当前活动窗口
 * @return
 */
int MenuDev_Status_MSG_Init(UI_Menu_t *pMenu)
{
	FUNC_ENTER

	if(pUISystem->nDevCurStatusMsg == MESSAGESTAGE_IDLE)
	{
		return 0;
	}

	return 1;
}

/**
 * @brief MenuDev_Status_MSG_Close
 * 设备状态信息关闭，主要功能就是关闭定时器
 * @param pMenu
 * 当前活动窗口
 * @return
 */
int MenuDev_Status_MSG_Close(UI_Menu_t *pMenu)
{
	FUNC_ENTER
    DisableTimer(&pUISystem->sKeyTimer);//关闭定时器
	return 1;
}

/**
 * @brief MenuDev_Status_MSG_Open
 * 打开设备状态信息
 * @param pMenu
 * 当前活动窗口
 * @return
 */

int MenuDev_Status_MSG_Open(UI_Menu_t *pMenu)
{
	int msg;

	FUNC_ENTER

    if(pUISystem->nDevCurStatusMsg == MESSAGESTAGE_IDLE)//无设备消息，函数直接返回
	{
		ERR("%s:no msg\n",__func__);
		return 0;
	}
	msg = pUISystem->nDevCurStatusMsg;

    //Init Timer，有新消息，则调用如下函数，并初始化
	EnableTimer(&pUISystem->sKeyTimer,1,TIMER_MESSAGEBOX,DEV_MESSAGE_DELAY,MenuDev_Status_MSG_TO);

	//Set Current msg

	MenuRefresh();

	return 1;
}

/**
 * @brief MenuDev_Status_MSG_MsgHandle
 * 处理设备状态的调用函数，如cf卡移除或者cf卡没有空间了。此函数会在void Pvr_UI_Start(void)这个函数中被调用，在cf卡被移除的情况下调用。
 * @param pMenu
 * 当前活动窗口
 * @param nMsgType
 * 消息类型
 * @param nMsgData
 * 忽略
 * @return
 */

TMsgHandleStat MenuDev_Status_MSG_MsgHandle(UI_Menu_t *pMenu,unsigned short nMsgType,unsigned short nMsgData)
{
	TMsgHandleStat retval=MsgHandled;
	unsigned int stat = pUISystem->nDevCurStatusMsg;

	FUNC_ENTER

	if (nMsgType!=MSG_KEY_EVENT)
	{
		return MsgNocare;
	}

	pUISystem->nDevCurStatusMsg = MESSAGESTAGE_IDLE;
    DisableTimer(&pUISystem->sKeyTimer);//禁用定时器

	if(stat == MESSAGESTAGE_CFCARDREMOVE)
    {
        LOGT("change to blank\r\n");
		SystemChangeWindow(MENUBLANK,1,1,0);
    }
	else
    {
        LOGT("menu restore\r\n");
		SystemMenuRestore();
    }
	MenuRefresh();
	return retval;
}

/**
 * @brief MenuDev_Status_MSG_Refresh
 * 设备状态刷新函数，显示消息：如cf卡、sd卡、u盘移除
 * @param pMenu
 * 当前活动窗口
 * @return
 */\

int MenuDev_Status_MSG_Refresh(UI_Menu_t *pMenu)
{
	GUI_BITMAP *pBmp_BG = (&bmMessageBoxBG_360_240);
	GUI_BITMAP *pBmp_SubBG = (&bmMessageRect_310_130);
	GUI_RECT Rect;
	const char *MessageDisp=NULL;
	const char *MessageHelp=NULL;

	FUNC_ENTER

	GUI_SetBkColor(GUI_BLACK);
	GUI_SetFont(&GUI_Font20_SongTi_Unicode);
	GUI_SetColor(GUI_WHITE);

	//--------------------------------------------------------------------------//
	//	Purpose	:	绘制背景
	//--------------------------------------------------------------------------//
	GUI_DrawBitmap(pBmp_BG, 
							AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize), 
							AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize));
	GUI_DrawBitmap(pBmp_SubBG, 
							AUTO_GET_X0_FROM_WIDTH(pBmp_SubBG->XSize), 
							(AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_ALL_HEIGHT + BUTTON_V_INTERVAL_COMMON));

	//--------------------------------------------------------------------------//
	//	Purpose	:	填写Title
	//--------------------------------------------------------------------------//
	GUI_SetTextMode(GUI_TM_TRANS);
	GUI_DispStringHCenterAt(Message[pUISystem->allData.Language], 
							(LCD_WIDTH / 2), 
							(AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_BELOW_HEIGHT));

	//--------------------------------------------------------------------------//
	//	Purpose	:	填写显示内容
	//	Purpose	:	绘制下方提示信息
	//--------------------------------------------------------------------------//

	Rect.x0 = AUTO_GET_X0_FROM_WIDTH(pBmp_SubBG->XSize);
	Rect.y0 = AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize);
	Rect.x1 = AUTO_GET_X0_FROM_WIDTH(pBmp_SubBG->XSize) + pBmp_SubBG->XSize;
	Rect.y1 = AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + pBmp_BG->YSize;



	switch(pUISystem->nDevCurStatusMsg)
	{
	case MESSAGESTAGE_UDISKREMOVE:
		MessageDisp = UDiskRemoved[pUISystem->allData.Language];
        //zqh 2015年3月27日 modify，将“按任意键退出”改为“按退出键退出”
        MessageHelp = HelpTip7[pUISystem->allData.Language];
		break;
	case MESSAGESTAGE_UDISKDETECT:
		MessageDisp = UDiskDetected[pUISystem->allData.Language];
        //zqh 2015年3月27日 modify，将“按任意键退出”改为“按退出键退出”
        MessageHelp = HelpTip7[pUISystem->allData.Language];
		break;
	case MESSAGESTAGE_DISKFULL:
		MessageDisp = CFCardFull[pUISystem->allData.Language];
        //zqh 2015年3月27日 modify，将“按任意键退出”改为“按退出键退出”
        MessageHelp = HelpTip7[pUISystem->allData.Language];
		break;
	case MESSAGESTAGE_SDREMOVE:
		MessageDisp = SDRemoved[pUISystem->allData.Language];
		MessageHelp = HelpSDRemove[pUISystem->allData.Language];
		break;
	case MESSAGESTAGE_SDDETECT:
		MessageDisp = SDDetected[pUISystem->allData.Language];
        //zqh 2015年3月27日 modify，将“按任意键退出”改为“按退出键退出”
        MessageHelp = HelpTip7[pUISystem->allData.Language];//
		break;
	case MESSAGESTAGE_CFCARDREMOVE:
		MessageDisp = CFRemoved[pUISystem->allData.Language];
        //zqh 2015年3月27日 modify，将“按任意键退出”改为“按退出键退出”
        MessageHelp = HelpTip7[pUISystem->allData.Language];
		break;
	case MESSAGESTAGE_CFCARDDETECT:
		MessageDisp = CFDetected[pUISystem->allData.Language];
        //zqh 2015年3月27日 modify，将“按任意键退出”改为“按退出键退出”
        MessageHelp = HelpTip7[pUISystem->allData.Language];
		break;
	case MESSAGESTAGE_IDLE:
		break;
	default:
		pUISystem->nDevCurStatusMsg = MESSAGESTAGE_IDLE;
		ERR("%s :Invalid Message\n",__func__);
		return -1;
	}


	GUI_DispStringInRect(MessageDisp, &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
	GUI_DispStringAt(MessageHelp, 
					AUTO_GET_X0_FROM_WIDTH(pBmp_SubBG->XSize), 
					(AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) 
						+ pBmp_BG->YSize - GUI_Font20_SongTi_Unicode.YSize 
						- PROCESS_V_OFFSET_BG_BOTTON));


	//temp--设定刷新区域
	SetRefreshAreaPos(OSDPOS_BASEMENU, 
					AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize), 
					AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize), 
					pBmp_BG->XSize, 
					pBmp_BG->YSize, 
					ALPHA_COMMON,
					TRUE);
	return 1;
}


/*
* System Updata Menu
*/
static char Update_SourceFile[MAX_FS_PATH];
static char Update_DestFile[MAX_FS_PATH];
static char Update_BackUpFile[MAX_FS_PATH];

#if 0
// TODO: update tmp file : pvr.t daemon.t ...
static char Update_DestTmpFile[MAX_FS_PATH];
#endif

static int Update_In_Progress = 0;

/*
 * Update complete/failed, reboot pvr and remount system in daemon
 */
int update_reboot_code = EXIT_CODE_NORMAL;

/**
 * @brief Do_Reboot
 * 系统需要重启时，调用此函数
 * @param ignored
 * @return
 */
int Do_Reboot(int ignored)
{
	FUNC_ENTER;
	LOGT("restart exit\r\n");
	request_exit_main_task = 1;
	exit_code = update_reboot_code;

    SetLvdsSwitch(1);

    // NOTE: update need test
    // do not use watchdog to reboot
    // do reboot to remount rfs to ro
#if DEBUG_V2_1
//	// disable hardware wdt
//	EnableHardwareWDT(0);
    sync();
    sync();
    sync();

    LOGE("reboot in daemon\r\n");
//    request2reboot();
//    while(1)
//    {
//        sleep(1);
//    }
#else
    system("shutdown -r -n now");
#endif

	return 1;
}

/**
 * @brief Update_Reboot
 * 系统更新完成后，调用此函数，将界面切换到文件浏览窗口，以及显示窗体：“系统重启”。
 * @param ignored
 * 忽略此参数
 * @return
 */
int Update_Reboot(int ignored)
{
	FUNC_ENTER;
    SystemChangeWindow(MENUFILEBROWSER,1,1,0);//切换至文件浏览窗口
    //显示消息窗体：“系统重启”，并调用int Do_Reboot(int ignored)这个函数，对系统进行重启
    //zqh 2015年4月13日 modify，将显示“等待系统重启”菜单的显示时间有10S(DEV_MESSAGE_DELAY)改为3S(SYSTEMTOREBOOT_MESSAGE_DELAY)。
    MessageBox(SystemReboot[pUISystem->allData.Language],SYSTEMTOREBOOT_MESSAGE_DELAY,NULL,NULL,NULL,NULL,NULL,Do_Reboot);
	return 1;
}

/**
 * @brief StartCopyFile
 * 拷贝文件函数
 * @param src_file
 * 源文件
 * @param dst_file
 * 目的文件
 * @return
 */
int StartCopyFile(char *src_file,char *dst_file)
{
	if((src_file==NULL)||(dst_file==NULL))
	{
		ERR("start copy file error\r\n");
		return -1;
	}
	strncpy(pUISystem->sFileCopyFrom,src_file,MAX_FS_PATH);
	strncpy(pUISystem->sFileCopyTo,dst_file,MAX_FS_PATH);

	Pvr_Waiting(TRUE);

    SystemChangeWindow(MENUDOFILECOPY,1,1,1);//切换至文件拷贝窗口
	return 0;
}

/**
 * @brief SetUpdateFileName
 * 合成源文件、目的文件的全路径，比如：/media/cf/daemon
 * @param src_path
 * 源路径，例如：/media/cf
 * @param dst_path
 * 目的路径，例如：/media/usb
 * @param filename
 * 要拷贝的文件名字，例如：daemon
 * @return
 */
int SetUpdateFileName(char *src_path,char *dst_path,char *filename)
{
//	int pathlen;
//	int ret;
//	char errstr[MAX_FS_PATH];

	FUNC_ENTER;

	// set src file name
	memset(Update_SourceFile,0,sizeof(Update_SourceFile));
//	pathlen = strlen(src_path);//UPDATA_SRC_FILE_PATH
	strcpy(Update_SourceFile,src_path);
//	if(Update_SourceFile[pathlen-1]!='/')
//	{
//		strcat(Update_SourceFile,"/");
//	}
	strcat(Update_SourceFile,filename);
	LOGT("src file path = %s\n",Update_SourceFile);

	// set dst file name
	memset(Update_DestFile,0,sizeof(Update_DestFile));
	strcpy(Update_DestFile,dst_path);//UPDATA_DST_FILE_PATH
	strcat(Update_DestFile,filename);
	LOGT("dst file path = %s\r\n",Update_DestFile);

#if	0//def DEBUG_V2_1
	// set dest tmp file name
	strcpy(Update_DestTmpFile,Update_DestFile);
	strcat(Update_DestTmpFile,UPDATE_TMP_FILE_DES);
	LOGT("dest tmp file path = %s\r\n",Update_DestTmpFile);
#endif

	// set back up file name
	memset(Update_BackUpFile,0,sizeof(Update_BackUpFile));
	strcpy(Update_BackUpFile,Update_DestFile);
	strcat(Update_BackUpFile,UPDATE_BACK_FILE_DES);
	LOGT("back up file path = %s\r\n",Update_BackUpFile);

	return 0;
}

// check update file exist
/**
 * @brief CheckUpdateFile
 * 检查在U盘中有以pvr、uImage、daemon命名的文件
 * @param filename
 * 参数值为pvr、daemon、uImage
 * @return
 */
int CheckUpdateFile(char * filename)
{
	FILE * fp;
	char tmp_src[MAX_FS_PATH];
	int pathlen;

	FUNC_ENTER;

	memset(tmp_src,0,sizeof(tmp_src));
    pathlen = strlen(UPDATE_SRC_FILE_PATH);//UPDATE_SRC_FILE_PATH：假设为/media/usb
	sprintf(tmp_src,UPDATE_SRC_FILE_PATH);
    if(tmp_src[pathlen-1]!='/')//UPDATE_SRC_FILE_PATH假设字符串中最后一个字符不是'/'则将'/'添加在UPDATE_SRC_FILE_PATH字符串中
	{
		strcat(tmp_src,"/");
	}
    strcat(tmp_src,filename);//合成要打开文件的全路径：比如：/media/usb/pvr

	LOGT("fopen %s\r\n",tmp_src);
	fp = fopen(tmp_src,"rb");
	if(fp == NULL)
	{
		ERR("file %s not exist\r\n",filename);
		return -1;
	}
	fclose(fp);

	FUNC_EXIT;

	return 0;
}

int static do_update_kernel = 0;
/**
 * @brief DoUpdateKernel
 * 更新内核
 * @param pUser
 * 内核名字uImage
 * @return
 */
int DoUpdateKernel(void *pUser)
{
	char tmp_str[0x100];
//	int pathlen;

	FUNC_ENTER;

	Pvr_Waiting(TRUE);

	START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_KERNEL_UPDATE);

	Update_In_Progress = 1;
	do_update_kernel = 1;

	LOGT(UPDATE_KERNEL_CMD1);
	system(UPDATE_KERNEL_CMD1);

    // NOTE: test update kernel
#if 1
	sprintf(tmp_str,UPDATE_KERNEL_CMD2" %s%s",UPDATE_SRC_FILE_PATH,(char *)pUser);
#else
	sprintf(tmp_str,UPDATE_KERNEL_CMD2" %s",UPDATE_SRC_FILE_PATH);
	pathlen = strlen(tmp_str);
	if(tmp_str[pathlen-1]!='/')
	{
		strcat(tmp_str,"/");
	}
	strcat(tmp_str,(char *)pUser);
#endif
	LOGT("%s",tmp_str);
	system(tmp_str);

	sync();
	sync();

	Update_In_Progress = 0;

	STOP_WATCHDOG_TIMER_SIG();

	return 1;
}

/**
 * @brief UpdateKernelDeinit
 * 内核更新完后，调用此函数进行反初始化
 * @param pUser
 *
 * @return
 */
int UpdateKernelDeinit(void *pUser)
{
	if(Update_In_Progress)
	{
		LOGT("Update in progress,do not return parent\r\n");
		return 0;
	}
	else
	{
		if(do_update_kernel)
		{
			update_reboot_code = EXIT_CODE_REBOOT;
			Update_Reboot(1);
			return 1;
		}
		else
		{
			return SystemChangeWindow(MENUFILEBROWSER,1,1,0);;
		}
	}
}

/**
 * @brief SetUpdateInProgress
 * create or delete update lock file to indicate update "in"/"not in" progress
 * @param stat
 * stat = 1 : update in progress. create lock file
 * stat = 0 : update complete. remove lock file
 */
void SetUpdateInProgress(int stat)
{
	FILE * fp = NULL;
	if(stat)
	{
		fp = fopen(UPDATE_LOCK_FILE,"wr");

		if(fp ==NULL)
		{
			LOGE("set update lock file error\r\n");
		}else//SACH:W10865
		{
			fclose(fp);
		}
		
	}
	else
	{
		if(remove(UPDATE_LOCK_FILE))
		{
			LOGE("remove update lock file error : %s\r\n",strerror(errno));
		}
	}
}


/** zqh 2015年5月20日 add
 * @brief SetLanguage_Deinit
 * 当系统语言设置完成后，系统界面切回menublank状态。
 * @param nOKFail
 * @return
 */
int SetLanguage_Deinit(int nOKFail)
{
    SystemChangeWindow(MENULANGUAGESEL,1,1,0);//窗口切换
    return 1;
}


//zqh 2015年4月1日 add，添加“语言”菜单的按键消息处理函数
int LanguageSwitchFlag = 0;

TMsgHandleStat MenuLanguageSel_MsgHandle(UI_Menu_t *pMenu,unsigned short nMsgType,unsigned short nMsgData)
{
    TMsgHandleStat retval=MsgHandled;
    //int ButtonIdx = pMenu->nButtonIdx;

    if (nMsgType!=MSG_KEY_EVENT){
        return MsgNocare;
    }

    switch(nMsgData){
        case KEY_UP:
            MenuChangeFocus(-1);
            break;

        case KEY_DOWN:
            MenuChangeFocus(1);
            break;

        case KEY_ESC:
            if(LanguageSwitchFlag){
                //SystemChangeWindow(MENUBLANK,1,1,0);

                #if (USE_MOUNT_CF_THREAD) || (USE_MOUNT_CF_THREAD_S)
                START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_DELETE_FILE);
                if(CF_Mount_Runing)
                {
                    StopMountCF_Thread();
                }
                STOP_WATCHDOG_TIMER_SIG();
                #endif

                SystemUmountDev(CF_MOUNT_DIR);
                SetLvdsSwitch(1);

                LOGT("MenuLanguage exit \r\n");
                request_exit_main_task = 1;
                exit_code = EXIT_CODE_NORMAL;
                LanguageSwitchFlag = 0;
            }
            else
                SystemChangeWindow(MENUSYSTEMFUNC,1,1,0);
            break;

        case KEY_ENTER:
            if(pUISystem->allData.Language != pMenu->nButtonIdx)
                LanguageSwitchFlag = 1;
            else
                LanguageSwitchFlag = 0;

            pUISystem->allData.Language = pMenu->nButtonIdx;
            system("mount / -o remount,rw");
            SaveAllDataFlash();
            system("mount / -o remount,ro");
            printf("pUISystem->allData.Language = %d, LanguageSwitchFlag = %d\n", pUISystem->allData.Language,LanguageSwitchFlag);
            //printf("msgbox48[] = %s\n", MsgBox48[pUISystem->allData.Language]);
            MessageBox(MsgBox48[pUISystem->allData.Language],CODEC_MESSAGE_DELAY,
                NULL,NULL,NULL,
                NULL,NULL,SetLanguage_Deinit);
            break;

        default:
            //retval=MsgNeedDispatch;
            break;
    }
    return retval;
}

/**
 * @brief MenuLanguageSel_Init
 * 系统更新初始化函数，设定界面的初始选中button
 * @param pMenu
 * 当前活动窗口
 * @return
 */
int MenuLanguageSel_Init(UI_Menu_t *pMenu)
{
    MenuButtonMaskBitmap[pMenu->nMenuIdx] = 1<<BUTTON_CNT_LANGUAGE;
    pMenu->nButtonIdx = pUISystem->allData.Language;
    return 1;
}

/**zqh 2015年4月1日 add
 * @brief MenuLanguageSel_Refresh_Item
 * 绘制“语言”菜单， 绘制button，比如：简体中文、繁體中文、English
 * @param pMenu
 * 当前活动窗口
 * @param nIdx
 * 当前窗口的button值，取值范围从0-2.
 * @param nFocus
 * 是否将当前button设置成高亮状态
 * @return
 */

int MenuLanguageSel_Refresh_Item(struct UI_Menu_tag *pMenu,int nIdx,int nFocus)
{
    INT32U bmp_x0;
    INT32U bmp_y0;
    INT32U bmp_FirstList_x0;
    INT32U bmp_FirstList_y0;
    INT32U tmp_interval;
    GUI_BITMAP *pBmp_BG;
    GUI_BITMAP *pBmp_Sel;
    GUI_BITMAP *pBmp_UnSel;
    GUI_BITMAP *pBmp_Invalid;

    GUI_SetFont(&GUI_Font20_SongTi_Unicode);
    GUI_SetColor(GUI_WHITE);
    GUI_SetBkColor(GUI_BLACK);

    pBmp_BG = (&bmMainMenuBG_360_320);
    pBmp_UnSel = (&bmFileList_Flat_GBlue_240_30);
    pBmp_Sel = (&bmFileList_Flat_Orange_240_30);


    tmp_interval = (pBmp_BG->YSize - TITLE_ALL_HEIGHT- pBmp_Sel->YSize * (BUTTON_CNT_SYSTEMFUNC+1)) / (BUTTON_CNT_SYSTEMFUNC+2);
    bmp_FirstList_x0 = AUTO_GET_X0_FROM_WIDTH(pBmp_Sel->XSize);
    bmp_FirstList_y0 = AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize)+TITLE_ALL_HEIGHT + tmp_interval;

    bmp_x0 = bmp_FirstList_x0;
    bmp_y0 = bmp_FirstList_y0 + (pBmp_Sel->YSize + tmp_interval) * (nIdx);

    //画Button
    if(nFocus){	//绘制高亮选中条目
        GUI_DrawBitmap(pBmp_Sel, bmp_x0, bmp_y0);
    }else{			//绘制非高亮条目
        GUI_DrawBitmap(pBmp_UnSel, bmp_x0, bmp_y0);
    }

    if((1<<nIdx)&MenuButtonMaskBitmap[pUISystem->nMenuIdx])
    {	//绘制需要跳过的条目
        GUI_SetColor(GUI_GRAY);

        //写Content
        GUI_DispStringHCenterAt(LanguageList[nIdx],
                                (bmp_x0 + pBmp_UnSel->XSize / 2),
                                (bmp_y0 + BUTTON_V_OFFSET_DISPLAY));
        GUI_SetColor(GUI_WHITE);

    }
    else
    {
        //写Content
        GUI_DispStringHCenterAt(LanguageList[nIdx],
                                (bmp_x0 + pBmp_UnSel->XSize / 2),
                                (bmp_y0 + BUTTON_V_OFFSET_DISPLAY));
    }

    //NOTE: Do not SetRefreshArea should notify
    return 1;

}

/** zqh 2015年4月1日 add
 * @brief MenuLanguageSel_Refresh
 * 刷新系统升级界面函数，实际上是调用MenuLanguageSel_Refresh_Item()这个函数实现的，
 * 此函数只绘制背景图片，还有标题，还有提示帮助信息。
 * @param pMenu
 * 当前活动窗口
 * @return
 */

int MenuLanguageSel_Refresh(UI_Menu_t *pMenu)
{
    INT8U i;
    INT32U bmp_FirstList_x0;
    INT32U bmp_FirstList_y0;
    INT32U tmp_interval;
    GUI_BITMAP *pBmp_BG;
    GUI_BITMAP *pBmp_Sel;
    GUI_BITMAP *pBmp_UnSel;
    int Index;
	const char *MessageHelp=NULL;

    FUNC_ENTER;

    GUI_SetFont(&GUI_Font20_SongTi_Unicode);
    GUI_SetColor(GUI_WHITE);
    GUI_SetBkColor(GUI_BLACK);

    //绘制背景
    pBmp_BG = (&bmMainMenuBG_360_320);
    GUI_DrawBitmap(pBmp_BG,
                            AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize),
                            AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize));

    //填写Title "语言"
    GUI_SetTextMode(GUI_TM_TRANS);
    GUI_DispStringHCenterAt(Language[pUISystem->allData.Language],
                            (LCD_WIDTH / 2),
                            (AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_BELOW_HEIGHT));

    //绘制Button
    for(i = 0; i < BUTTON_CNT_LANGUAGE; i++)
    {
        MenuRefreshItemOnly(i);//调用此函数MenuLanguageSel_Refresh_Item()具体实现button的绘制
    }

    //--------------------------------------------------------------------------//
    //	Purpose	:	填写显示内容
    //	Purpose	:	绘制下方提示信息"按“确认”键設置系統語言"
    //--------------------------------------------------------------------------//
    MessageHelp=HelpTip10[pUISystem->allData.Language];

    if(pUISystem->allData.Language == ENGLISH)
        GUI_SetFont(&GUI_Font8x16);

    GUI_DispStringHCenterAt(MessageHelp,
                    AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize) + pBmp_BG->XSize/2,
                    (AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + pBmp_BG->YSize - GUI_Font20_SongTi_Unicode.YSize - PROCESS_V_OFFSET_BG_BOTTON));

    //设定刷新区域
    SetRefreshAreaPos(OSDPOS_BASEMENU,
                    AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize),
                    AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize),
                    pBmp_BG->XSize,
                    pBmp_BG->YSize,
                    ALPHA_COMMON,
                    TRUE);
}

/**
 * @brief MenuSystemUpdate_Close
 * 什么都不做
 * @param pMenu
 * @return
 */

int MenuSystemUpdate_Close(UI_Menu_t *pMenu)
{
	return 0;
}

/**
 * @brief MenuSystemUpdate_Open
 * 界面刷新
 * @param pMenu
 * 当前活动窗口
 * @return
 */
int MenuSystemUpdate_Open(UI_Menu_t *pMenu)
{
    //! refresh menu only
    MenuRefresh();//界面刷新
    return 0;
}
#if   UPDATELOADMODULESSCRIPT
//2014-11-17添加 :此函数用于 更新/opt目录下的loadmodule-ef.sh脚本文件
void UpdateLoadModuleSh(void)
{
	int ret;
	ret = CheckUpdateFile(UPDATE_LOAD_MODULE_SH_NAME);
	char buffer[200];

	if(ret>=0)
	{
		SetUpdateFileName(UPDATE_SRC_FILE_PATH,UPDATE_DST_FILE_PATH,UPDATE_LOAD_MODULE_SH_NAME);
		sprintf(buffer, "cp %s %s", Update_SourceFile, Update_DestFile);
		system("mount / -o remount,rw");
		system(buffer);
		system("mount / -o remount,ro");
//		buffer[strlen(buffer)] = '\0';
//		printf("buffer = %s\n", buffer);
		return ;
	}
	return ;
}
#endif

/**
 * @brief MenuSystemUpdate_MsgHandle
 * 系统更新界面，按键消息处理函数，以及更新文件系统、内核、显示系统版本、
 * @param pMenu
 * 当前活动窗口
 * @param nMsgType
 * 消息类型
 * @param nMsgData
 * 消息数据
 * @return
 */

TMsgHandleStat MenuSystemUpdate_MsgHandle(UI_Menu_t *pMenu,unsigned short nMsgType,unsigned short nMsgData)
{
	TMsgHandleStat retval=MsgHandled;
	int ret;
	struct utsname testbuff;
	char strbuff[0x100];

	if (nMsgType!=MSG_KEY_EVENT){
		return MsgNocare;
	}

	if(Update_In_Progress)
	{
		return MsgHandled;
	}

	switch(nMsgData){
        case KEY_UP://上移
			MenuChangeFocus(-1);
			break;
        case KEY_DOWN://下移
			MenuChangeFocus(1);
			break;
        case KEY_ESC://退出
			SystemChangeWindow(MENUFILEBROWSER,1,1,0);
            CLEARBIT(pUISystem->SysState.CurState, SYS_BIT_SYSTEMUPDATE);//设置系统更新状态位
            SETBIT(pUISystem->SysState.CurState, SYS_BIT_FILEBROSWER);
			break;
		case KEY_RIGHT:
		case KEY_ENTER:
			switch(pMenu->nButtonIdx)
			{
            case MenuSystemUpdate_Button_ID_SysInfo://系统消息
				ret = uname(&testbuff);
				if(ret<0)
				{
					ERR("%s:Get System Info Error: %s\n",__func__,strerror(errno));
				}
				else
				{
					sprintf(strbuff,"%s\n%s\n%s\n%s",
						testbuff.sysname,
						testbuff.release,
						testbuff.version,
                        testbuff.machine);//弹出系统消息窗口
					MessageBox(strbuff,DEV_MESSAGE_DELAY,
						NULL,NULL,NULL,
						NULL,NULL,NULL);
				}
				break;
            case MenuSystemUpdate_Button_ID_Daemon://升级daemon程序
				ret = DoUDiskDetect();
				if((ret<0)||(!TESTBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_Mount_Flag)))
				{
					ERR("Need Mount UDisk first!\r\n");
					CLEARBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_Mount_Flag);
					MessageBox(HelpMountUDisk[pUISystem->allData.Language],DEV_MESSAGE_DELAY,
                        NULL,NULL,NULL,NULL,NULL,NULL);//弹出“请先插入u盘”消息弹窗
					break;
				}

				if(pUISystem->nFileCopyToStage != MESSAGESTAGE_IDLE)
				{
					ERR("Copy not complete\r\n");
					MessageBox("Copy Not Complete",DEV_MESSAGE_DELAY,
						NULL,NULL,NULL,NULL,NULL,NULL);
					break;
				}
				else
				{
                    ret = CheckUpdateFile(UPDATE_DAEMON_NAME);//检测u盘里是否有daemon文件
					if(ret>=0)
					{
						SetUpdateFileName(UPDATE_SRC_FILE_PATH,UPDATE_DST_FILE_PATH,UPDATE_DAEMON_NAME);
						pUISystem->nFileCopyToStage = MESSAGESTAGE_UPDATE_BACKUP;
						// Set Update in progress
//						Update_In_Progress = 1;
                        // remount rw fs.将根文件系统挂载为可读写的。
                        ret = mount(RFS_MOUNT_NOD,RFS_MOUNT_DST,RFS_MOUNT_TYPE,MS_REMOUNT,0);
						if(ret<0)
						{
							ERR("remount RFS to rw err:%s\r\n",strerror(errno));
//							break;
						}
						update_reboot_code = EXIT_CODE_REBOOT;
                        StartCopyFile(Update_DestFile,Update_BackUpFile);//开始拷贝文件
					}
					else
					{
                        ERR("%s:No update file!\n",__func__);//
						MessageBox(NoUpdateFileDaemon[pUISystem->allData.Language],DEV_MESSAGE_DELAY,
							NULL,NULL,NULL,
                            NULL,NULL,NULL);//弹出“U盘里无daemon”消息弹窗
					}
				}
				break;
            case MenuSystemUpdate_Button_ID_FileSys://更新文件系统
				ret = DoUDiskDetect();
				if((ret<0)||(!TESTBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_Mount_Flag)))
				{
					ERR("Need Mount UDisk first!\r\n");
					CLEARBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_Mount_Flag);
					MessageBox(HelpMountUDisk[pUISystem->allData.Language],DEV_MESSAGE_DELAY,
                        NULL,NULL,NULL,NULL,NULL,NULL);//弹出“请先插入u盘”消息弹窗
                    break;
				}

				if(pUISystem->nFileCopyToStage != MESSAGESTAGE_IDLE)
				{
					ERR("Copy not complete\r\n");
					MessageBox("Copy Not Complete",DEV_MESSAGE_DELAY,
						NULL,NULL,NULL,NULL,NULL,NULL);
					break;
				}
				else
				{
				#if UPDATELOADMODULESSCRIPT
				UpdateLoadModuleSh();//zqh   add	
				#endif
                    ret = CheckUpdateFile(UPDATE_FILE_NAME);//检查U盘里是否有以pvr命名的文件
					if(ret>=0)
					{
						SetUpdateFileName(UPDATE_SRC_FILE_PATH,UPDATE_DST_FILE_PATH,UPDATE_FILE_NAME);
						pUISystem->nFileCopyToStage = MESSAGESTAGE_UPDATE_BACKUP;
                        //将根文件系统挂载为可读写的，默认文件系统为只读的
						ret = mount(RFS_MOUNT_NOD,RFS_MOUNT_DST,RFS_MOUNT_TYPE,MS_REMOUNT,0);
						if(ret<0)
						{
							ERR("remount RFS to rw err:%s\r\n",strerror(errno));
//							break;
						}
						update_reboot_code = EXIT_CODE_UPDATE;
                        // set update in progress
						SetUpdateInProgress(1);
                        StartCopyFile(Update_DestFile,Update_BackUpFile);//开始拷贝文件
					}
					else
					{
						ERR("%s:No update file!\n",__func__);
						MessageBox(NoUpdateFilePvr[pUISystem->allData.Language],DEV_MESSAGE_DELAY,
							NULL,NULL,NULL,
                            NULL,NULL,NULL);//弹出“无pvr文件”弹窗
					}
				}
				break;
            case MenuSystemUpdate_Button_ID_Kernel://更新内核
				ret = DoUDiskDetect();
				if((ret<0)||(!TESTBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_Mount_Flag)))
				{
					ERR("Need Mount UDisk first!\r\n");
					CLEARBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_Mount_Flag);
					MessageBox(HelpMountUDisk[pUISystem->allData.Language],DEV_MESSAGE_DELAY,
                        NULL,NULL,NULL,NULL,NULL,NULL);//弹出“请先插入u盘”消息弹窗
					break;
				}
				ret = CheckUpdateFile(UPDATE_KERNEL_NAME);
				if(ret<0)
				{
					ERR("%s:No update file!\n",__func__);
					MessageBox(NoUpdateFileuImage[pUISystem->allData.Language],DEV_MESSAGE_DELAY,
						NULL,NULL,NULL,
						NULL,NULL,NULL);
					break;
				}
				// 弹出升级内核确认信息
				MessageBox(DoKernelUpdate[pUISystem->allData.Language],0,DoUpdateKernel,
							NULL,UPDATE_KERNEL_NAME,
							UpdataSuc[pUISystem->allData.Language],
							UpdataFailed[pUISystem->allData.Language],
							UpdateKernelDeinit);
				break;
			default:
				retval=MsgNeedDispatch;
				break;
			}
			break;
		default:
			retval=MsgNeedDispatch;
			break;
	}
	return retval;

}

/**
 * @brief MenuSystemUpdate_Init
 * 系统更新初始化函数，设定界面的初始选中button
 * @param pMenu
 * 当前活动窗口
 * @return
 */
int MenuSystemUpdate_Init(UI_Menu_t *pMenu)
{
    // disable uboot update
	MenuButtonMaskBitmap[pMenu->nMenuIdx] = 1<<MenuSystemUpdate_Button_ID_uboot;
#if DEBUG_V2_1
//	MenuButtonMaskBitmap[pMenu->nMenuIdx] |= 1<<MenuSystemUpdate_Button_ID_Daemon;
#endif

	pMenu->nButtonIdx=MenuSetUsefulButton(pMenu->nMenuIdx);

	return 1;
}

/**
 * @brief MenuSystemUpdate_Refresh_Item
 * 绘制系统升级界面，比如：绘制button，将u-boot设置成灰色
 * @param pMenu
 * 当前活动窗口
 * @param nIdx
 * 当前窗口的button值，取值范围从0-5.
 * @param nFocus
 * 是否将当前button设置成高亮状态
 * @return
 */

int MenuSystemUpdate_Refresh_Item(struct UI_Menu_tag *pMenu,int nIdx,int nFocus)
{
	INT32U bmp_x0;
	INT32U bmp_y0;
	INT32U bmp_FirstList_x0;
	INT32U bmp_FirstList_y0;
	INT32U tmp_interval;
	GUI_BITMAP *pBmp_BG;
	GUI_BITMAP *pBmp_Sel;
	GUI_BITMAP *pBmp_UnSel;
    GUI_BITMAP *pBmp_Invalid;

    pBmp_BG = (&bmMainMenuBG_360_320);
	pBmp_UnSel = (&bmButton_Blue_785C4C_200_29_UP);
	pBmp_Sel = (&bmButton_Yellow_2060F0_200_29_UP);
	pBmp_Invalid = (&bmButton_Invalid_200_29);

	tmp_interval = (pBmp_BG->YSize - TITLE_ALL_HEIGHT - pBmp_Sel->YSize * (BUTTON_CNT_SYSTEMUPDATA)) / (BUTTON_CNT_SYSTEMUPDATA + 1);
	bmp_FirstList_x0 = AUTO_GET_X0_FROM_WIDTH(pBmp_Sel->XSize);
	bmp_FirstList_y0 = AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_ALL_HEIGHT + tmp_interval;
	
	bmp_x0 = bmp_FirstList_x0;
	bmp_y0 = bmp_FirstList_y0 + (pBmp_Sel->YSize + tmp_interval) * (nIdx);

	//画Button
	if((1<<nIdx)&MenuButtonMaskBitmap[pUISystem->nMenuIdx]){	//绘制需要跳过的条目
			GUI_DrawBitmap(pBmp_Invalid, bmp_x0, bmp_y0);
	}else{
		if(nFocus){	//绘制高亮选中条目
			GUI_DrawBitmap(pBmp_Sel, bmp_x0, bmp_y0);
		}else{			//绘制非高亮条目
			GUI_DrawBitmap(pBmp_UnSel, bmp_x0, bmp_y0);
		}
	}

	if((1<<nIdx)&MenuButtonMaskBitmap[pUISystem->nMenuIdx])
	{
		//绘制需要跳过的条目
        GUI_SetColor(GUI_GRAY);//将button颜色设置成灰色，比如：禁用u-boot button

		//写Content
		GUI_DispStringHCenterAt(SystemUpdateList[nIdx], 
								(bmp_x0 + pBmp_UnSel->XSize / 2),
								(bmp_y0 + BUTTON_V_OFFSET_DISPLAY));
		GUI_SetColor(GUI_WHITE);

	}
	else
	{
		//写Content
		GUI_DispStringHCenterAt(SystemUpdateList[nIdx], 
								(bmp_x0 + pBmp_UnSel->XSize / 2),
								(bmp_y0 + BUTTON_V_OFFSET_DISPLAY));
	}

    //NOTE: Do not SetRefreshArea should notify
	return 1;

}

/**
 * @brief MenuSystemUpdate_Refresh
 * 刷新系统升级界面函数，实际上是调用MenuSystemUpdate_Refresh_Item()这个函数实现的，
 * 此函数只绘制背景图片，还有标题。
 * @param pMenu
 * 当前活动窗口
 * @return
 */

int MenuSystemUpdate_Refresh(UI_Menu_t *pMenu)
{
	INT8U i = 0;
	INT32U bmp_FirstList_x0;
	INT32U bmp_FirstList_y0;
	INT32U tmp_interval;
	GUI_BITMAP *pBmp_BG;
	GUI_BITMAP *pBmp_Sel;
	GUI_BITMAP *pBmp_UnSel;
	int Index = 0;

	FUNC_ENTER;

    GUI_SetFont(&GUI_Font20_SongTi_Unicode);
    GUI_SetColor(GUI_WHITE);
    GUI_SetBkColor(GUI_BLACK);

	//变量保护
	Index = pMenu->nButtonIdx;
	Index = (Index > (BUTTON_CNT_SYSTEMUPDATA- 1)) ? 0 : Index;

	//绘制背景
	pBmp_BG = (&bmMainMenuBG_360_320);
	GUI_DrawBitmap(pBmp_BG, 
							AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize), 
							AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize));

	//填写Title
	GUI_SetTextMode(GUI_TM_TRANS);
	GUI_DispStringHCenterAt(SystemUpdate[pUISystem->allData.Language], 
							(LCD_WIDTH / 2), 
							(AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_BELOW_HEIGHT));

	//绘制Button	
	for(i = 0; i < BUTTON_CNT_SYSTEMUPDATA; i++)
	{
        MenuRefreshItemOnly(i);//调用此函数MenuSystemUpdate_Refresh_Item()具体实现button的绘制
	}

	//设定刷新区域
	SetRefreshAreaPos(OSDPOS_BASEMENU, 
					AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize), 
					AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize), 
					pBmp_BG->XSize, 
					pBmp_BG->YSize, 
					ALPHA_COMMON,
					TRUE);
}

/**
 * @brief NeedFormatCallback
 * 格式化cf卡后的回调函数，将界面切换至系统功能窗口
 * @param pUser
 * @return
 */

int NeedFormatCallback(void *pUser)
{
	SystemChangeWindow(MENUSYSTEMFUNC,1,1,0);
	return 1;
}

/**
 * @brief DoGetSystemLOG
 * 获得系统的log日志，将cf卡里的log文件拷贝到u盘里
 * @param pUser
 * @return
 */

int DoGetSystemLOG(void *pUser)
{
	int ret;
	char buf[MAX_FS_PATH];
	char full_path[MAX_FS_PATH];
	FILE *fp = NULL;

	FUNC_ENTER;

	Pvr_Waiting(TRUE);

	// get log
    sprintf(full_path,"%s%s",LOG_PATH_CFCARD,LOG_FILE_LOG);//合成log所在位置的全路径：比如/media/cf/pvr_log.log
	fp = fopen(full_path,"r");
    if(fp != NULL)//判断是否有pvr_log.log这个文件
	{
		fclose(fp);
		START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_MOVE_LOG);

        sprintf(buf,"cp %s %s",full_path,pUISystem->Vol_Disk[DISK_USB]);//合成这个字符串:cp /media/cf/pvr_log.log /media/usb
		LOGT("%s\r\n",buf);
        system(buf);//将pvr_log.log文件从cf卡里拷贝到优盘上

		STOP_WATCHDOG_TIMER_SIG();
	}

	// get old log
	sprintf(full_path,"%s%s",LOG_PATH_CFCARD,LOG_FILE_LOG_OLD);
	fp = fopen(full_path,"r");
	if(fp != NULL)
	{
		fclose(fp);
		START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_MOVE_LOG);

		sprintf(buf,"cp %s %s",full_path,pUISystem->Vol_Disk[DISK_USB]);
		LOGT("%s\r\n",buf);
		system(buf);

		STOP_WATCHDOG_TIMER_SIG();
	}

	// get start up log
	sprintf(full_path,"%s%s",LOG_PATH_CFCARD,LOG_FILE_STARTUP);
	fp = fopen(full_path,"r");
	if(fp != NULL)
	{
		fclose(fp);
		START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_MOVE_LOG);

		sprintf(buf,"cp %s %s",full_path,pUISystem->Vol_Disk[DISK_USB]);
		LOGT("%s\r\n",buf);
		system(buf);

		STOP_WATCHDOG_TIMER_SIG();
	}

	// get format msg
	sprintf(full_path,"%s%s",LOG_PATH_CFCARD,LOG_FILE_FMT);
	fp = fopen(full_path,"r");
	if(fp != NULL)
	{
		fclose(fp);
		START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_MOVE_LOG);

		sprintf(buf,"cp %s %s",full_path,pUISystem->Vol_Disk[DISK_USB]);
		LOGT("%s\r\n",buf);
		system(buf);

		STOP_WATCHDOG_TIMER_SIG();
	}

	// get fsck message
	sprintf(full_path,"%s%s",LOG_PATH_CFCARD,LOG_FILE_FSCK);
	fp = fopen(full_path,"r");
	if(fp != NULL)
	{
		fclose(fp);
		START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_MOVE_LOG);

		sprintf(buf,"cp %s %s",full_path,pUISystem->Vol_Disk[DISK_USB]);
		LOGT("%s\r\n",buf);
		system(buf);

		STOP_WATCHDOG_TIMER_SIG();
	}

	// get last kernel message
	sprintf(full_path,"%s%s",LOG_PATH_CFCARD,LOG_FILE_KERNEL_LAST);
	fp = fopen(full_path,"r");
	if(fp != NULL)
	{
		fclose(fp);
		START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_MOVE_LOG);

		sprintf(buf,"cp %s %s",full_path,pUISystem->Vol_Disk[DISK_USB]);
		LOGT("%s\r\n",buf);
		system(buf);

		STOP_WATCHDOG_TIMER_SIG();
	}

	// get kernel start msg
	sprintf(full_path,"%s%s",LOG_PATH_CFCARD,LOG_FILE_KERNEL_START);
	fp = fopen(full_path,"r");
	if(fp != NULL)
	{
		fclose(fp);
		START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_MOVE_LOG);

		sprintf(buf,"cp %s %s",full_path,pUISystem->Vol_Disk[DISK_USB]);
		LOGT("%s\r\n",buf);
		system(buf);

		STOP_WATCHDOG_TIMER_SIG();
	}

//	sprintf(full_path,"%s%s",pUISystem->Vol_Disk[DISK_USB],LOG_FILE_KERNEL_START);
//	get_kernel_msg(full_path);

	return 1;
}

/**
 * @brief FormatCFCard_Deinit
 * 格式完cf后的回调函数，主要工作是切换窗口值文件浏览窗口
 * @param nOKFail
 * @return
 */
int FormatCFCard_Deinit(int nOKFail)
{
    SystemChangeWindow(MENUFILEBROWSER,1,1,0);//窗口切换
	return 1;
}


/** zqh 2015年5月20日 add
 * @brief CFCardFsck_Deinit
 * cf文件系统检测完毕后调用的回调函数，主要工作是切换窗口到系统功能窗口
 * @param nOKFail
 * @return
 */
int CFCardFsck_Deinit(int nOKFail)
{
    SystemChangeWindow(MENUSYSTEMFUNC,1,1,0);//窗口切换
    return 1;
}


#include <sys/wait.h>
#include <signal.h>
#include <errno.h>


/**
 * drop caches
 * use system call to drop cachces
 * call this function before mkfs / fsck
 */
void DropCaches(void)
{
    system("sync");
    system("sync");
    system("echo 3 > /proc/sys/vm/drop_caches");
    usleep(500000);
}

/**
 * @brief DoFormatCFCard
 * 格式化cf卡,将日志文件保存到/tmp目录下
 * @param pUser
 * @return
 */

int DoFormatCFCard(void *pUser)
{
	int ret,system_result;
	char tmp_path[DEV_PATH_LEN];
	char buf[MAX_FS_PATH];

	FUNC_ENTER;
	Pvr_Waiting(TRUE);

    ret = SystemGetMountDevPath(tmp_path,DISK_CF);//检测CF卡是否挂载
	if(ret<0)
		return -1;

	if(!TESTBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_REMOVE))
	{
        // close cf card log file , redirect log to tmp file
        // TODO: need test log in tmpfs
		close_log();

        //以下move_log函数是将CF卡内的log文件拷贝到/tmp目录下
		move_log(LOG_PATH_CFCARD,LOG_PATH_TMP,LOG_FILE_LOG);
		move_log(LOG_PATH_CFCARD,LOG_PATH_TMP,LOG_FILE_LOG_OLD);
		move_log(LOG_PATH_CFCARD,LOG_PATH_TMP,LOG_FILE_STARTUP);
		move_log(LOG_PATH_CFCARD,LOG_PATH_TMP,LOG_FILE_FSCK);

		move_log(LOG_PATH_CFCARD,LOG_PATH_TMP,LOG_FILE_KERNEL_START);
		move_log(LOG_PATH_CFCARD,LOG_PATH_TMP,LOG_FILE_KERNEL_LAST);

		redirect_log(LOG_PATH_TMP,LOG_FILE_FMT);

        SETBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_FORMAT);
        ret = SystemUmountDev(pUISystem->Vol_Disk[DISK_CF]);//卸载CF卡
		if(ret<0)
			return -1;
	}

	//  Add Format CFCard
	//  Add watchdog
	START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_FORMAT_CF);

#if (USE_MOUNT_CF_THREAD)||(USE_MOUNT_CF_THREAD_S)
    DropCaches();
#endif

#if MOUNT_CF_EXT //DEBUG_V2_1
    sprintf(buf,"/sbin/mkfs.ext3 %s",tmp_path);//合成/sbin/mkfs.ext3 /dev/sda1这个字符串，假设cf卡在dev下生成sda1这个设备
#else
	sprintf(buf,"/usr/sbin/mkfs.vfat %s",tmp_path);
#endif
    LOGT("system %s\r\n",buf);
    ret = system(buf);//执行格式化cf卡命令
    system_result = 0;
    if(ret != -1)
    {
        if(WIFEXITED(ret))
        {
            if (0 == WEXITSTATUS(ret))  
            {  
                LOGT("run shell script successfully.\n");
                system_result = 1;
            }
        }
    }
//	system(buf);
    STOP_WATCHDOG_TIMER_SIG();

    // NOTE: mkfs again may not resolve this error
    // deprecated   
    if(system_result == 0)
    {
        LOGE("system mkfs error ret=%d,exit code: %d\r\n",ret,WEXITSTATUS(ret));
        /*  
        LOGT("start system mkfs again\r\n");
        START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_FORMAT_CF);
        sleep(5); // delay 5s to free memory
        ret = system(buf);
        LOGT("system mkfs: ret2=%d,exit code: %d\r\n",ret,WEXITSTATUS(ret));
        if(ret != -1)
        {
            if(WIFEXITED(ret))
            {
                if (0 == WEXITSTATUS(ret))
                {
                    LOGT("run shell script successfully.\n");
                    system_result = 1;
                }
            }
        }
        STOP_WATCHDOG_TIMER_SIG();
        */
        if(system_result == 0)
        {
            return -1;
        }
    }

	LOGT("kernel message:\r\n");

	sprintf(buf,"%s%s",LOG_PATH_TMP,LOG_FILE_FMT);
    get_kernel_msg(buf);//获得内核信息保存在/tmp/pvr_fmt.log

    CLEARBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_NEEDFORMAT);
    CLEARBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_FORMAT);
#if !(USE_MOUNT_CF_THREAD)
	ret = SystemMountCFCard();
	if(ret<0)
	{
		return -1;
	}
#endif

	//  move tmp log file to cfcard
//	move_log(LOG_PATH_TMP,LOG_PATH_CFCARD,LOG_FILE_FMT);
	
	return 1;
}

/**
 * @brief DoCFCardFsck
 * CF卡文件系统检测
 * @param pUser
 * @return
 */
int DoCFCardFsck(void *pUser)
{
	int ret,system_result;
	char tmp_path[DEV_PATH_LEN];
	char buf[MAX_FS_PATH];

	FUNC_ENTER;
	Pvr_Waiting(TRUE);

    ret = SystemGetMountDevPath(tmp_path,DISK_CF);//检测cf卡是否插入
	if(ret<0)
		return -1;

	if(!TESTBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_REMOVE))
	{
        // close cf card log file,redirect log to tmp file
        // TODO: need test log in tmpfs
		close_log();
		redirect_log(LOG_PATH_TMP,LOG_FILE_FSCK);

        SETBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_FSCK);
		ret = SystemUmountDev(pUISystem->Vol_Disk[DISK_CF]);
		if(ret<0)
			return -1;
	}

	//  Add CFCard Fsck
	//  Add watchdog
	START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_FSCK_CF);
#if (USE_MOUNT_CF_THREAD)||(USE_MOUNT_CF_THREAD_S)
    DropCaches();
#endif

#if MOUNT_CF_EXT
    sprintf(buf,"fsck.ext3 %s -yf",tmp_path);//合成字符串fsck.ext3 /dev/sda1 -yf，假设cf卡在dev下生成sda1这个设备
#else
	sprintf(buf,"fsck.vfat %s -yf",tmp_path);
#endif
	LOGT("%s\r\n",buf);
    ret = system(buf);//进行cf卡文件系统的检测
    system_result = 0;
    if(ret != -1)
    {
        if(WIFEXITED(ret))
        {
            if (0 == WEXITSTATUS(ret))  
            {  
                LOGT("system call successfully\r\n");
                system_result = 1;
            }
        }
    }

    STOP_WATCHDOG_TIMER_SIG();

    // NOTE: fsck again may not resolve this error
    // deprecated
    if(system_result == 0)
    {
        LOGE("system fsck error ret=%d,exit code: %d\r\n",ret,WEXITSTATUS(ret));
        /*
        LOGT("start system fsck again\r\n");

        START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_FSCK_CF);
        sleep(5); // delay 5s to free memory
        ret = system(buf);
        LOGT("system fsck: ret2=%d,exit code: %d\r\n",ret,WEXITSTATUS(ret));
        if(ret != -1)
        {
            if(WIFEXITED(ret))
            {
                if (0 == WEXITSTATUS(ret))
                {
                    LOGT("system call successfully\r\n");
                    system_result = 1;
                }
            }
        }
        STOP_WATCHDOG_TIMER_SIG();
        */
        if(system_result == 0)
        {
            return -1;
        }
    }

	LOGT("kernel message:\r\n");

	sprintf(buf,"%s%s",LOG_PATH_TMP,LOG_FILE_FSCK);
    get_kernel_msg(buf);//将内核信息保存到/tmp/pvr_fsck.log

    CLEARBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_FSCK);

#if !(USE_MOUNT_CF_THREAD)
	ret = SystemMountCFCard();
	if(ret<0)
	{
		return -1;
	}
#endif

	//  move tmp log file to cfcard
//	move_log(LOG_PATH_TMP,LOG_PATH_CFCARD,LOG_FILE_FSCK);

	return 1;
}

#if MOUNT_CF_EXT// DEBUG_V2_1
static const char *UpdateToolsFile[]=
{
    "sbin/fsck.*",
    "sbin/mkfs.*",
    "usr/lib/lib*",
    "usr/sbin/dosfsck",
    "usr/sbin/fsck.*",
    "usr/sbin/mkdosfs",
    "usr/sbin/mkfs.*"
};
static const char *UpdateToolsPath[]=
{
    "/sbin/",
    "/sbin/",
    "/usr/lib/",
    "/usr/sbin/",
    "/usr/sbin/",
    "/usr/sbin/",
    "/usr/sbin/"
};
#else
static const char *UpdateToolsFile[]=
{
    "dosfsck",
    "mkdosfs"
};
static const char *UpdateToolsPath[]=
{
    "/usr/sbin/",
    "/usr/sbin/"
};
#endif

int do_update_system_tools = 0;
/**
 * @brief DoUpdateSystemTools
 * 更新系统工具，从U盘里拷贝到系统上
 * @param pUser
 * @return
 */
int DoUpdateSystemTools(void *pUser)
{
	char tmp_str[0x100];
	int i;
	int ret;
    int count;

	FUNC_ENTER;

	Pvr_Waiting(TRUE);

    count = sizeof(UpdateToolsFile)/sizeof(UpdateToolsFile[0]);

	START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_FORMAT_CF);

	LOGT("update system tools\r\n");

	LOGT("mount file system to rw\r\n");
	ret = mount(RFS_MOUNT_NOD,RFS_MOUNT_DST,RFS_MOUNT_TYPE,MS_REMOUNT,0);
	if(ret<0)
	{
		ERR("remount RFS to rw err:%s\r\n",strerror(errno));
	}
	LOGT("mount complete\r\n");

    // 只更新U盘里有的升级文件
	do_update_system_tools = 1;
	for(i=0;i<count;i++)
	{
#if !(MOUNT_CF_EXT) // !(DEBUG_V2_1)
        if(CheckUpdateFile(UpdateToolsFile[i])<0)//检测要更新的文件在U盘里是否存在
		{
			LOGT("no update file: %s\r\npass this file\r\n",UpdateToolsFile[i]);
			continue;
		}
#endif
		sprintf(tmp_str,"cp %s%s %s",UPDATE_SRC_FILE_PATH,UpdateToolsFile[i],UpdateToolsPath[i]);
		LOGT("%s\r\n",tmp_str);

		system(tmp_str);

		sync();
		sync();

		sprintf(tmp_str,"chmod +x %s*",UpdateToolsPath[i]);
		LOGT("%s\r\n",tmp_str);
		system(tmp_str);

	}

	STOP_WATCHDOG_TIMER_SIG();
	FUNC_EXIT;

	return 1;
}

/**
 * @brief UpdateSystemToolsDeinit
 * 系统更新后，反初始化会调用此函数，即进行界面窗口的切换
 * @param pUser
 * @return
 */

int UpdateSystemToolsDeinit(void *pUser)
{
	FUNC_ENTER;
    LOGT("MenuRestore\r\n");
	SystemMenuRestore();

	return 1;
}

/**
 * @brief DoDisableSystemLog
 * 禁用系统日志的功能
 * @param pUser
 * @return
 */
int DoDisableSystemLog(void *pUser)
{
    START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_DELETE_FILE);

    set_log_en(0);//将opt下的.pvr_log_en文件删除

    STOP_WATCHDOG_TIMER_SIG();

    update_reboot_code = EXIT_CODE_REBOOT;
	Update_Reboot(1);
    return 1;
}

/**
 * @brief DoEnableSystemLog
 * 使能系统日志功能
 * @param pUser
 * @return
 */
int DoEnableSystemLog(void *pUser)
{
    START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_DELETE_FILE);

    set_log_en(1);//在opt目录下新建这个.pvr_log_en文件

    STOP_WATCHDOG_TIMER_SIG();

    update_reboot_code = EXIT_CODE_REBOOT;
    Update_Reboot(1);
    return 1;
}

/**
 * @brief MenuSystemFunc_Init
 * 系统功能窗口的初始化
 * 设定初始化选中的button
 * @param pMenu
 * @return
 */
int MenuSystemFunc_Init(UI_Menu_t *pMenu)
{
	GUI_SetBkColor(GUI_BLACK);
	pMenu->nButtonIdx=0;
	return 1;
}

/**
 * @brief MenuSystemFunc_MsgHandle
 * 系统功能按键消息处理函数。
 * @param pMenu
 * @param nMsgType
 * @param nMsgData
 * @return
 */

TMsgHandleStat MenuSystemFunc_MsgHandle(UI_Menu_t *pMenu,unsigned short nMsgType,unsigned short nMsgData)
{
 	TMsgHandleStat retval=MsgHandled;

	if (nMsgType!=MSG_KEY_EVENT){
		return MsgNocare;
	}

	switch(nMsgData){
        case KEY_UP://上移
			MenuChangeFocus(-1);
			break;
        case KEY_DOWN://下移
			MenuChangeFocus(1);
			break;
        case KEY_ENTER://确认键
			switch(pMenu->nButtonIdx)
			{
                case MenuSystemFunc_Button_ID_CopyToCF://文件拷贝到cf卡，切换到浏览u盘的界面
                    if(CheckMountFlags() < 0)
                    {
                    	retval=MsgNeedDispatch;
                        break;
                    }

					// change to udisk file list
					if((DoUDiskDetect()<0)||(!TESTBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_Mount_Flag)))
					{
						ERR("Need Mount UDisk first!\n");
						CLEARBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_Mount_Flag);
						MessageBox(HelpMountUDisk[pUISystem->allData.Language],DEV_MESSAGE_DELAY,
                            NULL,NULL,NULL,NULL,NULL,NULL);//弹出“u盘未挂载”的消息弹窗
						break;
					}

					SystemChangeWindow(MENUUDISKFILELIST,0,1,1);
                    break;
                    // zqh 2015年3月24日 modify，将“获取系统日志”功能屏蔽掉
                    #if 0
                case MenuSystemFunc_Button_ID_GetLOG://获取系统日志
                    if(CheckMountFlags() < 0)
                    {
                    	retval=MsgNeedDispatch;
                        break;
                    }
                    // get system log file
					if((DoUDiskDetect()<0)||(!TESTBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_Mount_Flag)))
					{
						ERR("Need Mount UDisk first!\n");
						CLEARBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_Mount_Flag);
						MessageBox(HelpMountUDisk[pUISystem->allData.Language],DEV_MESSAGE_DELAY,
							NULL,NULL,NULL,NULL,NULL,NULL);
						break;
					}

					MessageBox(GetSystemLog[pUISystem->allData.Language],CODEC_MESSAGE_DELAY,DoGetSystemLOG,NULL,NULL,
								MsgBox35[pUISystem->allData.Language],MsgBox36[pUISystem->allData.Language],
								NULL);
					break;
                    #endif
                case MenuSystemFunc_Button_ID_FormatCF://格式化CF卡
                    if(CheckMountFlags() < 0)
                    {
                    	retval=MsgNeedDispatch;
                        break;
                    }
					//  Format CFCard msg box
					MessageBox(MsgBox9[pUISystem->allData.Language],CODEC_MESSAGE_DELAY,DoFormatCFCard,NULL,NULL,
								MsgBox15[pUISystem->allData.Language],MsgBox20[pUISystem->allData.Language],
								FormatCFCard_Deinit);
					break;
                case MenuSystemFunc_Button_ID_FSCK://CF卡文件系统检测
                    if(CheckMountFlags() < 0)
                    {
                    	retval=MsgNeedDispatch;
                        break;
                    }
                    //  Add fsck msg box zqh 2015年4月20日 modify，将显示“检查电子盘文件系统”改为“检查电子盘文件系统，是否继续？”
                    //zqh 2015年5月20日 modify，当cf卡文件系统检测完毕后，调用的函数由FormatCFCard_Deinit()改为CFCardFsck_Deinit()这个函数
                    MessageBox(MsgBox47[pUISystem->allData.Language],CODEC_MESSAGE_DELAY,DoCFCardFsck,NULL,NULL,
								CFCardFsckOK[pUISystem->allData.Language],CFCardFsckFail[pUISystem->allData.Language],
                                CFCardFsck_Deinit);
					break;
                    //zqh 2015年3月24日 modify，将“使能/开启日志功能”屏蔽掉
                    #if 0
#if DEBUG_V2_1
                case MenuSystemFunc_Button_ID_LogEnable://使能关闭log功能
                    if(is_log_en())
                    {
                        MessageBox(DisableSystemLog[pUISystem->allData.Language],CODEC_MESSAGE_DELAY,DoDisableSystemLog,
                            NULL,NULL,NULL,NULL,NULL);
                    }
                    else
                    {
                        MessageBox(EnableSystemLog[pUISystem->allData.Language],CODEC_MESSAGE_DELAY,DoEnableSystemLog,
                            NULL,NULL,NULL,NULL,NULL);
                    }
                    break;
#else
				case MenuSystemFunc_Button_ID_Updatetools:
					// TODO: Add update system tools
					if((DoUDiskDetect()<0)||(!TESTBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_Mount_Flag)))
					{
						ERR("Need Mount UDisk first!\n");
						CLEARBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_Mount_Flag);
						MessageBox(HelpMountUDisk[pUISystem->allData.Language],DEV_MESSAGE_DELAY,
							NULL,NULL,NULL,NULL,NULL,NULL);
						break;
					}

					MessageBox(UpdateSystemTools[pUISystem->allData.Language],CODEC_MESSAGE_DELAY,DoUpdateSystemTools,NULL,NULL,
								UpdateSystemToolsOK[pUISystem->allData.Language],UpdateSystemToolsFail[pUISystem->allData.Language],
								UpdateSystemToolsDeinit);
					break;
#endif
                    #endif
                case MenuSystemFunc_Button_ID_Language:
                    SystemChangeWindow(MENULANGUAGESEL,0,1,1);
                    break;
				default:
					ERR("%s invalid button id: %d\r\n",__func__,pMenu->nButtonIdx);
					break;
			}
			break;
        case KEY_ESC://退出按键消息，窗口切换至文件浏览窗口
            SystemChangeWindow(MENUFILEBROWSER,1,1,0);
            CLEARBIT(pUISystem->SysState.CurState, SYS_BIT_SYSTEMFUNCTION);//设置系统更新状态位
            SETBIT(pUISystem->SysState.CurState, SYS_BIT_FILEBROSWER);
			break;
		default:
			retval=MsgNeedDispatch;
			break;
	}

	return retval;
}

/**
 * @brief MenuSystemFunc_RefreshItem
 * 系统功能界面的绘制函数，比如绘制button，在button上显示文字
 * @param pMenu
 * 当前活动窗口
 * @param nIdx
 * 输入参数：绘制第N个button，取值范围从0~4.
 * @param nFocus
 * 输入参数：button是否高亮
 * @return
 */
int MenuSystemFunc_RefreshItem(struct UI_Menu_tag *pMenu,int nIdx,int nFocus)
{
	INT32U bmp_x0;
	INT32U bmp_y0;
	INT32U bmp_FirstList_x0;
	INT32U bmp_FirstList_y0;
	INT32U tmp_interval;
	GUI_BITMAP *pBmp_BG;
	GUI_BITMAP *pBmp_Sel;
	GUI_BITMAP *pBmp_UnSel;
	GUI_BITMAP *pBmp_Invalid;

    //zqh 2015年5月25日add，调整当系统语言为英文时，将字体变小。
    //printf("pUISystem->allData.Language = %d\n", pUISystem->allData.Language);
    if(pUISystem->allData.Language == ENGLISH)
        GUI_SetFont(&GUI_Font8x16);
    else
        GUI_SetFont(&GUI_Font20_SongTi_Unicode);

    //	GUI_BITMAP *pBmp_SubBG = (&bmMessageRect_310_130);
	//绘制Button
	pBmp_BG = (&bmMainMenuBG_360_320);
	pBmp_UnSel = (&bmFileList_Flat_GBlue_240_30);
	pBmp_Sel = (&bmFileList_Flat_Orange_240_30);

	tmp_interval = (pBmp_BG->YSize - TITLE_ALL_HEIGHT- pBmp_Sel->YSize * (BUTTON_CNT_SYSTEMFUNC+1)) / (BUTTON_CNT_SYSTEMFUNC+2);
	bmp_FirstList_x0 = AUTO_GET_X0_FROM_WIDTH(pBmp_Sel->XSize);
	bmp_FirstList_y0 = AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize)+TITLE_ALL_HEIGHT + tmp_interval;
	
	bmp_x0 = bmp_FirstList_x0;
	bmp_y0 = bmp_FirstList_y0 + (pBmp_Sel->YSize + tmp_interval) * (nIdx);

	//画Button
	if(nFocus){	//绘制高亮选中条目
		GUI_DrawBitmap(pBmp_Sel, bmp_x0, bmp_y0);
	}else{			//绘制非高亮条目
		GUI_DrawBitmap(pBmp_UnSel, bmp_x0, bmp_y0);
	}

	if((1<<nIdx)&MenuButtonMaskBitmap[pUISystem->nMenuIdx])
	{	//绘制需要跳过的条目
		GUI_SetColor(GUI_GRAY);

		//写Content
		GUI_DispStringHCenterAt(SystemFuncList[nIdx], 
								(bmp_x0 + pBmp_UnSel->XSize / 2),
								(bmp_y0 + BUTTON_V_OFFSET_DISPLAY));
		GUI_SetColor(GUI_WHITE);

	}
	else
	{
		//写Content
        if(pUISystem->allData.Language == ENGLISH){
            if(nIdx%2 == 0)
                GUI_DispStringHCenterAt(SystemFuncList[nIdx],
                                    (bmp_x0 + pBmp_UnSel->XSize / 2),bmp_y0);
            else
                GUI_DispStringHCenterAt(SystemFuncList[nIdx],
                                    (bmp_x0 + pBmp_UnSel->XSize / 2),
                                    (bmp_y0 + 8));
        }
        else
        {
            GUI_DispStringHCenterAt(SystemFuncList[nIdx],
                                    (bmp_x0 + pBmp_UnSel->XSize / 2),
                                    (bmp_y0 + BUTTON_V_OFFSET_DISPLAY));
        }
    }

	return 1;
}

/**
 * @brief MenuSystemFunc_Refresh
 * 系统功能界面的刷新函数，比如设置背景、标题，调用绘制button函数。
 * @param pMenu
 * @return
 */
int MenuSystemFunc_Refresh(UI_Menu_t *pMenu)
{
	GUI_BITMAP *pBmp_BG = (&bmMainMenuBG_360_320);
//	GUI_BITMAP *pBmp_SubBG = (&bmMessageRect_310_130);
	GUI_RECT Rect;
	const char *MessageDisp=NULL;
	const char *MessageHelp=NULL;
	int i;

	GUI_SetFont(&GUI_Font20_SongTi_Unicode);
	GUI_SetColor(GUI_WHITE);
	GUI_SetBkColor(GUI_BLACK);
	//--------------------------------------------------------------------------//
	//	Purpose	:	绘制背景
	//--------------------------------------------------------------------------//
	GUI_DrawBitmap(pBmp_BG, 
				AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize), 
				AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize));

	//--------------------------------------------------------------------------//
	//	Purpose	:	填写Title
	//--------------------------------------------------------------------------//
    //zqh 2015年3月30日 modify，将title信息的“提示信息”改为“系统功能”
    GUI_SetTextMode(GUI_TM_TRANS);
    GUI_DispStringHCenterAt(SystemFunction[pUISystem->allData.Language],
							(LCD_WIDTH / 2), 
							(AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_BELOW_HEIGHT));

//	GUI_DrawBitmap(pBmp_SubBG, 
//							AUTO_GET_X0_FROM_WIDTH(pBmp_SubBG->XSize), 
//							(AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_ALL_HEIGHT + BUTTON_V_INTERVAL_COMMON));


    for(i = 0; i < BUTTON_CNT_SYSTEMFUNC; i++)
	{
		MenuRefreshItemOnly(i);
	}

	//--------------------------------------------------------------------------//
	//	Purpose	:	填写显示内容
	//	Purpose	:	绘制下方提示信息
	//--------------------------------------------------------------------------//
	//zqh 2015年3月27日modify，将MessageHelp的值赋值为" "
	//MessageHelp=HelpTip1[pUISystem->allData.Language];

    if(pUISystem->allData.Language == ENGLISH)
        GUI_SetFont(&GUI_Font20_SongTi_Unicode);

	MessageHelp = " ";

	GUI_DispStringHCenterAt(MessageHelp, 
					AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize) + pBmp_BG->XSize/2, 
					(AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + pBmp_BG->YSize - GUI_Font20_SongTi_Unicode.YSize - PROCESS_V_OFFSET_BG_BOTTON));
	//temp--设定刷新区域
	SetRefreshAreaPos(OSDPOS_BASEMENU, 
					AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize), 
					AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize), 
					pBmp_BG->XSize, 
					pBmp_BG->YSize, 
					ALPHA_COMMON,
					TRUE);
	return 1;
}

// 增加从U盘拷贝到CF卡的操作
int DoCopyToCFCard(void *pUser)
{
	int ret;
	char *filename;
	char src_path[MAX_FS_PATH];
	char dest_path[MAX_FS_PATH];

    filename=GetCurrentFileName(pUISystem->FileBrowserIndex_UDisk,pUISystem->FileBrowserSelFile_UDisk,
									&pUISystem->FileBrowserdirlist_UDisk);
	if(filename == NULL)
	{
		LOGE("%s: invalid file\r\n",__func__);
		return -1;
	}
	snprintf(src_path,MAX_FS_PATH,"%s%s",pUISystem->Vol_Disk[DISK_USB],filename);
	snprintf(dest_path,MAX_FS_PATH,"%s%s",pUISystem->Vol_Disk[DISK_CF],filename);

	pUISystem->nFileCopyToStage = MESSAGESTAGE_COPY_TO_CFCARD;

    StartCopyFile(src_path,dest_path);//拷贝文件

	return 1;
}

/**
 * @brief MenuUDiskFileList_PageChange
 * 浏览U盘上文件，翻页操作的处理函数
 * @param pMenu
 * 当前活动窗口
 * @param adjustval
 * 向前/后翻页、
 * @return
 */
int MenuUDiskFileList_PageChange(UI_Menu_t *pMenu, int adjustval)
{
    if(pUISystem->FileBrowserdirlist_UDisk.nCount <= BUTTON_CNT_UDISKFILELIST)//U盘文件浏览窗口，单页显示4个文件
		return 0;

	if(adjustval>0)	{	//后翻页
		if((pUISystem->FileBrowserIndex_UDisk + BUTTON_CNT_UDISKFILELIST) < pUISystem->FileBrowserdirlist_UDisk.nCount) {
			pUISystem->FileBrowserIndex_UDisk += BUTTON_CNT_UDISKFILELIST;
		} else {
			pUISystem->FileBrowserIndex_UDisk = 0;
		}
	} else if(adjustval<0)	{
		if((pUISystem->FileBrowserIndex_UDisk  - BUTTON_CNT_UDISKFILELIST) >= 0) {
			pUISystem->FileBrowserIndex_UDisk -= BUTTON_CNT_UDISKFILELIST;
		} else {
			pUISystem->FileBrowserIndex_UDisk = ((pUISystem->FileBrowserdirlist_UDisk.nCount-1) / BUTTON_CNT_UDISKFILELIST) * BUTTON_CNT_UDISKFILELIST;
		}
	}
	pMenu->nButtonIdx = 0;
	return 1;
}

/**
 * @brief MenuUDiskFileList_SingleChange
 * U盘文件浏览界面，选中文件的上下切换
 * @param pMenu
 * 当前活动窗口
 * @param adjustval
 * 光标上/下移
 * @return
 */

int MenuUDiskFileList_SingleChange(UI_Menu_t *pMenu, int adjustval)
{
	int index;		//总的Index(0---TotalNum-1)

	if(pUISystem->FileBrowserdirlist_UDisk.nCount <= 1) {
		return 0;
	}

	index = pUISystem->FileBrowserIndex_UDisk + pMenu->nButtonIdx;
	
	if(adjustval>0)	{	//下一个
		if((index + 1) < pUISystem->FileBrowserdirlist_UDisk.nCount) {
			index++;
		} else {
			index = 0;
		}
	} else if(adjustval<0)	{			//上一个
		if(index <= 0) {
			index = pUISystem->FileBrowserdirlist_UDisk.nCount - 1;
		} else {
			index --;
		}
	}
	pUISystem->FileBrowserIndex_UDisk = (index / BUTTON_CNT_UDISKFILELIST) * BUTTON_CNT_UDISKFILELIST;
	pMenu->nButtonIdx = index - pUISystem->FileBrowserIndex_UDisk;
	return 1;
}

/**
 * @brief MenuUDiskFileList_ChangeFocus
 * 浏览U盘文件，处理“上\下\左\右”四种按键的函数
 * @param pMenu
 * 当前活动窗口
 * @param nMsgData
 * 具体的按键消息
 * @return
 */
int MenuUDiskFileList_ChangeFocus(UI_Menu_t *pMenu, unsigned short nMsgData)
{
	int ret=0;
	switch(nMsgData)
	{
	case KEY_LEFT:
		ret=MenuUDiskFileList_PageChange(pMenu, -1);
		break;
	case KEY_RIGHT:
		ret=MenuUDiskFileList_PageChange(pMenu, 1);
		break;
	case KEY_UP:
		ret=MenuUDiskFileList_SingleChange(pMenu, -1);
		break;
	case KEY_DOWN:
		ret=MenuUDiskFileList_SingleChange(pMenu, 1);
		break;
	default:
		ERR("%s:Invalid KeyMsg\r\n",__func__);//should not happen
		break;		
	}

	if(ret)
	{
		pUISystem->FileBrowserSelFile_UDisk=pMenu->nButtonIdx;
//		MenuUDiskFileList_RefreshPage(pMenu);
		MenuRefresh();
	}
	return ret;
}

/**
 * @brief MenuUDiskFileList_Init
 * 切换到U盘文件浏览窗口的初始化函数，比如：获得优盘里.avi文件的个数、设置初始化选中的button
 * @param pMenu
 * 当前活动窗口
 * @return
 */

int MenuUDiskFileList_Init(UI_Menu_t *pMenu)
{
	int i;

	// Set FileBrowserdirlist_UDisk directory and contain file type
	strcpy(pUISystem->FileBrowserdirlist_UDisk.directory,pUISystem->Vol_Disk[DISK_USB]);
	pUISystem->FileBrowserdirlist_UDisk.contain = CONTAIN_FILE;
	pUISystem->FileBrowserdirlist_UDisk.nCount = 0;

	//Start Scan
	if (!Scandir(&(pUISystem->FileBrowserdirlist_UDisk),VIDEO_FILES, pUISystem->allData.CurSortMethod)) 
	{
		LOGE("%s: Scandir failed\n",__func__);
		ReleaseList(&(pUISystem->FileBrowserdirlist_UDisk));
		return -1;
	}

	MenuButtonMaskBitmap[pMenu->nMenuIdx]=0;

	if(pUISystem->FileBrowserdirlist_UDisk.nCount==0)
	{
		for (i=0;i<BUTTON_CNT_UDISKFILELIST;i++)
		{
			MenuButtonMaskBitmap[pMenu->nMenuIdx]|=(1<<i);
		}
		pMenu->nButtonIdx=-1;	//set to invalid button

		return 1;
	}

    // 总是设置为默认位置
	{
    pUISystem->FileBrowserIndex_UDisk = 0;
	}
	{
    pUISystem->FileBrowserSelFile_UDisk = 0;
	}

	pMenu->nButtonIdx=0;

	return 1;
}

/**
 * @brief MenuUDiskFileList_Close
 * 关闭浏览U盘文件的操作函数，释放FileBrowserdirlist_UDisk内存
 * @param pMenu
 * 当前活动窗口
 * @return
 */
int MenuUDiskFileList_Close(UI_Menu_t *pMenu)
{
    ReleaseList(&(pUISystem->FileBrowserdirlist_UDisk));//释放内存
	return 1;
}

/**
 * @brief MenuUDiskFileList_MsgHandle
 * 浏览U盘文件时，相应按键消息的处理函数。
 * @param pMenu
 * @param nMsgType
 * @param nMsgData
 * @return
 */

TMsgHandleStat MenuUDiskFileList_MsgHandle(UI_Menu_t *pMenu,unsigned short nMsgType,unsigned short nMsgData)
{
 	TMsgHandleStat retval=MsgHandled;

	if (nMsgType!=MSG_KEY_EVENT){
		return MsgNocare;
	}

	switch(nMsgData){
		case KEY_LEFT:
		case KEY_RIGHT:
		case KEY_UP:
        case KEY_DOWN://上、下、左、右四种按键
			MenuUDiskFileList_ChangeFocus(pMenu,nMsgData);
			break;
        case KEY_ENTER://确认键
            if(CheckMountFlags() < 0)
            {
            	retval=MsgNeedDispatch;
                break;
            }
			// copy file to cfcard
			MessageBox(CopyToCFCard[pUISystem->allData.Language],CODEC_MESSAGE_DELAY,DoCopyToCFCard,NULL,
						NULL,NULL,NULL,NULL);
			break;
        case KEY_ESC://退出键
            LOGT("MenuRestore\r\n");
			SystemMenuRestore();
			break;
		default:
			retval=MsgNeedDispatch;
			break;
	}

    return retval;
}

/**
 * @brief MenuUDiskFileList_RefreshItem
 * 绘制U盘文件浏览界面的button及文字的函数
 * @param pMenu
 * 当前活动窗口
 * @param nIdx
 *
 * @param nFocus
 * @return
 */
int MenuUDiskFileList_RefreshItem(struct UI_Menu_tag *pMenu,int nIdx,int nFocus)
{
	INT32U bmp_x0;
	INT32U bmp_y0;
	INT32U tmp_interval;
	INT32U bmp_FirstList_x0;
	INT32U bmp_FirstList_y0;
	INT32U CurPage;
	INT32U TotalPage;
	char DispBuf[DISPBUF_LEN];

	GUI_BITMAP *pBmp_BG = NULL;
	GUI_BITMAP *pBmp_Sel = NULL;
	GUI_BITMAP *pBmp_UnSel = NULL;

	struct FileListBlock *dirlist = &(pUISystem->FileBrowserdirlist_UDisk);

	pBmp_BG = (&bmMainMenuBG_360_320);
	pBmp_UnSel = (&bmFileList_Flat_GBlue_240_30);
	pBmp_Sel = (&bmFileList_Flat_Orange_240_30);

	tmp_interval=(pBmp_BG->YSize-TITLE_ALL_HEIGHT-pBmp_Sel->YSize*(BUTTON_CNT_UDISKFILELIST+1))/(BUTTON_CNT_UDISKFILELIST+1+1);
	bmp_FirstList_x0 = AUTO_GET_X0_FROM_WIDTH(pBmp_Sel->XSize);
	bmp_FirstList_y0 = AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_ALL_HEIGHT + tmp_interval;

	bmp_x0 = bmp_FirstList_x0;
	bmp_y0 = bmp_FirstList_y0 + (pBmp_Sel->YSize + tmp_interval) * (nIdx);

	if(nIdx == 0)
	{
		//--填写Page，如1/23
		if(dirlist->nCount<=0)
		{
			GUI_DispStringHCenterAt("0/0",(bmp_x0),(bmp_y0+(GUI_Font20_SongTi_Unicode.YSize+tmp_interval)*5));
			return 1;
		}
		else
		{
			CurPage=pUISystem->FileBrowserIndex_UDisk/BUTTON_CNT_UDISKFILELIST+1;
			TotalPage=dirlist->nCount/BUTTON_CNT_UDISKFILELIST+(((dirlist->nCount)%BUTTON_CNT_UDISKFILELIST>0)?1:0);
			snprintf(DispBuf,DISPBUF_LEN,"%d/%d",CurPage,TotalPage);
			GUI_DispStringAt((const char*)DispBuf,(bmp_x0+OFFSET_H_FILL+BUTTON_H_INTERVAL_COMMON*2),(bmp_y0+(GUI_Font20_SongTi_Unicode.YSize+tmp_interval)*5));
		}
	}

	//画Button
	//File list
	if ((nIdx+pUISystem->FileBrowserIndex_UDisk)>=dirlist->nCount)
	{
		return 1;
	}

	if(nFocus)
	{
		GUI_DrawBitmap(pBmp_Sel, bmp_x0, bmp_y0);
	}else{
		GUI_DrawBitmap(pBmp_UnSel, bmp_x0, bmp_y0);
	}
	//填写文件信息
	//--填写Title
	GUI_DispStringHCenterAt(dirlist->attribute[nIdx+pUISystem->FileBrowserIndex_UDisk].longname, 
		(bmp_x0 + pBmp_Sel->XSize/2),
		(bmp_y0 + BUTTON_V_OFFSET_DISPLAY));

}

/**
 * @brief MenuUDiskFileList_RefreshPage
 * 绘制U盘文件浏览窗口显示内容的函数
 * @param pMenu
 * 当前活动窗口
 * @return
 */
int MenuUDiskFileList_RefreshPage(struct UI_Menu_tag *pMenu)
{
	INT8U i;
	int nFocus;
	
    for(i = 0; i < BUTTON_CNT_UDISKFILELIST; i++)
	{
		if(i == pMenu->nButtonIdx)
			nFocus=1;
		else
			nFocus=0;

        MenuUDiskFileList_RefreshItem(pMenu,i,nFocus);//绘制button及文字的函数
	}

	RefreshOSDDisplay();

	return 1;
}

/**
 * @brief MenuUDiskFileList_Refresh
 * 绘制U盘文件浏览窗口的函数，比如设置：背景、标题等
 * @param pMenu
 * @return
 */
int MenuUDiskFileList_Refresh(UI_Menu_t *pMenu)
{
	int i;
	GUI_BITMAP *pBmp_BG = NULL;

	pBmp_BG = (&bmMainMenuBG_360_320);

	GUI_SetBkColor(GUI_BLACK);
	GUI_SetFont(&GUI_Font20_SongTi_Unicode);
	GUI_SetColor(GUI_WHITE);

	//--------------------------------------------------------------------------//
	//	Purpose	:	绘制背景
	//--------------------------------------------------------------------------//
	GUI_DrawBitmap(pBmp_BG, 
							AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize), 
							AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize));

	//--------------------------------------------------------------------------//
	//	Purpose	:	填写Title
	//--------------------------------------------------------------------------//
	GUI_SetTextMode(GUI_TM_TRANS);
	GUI_DispStringHCenterAt(UDiskFileList[pUISystem->allData.Language], 
							(LCD_WIDTH / 2), 
							(AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_BELOW_HEIGHT));

	//--------------------------------------------------------------------------//
	//	Purpose	:	填写显示内容
	//--------------------------------------------------------------------------//
	MenuUDiskFileList_RefreshPage(pMenu);

	//设定刷新区域
	SetRefreshAreaPos(OSDPOS_BASEMENU, 
					AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize), 
					AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize), 
					pBmp_BG->XSize, 
					pBmp_BG->YSize, 
					ALPHA_COMMON,
					TRUE);
	return 1;
}

/*
*Do File Copy Menu 
*display file copy progress and do file copy
*/

/*
 * DoFileCopyMessage_Deinit
 * 拷贝完成/失败的提示信息显示完成后的处理
 * 根据当前文件拷贝状态,修改文件拷贝状态并返回父窗口
 */
int DoFileCopyMessage_Deinit(int ignored)
{
	int ret;

	switch(pUISystem->nFileCopyToStage)
	{
		// copy file complete.Return to parent menu
		case MESSAGESTAGE_COPY_TO_UDISK:
			LOGT("copy stop\r\n");
			pUISystem->nFileCopyToStage = MESSAGESTAGE_IDLE;
            //zqh 2015年3月30日 modify，将当文件从电子盘往U盘里复制完成后，应回到“文件操作”的界面，而不是返回“文件浏览”的界面
            //SystemChangeWindow(MENUFILEBROWSER,1,1,0);
            SystemChangeWindow(MENUFILEOPERATION,1,1,0);
            break;
		case MESSAGESTAGE_COPY_TO_CFCARD:
			LOGT("copy stop\r\n");
			pUISystem->nFileCopyToStage = MESSAGESTAGE_IDLE;

    	    sync();
	        sync();
            //zqh 2015年3月30日 modify，当文件从U盘复制到电子盘完成后，应回到“系统功能”的界面，而不是返回“文件浏览”的界面
            //SystemChangeWindow(MENUFILEBROWSER,1,1,0);
            SystemChangeWindow(MENUSYSTEMFUNC,1,1,0);

            // 文件拷贝增加了fsync函数,此处不需要umount cf卡
			break;

		// back up complete. Start update
		case MESSAGESTAGE_UPDATE_BACKUP:
			LOGT("backup ok\r\n");

            ret = DoUDiskDetect();
			if((ret<0)||(!TESTBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_Mount_Flag)))
			{
				pUISystem->nFileCopyToStage = MESSAGESTAGE_IDLE;
				SystemChangeWindow(MENUFILEBROWSER,1,1,0);

				ERR("Need Mount UDisk first!\r\n");
				CLEARBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_Mount_Flag);
                MessageBox(HelpMountUDisk[pUISystem->allData.Language],DEV_MESSAGE_DELAY,
                        NULL,NULL,NULL,NULL,NULL,NULL);
                break;
			}

			// Start update
			pUISystem->nFileCopyToStage = MESSAGESTAGE_UPDATE_UPDATE;

#if 0
            // Tag 0607-2012
            // do not rm pvr. copy pvr to /opt/pvr.t. remove pvr in daemon & rename pvr.t to pvr
            START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_DELETE_FILE);
			if(remove(Update_DestFile))
			{
				ERR("%s:Delete File failed!\n",__func__);
			}

            sync();
			sync();
			STOP_WATCHDOG_TIMER_SIG();
#endif
            // 直接拷贝到 pvr/daemon
			StartCopyFile(Update_SourceFile,Update_DestFile);
#if 0
			StartCopyFile(Update_SourceFile,Update_DestTmpFile);
#endif
			break;
		// update or copyback complete. Return to parent menu
		case MESSAGESTAGE_UPDATE_UPDATE:
		case MESSAGESTAGE_UPDATE_COPYBACK:
			LOGT("update ok\r\n");
			// resset Update progress
			pUISystem->nFileCopyToStage = MESSAGESTAGE_IDLE;

            // set update not in progress
            SetUpdateInProgress(0);
            Update_Reboot(1);
            break;

		// copy file canceled. Return to parent menu and remove file
        case MESSAGESTAGE_CANCEL_COPYTOUDISK:
            LOGT("copy to udisk cancel\r\n");

			// fixed 0516 有可能会从U盘拷贝到CF卡过程中拔掉U盘
//			ret = DoUDiskDetect();
//			if(!(TESTBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_REMOVE)))
			{
				START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_DELETE_FILE);
				if(remove(pUISystem->sFileCopyTo))
				{
					ERR("%s:Delete File:%s failed!\n",__func__,pUISystem->sFileCopyTo);
                }

				sync();
				sync();
				STOP_WATCHDOG_TIMER_SIG();
			}

			pUISystem->nFileCopyToStage = MESSAGESTAGE_IDLE;
            //zqh 2015年3月30日 modify，当从电子盘往U盘中复制文件时，取消复制后，应回到“文件操作”的界面，而不是返回“文件浏览”的界面
            //SystemChangeWindow(MENUFILEBROWSER,1,1,0);
            SystemChangeWindow(MENUFILEOPERATION,1,1,0);
			break;

            // copy file canceled. Return to parent menu and remove file
        case MESSAGESTAGE_CANCEL_COPYTOCFCARD:
            LOGT("copy to cf card cancel\r\n");

            // fixed 0516 有可能会从U盘拷贝到CF卡过程中拔掉U盘
//			ret = DoUDiskDetect();
//			if(!(TESTBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_REMOVE)))
            {
                START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_DELETE_FILE);
                if(remove(pUISystem->sFileCopyTo))
                {
                    ERR("%s:Delete File:%s failed!\n",__func__,pUISystem->sFileCopyTo);
                }

                sync();
                sync();
                STOP_WATCHDOG_TIMER_SIG();
            }

            pUISystem->nFileCopyToStage = MESSAGESTAGE_IDLE;
            //zqh 2015年3月30日 modify，当从U盘往电子盘复制文件的过程中，取消复制时，应回到“系统功能”的界面，而不是返回“文件浏览”的界面
            //SystemChangeWindow(MENUFILEBROWSER,1,1,0);
            SystemChangeWindow(MENUSYSTEMFUNC,1,1,0);
            break;

		// backup canceled. Remove backup file and return to parent.
		case MESSAGESTAGE_CANCEL_BACKUP:
			LOGT("backup cancel\r\n");

			START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_DELETE_FILE);

			if(remove(pUISystem->sFileCopyTo))
			{
				ERR("%s:Delete File failed!\n",__func__);
            }

			sync();
			sync();

			STOP_WATCHDOG_TIMER_SIG();

			pUISystem->nFileCopyToStage = MESSAGESTAGE_IDLE;

#if 0
            // remount to ro fs
			ret = mount(RFS_MOUNT_NOD,RFS_MOUNT_DST,RFS_MOUNT_TYPE,MS_RDONLY|MS_REMOUNT,0);
			if(ret<0)
			{
				ERR("remount RFS to ro err:%s\r\n",strerror(errno));
			}
			SystemChangeWindow(MENUFILEBROWSER,1,1,0);
#else
			// set update not in progress
			SetUpdateInProgress(0);
//            // reboot to set rfs in ro mode
//			Update_Reboot(1);
			SystemChangeWindow(MENUFILEBROWSER,1,1,0);
#endif
			break;

		// update canceled. Start copy back.
		case MESSAGESTAGE_CANCEL_UPDATE:
			LOGT("update cancel\r\n");

// Tag 0607-2012
// do not remove pvr now. so we do not need to copy pvr back.
//			LOGT("start copyback\r\n");

			START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_DELETE_FILE);

			pUISystem->nFileCopyToStage = MESSAGESTAGE_UPDATE_COPYBACK;
			if(remove(Update_DestFile))
            {
                LOGE("Delete File:%s failed!\n",Update_DestFile);
            }         
#if 0
			if(remove(Update_DestTmpFile))
			{
				ERR("%s:Delete File:%s failed!\n",__func__,Update_DestTmpFile);
			}
#endif

			sync();
			sync();
			STOP_WATCHDOG_TIMER_SIG();

			SetUpdateInProgress(0);
//            // reboot to set rfs in ro mode
//			Update_Reboot(1);

			SystemChangeWindow(MENUFILEBROWSER,1,1,0);
//			StartCopyFile(Update_BackUpFile,Update_DestFile);
			break;
		default:
			break;
	}

	return 1;
}

int MenuDoFileCopy_Close(UI_Menu_t *pMenu)
{
	return 1;
}

/**
 * @brief MenuDoFileCopy_MsgHandle
 * 拷贝文件窗口的按键消息处理函数
 * @param pMenu
 * @param nMsgType
 * @param nMsgData
 * @return
 */
TMsgHandleStat MenuDoFileCopy_MsgHandle(UI_Menu_t *pMenu,unsigned short nMsgType,unsigned short nMsgData)
{
	TMsgHandleStat retval=MsgHandled;

	if (nMsgType!=MSG_KEY_EVENT){
		return MsgNocare;
	}

	switch(nMsgData){
		case KEY_ESC:
//			SystemMenuRestore();
			break;
		default:
			retval=MsgNeedDispatch;
			break;
	}

	return retval;
}

/**
 * @brief MenuDoFileCopy_Init
 * 拷贝文件之前的初始化，检测源文件路径跟目的文件路径是否存在
 * @param pMenu
 * 当前活动窗口
 * @return
 */
int MenuDoFileCopy_Init(UI_Menu_t *pMenu)
{
	if((pUISystem->sFileCopyFrom[0]==0)||(pUISystem->sFileCopyTo[0]==0))
	{
		ERR("Invalid file path\n");
		return -1;
	}
	return 1;
}

/**
 * @brief GetFileShortName
 * 获得文件的短文件名
 * @param fullname
 * 全路径名，例如：/media/cf/2010*.avi
 * @param shortname
 * 短文件名2010*.avi
 * @return
 */
int GetFileShortName(char *fullname,char *shortname)
{
	char *str;
	char tmpstr[MAX_FS_PATH];
	char tmpstr2[MAX_FS_PATH];

	FUNC_ENTER;

	memset(tmpstr,0,sizeof(tmpstr));
	memset(tmpstr2,0,sizeof(tmpstr2));
	strncpy(tmpstr,fullname,MAX_FS_PATH-1);
	strncpy(tmpstr2,fullname,MAX_FS_PATH-1);
	LOGT("Full Path is %s\n",tmpstr2);
	do
	{
		str = strstr(tmpstr2,"/");
		if(str==NULL) break;
		str++;
		strncpy(tmpstr,str,MAX_FS_PATH-1);
		memcpy(tmpstr2,tmpstr,sizeof(tmpstr2));
	}while(1);
	LOGT("short name is %s\n",tmpstr2);
	strncpy(shortname,tmpstr,MAX_FS_PATH);
	return 1;
}

// 增加文件拷贝线程,在刷新过程中检查文件拷贝进度,将刷新放在每秒的timeout中
extern int stdin_get_message(struct timeval tv,int fd);
extern unsigned int watchdog_time_out_cnt;
#include <pthread.h>
#include <signal.h>

static pthread_t file_copy_thread;
static int file_copy_running = 0;//文件拷贝运行状态
static int file_copy_en = 0;//文件拷贝使能标志
struct FileCopyParam
{
    int from_fd;
    int to_fd;
};

#define BUFFER_SIZE 		(1024*64)
static char FileCopyBuffer[BUFFER_SIZE];

/**
 * @brief FileCopyThread
 * 文件拷贝的线程，此线程在这个函数中StartFileCopyThread()创建
 * @param arg
 * @return
 */
void *FileCopyThread(void *arg)
{
	int bytes_read,bytes_write;
	char *ptr;
//    char *FileCopyBuffer=NULL;
    struct FileCopyParam * param = (struct FileCopyParam *)arg;
    int from_fd;
    int to_fd;
    int ret =0;

    FUNC_ENTER;

    ret = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
    if(ret)
    {
        LOGE("set cancel state error\r\n");
        return;
    }

    ret = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
    if(ret)
    {
        LOGE("set cancel type error\r\n");
        return;
    }

    from_fd = param->from_fd;
    to_fd = param->to_fd;

    file_copy_running = 1;
    file_copy_en = 1;

	/* 以下代码是一个经典的拷贝文件的代码 */
	while( file_copy_en && (bytes_read=read(from_fd,FileCopyBuffer,BUFFER_SIZE)))
	{
        /* 一个致命的错误发生了 */
        // NOTE: EINTR
		if((bytes_read==-1)/*&&(errno!=EINTR)*/)
		{
			// add debug msg
			LOGE("read file error:%s",strerror(errno));
			break;
		}
		else if(bytes_read>0)
		{
			ptr=FileCopyBuffer;
			while((bytes_write=write(to_fd,ptr,bytes_read)))
			{
				/* 一个致命错误发生了 */
				if((bytes_write==-1)/*&&(errno!=EINTR)*/)
				{
					// add debug msg
					LOGE("%s:write file error:%s",__func__,strerror(errno));
					// set fault write stat
					bytes_write=-1;
					break;
				}
				/* 写完了所有读的字节 */
				else if(bytes_write==bytes_read)
				{
					break;
				}
				/* 只写了一部分,继续写 */
				else if(bytes_write>0)
				{
					ptr+=bytes_write;
					bytes_read-=bytes_write;
				}
			}
			
			/* 写的时候发生的致命错误 */
			if(bytes_write==-1)
			{
				break;
			}
		}
	}

    fsync(to_fd);

    file_copy_running = 0;//文件拷贝完成，此变量在StopFileCopyThread()中调用

    FUNC_EXIT;
    pthread_exit(0);
}

/**
 * @brief StartFileCopyThread
 * 创建文件拷贝的线程
 * @param from_fd
 * 拷贝的源文件
 * @param to_fd
 * 拷贝的目的文件
 * @return
 */
int StartFileCopyThread(int from_fd,int to_fd)
{
    int ret = 0;
    struct FileCopyParam param;

    param.from_fd = from_fd;
    param.to_fd = to_fd;

    FUNC_ENTER;

    ret = pthread_create(&file_copy_thread,NULL,FileCopyThread,&param);//创建线程
	if(ret)
	{
		LOGE("file_copy_thread create err:%s\r\n",strerror(errno));
        return -1;
	}
    FEED_WATCHDOG();
    sleep(1);
    FEED_WATCHDOG();

	return 0;
}

/**
 * @brief StopFileCopyThread
 * 停止文件拷贝的线程函数
 */

void StopFileCopyThread(void)
{
    int ret;
    int i;

    FUNC_ENTER;
	file_copy_en = 0;

    // 最多等待10s,然后强制停止拷贝
    for(i=0;i<20;i++)
    {
        if(file_copy_running == 0)
            return;
        usleep(500000);
        FEED_WATCHDOG();
    }

    START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_DELETE_FILE);

    ret = pthread_cancel(file_copy_thread);//终止文件拷贝线程
	if(ret)
	{
		LOGE("file_copy_thread cancel err:%s\r\n",strerror(errno));
	}

    if(pthread_join(file_copy_thread,&i)==0)//SACH:W10845
	{
		STOP_WATCHDOG_TIMER_SIG();
	}    
}

/**
 * @brief MenuDoFileCopy_Refresh
 * 文件拷贝窗口的绘制函数，包括绘制显示背景、标题、拷贝的进度条等
 * @param pMenu
 * 当前活动窗口
 * @return
 */

int MenuDoFileCopy_Refresh(UI_Menu_t *pMenu)
{
	GUI_BITMAP *pProgress = &bmProgressBar_240_20;
	GUI_BITMAP *pProgressFill = &bmProgressBarFill_12_20;
	GUI_BITMAP *pBmp_BG = &bmMessageBoxBG_360_240;
	GUI_BITMAP *pBmp_SubBG = (&bmMessageRect_310_130);
	GUI_RECT Rect;
	char MessageDisp[MAX_FS_PATH+10];
	char tmpstr[MAX_FS_PATH];
	char MessageProgress[6];
	int bmp_sub_x0;
	int bmp_sub_y0;
	int bmp_pro_x0;
	int bmp_pro_y0;

//固定拷贝一定长度然后刷新界面
	unsigned int FILE_COPY_OP_CNT = 2048/64;

	char *src_file = pUISystem->sFileCopyFrom;
	char *dst_file = pUISystem->sFileCopyTo;
	int from_fd,to_fd;
#if !(USE_FILE_COPY_THREAD)
	int bytes_read,bytes_write;
	char buffer[BUFFER_SIZE];
	char *ptr;
#endif
	struct stat stat_src;
	struct stat stat_dst;
	int nProgress;
	float fPercent;
	int nPercent5;
	int blockcnt;
	int i;
	int ret;
	static int musb_tick_cnt=0;

//used in get key
	struct timeval tv;
	int my_key;
	unsigned short nMsgType;
	unsigned short nMsgData;
	TMsgHandleStat tMsgState;

	FUNC_ENTER;

	GUI_SetFont(&GUI_Font20_SongTi_Unicode);
	GUI_SetColor(GUI_WHITE);
	GUI_SetBkColor(GUI_BLACK);

	//绘制背景
	bmp_sub_x0 = AUTO_GET_X0_FROM_WIDTH(pBmp_SubBG->XSize);
	bmp_sub_y0 = AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_ALL_HEIGHT + BUTTON_V_INTERVAL_COMMON + TITLE_ALL_HEIGHT/2;
	GUI_DrawBitmap(pBmp_BG,
				AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize),
				AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize));
	GUI_DrawBitmap(pBmp_SubBG,
				bmp_sub_x0,
				bmp_sub_y0);

	//填写Title与底部提示信息
	GUI_SetTextMode(GUI_TM_TRANS);

	memset(MessageDisp,0,sizeof(MessageDisp));
	switch(pUISystem->nFileCopyToStage)
	{
		case MESSAGESTAGE_UPDATE_UPDATE:
			FILE_COPY_OP_CNT = 1024/64;
			strncpy(MessageDisp,FileUpdate[pUISystem->allData.Language],MAX_FS_PATH-1);
			break;
		case MESSAGESTAGE_UPDATE_BACKUP:
			FILE_COPY_OP_CNT = 512/64;
			strncpy(MessageDisp,FileBackUp[pUISystem->allData.Language],MAX_FS_PATH-1);
			break;
		case MESSAGESTAGE_UPDATE_COPYBACK:
			FILE_COPY_OP_CNT = 512/64;
			strncpy(MessageDisp,FileCopyBack[pUISystem->allData.Language],MAX_FS_PATH-1);
			break;
		case MESSAGESTAGE_COPY_TO_CFCARD://SACH:W10867
		case MESSAGESTAGE_COPY_TO_UDISK:
			FILE_COPY_OP_CNT = 2048/64;
		default:	// 如果已经手动停止,则不更改状态
			strncpy(MessageDisp,FileCopy[pUISystem->allData.Language],MAX_FS_PATH-1);
			break;
	}
    //strncat(MessageDisp,":",MAX_FS_PATH-strlen(MessageDisp)-1);

    LOGT("Copy from %s to %s\r\n",src_file,dst_file);
	GetFileShortName(src_file,tmpstr);

	GUI_DispStringHCenterAt(MessageDisp, 
							(LCD_WIDTH / 2), 
							(AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_BELOW_HEIGHT));

	Rect.x0 = AUTO_GET_X0_FROM_WIDTH(pBmp_SubBG->XSize);
	Rect.y0 = AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize);
	Rect.x1 = AUTO_GET_X0_FROM_WIDTH(pBmp_SubBG->XSize) + pBmp_SubBG->XSize;
	Rect.y1 = AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + pBmp_BG->YSize;

	bmp_pro_x0 = bmp_sub_x0 + (pBmp_SubBG->XSize-pProgress->XSize)/2;
	bmp_pro_y0 = bmp_sub_y0 + pBmp_SubBG->YSize - pProgress->YSize/2 - 25;

	memset(MessageDisp,0,sizeof(MessageDisp));
    //zqh 2015年3月30日 modify，将“复制文件”改为“文件名”
    strncpy(MessageDisp,FileName[pUISystem->allData.Language],MAX_FS_PATH-1);
	strncat(MessageDisp,":",MAX_FS_PATH-strlen(MessageDisp)-1);
	GetFileShortName(src_file,tmpstr);
	strncat(MessageDisp,tmpstr,MAX_FS_PATH-strlen(MessageDisp)-1);
	GUI_DispStringInRect(MessageDisp, &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);

	// feed watchdog before copy files
	FEED_WATCHDOG();
#if USE_FILE_COPY_THREAD
    StartParseMUSBStatus(10);

	//文件复制
	/* 打开源文件 */
    from_fd=open(src_file,O_RDONLY);
	if(from_fd < 0)   /*open file readonly,返回-1表示出错，否则返回文件描述符*/
	{
		ERR("Open %s Error:%s\n",src_file,strerror(errno));
		strcpy(MessageDisp,MsgBox36[pUISystem->allData.Language]);
		MessageBox(MessageDisp,DEV_MESSAGE_DELAY,
					NULL,NULL,NULL,NULL,NULL,DoFileCopyMessage_Deinit);

		goto MenuFileCopyRefreshEnd;
	}
    LOGT("from_fd=%d\r\n",from_fd);
// 1    close(from_fd);

	/* 创建目的文件 */
	
	// 检查目标文件是否存在，如果存在，则先删除旧文件
	to_fd = open(dst_file,O_RDONLY);
	if(to_fd > 0)
    {
        close(to_fd);
        START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_DELETE_FILE);
        LOGT("file exist ,remove old file\r\n");
        if(remove(dst_file) < 0)
        {
            STOP_WATCHDOG_TIMER_SIG();
            LOGE("remove %s error\r\n",dst_file);
            close(from_fd);
            strcpy(MessageDisp,UDiskReadOnly[pUISystem->allData.Language]);
            MessageBox(MessageDisp,DEV_MESSAGE_DELAY,
                    NULL,NULL,NULL,NULL,NULL,DoFileCopyMessage_Deinit);

            goto MenuFileCopyRefreshEnd;
        }
        STOP_WATCHDOG_TIMER_SIG();
    }

	/* 使用了O_CREAT选项-创建文件,open()函数需要第3个参数,
	mode=S_IRUSR|S_IWUSR表示S_IRUSR 用户可以读 S_IWUSR 用户可以写*/
	/*
	S_IRWXU is the bitwise OR of S_IRUSR, S_IWUSR and S_IXUSR.
	S_IRWXG is the bitwise OR of S_IRGRP, S_IWGRP and S_IXGRP.
	S_IRWXO is the bitwise OR of S_IROTH, S_IWOTH and S_IXOTH.
	*/
    to_fd=open(dst_file,O_WRONLY|O_CREAT,S_IRWXU|S_IRWXG|S_IRWXO);
	if(to_fd < 0)
    {
		ERR("Open %s Error:%s error code:%d\r\n",dst_file,strerror(errno),errno);
		close(from_fd);
		strcpy(MessageDisp,UDiskReadOnly[pUISystem->allData.Language]);
		MessageBox(MessageDisp,DEV_MESSAGE_DELAY,
					NULL,NULL,NULL,NULL,NULL,DoFileCopyMessage_Deinit);
		goto MenuFileCopyRefreshEnd;
	}
    LOGT("to_fd=%d\r\n",to_fd);
// 1    close(to_fd);

	stat(src_file,&stat_src);
	nProgress = 0;
	SetRefreshAreaPos(OSDPOS_BASEMENU, 
					AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize), 
					AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize), 
					pBmp_BG->XSize, 
					pBmp_BG->YSize, 
					ALPHA_COMMON,
					TRUE);

    // 创建拷贝进程
// 1    StartFileCopyThread(src_file,dst_file);
    StartFileCopyThread(from_fd,to_fd);

    // 刷新拷贝进度,获取按键信息
    while(file_copy_running)
    {
        if(stat(dst_file,&stat_dst) < 0)
        {
            LOGE("check dst file size error %s error code:%d \r\n",strerror(errno),errno);
            break;
        }

        nProgress = stat_dst.st_size;
    	fPercent = nProgress*100.0/stat_src.st_size;
    	LOGT("%3.1f%% complete,copy %d bytes success\n",fPercent,nProgress);
    	//填写显示内容
    	nPercent5 = fPercent/5;
    	sprintf(MessageProgress,"%3.1f%%",fPercent);
    	GUI_DrawBitmap(pProgress,bmp_pro_x0,bmp_pro_y0);
    	for(blockcnt=0;blockcnt<nPercent5;blockcnt++)
    	{
    		GUI_DrawBitmap(pProgressFill,
    			(bmp_pro_x0+pProgressFill->XSize*blockcnt),
    			bmp_pro_y0);
    	}
    	GUI_DispStringHCenterAt(MessageProgress,
    		(LCD_WIDTH / 2),
    		bmp_pro_y0);
    	RefreshOSDDisplay();

        //get key message
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        if(stdin_get_message(tv,rs422fd)!=0)
        {
        	my_key = GUI_GetKey();
        	if (my_key)
        	{
        		nMsgType=(my_key>>16)&0xffff;
        		nMsgData=(my_key&0xffff);
        		LOGT("nMsgType = 0x%x;nMsgData = 0x%x\n",nMsgType,nMsgData);

        		// 当前是Copyback时,不能取消
        		if(pUISystem->nFileCopyToStage != MESSAGESTAGE_UPDATE_COPYBACK)
        			tMsgState = MenuDoFileCopy_MsgHandle(PACTIVE_MENU,nMsgType,nMsgData);
        		else
        			tMsgState = MsgNeedDispatch;

        		if(tMsgState == MsgHandled)
        		{
        			//Cancel
        			LOGT("%s:Copy Stoped!\n",__func__);
        			break;
        		}
        	}
        }else{//zqh 2015年10月16日 add， 复制文件过程中也要给上位机发送板卡状态。
            Handle_Timer_Tick(&pUISystem->sDevScanTimer);
        }
        if(nProgress == stat_src.st_size)
        {
            LOGT("copy complete\r\n");
            break;
        }
    }
    LOGT("exit while loop\r\n");

    // 退出时确认关闭拷贝线程
    StopFileCopyThread();

    // 确保文件写入
    fsync(to_fd);

    // 拷贝结束后关闭文件
    close(from_fd);
    close(to_fd);

    // 退出拷贝时再次检查目标文件大小
    if(stat(dst_file,&stat_dst) < 0)
    {
        LOGE("check dst file size error %s error code:%d \r\n",strerror(errno),errno);
        nProgress = 0;
    }

    nProgress = stat_dst.st_size;

    StopParseMUSBStatus();
#else
	//文件复制
	/* 打开源文件 */
	if((from_fd=open(src_file,O_RDONLY)) < 0)   /*open file readonly,返回-1表示出错，否则返回文件描述符*/
	{
		ERR("Open %s Error:%s\n",src_file,strerror(errno));
		strcpy(MessageDisp,MsgBox36[pUISystem->allData.Language]);
		MessageBox(MessageDisp,DEV_MESSAGE_DELAY,
					NULL,NULL,NULL,NULL,NULL,DoFileCopyMessage_Deinit);

		goto MenuFileCopyRefreshEnd;
	}
    LOGT("to_fd=%d\r\n",from_fd);

	/* 创建目的文件 */
	/* 使用了O_CREAT选项-创建文件,open()函数需要第3个参数,
	mode=S_IRUSR|S_IWUSR表示S_IRUSR 用户可以读 S_IWUSR 用户可以写*/
	/*
	S_IRWXU is the bitwise OR of S_IRUSR, S_IWUSR and S_IXUSR.
	S_IRWXG is the bitwise OR of S_IRGRP, S_IWGRP and S_IXGRP.
	S_IRWXO is the bitwise OR of S_IROTH, S_IWOTH and S_IXOTH.
	*/
	if((to_fd=open(dst_file,O_WRONLY|O_CREAT,S_IRWXU|S_IRWXG|S_IRWXO)) < 0) 
	{
		ERR("Open %s Error:%s\n",dst_file,strerror(errno));
		close(from_fd);
//		SystemMenuRestore();
		strcpy(MessageDisp,UDiskReadOnly[pUISystem->allData.Language]);
		MessageBox(MessageDisp,DEV_MESSAGE_DELAY,
					NULL,NULL,NULL,NULL,NULL,DoFileCopyMessage_Deinit);
		goto MenuFileCopyRefreshEnd;
	}
    LOGT("to_fd=%d\r\n",to_fd);

	if(stat(src_file,&stat_src) == -1)//SACH:W10848
	{
		return -1;
	}
	nProgress = 0;
	i = 0;

	StartParseMUSBStatus(10);

	SetRefreshAreaPos(OSDPOS_BASEMENU, 
					AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize), 
					AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize), 
					pBmp_BG->XSize, 
					pBmp_BG->YSize, 
					ALPHA_COMMON,
					TRUE);

	/* 以下代码是一个经典的拷贝文件的代码 */
	while((bytes_read=read(from_fd,buffer,BUFFER_SIZE)))
	{
	    START_WATCHDOG_TIMER_SIG(120);
		/* 一个致命的错误发生了 */
		// TODO EINTR
		if((bytes_read==-1)/*&&(errno!=EINTR)*/)
		{
			// add debug msg
			LOGE("%s:read file error:%s",__func__,strerror(errno));
			break;
		}
		else if(bytes_read>0)
		{
			ptr=buffer;
			while((bytes_write=write(to_fd,ptr,bytes_read)))
			{
				/* 一个致命错误发生了 */
				if((bytes_write==-1)/*&&(errno!=EINTR)*/)
				{
					// add debug msg
					LOGE("%s:write file error:%s",__func__,strerror(errno));
					// set fault write stat
					bytes_write=-1;
					break;
				}
				/* 写完了所有读的字节 */
				else if(bytes_write==bytes_read)
				{
					nProgress += bytes_write;
					break;
				}
				/* 只写了一部分,继续写 */
				else if(bytes_write>0)
				{
					ptr+=bytes_write;
					bytes_read-=bytes_write;
					nProgress += bytes_write;
				}
			}
			
			/* 写的时候发生的致命错误 */
			if(bytes_write==-1)
			{
				break;
			}
		}
        STOP_WATCHDOG_TIMER_SIG();
        // 拷贝文件操作超时
        if(watchdog_time_out_cnt >= 60)
        {
            LOGT("Copy failed!\n");
			break;
        }
		i++;
		if(i == FILE_COPY_OP_CNT)
		{
			i = 0;

			fPercent = nProgress*100.0/stat_src.st_size;
			LOGT("%3.1f%% complete,copy %d bytes success\n",fPercent,nProgress);
			//填写显示内容
			nPercent5 = fPercent/5;
			sprintf(MessageProgress,"%3.1f%%",fPercent);
			GUI_DrawBitmap(pProgress,bmp_pro_x0,bmp_pro_y0);
			for(blockcnt=0;blockcnt<nPercent5;blockcnt++)
			{
				GUI_DrawBitmap(pProgressFill,
					(bmp_pro_x0+pProgressFill->XSize*blockcnt),
					bmp_pro_y0);
			}
			GUI_DispStringHCenterAt(MessageProgress,
				(LCD_WIDTH / 2),
				bmp_pro_y0);
			RefreshOSDDisplay();
		}

		//get key message
		tv.tv_sec = 0;
		tv.tv_usec = 0;
		if(stdin_get_message(tv,rs422fd)!=0)
		{
			my_key = GUI_GetKey();
			if (my_key)
			{
				nMsgType=(my_key>>16)&0xffff;
				nMsgData=(my_key&0xffff);
				LOGT("nMsgType = 0x%x;nMsgData = 0x%x\n",nMsgType,nMsgData);

				// 当前是Copyback时,不能取消
				if(pUISystem->nFileCopyToStage != MESSAGESTAGE_UPDATE_COPYBACK)
					tMsgState = MenuDoFileCopy_MsgHandle(PACTIVE_MENU,nMsgType,nMsgData);
				else
					tMsgState = MsgNeedDispatch;

				if(tMsgState == MsgHandled)
				{
					//Cancel
					LOGT("%s:Copy Stoped!\n",__func__);
					break;
				}
			}
		}
	}

	close(from_fd);
	close(to_fd);

	StopParseMUSBStatus();
#endif

	if(nProgress == stat_src.st_size)
	{
		LOGT("copy complete!\n");
		fPercent = nProgress*100.0/stat_src.st_size;
		LOGT("%.1f%% complete,copy %d bytes success\n",fPercent,nProgress);

		//填写显示内容
		nPercent5 = fPercent/5;
		LOGT("nPercent5 = %d\n",nPercent5);
		sprintf(MessageProgress,"%3.1f%%",fPercent);
		GUI_DrawBitmap(pProgress,bmp_pro_x0,bmp_pro_y0);
		for(blockcnt=0;blockcnt<nPercent5;blockcnt++)
		{
			GUI_DrawBitmap(pProgressFill,
				(bmp_pro_x0+pProgressFill->XSize*blockcnt),
				bmp_pro_y0);
		}
		GUI_DispStringHCenterAt(MessageProgress,
			(LCD_WIDTH / 2),
			bmp_pro_y0);
//Copy file success
		// display "Copy success!" message
		memset(MessageDisp,0,sizeof(MessageDisp));
		switch(pUISystem->nFileCopyToStage)
		{
			case MESSAGESTAGE_COPY_TO_CFCARD:
			case MESSAGESTAGE_COPY_TO_UDISK:
				strcpy(MessageDisp,MsgBox35[pUISystem->allData.Language]);
				break;
			case MESSAGESTAGE_UPDATE_BACKUP:
				strcpy(MessageDisp,FileBackUpSuc[pUISystem->allData.Language]);
				break;
			case MESSAGESTAGE_UPDATE_UPDATE:
				strcpy(MessageDisp,MsgBox35[pUISystem->allData.Language]);
				break;
			case MESSAGESTAGE_UPDATE_COPYBACK:
				strcpy(MessageDisp,FileCopyBackSuc[pUISystem->allData.Language]);
				break;
			default:
				strcpy(MessageDisp,MsgBox35[pUISystem->allData.Language]);
				break;
		}

		MessageBox(MessageDisp,DEV_MESSAGE_DELAY,
					NULL,NULL,NULL,NULL,NULL,DoFileCopyMessage_Deinit);
//		SystemChangeWindow(MENUFILEBROWSER,1,1,0);
	}
	else
	{
		// Copy未完成,跟据当前状态,更改CopyToStage的状态
		memset(MessageDisp,0,sizeof(MessageDisp));
		switch(pUISystem->nFileCopyToStage)
		{
			case MESSAGESTAGE_COPY_TO_CFCARD:
                strcpy(MessageDisp,MsgBox36[pUISystem->allData.Language]);
                pUISystem->nFileCopyToStage = MESSAGESTAGE_CANCEL_COPYTOCFCARD;//zqh 2015年3月30日 add
			case MESSAGESTAGE_COPY_TO_UDISK:
				strcpy(MessageDisp,MsgBox36[pUISystem->allData.Language]);
                pUISystem->nFileCopyToStage = MESSAGESTAGE_CANCEL_COPYTOUDISK;//zqh 2015年3月30日 add
				break;
			case MESSAGESTAGE_UPDATE_BACKUP:
				strcpy(MessageDisp,FileBackUpFail[pUISystem->allData.Language]);
				pUISystem->nFileCopyToStage = MESSAGESTAGE_CANCEL_BACKUP;
				break;
			case MESSAGESTAGE_UPDATE_UPDATE:
				strcpy(MessageDisp,MsgBox36[pUISystem->allData.Language]);
				pUISystem->nFileCopyToStage = MESSAGESTAGE_CANCEL_UPDATE;
				break;
            // 回拷失败
			case MESSAGESTAGE_UPDATE_COPYBACK:	
				strcpy(MessageDisp,FileCopyBackFail[pUISystem->allData.Language]);
				break;
			default:
				strcpy(MessageDisp,MsgBox36[pUISystem->allData.Language]);
				break;
		}

		ERR("copy failed!\n");
		MessageBox(MessageDisp,DEV_MESSAGE_DELAY,
			NULL,NULL,NULL,NULL,NULL,DoFileCopyMessage_Deinit);
/*
		ret = DoUDiskDetect();

		if(!(TESTBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_REMOVE)))
		{
			if(remove(dst_file))
			{
				ERR("%s:Delete File failed!\n",__func__);
			}
		}
*/
	}

MenuFileCopyRefreshEnd:

	//temp--设定刷新区域
	SetRefreshAreaPos(OSDPOS_BASEMENU, 
					AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize), 
					AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize), 
					pBmp_BG->XSize, 
					pBmp_BG->YSize, 
					ALPHA_COMMON,
					TRUE);

	return 1;
}

