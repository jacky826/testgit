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

//! UI Message 的返回状态定义
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

//! timer 函数接口
typedef TMsgHandleStat (TimerTO_fn)(struct TIMER_STRUCT_tag *timer);

//! timer 使用相关宏定义
#define DVR_TIMER_DIS					(0)			// 使不能定时器 

//! timer 类型定义,大部分未使用
#define TIMER_NUMKEY					(1)
#define TIMER_AUTOSCAN					(2)
#define TIMER_NUMKEYINPUT				(3)
#define TIMER_MESSAGEBOX				(4)         /// messagebox 类型,弹出窗口使用

#define TIMER_OSDDISPLAY_TMP			(5)
#define TIMER_OSDDISPLAY_STATIC		(6)

#define TIMER_REPEAT_AB				(7)

#define TIMER_DEVSCAN				(8)             /// 每秒设备状态检测timer

/*
 * UI系统中用到的timer结构体
 */
typedef struct TIMER_STRUCT_tag {
    INT32U TimerCnt;                // 秒级计数器，记录开始计时的系统时间,未使用
    unsigned int TimerStatus;       // 定时器状态，0表示关，1表示开
    int TimerDelay;                 // 定时器设定时间
    int TimerInterval;              // 定时器超时时间

    TimerTO_fn	*TO_fn;             //定时器超时处理程序
}TEF_TIMER_STRUCT;

/*System Stat*/
//define for TEF_SYS_STATE.CurState
//--------------------------------------------------------------------------//
//	purpose:每一位表示一个独立的状态
//	bit31-------------------------------------------------------------bit0
//	--------------------------------------------256-128-64-32-16-8-4-2-1
//--------------------------------------------------------------------------//
#define SYS_STAT_IDLE					0x00000000
#define IS_SYS_STATE_IDLE(x)			((x)==SYS_STAT_IDLE)		

//! 系统当前运行状态标志，设置CurState
#define SYS_BIT_RECORDING_ONE_CLICK     (1<<1)  /// 录制状态
#define SYS_BIT_PLAYBACK                (1<<2)  /// 播放状态
#define SYS_BIT_GETTIME                 (1<<3)  /// 需要校时状态
#define SYS_BIT_POWEROFF                (1<<7)  /// 未使用

//ZQH 2015年9月24日 add，添加文件浏览状态标志位
#define SYS_BIT_FILEBROSWER             (1<<11)  ///文件浏览状态
#define SYS_BIT_SYSTEMUPDATE            (1<<12)  ///系统升级菜单
#define SYS_BIT_SYSTEMFUNCTION          (1<<13)  ///系统功能菜单

//! 系统当前设备状态标志，表示各设备是否在位以及加载状态
#define SYS_BIT_CFCard_REMOVE           ( 1<<1 ) /// CF卡不在位
#define SYS_BIT_UDisk_REMOVE            ( 1<<2 ) /// U盘不在位
#define SYS_BIT_SDCard_REMOVE           ( 1<<3 ) /// SD卡不在位

#define SYS_BIT_CFCard_Mount_Flag       ( 1<<4 ) /// CF卡已加载
#define SYS_BIT_UDisk_Mount_Flag        ( 1<<5 ) /// U盘已加载
#define SYS_BIT_SDCard_Mount_Flag       ( 1<<6 ) /// SD卡已加载

//zqh 2015年10月18日 add， 当硬盘空间小于100MB时，进行删除文件操作
#define SYS_BIT_Disk_Full               ( 1<<7 ) /// CF卡录制盘满，当硬盘空间小于100MB，开始置1

#define SYS_BIT_CFCard_MOUNTING         ( 1<<8 ) /// CF卡加载过程中状态
#define SYS_BIT_CFCard_FORMAT           ( 1<<9 ) /// CF卡正在格式化
#define SYS_BIT_CFCard_FSCK             ( 1<<10 ) /// CF卡正在文件系统检查
#define SYS_BIT_CFCard_RESET            ( 1<<11 ) /// CF卡正在复位
#define SYS_BIT_CFCard_NEEDFORMAT       ( 1<<12 ) /// CF卡需要格式化

