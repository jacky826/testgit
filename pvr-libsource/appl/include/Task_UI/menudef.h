#ifndef _MENUS_DEF_HEADER_
#define _MENUS_DEF_HEADER_


/**********************************************************
 * 各窗口变量定义
 */

/**
 * 窗口ID定义,同时也是各个窗口在pUISystem->nMenus[]里的index
 */
#define MENUBLANK			0                           // 0
#define MENUFILEBROWSER		(MENUBLANK+1)               // 1
#define MENUFILEOPERATION	(MENUFILEBROWSER+1)         // 2
#define MENUFILECOPYTO		(MENUFILEOPERATION+1)       // 3
#define MENUCODECSETTING	(MENUFILECOPYTO+1)          // 4
#define MENUPLAYBACK		(MENUCODECSETTING+1)        // 5
#define MENUDEVSTATMSG		(MENUPLAYBACK+1)            // 6
#define MENUSYSTEMUPDATE	(MENUDEVSTATMSG+1)          // 7
#define MENUDOFILECOPY		(MENUSYSTEMUPDATE+1)        // 8
#define MENUMESSAGEBOX		(MENUDOFILECOPY+1)          // 9

// sys functions & udisk file list menu
#define MENUSYSTEMFUNC		(MENUMESSAGEBOX+1)          // 10
#define MENUUDISKFILELIST	(MENUSYSTEMFUNC+1)          // 11
#define MENULANGUAGESEL     (MENUUDISKFILELIST+1)       // 12

/**
 * 窗口总数，添加新窗口时注意更新
 * zqh 2015年3月31日 modify，添加“语言”选择菜单*/


#if 0
#define UI_MENU_COUNT			(MENUUDISKFILELIST+1)
#else
#define UI_MENU_COUNT			(MENULANGUAGESEL+1)
#endif

/**
 * 各个窗口的按钮个数,用于按钮切换、按钮无效设置
 */
extern const int MenuButtonCnt[];

/**
 * 各个窗口的按钮无效掩码设置,如需设置,在窗口初始化时修改
 */
extern int MenuButtonMaskBitmap[];

#define MENU_MASK_NONE                              (0)
#define BUTTON_CNT_STORAGEINFO                      (SUPPORT_DISK_CNT)


/**
 * Blank
 * ---------------MenuBlank.c
 */
#define BUTTON_CNT_BLANK                            (0)
#define BUTTON_MASK_BLANK                           MENU_MASK_NONE

#define DELAY_BLANKPROGRAM                          (2)			// 2 s
#define DELAY_BLANKOSD                              (2)			// 2s
TMsgHandleStat MenuBlank_MsgHandle(UI_Menu_t *pMenu,unsigned short nMsgType,unsigned short nMsgData);

//Init NULL

//Open DefaultMenuOpen

int MenuBlank_Open(UI_Menu_t *pMenu);

int MenuBlank_Close(UI_Menu_t *pMenu);

int MenuBlank_Refresh(UI_Menu_t *pMenu);

//Change Focus NULL



/*File Browser: List File info
 *--------------- MenuFile.c
 */
#define MenuFileBrowser_Button_ID_MountUdisk	(BUTTON_CNT_FILEBROWSER_FILELIST+0)

#define BUTTON_CNT_FILEBROWSER_FILELIST			(FILES_PER_BLOCK)
#define BUTTON_CNT_FILEBROWSER_SET_LIST			(1)

#define BUTTON_CNT_FILEBROWSER					(BUTTON_CNT_FILEBROWSER_FILELIST+BUTTON_CNT_FILEBROWSER_SET_LIST)
#define BUTTON_MASK_FILEBROWSER					MENU_MASK_NONE

#define MenuFileBrowser_FocusGroup_FileList		(0)
#define MenuFileBrowser_FocusGroup_Setting		(1)

#define BUTTON_CNT_FILESUBINFO					(6)
#define ALL_FILES									"*"
#define VIDEO_FILES								".avi|"

