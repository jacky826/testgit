/*
**********************************************************************

----------------------------------------------------------------------
File        : LCDConf.h
Purpose     : S3C44B0X的lcd控制器的头文件
Data		: 2003-7-14 13:08
----------------------------------------------------------------------
**********************************************************************
*/

#ifndef LCDCONF_H
#define LCDCONF_H

/*********************************************************************
*
*                   General configuration of LCD
*
**********************************************************************
*/

extern unsigned char *ucgui_fb_addr;
extern int ucgui_x_res;
extern int ucgui_y_res;

#if 0
#define LCD_XSIZE      720      /* X-resolution of LCD, Logical coor. */
#define LCD_YSIZE      480      /* Y-resolution of LCD, Logical coor. */
#else
#define LCD_XSIZE      ucgui_x_res      /* X-resolution of LCD, Logical coor. */
#define LCD_YSIZE      ucgui_y_res      /* Y-resolution of LCD, Logical coor. */
#endif
#define LCD_CONTROLLER 0


#define LCD_BITSPERPIXEL    16




#endif /* LCDCONF_H */

 
