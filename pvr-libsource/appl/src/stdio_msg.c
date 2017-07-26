#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <GUI.H>
#include <pvr_types.h>
#include <Key.h>
#include "../../pvr_debug.h"
#include <ti/sdo/dmai/Fifo.h>

#include <errno.h>

#include "RTC.h"
#include "UserMsg.h"
/*************** ryan add ***********************************/
#define KEY_PRESS		1
#define KEY_UNPRESS		0

//#if  USE_SERIAL_MSG_422

//! 按键协议键值定义

#define REMOTE_KEY_PLAY				KEYPLAY
#define REMOTE_KEY_REC				(KEYENTER|KEYREC)
#define REMOTE_KEY_STOP				KEYSTOP 
#define REMOTE_KEY_REWIND			KEYREWIND
#define REMOTE_KEY_FORWARD			KEYFORWARD
#define REMOTE_KEY_NEXT     		(KEYREC)
#define REMOTE_KEY_UP				KEYUP
#define REMOTE_KEY_DOWN				KEYDOWN
#define REMOTE_KEY_LEFT				KEYREWIND
#define REMOTE_KEY_RIGHT			KEYFORWARD
#define REMOTE_KEY_ENTER			KEYENTER
#define REMOTE_KEY_ESC				KEYSTOP
#define REMOTE_KEY_BROWSER			(KEYENTER|KEYPLAY)
#define REMOTE_KEY_UPDATE			(KEYDOWN|KEYFORWARD)  //haiqing
#define REMOTE_KEY_SYSTEM			(KEYUP|KEYREWIND)	// copy to cf,format

// The remote key value with pc
#define REMOTE_KEY_PC_PLAY				('p')
#define REMOTE_KEY_PC_REC				('r')
#define REMOTE_KEY_PC_STOP				('s')
#define REMOTE_KEY_PC_REWIND			('j')
#define REMOTE_KEY_PC_FORWARD			('l')
#define REMOTE_KEY_PC_NEXT				('t')
#define REMOTE_KEY_PC_UP				('i')
#define REMOTE_KEY_PC_DOWN				('k')
#define REMOTE_KEY_PC_LEFT				('j')
#define REMOTE_KEY_PC_RIGHT             ('l')
#define REMOTE_KEY_PC_ENTER             ('e')
#define REMOTE_KEY_PC_ESC			    ('q')
#define REMOTE_KEY_PC_BROWSER			('b')
#define REMOTE_KEY_PC_UPDATE			('u')
#define REMOTE_KEY_PC_SYSTEM			('c')

//#endif
extern int rs422fd;
typedef struct _KEY_MAP {
	BYTE RemoteKey;
	BYTE GuiKey;
} KEY_MAP;
const KEY_MAP KeyMap[] =
{
    //	Remote key					GUI Key
    {REMOTE_KEY_PLAY,			KEY_PLAY},
    {REMOTE_KEY_REC,			KEY_REC},
    {REMOTE_KEY_STOP,			KEY_STOP},
    {REMOTE_KEY_REWIND,         KEY_REWIND},
    {REMOTE_KEY_FORWARD,		KEY_FORWARD},
    {REMOTE_KEY_NEXT,			KEY_NEXT},
    {REMOTE_KEY_UPDATE,			KEY_UPDATE},
    {REMOTE_KEY_UP,				KEY_UP},
    {REMOTE_KEY_DOWN,			KEY_DOWN},
    {REMOTE_KEY_LEFT,			KEY_LEFT},
    {REMOTE_KEY_RIGHT,			KEY_RIGHT},
    {REMOTE_KEY_ENTER,			KEY_ENTER},
    {REMOTE_KEY_ESC,			KEY_ESC},
    {REMOTE_KEY_BROWSER,		KEY_BROWSER},
    {REMOTE_KEY_SYSTEM,			KEY_SYSTEM},
};

const KEY_MAP KeyMapPC[] =
{
    //	Remote key              GUI Key
    {REMOTE_KEY_PC_PLAY,        KEY_PLAY},
    {REMOTE_KEY_PC_REC,         KEY_REC},
    {REMOTE_KEY_PC_STOP,        KEY_STOP},
    {REMOTE_KEY_PC_REWIND,      KEY_REWIND},
    {REMOTE_KEY_PC_FORWARD,     KEY_FORWARD},
    {REMOTE_KEY_PC_NEXT,        KEY_NEXT},
    {REMOTE_KEY_PC_UPDATE,      KEY_UPDATE},
    {REMOTE_KEY_PC_UP,          KEY_UP},
    {REMOTE_KEY_PC_DOWN,        KEY_DOWN},
    {REMOTE_KEY_PC_LEFT,        KEY_LEFT},
    {REMOTE_KEY_PC_RIGHT,       KEY_RIGHT},
    {REMOTE_KEY_PC_ENTER,       KEY_ENTER},
    {REMOTE_KEY_PC_ESC,         KEY_ESC},
    {REMOTE_KEY_PC_BROWSER,     KEY_BROWSER},
    {REMOTE_KEY_PC_SYSTEM,      KEY_SYSTEM},
};

#define dim(x) (sizeof(x) / sizeof(x[0]))

