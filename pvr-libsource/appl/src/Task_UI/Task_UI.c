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

//! 看门狗
//! [0]
/******************************watch dog part*********************/
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mount.h>

#include <pthread.h>

//! 使用fork创建子进程时的进程pid
static pid_t pid_watchdog_tmr = -1;

//! 控制看门狗线程喂狗操作变量
unsigned int watchdog_tmr_tick = 0;     // 该变量不为0时，表示需要进行喂狗操作，在主线程中通过FEED_WATCHDOG设置
unsigned int watchdog_time_out = 0;     // 设定一段时间内自动喂狗的时间限定变量，以500ms为单位
unsigned int watchdog_time_out_cnt = 0; // 每次喂狗自增变量，增加到watchdog_time_out时，不再进行喂狗

//! 使用多线程方式时，创建的看门狗线程
pthread_t watchdog_tmr_thread;

//! 控制看门狗线程运行状态的变量
static int watchdog_thread_running = 0; // 该变量表示看门狗线程正在运行中，用于停止看门狗线程时，判断子线程是否退出完毕
static int watchdog_thread_en = 0;      // 该变量用于控制看门狗运行使能

//! 看门狗线程回调函数
void * WatchdogTimer_Thread(void *arg);

//! 使用软件看门狗，在检测不到看门狗文件的时候，创建该文件的函数
int Mknod_Watchdog(void);

/**
 * @brief START_WATCHDOG_TIMER_SIG
 * 开始看门狗线程/进程，并在一段超时时间内，进行喂狗，使得硬件看门狗不被触发
 * 注意：只有使用线程的时候，可以控制子线程的喂狗超时时间，使用子进程的方式时，只能通过父进程终止子进程
 * @param timeout
 * 超时设定时间
 */
