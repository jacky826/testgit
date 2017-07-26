#ifndef PVR_ENCODER_HEADER
#define PVR_ENCODER_HEADER

#include <all_data.h>

//called when system startup
void sys_init_encoder(void);
//called when system exit
void sys_deinit_encoder(void);

//called when want to start encoding,in the context of ui and schedule record
int start_encoder(const char *pdestfile,SaveAndPlayPara *penpara);

//called when want to stop encoding,in the context of ui and schedule record 
int stop_encoder(void);

//change the destination avi file and  don't stop the background encoder thread
int change_dest_filename(const char *pdestfile,SaveAndPlayPara *penpara);

//in the context of ui and schedule record
int pause_encoder(void);
//in the context of ui and schedule record
int restart_encoder(void);

int rec_add_video(char *pbuf,int buf_len,int keyframe);

int rec_add_audio(char *pbuf,int buf_len);

#endif
