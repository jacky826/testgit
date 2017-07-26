#ifndef PVR_WATCHDOG_HEADER
#define PVR_WATCHDOG_HEADER

void request2reboot(void);
void FEED_WATCHDOG(void);

void START_WATCHDOG_TIMER_SIG(unsigned int timeout);
void STOP_WATCHDOG_TIMER_SIG(void);

#endif
