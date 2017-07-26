/*
*pvr串口通信协议实现
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <GUI.H>
#include <pvr_types.h>
#include <time.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <config.h>	//Config for platform
#include <fcntl.h>

#include <errno.h>

#include "pvr_debug.h"
#include "UserMsg.h"
/* These are the hash definitions */

#define DEV "/dev/ttyS1"
static struct termios ttySave;
extern int rs422fd;
int InitComMsg422(struct termios *oldtio)
{
        int fd,status;
        unsigned char cmd[17];
        struct termios newtio;        
        int n;
        
        //open carmo device
        fd = open(DEV, O_RDWR| O_NOCTTY|O_NDELAY);
        if (fd ==-1)
        {
                LOGE("open"DEV": %s\r\n",strerror(errno));
                return -1;
        }

        LOGT("open dev rs422 ok\r\n");

        tcgetattr(fd,oldtio); //save current serial port settings
//        memcpy(&newtio,&oldtio,sizeof(struct termios));
		bzero(&newtio, sizeof(newtio)); // clear struct for new port settings
        //configure the serial port;
        cfsetispeed(&newtio,B38400);
        cfsetospeed(&newtio,B38400);
        newtio.c_cflag |=CLOCAL|CREAD;
        /*8N1*/
        newtio.c_cflag &= ~CSIZE; /* Mask the character size bits */
        newtio.c_cflag |= CS8;    /* Select 8 data bits */
        newtio.c_cflag &= ~PARENB;
        newtio.c_cflag &= ~CSTOPB;
        newtio.c_cflag &= ~CRTSCTS;//disable hardware flow control;
        newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);/*raw input*/
        newtio.c_oflag  &= ~OPOST;   /*raw output*/
        tcflush(fd,TCIFLUSH);//clear input buffer
        newtio.c_cc[VTIME] = 10000; /* inter-character timer unused */
        newtio.c_cc[VMIN] = 0; /* blocking read until 0 character arrives */
        tcsetattr(fd,TCSANOW,&newtio);

		n=fcntl(fd,F_GETFL,0);
		if(fcntl(fd,F_SETFL,n|O_NONBLOCK)<0) {  //no block 
			ERR("set rs422 nonblock failed\r\n");
			close(fd);
			return -1;
		}
		LOGT("422 init ok\r\n");
        return fd;
}    //end
int InitComMsg(void)
{

	struct termios ttynew;
	long vdisable;
	int n;

	tcgetattr(STDIN_FILENO, &ttySave);
	memcpy(&ttynew,&ttySave,sizeof(struct termios));

	if ((vdisable=fpathconf(STDIN_FILENO, _PC_VDISABLE))<0)
	{
		printf("vdisable not avialiable\r\n");
	}

	/* set control chars */
	ttynew.c_cc[VINTR] = vdisable;	/*3= C-c */
	ttynew.c_cc[VQUIT] = vdisable;	/* 28=C-\ */
	ttynew.c_cc[VERASE] =vdisable ; /*127= C-? */
	ttynew.c_cc[VKILL] = vdisable;	/*21= C-u */
	ttynew.c_cc[VEOF] = vdisable;	/*4= C-d */
	ttynew.c_cc[VSTART] = vdisable; /* 17=C-q */
	ttynew.c_cc[VSTOP] = vdisable;	/* 19 =C-s */
	ttynew.c_cc[VSUSP] = vdisable;	/* 26= C-z */

	/* use line dicipline 0 */
	ttynew.c_line = 0;

	/* Make it be sane */
	ttynew.c_cflag &= CBAUD | CBAUDEX | CSIZE | CSTOPB | PARENB | PARODD;
	ttynew.c_cflag |= CREAD | HUPCL | CLOCAL;


	/* input modes */
	ttynew.c_iflag = ICRNL | IXON | IXOFF;

	/* output modes */
	ttynew.c_oflag = OPOST | ONLCR;

	/* local modes */
	ttynew.c_lflag =ISIG | IEXTEN;
	
	ttynew.c_iflag &=~(ICRNL|INLCR|IGNCR);
	ttynew.c_oflag &=~(OCRNL|ONLCR);

	tcsetattr(STDIN_FILENO, TCSANOW, &ttynew);


	n=fcntl(STDIN_FILENO,F_GETFL,0);
	if(fcntl(STDIN_FILENO,F_SETFL,n|O_NONBLOCK)<0) {  //no block 
		ERR("set rs232 nonblock failed\r\n");
		return -1;
	}

	return 0;	
}