//zqh 2015年10月18日 add， 用于当硬盘空间小于500M时，给用户警告信息。
#define SYS_BIT_CFCard_FreeSpaceWarning ( 1<<13 ) /// 当硬盘空间小于500M时，开始置1。


//! 系统回复状态信息标志
//! zqh 2015年10月18日 modify，将状态位修改如下。
//! b2b1b0：000/001/010/011/100/101=待机/录制/回放/文件浏览/系统升级界面/系统功能界面
//! b3——校时请求（0/1=校时成功/请求校时）
//! b4——U盘加载状态（0/1=U盘加载/ U盘未加载）
//! b5——录制状态 0手动  1自动录制(目前先保留)
//! b6——硬盘空间小于500M时，对用户进行警告，此状态位置1，当硬盘空间小于100M时开始删除文件。
//! b7——硬盘加载状态是否正常：1表示故障， 0表示加载正常

//!b2b1b0
#define REPLY_STAT_BIT_STANDBY          (0x00)  /// 待机状态标志位
#define REPLY_STAT_BIT_RECORD           (0x01)  /// 录制状态标志位
#define REPLY_STAT_BIT_PLAY             (0x02)  /// 播放状态标志位
#define REPLY_STAT_BIT_FILEBROSWER      (0x03)  /// 文件浏览状态标志位
#define REPLY_STAT_BIT_SYSTEMEUPDATE    (0x04)  /// 进入系统升级界面
#define REPLY_STAT_BIT_SYSTEMFUNC       (0x05)  /// 进入系统功能界面
//!b3
#define REPLY_STAT_BIT_GETTIME          (1<<3)  /// 获取时间标志位
//!b4
#define REPLY_STAT_BIT_UDISK            (1<<4)  /// U盘已加载标志位
//!b5 保留位，为自动、手动录制状态为。

//!b6 当电子盘空间小于500MB时置1
#define REPLY_STAT_BIT_CFCARDFULL       (1<<6)
//!b7 当电子盘加载故障时该位置1
#define REPLY_STAT_BIT_CFCARDERROR      (1<<7)
//TODO 其他标志位待添加

//! 全局系统状态标志定义
typedef struct _SYS_STATE
{
    INT32U CurState;    /// 系统运行状态
    INT32U DevState;    /// 系统设备状态
}TEF_SYS_STATE;

