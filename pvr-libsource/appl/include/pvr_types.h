#ifndef  __TYPES_DEFINED
#define __TYPES_DEFINED

#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
//--------------------------------------------------------------------------//
// Types declaration															//
//--------------------------------------------------------------------------//
#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned long
#define ULONGLONG unsigned long long

#ifndef INT16U
typedef short int INT16U;
#endif

#ifndef INT8U
typedef unsigned char INT8U;
#endif

#ifndef INT8S
typedef char INT8S;
#endif

#ifndef INT32U
typedef unsigned int INT32U;
#endif

#define BOOL int
#define TRUE 1
#define FALSE 0

#define HANDLE void *

#ifndef NULL
#define NULL 0
#endif

/* Function error codes */
#define SUCCESS             0
#define FAILURE             -1

/* Thread error codes */
#define THREAD_SUCCESS      (void *) 0
#define THREAD_FAILURE      (void *) -1

/* The input buffer height alignment restriction imposed by codecs */
#define CODECHEIGHTALIGN       16

#endif