int DeInitComMsg(void)
{
	int n;
	tcsetattr(0, TCSANOW, &ttySave);

	n=fcntl(STDIN_FILENO,F_GETFL,0);
	if(fcntl(STDIN_FILENO,F_SETFL,n&~O_NONBLOCK)<0) {  //no block 
		ERR("set rs232 nonblock failed\r\n");
		return -1;
	}


	return 0;
}

enum {
	eWaiting=0,
	eWait2,
	eRead,
	eReadComplete=RECV_USERMSGBUF_SIZE,
	e422ReadComplete=RECV_USERMSGBUF422_SIZE
};

// Add recv_header flag.If received a 0xaa,this flag is set to 1.
int SerialGetUserMsg(TUserMsgRecv *pMsgRecv)
{
	char nRet=0;
	int i;
	int ret;
	int len;

	static int nState=eWaiting;
	static char buf[RECV_USERMSGBUF_SIZE+1];
	static int recv_header = 0;

	memset(&buf[nState],0,(sizeof(buf)-nState));

	while(1)
	{
		ret = read(STDIN_FILENO,&nRet,sizeof(nRet));
		if(ret<0)
		{
			FUNC_EXIT
			return -1;
		}
		if(ret==0)
		{
			LOGT("read finished\r\n");
			FUNC_EXIT
			break;
		}

		LOGT("get char 0x%02x\t",nRet);
		LOGT("nState = %d\r\n",nState);

#if 1
		if(nRet == 0xaa)
		{
			recv_header = 1;
		}
		else
		{
			if((recv_header)&&(nRet == 0x55))
			{
				buf[eWaiting] = 0xaa;
				nState = eWait2;
			}
			recv_header = 0;
		}

		if(nState >= eWait2)
		{
			buf[nState] = nRet;
			nState++;
		}

		if(nState >= eReadComplete)
		{
			memcpy((void *)pMsgRecv,buf,sizeof(TUserMsgRecv));
			nState = eWaiting;
			return 1;
		}
#else
		if(nState>=eReadComplete)
		{
			LOGT("nState ERROR\r\n");
			nState = eWaiting;
			return 0;
		}

		switch(nRet)
		{
			case 0xAA:
				recv_header = 1;
				if(nState>=eRead)
				{
					buf[nState] = nRet;
					nState++;
				}
				break;
			case 0x55:
				if(recv_header == 1)
				{
					buf[eWaiting] = 0xAA;
					buf[eWait2] = 0x55;
					nState = eRead;
				}
				else
				{
					buf[nState] = nRet;
					nState++;
				}
				recv_header = 0;
				break;
			default:
				recv_header = 0;
				if(nState>=eRead)
				{
					buf[nState] = nRet;
					nState++;
				}
				break;
		}
		if (nState==eReadComplete)
		{
			memcpy((void *)pMsgRecv,buf,sizeof(TUserMsgRecv));
			nState=eWaiting;
			return 1;
		}
#endif

#if 0
		if (nState==eWaiting)
		{
			if (nRet==0xAA)
			{
				recv_header = 1;
			}
			buf[nState]=nRet;
			nState++;
			continue;
		}

		if (recv_header)//(nState==eWait2)
		{
			if (nRet == 0xAA)
			{
				recv_header = 1;
				buf[nState]=nRet;
				nState++;
				continue;
			}

			//start receive
			if (nRet==0x55)
			{
//				buf[nState]=nRet;
				buf[eWaiting] = 0xAA;
				buf[eWait2] = 0x55;
				nState = eRead;
			}else{
				buf[nState]=nRet;
				nState++;
			}
			recv_header = 0;
			continue;
		}
		if(nState>=eRead)
		{
			if(nRet==0xAA)
			{
				recv_header = 1;
//				LOGT("SerialGetUserMsg Reset!\r\n");
//				nState=eWaiting;
//				buf[nState]=nRet;
//				nState++;
//				continue;
			}
			buf[nState]=nRet;
			nState++;
			if (nState==eReadComplete)
			{
				memcpy((void *)pMsgRecv,buf,sizeof(TUserMsgRecv));
				nState=eWaiting;
				return 1;
			}
		}
#endif
	}

	return 0;
}

