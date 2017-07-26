/***
 * 此文件是加载CF卡时切换的实现,用来响应CF卡加载时的按键响应,在加载过程中不进行录制等操作
 * 在加载过程中,按任意键将显示加载中的提示
 */
#include <pthread.h>
#include <errno.h>

#include "GUI.H"
#include "Key.h"

#include "config.h"//Config for platform
#include "UI_Message.h"	//UI message

#include "Task_UI/task_ui_var.h"
#include "Task_UI/pvr_ui_func.h"
#include "../../avi/encoder.h"

#include "UserMsg.h"
#include "ui_misc.h"

int CF_Mount_Runing = 0;

static int CF_Mount_Loop_En = 1;
static pthread_t CF_Mount_thread;

static pthread_attr_t pThread_Attr;

extern int CFCardDetectStatus;
/**
 * @brief MountCFThread
 * cf卡在位检测线程实现函数
 */
void MountCFThread()
{
    CF_Mount_Runing = 1;
    CF_Mount_Loop_En = 1;
    while(CF_Mount_Loop_En)
    {
        // 检查cf卡在位状态,并设置系统状态
        // 在系统主流程中,根据异常系统状态,显示不同的界面信息
        CFCardDetectStatus = DoCFCardDetect();
        switch(CFCardDetectStatus)
        {
        case ERR_CF_MOUNT_OK:
            break;
        case ERR_CF_RESET:
            // 复位cf卡控制器与hub,需要注意的是,连续两次复位之后仍检测不到cf卡,就会进行系统复位
            // 设置了系统复位之后,将等待系统复位完成,而不再进行cf在位检查
            ResetCFDevice();
            break;
        case ERR_CF_NOT_IN:
            // cf卡在位状态已经在检查时设置完成,此处进行检查确认
            if(TESTBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_REMOVE) == 0)
            {
                SETBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_REMOVE);
            }
            break;
        case ERR_CF_NEED_MOUNT:
            if((TESTBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_FORMAT) == 0)
                &&(TESTBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_FSCK) == 0)
                &&(TESTBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_NEEDFORMAT)== 0))
            {
                CFCardDetectStatus = DoMountCFCard();
                if(CFCardDetectStatus == ERR_CF_MOUNT_ERR)
                {
                    LOGE("CFCard need format");
                    SystemUmountDev(CF_MOUNT_DIR);
                }
            }
            break;
        case ERR_CF_FORMAT:
        case ERR_CF_MOUNT_ERR:
        default:
            break;
        }
        //zqh 2015年10月16日 add， 添加电子盘是否正常加载标志位。
        if(CFCardDetectStatus == ERR_CF_MOUNT_OK){
            SETBIT(pUISystem->SysState.DevState, SYS_BIT_CFCard_Mount_Flag);
        }else{
            CLEARBIT(pUISystem->SysState.DevState, SYS_BIT_CFCard_Mount_Flag);
        }

        usleep(2000000);
    }

    CF_Mount_Runing = 0;

    CFCardDetectStatus = ERR_CF_MOUNT_OK;// 停止检测，认为CF卡一直在位

    FUNC_EXIT;

    pthread_exit(0);
}

void MountCF_Thread_S(void)
{
    int ret;

    CF_Mount_Runing = 1;
    CF_Mount_Loop_En = 1;

    {
        // 检查cf卡在位状态,并设置系统状态
        // 在系统主流程的timer中,根据异常系统状态,显示不同的界面信息
        ret = DoCFCardDetect();
        switch(ret)
        {
        case ERR_CF_MOUNT_OK:
            break;
        case ERR_CF_RESET:
            // 复位cf卡控制器与hub,需要注意的是,连续两次复位之后仍检测不到cf卡,就会进行系统复位
            // 设置了系统复位之后,将等待系统复位完成,而不再进行cf在位检查
            ResetCFDevice();
            break;
        case ERR_CF_NOT_IN:
            // cf卡在位状态已经在检查时设置完成,此处进行检查确认
            if(TESTBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_REMOVE) == 0)
            {
                SETBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_REMOVE);
            }
            break;
        case ERR_CF_NEED_MOUNT:
            if((TESTBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_FORMAT) == 0)
                &&(TESTBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_FSCK) == 0)
                &&(TESTBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_NEEDFORMAT)== 0))
            {
                ret = DoMountCFCard();
                if(ret == ERR_CF_MOUNT_ERR)
                {
                    LOGE("CFCard need format");
                    SystemUmountDev(CF_MOUNT_DIR);
                }
            }
            break;
        case ERR_CF_FORMAT:
        case ERR_CF_MOUNT_ERR:
        default:
            break;
        }
    }

    FUNC_EXIT;
    CF_Mount_Runing = 0;

    pthread_exit(0);
}

