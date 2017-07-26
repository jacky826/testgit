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
extern void Handle_Timer_Tick(TEF_TIMER_STRUCT *pTimer);//Ϊ�˵��ô˺���

/*Message Box*/
/**
 * @brief MessageBox_Deinit
 * ��Ϣ���ڵķ���ʼ��
 * @return
 */
int MessageBox_Deinit(void)
{
    DisableTimer(&pUISystem->sKeyTimer);//�رն�ʱ��

	if (VALIDFUNCTION(pUISystem->pMessageDeinit_fn)){
		return pUISystem->pMessageDeinit_fn(pUISystem->nMessageStage);
	}else{
	    LOGT("MenuRestore\r\n");
		return SystemMenuRestore();
	}
}

/**
 * @brief MessageBox_Cancel
 * ����MessageBox_Deinit()�������з���ʼ��
 * ��������Ϣ״̬��ΪMenuMessageBox_Refresh()��ʾ��׼��
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
 * ��Ϣ��������ʾ��ʱ��������
 * ��MenuMessageBox_Open()��EnableTimer()�ĺ����н��г�ʼ��
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
 * ��Ϣ������ʼ����
 * @param pMenu
 * ��ǰ�����
 * @return
 */
int MenuMessageBox_Init(UI_Menu_t *pMenu){
	//Do some check
    //FUNC_ENTER
    if((pUISystem->pMessageDisplay== NULL)||(strlen(pUISystem->pMessageDisplay)==0)) {//����Ƿ�����Ҫ��ʾ�����ݣ�����ֱ�ӷ���
		return 0;
	}//Nothing to Display
	DisableTimer(&pUISystem->sKeyTimer);
    pUISystem->nMessageStage=MESSAGESTAGE_RUN;//����Ϣ״̬���óɡ��������С�״̬
	return 1;
}

/**
 * @brief MenuMessageBox_MsgHandle
 * ����Ϣ���������£���ӦĳЩ�����Ĵ�����
 * @param pMenu
 * ��ǰ�����
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
        case KEY_ENTER://ȷ�ϼ�
			//OK
            if (VALIDFUNCTION(pUISystem->pMessageUser_fn)){//���裺��ɾ���ļ��Ĳ�������pUISystem->pMessageUser_fn =  Delete_Callback()����
				val=pUISystem->pMessageUser_fn(pUISystem->pMessageUser);
				if (val>=0){
					pUISystem->nMessageStage=MESSAGESTAGE_OK;
				}else{
					pUISystem->nMessageStage=MESSAGESTAGE_FAIL;
				}
				if ((pUISystem->pMessageOK)&&(pUISystem->pMessageFail)){//Display the notify 
					if (pUISystem->nMessageTimeout>0){
                        //������Ϣ��������ʾʱ��Ϊ(pUISystem->nMessageTimeout)��
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
 * ��Ϣ�����ĳ�ʼ����ʹ�ܶ�ʱ��
 * ��ʼ������Ϣ������ʱ��Ļص�������MenuMessageBox_KeyTimer_TO()
 * @param pMenu
 * @return
 */
int MenuMessageBox_Open(UI_Menu_t *pMenu){//Over write default close timer
    //FUNC_ENTER
    MenuRefresh();//���Ƶ������棬ͨ������MenuMessageBox_Refresh()ʵ�ֻ���
	if (pUISystem->nMessageTimeout>0){
		EnableTimer(&(pUISystem->sKeyTimer),1,TIMER_MESSAGEBOX,pUISystem->nMessageTimeout,MenuMessageBox_KeyTimer_TO);
	}
	return 1;
}

/**
 * @brief MenuMessageBox_Close
 * �ر���Ϣ�������Ա���pUISystem->nMessageStage�ָ�Ĭ��ֵ���رն�ʱ��
 * @param pMenu
 * ��ǰ�����
 * @return
 */
int MenuMessageBox_Close(UI_Menu_t *pMenu){
	pUISystem->nMessageStage=MESSAGESTAGE_IDLE;
	DisableTimer(&pUISystem->sKeyTimer);
	return 1;
}

/**
 * @brief MenuMessageBox_Refresh
 * ��Ϣ�����Ľ���ˢ�º���
 * @param pMenu
 * ��ǰ�����
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
	//	Purpose	:	���Ʊ���
	//--------------------------------------------------------------------------//
	GUI_DrawBitmap(pBmp_BG, 
							AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize), 
							AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize));
	GUI_DrawBitmap(pBmp_SubBG, 
							AUTO_GET_X0_FROM_WIDTH(pBmp_SubBG->XSize), 
							(AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_ALL_HEIGHT + BUTTON_V_INTERVAL_COMMON));

	//--------------------------------------------------------------------------//
	//	Purpose	:	��дTitle
	//--------------------------------------------------------------------------//
    //FUNC_ENTER
    GUI_SetTextMode(GUI_TM_TRANS);
    //zqh 2015��3��30�� modify��������ϵͳ�����˵�ʱ����Title�ġ���ʾ��Ϣ����Ϊ������汾��
    if(memcmp(pUISystem->pMessageDisplay, "Ver:", 4) == 0)//zqh add
        GUI_DispStringHCenterAt(SoftwareVersion[pUISystem->allData.Language],
                            (LCD_WIDTH / 2),
                            (AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_BELOW_HEIGHT));

    else
        //zqh 2015��4��15�� add������ʾ��ϵͳ��Ϣ���˵���title��������ʾ��Ϣ����Ϊ��ϵͳ��Ϣ��
        if(memcmp(pUISystem->pMessageDisplay, "Linux", 5) == 0)//zqh add
            GUI_DispStringHCenterAt(SystemInfo[pUISystem->allData.Language],
                                (LCD_WIDTH / 2),
                                (AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_BELOW_HEIGHT));
    else
        GUI_DispStringHCenterAt(Message[pUISystem->allData.Language],
							(LCD_WIDTH / 2), 
							(AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_BELOW_HEIGHT));

	//--------------------------------------------------------------------------//
	//	Purpose	:	��д��ʾ����
	//	Purpose	:	�����·���ʾ��Ϣ
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
			//zqh 2015��3��27��modify����HelpTip2��ΪHelpTip7
            MessageHelp=HelpTip7[pUISystem->allData.Language];
			break;
		case MESSAGESTAGE_FAIL:	
			MessageDisp=pUISystem->pMessageFail;	
			//zqh 2015��3��27��modify����HelpTip2��ΪHelpTip7
            MessageHelp=HelpTip7[pUISystem->allData.Language];
			break;
		case MESSAGESTAGE_RUN:
		default:
            //FUNC_ENTER
			MessageDisp=pUISystem->pMessageDisplay;	
			if (VALIDFUNCTION(pUISystem->pMessageUser_fn)){
                //FUNC_ENTER
				//zqh 2015��3��27��modify����HelpTip1��ΪHelpTip8
				MessageHelp=HelpTip8[pUISystem->allData.Language];
			}else{
                //FUNC_ENTER
                //zqh 2015��3��27��modify����HelpTip2��ΪHelpTip7,
                //������ϵͳ�����˵�ʱ,��ʾ��ͬ�İ�����Ϣ,��ʾ������ϢΪ��"�����������ϵͳ�����˵�"
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

    //zqh 2015��5��25�� add������ʾ��Ϣ�����嶼��С��
    if(pUISystem->allData.Language == ENGLISH)
        GUI_SetFont(&GUI_Font8x16);

	GUI_DispStringAt(MessageHelp, 
					AUTO_GET_X0_FROM_WIDTH(pBmp_SubBG->XSize), 
					(AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + pBmp_BG->YSize - GUI_Font20_SongTi_Unicode.YSize - PROCESS_V_OFFSET_BG_BOTTON));

    //temp--�趨ˢ������
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
 * �޸�ɫ�ȡ��Աȶȡ����ȵĵ��ڡ�
 * @param pData
 * �ϴε���ֵ
 * @param nOffset
 * ����ֵ��һ��Ϊ1����-1
 * @param nRange
 * ɫ�ȡ����ȡ��Աȶȵķ�Χ
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
 * ��ϵͳ�����ȡ�ɫ�ȡ��Աȶȡ������������趨���������ʾ
 * @param pMenu
 * ��ǰ����
 * @param nOffset
 * ƫ������һ��Ϊ1��-1
 * @return
 */

