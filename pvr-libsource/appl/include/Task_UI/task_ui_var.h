#ifndef _TASK_UI_VAR_HEADER_
#define _TASK_UI_VAR_HEADER_

#include "../all_data.h"
#include "../File_manager.h"
#include "../pvr_types.h"

/*Task UI's variables invoke only by Task_UI*/

/*UI System*/
/*
 * -------------------------Misc
 */

//! UI Message �ķ���״̬����
typedef enum {
	MsgNocare=0,			//Msg not handled at this level
	MsgHandled=1,			//Msg have been handled and should be terminated
	MsgNeedDispatch=2	//Msg have been handled and need dispatch to next level
} TMsgHandleStat;

/*OSD */
/*
 ----	STATIC OSD USER 
 */
typedef int (OSD_Restore_fn)(void *pUser);
typedef int (OSD_Refresh_fn)(void *pUser);
typedef int (OSD_Close_fn)(void *pUser);

typedef struct{
	//Fixed at design time
	OSD_Refresh_fn *OSD_Refresh;//Update part
	OSD_Close_fn	 *OSD_Close;//close
	OSD_Restore_fn *OSD_Restore;//Redraw all

	int nMenu;//For check
	int nPos;//for Mask & Restore
	
	//Can change at runtime
	int TimerInterval;
	void *pUser;
	
	//Change at runtime atomatic
	int nNextOSD;//OSD current menu show in next 
}TEF_OSD_STRUCT;


/*Timers */
struct TIMER_STRUCT_tag;

//! timer �����ӿ�
typedef TMsgHandleStat (TimerTO_fn)(struct TIMER_STRUCT_tag *timer);

//! timer ʹ����غ궨��
#define DVR_TIMER_DIS					(0)			// ʹ���ܶ�ʱ�� 

//! timer ���Ͷ���,�󲿷�δʹ��
#define TIMER_NUMKEY					(1)
#define TIMER_AUTOSCAN					(2)
#define TIMER_NUMKEYINPUT				(3)
#define TIMER_MESSAGEBOX				(4)         /// messagebox ����,��������ʹ��

#define TIMER_OSDDISPLAY_TMP			(5)
#define TIMER_OSDDISPLAY_STATIC		(6)

#define TIMER_REPEAT_AB				(7)

#define TIMER_DEVSCAN				(8)             /// ÿ���豸״̬���timer

/*
 * UIϵͳ���õ���timer�ṹ��
 */
typedef struct TIMER_STRUCT_tag {
    INT32U TimerCnt;                // �뼶����������¼��ʼ��ʱ��ϵͳʱ��,δʹ��
    unsigned int TimerStatus;       // ��ʱ��״̬��0��ʾ�أ�1��ʾ��
    int TimerDelay;                 // ��ʱ���趨ʱ��
    int TimerInterval;              // ��ʱ����ʱʱ��

    TimerTO_fn	*TO_fn;             //��ʱ����ʱ�������
}TEF_TIMER_STRUCT;

/*System Stat*/
//define for TEF_SYS_STATE.CurState
//--------------------------------------------------------------------------//
//	purpose:ÿһλ��ʾһ��������״̬
//	bit31-------------------------------------------------------------bit0
//	--------------------------------------------256-128-64-32-16-8-4-2-1
//--------------------------------------------------------------------------//
#define SYS_STAT_IDLE					0x00000000
#define IS_SYS_STATE_IDLE(x)			((x)==SYS_STAT_IDLE)		

//! ϵͳ��ǰ����״̬��־������CurState
#define SYS_BIT_RECORDING_ONE_CLICK     (1<<1)  /// ¼��״̬
#define SYS_BIT_PLAYBACK                (1<<2)  /// ����״̬
#define SYS_BIT_GETTIME                 (1<<3)  /// ��ҪУʱ״̬
#define SYS_BIT_POWEROFF                (1<<7)  /// δʹ��

//ZQH 2015��9��24�� add������ļ����״̬��־λ
#define SYS_BIT_FILEBROSWER             (1<<11)  ///�ļ����״̬
#define SYS_BIT_SYSTEMUPDATE            (1<<12)  ///ϵͳ�����˵�
#define SYS_BIT_SYSTEMFUNCTION          (1<<13)  ///ϵͳ���ܲ˵�

//! ϵͳ��ǰ�豸״̬��־����ʾ���豸�Ƿ���λ�Լ�����״̬
#define SYS_BIT_CFCard_REMOVE           ( 1<<1 ) /// CF������λ
#define SYS_BIT_UDisk_REMOVE            ( 1<<2 ) /// U�̲���λ
#define SYS_BIT_SDCard_REMOVE           ( 1<<3 ) /// SD������λ

