#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#include <sys/mount.h>

#include "DaemonConf.h"

/******************* watchdog thread *************************/
#include <sys/mman.h>

char *fpga_base = NULL;
#define FPGA_BASE						0X4000000
#define FPGA_MAP_SIZE					0x100000

#define VIDEO_LOST_REG_OFFSET			(0x2200<<2)
#define CF_STATUS_REG_OFFSET			(0x2400<<2)
#define SD_STATUS_REG_OFFSET			(0x2600<<2)

#define CF_RESET_REG_OFFSET				(0x2800<<2)
#define HUB_RESET_REG_OFFSET			(0x2a00<<2)
#define WATCHDOG_SET2_REG_OFFSET		(0x2c00<<2)
#define WATCHDOG_WDI_REG_OFFSET			(0x2e00<<2)

#define pVIDEO_LOST_REG					(volatile char *)(fpga_base+VIDEO_LOST_REG_OFFSET)
#define pCF_STATUS_REG					(volatile char *)(fpga_base+CF_STATUS_REG_OFFSET)
#define pSD_STATUS_REG					(volatile char *)(fpga_base+SD_STATUS_REG_OFFSET)
#define pFPAG_LED2_REG					(volatile char *)(fpga_base+FPGA_LED2_REG_OFFSET)
#define pCF_RESET_REG					(volatile char *)(fpga_base+CF_RESET_REG_OFFSET)
#define pHUB_RESET_REG					(volatile char *)(fpga_base+HUB_RESET_REG_OFFSET)
#define pWATCHDOG_SET2_REG				(volatile char *)(fpga_base+WATCHDOG_SET2_REG_OFFSET)
#define pWATCHDOG_WDI_REG				(volatile char *)(fpga_base+WATCHDOG_WDI_REG_OFFSET)

int Initial_map_fpga(void)
{
#if DEBUG_V2_1
	int fd;
	fd = open("/dev/mem",O_RDWR|O_SYNC,0);
	if(fd>0) {
		fpga_base = mmap(0,FPGA_MAP_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,
							fd,FPGA_BASE);
		close(fd);
		if(((int)fpga_base) == -1) {
			LOGE("failed to map fpga base\r\n");
			fpga_base = NULL;
			return -1;
		}
	}else {
		LOGE("failed to open /dev/mem \r\n");
		return -1;
	}
#endif
    return 1;
}

void Deinital_map_fpga(void)
{
#if DEBUG_V2_1
	if(fpga_base!=NULL) {
		munmap((void *)fpga_base, FPGA_MAP_SIZE);
	}
#endif
}

void EnableHardwareWDT(int enable)
{
#ifdef ENABLE_HARDWARE_WATCHDOG
	*pWATCHDOG_SET2_REG = enable;
#endif
}

void FeedHardwareWDT(void)
{
#ifdef ENABLE_HARDWARE_WATCHDOG
	static int stat = 0;
	stat = !stat;
	*pWATCHDOG_WDI_REG = stat;
#endif
}

pthread_t watchdog_tmr_thread;
static int watchdog_thread_running = 0;
static int watchdog_thread_en = 0;
static int watchdog_time_out = 0;
static int watchdog_fd = -1;

int Mknod_Watchdog(void)
{
	int ret;

	ret = mount(RFS_MOUNT_NOD,RFS_MOUNT_DST,RFS_MOUNT_TYPE,MS_REMOUNT,0);
	if(ret<0)
	{
		LOGE("remount RFS to rw err:%s\r\n",strerror(errno));
	}

	ret = remove("/dev/watchdog");
	if(ret<0)
	{
		LOGE("remove watchdog error:%s\r\n",strerror(errno));
	}

	LOGT("mknod /dev/watchdog c 10 130\r\n");
	system("mknod /dev/watchdog c 10 130");

	ret = mount(RFS_MOUNT_NOD,RFS_MOUNT_DST,RFS_MOUNT_TYPE,MS_RDONLY|MS_REMOUNT,0);
	if(ret<0)
	{
		LOGE("remount RFS to rw err:%s\r\n",strerror(errno));
	}

	return 1;

}

