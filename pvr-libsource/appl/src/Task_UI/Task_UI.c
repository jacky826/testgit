/*
----------------------------------------------------------------------
File		:	Task_UI.c
Purpose	:
----------------------------------------------------------------------
Version-Date			:	2007.1.26
Author-Explanation		:	Shadow
----------------------------------------------------------------------
*/

#include <config.h>	//Config for platform
#include <UI_Message.h>	//UI message
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>    //haiqin add
#include <Task_UI/task_ui_var.h>
#include <Task_UI/pvr_ui_func.h>
#include <pvr_types.h>
#include <Key.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../avi/encoder.h"
#include "../../avi/decoder.h"

#include <errno.h>      //errno

#include <signal.h>		// signal

#include "UserMsg.h"
#include "Task_UI/ui_misc.h"

//! ���Ź�
//! [0]
/******************************watch dog part*********************/
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mount.h>

#include <pthread.h>

//! ʹ��fork�����ӽ���ʱ�Ľ���pid
static pid_t pid_watchdog_tmr = -1;

//! ���ƿ��Ź��߳�ι����������
unsigned int watchdog_tmr_tick = 0;     // �ñ�����Ϊ0ʱ����ʾ��Ҫ����ι�������������߳���ͨ��FEED_WATCHDOG����
unsigned int watchdog_time_out = 0;     // �趨һ��ʱ�����Զ�ι����ʱ���޶���������500msΪ��λ
unsigned int watchdog_time_out_cnt = 0; // ÿ��ι���������������ӵ�watchdog_time_outʱ�����ٽ���ι��

//! ʹ�ö��̷߳�ʽʱ�������Ŀ��Ź��߳�
pthread_t watchdog_tmr_thread;

//! ���ƿ��Ź��߳�����״̬�ı���
static int watchdog_thread_running = 0; // �ñ�����ʾ���Ź��߳����������У�����ֹͣ���Ź��߳�ʱ���ж����߳��Ƿ��˳����
static int watchdog_thread_en = 0;      // �ñ������ڿ��ƿ��Ź�����ʹ��

//! ���Ź��̻߳ص�����
void * WatchdogTimer_Thread(void *arg);

//! ʹ��������Ź����ڼ�ⲻ�����Ź��ļ���ʱ�򣬴������ļ��ĺ���
int Mknod_Watchdog(void);

/**
 * @brief START_WATCHDOG_TIMER_SIG
 * ��ʼ���Ź��߳�/���̣�����һ�γ�ʱʱ���ڣ�����ι����ʹ��Ӳ�����Ź���������
 * ע�⣺ֻ��ʹ���̵߳�ʱ�򣬿��Կ������̵߳�ι����ʱʱ�䣬ʹ���ӽ��̵ķ�ʽʱ��ֻ��ͨ����������ֹ�ӽ���
 * @param timeout
 * ��ʱ�趨ʱ��
 */
void START_WATCHDOG_TIMER_SIG(unsigned int timeout)
{
#ifdef DEBUG_WATCHDOG_THREAD
    // ʹ���̷߳�ʽʱ��ͨ���趨ȫ�ֱ�����ֵ������ι���̵߳�����
	watchdog_time_out = timeout;
	watchdog_time_out_cnt = 0;
#else
    // ʹ��fork���̵ķ�ʽʱ�������ӽ��̿�ʼ���Ź�ι������
	pid_t pid = -1;

	FUNC_ENTER;

	FEED_WATCHDOG();

    // ���Ѿ��п��Ź��ӽ��̵�����£�����ֹ�ӽ���
	if(pid_watchdog_tmr > 0)
	{
		LOGT("watchdog tmr not closed\r\n");
		kill(pid_watchdog_tmr,SIGKILL);
		pid = wait(0);
		LOGT("stop watchdog tmr pid=%d\r\n",pid);
	}

    // �������Ź��ӽ���
	pid_watchdog_tmr = fork();

	switch(pid_watchdog_tmr)
	{
		case -1:
			// error
			ERR("start watchdog timer error\r\n");
			return;
		case 0:
			// child
			LOGT("Enter Watchdog Timer Child Progress\r\n");
            // ���ӽ����г�������ι����ֱ��������ǿ�ƽ��ӽ�����ֹ
			while(1)
			{
				usleep(500000);
				FEED_WATCHDOG();
			}
			exit(0);
		default:
			// parent
			break;
	}
#endif
}

/**
 * @brief STOP_WATCHDOG_TIMER_SIG
 * ��ֹ���Ź��Զ�����ι���߳�/���̡�
 * �����ӽ��̷�ʽ�Զ�ι������������̽����쳣״̬���޷�����STOP_WATCHDOG_TIMER_SIG����
 * �������ӽ���һֱ�������У��뿴�Ź������ͼ��㣡�
 */
void STOP_WATCHDOG_TIMER_SIG(void)
{
#ifdef DEBUG_WATCHDOG_THREAD
	LOGT("watchdog timer count : %d\r\n",watchdog_time_out_cnt);
	watchdog_time_out = 0;
#else
	pid_t pid = -1;

	FUNC_ENTER;

	if(pid_watchdog_tmr <= 0)
	{
		LOGT("invalid watchdog tmr pid\r\n");
		return;
	}

	LOGT("close watchdog tmr\r\n");

	kill(pid_watchdog_tmr,SIGKILL);

	pid = wait(0);

	LOGT("stop watchdog tmr pid=%d\r\n",pid);

	pid_watchdog_tmr = -1;
#endif
}