#define SYS_BIT_CFCard_Mount_Flag       ( 1<<4 ) /// CF���Ѽ���
#define SYS_BIT_UDisk_Mount_Flag        ( 1<<5 ) /// U���Ѽ���
#define SYS_BIT_SDCard_Mount_Flag       ( 1<<6 ) /// SD���Ѽ���

//zqh 2015��10��18�� add�� ��Ӳ�̿ռ�С��100MBʱ������ɾ���ļ�����
#define SYS_BIT_Disk_Full               ( 1<<7 ) /// CF��¼����������Ӳ�̿ռ�С��100MB����ʼ��1

#define SYS_BIT_CFCard_MOUNTING         ( 1<<8 ) /// CF�����ع�����״̬
#define SYS_BIT_CFCard_FORMAT           ( 1<<9 ) /// CF�����ڸ�ʽ��
#define SYS_BIT_CFCard_FSCK             ( 1<<10 ) /// CF�������ļ�ϵͳ���
#define SYS_BIT_CFCard_RESET            ( 1<<11 ) /// CF�����ڸ�λ
#define SYS_BIT_CFCard_NEEDFORMAT       ( 1<<12 ) /// CF����Ҫ��ʽ��

//zqh 2015��10��18�� add�� ���ڵ�Ӳ�̿ռ�С��500Mʱ�����û�������Ϣ��
#define SYS_BIT_CFCard_FreeSpaceWarning ( 1<<13 ) /// ��Ӳ�̿ռ�С��500Mʱ����ʼ��1��


//! ϵͳ�ظ�״̬��Ϣ��־
//! zqh 2015��10��18�� modify����״̬λ�޸����¡�
//! b2b1b0��000/001/010/011/100/101=����/¼��/�ط�/�ļ����/ϵͳ��������/ϵͳ���ܽ���
//! b3����Уʱ����0/1=Уʱ�ɹ�/����Уʱ��
//! b4����U�̼���״̬��0/1=U�̼���/ U��δ���أ�
//! b5����¼��״̬ 0�ֶ�  1�Զ�¼��(Ŀǰ�ȱ���)
//! b6����Ӳ�̿ռ�С��500Mʱ�����û����о��棬��״̬λ��1����Ӳ�̿ռ�С��100Mʱ��ʼɾ���ļ���
//! b7����Ӳ�̼���״̬�Ƿ�������1��ʾ���ϣ� 0��ʾ��������

//!b2b1b0
#define REPLY_STAT_BIT_STANDBY          (0x00)  /// ����״̬��־λ
#define REPLY_STAT_BIT_RECORD           (0x01)  /// ¼��״̬��־λ
#define REPLY_STAT_BIT_PLAY             (0x02)  /// ����״̬��־λ
#define REPLY_STAT_BIT_FILEBROSWER      (0x03)  /// �ļ����״̬��־λ
#define REPLY_STAT_BIT_SYSTEMEUPDATE    (0x04)  /// ����ϵͳ��������
#define REPLY_STAT_BIT_SYSTEMFUNC       (0x05)  /// ����ϵͳ���ܽ���
//!b3
#define REPLY_STAT_BIT_GETTIME          (1<<3)  /// ��ȡʱ���־λ
//!b4
#define REPLY_STAT_BIT_UDISK            (1<<4)  /// U���Ѽ��ر�־λ
//!b5 ����λ��Ϊ�Զ����ֶ�¼��״̬Ϊ��

//!b6 �������̿ռ�С��500MBʱ��1
#define REPLY_STAT_BIT_CFCARDFULL       (1<<6)
//!b7 �������̼��ع���ʱ��λ��1
#define REPLY_STAT_BIT_CFCARDERROR      (1<<7)
//TODO ������־λ�����

//! ȫ��ϵͳ״̬��־����
typedef struct _SYS_STATE
{
    INT32U CurState;    /// ϵͳ����״̬
    INT32U DevState;    /// ϵͳ�豸״̬
}TEF_SYS_STATE;