int ChangeCodecSetting(UI_Menu_t *pMenu,int nOffset)
{
	switch(pMenu->nButtonIdx)
	{
    case MenuCodecSetting_Button_ID_CodecLevel://�����������趨
		ChangeCodecSettingContent(&pUISystem->allData.codec_info.codec_level,
			nOffset,CODECLEVEL_RANGE);
		break;
    case MenuCodecSetting_Button_ID_Brightness://���ȵ��趨
		ChangeCodecSettingContent(&pUISystem->allData.codec_info.brightness,
			nOffset,BRIGHTNESS_RANGE);
		break;
    case MenuCodecSetting_Button_ID_Chroma://ɫ�ȵ��趨
		ChangeCodecSettingContent(&pUISystem->allData.codec_info.chroma,
			nOffset,CHROMA_RANGE);
		break;
    case MenuCodecSetting_Button_ID_Contrast://�Աȶȵ��趨
		ChangeCodecSettingContent(&pUISystem->allData.codec_info.contrast,
			nOffset,CONTRAST_RANGE);
		break;
	default://never should happen
		break;
	}

	if (VALIDFUNCTION(PACTIVE_MENU->menuRefreshItem)){
        if (MenuRefreshItemOnly(PACTIVE_MENU->nButtonIdx)>0){//ˢ����ʾ����
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
 * ���ȡ��Աȶȡ��ҶȵȽ���İ�����Ϣ������
 * @param pMenu
 * ��ǰ�����
 * @param nMsgType
 * ��Ϣ����
 * @param nMsgData
 * ��Ϣ����
 * @return
 */
TMsgHandleStat MenuCodecSetting_MsgHandle(UI_Menu_t *pMenu,unsigned short nMsgType,unsigned short nMsgData)
{
	TMsgHandleStat retval=MsgHandled;

	if (nMsgType!=MSG_KEY_EVENT){
		return MsgNocare;
	}
	switch(nMsgData){
        case KEY_UP://�������
			MenuChangeFocus(-1);
			break;
        case KEY_DOWN://�������
			MenuChangeFocus(1);
			break;
		case KEY_LEFT:
        case KEY_RIGHT://���ڻҶȡ����ȡ��Աȶȡ���������ֵ�ı仯
			ChangeCodecSetting(pMenu,(nMsgData == KEY_LEFT?-1:1));
			break;
//		case KEY_MENU:
//			SaveAllDataFlash();
//			SystemChangeWindow(MENUBLANK, 1, 1,0);
//			break;
		case KEY_ESC:
            SaveAllDataFlash();//���Աȶȡ����ȡ��Ҷȡ������������ݱ�����/opt/.pvr_config.cfg�ļ���
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
 * �������ô��ڵĳ�ʼ�������磺������ʾʱ������λ�á�
 * @param pMenu
 * ��ǰ�����
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
 * ���Ʊ���������棬��������ʾ�ַ�����Աȶȡ����ȡ��Ҷȵȡ�
 * @param pMenu
 * ��ǰ�����
 * @param nIdx
 * ѡ�е�buttonֵ
 * @param nFocus
 * ����ѡ��button
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

	//��Button
	if(nIdx<BUTTON_CNT_CODECSETTING_LEFT)
	{
		GUI_DrawBitmap(pBmp_Left, bmp_x0, bmp_y0);
		//дContent
		GUI_DispStringHCenterAt(CodecSetLeftList[nIdx], 
							(bmp_x0 + pBmp_UnSel->XSize / 2),
							(bmp_y0 + BUTTON_V_OFFSET_DISPLAY));

	}else{
		if(nFocus){	//���Ƹ���ѡ����Ŀ
			GUI_DrawBitmap(pBmp_Sel, bmp_x0, bmp_y0);
		}else{			//���ƷǸ�����Ŀ
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
//	Purpose		:	���ƴ���ѡ�����
//	Parameter	:	@Index - ��ѡ�е���Ŀ������0 ~ (BUTTON_CNT_STORAGEINFO - 1)
//--------------------------------------------------------------------------//
//
void Pvr_CodecSetting_SetFocus(INT8S Index)
{
	INT8U i;
	GUI_BITMAP *pBmp_BG = &bmMainMenuBG_360_320;
	
	//��������
	Index = (Index > (BUTTON_CNT_CODECSETTING- 1)) ? 0 : Index;

	//���Ʊ���
	GUI_DrawBitmap(pBmp_BG, 
						AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize), 
						AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize));

	//��дTitle:FileBrowser
	GUI_SetTextMode(GUI_TM_TRANS);
	GUI_DispStringHCenterAt(CodecSetting[pUISystem->allData.Language], 
							(LCD_WIDTH / 2), 
							(AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_BELOW_HEIGHT));

	//����Button(��BUTTON_CNT_STORAGEINFO��,����Ϊ0 ~ (BUTTON_CNT_STORAGEINFO - 1))
	for(i = 0; i < BUTTON_CNT_CODECSETTING; i++)
	{
		MenuRefreshItemOnly(i);
	}

	//�趨ˢ������
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
 * ������������ˢ��
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
 * ������pUISystem->SysState.CurState�������ó�MESSAGESTAGE_IDLE��
 * ������Ϊϵͳ����ʱcf��δ���룬���ô����л�����.
 * ���ô˺����Ƿ�������������£�Ӧ�ó�������֮ǰ��CF�������ڰ����ϣ�����Ӧ�ó���������cf�����γ�������ô˺���
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

    if(stat == MESSAGESTAGE_CFCARDREMOVE)//ϵͳ����ʱ��cf��δ���룬�����if���
    {
        LOGT("change to blank\r\n");
        SystemChangeWindow(MENUBLANK,1,1,0);//�л���blank����
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
 * �豸״̬��Ϣ��ʼ����pUISystem->nDevCurStatusMsg��������ĳ�ʼֵΪMESSAGESTAGE_IDLE��
 * ���������void Pvr_UI_Start(void)�н��еĳ�ʼ����
 * @param pMenu
 * ��ǰ�����
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
 * �豸״̬��Ϣ�رգ���Ҫ���ܾ��ǹرն�ʱ��
 * @param pMenu
 * ��ǰ�����
 * @return
 */
int MenuDev_Status_MSG_Close(UI_Menu_t *pMenu)
{
	FUNC_ENTER
    DisableTimer(&pUISystem->sKeyTimer);//�رն�ʱ��
	return 1;
}

/**
 * @brief MenuDev_Status_MSG_Open
 * ���豸״̬��Ϣ
 * @param pMenu
 * ��ǰ�����
 * @return
 */

int MenuDev_Status_MSG_Open(UI_Menu_t *pMenu)
{
	int msg;

	FUNC_ENTER

    if(pUISystem->nDevCurStatusMsg == MESSAGESTAGE_IDLE)//���豸��Ϣ������ֱ�ӷ���
	{
		ERR("%s:no msg\n",__func__);
		return 0;
	}
	msg = pUISystem->nDevCurStatusMsg;

    //Init Timer��������Ϣ����������º���������ʼ��
	EnableTimer(&pUISystem->sKeyTimer,1,TIMER_MESSAGEBOX,DEV_MESSAGE_DELAY,MenuDev_Status_MSG_TO);

	//Set Current msg

	MenuRefresh();

	return 1;
}

/**
 * @brief MenuDev_Status_MSG_MsgHandle
 * �����豸״̬�ĵ��ú�������cf���Ƴ�����cf��û�пռ��ˡ��˺�������void Pvr_UI_Start(void)��������б����ã���cf�����Ƴ�������µ��á�
 * @param pMenu
 * ��ǰ�����
 * @param nMsgType
 * ��Ϣ����
 * @param nMsgData
 * ����
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
    DisableTimer(&pUISystem->sKeyTimer);//���ö�ʱ��

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
 * �豸״̬ˢ�º�������ʾ��Ϣ����cf����sd����u���Ƴ�
 * @param pMenu
 * ��ǰ�����
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
	//	Purpose	:	���Ʊ���
	//--------------------------------------------------------------------------//
	GUI_DrawBitmap(pBmp_BG, 
							AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize), 
							AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize));
	GUI_DrawBitmap(pBmp_SubBG, 
							AUTO_GET_X0_FROM_WIDTH(pBmp_SubBG->XSize), 
							(AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_ALL_HEIGHT + BUTTON_V_INTERVAL_COMMON));

	//--------------------------------------------------------------------------//
	//	Purpose	:	��дTitle
	//--------------------------------------------------------------------------//
	GUI_SetTextMode(GUI_TM_TRANS);
	GUI_DispStringHCenterAt(Message[pUISystem->allData.Language], 
							(LCD_WIDTH / 2), 
							(AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_BELOW_HEIGHT));

	//--------------------------------------------------------------------------//
	//	Purpose	:	��д��ʾ����
	//	Purpose	:	�����·���ʾ��Ϣ
	//--------------------------------------------------------------------------//

	Rect.x0 = AUTO_GET_X0_FROM_WIDTH(pBmp_SubBG->XSize);
	Rect.y0 = AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize);
	Rect.x1 = AUTO_GET_X0_FROM_WIDTH(pBmp_SubBG->XSize) + pBmp_SubBG->XSize;
	Rect.y1 = AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + pBmp_BG->YSize;



	switch(pUISystem->nDevCurStatusMsg)
	{
	case MESSAGESTAGE_UDISKREMOVE:
		MessageDisp = UDiskRemoved[pUISystem->allData.Language];
        //zqh 2015��3��27�� modify��������������˳�����Ϊ�����˳����˳���
        MessageHelp = HelpTip7[pUISystem->allData.Language];
		break;
	case MESSAGESTAGE_UDISKDETECT:
		MessageDisp = UDiskDetected[pUISystem->allData.Language];
        //zqh 2015��3��27�� modify��������������˳�����Ϊ�����˳����˳���
        MessageHelp = HelpTip7[pUISystem->allData.Language];
		break;
	case MESSAGESTAGE_DISKFULL:
		MessageDisp = CFCardFull[pUISystem->allData.Language];
        //zqh 2015��3��27�� modify��������������˳�����Ϊ�����˳����˳���
        MessageHelp = HelpTip7[pUISystem->allData.Language];
		break;
	case MESSAGESTAGE_SDREMOVE:
		MessageDisp = SDRemoved[pUISystem->allData.Language];
		MessageHelp = HelpSDRemove[pUISystem->allData.Language];
		break;
	case MESSAGESTAGE_SDDETECT:
		MessageDisp = SDDetected[pUISystem->allData.Language];
        //zqh 2015��3��27�� modify��������������˳�����Ϊ�����˳����˳���
        MessageHelp = HelpTip7[pUISystem->allData.Language];//
		break;
	case MESSAGESTAGE_CFCARDREMOVE:
		MessageDisp = CFRemoved[pUISystem->allData.Language];
        //zqh 2015��3��27�� modify��������������˳�����Ϊ�����˳����˳���
        MessageHelp = HelpTip7[pUISystem->allData.Language];
		break;
	case MESSAGESTAGE_CFCARDDETECT:
		MessageDisp = CFDetected[pUISystem->allData.Language];
        //zqh 2015��3��27�� modify��������������˳�����Ϊ�����˳����˳���
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


	//temp--�趨ˢ������
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
 * ϵͳ��Ҫ����ʱ�����ô˺���
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
 * ϵͳ������ɺ󣬵��ô˺������������л����ļ�������ڣ��Լ���ʾ���壺��ϵͳ��������
 * @param ignored
 * ���Դ˲���
 * @return
 */
int Update_Reboot(int ignored)
{
	FUNC_ENTER;
    SystemChangeWindow(MENUFILEBROWSER,1,1,0);//�л����ļ��������
    //��ʾ��Ϣ���壺��ϵͳ��������������int Do_Reboot(int ignored)�����������ϵͳ��������
    //zqh 2015��4��13�� modify������ʾ���ȴ�ϵͳ�������˵�����ʾʱ����10S(DEV_MESSAGE_DELAY)��Ϊ3S(SYSTEMTOREBOOT_MESSAGE_DELAY)��
    MessageBox(SystemReboot[pUISystem->allData.Language],SYSTEMTOREBOOT_MESSAGE_DELAY,NULL,NULL,NULL,NULL,NULL,Do_Reboot);
	return 1;
}

/**
 * @brief StartCopyFile
 * �����ļ�����
 * @param src_file
 * Դ�ļ�
 * @param dst_file
 * Ŀ���ļ�
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

    SystemChangeWindow(MENUDOFILECOPY,1,1,1);//�л����ļ���������
	return 0;
}

/**
 * @brief SetUpdateFileName
 * �ϳ�Դ�ļ���Ŀ���ļ���ȫ·�������磺/media/cf/daemon
 * @param src_path
 * Դ·�������磺/media/cf
 * @param dst_path
 * Ŀ��·�������磺/media/usb
 * @param filename
 * Ҫ�������ļ����֣����磺daemon
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
 * �����U��������pvr��uImage��daemon�������ļ�
 * @param filename
 * ����ֵΪpvr��daemon��uImage
 * @return
 */
int CheckUpdateFile(char * filename)
{
	FILE * fp;
	char tmp_src[MAX_FS_PATH];
	int pathlen;

	FUNC_ENTER;

	memset(tmp_src,0,sizeof(tmp_src));
    pathlen = strlen(UPDATE_SRC_FILE_PATH);//UPDATE_SRC_FILE_PATH������Ϊ/media/usb
	sprintf(tmp_src,UPDATE_SRC_FILE_PATH);
    if(tmp_src[pathlen-1]!='/')//UPDATE_SRC_FILE_PATH�����ַ��������һ���ַ�����'/'��'/'�����UPDATE_SRC_FILE_PATH�ַ�����
	{
		strcat(tmp_src,"/");
	}
    strcat(tmp_src,filename);//�ϳ�Ҫ���ļ���ȫ·�������磺/media/usb/pvr

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
 * �����ں�
 * @param pUser
 * �ں�����uImage
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
 * �ں˸�����󣬵��ô˺������з���ʼ��
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


/** zqh 2015��5��20�� add
 * @brief SetLanguage_Deinit
 * ��ϵͳ����������ɺ�ϵͳ�����л�menublank״̬��
 * @param nOKFail
 * @return
 */
int SetLanguage_Deinit(int nOKFail)
{
    SystemChangeWindow(MENULANGUAGESEL,1,1,0);//�����л�
    return 1;
}


//zqh 2015��4��1�� add����ӡ����ԡ��˵��İ�����Ϣ������
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
 * ϵͳ���³�ʼ���������趨����ĳ�ʼѡ��button
 * @param pMenu
 * ��ǰ�����
 * @return
 */
int MenuLanguageSel_Init(UI_Menu_t *pMenu)
{
    MenuButtonMaskBitmap[pMenu->nMenuIdx] = 1<<BUTTON_CNT_LANGUAGE;
    pMenu->nButtonIdx = pUISystem->allData.Language;
    return 1;
}

/**zqh 2015��4��1�� add
 * @brief MenuLanguageSel_Refresh_Item
 * ���ơ����ԡ��˵��� ����button�����磺�������ġ����w���ġ�English
 * @param pMenu
 * ��ǰ�����
 * @param nIdx
 * ��ǰ���ڵ�buttonֵ��ȡֵ��Χ��0-2.
 * @param nFocus
 * �Ƿ񽫵�ǰbutton���óɸ���״̬
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

    //��Button
    if(nFocus){	//���Ƹ���ѡ����Ŀ
        GUI_DrawBitmap(pBmp_Sel, bmp_x0, bmp_y0);
    }else{			//���ƷǸ�����Ŀ
        GUI_DrawBitmap(pBmp_UnSel, bmp_x0, bmp_y0);
    }

    if((1<<nIdx)&MenuButtonMaskBitmap[pUISystem->nMenuIdx])
    {	//������Ҫ��������Ŀ
        GUI_SetColor(GUI_GRAY);

        //дContent
        GUI_DispStringHCenterAt(LanguageList[nIdx],
                                (bmp_x0 + pBmp_UnSel->XSize / 2),
                                (bmp_y0 + BUTTON_V_OFFSET_DISPLAY));
        GUI_SetColor(GUI_WHITE);

    }
    else
    {
        //дContent
        GUI_DispStringHCenterAt(LanguageList[nIdx],
                                (bmp_x0 + pBmp_UnSel->XSize / 2),
                                (bmp_y0 + BUTTON_V_OFFSET_DISPLAY));
    }

    //NOTE: Do not SetRefreshArea should notify
    return 1;

}

/** zqh 2015��4��1�� add
 * @brief MenuLanguageSel_Refresh
 * ˢ��ϵͳ�������溯����ʵ�����ǵ���MenuLanguageSel_Refresh_Item()�������ʵ�ֵģ�
 * �˺���ֻ���Ʊ���ͼƬ�����б��⣬������ʾ������Ϣ��
 * @param pMenu
 * ��ǰ�����
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

    //���Ʊ���
    pBmp_BG = (&bmMainMenuBG_360_320);
    GUI_DrawBitmap(pBmp_BG,
                            AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize),
                            AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize));

    //��дTitle "����"
    GUI_SetTextMode(GUI_TM_TRANS);
    GUI_DispStringHCenterAt(Language[pUISystem->allData.Language],
                            (LCD_WIDTH / 2),
                            (AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_BELOW_HEIGHT));

    //����Button
    for(i = 0; i < BUTTON_CNT_LANGUAGE; i++)
    {
        MenuRefreshItemOnly(i);//���ô˺���MenuLanguageSel_Refresh_Item()����ʵ��button�Ļ���
    }

    //--------------------------------------------------------------------------//
    //	Purpose	:	��д��ʾ����
    //	Purpose	:	�����·���ʾ��Ϣ"����ȷ�ϡ����O��ϵ�y�Z��"
    //--------------------------------------------------------------------------//
    MessageHelp=HelpTip10[pUISystem->allData.Language];

    if(pUISystem->allData.Language == ENGLISH)
        GUI_SetFont(&GUI_Font8x16);

    GUI_DispStringHCenterAt(MessageHelp,
                    AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize) + pBmp_BG->XSize/2,
                    (AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + pBmp_BG->YSize - GUI_Font20_SongTi_Unicode.YSize - PROCESS_V_OFFSET_BG_BOTTON));

    //�趨ˢ������
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
 * ʲô������
 * @param pMenu
 * @return
 */

int MenuSystemUpdate_Close(UI_Menu_t *pMenu)
{
	return 0;
}

/**
 * @brief MenuSystemUpdate_Open
 * ����ˢ��
 * @param pMenu
 * ��ǰ�����
 * @return
 */
int MenuSystemUpdate_Open(UI_Menu_t *pMenu)
{
    //! refresh menu only
    MenuRefresh();//����ˢ��
    return 0;
}
#if   UPDATELOADMODULESSCRIPT
//2014-11-17��� :�˺������� ����/optĿ¼�µ�loadmodule-ef.sh�ű��ļ�
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
 * ϵͳ���½��棬������Ϣ���������Լ������ļ�ϵͳ���ںˡ���ʾϵͳ�汾��
 * @param pMenu
 * ��ǰ�����
 * @param nMsgType
 * ��Ϣ����
 * @param nMsgData
 * ��Ϣ����
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
        case KEY_UP://����
			MenuChangeFocus(-1);
			break;
        case KEY_DOWN://����
			MenuChangeFocus(1);
			break;
        case KEY_ESC://�˳�
			SystemChangeWindow(MENUFILEBROWSER,1,1,0);
            CLEARBIT(pUISystem->SysState.CurState, SYS_BIT_SYSTEMUPDATE);//����ϵͳ����״̬λ
            SETBIT(pUISystem->SysState.CurState, SYS_BIT_FILEBROSWER);
			break;
		case KEY_RIGHT:
		case KEY_ENTER:
			switch(pMenu->nButtonIdx)
			{
            case MenuSystemUpdate_Button_ID_SysInfo://ϵͳ��Ϣ
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
                        testbuff.machine);//����ϵͳ��Ϣ����
					MessageBox(strbuff,DEV_MESSAGE_DELAY,
						NULL,NULL,NULL,
						NULL,NULL,NULL);
				}
				break;
            case MenuSystemUpdate_Button_ID_Daemon://����daemon����
				ret = DoUDiskDetect();
				if((ret<0)||(!TESTBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_Mount_Flag)))
				{
					ERR("Need Mount UDisk first!\r\n");
					CLEARBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_Mount_Flag);
					MessageBox(HelpMountUDisk[pUISystem->allData.Language],DEV_MESSAGE_DELAY,
                        NULL,NULL,NULL,NULL,NULL,NULL);//���������Ȳ���u�̡���Ϣ����
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
                    ret = CheckUpdateFile(UPDATE_DAEMON_NAME);//���u�����Ƿ���daemon�ļ�
					if(ret>=0)
					{
						SetUpdateFileName(UPDATE_SRC_FILE_PATH,UPDATE_DST_FILE_PATH,UPDATE_DAEMON_NAME);
						pUISystem->nFileCopyToStage = MESSAGESTAGE_UPDATE_BACKUP;
						// Set Update in progress
//						Update_In_Progress = 1;
                        // remount rw fs.�����ļ�ϵͳ����Ϊ�ɶ�д�ġ�
                        ret = mount(RFS_MOUNT_NOD,RFS_MOUNT_DST,RFS_MOUNT_TYPE,MS_REMOUNT,0);
						if(ret<0)
						{
							ERR("remount RFS to rw err:%s\r\n",strerror(errno));
//							break;
						}
						update_reboot_code = EXIT_CODE_REBOOT;
                        StartCopyFile(Update_DestFile,Update_BackUpFile);//��ʼ�����ļ�
					}
					else
					{
                        ERR("%s:No update file!\n",__func__);//
						MessageBox(NoUpdateFileDaemon[pUISystem->allData.Language],DEV_MESSAGE_DELAY,
							NULL,NULL,NULL,
                            NULL,NULL,NULL);//������U������daemon����Ϣ����
					}
				}
				break;
            case MenuSystemUpdate_Button_ID_FileSys://�����ļ�ϵͳ
				ret = DoUDiskDetect();
				if((ret<0)||(!TESTBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_Mount_Flag)))
				{
					ERR("Need Mount UDisk first!\r\n");
					CLEARBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_Mount_Flag);
					MessageBox(HelpMountUDisk[pUISystem->allData.Language],DEV_MESSAGE_DELAY,
                        NULL,NULL,NULL,NULL,NULL,NULL);//���������Ȳ���u�̡���Ϣ����
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
                    ret = CheckUpdateFile(UPDATE_FILE_NAME);//���U�����Ƿ�����pvr�������ļ�
					if(ret>=0)
					{
						SetUpdateFileName(UPDATE_SRC_FILE_PATH,UPDATE_DST_FILE_PATH,UPDATE_FILE_NAME);
						pUISystem->nFileCopyToStage = MESSAGESTAGE_UPDATE_BACKUP;
                        //�����ļ�ϵͳ����Ϊ�ɶ�д�ģ�Ĭ���ļ�ϵͳΪֻ����
						ret = mount(RFS_MOUNT_NOD,RFS_MOUNT_DST,RFS_MOUNT_TYPE,MS_REMOUNT,0);
						if(ret<0)
						{
							ERR("remount RFS to rw err:%s\r\n",strerror(errno));
//							break;
						}
						update_reboot_code = EXIT_CODE_UPDATE;
                        // set update in progress
						SetUpdateInProgress(1);
                        StartCopyFile(Update_DestFile,Update_BackUpFile);//��ʼ�����ļ�
					}
					else
					{
						ERR("%s:No update file!\n",__func__);
						MessageBox(NoUpdateFilePvr[pUISystem->allData.Language],DEV_MESSAGE_DELAY,
							NULL,NULL,NULL,
                            NULL,NULL,NULL);//��������pvr�ļ�������
					}
				}
				break;
            case MenuSystemUpdate_Button_ID_Kernel://�����ں�
				ret = DoUDiskDetect();
				if((ret<0)||(!TESTBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_Mount_Flag)))
				{
					ERR("Need Mount UDisk first!\r\n");
					CLEARBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_Mount_Flag);
					MessageBox(HelpMountUDisk[pUISystem->allData.Language],DEV_MESSAGE_DELAY,
                        NULL,NULL,NULL,NULL,NULL,NULL);//���������Ȳ���u�̡���Ϣ����
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
				// ���������ں�ȷ����Ϣ
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
 * ϵͳ���³�ʼ���������趨����ĳ�ʼѡ��button
 * @param pMenu
 * ��ǰ�����
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
 * ����ϵͳ�������棬���磺����button����u-boot���óɻ�ɫ
 * @param pMenu
 * ��ǰ�����
 * @param nIdx
 * ��ǰ���ڵ�buttonֵ��ȡֵ��Χ��0-5.
 * @param nFocus
 * �Ƿ񽫵�ǰbutton���óɸ���״̬
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

	//��Button
	if((1<<nIdx)&MenuButtonMaskBitmap[pUISystem->nMenuIdx]){	//������Ҫ��������Ŀ
			GUI_DrawBitmap(pBmp_Invalid, bmp_x0, bmp_y0);
	}else{
		if(nFocus){	//���Ƹ���ѡ����Ŀ
			GUI_DrawBitmap(pBmp_Sel, bmp_x0, bmp_y0);
		}else{			//���ƷǸ�����Ŀ
			GUI_DrawBitmap(pBmp_UnSel, bmp_x0, bmp_y0);
		}
	}

	if((1<<nIdx)&MenuButtonMaskBitmap[pUISystem->nMenuIdx])
	{
		//������Ҫ��������Ŀ
        GUI_SetColor(GUI_GRAY);//��button��ɫ���óɻ�ɫ�����磺����u-boot button

		//дContent
		GUI_DispStringHCenterAt(SystemUpdateList[nIdx], 
								(bmp_x0 + pBmp_UnSel->XSize / 2),
								(bmp_y0 + BUTTON_V_OFFSET_DISPLAY));
		GUI_SetColor(GUI_WHITE);

	}
	else
	{
		//дContent
		GUI_DispStringHCenterAt(SystemUpdateList[nIdx], 
								(bmp_x0 + pBmp_UnSel->XSize / 2),
								(bmp_y0 + BUTTON_V_OFFSET_DISPLAY));
	}

    //NOTE: Do not SetRefreshArea should notify
	return 1;

}

/**
 * @brief MenuSystemUpdate_Refresh
 * ˢ��ϵͳ�������溯����ʵ�����ǵ���MenuSystemUpdate_Refresh_Item()�������ʵ�ֵģ�
 * �˺���ֻ���Ʊ���ͼƬ�����б��⡣
 * @param pMenu
 * ��ǰ�����
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

	//��������
	Index = pMenu->nButtonIdx;
	Index = (Index > (BUTTON_CNT_SYSTEMUPDATA- 1)) ? 0 : Index;

	//���Ʊ���
	pBmp_BG = (&bmMainMenuBG_360_320);
	GUI_DrawBitmap(pBmp_BG, 
							AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize), 
							AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize));

	//��дTitle
	GUI_SetTextMode(GUI_TM_TRANS);
	GUI_DispStringHCenterAt(SystemUpdate[pUISystem->allData.Language], 
							(LCD_WIDTH / 2), 
							(AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_BELOW_HEIGHT));

	//����Button	
	for(i = 0; i < BUTTON_CNT_SYSTEMUPDATA; i++)
	{
        MenuRefreshItemOnly(i);//���ô˺���MenuSystemUpdate_Refresh_Item()����ʵ��button�Ļ���
	}

	//�趨ˢ������
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
 * ��ʽ��cf����Ļص��������������л���ϵͳ���ܴ���
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
 * ���ϵͳ��log��־����cf�����log�ļ�������u����
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
    sprintf(full_path,"%s%s",LOG_PATH_CFCARD,LOG_FILE_LOG);//�ϳ�log����λ�õ�ȫ·��������/media/cf/pvr_log.log
	fp = fopen(full_path,"r");
    if(fp != NULL)//�ж��Ƿ���pvr_log.log����ļ�
	{
		fclose(fp);
		START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_MOVE_LOG);

        sprintf(buf,"cp %s %s",full_path,pUISystem->Vol_Disk[DISK_USB]);//�ϳ�����ַ���:cp /media/cf/pvr_log.log /media/usb
		LOGT("%s\r\n",buf);
        system(buf);//��pvr_log.log�ļ���cf��������������

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
 * ��ʽ��cf��Ļص���������Ҫ�������л�����ֵ�ļ��������
 * @param nOKFail
 * @return
 */
int FormatCFCard_Deinit(int nOKFail)
{
    SystemChangeWindow(MENUFILEBROWSER,1,1,0);//�����л�
	return 1;
}


/** zqh 2015��5��20�� add
 * @brief CFCardFsck_Deinit
 * cf�ļ�ϵͳ�����Ϻ���õĻص���������Ҫ�������л����ڵ�ϵͳ���ܴ���
 * @param nOKFail
 * @return
 */
int CFCardFsck_Deinit(int nOKFail)
{
    SystemChangeWindow(MENUSYSTEMFUNC,1,1,0);//�����л�
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
 * ��ʽ��cf��,����־�ļ����浽/tmpĿ¼��
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

    ret = SystemGetMountDevPath(tmp_path,DISK_CF);//���CF���Ƿ����
	if(ret<0)
		return -1;

	if(!TESTBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_REMOVE))
	{
        // close cf card log file , redirect log to tmp file
        // TODO: need test log in tmpfs
		close_log();

        //����move_log�����ǽ�CF���ڵ�log�ļ�������/tmpĿ¼��
		move_log(LOG_PATH_CFCARD,LOG_PATH_TMP,LOG_FILE_LOG);
		move_log(LOG_PATH_CFCARD,LOG_PATH_TMP,LOG_FILE_LOG_OLD);
		move_log(LOG_PATH_CFCARD,LOG_PATH_TMP,LOG_FILE_STARTUP);
		move_log(LOG_PATH_CFCARD,LOG_PATH_TMP,LOG_FILE_FSCK);

		move_log(LOG_PATH_CFCARD,LOG_PATH_TMP,LOG_FILE_KERNEL_START);
		move_log(LOG_PATH_CFCARD,LOG_PATH_TMP,LOG_FILE_KERNEL_LAST);

		redirect_log(LOG_PATH_TMP,LOG_FILE_FMT);

        SETBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_FORMAT);
        ret = SystemUmountDev(pUISystem->Vol_Disk[DISK_CF]);//ж��CF��
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
    sprintf(buf,"/sbin/mkfs.ext3 %s",tmp_path);//�ϳ�/sbin/mkfs.ext3 /dev/sda1����ַ���������cf����dev������sda1����豸
#else
	sprintf(buf,"/usr/sbin/mkfs.vfat %s",tmp_path);
#endif
    LOGT("system %s\r\n",buf);
    ret = system(buf);//ִ�и�ʽ��cf������
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
    get_kernel_msg(buf);//����ں���Ϣ������/tmp/pvr_fmt.log

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
 * CF���ļ�ϵͳ���
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

    ret = SystemGetMountDevPath(tmp_path,DISK_CF);//���cf���Ƿ����
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
    sprintf(buf,"fsck.ext3 %s -yf",tmp_path);//�ϳ��ַ���fsck.ext3 /dev/sda1 -yf������cf����dev������sda1����豸
#else
	sprintf(buf,"fsck.vfat %s -yf",tmp_path);
#endif
	LOGT("%s\r\n",buf);
    ret = system(buf);//����cf���ļ�ϵͳ�ļ��
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
    get_kernel_msg(buf);//���ں���Ϣ���浽/tmp/pvr_fsck.log

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
 * ����ϵͳ���ߣ���U��������ϵͳ��
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

    // ֻ����U�����е������ļ�
	do_update_system_tools = 1;
	for(i=0;i<count;i++)
	{
#if !(MOUNT_CF_EXT) // !(DEBUG_V2_1)
        if(CheckUpdateFile(UpdateToolsFile[i])<0)//���Ҫ���µ��ļ���U�����Ƿ����
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
 * ϵͳ���º󣬷���ʼ������ô˺����������н��洰�ڵ��л�
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
 * ����ϵͳ��־�Ĺ���
 * @param pUser
 * @return
 */
int DoDisableSystemLog(void *pUser)
{
    START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_DELETE_FILE);

    set_log_en(0);//��opt�µ�.pvr_log_en�ļ�ɾ��

    STOP_WATCHDOG_TIMER_SIG();

    update_reboot_code = EXIT_CODE_REBOOT;
	Update_Reboot(1);
    return 1;
}

/**
 * @brief DoEnableSystemLog
 * ʹ��ϵͳ��־����
 * @param pUser
 * @return
 */
int DoEnableSystemLog(void *pUser)
{
    START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_DELETE_FILE);

    set_log_en(1);//��optĿ¼���½����.pvr_log_en�ļ�

    STOP_WATCHDOG_TIMER_SIG();

    update_reboot_code = EXIT_CODE_REBOOT;
    Update_Reboot(1);
    return 1;
}