//! ��ϵͳ״̬�쳣ʱ��ͨ��ʹ��Ӳ�����Ź����и�λ�ı�־
int need_to_reboot = 0;

//! ���쳣ʱ���øú�����ͨ�����Ź�������λ
void request2reboot(void)
{
#if 1
	FUNC_ENTER;
	LOGE("System reboot by WDT\r\n");
//    // disable hardware wdt
//	EnableHardwareWDT(0);
//	system("shutdown -r -n now");
	need_to_reboot = 1;
#else

	LOGT("post reboot sig\r\n");
	need_to_reboot = 1;
#endif
}

//! ������Ź��ļ�������
int watchdog_fd = -1;

/**
 * @brief OPEN_WATCHDOG
 * �򿪿��Ź���
 * 1��ʹ�ö��߳�ʱ���������Ź��̣߳�ι����ʵ�ʲ����ڿ��Ź��߳��н��У�������ʹ���߳�ι����
 * 2��ʹ��fork �ӽ���ʱ��ʹ��Ӳ�����Ź������Ƽ���
 * 3��ʹ��������Ź�ʱ���򿪿��Ź��ļ�����ι��ʱд�������ּ���ι���������ã�
 */
void OPEN_WATCHDOG(void)
{
#ifdef DEBUG_WATCHDOG_THREAD
	int ret;

    watchdog_thread_en = 1;//�������߳�WatchdogTimer_Thread������ι��
	ret = pthread_create(&watchdog_tmr_thread,NULL,WatchdogTimer_Thread,NULL);
	if(ret)
	{
		LOGE("watchdog thread create err:%s\r\n",strerror(errno));
		LOGE("reboot now\r\n");
	    // disable hardware wdt
    	EnableHardwareWDT(0);
		system("shutdown -r -n now");
	}

#else
	// Enable Hardware WDT
	EnableHardwareWDT(1);

#ifdef ENABLE_SOFTWARE_WATCHDOG
	if(watchdog_fd < 0)
		watchdog_fd = open("/dev/watchdog",O_RDWR);
	if(watchdog_fd < 0) {
//		LOGE("open watchdog failed:%s\r\nreboot system\r\n",strerror(errno));
//		system("shutdown -r -n now");
		LOGE("open watchdog failed:%s\r\n",strerror(errno));
		LOGT("mk watchdog nod\r\n");
        Mknod_Watchdog();//����watchdog�豸�ڵ�
        watchdog_fd = open("/dev/watchdog",O_RDWR);//���豸�ļ�
		if(watchdog_fd<0)
		{
			LOGE("open watchdog failed again:%s\r\nreboot system\r\n",strerror(errno));
			system("shutdown -r -n now");
		}
	}
#endif

#endif
}

/**
 * @brief FEED_WATCHDOG
 * ���Ź�ι������
 * 1��ʹ�ö��߳�ʱ��watchdog_tmr_tick��������ι���߳��иñ������㣬����ι������
 * 2��ʹ��fork�ӽ���ʱ�����ڲ���Ҫ�����ӽ��̣�ֱ�ӽ���ι������
 * 3��ʹ��������Ź�ʱ���Կ��Ź��ļ�����д����
 */
void FEED_WATCHDOG(void)
{
#ifdef DEBUG_WATCHDOG_THREAD
	watchdog_tmr_tick++;
#else
	FeedHardwareWDT();
#ifdef ENABLE_SOFTWARE_WATCHDOG
	if(need_to_reboot == 0 && watchdog_fd > 0)
//	if(watchdog_fd > 0)
		write(watchdog_fd,"tt",2);
#endif

#endif
}

/**
 * @brief CLOSE_WATCHDOG
 * �رտ��Ź�����
 * ͨ��ȫ�ֱ������رտ��Ź��߳�
 * ����ʹ��fork�ӽ��̷�ʽ�����Ŀ��Ź����̣�ʹ��STOP_WATCHDOG_TIMER_SIGֹͣ����
 */
void CLOSE_WATCHDOG(void)
{
#ifdef ENABLE_SOFTWARE_WATCHDOG
    // ��Ҫ��һ�����ԣ�������Ź��ر��ļ�������֮���ƺ��Ի�����
	if(watchdog_fd > 0)
		close(watchdog_fd);
#endif

// ���ڸ�λǰ�رտ��Ź�
//	// disable hardware wdt
//	EnableHardwareWDT(0);

#ifdef DEBUG_WATCHDOG_THREAD
	watchdog_thread_en = 0;
 
	while(watchdog_thread_running)
	{
		usleep(1000);
	}
#endif

	FUNC_EXIT;
}

/**
 * @brief WatchdogTimer_Thread
 * ���̷߳�ʽ�����Ŀ��Ź��̣߳�ʹ�ö��̷߳�ʽʱ��ι�������ղ������ڴ˴����
 * @param arg
 * ���Բ���
 * @return
 * ���Է���
 */