//! �ļ��ط���Ϣ�ṹ��,�ļ��ط�״̬�벥���ٶȵ�
typedef struct {
    //! �ļ���Ϣ,��ʼ����ʱ,��ֵ�ļ���Ϣ
	PlayerInfo sFileInfo;//May be call from blank , so Fill this at init
	
#define PLAYBACK_PLAY	0
#define PLAYBACK_PAUSE	1
    //! �ļ�����״̬,��ͣ/����
    int PlayState;

    //! ?
    int PlayFastorSlow;// nFastorSlow=1 fast -1 slow, 0 normal

    //! �ļ������ٶȲ���
    int PlaySpeed;

    //! ���ŷ���,������
	enum PlayerOrient PlayOrient;

/* Goto specified time */       /// δʹ��
#define GOTO_OUT		0
#define GOTO_IN			1
#define GOTO_TIMELEN	6
	int GotoState;//have enter the GOTO menu=1,out of the GOTO menu=0
	char GotoSpecTime[8];//only use [0]--[5], eg.01:23:45, initialized to -1
	int BytesToFillZero;//0--GOTO_TIMELEN
	DATE_TIME TotalPlayTime;

#define PLAYMODE_NORMAL	0
#define PLAYMODE_ABREP		1
#define PLAYMODE_REPFILE	2

	int PlayMode;
#define NOVALID_PLAYPOS	(-1)	
	int PLAY_REP_A;
	int PLAY_REP_B;
}TEF_PLAYBACK_DATA;


/*
 * -------------------------Message Box Call Back
 */
typedef int (MessageboxCB_fn)(void *pUser);
typedef int (MessageboxDeinit_fn)(int nOKFaile);

//! message box ����״̬����
#define MESSAGESTAGE_IDLE				0       /// δ����
#define MESSAGESTAGE_RUN				1       /// ������ʾ

#define MESSAGESTAGE_OK					2       /// �ѵ�ȷ��
#define MESSAGESTAGE_FAIL				3       /// �ѵ��
#define MESSAGESTAGE_CANCEL				4       /// ��ȡ��

//message used in dev state change
//MSG priority
#define MESSAGESTAGE_CFCARDREMOVE		1       /// CF������λ��־λλ��
#define MESSAGESTAGE_CFCARDDETECT		2       /// ��⵽cf����־λλ��
#define MESSAGESTAGE_UDISKREMOVE		3       /// U�̲���λ��־λλ��
#define MESSAGESTAGE_UDISKDETECT		4       /// ��⵽U�̱�־λλ��
#define MESSAGESTAGE_SDREMOVE			5       /// sd������λ��־λλ��
#define MESSAGESTAGE_SDDETECT			6       /// ��⵽sd����־λλ��
#define MESSAGESTAGE_DISKFULL			7       /// ����¼��������־λλ��

//Dev MSG ��־λ����
#define MESSAGE_DEV_CFCARDREMOVE		(1<<MESSAGESTAGE_CFCARDREMOVE)  /// CF������λ��־λ
#define MESSAGE_DEV_CFCARDDETECT		(1<<MESSAGESTAGE_CFCARDDETECT)  /// ��⵽cf����־λ
#define MESSAGE_DEV_UDISKREMOVE			(1<<MESSAGESTAGE_UDISKREMOVE)   /// U�̲���λ��־λ
#define MESSAGE_DEV_UDISKDETECT			(1<<MESSAGESTAGE_UDISKDETECT)   /// ��⵽U�̱�־λ
#define MESSAGE_DEV_SDREMOVE			(1<<MESSAGESTAGE_SDREMOVE)      /// sd������λ��־λ
#define MESSAGE_DEV_SDDETECT			(1<<MESSAGESTAGE_SDDETECT)      /// ��⵽sd����־λ
#define MESSAGE_DEV_DISKFULL			(1<<MESSAGESTAGE_DISKFULL)      /// ����¼��������־λ

#define MAX_DEV_MSG_CNT					(MESSAGESTAGE_DISKFULL+1)

//! ��ͬ���ļ�����״̬��־,���ݸ�״̬,��ʾ��ͬ�Ŀ�����ʾ��Ϣ
//! ���ڿ������/ʧ�ܺ�,�����Ӧ״̬,���ж�Ӧ����(ע��ȡ��֮��Ĳ���)
#define MESSAGESTAGE_COPY_TO_UDISK		 1
#define MESSAGESTAGE_COPY_TO_CFCARD		 2
#define MESSAGESTAGE_UPDATE_BACKUP		 3
#define MESSAGESTAGE_UPDATE_UPDATE		 4
#define MESSAGESTAGE_UPDATE_COPYBACK	 5
#define MESSAGESTAGE_CANCEL_COPYTOUDISK  6//zqh 2015��3��30�� modify����ӱ�־�������ӵ�������U�̸����ļ���ʱ����ȡ�����Ƶ����
#define MESSAGESTAGE_CANCEL_BACKUP		 7
#define MESSAGESTAGE_CANCEL_UPDATE		 8
#define MESSAGESTAGE_CANCEL_COPYTOCFCARD 9//zqh 2015��3��30�� modify����ӱ�־��������U��������̸����ļ������С�����ȡ�����Ƶ�����
/*
 * -------------------------Menus
 */