/**
 * @brief MenuSystemFunc_Init
 * ϵͳ���ܴ��ڵĳ�ʼ��
 * �趨��ʼ��ѡ�е�button
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
 * ϵͳ���ܰ�����Ϣ��������
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
        case KEY_UP://����
			MenuChangeFocus(-1);
			break;
        case KEY_DOWN://����
			MenuChangeFocus(1);
			break;
        case KEY_ENTER://ȷ�ϼ�
			switch(pMenu->nButtonIdx)
			{
                case MenuSystemFunc_Button_ID_CopyToCF://�ļ�������cf�����л������u�̵Ľ���
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
                            NULL,NULL,NULL,NULL,NULL,NULL);//������u��δ���ء�����Ϣ����
						break;
					}

					SystemChangeWindow(MENUUDISKFILELIST,0,1,1);
                    break;
                    // zqh 2015��3��24�� modify��������ȡϵͳ��־���������ε�
                    #if 0
                case MenuSystemFunc_Button_ID_GetLOG://��ȡϵͳ��־
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
                case MenuSystemFunc_Button_ID_FormatCF://��ʽ��CF��
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
                case MenuSystemFunc_Button_ID_FSCK://CF���ļ�ϵͳ���
                    if(CheckMountFlags() < 0)
                    {
                    	retval=MsgNeedDispatch;
                        break;
                    }
                    //  Add fsck msg box zqh 2015��4��20�� modify������ʾ�����������ļ�ϵͳ����Ϊ�����������ļ�ϵͳ���Ƿ��������
                    //zqh 2015��5��20�� modify����cf���ļ�ϵͳ�����Ϻ󣬵��õĺ�����FormatCFCard_Deinit()��ΪCFCardFsck_Deinit()�������
                    MessageBox(MsgBox47[pUISystem->allData.Language],CODEC_MESSAGE_DELAY,DoCFCardFsck,NULL,NULL,
								CFCardFsckOK[pUISystem->allData.Language],CFCardFsckFail[pUISystem->allData.Language],
                                CFCardFsck_Deinit);
					break;
                    //zqh 2015��3��24�� modify������ʹ��/������־���ܡ����ε�
                    #if 0
#if DEBUG_V2_1
                case MenuSystemFunc_Button_ID_LogEnable://ʹ�ܹر�log����
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
        case KEY_ESC://�˳�������Ϣ�������л����ļ��������
            SystemChangeWindow(MENUFILEBROWSER,1,1,0);
            CLEARBIT(pUISystem->SysState.CurState, SYS_BIT_SYSTEMFUNCTION);//����ϵͳ����״̬λ
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
 * ϵͳ���ܽ���Ļ��ƺ������������button����button����ʾ����
 * @param pMenu
 * ��ǰ�����
 * @param nIdx
 * ������������Ƶ�N��button��ȡֵ��Χ��0~4.
 * @param nFocus
 * ���������button�Ƿ����
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

    //zqh 2015��5��25��add��������ϵͳ����ΪӢ��ʱ���������С��
    //printf("pUISystem->allData.Language = %d\n", pUISystem->allData.Language);
    if(pUISystem->allData.Language == ENGLISH)
        GUI_SetFont(&GUI_Font8x16);
    else
        GUI_SetFont(&GUI_Font20_SongTi_Unicode);

    //	GUI_BITMAP *pBmp_SubBG = (&bmMessageRect_310_130);
	//����Button
	pBmp_BG = (&bmMainMenuBG_360_320);
	pBmp_UnSel = (&bmFileList_Flat_GBlue_240_30);
	pBmp_Sel = (&bmFileList_Flat_Orange_240_30);

	tmp_interval = (pBmp_BG->YSize - TITLE_ALL_HEIGHT- pBmp_Sel->YSize * (BUTTON_CNT_SYSTEMFUNC+1)) / (BUTTON_CNT_SYSTEMFUNC+2);
	bmp_FirstList_x0 = AUTO_GET_X0_FROM_WIDTH(pBmp_Sel->XSize);
	bmp_FirstList_y0 = AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize)+TITLE_ALL_HEIGHT + tmp_interval;
	
	bmp_x0 = bmp_FirstList_x0;
	bmp_y0 = bmp_FirstList_y0 + (pBmp_Sel->YSize + tmp_interval) * (nIdx);

	//��Button
	if(nFocus){	//���Ƹ���ѡ����Ŀ
		GUI_DrawBitmap(pBmp_Sel, bmp_x0, bmp_y0);
	}else{			//���ƷǸ�����Ŀ
		GUI_DrawBitmap(pBmp_UnSel, bmp_x0, bmp_y0);
	}

	if((1<<nIdx)&MenuButtonMaskBitmap[pUISystem->nMenuIdx])
	{	//������Ҫ��������Ŀ
		GUI_SetColor(GUI_GRAY);

		//дContent
		GUI_DispStringHCenterAt(SystemFuncList[nIdx], 
								(bmp_x0 + pBmp_UnSel->XSize / 2),
								(bmp_y0 + BUTTON_V_OFFSET_DISPLAY));
		GUI_SetColor(GUI_WHITE);

	}
	else
	{
		//дContent
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
 * ϵͳ���ܽ����ˢ�º������������ñ��������⣬���û���button������
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
	//	Purpose	:	���Ʊ���
	//--------------------------------------------------------------------------//
	GUI_DrawBitmap(pBmp_BG, 
				AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize), 
				AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize));

	//--------------------------------------------------------------------------//
	//	Purpose	:	��дTitle
	//--------------------------------------------------------------------------//
    //zqh 2015��3��30�� modify����title��Ϣ�ġ���ʾ��Ϣ����Ϊ��ϵͳ���ܡ�
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
	//	Purpose	:	��д��ʾ����
	//	Purpose	:	�����·���ʾ��Ϣ
	//--------------------------------------------------------------------------//
	//zqh 2015��3��27��modify����MessageHelp��ֵ��ֵΪ" "
	//MessageHelp=HelpTip1[pUISystem->allData.Language];

    if(pUISystem->allData.Language == ENGLISH)
        GUI_SetFont(&GUI_Font20_SongTi_Unicode);

	MessageHelp = " ";

	GUI_DispStringHCenterAt(MessageHelp, 
					AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize) + pBmp_BG->XSize/2, 
					(AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + pBmp_BG->YSize - GUI_Font20_SongTi_Unicode.YSize - PROCESS_V_OFFSET_BG_BOTTON));
	//temp--�趨ˢ������
	SetRefreshAreaPos(OSDPOS_BASEMENU, 
					AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize), 
					AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize), 
					pBmp_BG->XSize, 
					pBmp_BG->YSize, 
					ALPHA_COMMON,
					TRUE);
	return 1;
}

// ���Ӵ�U�̿�����CF���Ĳ���
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

    StartCopyFile(src_path,dest_path);//�����ļ�

	return 1;
}

/**
 * @brief MenuUDiskFileList_PageChange
 * ���U�����ļ�����ҳ�����Ĵ�����
 * @param pMenu
 * ��ǰ�����
 * @param adjustval
 * ��ǰ/��ҳ��
 * @return
 */