void * WatchdogTimer_Thread(void *arg)
{
#ifdef DEBUG_WATCHDOG_THREAD
	// Enable Hardware WDT
	EnableHardwareWDT(1);

#ifdef ENABLE_SOFTWARE_WATCHDOG
	if(watchdog_fd < 0)
		watchdog_fd = open("/dev/watchdog",O_RDWR);
	if(watchdog_fd < 0) {
//		LOGE("open watchdog failed:%s\r\nreboot system\r\n",strerror(errno));
//		system("shutdown -r -n now");
		LOGE("open watchdog failed:%s\r\n",strerror(errno));
		LOGT("mk watchdog nod\r\n");
		Mknod_Watchdog();
		watchdog_fd = open("/dev/watchdog",O_RDWR);
		if(watchdog_fd<0)
		{
			LOGE("open watchdog failed again:%s\r\nreboot system\r\n",strerror(errno));
			system("shutdown -r -n now");
		}
	}
#endif

    // ȫ�ֱ�����־��λ
	watchdog_thread_running = 1;

    // ���Ź��߳���ѭ��
    while(watchdog_thread_en)//watchdog_thread_en������void OPEN_WATCHDOG(void)�����н��г�ʼ��
	{
        // ϵͳ�쳣ʱ��������ι�����ȴ�Ӳ�����Ź���λ
		if(need_to_reboot==0)
		{
            // ���߳��е�����FEED_WATCHDOG��ͨ��START_WATCHDOG_TIMER_SIG������һ��ʱ���Զ�ι���󣬽���ι������
			if((watchdog_tmr_tick)||(watchdog_time_out))
			{
				FeedHardwareWDT();
#ifdef ENABLE_SOFTWARE_WATCHDOG
	//			if(need_to_reboot == 0 && watchdog_fd > 0)
				if(watchdog_fd > 0)
					write(watchdog_fd,"tt",2);
#endif
				watchdog_tmr_tick = 0;

				if(watchdog_time_out_cnt<watchdog_time_out)
				{
					watchdog_time_out_cnt++;
				}
				else
				{
					if(watchdog_time_out)
						LOGE("watchdog timeout:%d\r\n",watchdog_time_out_cnt);
#if 0 // debug only
					// 08-27-2012 add disable hardware wdt
					LOGE("disable watchdog\r\n");
					EnableHardwareWDT(0);
#endif
					watchdog_time_out = 0;					
				}
			}
		}

        // ���Թ��ܣ����USBд״̬����release��δ����
		ParseMUSBStatus();

        // ��ʱ500ms����һ��ι��
		usleep(500000);
	}

    // �ڵ�����STOP_WATCHDOG֮���˳���ѭ��������running��־
	watchdog_thread_running = 0;

#endif
	return NULL;
}

/**
 * @brief Mknod_Watchdog
 * ʹ��������Ź�����ʱ�����ϵͳ��û�п��Ź��豸�ļ����������Ź��豸�ļ�
 * @return
 */
int Mknod_Watchdog(void)
{
	int ret;
	dev_t dev;

	FUNC_ENTER;

    //�����ļ�ϵͳ���¹���Ϊ�ɶ�д�ļ�ϵͳ
	ret = mount(RFS_MOUNT_NOD,RFS_MOUNT_DST,RFS_MOUNT_TYPE,MS_REMOUNT,0);
	if(ret<0)
	{
		ERR("remount RFS to rw err:%s\r\n",strerror(errno));
	}

    ret = remove("/dev/watchdog");//�Ƴ��豸�ڵ�
	if(ret<0)
	{
		LOGE("remove watchdog error:%s\r\n",strerror(errno));
	}

	LOGT("mknod /dev/watchdog c 10 130\r\n");
    system("mknod /dev/watchdog c 10 130");//����watchdog�豸�ڵ�

    //�����ļ�ϵͳ���¹���Ϊֻ���ļ�ϵͳ
	ret = mount(RFS_MOUNT_NOD,RFS_MOUNT_DST,RFS_MOUNT_TYPE,MS_RDONLY|MS_REMOUNT,0);
	if(ret<0)
	{
		ERR("remount RFS to rw err:%s\r\n",strerror(errno));
	}

//	exit_code = EXIT_CODE_UPDATE;

	FUNC_EXIT;

	return 1;
}

/******************************watch dog part end******************/
//! [0]

//! ��־��¼
//! [1]
/****************************** sys log part **********************/
#include <sys/statfs.h>
#include <sys/stat.h>
#include <sys/time.h>

FILE *fp_log_stdout = NULL;     // ��׼����ļ�ָ�룬�ض���cf�����ļ�ʱʹ��
FILE *fp_log_stderr = NULL;     // ��׼��������ļ�ָ�룬�ض���cf�����ļ�ʱʹ��

/*
static void fileSetCloseOnExec(FILE * file)
{
	int flags;
	if(file != NULL)
	{
		flags = fcntl( fileno( file ), F_GETFD );
		if (flags != -1)
		{
			fcntl( fileno( file ), F_SETFD, flags | FD_CLOEXEC);
		}
	}
}
*/

/**
 * @brief redirect_log_console
 * �ر���־���ܺ󣬽���׼����ͱ�׼��������ض���console�����ڣ�
 */
