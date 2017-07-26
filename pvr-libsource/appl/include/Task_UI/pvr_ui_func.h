#ifndef _PVR_FUNC_HEADER_
#define _PVR_FUNC_HEADER_

#include <LCDConf.h>


/*
 ----	Misc function
 */
void Pvr_UI_Init(void);
void Pvr_UI_Start(void);
void Pvr_UI_Stop(void);
void Pvr_UI_DeInit(void);

/*
 ----	System Provided API 
 */
#define VALIDMENU(nMenu)	(nMenu<UI_MENU_COUNT)	//检测当前菜单是否有效
									//如果使用菜单指针，可能因为无效指针引起麻烦
									//可以检查，菜单索引是否有效
#define PGETMENU(nMenu)		(&pUISystem->Menus[nMenu])
#define PACTIVE_MENU			PGETMENU(pUISystem->nMenuIdx)

#define VALIDHANDLE(fn)		(fn!=NULL)	//检查处理程序是否有效
#define VALIDFUNCTION(fn)	(fn!=NULL)	//检查处理程序是否有效

int SystemChangeWindow(int nNewMenu,int nNeedClose,int nNeedInit,int nNeedSaveFrom);
int SystemMenuRestore(void);
int MenuSetUsefulButton(int nMenuIdx);
int MenuChangeFocusCustom(int nOffset,int nButtonCnt);
int MenuChangeFocus(int nOffset);
int MenuRefreshItemOnly(int nIdx);
int MenuRefreshItem(int nIdx);
int MenuRefresh(void);

int MenuRefreshAllControl(void);

void DisableTimer(TEF_TIMER_STRUCT *pTimer);//Ignore function

void CloseTimer(TEF_TIMER_STRUCT *pTimer);//Still call the function

void EnableTimer(TEF_TIMER_STRUCT *pTimer,int nPerformClose,unsigned int TimerUser,int TimerTO,TimerTO_fn *fn);

int MessageBox(const char *pContent, int nTimeout,
	MessageboxCB_fn *pfn,MessageboxCB_fn *pCancelfn, void *pUser,
	const char *pOk,const char *pFail,MessageboxDeinit_fn *pDeinitfn);

/*
 ----	Default handler for Menu set handler to NULL 
 */
TMsgHandleStat DefaultTimerHandle(TEF_TIMER_STRUCT *KeyTimer);
int DefaultMenuOpen(UI_Menu_t *pMenu);

/*
 ----	Misc Functions for UI Operation from PVR_common.c
 */
void SetRefreshAreaPos(int nPos, DWORD x0, DWORD y0, DWORD width, DWORD height, WORD alpha, BOOL transC);
int RefreshOSDDisplay(void);

//--------------------------------------------------------------------------//
//	Purpose	:	透明度设定(0:Cover -- 256:Transparent)
//--------------------------------------------------------------------------//
//
#define ALPHA_COMMON					(64)


//--------------------------------------------------------------------------//
//	Purpose	:	位置(坐标)与大小(长宽)设定,单位为像素
//--------------------------------------------------------------------------//
//
#define LCD_WIDTH						LCD_XSIZE
#define LCD_HEIGHT						LCD_YSIZE



