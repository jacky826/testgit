#ifndef _UI_MSG_HEADER_
#define _UI_MSG_HEADER_

 /*各个任务通知TaskUI  消息定义
   */

/*消息类型定义*/
/*
按键消息:获得按键事件(PF或红外)
参数为按键键值，参见  key.h
*/
#define MSG_KEY_EVENT		0x0

/*
解码时消息：
解码器报告自身状态，参数为解码器状态
*/
#define MSG_DECODER_EVENT	0x1
#define MSG_PARA_DEC_FILEEND	0x1
#define MSG_PARA_DEC_FILEBEGIN	0x2
#define MSG_PARA_DEC_STOP 0x0
/*
编码时消息：
编码器报告自身状态，参数为编码器状态
如果是MSG_PARA_ENC_STOP表示编码器正常停止
其他值表示编码器异常停止的具体错误号
*/
#define MSG_PARA_ENC_STOP	0x0
//#define MSG_PARA_ENC_RESTART	0x1

#define MSG_ENCODER_EVENT	0x2

/*
定时器消息：
RTC  定时器产生中断，报告UI，唤醒UI 作定时存储检查，参数待定
*/
#define MSG_RTC_EVENT		0x3

/*
搜台任务消息：
搜台任务要求TaskUI响应事件,参数待定
*/
#define MSG_AUTOSCAN_EVENT	0x4

#define MSG_AUTOSCAN_START		0x00
#define MSG_AUTOSCAN_POS			0x01//搜台更新位置
#define MSG_AUTOSCAN_COMPLETE	0x02//搜台完成
#define MSG_AUTOSCAN_FIND			0x03//发现一个台

/*
USB 检测消息：
USB Host 检测到插入或退出
*/
#define MSG_USBDEC_EVENT	0x5

#define USBEVENT_DATA_DISK(d)	((d&0xff00)>>8)
#define USBEVENT_DATA_VOL(d)	(d&0xff)

#define USBEVENT_DATA(disk,vol)	(((disk&0xff)<<8)|(vol&0xff))

#define USBEVENT_DATA_ERRDISK	(0xff)


/*消息参数定义*/

//#include <error.h>

/*消息处理函数配置*/

#define UICODEC_MSG_FIFO_CNT			8


/*消息处理函数*/

/*Init_UI_Message
	初始化UI_Message使用的资源,需要在OSStart之前调用
	返回值1表示成功
			0 表示失败
 */
int Init_UI_Message(void);
int Deinit_UI_Message(void);

/*Post_UI_Message
	向TaskUI发送消息,nMsgType 为消息类型,nMsgData 为消息参数
	返回值1表示成功,TaskUI任务会响应消息
			0 表示不成功,可能由于消息不合法或队列满,TaskUI任务不会响应消息
 */
int Post_UI_Message(unsigned short nMsgType,unsigned short nMsgData);

#endif