TMsgHandleStat MenuFileBrowser_MsgHandle(UI_Menu_t *pMenu,unsigned short nMsgType,unsigned short nMsgData);

int MenuFileBrowser_Init(UI_Menu_t *pMenu);
//Open DefaultMenuOpen
int MenuFileBrowser_Close(UI_Menu_t *pMenu);
int MenuFileBrowser_Refresh(UI_Menu_t *pMenu);
int MenuFileBrowser_RefreshItem(struct UI_Menu_tag *pMenu,int nIdx,int nFocus);

/*File Operation: Change Codec Setting
*--------------- MenuTools.c
 */
#define BUTTON_CNT_CODECSETTING_LEFT				(4)
#define BUTTON_CNT_CODECSETTING						(BUTTON_CNT_CODECSETTING_LEFT*2)
#define BUTTON_MASK_CODECSETTING					MENU_MASK_NONE

#define FULLDISKSTRATEGY_CNT						(2)
#define CODECLEVEL_CNT								(4)
#define CODECLEVEL_RANGE							CODECLEVEL_CNT
#define BRIGHTNESS_RANGE							(256)
#define CHROMA_RANGE								BRIGHTNESS_RANGE
#define CONTRAST_RANGE								BRIGHTNESS_RANGE

#define MenuCodecSetting_Button_ID_CodecLevel		(BUTTON_CNT_CODECSETTING_LEFT+0)
#define MenuCodecSetting_Button_ID_Brightness		(MenuCodecSetting_Button_ID_CodecLevel+1)
#define MenuCodecSetting_Button_ID_Chroma			(MenuCodecSetting_Button_ID_Brightness+1)
#define MenuCodecSetting_Button_ID_Contrast			(MenuCodecSetting_Button_ID_Chroma+1)

TMsgHandleStat MenuCodecSetting_MsgHandle(UI_Menu_t *pMenu,unsigned short nMsgType,unsigned short nMsgData);

int MenuCodecSetting_Init(UI_Menu_t *pMenu);
//Open DefaultMenuOpen
//Close NULL
int MenuCodecSetting_Refresh(UI_Menu_t *pMenu);
int MenuCodecSetting_RefreshItem(struct UI_Menu_tag *pMenu,int nIdx,int nFocus);

//zqh 2015年3月31日 add，添加选择“语言”功能
/* Language select: select system language
 * ------------------------ MenuTools.c
*/

#define BUTTON_CNT_LANGUAGE                        (3)
#define BUTTON_MASK_LANGUAGE                   MENU_MASK_NONE
TMsgHandleStat MenuLanguageSel_MsgHandle(UI_Menu_t *pMenu,unsigned short nMsgType,unsigned short nMsgData);

int MenuLanguageSel_Init(UI_Menu_t *pMenu);
//Open DefaultMenuOpen
//Close NULL
int MenuLanguageSel_Refresh(UI_Menu_t *pMenu);
int MenuLanguageSel_Refresh_Item(struct UI_Menu_tag *pMenu,int nIdx,int nFocus);


/*File Operation: List File Operation
 *--------------- MenuFile.c
 */
//#define BUTTON_CNT_FILEOPERATION					(6)
#define BUTTON_CNT_FILEOPERATION					(4)
#define BUTTON_MASK_FILEOPERATION				MENU_MASK_NONE
TMsgHandleStat MenuFileOper_MsgHandle(UI_Menu_t *pMenu,unsigned short nMsgType,unsigned short nMsgData);

int MenuFileOper_Init(UI_Menu_t *pMenu);
//Open DefaultMenuOpen
//Close NULL
int MenuFileOper_Refresh(UI_Menu_t *pMenu);
int MenuFileOper_RefreshItem(struct UI_Menu_tag *pMenu,int nIdx,int nFocus);

/*File Copy TO: Copy to disk
 *--------------- MenuFile.c
 */
//BUTTON_CNT_STORAGEINFO
#define BUTTON_MASK_COPYTO				MENU_MASK_NONE

TMsgHandleStat MenuCopyTo_MsgHandle(UI_Menu_t *pMenu,unsigned short nMsgType,unsigned short nMsgData);