// Add recv_header flag.If received a 0xaa,this flag is set to 1.
int SerialGetUserMsg422(TUserMsgRecv422 *pMsgRecv,int fd)
{
	char nRet=0;
	int len;
	int ret;

	static int nState=eWaiting;
	static char buf[RECV_USERMSGBUF422_SIZE+1];
	static int recv_header = 0;

	memset(&buf[nState],0,(sizeof(buf)-nState));

	while(1)
	{
		ret=read(fd,&nRet,sizeof(nRet));
		if(ret<0)
		{
			FUNC_EXIT
			return -1;
		}
		if(ret==0) {
			LOGT("read uart finished\r\n");
			FUNC_EXIT
			break;
		}
        //LOGT("get char 0x%02x\t",nRet);//zqh 2015年10月18日 modify，将422收到数据信息屏蔽，串口输出耗时，影响程序运行。
        //LOGT("nState = %d\r\n",nState);

#if 1
		if(nRet == 0xaa)
		{
			recv_header = 1;
		}
		else
		{
			if((recv_header)&&(nRet == 0x55))
			{
				buf[eWaiting] = 0xaa;
				nState = eWait2;
			}
			recv_header = 0;
		}

		if(nState >= eWait2)
		{
			buf[nState] = nRet;
			nState++;
		}

		if(nState >= e422ReadComplete)
		{
			memcpy((void *)pMsgRecv,buf,sizeof(TUserMsgRecv422));
			nState = eWaiting;
			return 1;
		}
#else
		if(nState>=e422ReadComplete)
		{
			LOGT("nState ERROR\r\n");
			nState = eWaiting;
			return 0;
		}

		switch(nRet)
		{
			case 0xAA:
				recv_header = 1;
				if(nState>=eRead)
				{
					buf[nState] = nRet;
					nState++;
				}
				break;
			case 0x55:
				if(recv_header == 1)
				{
					buf[eWaiting] = 0xAA;
					buf[eWait2] = 0x55;
					nState = eRead;
				}
				else
				{
					buf[nState] = nRet;
					nState++;
				}
				recv_header = 0;
				break;
			default:
				recv_header = 0;
				if(nState>=eRead)
				{
					buf[nState] = nRet;
					nState++;
				}
				break;
		}
		if (nState==e422ReadComplete)
		{
			memcpy((void *)pMsgRecv,buf,sizeof(TUserMsgRecv422));
			nState=eWaiting;
			return 1;
		}
#endif

#if 0
		if (nState==eWaiting)
		{
			if (nRet==0xAA)
			{
				recv_header = 1;
			}
			buf[nState]=nRet;
			nState++;
			continue;
		}
		if (recv_header)//(nState==eWait2)
		{
			if (nRet == 0xAA)
			{
				recv_header = 1;
				buf[nState]=nRet;
				nState++;
				continue;
			}

			//start receive
			if (nRet==0x55)
			{
//				buf[nState]=nRet;
				buf[eWaiting] = 0xAA;
				buf[eWait2] = 0x55;
				nState = eRead;
			}else{
				buf[nState]=nRet;
				nState++;
			}
			recv_header = 0;
			continue;
		}
		if(nState>=eRead)
		{
			if(nRet==0xAA)
			{
				recv_header = 1;
//				LOGT("SerialGetUserMsg Reset!\r\n");
//				nState=eWaiting;
//				buf[nState]=nRet;
//				nState++;
//				continue;
			}
			buf[nState]=nRet;
			nState++;
			if(nState==e422ReadComplete)
			{
				memcpy((void *)pMsgRecv,buf,sizeof(TUserMsgRecv422));
				nState=eWaiting;
				return 1;
			}
		}
#endif
	}
	return 0;
}

int SerialSendUserMsg(TUserMsgSend *pMsgSend)
{
	int ret;
	int bytecount;
	FUNC_ENTER
	bytecount = sizeof(TUserMsgSend);
	ret=write(STDOUT_FILENO,(void *)pMsgSend,bytecount);
	if(ret!= bytecount)
	{
		LOGE("SerialSendUserMsgErr :%d\r\n",ret);
		return -1;
	}
	return 1;
}

