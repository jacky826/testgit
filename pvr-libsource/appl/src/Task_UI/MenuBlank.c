#include <config.h>	//Config for platform
#include <UI_Message.h>	//UI message

#include <Task_UI/task_ui_var.h>
#include <Task_UI/pvr_ui_func.h>
#include <GUI.H>
#include <Key.h>
#include "../../avi/encoder.h"

/*
 *���ļ��Ǹ��˵�����û��GUI���档��ǰϵͳû����������˵�ʱ�����е�
 *������Ϣ�ͻ��ɴ˴��ڴ���Ŀǰ����?

 * KEY_BROWSER : �л����ļ�������ڡ�
 * KEY_DISPLAY:��ʾ��ǰ״̬(�Ƿ���¼��)
 * KEY_REC: start/pause¼��
 * KEY_STOP:ֹͣ¼��
 * KEY_PLAY:�ط����¼���ļ���
 * ��������л���ϵͳ���ô��ڵ���Ϣ����
 */

/*
 * @brief MenuBlank_Refresh
 * ���˵�ˢ�º������������ô��ڵı���ɫ�����弰GUI��ɫ
 * @param pMenu
 * @returnΪ1��
 */
 
int MenuBlank_Refresh(UI_Menu_t *pMenu){
	//���ñ���ɫ
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
    //	Purpose	:	���Ʊ���
    //--------------------------------------------------------------------------//
    GUI_DrawBitmap(pBmp_BG,\
                            AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize),\
                            AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize));
    GUI_DrawBitmap(pBmp_SubBG,\
                            AUTO_GET_X0_FROM_WIDTH(pBmp_SubBG->XSize),\
                            (AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_ALL_HEIGHT + BUTTON_V_INTERVAL_COMMON));

    //--------------------------------------------------------------------------//
    //	Purpose	:	��дTitle
    //--------------------------------------------------------------------------//
    //FUNC_ENTER
    GUI_SetTextMode(GUI_TM_TRANS);
    GUI_DispStringHCenterAt(Message[pUISystem->allData.Language],\
                    (LCD_WIDTH / 2),\
                    (AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_BELOW_HEIGHT));\

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

    GUI_DispStringInRect(MessageDisp, &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);

    //GUI_DispStringAt(HelpTip7[pUISystem->allData.Language],\
                    AUTO_GET_X0_FROM_WIDTH(pBmp_SubBG->XSize),\
                    (AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + pBmp_BG->YSize - GUI_Font20_SongTi_Unicode.YSize - PROCESS_V_OFFSET_BG_BOTTON));

    //temp--�趨ˢ������
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
 * ���˵�����������GUI���ڴ˺����д�������Ϣ���Լ���ʾ��Ӧ�ĵ�����
 * @param pMenu
 * pMenu��ǰ����
 *
 * @param nMsgType
 * nMsgType�ж���Ϣ�����Ƿ�Ϊ������Ϣ
 *
 * @param nMsgData
 * nMsgDataΪ������Ϣ�ľ�������
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

    //�ж���Ϣ�����Ƿ��ǰ����¼�
	if (nMsgType!=MSG_KEY_EVENT){
		return MsgNocare;
	}

	LOGT("%s:current state = 0x%x MsgData=0x%x\n",__func__,pUISystem->SysState.CurState,nMsgData);
	
    //����İ�������
	switch(nMsgData){
        case KEY_BROWSER://�л����ļ��������
			if(((pUISystem->SysState.CurState)&(SYS_BIT_RECORDING_ONE_CLICK))||\
                ((pUISystem->SysState.CurState)&(SYS_BIT_PLAYBACK)))//�жϵ�ǰϵͳ�Ƿ�Ϊ¼�ƻ��߻طŵ�״̬
			{
				retval=MsgNeedDispatch;
			}
			else
			{
                if(CheckMountFlags() < 0)//���cf�Ƿ����
                {
    				retval=MsgNeedDispatch;
                    break;
                }
				SetLvdsSwitch(0);
                MessageBox_Refresh(EnterFileBrowser[pUISystem->allData.Language]);
                //MessageBox(EnterFileBrowser[pUISystem->allData.Language],3,NULL,NULL,NULL,NULL,NULL,NULL);
                //�л����ļ��������
				SystemChangeWindow(MENUFILEBROWSER, 1, 1,1);//Change menu to Filebrowser,Close to disable key timer
				SETBIT(pUISystem->SysState.CurState, SYS_BIT_FILEBROSWER);//�����ļ����״̬λ
			}
			break;
        case KEY_REC://¼����Ƶ
			if (TESTBIT(pUISystem->SysState.CurState,SYS_BIT_RECORDING_ONE_CLICK)){
				retval=MsgNeedDispatch;
			}else{
                if(CheckMountFlags() < 0)//���cf�Ƿ����
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
        case KEY_STOP://ֹͣ¼��
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
 * �˺���ɶ��û��
 *
 * @param pMenu
 * pMenu��ǰ����
 *
 * @return 1
 */
int MenuBlank_Close(UI_Menu_t *pMenu){
	return 1;
}

/*
 * @brief MenuBlank_Open  1����Ƶ�л�
 *                        2�����ڵ�ˢ�£�ʵ�����ǵ���MenuBlank_Refresh(UI_Menu_t *pMenu)�������
 * @param pMenu
 * pMenu��ǰ����
 *
 * @return 1
 */
int MenuBlank_Open(UI_Menu_t *pMenu){
    SetLvdsSwitch(1);//��Ƶ�л�
    MenuRefresh();//����ˢ��
	return 1;
}