int MenuCopyTo_Init(UI_Menu_t *pMenu);
//Open DefaultMenuOpen
//Close NULL
int MenuCopyTo_Refresh(UI_Menu_t *pMenu);
int MenuCopyTo_RefreshItem(struct UI_Menu_tag *pMenu,int nIdx,int nFocus);
//Change Focus NULL,just use refresh

#define SYSTEMTOREBOOT_MESSAGE_DELAY					(3)//zqh 2015年4月15日 modify，当弹出“等待系统重启”菜单时，调用此延时时间
/*Play back
 *--------------- MenuPlayback.c
 */
#define BUTTON_CNT_PLAYBACK					(0)
#define BUTTON_MASK_PLAYBACK				MENU_MASK_NONE
TMsgHandleStat MenuPlayback_MsgHandle(UI_Menu_t *pMenu,unsigned short nMsgType,unsigned short nMsgData); 

int MenuPlayback_Init(UI_Menu_t *pMenu);
int MenuPlayback_Close(UI_Menu_t *pMenu);
int MenuPlayback_Refresh(UI_Menu_t *pMenu);

/*Message Box
 *--------------- MenuTools.c
 */
#define BUTTON_CNT_MESSAGEBOX					(0)
#define BUTTON_MASK_MESSAGEBOX				MENU_MASK_NONE

#define CODEC_DEBUG_MSGLEN					MAX_FS_PATH+64
#define CODEC_MESSAGE_DELAY					(5)//zqh 2015年4月13日modify，将延时由3S改为5S。
TMsgHandleStat MenuMessageBox_MsgHandle(UI_Menu_t *pMenu,unsigned short nMsgType,unsigned short nMsgData); 

int MenuMessageBox_Init(UI_Menu_t *pMenu);
int MenuMessageBox_Open(UI_Menu_t *pMenu);
int MenuMessageBox_Close(UI_Menu_t *pMenu);
int MenuMessageBox_Refresh(UI_Menu_t *pMenu);

/*Device Status Message Box
 *--------------- MenuTools.c
 */
#define BUTTON_CNT_DEVSTATUS				(0)
#define BUTTON_MASK_DEVSTATUS				MENU_MASK_NONE

#define DEV_MESSAGE_DELAY					(10)

TMsgHandleStat MenuDev_Status_MSG_MsgHandle(UI_Menu_t *pMenu,unsigned short nMsgType,unsigned short nMsgData);

int MenuDev_Status_MSG_Init(UI_Menu_t *pMenu);
int MenuDev_Status_MSG_Open(UI_Menu_t *pMenu);
int MenuDev_Status_MSG_Close(UI_Menu_t *pMenu);
int MenuDev_Status_MSG_Refresh(UI_Menu_t *pMenu);

/*System Updata Menu
 *--------------- MenuTools.c
 */
#define BUTTON_CNT_SYSTEMUPDATA				(5)

#define MenuSystemUpdate_Button_ID_uboot	(0)
#define MenuSystemUpdate_Button_ID_Kernel	(1)
#define MenuSystemUpdate_Button_ID_FileSys	(2)
#define MenuSystemUpdate_Button_ID_Daemon	(3)
#define MenuSystemUpdate_Button_ID_SysInfo	(4)

#define BUTTON_MASK_SYSTEMUPDATA			MENU_MASK_NONE

TMsgHandleStat MenuSystemUpdate_MsgHandle(UI_Menu_t *pMenu,unsigned short nMsgType,unsigned short nMsgData);
int MenuSystemUpdate_Close(UI_Menu_t *pMenu);
int MenuSystemUpdate_Open(UI_Menu_t *pMenu);
int MenuSystemUpdate_Init(UI_Menu_t *pMenu);
int MenuSystemUpdate_Refresh(UI_Menu_t *pMenu);
int MenuSystemUpdate_Refresh_Item(struct UI_Menu_tag *pMenu,int nIdx,int nFocus);

