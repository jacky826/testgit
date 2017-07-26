#ifndef _UI_MSG_HEADER_
#define _UI_MSG_HEADER_

 /*��������֪ͨTaskUI  ��Ϣ����
   */

/*��Ϣ���Ͷ���*/
/*
������Ϣ:��ð����¼�(PF�����)
����Ϊ������ֵ���μ�  key.h
*/
#define MSG_KEY_EVENT		0x0

/*
����ʱ��Ϣ��
��������������״̬������Ϊ������״̬
*/
#define MSG_DECODER_EVENT	0x1
#define MSG_PARA_DEC_FILEEND	0x1
#define MSG_PARA_DEC_FILEBEGIN	0x2
#define MSG_PARA_DEC_STOP 0x0
/*
����ʱ��Ϣ��
��������������״̬������Ϊ������״̬
�����MSG_PARA_ENC_STOP��ʾ����������ֹͣ
����ֵ��ʾ�������쳣ֹͣ�ľ�������
*/
#define MSG_PARA_ENC_STOP	0x0
//#define MSG_PARA_ENC_RESTART	0x1

#define MSG_ENCODER_EVENT	0x2

/*
��ʱ����Ϣ��
RTC  ��ʱ�������жϣ�����UI������UI ����ʱ�洢��飬��������
*/
#define MSG_RTC_EVENT		0x3

/*
��̨������Ϣ��
��̨����Ҫ��TaskUI��Ӧ�¼�,��������
*/
#define MSG_AUTOSCAN_EVENT	0x4

#define MSG_AUTOSCAN_START		0x00
#define MSG_AUTOSCAN_POS			0x01//��̨����λ��
#define MSG_AUTOSCAN_COMPLETE	0x02//��̨���
#define MSG_AUTOSCAN_FIND			0x03//����һ��̨

/*
USB �����Ϣ��
USB Host ��⵽������˳�
*/
#define MSG_USBDEC_EVENT	0x5

#define USBEVENT_DATA_DISK(d)	((d&0xff00)>>8)
#define USBEVENT_DATA_VOL(d)	(d&0xff)

#define USBEVENT_DATA(disk,vol)	(((disk&0xff)<<8)|(vol&0xff))

#define USBEVENT_DATA_ERRDISK	(0xff)


/*��Ϣ��������*/

//#include <error.h>

/*��Ϣ����������*/

#define UICODEC_MSG_FIFO_CNT			8


/*��Ϣ������*/

/*Init_UI_Message
	��ʼ��UI_Messageʹ�õ���Դ,��Ҫ��OSStart֮ǰ����
	����ֵ1��ʾ�ɹ�
			0 ��ʾʧ��
 */
int Init_UI_Message(void);
int Deinit_UI_Message(void);

/*Post_UI_Message
	��TaskUI������Ϣ,nMsgType Ϊ��Ϣ����,nMsgData Ϊ��Ϣ����
	����ֵ1��ʾ�ɹ�,TaskUI�������Ӧ��Ϣ
			0 ��ʾ���ɹ�,����������Ϣ���Ϸ��������,TaskUI���񲻻���Ӧ��Ϣ
 */
int Post_UI_Message(unsigned short nMsgType,unsigned short nMsgData);

#endif

