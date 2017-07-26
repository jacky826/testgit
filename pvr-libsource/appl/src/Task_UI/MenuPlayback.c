#include <config.h>	//Config for platform
#include <UI_Message.h>	//UI message

#include <Task_UI/task_ui_var.h>
#include <Task_UI/pvr_ui_func.h>
#include <pvr_types.h>
#include <Key.h>


int MenuPlayback_Init(UI_Menu_t *pMenu){
	pUISystem->sPlayback.PlayMode=PLAYMODE_NORMAL;
	pUISystem->sPlayback.PlayState=PLAYBACK_PLAY;
	pUISystem->sPlayback.PlaySpeed=0;
	pUISystem->sPlayback.PlayOrient=FOWARD;
	pUISystem->sPlayback.PlayFastorSlow=0;

	pUISystem->sPlayback.PLAY_REP_A=NOVALID_PLAYPOS;
	pUISystem->sPlayback.PLAY_REP_B=NOVALID_PLAYPOS;
	memset(pUISystem->sPlayback.GotoSpecTime,'_',sizeof(pUISystem->sPlayback.GotoSpecTime));

	pUISystem->sPlayback.BytesToFillZero=0;
	pUISystem->sPlayback.TotalPlayTime.Hour = 0;
	pUISystem->sPlayback.TotalPlayTime.Minute = 0;
	pUISystem->sPlayback.TotalPlayTime.Second = 0;

	if (StartPlayFile(pUISystem->sRecFilename)!=ERR_OK){
		goto ERR_EXIT;
	}
	return 1;
ERR_EXIT:
    // TODO: post a message box
	return 0;
}

extern int StopTmpOSD(void);

TMsgHandleStat MenuPlayBack_DecodeHandle(UI_Menu_t *pMenu,unsigned short nMsgData){
	TMsgHandleStat retval=MsgHandled;
	int nNeedClearStat=1;
	LOGT("%s:MsgData= 0x%x PlayMode=%d\n",__func__,nMsgData,pUISystem->sPlayback.PlayMode);
	switch(nMsgData){
		case MSG_PARA_DEC_FILEEND:
			Player_Stop();
			if (pUISystem->sPlayback.PlayMode==PLAYMODE_REPFILE){
				if (StartPlayFile(pUISystem->sRecFilename)==ERR_OK){
					nNeedClearStat=0;
					return MsgHandled;
				}
			}
            StopTmpOSD();
			break;
		case MSG_PARA_DEC_FILEBEGIN:
            DoAdjustPlaySpeed(FOWARD,0);
			nNeedClearStat=0;
			break;
		default:
			break;
	}
	if (nNeedClearStat){
		ClearPlayState();
        LOGT("MenuRestore\r\n");
		SystemMenuRestore();
        SETBIT(pUISystem->SysState.CurState, SYS_BIT_FILEBROSWER);//设置文件浏览状态位
		ShowDecoderMessage(nMsgData);
	}
	return retval;
}

int AdjustPlaySpeed(enum PlayerOrient playOrient, int nFastorSlow){//nFastorSlow=1 fast -1 slow, 0 normal
	enum PlayBackSpeed playbackSpeed;

	if (pUISystem->sPlayback.PlayState!=PLAYBACK_PLAY){
		return GUI_ERR(ERR_SYSSTATE);
	}
	
	if (nFastorSlow!=pUISystem->sPlayback.PlayFastorSlow){
		pUISystem->sPlayback.PlaySpeed=0;
		pUISystem->sPlayback.PlayFastorSlow=nFastorSlow;
	}

	if (playOrient!=pUISystem->sPlayback.PlayOrient){
		pUISystem->sPlayback.PlaySpeed=0;
		pUISystem->sPlayback.PlayOrient=playOrient;
	}
	switch (nFastorSlow){
		case 0:
			pUISystem->sPlayback.PlaySpeed=0;
			playbackSpeed=(enum PlayBackSpeed)PlaybackSpeedFast[pUISystem->sPlayback.PlaySpeed];
			break;
		case 1:
			AdjustValue(&(pUISystem->sPlayback.PlaySpeed),1,0,PLAYSPEEDFAST_CNT);		
			playbackSpeed=(enum PlayBackSpeed)PlaybackSpeedFast[pUISystem->sPlayback.PlaySpeed];
			break;
		case -1:
			AdjustValue(&(pUISystem->sPlayback.PlaySpeed),1,0,PLAYSPEEDSLOW_CNT);		
			playbackSpeed=(enum PlayBackSpeed)PlaybackSpeedSlow[pUISystem->sPlayback.PlaySpeed];
			break;
	}

	if (pUISystem->sPlayback.PlaySpeed==0){//Normal
		pUISystem->sPlayback.PlayOrient=FOWARD;
		pUISystem->sPlayback.PlayFastorSlow=0;
	}

	return Player_SetSpeed(playOrient,playbackSpeed);
}

