/*
* UCS - GB2312
*	Hoking
* 2005. 7.27
*/

#include <stdlib.h>
#include <stdio.h>
#include <UCS2GB2312.h>

#include <gb2312_unicode.h>
#include <unicode_gb2312.h>


#define BIG_FONT		(0)

char* ucs2gb_char(char* unicode_str, char* gb_str)
{
	return (IntToStr(ucs2gb(strHexToInt(unicode_str)), gb_str));
}

char* gb2ucs_char(char* gb_str, char* unicode_str)
{
	return (IntToStr(_gb2ucs(strHexToInt(gb_str)), unicode_str));
}

char* ucs2gb_int(INT16U* unicode, char* gb_str, int gb_len)
{
	int i = 0;
	int j = 0;
	int tmp_int = 0;
	
	if (unicode == NULL)
		return 0;
		
	if (gb_str == NULL)
		return 0;
	
#if BIG_FONT
	for(i = 0; (unicode[i/2] != 0) && (i < gb_len); i += 2) {
		tmp_int = ucs2gb(unicode[i/2]);
		gb_str[i] = (tmp_int >> 8) & 0x00ff;
		gb_str[i + 1] = tmp_int & 0x00ff;
	}
	gb_str[i] = '\0';
#else
	i = 0;
	j = 0;
	while ((unicode[i] != 0) && (j < gb_len)) {
		tmp_int = ucs2gb(unicode[i]);
		if ((tmp_int >= 0x20) && (tmp_int <= 0x7e)) {	/* ASCII */
			gb_str[j] = tmp_int & 0x00ff;
			i++;
			j++;
		} else {	/* 中文 */
			gb_str[j] = (tmp_int >> 8) & 0x00ff;
			j++;
			gb_str[j] = tmp_int & 0x00ff;
			i++;
			j++;
		}
	}
	gb_str[j] = '\0';
#endif
	
#if UCS2GBDEBUG
	printf("UCS2GB2312: convert num = %d\n", i);
#endif
	return gb_str;
}

INT16U* gb2ucs_int(INT8U* gb_str, INT16U* unicode, int ucs_len)
{
	int i = 0;
	int j = 0;
	int tmp_int = 0;
	
	if (unicode == NULL)
		return 0;
		
	if (gb_str == NULL)
		return 0;

#if BIG_FONT
	for(i = 0; (gb_str[i] != 0) && (i < ucs_len); i += 2) {
		tmp_int = gb_str[i] * 256 + gb_str[i + 1];
		*(unicode + i / 2) = _gb2ucs(tmp_int);
	}
	*(unicode + i / 2) = '\0';
#else
	i = 0;
	j = 0;
	while ((gb_str[i] != 0) && (i < ucs_len)) {
		if ((gb_str[i] >= 0x20) && (gb_str[i] <= 0x7e)) {	/* ASCII */
			tmp_int = gb_str[i];
			*(unicode + j) = _gb2ucs(tmp_int);
			i++;
			j++;
		} else {	/* 中文 */
			tmp_int = gb_str[i] * 256 + gb_str[i + 1];
			*(unicode + j) = _gb2ucs(tmp_int);
			i += 2;
			j++;
		}
	}
	*(unicode + j) = '\0';
#endif

	return unicode;
}

INT16U ucs2gb(INT16U unicode)
{
	INT16U gbstart = 0;
	INT16U gbmid = (TABNUM -2) / 2;
	INT16U gbend = TABNUM - 2;
#if UCS2GBDEBUG
	INT16U i = 0;
#endif

#if UCS2GBDEBUG
	printf("Unicode = %x\n\r", unicode);
#endif

	if (unicode & 0xff00) {	// 中文区域
		while (gbmid != gbstart) {
			if (unicode <= unicode_gb2312[gbmid + 1])
				gbend = gbmid;
			else
				gbstart = gbmid;

			if ((gbmid = gbstart + ((gbend - gbstart) / 2)) %2)	// 奇数
				gbmid -= 1;
#if UCS2GBDEBUG
			i++;
#endif
		}
#if UCS2GBDEBUG
		printf("convert circle is : %d\n\r", i);
#endif
		if (unicode == unicode_gb2312[gbstart + 1])
			return (unicode_gb2312[gbstart] + 0x8080);
		else
			return (unicode_gb2312[gbend] + 0x8080);
	} else {	// ASCII区域
#if BIG_FONT
		return (unicode + 0xa380);
#else
		return unicode;
#endif
	}
	
}

INT16U _gb2ucs(INT16U gb2312)
{
	INT16U gbstart = 0;
	INT16U gbmid = (TABNUM -2) / 2;
	INT16U gbend = TABNUM - 2;
#if UCS2GBDEBUG
	INT16U i = 0;
#endif

#if UCS2GBDEBUG
	printf("GB2312 = %x\n\r", gb2312);
#endif

	if (gb2312 >= 0xb0a0) {	// 中文区域
		gb2312 -= 0x8080;
		while (gbmid != gbstart) {
			if (gb2312 <= gb2312_unicode[gbmid])
				gbend = gbmid;
			else
				gbstart = gbmid;

			if ((gbmid = gbstart + ((gbend - gbstart) / 2)) %2)	// 奇数
				gbmid -= 1;
#if UCS2GBDEBUG
			i++;
#endif
		}
#if UCS2GBDEBUG
		printf("convert circle is : %d\n\r", i);
#endif
		if (gb2312 == gb2312_unicode[gbstart])
			return (gb2312_unicode[gbstart + 1]);
		else
			return (gb2312_unicode[gbend + 1]);
	} else {	// ASCII区域
#if BIG_FONT
		return (gb2312 - 0xa380);
#else
		return (gb2312);
#endif
	}
}

INT16U strHexToInt(char* strHex)
{
	int i; 
	if (strHex == NULL)
		return 0;
	if(sscanf(strHex,"%x",&i) > 0)//SACH:W10850
	{
		return i;
	}

	return 0;
	
}

char* IntToStr(INT16U code_int, char* str)
{
	char i;
	if (str == NULL)
		return 0;
	for (i = 0; i != 4; i++) {
		str[i] = (code_int >> (3-i)*4) & 0x000f;
	}

	for (i = 0; i != 4; i++) {
		if (str[i] < 10) {
			str[i] += '0';
		} else {
			switch (str[i]) {
			case 10:
				str[i] = 'a';
				break;
			case 11:
				str[i] = 'b';
				break;
			case 12:
				str[i] = 'c';
				break;
			case 13:
				str[i] = 'd';
				break;
			case 14:
				str[i] = 'e';
				break;
			case 15:
				str[i] = 'f';
				break;
			default:
				str[i] = 0;
				break;
			}
		}
	}
	str[4] = '\0';
	return str;
}