/*Message handler*/
struct UI_Menu_tag;


typedef TMsgHandleStat (Pvr_UImessage_fn)(struct UI_Menu_tag *pMenu,unsigned short nMsgType,unsigned short nMsgData);

/*Windows structure */
/* Windows functions
 * return value: 0-failed 1-successful
 */
typedef int (MenuInit_fn)(struct UI_Menu_tag *pMenu);		//Invoke when menu need init, before system menu change active

typedef int (MenuOpen_fn)(struct UI_Menu_tag *pMenu);	//Invoke when menu need open, will refresh menu automatic, will call after system change active
typedef int (MenuClose_fn)(struct UI_Menu_tag *pMenu);	//Invoke when menu need close, some menu need close timer 

typedef int (MenuRefresh_fn)(struct UI_Menu_tag *pMenu);	//function to re produce the menu in GUI buffer
typedef int (MenuRefreshItem_fn)(struct UI_Menu_tag *pMenu,int nIdx,int nFocus);	//Invoke when change active button, current we just call refresh to redraw menu
											//nEnable is the button enable ; nFocus is Button get Focus
/*
 * ͼ�δ��ڽṹ�嶨��
 */
typedef struct UI_Menu_tag{
    //! ��ǰ�����б�ѡ�е�button id
    int nButtonIdx;

    //! ��ǰ���ڵĸ�����id
    int nFromMenuIdx;
/*Handler*/
    //! ���ڰ�����Ϣ������
    Pvr_UImessage_fn *msgHandle_fn;
/*menu init & self api*/
    //! �����ڱ�����ʼ����������Щ����£���Щ��������Ҫ��ʼ������ʱ�᲻���øú����л�
    MenuInit_fn *menuInit;

    //! ���ڿ�ʼ���ƺ�����ֻ����menuRefresh����ʱ�������ÿգ�����Ĭ�ϻ��ƺ���
    MenuOpen_fn *menuOpen;

    //! �л�����һ������ǰ����Ҫ���õı�������ʼ������
	MenuClose_fn *menuClose;

    //! ����ˢ�º���
	MenuRefresh_fn *menuRefresh;

    //! ������button��item�ػ溯��
	MenuRefreshItem_fn *menuRefreshItem;

    //! ����id
    int nMenuIdx;

//Static OSD
    //! OSD��ʾʱʹ�ñ���,Ĭ�ϴ��ڲ�ʹ��
	int nShowOSDIdx;//The First Show OSD idx 
}UI_Menu_t;

#include "menudef.h"
#include "osddef.h"
#include "imageindex.h"

typedef struct _OSD_ALPHA_RECT {
	DWORD Left;
	DWORD Top;
	DWORD Width;
	DWORD Height;
	WORD Alpha; // (0:Cover -- 256:Transparent)
	WORD TransColor; // Enable transparence color (black)
} OSD_ALPHA_RECT;

/*
 * UIϵͳȫ��״̬�����ṹ��
 */