void * WatchdogTimer_Thread(void *arg)
{
#ifdef ENABLE_SOFTWARE_WATCHDOG
	if(watchdog_fd < 0)
		watchdog_fd = open("/dev/watchdog",O_RDWR);
	if(watchdog_fd < 0) {
		LOGT("open watchdog failed:%s\r\n",strerror(errno));
		LOGT("mk watchdog nod\r\n");
		Mknod_Watchdog();
		watchdog_fd = open("/dev/watchdog",O_RDWR);
		if(watchdog_fd<0)
		{
			LOGT("open watchdog failed again:%s\r\nreboot system\r\n",strerror(errno));
			system("shutdown -r -n now");
		}
	}
#endif

	watchdog_thread_running = 1;

	while(watchdog_thread_en)
	{
		if(watchdog_time_out)
		{
			FeedHardwareWDT();
#ifdef ENABLE_SOFTWARE_WATCHDOG
			if(watchdog_fd > 0)
				write(watchdog_fd,"tt",2);
#endif
			watchdog_time_out--;
		}
		usleep(500000);
	}

	watchdog_thread_running = 0;

	return NULL;
}
void START_WATCHDOG_TIMER_SIG(int timeout)
{
	watchdog_time_out = timeout;
}

void STOP_WATCHDOG_TIMER_SIG(void)
{
	LOGT("watchdog stoped, time %d",watchdog_time_out);
	watchdog_time_out = 0;
}

void OPEN_WATCHDOG(void)
{
	int ret;

	Initial_map_fpga();

	EnableHardwareWDT(1);

	watchdog_thread_en = 1;
	ret = pthread_create(&watchdog_tmr_thread,NULL,WatchdogTimer_Thread,NULL);
	if(ret)
	{
		LOGE("watchdog thread create err:%s\r\n",strerror(errno));
		LOGT("reboot now\r\n");
        EnableHardwareWDT(0);
		system("shutdown -r -n now");
	}
}

void CLOSE_WATCHDOG(void)
{
#ifdef ENABLE_SOFTWARE_WATCHDOG
	if(watchdog_fd > 0)
		close(watchdog_fd);
#endif
// 仅在复位前关闭看门狗
//	// disable hardware wdt
//	EnableHardwareWDT(0);

	watchdog_thread_en = 0;

	while(watchdog_thread_running)
	{
		usleep(1000);
	}

	Deinital_map_fpga();
}

/******************* watchdog thread end *************************/

/*********************** log start ****************************/
FILE *fp_log_stdout = NULL;
FILE *fp_log_stderr = NULL;
const char *start_up_log = "/tmp/pvr_startup.log";
const char *kernel_last_log = "/tmp/kernel_msg_last.log";
const char *kernel_start_log = "/tmp/kernel_start.log";

void set_log_en(int en)
{
#if WRITE_LOG_TO_CF
    FILE *fp=0;
    int ret;
    ret = mount(RFS_MOUNT_NOD,RFS_MOUNT_DST,RFS_MOUNT_TYPE,MS_REMOUNT,0);
	if(ret<0)
	{
		LOGE("remount RFS to rw err:%s\r\n",strerror(errno));
	}

    if(en)
    {
        fp = fopen(WRITE_LOG_EN_FILE,"wr");
    	if(fp==NULL)
    	{
    		LOGE("%s: open "WRITE_LOG_EN_FILE" err\n",__func__);
        }
        else
        {
            fclose(fp);
        }
    }
    else
    {
        ret=remove(WRITE_LOG_EN_FILE);
        if(ret)
        {
            LOGE("%s: remove "WRITE_LOG_EN_FILE" err\n",__func__);
        }
    }

    ret = mount(RFS_MOUNT_NOD,RFS_MOUNT_DST,RFS_MOUNT_TYPE,MS_RDONLY|MS_REMOUNT,0);
    if(ret <0)
	{
		LOGE("remount RFS to ro failed:%s\r\n",strerror(errno));
	}
#endif
}