//! 文件回放信息结构体,文件回放状态与播放速度等
typedef struct {
    //! 文件信息,开始播放时,赋值文件信息
	PlayerInfo sFileInfo;//May be call from blank , so Fill this at init
	
#define PLAYBACK_PLAY	0
#define PLAYBACK_PAUSE	1
    //! 文件播放状态,暂停/播放
    int PlayState;

    //! ?
    int PlayFastorSlow;// nFastorSlow=1 fast -1 slow, 0 normal

    //! 文件播放速度参数
    int PlaySpeed;

    //! 播放方向,正向反向
	enum PlayerOrient PlayOrient;

/* Goto specified time */       /// 未使用
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

//! message box 运行状态定义
#define MESSAGESTAGE_IDLE				0       /// 未运行
#define MESSAGESTAGE_RUN				1       /// 正在显示

#define MESSAGESTAGE_OK					2       /// 已点确认
#define MESSAGESTAGE_FAIL				3       /// 已点否
#define MESSAGESTAGE_CANCEL				4       /// 已取消

//message used in dev state change
//MSG priority
#define MESSAGESTAGE_CFCARDREMOVE		1       /// CF卡不在位标志位位置
#define MESSAGESTAGE_CFCARDDETECT		2       /// 检测到cf卡标志位位置
#define MESSAGESTAGE_UDISKREMOVE		3       /// U盘不在位标志位位置
#define MESSAGESTAGE_UDISKDETECT		4       /// 检测到U盘标志位位置
#define MESSAGESTAGE_SDREMOVE			5       /// sd卡不在位标志位位置
#define MESSAGESTAGE_SDDETECT			6       /// 检测到sd卡标志位位置
#define MESSAGESTAGE_DISKFULL			7       /// 磁盘录制已满标志位位置

//Dev MSG 标志位定义
#define MESSAGE_DEV_CFCARDREMOVE		(1<<MESSAGESTAGE_CFCARDREMOVE)  /// CF卡不在位标志位
#define MESSAGE_DEV_CFCARDDETECT		(1<<MESSAGESTAGE_CFCARDDETECT)  /// 检测到cf卡标志位
#define MESSAGE_DEV_UDISKREMOVE			(1<<MESSAGESTAGE_UDISKREMOVE)   /// U盘不在位标志位
#define MESSAGE_DEV_UDISKDETECT			(1<<MESSAGESTAGE_UDISKDETECT)   /// 检测到U盘标志位
#define MESSAGE_DEV_SDREMOVE			(1<<MESSAGESTAGE_SDREMOVE)      /// sd卡不在位标志位
#define MESSAGE_DEV_SDDETECT			(1<<MESSAGESTAGE_SDDETECT)      /// 检测到sd卡标志位
#define MESSAGE_DEV_DISKFULL			(1<<MESSAGESTAGE_DISKFULL)      /// 磁盘录制已满标志位

#define MAX_DEV_MSG_CNT					(MESSAGESTAGE_DISKFULL+1)

//! 不同的文件拷贝状态标志,根据该状态,显示不同的拷贝提示信息
//! 并在拷贝完成/失败后,进入对应状态,进行对应操作(注意取消之后的操作)
#define MESSAGESTAGE_COPY_TO_UDISK		 1
#define MESSAGESTAGE_COPY_TO_CFCARD		 2
#define MESSAGESTAGE_UPDATE_BACKUP		 3
#define MESSAGESTAGE_UPDATE_UPDATE		 4
#define MESSAGESTAGE_UPDATE_COPYBACK	 5
#define MESSAGESTAGE_CANCEL_COPYTOUDISK  6//zqh 2015年3月30日 modify，添加标志，当“从电子盘向U盘复制文件”时，有取消复制的命令。
#define MESSAGESTAGE_CANCEL_BACKUP		 7
#define MESSAGESTAGE_CANCEL_UPDATE		 8
#define MESSAGESTAGE_CANCEL_COPYTOCFCARD 9//zqh 2015年3月30日 modify，添加标志，当“从U盘向电子盘复制文件过程中”，有取消复制的命令
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
 * 图形窗口结构体定义
 */
typedef struct UI_Menu_tag{
    //! 当前窗口中被选中的button id
    int nButtonIdx;

    //! 当前窗口的父窗口id
    int nFromMenuIdx;
/*Handler*/
    //! 窗口按键消息处理函数
    Pvr_UImessage_fn *msgHandle_fn;
/*menu init & self api*/
    //! 窗口内变量初始化函数，有些情况下，有些变量不需要初始化，此时会不调用该函数切换
    MenuInit_fn *menuInit;

    //! 窗口开始绘制函数，只调用menuRefresh绘制时，可以置空，调用默认绘制函数
    MenuOpen_fn *menuOpen;

    //! 切换到下一个窗口前，需要调用的变量反初始化函数
	MenuClose_fn *menuClose;

    //! 窗口刷新函数
	MenuRefresh_fn *menuRefresh;

    //! 窗口内button等item重绘函数
	MenuRefreshItem_fn *menuRefreshItem;

    //! 窗口id
    int nMenuIdx;

//Static OSD
    //! OSD显示时使用变量,默认窗口不使用
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
 * UI系统全局状态变量结构体
 */
typedef struct{
    //! 当前active窗口id
    int nMenuIdx;

    //! 系统所有窗口资源指针
    UI_Menu_t Menus[UI_MENU_COUNT];

    /*Timers*/
    //! messagebox未接收到按键的倒计时处
    TEF_TIMER_STRUCT sKeyTimer;

    //! 回放timer,未使用
    TEF_TIMER_STRUCT sPlayBackTimer;

    //! 回放时osd显示timer,目前应该未实际启用,需要继续测试
    TEF_TIMER_STRUCT sOSDTimer[OSDPOS_COUNT];

    //! 1s 设备检测timer,用于检测文件录制大小,cf卡在位状态的界面显示,led状态控制
    TEF_TIMER_STRUCT sDevScanTimer;

    //! 显示设备检测提示信息时,要显示的当前设备状态信息的id
    unsigned int nDevCurStatusMsg;
    /*OSD Temp Use*/
    //! 回放osd使用结构体,作用以及使用方法待测试
    TEF_OSD_STRUCT sOSDStructStatic[UI_OSD_COUNT];

    //! 刷新时用到的矩形坐标
    OSD_ALPHA_RECT RefreshRect[OSDPOS_COUNT];		//used in the function of DisplayBlendRefresh()

    //! osd bit map 刷新时使用,具体原理作用待测试
    unsigned int nOSDBitMap;//Current OSD display position bitmap

    /*System Aware */
    //! 当前系统状态标志
    TEF_SYS_STATE SysState;

    //! u盘,cf卡/电子盘等存储介质
    char Vol_Disk[SUPPORT_DISK_CNT][MAX_FS_PATH];

    //file_manage.c
    //! 最多文件数,未使用!
    int max_filecount_ever;

    //! 文件列表内容属性,默认文件列表只包含视频文件,不包含目录
    unsigned long list_contain;

    /*Application Data for each menu*/
    // 0Blank
    //! 当前操作文件名,用于文件拷贝,删除,播放等
    char sRecFilename[MAX_FS_PATH];

    //! 未使用
    int nRecordPause;//0-not pause 1 pause

    //! 当前使用存储录制文件的存储介质.默认设置为cf卡
    int nFileSelectDisk;

    // 6SystemSetting
    // 7FileBrowsers
    //! 当前文件浏览界面文件列表第一个文件的index
    int FileBrowserIndex;		//0--TotalNum-1 ,当前页首行的Index

    //! 当前选择文件在当前文件浏览界面中的位置
    int FileBrowserSelFile;//0-FilePerBloc-1

    //! CF卡中的文件列表list
    struct FileListBlock FileBrowserdirlist;

    //! 从U盘拷贝文件到CF卡时,显示文件中首个文件在文件列表里的index
    int FileBrowserIndex_UDisk;		//0--TotalNum-1 ,当前页首行的Index

    //! 从U盘拷贝文件到CF卡时,显示文件中,当前选择的文件的位置
    int FileBrowserSelFile_UDisk;//0-FilePerBloc-1

    //! U盘中的文件列表list,在U盘拷贝文件到CF卡中使用
    struct FileListBlock FileBrowserdirlist_UDisk;

    //! 文件拷贝时(包括系统升级),源文件的文件路径
    char sFileCopyFrom[MAX_FS_PATH];

    //! 文件拷贝时,目标文件路径
    char sFileCopyTo[MAX_FS_PATH];

    //! 文件拷贝工作状态,参考 MESSAGESTAGE_XXX
    int nFileCopyToStage;	// DoFileCopyTo menu message stage

    //! 文件回放信息结构体
    TEF_PLAYBACK_DATA sPlayback;

    //! 编解码信息字符串
    char CODEC_ERRMSG[CODEC_DEBUG_MSGLEN];

    //! message box 显示信息字符串指针,初始化messagebox的时候赋值
    char *pMessageDisplay;

    //! message box 取消时的提示信息字符串指针,message box显示时,按退出键提示的信息
    char *pMessageFail;

    //! message box 确认时的提示信息字符串指针,message box显示时,按确认键提示的信息
    char *pMessageOK;

    //! message box 超时时间,超时之后,自动执行 cancel 操作
    int nMessageTimeout;//0-Any Key return 1-OK call fn

    //! message box 确认后执行的callback 函数指针
    MessageboxCB_fn *pMessageUser_fn;

    //! message box 取消时执行的callback 函数指针
    MessageboxCB_fn *pMessageCancel_fn;

    //! message box 确认/取消时,执行callback 函数的传入参数
    void *pMessageUser;

    //! message box 反初始化时,执行的callback 函数指针
    MessageboxDeinit_fn *pMessageDeinit_fn;

    //! message box 运行状态
    int nMessageStage;

    /*Application Data for System Setting*/
    //! 应用程序运行配置,初始化时,从配置文件(如果有)读入
    ALL_DATA allData;

    //! 存放所有BMP的C Stream的数据
    unsigned char  *BMPStream;
}UI_System_t;


/*UI System defination*/
//! 全局系统参数变量
extern UI_System_t *pUISystem;

#include "../my_spinit.h"
#include "../my_language.h"
#include "ui_misc.h"
#include "osd_misc.h"


#endif