typedef struct{
    //! ��ǰactive����id
    int nMenuIdx;

    //! ϵͳ���д�����Դָ��
    UI_Menu_t Menus[UI_MENU_COUNT];

    /*Timers*/
    //! messageboxδ���յ������ĵ���ʱ��
    TEF_TIMER_STRUCT sKeyTimer;

    //! �ط�timer,δʹ��
    TEF_TIMER_STRUCT sPlayBackTimer;

    //! �ط�ʱosd��ʾtimer,ĿǰӦ��δʵ������,��Ҫ��������
    TEF_TIMER_STRUCT sOSDTimer[OSDPOS_COUNT];

    //! 1s �豸���timer,���ڼ���ļ�¼�ƴ�С,cf����λ״̬�Ľ�����ʾ,led״̬����
    TEF_TIMER_STRUCT sDevScanTimer;

    //! ��ʾ�豸�����ʾ��Ϣʱ,Ҫ��ʾ�ĵ�ǰ�豸״̬��Ϣ��id
    unsigned int nDevCurStatusMsg;
    /*OSD Temp Use*/
    //! �ط�osdʹ�ýṹ��,�����Լ�ʹ�÷���������
    TEF_OSD_STRUCT sOSDStructStatic[UI_OSD_COUNT];

    //! ˢ��ʱ�õ��ľ�������
    OSD_ALPHA_RECT RefreshRect[OSDPOS_COUNT];		//used in the function of DisplayBlendRefresh()

    //! osd bit map ˢ��ʱʹ��,����ԭ�����ô�����
    unsigned int nOSDBitMap;//Current OSD display position bitmap

    /*System Aware */
    //! ��ǰϵͳ״̬��־
    TEF_SYS_STATE SysState;

    //! u��,cf��/�����̵ȴ洢����
    char Vol_Disk[SUPPORT_DISK_CNT][MAX_FS_PATH];

    //file_manage.c
    //! ����ļ���,δʹ��!
    int max_filecount_ever;

    //! �ļ��б���������,Ĭ���ļ��б�ֻ������Ƶ�ļ�,������Ŀ¼
    unsigned long list_contain;

    /*Application Data for each menu*/
    // 0Blank
    //! ��ǰ�����ļ���,�����ļ�����,ɾ��,���ŵ�
    char sRecFilename[MAX_FS_PATH];

    //! δʹ��
    int nRecordPause;//0-not pause 1 pause

    //! ��ǰʹ�ô洢¼���ļ��Ĵ洢����.Ĭ������Ϊcf��
    int nFileSelectDisk;

    // 6SystemSetting
    // 7FileBrowsers
    //! ��ǰ�ļ���������ļ��б��һ���ļ���index
    int FileBrowserIndex;		//0--TotalNum-1 ,��ǰҳ���е�Index

    //! ��ǰѡ���ļ��ڵ�ǰ�ļ���������е�λ��
    int FileBrowserSelFile;//0-FilePerBloc-1

    //! CF���е��ļ��б�list
    struct FileListBlock FileBrowserdirlist;

    //! ��U�̿����ļ���CF��ʱ,��ʾ�ļ����׸��ļ����ļ��б����index
    int FileBrowserIndex_UDisk;		//0--TotalNum-1 ,��ǰҳ���е�Index

    //! ��U�̿����ļ���CF��ʱ,��ʾ�ļ���,��ǰѡ����ļ���λ��
    int FileBrowserSelFile_UDisk;//0-FilePerBloc-1

    //! U���е��ļ��б�list,��U�̿����ļ���CF����ʹ��
    struct FileListBlock FileBrowserdirlist_UDisk;

    //! �ļ�����ʱ(����ϵͳ����),Դ�ļ����ļ�·��
    char sFileCopyFrom[MAX_FS_PATH];

    //! �ļ�����ʱ,Ŀ���ļ�·��
    char sFileCopyTo[MAX_FS_PATH];

    //! �ļ���������״̬,�ο� MESSAGESTAGE_XXX
    int nFileCopyToStage;	// DoFileCopyTo menu message stage

    //! �ļ��ط���Ϣ�ṹ��
    TEF_PLAYBACK_DATA sPlayback;

    //! �������Ϣ�ַ���
    char CODEC_ERRMSG[CODEC_DEBUG_MSGLEN];

    //! message box ��ʾ��Ϣ�ַ���ָ��,��ʼ��messagebox��ʱ��ֵ
    char *pMessageDisplay;

    //! message box ȡ��ʱ����ʾ��Ϣ�ַ���ָ��,message box��ʾʱ,���˳�����ʾ����Ϣ
    char *pMessageFail;

    //! message box ȷ��ʱ����ʾ��Ϣ�ַ���ָ��,message box��ʾʱ,��ȷ�ϼ���ʾ����Ϣ
    char *pMessageOK;

    //! message box ��ʱʱ��,��ʱ֮��,�Զ�ִ�� cancel ����
    int nMessageTimeout;//0-Any Key return 1-OK call fn

    //! message box ȷ�Ϻ�ִ�е�callback ����ָ��
    MessageboxCB_fn *pMessageUser_fn;

    //! message box ȡ��ʱִ�е�callback ����ָ��
    MessageboxCB_fn *pMessageCancel_fn;

    //! message box ȷ��/ȡ��ʱ,ִ��callback �����Ĵ������
    void *pMessageUser;

    //! message box ����ʼ��ʱ,ִ�е�callback ����ָ��
    MessageboxDeinit_fn *pMessageDeinit_fn;

    //! message box ����״̬
    int nMessageStage;

    /*Application Data for System Setting*/
    //! Ӧ�ó�����������,��ʼ��ʱ,�������ļ�(�����)����
    ALL_DATA allData;

    //! �������BMP��C Stream������
    unsigned char  *BMPStream;
}UI_System_t;


/*UI System defination*/
//! ȫ��ϵͳ��������
extern UI_System_t *pUISystem;

#include "../my_spinit.h"
#include "../my_language.h"
#include "ui_misc.h"
#include "osd_misc.h"


#endif

