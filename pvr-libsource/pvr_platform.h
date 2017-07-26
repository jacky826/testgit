#ifndef PVR_PLATFORM_HEADER
#define PVR_PLATFORM_HEADER

// FPGA access part
int Initial_map_fpga(void);
void Deinital_map_fpga(void);

int IsCFCardInSlot(void);
int IsSDCardInSlot(void);
int IsVideoInputOK(void);
void ResetCFCard(void);
void ResetHUB(void);
void EnableHardwareWDT(int enable);
void FeedHardwareWDT(void);

int SetLvdsSwitch(int stat);

int IsBoardSupportAudio(void);
int IsAudioInputMic(void);

#endif