int MenuUDiskFileList_PageChange(UI_Menu_t *pMenu, int adjustval)
{
    if(pUISystem->FileBrowserdirlist_UDisk.nCount <= BUTTON_CNT_UDISKFILELIST)//U���ļ�������ڣ���ҳ��ʾ4���ļ�
		return 0;

	if(adjustval>0)	{	//��ҳ
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
 * U���ļ�������棬ѡ���ļ��������л�
 * @param pMenu
 * ��ǰ�����
 * @param adjustval
 * �����/����
 * @return
 */

int MenuUDiskFileList_SingleChange(UI_Menu_t *pMenu, int adjustval)
{
	int index;		//�ܵ�Index(0---TotalNum-1)

	if(pUISystem->FileBrowserdirlist_UDisk.nCount <= 1) {
		return 0;
	}

	index = pUISystem->FileBrowserIndex_UDisk + pMenu->nButtonIdx;
	
	if(adjustval>0)	{	//��һ��
		if((index + 1) < pUISystem->FileBrowserdirlist_UDisk.nCount) {
			index++;
		} else {
			index = 0;
		}
	} else if(adjustval<0)	{			//��һ��
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
 * ���U���ļ���������\��\��\�ҡ����ְ����ĺ���
 * @param pMenu
 * ��ǰ�����
 * @param nMsgData
 * ����İ�����Ϣ
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
 * �л���U���ļ�������ڵĳ�ʼ�����������磺���������.avi�ļ��ĸ��������ó�ʼ��ѡ�е�button
 * @param pMenu
 * ��ǰ�����
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

    // ��������ΪĬ��λ��
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
 * �ر����U���ļ��Ĳ����������ͷ�FileBrowserdirlist_UDisk�ڴ�
 * @param pMenu
 * ��ǰ�����
 * @return
 */
int MenuUDiskFileList_Close(UI_Menu_t *pMenu)
{
    ReleaseList(&(pUISystem->FileBrowserdirlist_UDisk));//�ͷ��ڴ�
	return 1;
}

/**
 * @brief MenuUDiskFileList_MsgHandle
 * ���U���ļ�ʱ����Ӧ������Ϣ�Ĵ�������
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
        case KEY_DOWN://�ϡ��¡��������ְ���
			MenuUDiskFileList_ChangeFocus(pMenu,nMsgData);
			break;
        case KEY_ENTER://ȷ�ϼ�
            if(CheckMountFlags() < 0)
            {
            	retval=MsgNeedDispatch;
                break;
            }
			// copy file to cfcard
			MessageBox(CopyToCFCard[pUISystem->allData.Language],CODEC_MESSAGE_DELAY,DoCopyToCFCard,NULL,
						NULL,NULL,NULL,NULL);
			break;
        case KEY_ESC://�˳���
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
 * ����U���ļ���������button�����ֵĺ���
 * @param pMenu
 * ��ǰ�����
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
		//--��дPage����1/23
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

	//��Button
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
	//��д�ļ���Ϣ
	//--��дTitle
	GUI_DispStringHCenterAt(dirlist->attribute[nIdx+pUISystem->FileBrowserIndex_UDisk].longname, 
		(bmp_x0 + pBmp_Sel->XSize/2),
		(bmp_y0 + BUTTON_V_OFFSET_DISPLAY));

}

/**
 * @brief MenuUDiskFileList_RefreshPage
 * ����U���ļ����������ʾ���ݵĺ���
 * @param pMenu
 * ��ǰ�����
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

        MenuUDiskFileList_RefreshItem(pMenu,i,nFocus);//����button�����ֵĺ���
	}

	RefreshOSDDisplay();

	return 1;
}

/**
 * @brief MenuUDiskFileList_Refresh
 * ����U���ļ�������ڵĺ������������ã������������
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
	//	Purpose	:	���Ʊ���
	//--------------------------------------------------------------------------//
	GUI_DrawBitmap(pBmp_BG, 
							AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize), 
							AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize));

	//--------------------------------------------------------------------------//
	//	Purpose	:	��дTitle
	//--------------------------------------------------------------------------//
	GUI_SetTextMode(GUI_TM_TRANS);
	GUI_DispStringHCenterAt(UDiskFileList[pUISystem->allData.Language], 
							(LCD_WIDTH / 2), 
							(AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_BELOW_HEIGHT));

	//--------------------------------------------------------------------------//
	//	Purpose	:	��д��ʾ����
	//--------------------------------------------------------------------------//
	MenuUDiskFileList_RefreshPage(pMenu);

	//�趨ˢ������
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
 * �������/ʧ�ܵ���ʾ��Ϣ��ʾ��ɺ�Ĵ���
 * ���ݵ�ǰ�ļ�����״̬,�޸��ļ�����״̬�����ظ�����
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
            //zqh 2015��3��30�� modify�������ļ��ӵ�������U���︴����ɺ�Ӧ�ص����ļ��������Ľ��棬�����Ƿ��ء��ļ�������Ľ���
            //SystemChangeWindow(MENUFILEBROWSER,1,1,0);
            SystemChangeWindow(MENUFILEOPERATION,1,1,0);
            break;
		case MESSAGESTAGE_COPY_TO_CFCARD:
			LOGT("copy stop\r\n");
			pUISystem->nFileCopyToStage = MESSAGESTAGE_IDLE;

    	    sync();
	        sync();
            //zqh 2015��3��30�� modify�����ļ���U�̸��Ƶ���������ɺ�Ӧ�ص���ϵͳ���ܡ��Ľ��棬�����Ƿ��ء��ļ�������Ľ���
            //SystemChangeWindow(MENUFILEBROWSER,1,1,0);
            SystemChangeWindow(MENUSYSTEMFUNC,1,1,0);

            // �ļ�����������fsync����,�˴�����Ҫumount cf��
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
            // ֱ�ӿ����� pvr/daemon
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

			// fixed 0516 �п��ܻ��U�̿�����CF�������аε�U��
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
            //zqh 2015��3��30�� modify�����ӵ�������U���и����ļ�ʱ��ȡ�����ƺ�Ӧ�ص����ļ��������Ľ��棬�����Ƿ��ء��ļ�������Ľ���
            //SystemChangeWindow(MENUFILEBROWSER,1,1,0);
            SystemChangeWindow(MENUFILEOPERATION,1,1,0);
			break;

            // copy file canceled. Return to parent menu and remove file
        case MESSAGESTAGE_CANCEL_COPYTOCFCARD:
            LOGT("copy to cf card cancel\r\n");

            // fixed 0516 �п��ܻ��U�̿�����CF�������аε�U��
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
            //zqh 2015��3��30�� modify������U���������̸����ļ��Ĺ����У�ȡ������ʱ��Ӧ�ص���ϵͳ���ܡ��Ľ��棬�����Ƿ��ء��ļ�������Ľ���
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
 * �����ļ����ڵİ�����Ϣ������
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
 * �����ļ�֮ǰ�ĳ�ʼ�������Դ�ļ�·����Ŀ���ļ�·���Ƿ����
 * @param pMenu
 * ��ǰ�����
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
 * ����ļ��Ķ��ļ���
 * @param fullname
 * ȫ·���������磺/media/cf/2010*.avi
 * @param shortname
 * ���ļ���2010*.avi
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

// �����ļ������߳�,��ˢ�¹����м���ļ���������,��ˢ�·���ÿ���timeout��
extern int stdin_get_message(struct timeval tv,int fd);
extern unsigned int watchdog_time_out_cnt;
#include <pthread.h>
#include <signal.h>

static pthread_t file_copy_thread;
static int file_copy_running = 0;//�ļ���������״̬
static int file_copy_en = 0;//�ļ�����ʹ�ܱ�־
struct FileCopyParam
{
    int from_fd;
    int to_fd;
};

#define BUFFER_SIZE 		(1024*64)
static char FileCopyBuffer[BUFFER_SIZE];

/**
 * @brief FileCopyThread
 * �ļ��������̣߳����߳������������StartFileCopyThread()����
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

	/* ���´�����һ������Ŀ����ļ��Ĵ��� */
	while( file_copy_en && (bytes_read=read(from_fd,FileCopyBuffer,BUFFER_SIZE)))
	{
        /* һ�������Ĵ������� */
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
				/* һ�������������� */
				if((bytes_write==-1)/*&&(errno!=EINTR)*/)
				{
					// add debug msg
					LOGE("%s:write file error:%s",__func__,strerror(errno));
					// set fault write stat
					bytes_write=-1;
					break;
				}
				/* д�������ж����ֽ� */
				else if(bytes_write==bytes_read)
				{
					break;
				}
				/* ֻд��һ����,����д */
				else if(bytes_write>0)
				{
					ptr+=bytes_write;
					bytes_read-=bytes_write;
				}
			}
			
			/* д��ʱ�������������� */
			if(bytes_write==-1)
			{
				break;
			}
		}
	}

    fsync(to_fd);

    file_copy_running = 0;//�ļ�������ɣ��˱�����StopFileCopyThread()�е���

    FUNC_EXIT;
    pthread_exit(0);
}

/**
 * @brief StartFileCopyThread
 * �����ļ��������߳�
 * @param from_fd
 * ������Դ�ļ�
 * @param to_fd
 * ������Ŀ���ļ�
 * @return
 */
int StartFileCopyThread(int from_fd,int to_fd)
{
    int ret = 0;
    struct FileCopyParam param;

    param.from_fd = from_fd;
    param.to_fd = to_fd;

    FUNC_ENTER;

    ret = pthread_create(&file_copy_thread,NULL,FileCopyThread,&param);//�����߳�
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
 * ֹͣ�ļ��������̺߳���
 */

void StopFileCopyThread(void)
{
    int ret;
    int i;

    FUNC_ENTER;
	file_copy_en = 0;

    // ���ȴ�10s,Ȼ��ǿ��ֹͣ����
    for(i=0;i<20;i++)
    {
        if(file_copy_running == 0)
            return;
        usleep(500000);
        FEED_WATCHDOG();
    }

    START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_DELETE_FILE);

    ret = pthread_cancel(file_copy_thread);//��ֹ�ļ������߳�
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
 * �ļ��������ڵĻ��ƺ���������������ʾ���������⡢�����Ľ�������
 * @param pMenu
 * ��ǰ�����
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

//�̶�����һ������Ȼ��ˢ�½���
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

	//���Ʊ���
	bmp_sub_x0 = AUTO_GET_X0_FROM_WIDTH(pBmp_SubBG->XSize);
	bmp_sub_y0 = AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_ALL_HEIGHT + BUTTON_V_INTERVAL_COMMON + TITLE_ALL_HEIGHT/2;
	GUI_DrawBitmap(pBmp_BG,
				AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize),
				AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize));
	GUI_DrawBitmap(pBmp_SubBG,
				bmp_sub_x0,
				bmp_sub_y0);

	//��дTitle��ײ���ʾ��Ϣ
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
		default:	// ����Ѿ��ֶ�ֹͣ,�򲻸���״̬
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
    //zqh 2015��3��30�� modify�����������ļ�����Ϊ���ļ�����
    strncpy(MessageDisp,FileName[pUISystem->allData.Language],MAX_FS_PATH-1);
	strncat(MessageDisp,":",MAX_FS_PATH-strlen(MessageDisp)-1);
	GetFileShortName(src_file,tmpstr);
	strncat(MessageDisp,tmpstr,MAX_FS_PATH-strlen(MessageDisp)-1);
	GUI_DispStringInRect(MessageDisp, &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);

	// feed watchdog before copy files
	FEED_WATCHDOG();