void redirect_log_console(void)
{
    int flags;

    // [1] flush���ر���־��¼�ļ�
	fflush(stdout);
	fflush(stderr);

	if((fp_log_stdout != NULL)||(fp_log_stderr != NULL))
		close_log();

    // [2] ����׼����ض���console
	fp_log_stdout = freopen("/dev/console","w",stdout);
	if(fp_log_stdout == NULL)
	{
		LOGE("%s: reopen fp_log_stdout error: %s\r\n",__func__,strerror(errno));
	}
	else
	{
		flags = fcntl( fileno( fp_log_stdout ), F_GETFD );
		if (flags != -1)
		{
			if(fcntl( fileno( fp_log_stdout ), F_SETFD, flags | FD_CLOEXEC)== -1)//SACH:W10843
			{
				return;
			}
		}
	}

	setbuf(stdout,NULL);

    // [3] ����׼��������ض���console
	fp_log_stderr = freopen("/dev/console","w",stderr);
	if(fp_log_stderr == NULL)
	{
//		LOGE("%s: reopen fp_log_stderr error: %s\r\n",__func__,strerror(errno));//SACH:W10909
	}
	else
	{
		flags = fcntl( fileno( fp_log_stderr ), F_GETFD );
		if (flags != -1)
		{
			if(fcntl( fileno( fp_log_stderr ), F_SETFD, flags | FD_CLOEXEC)== -1)//SACH:W10843
			{
				return;
			}
		}
	}

	setbuf(stderr,NULL);

    FUNC_EXIT;
}

/**
 * @brief set_log_en
 * ʹ��/�ر���־��¼����
 * ͨ����/opt/Ŀ¼�´���һ���ļ�����ʶ��ǰ����log���ܣ������Ҫ�ر���־��¼���ܣ��򽫸��ļ�ɾ��
 * @param en
 * ��0 ��ʾʹ��log����
 * 0 ��ʾ�ر�log����
 */
void set_log_en(int en)
{
#if WRITE_LOG_TO_CF
    FILE *fp=0;
    int ret;

    // Ĭ���ļ�ϵͳΪֻ���������Ҫ�Ƚ��ļ�ϵͳ��Ϊ�ɶ�д
    ret = mount(RFS_MOUNT_NOD,RFS_MOUNT_DST,RFS_MOUNT_TYPE,MS_REMOUNT,0);
	if(ret<0)
	{
		LOGE("remount RFS to rw err:%s\r\n",strerror(errno));
	}

    // ���ݴ������������ɾ��log��¼ʹ���ļ���
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
		sync();
		sync();
    }
    else
    {
        ret=remove(WRITE_LOG_EN_FILE);
        if(ret)
        {
            LOGE("%s: remove "WRITE_LOG_EN_FILE" err\n",__func__);
        }
        // �ر�log��¼���ܺ���Ҫ��������Ϣ���¶���console
        redirect_log_console();
		sync();
		sync();
    }

    // ���½��ļ�ϵͳ��Ϊֻ��
    ret = mount(RFS_MOUNT_NOD,RFS_MOUNT_DST,RFS_MOUNT_TYPE,MS_RDONLY|MS_REMOUNT,0);
    if(ret <0)
    {
        LOGE("remount RFS to ro failed:%s\r\n",strerror(errno));
    }
#endif
}

/**
 * @brief is_log_en
 * ����/opt/Ŀ¼���Ƿ���logʹ�ܱ�ʶ�ļ����жϵ�ǰ�Ƿ�ʹ��log��¼����
 * @return
 * 0 �ر�log����
 * 1 ʹ��log����
 */
int is_log_en(void)
{
    int en=0;
#if WRITE_LOG_TO_CF
    FILE *fp=0;
    fp=fopen(WRITE_LOG_EN_FILE,"r");
    if(fp == NULL)
    {
        en = 0;
    }
    else
    {
        en = 1;
    }
    
#endif
    return en;
}

/**
 * @brief redirect_log
 * ��log�ض����ļ�����δ����cf��ʱ���ض���tmp�ļ�ϵͳ���ļ��У�����cf��֮���ض���cf��
 * @param dest_path
 * �ļ�����·����Ϊ����·��
 * @param dest_file
 * �ļ���
 * @return
 * 0
 */
int redirect_log(char *dest_path,char *dest_file)
{
#if WRITE_LOG_TO_CF
	char full_path[MAX_FS_PATH];
	int flags;
	struct tm *local;
	time_t t;

    // [0] �ض���ǰ���ж��Ƿ�ʹ�ܽ���־��¼��cf������
    // if log record disabled,redirect stdout/stderr to console(ttyS0)
    if(is_log_en() == 0)
    {
//        LOGT("log not enabled\n");
        return 0;
    }
//    LOGT("log enabled\n");

    // [1] �ض���ǰ���ȹر��Ѿ��򿪵��ļ�
	fflush(stdout);
	fflush(stderr);

	if((fp_log_stdout != NULL)||(fp_log_stderr != NULL))
		close_log();

    // [2] ���ݴ��������ñ��ض�����ļ�ȫ��
    sprintf(full_path,"%s%s",dest_path,dest_file);

    // [3] ����׼��������ļ�
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

    setbuf(stdout,NULL);//�ر�stdout������

    // [4] ����׼������������ļ�
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

    setbuf(stderr,NULL);//�ر�stderr������

	LOGT("redirect stdout to fileno: %d\r\n",fileno( fp_log_stdout ));

	LOGT("redirect stderr to fileno: %d\r\n",fileno( fp_log_stderr ));

	LOGT("redirect log to %s\r\n",full_path);

    // �ض�����ɺ󣬼�¼��ǰʱ���Ա��ڵ��Բ鿴
	t = time(NULL);
	//Get local time
	local = localtime(&t);
	if(local==NULL)
	{
		LOGE("get local time err\r\n");
	}
	LOGT("current time: %d-%d-%d %d:%d:%d\r\n",
			local->tm_year+1900,
			local->tm_mon+1,
			local->tm_mday,
			local->tm_hour,
			local->tm_min,
			local->tm_sec
		);
#endif
    return 0;
}