//--Common
#define TITLE_BELOW_HEIGHT				(13)		//(1)标题文字显示高度偏移
#define TITLE_ALL_HEIGHT				(40)		//(2)标题总高度
#define BUTTON_H_INTERVAL_COMMON		(5)		//(3)水平X方向间隔
#define BUTTON_V_INTERVAL_COMMON		(8)//(12)		//(4)垂直Y方向间隔
#define BUTTON_H_OFFSET_DISPLAY		(8)		//(5)显示文字距Button左上角X轴坐标的偏移
#define BUTTON_V_OFFSET_DISPLAY		(4)		//(6)显示文字距Button左上角Y轴坐标的偏移
//--TV Setting
#define PROCESS_V_OFFSET_BG_BOTTON	(30)		//(7)TV Setting 中状态条距背景图片下边沿的距离
//--System Setting
#define BUTTON_COMBOX_INTERVAL		(10)		//(8)System Setting中Button和Combox之间的间隔
#define BUTTON_V_INTERVAL_SYSSETTING	(8)		//(9)System Setting中上次两个Button之间的间隔
//--Schedule
#define BUTTON_SCHEDULE_1_CENTER		(20)
#define BUTTON_SCHEDULE_2_CENTER		(81)
#define BUTTON_SCHEDULE_3_CENTER		(148)
#define BUTTON_SCHEDULE_4_CENTER		(202)
#define BUTTON_SCHEDULE_5_CENTER		(258)
#define BUTTON_SCHEDULE_6_CENTER		(318)
#define BUTTON_SCHEDULE_7_CENTER		(378)
#define BUTTON_SCHEDULE_8_CENTER		(442)
#define BUTTON_SCHEDULE_9_CENTER		(504)

//--File Browser
#define RECT_H_PREVIEW					(176)
#define RECT_V_PREVIEW					(144)
#define RECT_H_OFFSET					(22)
//#define OFFSET_H_FILL					(310)
#define OFFSET_H_FILL					(100)//(210)
#define BUTTON_FILEBROWSER_1_CENTER	(8)//(92)
#define BUTTON_FILEBROWSER_2_CENTER	(228)
#define BUTTON_FILEBROWSER_3_CENTER	(348)
#define BUTTON_FILEBROWSER_4_CENTER	(458)

//MessageBox
#define READ_FB							(0)
#define WRITE_FB						(1)
//#define START_X_MSGBOX					((LCD_WIDTH / 2) - 90)
//#define END_X_MSGBOX					((LCD_WIDTH / 2) + 90)
//#define START_Y_MSGBOX					((LCD_HEIGHT / 2) - 60)
//#define END_Y_MSGBOX					((LCD_HEIGHT / 2) + 60)


//--------------------------------------------------------------------------//
//	Purpose	:	PVR Color
//--------------------------------------------------------------------------//
//
#define COLOR_BLUE_DEEP				(0x604030)
#define COLOR_BLUE_LOW					(0x785C4C)
#define COLOR_MESSAGEBOX_RECT			(0x785C4C)
#define COLOR_TEXTINPUT_RECT			(0x060606)
#define COLOR_BLACK_FAKE				(0x060606)

//--------------------------------------------------------------------------//
//	Purpose		:	根据图片长宽自动获得居中显示的左上角坐标
//--------------------------------------------------------------------------//
//
#define AUTO_GET_X0_FROM_WIDTH(i)			((LCD_WIDTH - (i)) / 2)
#define AUTO_GET_Y0_FROM_HEIGHT(i)		((LCD_HEIGHT - (i)) / 2)

//--------------------------------------------------------------------------//
//	Purpose		:	ICON DIR or FILE
//--------------------------------------------------------------------------//
//
#define DIR_ICON(i)					(1 << (2 * (i)))
#define FILE_ICON(i)					(2 << (2 * (i)))

#define TESTBIT(A,BIT)			(A&(BIT))
#define SETBIT(A,BIT)				(A|=(BIT))
#define CLEARBIT(A,BIT)			(A&=(~(BIT)))


void * BMPIndexSearch(int nImageName);


/*
 ----	Misc Functions for UI Operation from My_tool.c
 */
 void get_week_from_date(DATE_TIME *date);

void datetime_setting(int adjustval,INT8S ListIndex, DATE_TIME *tmp_date);

void datetime_change(int value, INT8S ListIndex, DATE_TIME *tmp_date);

void Pvr_Waiting(BOOL blend_or_not);

/*
 ----	Misc Functions for UI Operation ,Shadow Copy Code from PVR.c
 */
void AdjustValue(int *val,int adjval,int min,int max);

#endif