void START_WATCHDOG_TIMER_SIG(unsigned int timeout)
{
#ifdef DEBUG_WATCHDOG_THREAD
    // 使用线程方式时，通过设定全局变量的值，控制喂狗线程的运行
	watchdog_time_out = timeout;
	watchdog_time_out_cnt = 0;
#else
    // 使用fork进程的方式时，创建子进程开始看门狗喂狗操作
	pid_t pid = -1;

	FUNC_ENTER;

	FEED_WATCHDOG();

    // 在已经有看门狗子进程的情况下，先终止子进程
	if(pid_watchdog_tmr > 0)
	{
		LOGT("watchdog tmr not closed\r\n");
		kill(pid_watchdog_tmr,SIGKILL);
		pid = wait(0);
		LOGT("stop watchdog tmr pid=%d\r\n",pid);
	}

    // 创建看门狗子进程
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
            // 在子进程中持续进行喂狗，直到父进程强制将子进程终止
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
 * 终止看门狗自动持续喂狗线程/进程。
 * 对于子进程方式自动喂狗，如果父进程进入异常状态，无法调用STOP_WATCHDOG_TIMER_SIG函数
 * 将导致子进程一直持续运行，与看门狗设计意图相悖。
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

//! 在系统状态异常时，通过使用硬件看门狗进行复位的标志
int need_to_reboot = 0;

//! 在异常时调用该函数，通过看门狗触发复位
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

//! 软件看门狗文件描述符
int watchdog_fd = -1;

/**
 * @brief OPEN_WATCHDOG
 * 打开看门狗：
 * 1、使用多线程时，创建看门狗线程，喂狗的实际操作在看门狗线程中进行（本程序使用线程喂狗）
 * 2、使用fork 子进程时，使能硬件看门狗（不推荐）
 * 3、使用软件看门狗时，打开看门狗文件，在喂狗时写入任意字即可喂狗（调试用）
 */
void OPEN_WATCHDOG(void)
{
#ifdef DEBUG_WATCHDOG_THREAD
	int ret;

    watchdog_thread_en = 1;//创建子线程WatchdogTimer_Thread来进行喂狗
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
        Mknod_Watchdog();//创建watchdog设备节点
        watchdog_fd = open("/dev/watchdog",O_RDWR);//打开设备文件
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
 * 看门狗喂狗函数
 * 1、使用多线程时，watchdog_tmr_tick自增，在喂狗线程中该变量非零，进行喂狗操作
 * 2、使用fork子进程时，由于不需要创建子进程，直接进行喂狗操作
 * 3、使用软件看门狗时，对看门狗文件进行写操作
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
 * 关闭看门狗函数
 * 通过全局变量，关闭看门狗线程
 * 对于使用fork子进程方式创建的看门狗进程，使用STOP_WATCHDOG_TIMER_SIG停止进程
 */
void CLOSE_WATCHDOG(void)
{
#ifdef ENABLE_SOFTWARE_WATCHDOG
    // 需要进一步测试，软件看门狗关闭文件描述符之后似乎仍会重启
	if(watchdog_fd > 0)
		close(watchdog_fd);
#endif

// 仅在复位前关闭看门狗
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
 * 多线程方式创建的看门狗线程，使用多线程方式时，喂狗的最终操作均在此处完成
 * @param arg
 * 忽略参数
 * @return
 * 忽略返回
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

    // 全局变量标志置位
	watchdog_thread_running = 1;

    // 看门狗线程主循环
    while(watchdog_thread_en)//watchdog_thread_en变量在void OPEN_WATCHDOG(void)函数中进行初始化
	{
        // 系统异常时，不进行喂狗，等待硬件看门狗复位
		if(need_to_reboot==0)
		{
            // 主线程中调用了FEED_WATCHDOG或通过START_WATCHDOG_TIMER_SIG设置了一段时间自动喂狗后，进行喂狗操作
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

        // 测试功能，检测USB写状态，在release版未启用
		ParseMUSBStatus();

        // 延时500ms进行一次喂狗
		usleep(500000);
	}

    // 在调用了STOP_WATCHDOG之后，退出主循环，清零running标志
	watchdog_thread_running = 0;

#endif
	return NULL;
}

/**
 * @brief Mknod_Watchdog
 * 使用软件看门狗测试时，如果系统中没有看门狗设备文件，创建看门狗设备文件
 * @return
 */
int Mknod_Watchdog(void)
{
	int ret;
	dev_t dev;

	FUNC_ENTER;

    //将根文件系统重新挂载为可读写文件系统
	ret = mount(RFS_MOUNT_NOD,RFS_MOUNT_DST,RFS_MOUNT_TYPE,MS_REMOUNT,0);
	if(ret<0)
	{
		ERR("remount RFS to rw err:%s\r\n",strerror(errno));
	}

    ret = remove("/dev/watchdog");//移除设备节点
	if(ret<0)
	{
		LOGE("remove watchdog error:%s\r\n",strerror(errno));
	}

	LOGT("mknod /dev/watchdog c 10 130\r\n");
    system("mknod /dev/watchdog c 10 130");//创建watchdog设备节点

    //将根文件系统重新挂载为只读文件系统
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

//! 日志记录
//! [1]
/****************************** sys log part **********************/
#include <sys/statfs.h>
#include <sys/stat.h>
#include <sys/time.h>

FILE *fp_log_stdout = NULL;     // 标准输出文件指针，重定向到cf卡的文件时使用
FILE *fp_log_stderr = NULL;     // 标准错误输出文件指针，重定向到cf卡的文件时使用

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
 * 关闭日志功能后，将标准输出和标准错误输出重定向到console（串口）
 */
void redirect_log_console(void)
{
    int flags;

    // [1] flush并关闭日志记录文件
	fflush(stdout);
	fflush(stderr);

	if((fp_log_stdout != NULL)||(fp_log_stderr != NULL))
		close_log();

    // [2] 将标准输出重定向到console
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

    // [3] 将标准错误输出重定向到console
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
 * 使能/关闭日志记录功能
 * 通过在/opt/目录下创建一个文件，标识当前启用log功能，如果需要关闭日志记录功能，则将该文件删除
 * @param en
 * 非0 表示使能log功能
 * 0 表示关闭log功能
 */
void set_log_en(int en)
{
#if WRITE_LOG_TO_CF
    FILE *fp=0;
    int ret;

    // 默认文件系统为只读，因此需要先将文件系统改为可读写
    ret = mount(RFS_MOUNT_NOD,RFS_MOUNT_DST,RFS_MOUNT_TYPE,MS_REMOUNT,0);
	if(ret<0)
	{
		LOGE("remount RFS to rw err:%s\r\n",strerror(errno));
	}

    // 根据传入参数创建、删除log记录使能文件。
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
        // 关闭log记录功能后，需要将调试信息重新定向到console
        redirect_log_console();
		sync();
		sync();
    }

    // 重新将文件系统改为只读
    ret = mount(RFS_MOUNT_NOD,RFS_MOUNT_DST,RFS_MOUNT_TYPE,MS_RDONLY|MS_REMOUNT,0);
    if(ret <0)
    {
        LOGE("remount RFS to ro failed:%s\r\n",strerror(errno));
    }
#endif
}

/**
 * @brief is_log_en
 * 根据/opt/目录下是否有log使能标识文件，判断当前是否使能log记录功能
 * @return
 * 0 关闭log功能
 * 1 使能log功能
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
 * 将log重定向到文件，在未加载cf卡时，重定向到tmp文件系统的文件中，加载cf卡之后，重定向到cf中
 * @param dest_path
 * 文件所在路径，为绝对路径
 * @param dest_file
 * 文件名
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

    // [0] 重定向前，判断是否使能将日志记录到cf卡功能
    // if log record disabled,redirect stdout/stderr to console(ttyS0)
    if(is_log_en() == 0)
    {
//        LOGT("log not enabled\n");
        return 0;
    }
//    LOGT("log enabled\n");

    // [1] 重定向前，先关闭已经打开的文件
	fflush(stdout);
	fflush(stderr);

	if((fp_log_stdout != NULL)||(fp_log_stderr != NULL))
		close_log();

    // [2] 根据传入参数获得被重定向的文件全名
    sprintf(full_path,"%s%s",dest_path,dest_file);

    // [3] 将标准输出定向到文件
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

    setbuf(stdout,NULL);//关闭stdout缓冲区

    // [4] 将标准错误输出定向到文件
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

    setbuf(stderr,NULL);//关闭stderr缓冲区

	LOGT("redirect stdout to fileno: %d\r\n",fileno( fp_log_stdout ));

	LOGT("redirect stderr to fileno: %d\r\n",fileno( fp_log_stderr ));

	LOGT("redirect log to %s\r\n",full_path);

    // 重定向完成后，记录当前时间以便在调试查看
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
 * 关闭日志重定向文件
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
 * 将在临时目录中的log移动到cf卡中或反之，在获取上电内核信息，或者格式化cf卡操作时，会用到
 * @param src_path
 * log文件存放的源目录
 * @param dst_path
 * 目标目录
 * @param filename
 * log文件名
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

    sprintf(buf,"%s%s",src_path,filename);//合成源文件的全路径
	fp = fopen(buf,"r");
    if(fp != NULL)//检测源文件是否存在
	{
		fclose(fp);
		LOGT("%s found,move to %s\r\n",buf,dst_path);

        // 通过system系统调用，实现mv操作，需要注意的是，由于system系统调用是阻塞的，需要添加看门狗自动喂狗
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
 * 将一个log文件中的内容，添加到另一个文件中，系统启动时使用，系统启动时，在cf卡加载前的日志是记录在临时文件系统中的
 * @param src_path
 * 源目录
 * @param dst_path
 * 目标目录
 * @param filename
 * log文件名
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

    // 通过系统调用，实现cat功能
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
 * 检查log文件大小功能，检查当前写入的log文件(pvr_log.log)大小，如果超过最大大小，
 * 删除掉旧文件(pvr_log.old.log)，并将当前文件改名为旧文件，将新的日志继续记录
 * log文件记录过程中会保存两个文件，防止在删除文件之后，丢失上次有用的信息。
 * @return
 * -1: 检查过程中出现问题，cf卡不在位
 * 0: 检查完成
 */
int check_log_size(void)
{
#if WRITE_LOG_TO_CF
	struct stat file_stat;
	unsigned long long sizeinbyte;
	char buf[MAX_FS_PATH];

    // 检查文件前，判断cf卡在位状态
	if(TESTBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_REMOVE))
	{
		return -1;
	}

    // 不使能log记录时，直接返回
    if(is_log_en() == 0)
        return 0;

    sprintf(buf,"%s%s",LOG_PATH_CFCARD,LOG_FILE_LOG);

    // 检查当前写入的log文件
    stat(buf,&file_stat);
	sizeinbyte = file_stat.st_size/1024/1024;

	if(sizeinbyte > MAX_LOG_FILE_SIZE)
	{
		LOGT("log file reach max size\r\n");

        // 使用系统调用完成删除和改名功能，系统调用为阻塞函数，需要打开看门狗的自动喂狗
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

        // 将新log重定向到新建文件
		redirect_log(LOG_PATH_CFCARD,LOG_FILE_LOG);
	}

#endif

	return 0;
}

/**
 * @brief get_kernel_msg
 * 获取上电时的内核信息，注意，只保存本次上电的内核信息
 * @param full_path
 * 保存内核信息的文件路径全名
 */
void get_kernel_msg(char * full_path)
{
#if WRITE_LOG_TO_CF
	char buff[MAX_FS_PATH];

	START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_MOVE_LOG);

    // 使用system系统调用，将dmesg命令的输出内容写入log文件
	sprintf(buff,"dmesg >> %s",full_path);
	system(buff);

	STOP_WATCHDOG_TIMER_SIG();
#endif
}


/****************************** sys log part end ******************/
//! [1]

//! 系统主流程处理消息函数
//! [2]
/*
 ----	System Provide Handler
 */
int rs422fd = -1;   // rs422的全局文件描述符，打开、关闭，读取422消息时使用

/**
 * @brief SystemMsgHandle_Key
 * 保留函数
 * 目前没有按键消息在该函数处理
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
 * 解码消息处理函数，如果当前正在进行文件播放（正常情况），会不进行处理，将消息传递给文件播放界面处理
 * 如果当前不在文件播放界面，则停止文件播放。
 * @param nMsgData
 * @return
 */
static TMsgHandleStat SystemMsgHandle_Decoder(unsigned short nMsgData)
{
	if (pUISystem->nMenuIdx==MENUPLAYBACK){
        LOGT("1\r\n");          // 测试用调试信息，确定消息接收到的位置
        StopParseMUSBStatus();  // 测试用调试信息，检查usb写入状态
        return MsgNeedDispatch; // 正在文件播放过程中接收到解码消息，由文件播放界面处理
	}else{
        // 非文件播放界面接收到解码消息，停止播放，清除播放状态，显示提示信息
        StopTmpOSD();

		ClearPlayState();
		ShowDecoderMessage(nMsgData);

        LOGT("2\r\n");          // 测试用调试信息，确定消息接收到的位置
        StopParseMUSBStatus();  // 测试用调试信息，检查usb写入状态
	}
    return MsgHandled;          // 表明信息已经过处理
}

//! 表明需要退出应用程序的全局变量
int request_exit_main_task = 0;
//! 未支持编码时文件切换功能时，使用的全局变量，表明需要重新开始文件录制
int restart_record_mark = 0;

/**
 * @brief SystemMsgHandle_Encoder
 * 编码消息处理函数，接收到编码消息（停止编码或编码异常），清除当前编码状态标识并准备退出应用程序
 * @param nMsgData
 * 编码消息数据
 * @return
 * 默认返回消息已处理
 */
static TMsgHandleStat SystemMsgHandle_Encoder(unsigned short nMsgData)
{
	int ret;
#if USE_SERIAL_MSG_422
	TUserReply tReply;
#endif
    // restart msg process
	// if not defined macro "FILE_SWITCH",stop record first,then restart record
    // 编码消息（停止编码或编码异常），均需要停止当前编码
    switch(nMsgData)
    {
    case MSG_PARA_ENC_STOP:
    default:
        ClearRecordState(); // 清除当前编码系统状态
#ifndef ENABLE_FILE_SWITCH
        // 未使用编码库的文件切换功能时，在文件切换时，先停止编码，之后收到编码停止信息，在此处重新开始
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
            // 卸载cf卡，保证录制文件完全写入
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
            // 退出应用程序标识置位
            request_exit_main_task = 1;
            exit_code = EXIT_CODE_NORMAL;
#endif

#if USE_SERIAL_MSG_422
            // Send Reply
            tReply.ePvrStat=ePvrStop;
            SendUserMsg(&tReply);
#endif
            ShowEncoderMessage(nMsgData);// 显示退出信息，实际上由于程序已退出，会出现没有信息显示的情况
        }
    }

    return MsgHandled;
}

/**
 * @brief SystemMsgHandle
 * 系统消息处理函数，负责根据接收到的消息类型，将其传递给各自处理函数
 * @param nMsgType
 * 接收到的消息类型
 * @param nMsgData
 * 接收到的消息数据
 * @return
 * MsgHandled ：消息已被处理，不会再被处理
 * MsgNeedDispatch ： 需要进行处理
 */
static TMsgHandleStat SystemMsgHandle(unsigned short nMsgType,unsigned short nMsgData){
	TMsgHandleStat retval=MsgNeedDispatch;
    switch (nMsgType){
    case MSG_KEY_EVENT:
        // 按键消息均交给界面处理
        // TODO: 校时和查询系统状态消息可以放在此处进行处理，并没有窗口界面处理这两个消息，目前
        // 放在获取消息处进行了处理
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
 * 定时获取消息函数，另外，timer的处理入口也在此处
 * @param pMsgType
 * 传出参数，获取的消息类型，按键或者编解码消息
 * @param pMsgData
 * 传出参数，获取到的消息数据。
 * @return
 * 1：获取到有效消息
 * 2：未获取到有效消息
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

    // 使用select进行阻塞，设定获取消息最多阻塞时间
	tv.tv_sec = 1;
	tv.tv_usec = 0;

    // 在select阻塞1s时间内，获取消息，如果有消息返回非零，否则返回0
    // TODO: fd 可以不通过传参数获取到，直接使用全局变量即可
    t = stdin_get_message(tv,rs422fd);
    // 阻塞1s后，处理timer事件，比如MessageBox显示后自动关闭的计时，每1s进行的cf卡在位检查
    // 录制时每1s检查文件是否超过大小限制等
    if(t==0) {
        // keyTimer为messagebox未接收到按键的倒计时
		Handle_Timer_Tick(&pUISystem->sKeyTimer);

		//Disable PlayBackTimer
		//Handle_Timer_Tick(&pUISystem->sPlayBackTimer);
		//Enable DevScan timer

        // 检测cf卡是否在位、文件录制是否超过大小限制、状态灯显示等1s间隔处理的timer
		Handle_Timer_Tick(&pUISystem->sDevScanTimer);

        // TODO: 播放时osd显示用到的timer，目前似乎并未实际起作用，需要进一步测试
		for (t=0;t<OSDPOS_COUNT;t++){
			Handle_Timer_Tick(&(pUISystem->sOSDTimer[t]));
		}

        // 仅仅处理timer，并没有消息接收到，因此返回0
		return 0;
	}else {

        // 获取到消息
		my_key = GUI_GetKey();

        // 根据消息，将消息类型和消息数据分别赋值
		if (my_key){
			*pMsgType=(my_key>>16)&0xffff;
			*pMsgData=(my_key&0xffff);
			return 1;
		}else {

            // 未获取到有效消息，返回0
			LOGT("%s:empty key\n",__func__);
			return 0;
		}
	}
}

//! 全局系统状态变量指针，需要在系统启动时进行初始化
UI_System_t *pUISystem=NULL;//Initial at Pvr_Init;

/************************ryan end******************************/

/**
 * @brief Handle_Timer_Tick
 * timer处理函数
 * @param pTimer
 * 传入参数，timer指针
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
                msgStat=pTimer->TO_fn(pTimer);  //处理timer的timeout函数
			}
			if (msgStat!=MsgHandled){
				//Call a default handler to stop timer
				DefaultTimerHandle( pTimer);
			}
		}	
	}
}
//! [2]


//! 主流程函数
//! [3]

//! 保存422初始化之前的termios结构体，用于反初始化时，将其恢复
struct termios oldtio422;

/**
 * @brief Task_UI
 * 应用程序流程主线程
 * @param pdata
 * 未使用参数
 */
void Task_UI(void * pdata)
{
	unsigned short nMsgType;
	unsigned short nMsgData;
	int i;
	//Time Reply Message
	TUserReply tReply;
//	GUI_Init();

    // 全局系统状态变量初始化
	Pvr_UI_Init();

    // UI系统初始化
	Pvr_UI_Start();

    // encoder初始化
	sys_init_encoder();

    // 刷新当前显示（blank）
	MenuRefresh();

#if USE_SERIAL_MSG_232
    // 232串口初始化，禁用快捷键，无阻塞设定
    InitComMsg();
#endif

#if USE_SERIAL_MSG_422
    // 422 串口初始化，禁用快捷键，无阻塞
    rs422fd=InitComMsg422(&oldtio422);
	if(rs422fd==-1)
	{
		ERR("open rs422 failed\n");
	}

    tReply.ePvrStat=ePvrTime;
    // 初始化完成之后即发送一次消息，包含校时请求
	SendUserMsg(&tReply);
#endif
    // 初始化完毕，停止自动喂狗，并显示初始化所用时间
	STOP_WATCHDOG_TIMER_SIG();

    // 主消息循环
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
    // 退出应用程序主循环，进行反初始化，之后退出整个应用程序
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
