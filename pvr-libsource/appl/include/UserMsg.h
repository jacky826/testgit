#ifndef __USER_MESSAGE_HEAD__
#define __USER_MESSAGE_HEAD__

#include <sys/time.h>
#include "config.h"

#define KEYENTER		(1<<1)
#define KEYSTOP			(1<<0)
#define KEYDOWN			(1<<2)//(1<<2)
#define KEYUP			(1<<3)//(1<<3)
#define KEYFORWARD		(1<<4)//(1<<5)
#define KEYREWIND		(1<<6)//(1<<4)  //haiqing
#define KEYPLAY			(1<<5)//(1<<6)
#define KEYREC			(1<<7)

typedef struct {
	unsigned int nKeyVal;
	time_t tTime;
}TUserMsg;

typedef enum{
	ePvrStop = 0x00,
	ePvrRec = 0x01,
	ePvrTime = 0x11,
    ePvrInvalid
}EPVRSTAT;

typedef struct {
	EPVRSTAT ePvrStat;
}TUserReply;


#define MsgTestKey(pMsg, nAccept) \
do {\
	(pMsg)->nKeyVal &= (nAccept);\
}while (0)

struct UserMsgTime{
	char year[2];
	char mon;
	char day;
	char hour;
	char min;
	char sec;
};

typedef struct{
    unsigned char OSD_x[2];
    unsigned char OSD_y[2];
}OSDPos;

typedef struct _tagUserMsgRecv{
	char start[2];
	char keyVal;
	//struct UserMsgTime msgTime;
	char checksum;
}TUserMsgRecv;

typedef struct _tagUserMsgRecv422{
	char start[2];
	char keyVal;
    union {
        struct UserMsgTime msgTime;
        OSDPos OSD_Pos;
    }data;
	char checksum;
}TUserMsgRecv422;
typedef struct _tagUserMsgSend{
	char start[2];
	char state;
	char checksum;
}TUserMsgSend;

#define DEFAULT_USERMSGBUF_SIZE	20
#define RECV_USERMSGBUF_SIZE	4
#define RECV_USERMSGBUF422_SIZE	11
#define SEND_USERMSGBUF_SIZE	4
typedef struct _tagUserMsgBuf{
	int bytecount;
	union buffer{
		char recvBuf[RECV_USERMSGBUF_SIZE];
		char defBuf[DEFAULT_USERMSGBUF_SIZE];
	}bufdata;
}TUserMsgBuf;

int GetUserMsg(TUserMsgRecv *pMsg);
int SendUserMsg(TUserReply *pReply);
int GetUserMsg422(TUserMsgRecv422 *pMsg,int fd);//haiqing add
int InitComMsg(void);
int DeInitComMsg(void);

// 0121-2013 增加 OSD 图标左上角起始x,y坐标
extern unsigned short CUR_STATUS_X0;
extern unsigned short CUR_STATUS_Y0;

// 2014-04 增加系统状态回复
extern unsigned int getReplyStatus();


#endif