/**
 * @brief close_log
 * �ر���־�ض����ļ�
 */
void close_log(void)
{
#if WRITE_LOG_TO_CF
	LOGT("close log\r\n");
//	FEED_WATCHDOG();
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

/**
 * @brief move_log
 * ������ʱĿ¼�е�log�ƶ���cf���л�֮���ڻ�ȡ�ϵ��ں���Ϣ�����߸�ʽ��cf������ʱ�����õ�
 * @param src_path
 * log�ļ���ŵ�ԴĿ¼
 * @param dst_path
 * Ŀ��Ŀ¼
 * @param filename
 * log�ļ���
 * @return
 * 0
 */
int move_log(char * src_path,char *dst_path,char *filename)
{
#if WRITE_LOG_TO_CF
	char buf[MAX_FS_PATH];
	FILE *fp = NULL;

    if(is_log_en() == 0)
        return 0;

    sprintf(buf,"%s%s",src_path,filename);//�ϳ�Դ�ļ���ȫ·��
	fp = fopen(buf,"r");
    if(fp != NULL)//���Դ�ļ��Ƿ����
	{
		fclose(fp);
		LOGT("%s found,move to %s\r\n",buf,dst_path);

        // ͨ��systemϵͳ���ã�ʵ��mv��������Ҫע����ǣ�����systemϵͳ�����������ģ���Ҫ��ӿ��Ź��Զ�ι��
		sprintf(buf,"mv %s%s %s%s",src_path,filename,dst_path,filename);
		LOGT("%s\r\n",buf);
		START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_MOVE_LOG);
		system(buf);
		sync();
		sync();
		STOP_WATCHDOG_TIMER_SIG();
	}
#endif
 
	return 0;
}

/**
 * @brief cat_log
 * ��һ��log�ļ��е����ݣ���ӵ���һ���ļ��У�ϵͳ����ʱʹ�ã�ϵͳ����ʱ����cf������ǰ����־�Ǽ�¼����ʱ�ļ�ϵͳ�е�
 * @param src_path
 * ԴĿ¼
 * @param dst_path
 * Ŀ��Ŀ¼
 * @param filename
 * log�ļ���
 * @return
 * 0
 */
int cat_log(char * src_path,char *dst_path,char *filename)
{
#if WRITE_LOG_TO_CF
	char buf[MAX_FS_PATH];

//    FUNC_ENTER;

    if(is_log_en() == 0)
        return 0;

    // ͨ��ϵͳ���ã�ʵ��cat����
	sprintf(buf,"cat %s%s >> %s%s",src_path,filename,dst_path,filename);
	LOGT("%s\r\n",buf);
	START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_MOVE_LOG);
	system(buf);
	sprintf(buf,"rm %s%s",src_path,filename);
	system(buf);
	sync();
	sync();
	STOP_WATCHDOG_TIMER_SIG();
#endif

	return 0;
}

//  log file size ctrl. just check log file in cfcard
/**
 * @brief check_log_size
 * ���log�ļ���С���ܣ���鵱ǰд���log�ļ�(pvr_log.log)��С�������������С��
 * ɾ�������ļ�(pvr_log.old.log)��������ǰ�ļ�����Ϊ���ļ������µ���־������¼
 * log�ļ���¼�����лᱣ�������ļ�����ֹ��ɾ���ļ�֮�󣬶�ʧ�ϴ����õ���Ϣ��
 * @return
 * -1: �������г������⣬cf������λ
 * 0: ������
 */