int is_log_en(void)
{
    int en=0;
#if WRITE_LOG_TO_CF
    FILE *fp=0;
    fp=fopen(WRITE_LOG_EN_FILE,"r");
    if(fp == NULL)
    {
        LOGT("log not enabled\n");
        en = 0;
    }
    else
    {
        LOGT("log enabled\n");
        en = 1;
    }
    
#endif
    return en;
}

void get_kernel_msg(char * full_path)
{
#if WRITE_LOG_TO_CF
	char buff[100];
	sprintf(buff,"dmesg >> %s",full_path);
	system(buff);
#endif
}

void close_log(void)
{
#if WRITE_LOG_TO_CF
	LOGT("close log\r\n");
	fflush(stdout);
	fflush(stderr);

	if(fp_log_stdout != NULL)
		fclose(fp_log_stdout);
	if(fp_log_stderr != NULL)
		fclose(fp_log_stderr);

	fp_log_stdout = NULL;
	fp_log_stderr = NULL;
#endif
}

void redirect_log(char * full_path)
{
#if WRITE_LOG_TO_CF
	int flags;

    // if log record disabled,redirect stdout/stderr to console(ttyS0)
    if(is_log_en() == 0)
    {
        return;
    }

	fflush(stdout);
	fflush(stderr);

	if((fp_log_stdout != NULL)||(fp_log_stderr != NULL))
		close_log();

	fp_log_stdout = freopen(full_path,"a+",stdout);
	if(fp_log_stdout == NULL)
	{
		LOGE("reopen fp_log_stdout error: %s\r\n",strerror(errno));
	}
	else
	{
		flags = fcntl( fileno( fp_log_stdout ), F_GETFD );
		if (flags != -1)
		{
			fcntl( fileno( fp_log_stdout ), F_SETFD, flags | FD_CLOEXEC);
		}
	}

	setbuf(stdout,NULL);

	fp_log_stderr = freopen(full_path,"a+",stderr);
	if(fp_log_stderr == NULL)
	{
		LOGE("reopen fp_log_stderr error: %s\r\n",strerror(errno));
	}
	else
	{
		flags = fcntl( fileno( fp_log_stderr ), F_GETFD );
		if (flags != -1)
		{
			fcntl( fileno( fp_log_stderr ), F_SETFD, flags | FD_CLOEXEC);
		}
	}

	setbuf(stderr,NULL);

	LOGT("redirect log to %s\r\n",full_path);
#endif
}

/*********************** log end ************************/
/*
 * if there is update lock file.update not complete,need copy back.
*/
int GetUpdateLockFileStat(void)
{
	FILE *fp = NULL;
	fp = fopen(UPDATE_LOCK_FILE,"r");
	if(fp == NULL)
	{
		return 0;
	}

	fclose(fp);
//	mount("/dev/root","/",RFS_MOUNT_TYPE,MS_REMOUNT,0);
//	remove(UPDATE_LOCK_FILE);
	return -1;
}

void ClearUpdateLockStat(void)
{
    FILE *fp = NULL;
	fp = fopen(UPDATE_LOCK_FILE,"r");
	if(fp == NULL)
	{
		return;
	}

//    mount("/dev/root","/",RFS_MOUNT_TYPE,MS_REMOUNT,0);
	fclose(fp);//SACH:W10846
    if(remove(UPDATE_LOCK_FILE) != 0)//SACH:W10846
    	{
		return;
	}
//    mount("/dev/root","/",RFS_MOUNT_TYPE,MS_RDONLY|MS_REMOUNT,0);
}

const char * PVREXE="/opt/pvr" ;
const char * PVRBACK="/opt/pvr.tmp";

int child_flag=0;
int signal_child_stat=EXIT_CODE_NORMAL;

int pid=-1;	
int nExit=0;
void Signal_Usr1(int signo)
{
	nExit=1;

	if (pid>0)
	{	
		kill(pid, SIGKILL);
	}	
}