extern Fifo_Handle  hmsgFifo;
extern int msg_fd;
int stdin_get_message(struct timeval tv,int fd)
{
	fd_set         fds;
	int            ret;
	int 		maxfd;
    unsigned char KeyValue422,phy_key422=0;
    unsigned short j;
    int ret422;
    DATE_TIME tTime;
    TUserMsgRecv422 tMsg422;
	int phy_key=0;//SACH:10839
	int KeyValue=0;
	unsigned short i;
#if USE_SERIAL_MSG_422
	TUserMsgRecv tMsg;
#endif

	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds);
	if(fd!=-1)
	{
		FD_SET(fd,&fds);
	}
	if(msg_fd!=-1)
	{
		FD_SET(msg_fd,&fds);
	}

	maxfd = (STDIN_FILENO>msg_fd)?STDIN_FILENO:msg_fd;
	maxfd = (fd>maxfd)?fd:maxfd;
	maxfd +=1;

//	LOGT("fd = %d; msg_fd = %d; maxfd = %d\n",fd,msg_fd,maxfd);

	FEED_WATCHDOG();

	ret = select(maxfd, &fds, NULL, NULL, &tv);

	if (ret == -1) {
		LOGE("%s:Select failed on stdin\n",__func__);
		request2reboot();
		return -1;
	}
#if 0
	while (ret == -1) {
		if(errno == EINTR)
			ret = select(maxfd, &fds, NULL, NULL, &tv);
		else
		{
			LOGE("%s:Select failed on stdin\n",__func__);
			request2reboot();
			return -1;
		}
//		LOGE("%s:Select failed on stdin\n",__func__);
//		request2reboot();
//		return -1;
	}
#endif

    if (ret == 0)
	{
		return 0;
    }

	if (FD_ISSET(STDIN_FILENO/*fileno(stdin)*/, &fds)) 
	{
//		LOGT("stdin is set\n");
#if USE_SERIAL_MSG_232
        ret=GetUserMsg(&tMsg);
        if(ret<=0)
        {
            return ret;
        }
        phy_key=tMsg.keyVal;
        LOGT("phy_key = 0x%x\r\n",phy_key);
        KeyValue = 0x00; // None key
        for (i = 0; i < dim(KeyMap); i++)
        {
            if (KeyMap[i].RemoteKey == phy_key)
            {
                KeyValue = KeyMap[i].GuiKey;
                if (KeyValue != 0x00) // Valid key press
                {
                    GUI_StoreKey(KeyValue);
                    FUNC_EXIT
                    return 1;
                }
                break;
            }
        }
#else
        phy_key = getchar();
        KeyValue = 0x00;
//		LOGT("%s:getchar return 0x%x\n",__func__,phy_key);
        for (i = 0; i < dim(KeyMapPC); i++)
        {
            if (KeyMapPC[i].RemoteKey == phy_key)
            {
                KeyValue = KeyMapPC[i].GuiKey;
                if (KeyValue != 0x00) // Valid key press
                {
                    GUI_StoreKey(KeyValue);
                    return 1;
                }
                break;
            }
        }
#endif
	}
#if USE_SERIAL_MSG_422
	else if(FD_ISSET(fd,&fds))
	{
		int tmp;
		LOGT("rs422 is set\n");
		ret422=GetUserMsg422(&tMsg422,fd);
		if(ret422<=0)
		{
			return ret;
		}
		phy_key422=tMsg422.keyVal;
		LOGT("\r\n phy_key = 0x%x\r\n",phy_key422);
        // TODO set time / set pos / get sys stat-----move to SystemMsgHandle_Key
        switch(tMsg422.keyVal)
        {
        //        if(tMsg422.keyVal==KEY_SET_TIME)
        case KEY_SET_TIME:
        {
            tmp=(tMsg422.data.msgTime.year[1]<<8)|tMsg422.data.msgTime.year[0];
            tmp&=0xffff;
            tTime.Year=tmp-1900;
            tTime.Month=tMsg422.data.msgTime.mon-1;
            tTime.Day=tMsg422.data.msgTime.day;
            tTime.Hour=tMsg422.data.msgTime.hour;
            tTime.Minute=tMsg422.data.msgTime.min;
            tTime.Second=tMsg422.data.msgTime.sec;
            SetCurrentDateTime(&tTime);
            return 1;
        }
        case KEY_SET_OSD_POS:
//            0121-2013 增加OSD图标位置设置
//            else if(tMsg422.keyVal == KEY_SET_OSD_POS)
        {
            tmp = (tMsg422.data.OSD_Pos.OSD_x[1]<<8)|(tMsg422.data.OSD_Pos.OSD_x[0]);
            tmp &= 0xffff;
            CUR_STATUS_X0 = tmp;
            tmp = (tMsg422.data.OSD_Pos.OSD_y[1]<<8)|(tMsg422.data.OSD_Pos.OSD_y[0]);
            tmp &= 0xffff;
            CUR_STATUS_Y0 = tmp;
            return 1;
        }
        case KEY_GET_SYS_STAT:
        {
            tMsg.keyVal=0;
            SendUserMsg(&tMsg);
            return 1;
        }
        default:
#if USE_SERIAL_MSG_422
            for (j = 0; j < dim(KeyMap); j++)
            {
                if (KeyMap[j].RemoteKey == phy_key422)
                {
                    KeyValue422 = KeyMap[j].GuiKey;
                    if (KeyValue422 != 0x00) // Valid key press
                    {
                        GUI_StoreKey(KeyValue422);
                        return 1;
                    }
                    break;
                }
            }
#endif
            break;
        }
	}
#endif
	else if(FD_ISSET(msg_fd,&fds))
	{
		LOGT("msg_fd is set\n");
		unsigned int msg;
		if(Dmai_EOK!=Fifo_get(hmsgFifo,&msg)) {
			LOGE("%s:fifo read failed\n",__func__);
			request2reboot();
		}else {
			GUI_StoreKey(msg);
			LOGT("%s:got none key message 0x%x\n",__func__,msg);
			return 1;
		}
	}
	return 0;
}