#if USE_FILE_COPY_THREAD
    StartParseMUSBStatus(10);

	//�ļ�����
	/* ��Դ�ļ� */
    from_fd=open(src_file,O_RDONLY);
	if(from_fd < 0)   /*open file readonly,����-1��ʾ�������򷵻��ļ�������*/
	{
		ERR("Open %s Error:%s\n",src_file,strerror(errno));
		strcpy(MessageDisp,MsgBox36[pUISystem->allData.Language]);
		MessageBox(MessageDisp,DEV_MESSAGE_DELAY,
					NULL,NULL,NULL,NULL,NULL,DoFileCopyMessage_Deinit);

		goto MenuFileCopyRefreshEnd;
	}
    LOGT("from_fd=%d\r\n",from_fd);
// 1    close(from_fd);

	/* ����Ŀ���ļ� */
	
	// ���Ŀ���ļ��Ƿ���ڣ�������ڣ�����ɾ�����ļ�
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

	/* ʹ����O_CREATѡ��-�����ļ�,open()������Ҫ��3������,
	mode=S_IRUSR|S_IWUSR��ʾS_IRUSR �û����Զ� S_IWUSR �û�����д*/
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

    // ������������
// 1    StartFileCopyThread(src_file,dst_file);
    StartFileCopyThread(from_fd,to_fd);

    // ˢ�¿�������,��ȡ������Ϣ
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
    	//��д��ʾ����
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

        		// ��ǰ��Copybackʱ,����ȡ��
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
        }else{//zqh 2015��10��16�� add�� �����ļ�������ҲҪ����λ�����Ͱ忨״̬��
            Handle_Timer_Tick(&pUISystem->sDevScanTimer);
        }
        if(nProgress == stat_src.st_size)
        {
            LOGT("copy complete\r\n");
            break;
        }
    }
    LOGT("exit while loop\r\n");

    // �˳�ʱȷ�Ϲرտ����߳�
    StopFileCopyThread();

    // ȷ���ļ�д��
    fsync(to_fd);

    // ����������ر��ļ�
    close(from_fd);
    close(to_fd);

    // �˳�����ʱ�ٴμ��Ŀ���ļ���С
    if(stat(dst_file,&stat_dst) < 0)
    {
        LOGE("check dst file size error %s error code:%d \r\n",strerror(errno),errno);
        nProgress = 0;
    }

    nProgress = stat_dst.st_size;

    StopParseMUSBStatus();