void Signal_Child(int signo)
{
	int err = 0;
	int stat = 0;
	int tmp;

	if(nExit==1)
		return;

	err=waitpid(pid, &stat,WNOHANG);

	if(err)
	{
		redirect_log(start_up_log);
		if(WIFSIGNALED(stat))
		{
			LOGT("chiled is signaled\r\n");
			tmp = WTERMSIG(stat);
			LOGT("termsig %d\r\n",tmp);

			if(GetUpdateLockFileStat())
				signal_child_stat = EXIT_CODE_INVALID;
			else
				signal_child_stat = EXIT_CODE_PVR_ERROR;
		}
		else
		{
			signal_child_stat = WEXITSTATUS(stat);
		}
		pid = -1;
	}
}

void check_child_signal(int stat)
{
	char tmp_str[200];
	FILE *fp = NULL;

	LOGT("pvr exit_code: %d\r\n",stat);

	switch(stat)
	{
		case EXIT_CODE_PVR_ERROR:
            // dump core message
			get_kernel_msg(kernel_last_log);
		case EXIT_CODE_NORMAL:
			break;
		case EXIT_CODE_UPDATE:
            // NOTE: check is pvr update?
			OPEN_WATCHDOG();
			START_WATCHDOG_TIMER_SIG(120);
// do not use .t tmp file to do update. update file in pvr and remount/reboot
// to adjust old update method before v2.1
#if 0
			sprintf(tmp_str,"%s%s%s",UPDATE_DST_FILE_PATH,UPDATE_FILE_NAME,UPDATE_TMP_FILE_DES);
			fp = fopen(tmp_str,"r");
			if(fp != NULL)
			{
				fclose(fp);
				LOGT("pvr update file found\r\n");
				LOGT("remove %s%s & rename %s%s%s to %s%s\r\n"
						,UPDATE_DST_FILE_PATH,UPDATE_FILE_NAME
						,UPDATE_DST_FILE_PATH,UPDATE_FILE_NAME,UPDATE_TMP_FILE_DES
						,UPDATE_DST_FILE_PATH,UPDATE_FILE_NAME);
				sprintf(tmp_str,"rm %s%s",UPDATE_DST_FILE_PATH,UPDATE_FILE_NAME);
				system(tmp_str);
				sync();
				sync();
				sprintf(tmp_str,"mv %s%s%s %s%s"
						,UPDATE_DST_FILE_PATH,UPDATE_FILE_NAME,UPDATE_TMP_FILE_DES
						,UPDATE_DST_FILE_PATH,UPDATE_FILE_NAME);
				system(tmp_str);
			}
			LOGT("update complete\r\n");
#endif
            ClearUpdateLockStat();
			LOGT("daemon mount system to ro\r\n");
			if(mount(RFS_MOUNT_NOD,RFS_MOUNT_DST,RFS_MOUNT_TYPE,MS_RDONLY|MS_REMOUNT,0))
			{
				LOGE("remount failed:%s\r\n",strerror(errno));
    			STOP_WATCHDOG_TIMER_SIG();
//	    		CLOSE_WATCHDOG();
//                EnableHardwareWDT(0);
//                LOGT("use shutdown to reboot");
//                system("shutdown -r -n now");
			}
            else
            {
    			STOP_WATCHDOG_TIMER_SIG();
//	    		CLOSE_WATCHDOG();
            }
            LOGE("wait wdt to reboot\r\n");
            while(1)
            {
                sleep(1);
            }
		case EXIT_CODE_REBOOT:
            // NOTE: check is deamon update?
			OPEN_WATCHDOG();
			START_WATCHDOG_TIMER_SIG(120);
// do not use .t tmp file to do update. update file in pvr and remount/reboot
// to adjust old update method before v2.1
#if 0
			sprintf(tmp_str,"%s%s%s",UPDATE_DST_FILE_PATH,UPDATE_DAEMON_NAME,UPDATE_TMP_FILE_DES);
			fp = fopen(tmp_str,"r");
			if(fp != NULL)
			{
				fclose(fp);
				LOGT("daemon update file found\r\n");
				LOGT("remove %s%s & rename %s%s%s to %s%s\r\n"
						,UPDATE_DST_FILE_PATH,UPDATE_DAEMON_NAME
						,UPDATE_DST_FILE_PATH,UPDATE_DAEMON_NAME,UPDATE_TMP_FILE_DES
						,UPDATE_DST_FILE_PATH,UPDATE_DAEMON_NAME);
				sprintf(tmp_str,"rm %s%s",UPDATE_DST_FILE_PATH,UPDATE_DAEMON_NAME);
				system(tmp_str);
				sync();
				sync();
				sprintf(tmp_str,"mv %s%s%s %s%s"
						,UPDATE_DST_FILE_PATH,UPDATE_DAEMON_NAME,UPDATE_TMP_FILE_DES
						,UPDATE_DST_FILE_PATH,UPDATE_DAEMON_NAME);
				system(tmp_str);
			}
#endif
			STOP_WATCHDOG_TIMER_SIG();
			CLOSE_WATCHDOG();
            ClearUpdateLockStat();

            LOGE("wait wdt to reboot\r\n");
            while(1)
            {
                sleep(1);
            }
//            EnableHardwareWDT(0);
//			LOGT("update reboot system now\r\n");
//			system("shutdown -r -n now");
			// exit daemon
//			exit(0);
//			break;
        case EXIT_CODE_INVALID:
		default:
			LOGT("unexpected exit code. copy pvr back\r\n");
            // NOTE: add watchdog thread
			OPEN_WATCHDOG();
			START_WATCHDOG_TIMER_SIG(120);
			// execv error: copy pvr.tmp back to pvr & chmod
			// remount
			mount("/dev/root","/",RFS_MOUNT_TYPE,MS_REMOUNT,0);
			sprintf(tmp_str,"cp %s %s",PVRBACK,PVREXE);
			LOGT("%s\r\n",tmp_str);
			system(tmp_str);

			sprintf(tmp_str,"chmod +x %s",PVREXE);
			LOGT("%s\r\n",tmp_str);
			system(tmp_str);
			sync();
            ClearUpdateLockStat();
			mount("/dev/root","/",RFS_MOUNT_TYPE,MS_RDONLY|MS_REMOUNT,0);
			sync();
			STOP_WATCHDOG_TIMER_SIG();
			CLOSE_WATCHDOG();
			break;
	}
}

