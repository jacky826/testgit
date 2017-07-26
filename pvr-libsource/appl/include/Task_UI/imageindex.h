#ifndef  __IMAGE_INDEX_DEFINED
#define __IMAGE_INDEX_DEFINED

#include "../pvr_types.h"

#define AB_32X32 	(0)
#define BUTTON_BLUE_785C4C_200_29_UP 	(1)
#define BUTTON_INVALID_200_29 	(2)
#define BUTTON_YELLOW_2060F0_200_29_UP 	(3)
#define CHANNELSETTINGBG_360_240 	(4)
#define COMBOX_BLUE_785C4C_150_29_UP 	(5)
#define COMBOX_YELLOW_785C4C_150_29_UP 	(6)
#define CURSTATUS_160_90 	(7)
#define FF_32X32 	(8)
#define FILEGRID_BLUE_550_29 	(9)
#define FILEGRID_YELLOW_550_29 	(10)
#define FILESUBBG_550_156 	(11)
#define FR_32X32 	(12)
#define IGNORE_32X32 	(13)
#define LABEL_BLUE_785C4C_100_29_UP 	(14)
#define LABEL_BLUE_785C4C_150_29_UP 	(15)
#define LABEL_YELLOW_785C4C_100_29_UP 	(16)
#define LABEL_YELLOW_785C4C_150_29_UP 	(17)
#define LEFTARROW_DOWN 	(18)
#define LEFTARROW_UP 	(19)
#define LEFTRECT_UP 	(20)
#define MAINMENUBG_360_320 	(21)
#define MESSAGEBOXBG_360_240 	(22)
#define MESSAGERECT_310_130 	(23)
#define NEXT_32X32 	(24)
#define PAUSE_32X32 	(25)
#define PLAY_32X32 	(26)
#define PLAYER_240X40 	(27)
#define PREV_32X32 	(28)
#define PROGRESSBK_300_35 	(29)
#define PROGRESSBLOCK_BLUE_300_35 	(30)
#define PROGRESSBLOCK_YELLOW_300_35 	(31)
#define RECORDING 	(32)
#define RIGHTARROW_DOWN 	(33)
#define RIGHTARROW_UP 	(34)
#define RIGHTRECT_UP 	(35)
#define SCHEDULEDETAILSBG_360_400 	(36)
#define SCHEDULEGRID_BLUE_533_29 	(37)
#define SCHEDULEGRID_YELLOW_533_29 	(38)
#define SCHEDULEMENUBG_620_455 	(39)
#define SLOW_FW_32X32 	(40)
#define SLOW_REW_32X32 	(41)
#define STOP_32X32 	(42)
#define TIMEEDIT_BLUE_785C4C_150_29_UP 	(43)
#define TIMEEDIT_YELLOW_785C4C_150_29_UP 	(44)
#define TIMER_32X32 	(45)
#define VOICEDIS 	(46)
#define VOICEEN 	(47)

#define IMAGE_RESOURCE_NUM			48

#define IMAGE_RESOURCE_FLASH_ADD_8M		0x20600000
#define FAKEAACDATA_FLASH_ADD_8M		0x20580000
#define FAKEAACFRAME_FLASH_ADD_8M		0x205E0000

#define IMAGE_RESOURCE_FLASH_ADD_4M		0x20280000
#define FAKEAACDATA_FLASH_ADD_4M		0x20200000
#define FAKEAACFRAME_FLASH_ADD_4M		0x20260000

#define FLASH_IMAGE_MAGIC				(0x00644D42)
#define FLASH_IMAGE_OFFSET			(0)

#define IMAGE_RESOURCE_FLASH_LEN		0x00081d20

#define IMAGE_RESOURCE_NAME_MAX_LEN	64
typedef struct _IMAGE_RESOURCE_INDEX {
	char ImageName[IMAGE_RESOURCE_NAME_MAX_LEN]; // Image Name
	DWORD ReservedOffset; // The offset address in the NAND reserved field
	DWORD ReservedLength; // The length in the NAND reserved field
} IMAGE_RESOURCE_INDEX;

extern const IMAGE_RESOURCE_INDEX ImageResourceIndex[];

#include "GUI.H"

//BMP Source
//#define DrawStreamedBMP
extern GUI_BITMAP bmBG_DBlue_241_366;
extern GUI_BITMAP bmBG_DBlue_316_156;
extern GUI_BITMAP bmBG_DBlue_620_455;
extern GUI_BITMAP bmButton_Combox_DBlue_148_30;
extern GUI_BITMAP bmButton_Combox_Orange_148_30;
extern GUI_BITMAP bmButton_Flat_DBlue_148_30;
extern GUI_BITMAP bmFileList_Flat_GBlue_240_30;
extern GUI_BITMAP bmFileList_Flat_Orange_240_30;
extern GUI_BITMAP bmButton_Blue_785C4C_200_29_UP;
extern GUI_BITMAP bmButton_Invalid_200_29;
extern GUI_BITMAP bmButton_Yellow_2060F0_200_29_UP;
extern GUI_BITMAP bmMainMenuBG_360_320;
extern GUI_BITMAP bmMessageBoxBG_360_240;
extern GUI_BITMAP bmMessageRect_310_130;
extern GUI_BITMAP bmProgressBarFill_12_20;
extern GUI_BITMAP bmProgressBar_240_20;
extern GUI_BITMAP bm207;

#endif