int check_log_size(void)
{
#if WRITE_LOG_TO_CF
	struct stat file_stat;
	unsigned long long sizeinbyte;
	char buf[MAX_FS_PATH];

    // ����ļ�ǰ���ж�cf����λ״̬
	if(TESTBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_REMOVE))
	{
		return -1;
	}

    // ��ʹ��log��¼ʱ��ֱ�ӷ���
    if(is_log_en() == 0)
        return 0;

    sprintf(buf,"%s%s",LOG_PATH_CFCARD,LOG_FILE_LOG);

    // ��鵱ǰд���log�ļ�
    stat(buf,&file_stat);
	sizeinbyte = file_stat.st_size/1024/1024;

	if(sizeinbyte > MAX_LOG_FILE_SIZE)
	{
		LOGT("log file reach max size\r\n");

        // ʹ��ϵͳ�������ɾ���͸������ܣ�ϵͳ����Ϊ������������Ҫ�򿪿��Ź����Զ�ι��
		sprintf(buf,"rm %s%s",LOG_PATH_CFCARD,LOG_FILE_LOG_OLD);
		LOGT("%s\r\n",buf);
		START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_MOVE_LOG);
		system(buf);
		sync();
		sync();
		STOP_WATCHDOG_TIMER_SIG();

		close_log();

		sprintf(buf,"mv %s%s %s%s",LOG_PATH_CFCARD,LOG_FILE_LOG,LOG_PATH_CFCARD,LOG_FILE_LOG_OLD);
		LOGT("%s\r\n",buf);
		START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_MOVE_LOG);
		system(buf);
		sync();
		sync();
		STOP_WATCHDOG_TIMER_SIG();

        // ����log�ض����½��ļ�
		redirect_log(LOG_PATH_CFCARD,LOG_FILE_LOG);
	}

#endif

	return 0;
}

/**
 * @brief get_kernel_msg
 * ��ȡ�ϵ�ʱ���ں���Ϣ��ע�⣬ֻ���汾���ϵ���ں���Ϣ
 * @param full_path
 * �����ں���Ϣ���ļ�·��ȫ��
 */
void get_kernel_msg(char * full_path)
{
#if WRITE_LOG_TO_CF
	char buff[MAX_FS_PATH];

	START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_MOVE_LOG);

    // ʹ��systemϵͳ���ã���dmesg������������д��log�ļ�
	sprintf(buff,"dmesg >> %s",full_path);
	system(buff);

	STOP_WATCHDOG_TIMER_SIG();
#endif
}


/****************************** sys log part end ******************/
//! [1]

//! ϵͳ�����̴�����Ϣ����
//! [2]
/*
 ----	System Provide Handler
 */
int rs422fd = -1;   // rs422��ȫ���ļ����������򿪡��رգ���ȡ422��Ϣʱʹ��

/**
 * @brief SystemMsgHandle_Key
 * ��������
 * Ŀǰû�а�����Ϣ�ڸú�������
 * @param nMsgData
 * @return
 */
static TMsgHandleStat SystemMsgHandle_Key(unsigned short nMsgData)
{
    TMsgHandleStat retval=MsgNeedDispatch;

    return retval;
}

/**
 * @brief SystemMsgHandle_Decoder
 * ������Ϣ�������������ǰ���ڽ����ļ����ţ�������������᲻���д�������Ϣ���ݸ��ļ����Ž��洦��
 * �����ǰ�����ļ����Ž��棬��ֹͣ�ļ����š�
 * @param nMsgData
 * @return
 */
static TMsgHandleStat SystemMsgHandle_Decoder(unsigned short nMsgData)
{
	if (pUISystem->nMenuIdx==MENUPLAYBACK){
        LOGT("1\r\n");          // �����õ�����Ϣ��ȷ����Ϣ���յ���λ��
        StopParseMUSBStatus();  // �����õ�����Ϣ�����usbд��״̬
        return MsgNeedDispatch; // �����ļ����Ź����н��յ�������Ϣ�����ļ����Ž��洦��
	}else{
        // ���ļ����Ž�����յ�������Ϣ��ֹͣ���ţ��������״̬����ʾ��ʾ��Ϣ
        StopTmpOSD();

		ClearPlayState();
		ShowDecoderMessage(nMsgData);

        LOGT("2\r\n");          // �����õ�����Ϣ��ȷ����Ϣ���յ���λ��
        StopParseMUSBStatus();  // �����õ�����Ϣ�����usbд��״̬
	}
    return MsgHandled;          // ������Ϣ�Ѿ�������
}

//! ������Ҫ�˳�Ӧ�ó����ȫ�ֱ���
int request_exit_main_task = 0;
//! δ֧�ֱ���ʱ�ļ��л�����ʱ��ʹ�õ�ȫ�ֱ�����������Ҫ���¿�ʼ�ļ�¼��
int restart_record_mark = 0;

/**
 * @brief SystemMsgHandle_Encoder
 * ������Ϣ�����������յ�������Ϣ��ֹͣ���������쳣���������ǰ����״̬��ʶ��׼���˳�Ӧ�ó���
 * @param nMsgData
 * ������Ϣ����
 * @return
 * Ĭ�Ϸ�����Ϣ�Ѵ���
 */
