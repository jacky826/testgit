#include <config.h>	//Config for platform
#include <UI_Message.h>	//UI message

#include <Task_UI/task_ui_var.h>
#include <Task_UI/pvr_ui_func.h>
#include <GUI.H>
#include <Key.h>
#include "../../avi/encoder.h"

/*
 *此文件是根菜单，它没有GUI界面。当前系统没有其他顶层菜单时，所有的
 *按键消息就会由此窗口处理，目前包括?

 * KEY_BROWSER : 切换到文件浏览窗口。
 * KEY_DISPLAY:显示当前状态(是否在录像)
 * KEY_REC: start/pause录像
 * KEY_STOP:停止录像
 * KEY_PLAY:回放最近录像文件。
 * 还会包括切换到系统配置窗口的消息处理。
 */

/*
 * @brief MenuBlank_Refresh
 * 根菜单刷新函数，包括设置窗口的背景色、字体及GUI颜色
 * @param pMenu
 * @return为1。
 */
 
int MenuBlank_Refresh(UI_Menu_t *pMenu){
	//设置背景色
	GUI_SetBkColor(GUI_BLACK);
	GUI_SetFont(&GUI_Font20_SongTi_Unicode);
	GUI_SetColor(GUI_WHITE);
	//Restore OSD
	RestoreAllStaticOSD();
	return 1;
}

#include "UserMsg.h"

//JunR@201111: check video in signal in service via fpga's status register
int VideoInAvailable(void)
{
	return 1;
}

int MessageBox_Refresh(char *MessageDisp)
{
    GUI_BITMAP *pBmp_BG = (&bmMessageBoxBG_360_240);
    GUI_BITMAP *pBmp_SubBG = (&bmMessageRect_310_130);
    GUI_RECT Rect;

    FUNC_ENTER

    GUI_SetFont(&GUI_Font20_SongTi_Unicode);
    GUI_SetColor(GUI_WHITE);
    GUI_SetBkColor(GUI_BLACK);
    //--------------------------------------------------------------------------//
    //	Purpose	:	绘制背景
    //--------------------------------------------------------------------------//
    GUI_DrawBitmap(pBmp_BG,\
                            AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize),\
                            AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize));
    GUI_DrawBitmap(pBmp_SubBG,\
                            AUTO_GET_X0_FROM_WIDTH(pBmp_SubBG->XSize),\
                            (AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_ALL_HEIGHT + BUTTON_V_INTERVAL_COMMON));

    //--------------------------------------------------------------------------//
    //	Purpose	:	填写Title
    //--------------------------------------------------------------------------//
    //FUNC_ENTER
    GUI_SetTextMode(GUI_TM_TRANS);
    GUI_DispStringHCenterAt(Message[pUISystem->allData.Language],\
                    (LCD_WIDTH / 2),\
                    (AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_BELOW_HEIGHT));\

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

    GUI_DispStringInRect(MessageDisp, &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);

    //GUI_DispStringAt(HelpTip7[pUISystem->allData.Language],\
                    AUTO_GET_X0_FROM_WIDTH(pBmp_SubBG->XSize),\
                    (AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + pBmp_BG->YSize - GUI_Font20_SongTi_Unicode.YSize - PROCESS_V_OFFSET_BG_BOTTON));

    //temp--设定刷新区域
    SetRefreshAreaPos(OSDPOS_BASEMENU,
                    AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize),
                    AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize),
                    pBmp_BG->XSize,
                    pBmp_BG->YSize,
                    ALPHA_COMMON,
                    TRUE);
    RefreshOSDDisplay();

    return 1;
}

/*
 * @brief MenuBlank_MsgHandle
 * 根菜单处理函数，无GUI，在此函数中处理按键消息，以及显示对应的弹窗。
 * @param pMenu
 * pMenu当前窗口
 *
 * @param nMsgType
 * nMsgType判断消息类型是否为按键消息
 *
 * @param nMsgData
 * nMsgData为按键消息的具体类型
 *
 * @return
 */