int ChildProcess(void)
{
	//do exec
	char *cm[2];
	char tmp_str[0x64];

	cm[0]="PVR";
	cm[1]=NULL;

	LOGT("Exec %s\r\n",PVREXE);

	execv(PVREXE, cm);

	LOGT("Exec %s error.copy pvr back\r\n",PVREXE);

	// execv error: copy pvr.tmp back to pvr & chmod
	// remount
	mount("/dev/root","/",RFS_MOUNT_TYPE,MS_REMOUNT,0);
	sprintf(tmp_str,"cp %s %s",PVRBACK,PVREXE);
	LOGT("%s\r\n",tmp_str);
	system(tmp_str);

	sprintf(tmp_str,"chmod +x %s",PVREXE);
	LOGT("%s\r\n",tmp_str);
	system(tmp_str);
	sync();
	sync();
	mount("/dev/root","/",RFS_MOUNT_TYPE,MS_RDONLY|MS_REMOUNT,0);

	exit(EXIT_CODE_PVR_ERROR);	
}
#include <termios.h>
int main(int argc, char *argv[])
{
	struct termios ttySave;
	tcgetattr(0, &ttySave);

	get_kernel_msg(kernel_start_log);

	if (signal(SIGUSR1, Signal_Usr1))
	{
		//LOGT("REQUEST SIGNAL FAILED.\n");
	}	
	if (signal(SIGCHLD, Signal_Child))
	{
		//LOGT("REQUEST SIGNAL FAILED.\n");
	}

	redirect_log(start_up_log);

	signal_child_stat=EXIT_CODE_NORMAL;

	while (nExit==0)
	{
		
		if (pid<0)
		{
			check_child_signal(signal_child_stat);

			pid=fork();
		
			if (pid<0)
			{
				//LOGT("Fork ERROR ,never should happen\n");
				break;
			}	
			if (pid==0)
			{
				ChildProcess();
			}	
		}
		pause();
	}
	sleep(1);//Wait child exit;
	tcsetattr(0, TCSANOW, &ttySave);
	exit(0);
	return (-1);
}