static TMsgHandleStat SystemMsgHandle_Encoder(unsigned short nMsgData)
{
	int ret;
#if USE_SERIAL_MSG_422
	TUserReply tReply;
#endif
    // restart msg process
	// if not defined macro "FILE_SWITCH",stop record first,then restart record
    // ������Ϣ��ֹͣ���������쳣��������Ҫֹͣ��ǰ����
    switch(nMsgData)
    {
    case MSG_PARA_ENC_STOP:
    default:
        ClearRecordState(); // �����ǰ����ϵͳ״̬
#ifndef ENABLE_FILE_SWITCH
        // δʹ�ñ������ļ��л�����ʱ�����ļ��л�ʱ����ֹͣ���룬֮���յ�����ֹͣ��Ϣ���ڴ˴����¿�ʼ
        if(restart_record_mark)
        {
            if ((ret=StartRecord(0))!=ERR_OK)
            {
                LOGE("%s:StartRecord return 0x%x\n",__func__,ret);
                Post_UI_Message(MSG_ENCODER_EVENT,ret);

                return MsgNeedDispatch; // need test
            }

            //Start Message
            SETBIT(pUISystem->SysState.CurState,SYS_BIT_RECORDING_ONE_CLICK);

#if USE_SERIAL_MSG_422
            //Send Reply
            tReply.ePvrStat=ePvrRec;
            SendUserMsg(&tReply);
#endif
#ifdef EXIT_WHEN_FINISH
            request_exit_main_task = 0;
#endif
            restart_record_mark = 0;
        }
        else
#endif
        {
            // ж��cf������֤¼���ļ���ȫд��
#ifdef EXIT_WHEN_FINISH

#if NEED_UMOUNT_CF

#if (USE_MOUNT_CF_THREAD) || (USE_MOUNT_CF_THREAD_S)

            START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_DELETE_FILE);
            if(CF_Mount_Runing)
            {
                StopMountCF_Thread();
            }
            STOP_WATCHDOG_TIMER_SIG();
#endif
            SystemUmountDev(CF_MOUNT_DIR);

#endif
            // �˳�Ӧ�ó����ʶ��λ
            request_exit_main_task = 1;
            exit_code = EXIT_CODE_NORMAL;
#endif

#if USE_SERIAL_MSG_422
            // Send Reply
            tReply.ePvrStat=ePvrStop;
            SendUserMsg(&tReply);
#endif
            ShowEncoderMessage(nMsgData);// ��ʾ�˳���Ϣ��ʵ�������ڳ������˳��������û����Ϣ��ʾ�����
        }
    }

    return MsgHandled;
}

/**
 * @brief SystemMsgHandle
 * ϵͳ��Ϣ��������������ݽ��յ�����Ϣ���ͣ����䴫�ݸ����Դ�����
 * @param nMsgType
 * ���յ�����Ϣ����
 * @param nMsgData
 * ���յ�����Ϣ����
 * @return
 * MsgHandled ����Ϣ�ѱ����������ٱ�����
 * MsgNeedDispatch �� ��Ҫ���д���
 */
static TMsgHandleStat SystemMsgHandle(unsigned short nMsgType,unsigned short nMsgData){
	TMsgHandleStat retval=MsgNeedDispatch;
    switch (nMsgType){
    case MSG_KEY_EVENT:
        // ������Ϣ���������洦��
        // TODO: Уʱ�Ͳ�ѯϵͳ״̬��Ϣ���Է��ڴ˴����д�����û�д��ڽ��洦����������Ϣ��Ŀǰ
        // ���ڻ�ȡ��Ϣ�������˴���
        retval=SystemMsgHandle_Key(nMsgData);
        break;
    case MSG_DECODER_EVENT:
        LOGT("%s:DECODER MSG 0x%x data 0x%x\n",__func__,nMsgType,nMsgData);
        retval=SystemMsgHandle_Decoder(nMsgData);
        break;
    case MSG_ENCODER_EVENT:
        LOGT("%s:ENCODER MSG 0x%x data 0x%x\n",__func__,nMsgType,nMsgData);
        retval=SystemMsgHandle_Encoder(nMsgData);
        break;
    default:
        retval=MsgNeedDispatch;//Let Window complete the Job
        break;
    }
	return retval;
}

/**
 * @brief my_GUI_GetMsg
 * ��ʱ��ȡ��Ϣ���������⣬timer�Ĵ������Ҳ�ڴ˴�
 * @param pMsgType
 * ������������ȡ����Ϣ���ͣ��������߱������Ϣ
 * @param pMsgData
 * ������������ȡ������Ϣ���ݡ�
 * @return
 * 1����ȡ����Ч��Ϣ
 * 2��δ��ȡ����Ч��Ϣ
 */
int my_GUI_GetMsg(unsigned short *pMsgType,unsigned short *pMsgData)
{
	int my_key;
	int t;
	struct timeval tv;
#if USE_SERIAL_MSG_422
	TUserReply tReply;
#endif

//  Check Key Message check input from stdin/serial/background thread

    // ʹ��select�����������趨��ȡ��Ϣ�������ʱ��
	tv.tv_sec = 1;
	tv.tv_usec = 0;

    // ��select����1sʱ���ڣ���ȡ��Ϣ���������Ϣ���ط��㣬���򷵻�0
    // TODO: fd ���Բ�ͨ����������ȡ����ֱ��ʹ��ȫ�ֱ�������
    t = stdin_get_message(tv,rs422fd);
    // ����1s�󣬴���timer�¼�������MessageBox��ʾ���Զ��رյļ�ʱ��ÿ1s���е�cf����λ���
    // ¼��ʱÿ1s����ļ��Ƿ񳬹���С���Ƶ�
    if(t==0) {
        // keyTimerΪmessageboxδ���յ������ĵ���ʱ
		Handle_Timer_Tick(&pUISystem->sKeyTimer);

		//Disable PlayBackTimer
		//Handle_Timer_Tick(&pUISystem->sPlayBackTimer);
		//Enable DevScan timer

        // ���cf���Ƿ���λ���ļ�¼���Ƿ񳬹���С���ơ�״̬����ʾ��1s��������timer
		Handle_Timer_Tick(&pUISystem->sDevScanTimer);

        // TODO: ����ʱosd��ʾ�õ���timer��Ŀǰ�ƺ���δʵ�������ã���Ҫ��һ������
		for (t=0;t<OSDPOS_COUNT;t++){
			Handle_Timer_Tick(&(pUISystem->sOSDTimer[t]));
		}

        // ��������timer����û����Ϣ���յ�����˷���0
		return 0;
	}else {

        // ��ȡ����Ϣ
		my_key = GUI_GetKey();

        // ������Ϣ������Ϣ���ͺ���Ϣ���ݷֱ�ֵ
		if (my_key){
			*pMsgType=(my_key>>16)&0xffff;
			*pMsgData=(my_key&0xffff);
			return 1;
		}else {

            // δ��ȡ����Ч��Ϣ������0
			LOGT("%s:empty key\n",__func__);
			return 0;
		}
	}
}