/**
 * @brief StartMountCF_Thread
 * 此函数创建MountCFThread线程对cf卡进行检测及加载。
 * @return
 */
int StartMountCF_Thread(void)
{
    int ret=0;

    if(CF_Mount_Runing)//cf卡已加载，函数立即返回
    {
        return 0;
    }

    FUNC_ENTER;

#if USE_MOUNT_CF_THREAD//创建MountCFThread线程
	ret = pthread_create(&CF_Mount_thread,NULL,MountCFThread,NULL);
#elif USE_MOUNT_CF_THREAD_S
    ret = pthread_attr_init(&pThread_Attr);
	if(ret)
	{
		LOGE("CF_Mount_thread  create err:%s\r\n",strerror(errno));
        exit_code = EXIT_CODE_PVR_ERROR;
        return -1;
	}
	ret = pthread_attr_setdetachstate(&pThread_Attr,PTHREAD_CREATE_DETACHED);
	if(ret)
	{
		LOGE("CF_Mount_thread  create err:%s\r\n",strerror(errno));
        exit_code = EXIT_CODE_PVR_ERROR;
        return -1;
	}
	
	ret = pthread_create(&CF_Mount_thread,&pThread_Attr,MountCF_Thread_S,NULL);
#else
    ret = -1;
    LOGE("invalid function call\r\n");
    return ret;
#endif
    if(ret)
    {
        LOGE("CF_Mount_thread create err:%s\r\n",strerror(errno));
        exit_code = EXIT_CODE_PVR_ERROR;
        return -1;
    }

    return 0;
}

/**
 * @brief StopMountCF_Thread
 * 停止cf卡的检测，将循环标志位CF_Mount_Loop_En清零即可。
 */
void StopMountCF_Thread(void)
{
    int ret;

    if(CF_Mount_Runing)
    {
        START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_DELETE_FILE);
        CF_Mount_Loop_En = 0;//循环检测cf卡是否在位的标志位，在void MountCFThread()函数中初始化的。
#if USE_MOUNT_CF_THREAD
        pthread_join(CF_Mount_thread,&ret);	//SACH:W10849 不作变动
#endif

        STOP_WATCHDOG_TIMER_SIG();
    }
}

int CheckMountFlags(void)
{
    // cf卡复位与cf卡加载时,均显示正在检查的信息
    if(TESTBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_MOUNTING)
        || TESTBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_RESET))
    {
        SetLvdsSwitch(0);
        if(TESTBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_FORMAT))
        {
            MessageBox(FormattingCFCard[pUISystem->allData.Language],CODEC_MESSAGE_DELAY,NULL,NULL,
                        NULL,NULL,NULL,NULL);
        }
        else
        {
            MessageBox(CheckingCFCard[pUISystem->allData.Language],CODEC_MESSAGE_DELAY,NULL,NULL,
                        NULL,NULL,NULL,NULL);
        }

        return -1;
    }

    if(TESTBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_REMOVE))
    {
        SetLvdsSwitch(0);
        MessageBox(CFRemoved[pUISystem->allData.Language],CODEC_MESSAGE_DELAY,NULL,NULL,
                        NULL,NULL,NULL,NULL);
        return -1;
    }

    return 0;
}

