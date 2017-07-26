#ifndef  __RTC_DEFINED
#define __RTC_DEFINED


#define MIN_YEAR	1900
#define MAX_YEAR	2099


typedef struct _DATE_TIME {
	WORD Year;			// 1900~2099
	WORD Month;		// 1~12
	WORD DayOfWeek;	// 0~6  ( 0:Sunday, 1:Monday ... )
	WORD Day;			// 1~31
	WORD Hour;			// 0~23
	WORD Minute;		// 0~59
	WORD Second;		// 0~59
} DATE_TIME;


#endif