//! ȫ��ϵͳ״̬����ָ�룬��Ҫ��ϵͳ����ʱ���г�ʼ��
UI_System_t *pUISystem=NULL;//Initial at Pvr_Init;

/************************ryan end******************************/

/**
 * @brief Handle_Timer_Tick
 * timer������
 * @param pTimer
 * ���������timerָ��
 */
void Handle_Timer_Tick(TEF_TIMER_STRUCT *pTimer){
	TMsgHandleStat msgStat;
	if (pTimer->TimerStatus!=DVR_TIMER_DIS){
		//modify timer count
		if ((--pTimer->TimerDelay)<=0){
			//dispatch timer-timeout event
			msgStat=MsgNeedDispatch;
			pTimer->TimerDelay=pTimer->TimerInterval;
            if (VALIDHANDLE(pTimer->TO_fn)){
				//dispatch message in active widow's handle
                msgStat=pTimer->TO_fn(pTimer);  //����timer��timeout����
			}
			if (msgStat!=MsgHandled){
				//Call a default handler to stop timer
				DefaultTimerHandle( pTimer);
			}
		}	
	}
}
//! [2]


//! �����̺���
//! [3]

//! ����422��ʼ��֮ǰ��termios�ṹ�壬���ڷ���ʼ��ʱ������ָ�
struct termios oldtio422;

/**
 * @brief Task_UI
 * Ӧ�ó����������߳�
 * @param pdata
 * δʹ�ò���
 */
void Task_UI(void * pdata)
{
	unsigned short nMsgType;
	unsigned short nMsgData;
	int i;
	//Time Reply Message
	TUserReply tReply;
//	GUI_Init();

    // ȫ��ϵͳ״̬������ʼ��
	Pvr_UI_Init();

    // UIϵͳ��ʼ��
	Pvr_UI_Start();

    // encoder��ʼ��
	sys_init_encoder();

    // ˢ�µ�ǰ��ʾ��blank��
	MenuRefresh();

#if USE_SERIAL_MSG_232
    // 232���ڳ�ʼ�������ÿ�ݼ����������趨
    InitComMsg();
#endif

#if USE_SERIAL_MSG_422
    // 422 ���ڳ�ʼ�������ÿ�ݼ���������
    rs422fd=InitComMsg422(&oldtio422);
	if(rs422fd==-1)
	{
		ERR("open rs422 failed\n");
	}

    tReply.ePvrStat=ePvrTime;
    // ��ʼ�����֮�󼴷���һ����Ϣ������Уʱ����
	SendUserMsg(&tReply);
#endif
    // ��ʼ����ϣ�ֹͣ�Զ�ι��������ʾ��ʼ������ʱ��
	STOP_WATCHDOG_TIMER_SIG();

    // ����Ϣѭ��
	while(1){
		//Get a message & translate it
		if(my_GUI_GetMsg(&nMsgType,&nMsgData)){
			//dispatch message to system handle 
			if (SystemMsgHandle(nMsgType,nMsgData)!=MsgHandled){
				LOGT("%s: active menu index %d\n",__func__,pUISystem->nMenuIdx);
				if (VALIDMENU(pUISystem->nMenuIdx)&&
					VALIDHANDLE(PACTIVE_MENU->msgHandle_fn)){
					//dispatch message in active widow's handle
					PACTIVE_MENU->msgHandle_fn(PACTIVE_MENU,nMsgType,nMsgData);
					//TODO: need check return value?
				}
			}
		}
#ifdef EXIT_WHEN_FINISH
		if(request_exit_main_task!=0)
			break;
#endif			
	}
//exit_task_ui:
    // �˳�Ӧ�ó�����ѭ�������з���ʼ����֮���˳�����Ӧ�ó���
	CLOSE_WATCHDOG();

	sys_deinit_encoder();
#if USE_SERIAL_MSG_232
	DeInitComMsg();
#endif
#if USE_SERIAL_MSG_422
	if(rs422fd >= 0)//SACH:W10874
	{
		tcsetattr(rs422fd,TCSANOW,&oldtio422);  //haiqing add 
   	 	close(rs422fd);
	}
	
#endif

	Pvr_UI_DeInit();
}
//! [3]