TMsgHandleStat MenuBlank_MsgHandle(UI_Menu_t *pMenu,unsigned short nMsgType,unsigned short nMsgData){
	TMsgHandleStat retval=MsgHandled;
	int ret;
#if USE_SERIAL_MSG_422
    //Reply Message
    TUserReply tReply;
#endif
	int i;

    //判断消息类型是否是按键事件
	if (nMsgType!=MSG_KEY_EVENT){
		return MsgNocare;
	}

	LOGT("%s:current state = 0x%x MsgData=0x%x\n",__func__,pUISystem->SysState.CurState,nMsgData);
	
    //具体的按键类型
	switch(nMsgData){
        case KEY_BROWSER://切换至文件浏览窗口
			if(((pUISystem->SysState.CurState)&(SYS_BIT_RECORDING_ONE_CLICK))||\
                ((pUISystem->SysState.CurState)&(SYS_BIT_PLAYBACK)))//判断当前系统是否为录制或者回放的状态
			{
				retval=MsgNeedDispatch;
			}
			else
			{
                if(CheckMountFlags() < 0)//检查cf是否加载
                {
    				retval=MsgNeedDispatch;
                    break;
                }
				SetLvdsSwitch(0);
                MessageBox_Refresh(EnterFileBrowser[pUISystem->allData.Language]);
                //MessageBox(EnterFileBrowser[pUISystem->allData.Language],3,NULL,NULL,NULL,NULL,NULL,NULL);
                //切换至文件浏览窗口
				SystemChangeWindow(MENUFILEBROWSER, 1, 1,1);//Change menu to Filebrowser,Close to disable key timer
				SETBIT(pUISystem->SysState.CurState, SYS_BIT_FILEBROSWER);//设置文件浏览状态位
			}
			break;
        case KEY_REC://录制视频
			if (TESTBIT(pUISystem->SysState.CurState,SYS_BIT_RECORDING_ONE_CLICK)){
				retval=MsgNeedDispatch;
			}else{
                if(CheckMountFlags() < 0)//检查cf是否加载
                {
    				retval=MsgNeedDispatch;
                    break;
                }

				//Start Record
				//JunR@201111:check SYS_BIT_Disk_Full flag before start
				//JunR@201111:check cf/sd/usb mount flag before start
				//JunR@201111:check video in signal presented via fpga before start
				if(VideoInAvailable()==0) {
					//do someting 
				}
				
				if ((ret=StartRecord(0))==ERR_OK){
					//Start Message
					SETBIT(pUISystem->SysState.CurState,SYS_BIT_RECORDING_ONE_CLICK);
#if USE_SERIAL_MSG_422
                    //Send Reply
                    tReply.ePvrStat = ePvrRec;
                    SendUserMsg(&tReply);
#endif
				}else{
					//JunR@201111: more steps needed to deal with this error: reboot/send message via serial/show error screen?
					LOGE("%s:StartRecord return 0x%x\n",__func__,ret);
					Post_UI_Message(MSG_ENCODER_EVENT,ret);
					retval=MsgNeedDispatch;
					request2reboot();
				}
			}
			break;
        case KEY_STOP://停止录制
		    if(CheckMountFlags() < 0)
            {
				retval=MsgNeedDispatch;
                break;
            }

			if(StopRecord()!=ERR_OK)
			{
				retval=MsgNeedDispatch;
			}
			break;
		default:
			retval=MsgNeedDispatch;
			break;
	}
	
	return retval;
}

/*
 * @brief MenuBlank_Close
 * 此函数啥都没做
 *
 * @param pMenu
 * pMenu当前窗口
 *
 * @return 1
 */
int MenuBlank_Close(UI_Menu_t *pMenu){
	return 1;
}

/*
 * @brief MenuBlank_Open  1、视频切换
 *                        2、窗口的刷新，实际上是调用MenuBlank_Refresh(UI_Menu_t *pMenu)这个函数
 * @param pMenu
 * pMenu当前窗口
 *
 * @return 1
 */
int MenuBlank_Open(UI_Menu_t *pMenu){
    SetLvdsSwitch(1);//视频切换
    MenuRefresh();//窗口刷新
	return 1;
}