int SerialSendUserMsg422(TUserMsgSend *pMsgSend)
{
	int ret;
	int bytecount;
//	FUNC_ENTER
	bytecount = sizeof(TUserMsgSend);
	ret=write(rs422fd,(void *)pMsgSend,bytecount);
	if(ret!= bytecount)
	{
//		LOGE("SerialSendUserMsgErr :%d\r\n",ret);
		return -1;
	}
	return 1;
}

int GetUserMsg(TUserMsgRecv *pMsg)
{
	TUserMsgRecv tMsgRecv;
	int ret;
	int i;
	unsigned int checksum;
	unsigned char buf[20];

	FUNC_ENTER
	ret=SerialGetUserMsg(&tMsgRecv);

	if(ret<=0)
	{
		return ret;
	}

	checksum=0;
	memset(buf,0,sizeof(buf));
	memcpy(buf,&tMsgRecv,sizeof(tMsgRecv));
	for(i=0;i<RECV_USERMSGBUF_SIZE-1;i++)
	{
		checksum+=buf[i];
	}
	checksum&=0xff;
	if(checksum!=tMsgRecv.checksum)
	{
		ERR("%s:GetUserMsgErr checksum=0x%x\r\n",__func__,checksum);
		ERR("");
		return -1;
	}
	
	if((tMsgRecv.start[0]!=0xAA)||(tMsgRecv.start[1]!=0x55))
	{
		LOGE("GetUserMsgErr start=0x%x0x%x\r\n",tMsgRecv.start[1],tMsgRecv.start[0]);
		return -1;
	}
	
	memcpy((char *)pMsg,&tMsgRecv,sizeof(tMsgRecv));
	return 1;

}
int GetUserMsg422(TUserMsgRecv422 *pMsg,int fd)
{
	TUserMsgRecv422 tMsgRecv;
	int ret;
	int i;
	unsigned int checksum;
	unsigned char buf[20];

	FUNC_ENTER;
	ret=SerialGetUserMsg422(&tMsgRecv,fd);

	if(ret<=0)
	{
		return ret;
	}

	checksum=0;
	memset(buf,0,sizeof(buf));
	memcpy(buf,&tMsgRecv,sizeof(tMsgRecv));
	for(i=0;i<RECV_USERMSGBUF422_SIZE-1;i++)
	{
		checksum+=buf[i];
	}
	checksum&=0xff;

	if(checksum!=tMsgRecv.checksum)
	{
		LOGE("GetUserMsgErr checksum=0x%x,tMsgRecv.checksum=0x%x\r\n",checksum,tMsgRecv.checksum);
		return -1;
	}
	
	if((tMsgRecv.start[0]!=0xAA)||(tMsgRecv.start[1]!=0x55))
	{
		LOGE("GetUserMsgErr start=0x%x %x\r\n",tMsgRecv.start[1],tMsgRecv.start[0]);
		return -1;
	}

	memcpy((void *)pMsg,&tMsgRecv,sizeof(tMsgRecv));
	return 1;

}
int SendUserMsg(TUserReply *pReply)
{
	TUserMsgSend tMsgSend;
	int ret;
	int i;
	unsigned int checksum;
	unsigned char buf[20];
	unsigned char pvrState;
//	FUNC_ENTER;

	tMsgSend.start[0]=0xAA;
	tMsgSend.start[1]=0x66;
#if ENABLE_422_KEY_MSG   // ignore input status in old version
    checksum = getReplyStatus();
    pvrState = checksum & 0xff;
#else
	switch(pReply->ePvrStat)
	{
	case ePvrStop:
	case ePvrRec:
	case ePvrTime:
		pvrState=pReply->ePvrStat;
		break;
	default:
		LOGE("Pvr State Invalid\r\n");
		return -1;
	}
#endif
	tMsgSend.state=pvrState;
	checksum=0;
	memset(buf,0,sizeof(buf));
	memcpy(buf,&tMsgSend,sizeof(tMsgSend));
	for(i=0;i<SEND_USERMSGBUF_SIZE-1;i++)
	{
		checksum+=buf[i];
	}
	tMsgSend.checksum=checksum&0xff;
	ret=SerialSendUserMsg422(&tMsgSend);
	if(ret<0)
	{
		return ret;
	}
	return 1;
}

