/*************File io func in avi,mov,mp4 files**********************/
#include "ui_fileio_tools.h"
#include "movcommon.h"
#include <fcntl.h> 
#include <unistd.h>

const unsigned char ff_log2_tab[256]={
        0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
        5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
        6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
        6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7
};

static inline int av_log2(unsigned int v)
{
    int n;

    n = 0;
    if (v & 0xffff0000) {
        v >>= 16;
        n += 16;
    }
    if (v & 0xff00) {
        v >>= 8;
        n += 8;
    }
    n += ff_log2_tab[v];

    return n;
}

int url_fread(void *buffer, int size, unsigned long count, int stream)
{
	unsigned long readbytes;
	readbytes = read(stream,buffer,size*count);
	return readbytes;
}

void put_buffer(int s,  char *buf, int size)
{
	write(s,buf,size);	
}

int get_byte(int fp)
{
	char tag;	
	url_fread(&tag,sizeof(char),1,fp);
	return tag;	
}

unsigned int get_be16(int fp)
{
	unsigned char tag1,tag2;
	url_fread(&tag1,sizeof(char),1,fp);
	url_fread(&tag2,sizeof(char),1,fp);	
	return ((tag1<<8)|tag2);
}

unsigned int get_le16(int fp)
{
	unsigned short tag;
	url_fread(&tag,sizeof(short),1,fp);
	return tag;
}

unsigned int get_be24(int fp)
{
    unsigned int val;
    val = get_be16(fp) << 8;
    val |= get_byte(fp);
    return val;
}

unsigned int get_be32(int fp)
{
	unsigned int val;
	register int r0,r1,r2,r3,r4;
#if 0	
	val = get_be16(fp) << 16;
	val |= get_be16(fp);
#else
	url_fread(&val,sizeof(int),1,fp);
	r4=val;
	r0=(val<<24)&0xff000000;
	r1=(val<<8)&0xff0000;
	r2=(val>>8)&0xff00;
	r3=(val>>24)&0xff;
	r4=r0|r1;
	r4|=r2;
	r4|=r3;
	val=r4;
#endif
	return val;
}

unsigned int get_le32(int fp)
{
	unsigned int tag;
	url_fread(&tag,sizeof(int),1,fp);
	return tag;
}

unsigned long long get_be64(int fp)
{
    unsigned long long val;
    val = (unsigned long long)get_be32(fp) << 32;
    val |= (unsigned long long)get_be32(fp);
    return val;
}

void put_byte(int s, int b)
{
	char buf  = (char)(b & 0xff);//SACH:W10893
	write(s,&buf,1);
}

void put_le32(int s, unsigned int val)
{
    put_byte(s, val);
    put_byte(s, val >> 8);
    put_byte(s, val >> 16);
    put_byte(s, val >> 24);
}

void put_be32(int s, unsigned int val)
{
    put_byte(s, val >> 24);
    put_byte(s, val >> 16);
    put_byte(s, val >> 8);
    put_byte(s, val);
}

void put_le64(int s, unsigned long long val)
{
    put_le32(s, (unsigned int)(val & 0xffffffff));
    put_le32(s, (unsigned int)(val >> 32));
}

void put_be64(int s, unsigned long long val)
{
    put_be32(s, (unsigned int)(val >> 32));
    put_be32(s, (unsigned int)(val & 0xffffffff));
}

void put_le16(int s, unsigned int val)
{
    put_byte(s, val);
    put_byte(s, val >> 8);
}

void put_be16(int s, unsigned int val)
{
    put_byte(s, val >> 8);
    put_byte(s, val);
}

void put_be24(int s, unsigned int val)
{
    put_be16(s, val >> 8);
    put_byte(s, val);
}

void put_tag(int s, const char *tag)
{
    while (*tag) {
        put_byte(s, *tag++);
    }
}



//FIXME supprt 64bit varaint with wide placeholders
//int updateSize(int pb, int pos)//SACH:W10847
//{
//	long curpos = lseek(pb,0,SEEK_CUR);
//	unsigned long returnfp;
//	
//	returnfp = pos;
//	if(lseek(pb,returnfp,SEEK_SET)==-1)
//	{
//		return 0; 
//	}
//	put_be32(pb, curpos - pos); /* rewrite size */
//	returnfp = curpos;
//	lseek(pb,returnfp,SEEK_SET);	
//	return curpos - pos;
//}
#if 0 //SACH:W10878
unsigned long long av_rescale_rnd(unsigned long long a, unsigned long long b, unsigned long long c, enum AVRounding rnd)
{
    unsigned long long r=0;
//    assert(c > 0);
//    assert(b >=0);
//    assert(rnd >=0 && rnd<=5 && rnd!=4);

    if(a<0 && a != INT64_MIN) return -av_rescale_rnd(-a, b, c, rnd ^ ((rnd>>1)&1));

    if(rnd==AV_ROUND_NEAR_INF) r= c/2;
    else if(rnd&1)             r= c-1;

    if(b<=INT_MAX && c<=INT_MAX){
        if(a<=INT_MAX)
            return (a * b + r)/c;
        else
            return a/c*b + (a%c*b + r)/c;
    }else{
#if 1
        unsigned long long a0= a&0xFFFFFFFF;
        unsigned long long a1= a>>32;
        unsigned long long b0= b&0xFFFFFFFF;
        unsigned long long b1= b>>32;
        unsigned long long t1= a0*b1 + a1*b0;
        unsigned long long t1a= t1<<32;
        int i;

        a0 = a0*b0 + t1a;
        a1 = a1*b1 + (t1>>32) + (a0<t1a);
        a0 += r;
        a1 += a0<r;

        for(i=63; i>=0; i--){
//            int o= a1 & 0x8000000000000000ULL;
            a1+= a1 + ((a0>>i)&1);
            t1+=t1;
            if(/*o || */c <= a1){
                a1 -= c;
                t1++;
            }
        }
        return t1;
    }
#else
        AVInteger ai;
        ai= av_mul_i(av_int2i(a), av_int2i(b));
        ai= av_add_i(ai, av_int2i(r));

        return av_i2int(av_div_i(ai, av_int2i(c)));
    }
#endif
}
#endif

unsigned short language_code (const char *str)
{
    return ((((str[0]-0x60) & 0x1F)<<10) + (((str[1]-0x60) & 0x1F)<<5) + ((str[2]-0x60) & 0x1F));
}

int utf8len(unsigned char *b){
    int len=0;
    int val;
    while(*b){
        GET_UTF8(val, *b++, return -1;)
        len++;
    }
    return len;
}

int ascii_to_wc (int pb, unsigned char *b)
{
    int val;
    while(*b){
        GET_UTF8(val, *b++, return -1;)
        put_be16(pb, val);
    }
    put_be16(pb, 0x00);
    return 0;
}

int changeendian(int little)
{
	int big;
	int r1,r2,r3,r4;
	r1 = (little&0x000000ff)<<24;
	r2 = (little&0x0000ff00)<<8;
	r3 = (little&0x00ff0000)>>8;
	r4 = (little&0xff000000)>>24;
	big = r1|r2|r3|r4;
	return big;
}
