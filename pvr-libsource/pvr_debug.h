#ifndef PVR_DEBUG_HEADER
#define PVR_DEBUG_HEADER

#include <stdio.h>
#include <stdlib.h>

#define ENABLE_DEBUG_OUT

#ifndef ENABLE_DEBUG_OUT
#define LOGE(...) do{}while(0);  //information for error
#define LOGW(...) do{}while(0); //information for warning
#define LOGT(...) do{}while(0);   //information for trace
#define DEBUG_INIT do{}while(0);
#define DEBUG_DEINIT do{}while(0);
#else
void debug_init(void);

//#define USE_SYSLOG
#ifndef USE_SYSLOG
#define DEBUG_INIT //debug_init();

#define LOG(a,b...) do{ \
                        fprintf(stderr,a":%s:%d:",__func__,__LINE__);fflush(stderr); \
                        fprintf(stderr,b);fflush(stderr); \
                    }while(0)
#define LOGE(a...) LOG("E",a)
#define LOGW(a...) LOG("W",a)
#define LOGT(a...) LOG("T",a)

#define DEBUG_DEINIT do{}while(0);

#else
#include <syslog.h>
void debug_deinit(void);
#define DEBUG_INIT debug_init();
#define LOGE(args...) syslog(LOG_ERR,args);
#define LOGW(args...) syslog(LOG_WARNING,args);
#define LOGT(args...) syslog(LOG_INFO,args);
#define DEBUG_DEINIT debug_deinit();
#endif

#endif

#define ERR LOGE
#define FUNC_ENTER LOGT("enter\r\n");
#define FUNC_EXIT LOGT("exit\r\n");

void dump_mem(void *ptr,int len); 

int redirect_log(char *dest_path,char *dest_file);
void close_log(void);
int move_log(char * src_path,char *dst_path,char *filename);
int cat_log(char * src_path,char *dst_path,char *filename);

int check_log_size(void);
void get_kernel_msg(char *full_path);

void set_log_en(int en);
int is_log_en(void);

#endif

