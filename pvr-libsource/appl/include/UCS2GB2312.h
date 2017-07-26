/*
* UCS2GB2312.h
*/


#ifndef UCS2GB2312_H
#define UCS2GB2312_H

#include "pvr_types.h"

#define UCS2GBDEBUG			(0)

#define TABNUM				(14890)

#define GBSTARTCODE			(0x2121)	// (0xa1a1 - 0x8080)
#define GBMIDCODE			(0x5041)	// (0xd0c1 - 0x8080)
#define GBENDCODE			(0x777e)	// (0xf7fe - 0x8080)


INT16U ucs2gb(INT16U unicode);		// convert Unicode_int to GB2312_int
INT16U _gb2ucs(INT16U gb2312);		// convert GB2312_int to Unicode_int
INT16U strHexToInt(char* strHex);	// convert strHex to int
char* IntToStr(INT16U code_int, char* str);		// convert int to strHex

extern char* ucs2gb_char(char* unicode_str, char* gb_str);
extern char* gb2ucs_char(char* gb_str, char* unicode_str);
extern char* ucs2gb_int(INT16U* unicode, char* gb_str, int len);
extern INT16U* gb2ucs_int(INT8U* gb_str, INT16U* unicode, int ucs_len);


#endif // UCS2GB2312_H

