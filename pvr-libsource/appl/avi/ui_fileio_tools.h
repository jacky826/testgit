#ifndef _UI_FILEIO_TOOL_H_
#define _UI_FILEIO_TOOL_H_

#include "movcommon.h"
#ifndef INT64_MIN
#define INT64_MIN       (-0x7fffffffffffffffLL-1)
#endif

#ifndef INT64_MAX
#define INT64_MAX (9223372036854775807)
#endif

#ifndef INT_MAX
#define INT_MAX (0x7fffffff)
#endif
#define globalTimescale 1000
enum AVRounding {
    AV_ROUND_ZERO     = 0, ///< round toward zero
    AV_ROUND_INF      = 1, ///< round away from zero
    AV_ROUND_DOWN     = 2, ///< round toward -infinity
    AV_ROUND_UP       = 3, ///< round toward +infinity
    AV_ROUND_NEAR_INF = 5, ///< round to nearest and halfway cases away from zero
};

#define GET_UTF8(val, GET_BYTE, ERROR)\
    val= GET_BYTE;\
    {\
        int ones= 7 - av_log2(val ^ 255);\
        if(ones==1)\
            ERROR\
        val&= 127>>ones;\
        while(--ones > 0){\
            int tmp= GET_BYTE - 128;\
            if(tmp>>6)\
                ERROR\
            val= (val<<6) + tmp;\
        }\
    }

int url_fread(void *buffer, int size, unsigned long count, int stream);

int get_byte(int fp);
unsigned int get_be16(int fp);
unsigned int get_le16(int fp);
unsigned int get_be24(int fp);
unsigned int get_be32(int fp);
unsigned int get_le32(int fp);
unsigned long long get_be64(int fp);
void put_byte(int s, int b);
void put_le32(int s, unsigned int val);
void put_be32(int s, unsigned int val);
void put_le64(int s, unsigned long long val);
void put_be64(int s, unsigned long long val);
void put_le16(int s, unsigned int val);
void put_be16(int s, unsigned int val);
void put_be24(int s, unsigned int val);
void put_tag(int s, const char *tag);
void put_buffer(int s,  char *buf, int size);
//FIXME supprt 64bit varaint with wide placeholders
//int updateSize(int pb, int pos);//SACH:W10847
//unigned long long av_rescale_rnd(unsigned long long a, unsigned long long b, unsigned long long c, enum AVRounding rnd);//SACH:W10878
unsigned short language_code (const char *str);
int utf8len(unsigned char *b);
int ascii_to_wc (int pb, unsigned char *b);
#endif