#else
	//�ļ�����
	/* ��Դ�ļ� */
	if((from_fd=open(src_file,O_RDONLY)) < 0)   /*open file readonly,����-1��ʾ�������򷵻��ļ�������*/
	{
		ERR("Open %s Error:%s\n",src_file,strerror(errno));
		strcpy(MessageDisp,MsgBox36[pUISystem->allData.Language]);
		MessageBox(MessageDisp,DEV_MESSAGE_DELAY,
					NULL,NULL,NULL,NULL,NULL,DoFileCopyMessage_Deinit);

		goto MenuFileCopyRefreshEnd;
	}
    LOGT("to_fd=%d\r\n",from_fd);

	/* ����Ŀ���ļ� */
	/* ʹ����O_CREATѡ��-�����ļ�,open()������Ҫ��3������,
	mode=S_IRUSR|S_IWUSR��ʾS_IRUSR �û����Զ� S_IWUSR �û�����д*/
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

	/* ���´�����һ������Ŀ����ļ��Ĵ��� */
	while((bytes_read=read(from_fd,buffer,BUFFER_SIZE)))
	{
	    START_WATCHDOG_TIMER_SIG(120);
		/* һ�������Ĵ������� */
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
				/* һ�������������� */
				if((bytes_write==-1)/*&&(errno!=EINTR)*/)
				{
					// add debug msg
					LOGE("%s:write file error:%s",__func__,strerror(errno));
					// set fault write stat
					bytes_write=-1;
					break;
				}
				/* д�������ж����ֽ� */
				else if(bytes_write==bytes_read)
				{
					nProgress += bytes_write;
					break;
				}
				/* ֻд��һ����,����д */
				else if(bytes_write>0)
				{
					ptr+=bytes_write;
					bytes_read-=bytes_write;
					nProgress += bytes_write;
				}
			}
			
			/* д��ʱ�������������� */
			if(bytes_write==-1)
			{
				break;
			}
		}
        STOP_WATCHDOG_TIMER_SIG();
        // �����ļ�������ʱ
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
			//��д��ʾ����
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

				// ��ǰ��Copybackʱ,����ȡ��
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

		//��д��ʾ����
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
		// Copyδ���,���ݵ�ǰ״̬,����CopyToStage��״̬
		memset(MessageDisp,0,sizeof(MessageDisp));
		switch(pUISystem->nFileCopyToStage)
		{
			case MESSAGESTAGE_COPY_TO_CFCARD:
                strcpy(MessageDisp,MsgBox36[pUISystem->allData.Language]);
                pUISystem->nFileCopyToStage = MESSAGESTAGE_CANCEL_COPYTOCFCARD;//zqh 2015��3��30�� add
			case MESSAGESTAGE_COPY_TO_UDISK:
				strcpy(MessageDisp,MsgBox36[pUISystem->allData.Language]);
                pUISystem->nFileCopyToStage = MESSAGESTAGE_CANCEL_COPYTOUDISK;//zqh 2015��3��30�� add
				break;
			case MESSAGESTAGE_UPDATE_BACKUP:
				strcpy(MessageDisp,FileBackUpFail[pUISystem->allData.Language]);
				pUISystem->nFileCopyToStage = MESSAGESTAGE_CANCEL_BACKUP;
				break;
			case MESSAGESTAGE_UPDATE_UPDATE:
				strcpy(MessageDisp,MsgBox36[pUISystem->allData.Language]);
				pUISystem->nFileCopyToStage = MESSAGESTAGE_CANCEL_UPDATE;
				break;
            // �ؿ�ʧ��
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

	//temp--�趨ˢ������
	SetRefreshAreaPos(OSDPOS_BASEMENU, 
					AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize), 
					AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize), 
					pBmp_BG->XSize, 
					pBmp_BG->YSize, 
					ALPHA_COMMON,
					TRUE);

	return 1;
}