/*File Copy Menu
 *--------------- MenuTools.c
 */
#define BUTTON_CNT_DOFILECOPY				(0)
#define BUTTON_MASK_DOFILECOPY				MENU_MASK_NONE
TMsgHandleStat MenuDoFileCopy_MsgHandle(UI_Menu_t *pMenu,unsigned short nMsgType,unsigned short nMsgData);
int MenuDoFileCopy_Close(UI_Menu_t *pMenu);
int MenuDoFileCopy_Init(UI_Menu_t *pMenu);
int MenuDoFileCopy_Refresh(UI_Menu_t *pMenu);


/*System Func Menu
 *--------------- MenuTools.c
 */
int NeedFormatCallback(void *pUser);

//zqh 2015年3月24日 modify，將5个button改为3个button，即将：“获取系统日志”及“开启/关闭日志“功能去掉
//zqh 2015年3月31日 modify，添加“语言选择”功能，将button数量改为4个。
#define BUTTON_CNT_SYSTEMFUNC					(4)

//zqh s2015年3月24日 modify， 将“MenuSystemFunc_Button_ID_GetLOG”及“MenuSystemFunc_Button_ID_LogEnable”去掉
//zqh 2015年3月31日 add，添加“语言选择”定义。 MenuSystemFunc_Button_ID_Language
#define MenuSystemFunc_Button_ID_CopyToCF		(0)
//#define MenuSystemFunc_Button_ID_GetLOG			(1)
#define MenuSystemFunc_Button_ID_FormatCF		(1)
#define MenuSystemFunc_Button_ID_FSCK			(2)
#define MenuSystemFunc_Button_ID_Language       (3)//zqh add
//#define MenuSystemFunc_Button_ID_Updatetools	(4)
//#define MenuSystemFunc_Button_ID_LogEnable      MenuSystemFunc_Button_ID_Updatetools

#define BUTTON_MASK_SYSTEMFUNC				MENU_MASK_NONE
int MenuSystemFunc_Init(UI_Menu_t *pMenu);
TMsgHandleStat MenuSystemFunc_MsgHandle(UI_Menu_t *pMenu,unsigned short nMsgType,unsigned short nMsgData);
int MenuSystemFunc_RefreshItem(struct UI_Menu_tag *pMenu,int nIdx,int nFocus);
int MenuSystemFunc_Refresh(UI_Menu_t *pMenu);


/*Copy to CFCard Menu
 *--------------- MenuTools.c
 */
#define BUTTON_CNT_UDISKFILELIST				(4)
#define BUTTON_MASK_UDISKFILELIST				MENU_MASK_NONE
int MenuUDiskFileList_Init(UI_Menu_t *pMenu);
int MenuUDiskFileList_Close(UI_Menu_t *pMenu);
TMsgHandleStat MenuUDiskFileList_MsgHandle(UI_Menu_t *pMenu,unsigned short nMsgType,unsigned short nMsgData);
int MenuUDiskFileList_RefreshItem(struct UI_Menu_tag *pMenu,int nIdx,int nFocus);
int MenuUDiskFileList_Refresh(UI_Menu_t *pMenu);


/*
 * StartCopyFile,Change window to DoFileCopy
 */
int StartCopyFile(char *src_file,char *dst_file);


/**
 * @brief Mount CFCard Func,without UI
 * --------------- MenuMountCF.c
 */
//#define BUTTON_CNT_MOUNTCF 					    (0)
//#define BUTTON_MASK_MOUNTCF				        MENU_MASK_NONE
//int MenuMountCF_Init(UI_Menu_t *pMenu);
//int MenuMountCF_Close(UI_Menu_t *pMenu);
//int MenuMountCF_Refresh(UI_Menu_t *pMenu);
//TMsgHandleStat MenuMountCF_MsgHandle(UI_Menu_t *pMenu,unsigned short nMsgType,unsigned short nMsgData);

int StartMountCF_Thread(void);
int CheckMountFlags(void);
void StopMountCF_Thread(void);
extern int CF_Mount_Runing;

#endif
