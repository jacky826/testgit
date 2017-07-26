/*
----------------------------------------------------------------------
File		:	UI_Message.c
Purpose	:	UI_Message Init/Post/Query
----------------------------------------------------------------------
Version-Date			:	2007.2.2
Author-Explanation		:	Shadow
----------------------------------------------------------------------
*/
#include <string.h>

#include <config.h>	//Config for platform
#include <UI_Message.h>
#include <Task_UI/task_ui_var.h>
#include <ti/sdo/dmai/Fifo.h>
#include "../../pvr_debug.h"
#include <fcntl.h>

#define MSG_TYPE(x)	(((x)&0xffff0000)>>16)
#define MSG_DATA(x)	((x)&0xffff)

#define GENERATE_MSG(t,d)	((((t)&0xffff)<<16)|((d)&0xffff))

/*Init_UI_Message
	初始化UI_Message使用的资源
	返回值1表示成功
			0 表示失败
 */

Fifo_Handle  hmsgFifo=NULL;
int msg_fd=-1;
typedef struct Fifo_Object_hank {
    pthread_mutex_t mutex;
    Int             numBufs;
    Int16           flush;
    Int             pipes[2];
} Fifo_Object;


int Init_UI_Message(void)
{
	int read_val,ret;
	Fifo_Attrs          fAttrs;

	memset(&fAttrs,0,sizeof(fAttrs));
	hmsgFifo = Fifo_create(&fAttrs);
	if(hmsgFifo == NULL) {
		LOGE("%s: create fifo failed\n",__func__);
		request2reboot();
		return -1;
	}

	read_val=fcntl(((Fifo_Object *)hmsgFifo)->pipes[0],F_GETFL,0);
	ret = fcntl(((Fifo_Object *)hmsgFifo)->pipes[0],F_SETFL,read_val|O_NONBLOCK);
	
	if(ret<0) {
		LOGE("%s: msg set0 noneblock failed\n",__func__);
		request2reboot();
		return -1;
	}

	read_val=fcntl(((Fifo_Object *)hmsgFifo)->pipes[1],F_GETFL,0);
	ret = fcntl(((Fifo_Object *)hmsgFifo)->pipes[1],F_SETFL,read_val|O_NONBLOCK);
	
	if(ret<0) {
		LOGE("%s: msg set1 noneblock failed\n",__func__);
		request2reboot();
		return -1;
	}
	msg_fd = ((Fifo_Object *)hmsgFifo)->pipes[0];
	LOGT("%s: msg_fd=%d\n",__func__,msg_fd);
	return 1;
}

int Deinit_UI_Message(void)
{
	if(hmsgFifo!=NULL) {
		Fifo_delete(hmsgFifo);
		hmsgFifo=NULL;
	}
	return 1;
}
/*Post_UI_Message
	向TaskUI发送消息,nMsgType 为消息类型,nMsgData 为消息参数
	返回值1表示成功,TaskUI任务会响应消息
			0 表示不成功
 */
 int Post_UI_Message(unsigned short nMsgType,unsigned short nMsgData){
	unsigned int nMsg;

	if ((nMsgType!=MSG_DECODER_EVENT)&&(nMsgType!=MSG_ENCODER_EVENT)){
		return 0;
	}
	
	nMsg=GENERATE_MSG(nMsgType,nMsgData);
	if(Dmai_EOK != Fifo_put(hmsgFifo,nMsg)) {
		LOGE("%s: write fifo failed\n",__func__);
		request2reboot();
	}
	
	return 1;
}