int IsNormalPlaySpeed(void)
{
	if((pUISystem->sPlayback.PlaySpeed==0)&&
		(pUISystem->sPlayback.PlayOrient==FOWARD)&&
		(pUISystem->sPlayback.PlayFastorSlow==0))
		return 1;
	else
		return 0;
}

void DoAdjustPlaySpeed(enum PlayerOrient playOrient, int nFastorSlow){
	if (AdjustPlaySpeed(playOrient,nFastorSlow)==ERR_OK){
		DisplayControl();//Display OSD
	}else{
		;//Display "NULL"
	}
}

void DoPausePlayback(void){
	BOOL tmpval;

	if (pUISystem->sPlayback.PlayFastorSlow==0){
		pUISystem->sPlayback.PlayState=(pUISystem->sPlayback.PlayState==PLAYBACK_PLAY)?PLAYBACK_PAUSE:PLAYBACK_PLAY;	
		tmpval=(pUISystem->sPlayback.PlayState==PLAYBACK_PLAY)?0:1;	
		Player_Pause(tmpval);
		if (tmpval){
			DisplayPause();
		}else{
			DisplayControl();
		}
	}else{
		DoAdjustPlaySpeed(FOWARD,0);
	}
	
}

TMsgHandleStat MenuPlayBack_PlayBackTimer_TO(TEF_TIMER_STRUCT *pTimer){
	int nPosition;
	
	if (pUISystem->sPlayback.PLAY_REP_B > 0){
		nPosition = Player_GetPos();
		if (nPosition >= pUISystem->sPlayback.PLAY_REP_B){
			Player_GotoPos(pUISystem->sPlayback.PLAY_REP_A);
		}
	}
	return MsgHandled;
}

TMsgHandleStat MenuPlayback_KeyHandle(UI_Menu_t *pMenu,unsigned short nMsgData){
	TMsgHandleStat retval=MsgHandled;
	int idx;

    if(CheckMountFlags() < 0)
    {
        Player_Stop();
		StopTmpOSD();
    	retval=MsgHandled;
        return retval;
    }

	switch(nMsgData){
		case KEY_PLAY:
			DoPausePlayback();
			break;
		case KEY_STOP:
			Player_Stop();
//			DisplayStop();
            StopTmpOSD();
			break;
		case KEY_FORWARD:
            if(pUISystem->sPlayback.PlayOrient == BACKWARD)
			{
				DoAdjustPlaySpeed(FOWARD,0);
			}
			else	
			{
				DoAdjustPlaySpeed(FOWARD,1);
			}
			break;
		case KEY_REWIND:
			DoAdjustPlaySpeed(BACKWARD,1);
			break;	
		case KEY_SFORWARD:
			DoAdjustPlaySpeed(FOWARD,-1);
			break;
		case KEY_SBACKWARD:
			DoAdjustPlaySpeed(BACKWARD,-1);
			break;
		default:
			retval=MsgNeedDispatch;
			break;
	}
	return retval;
}


TMsgHandleStat MenuPlayback_MsgHandle(UI_Menu_t *pMenu,unsigned short nMsgType,unsigned short nMsgData){
	TMsgHandleStat retval=MsgHandled;
	switch(nMsgType){
		case MSG_DECODER_EVENT:
			retval=MenuPlayBack_DecodeHandle(pMenu, nMsgData);
			break;
		case MSG_KEY_EVENT:
			retval=MenuPlayback_KeyHandle(pMenu, nMsgData);
			break;
		default:
			retval=MsgNocare;
			break;
	}
	return retval;
}

int MenuPlayback_Close(UI_Menu_t *pMenu){
	//Disable PlayBackTimer 
//	DisableTimer(&pUISystem->sPlayBackTimer);//Do close OSD
	return 1;
}

int MenuPlayback_Refresh(UI_Menu_t *pMenu){
	//Restore OSD
	RestoreAllStaticOSD();
    DisplayControl();
	return 1;
}

