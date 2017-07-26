/***
 * ���ļ��Ǽ���CF��ʱ�л���ʵ��,������ӦCF������ʱ�İ�����Ӧ,�ڼ��ع����в�����¼�ƵȲ���
 * �ڼ��ع�����,�����������ʾ�����е���ʾ
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
 * cf����λ����߳�ʵ�ֺ���
 */
void MountCFThread()
{
    CF_Mount_Runing = 1;
    CF_Mount_Loop_En = 1;
    while(CF_Mount_Loop_En)
    {
        // ���cf����λ״̬,������ϵͳ״̬
        // ��ϵͳ��������,�����쳣ϵͳ״̬,��ʾ��ͬ�Ľ�����Ϣ
        CFCardDetectStatus = DoCFCardDetect();
        switch(CFCardDetectStatus)
        {
        case ERR_CF_MOUNT_OK:
            break;
        case ERR_CF_RESET:
            // ��λcf����������hub,��Ҫע�����,�������θ�λ֮���Լ�ⲻ��cf��,�ͻ����ϵͳ��λ
            // ������ϵͳ��λ֮��,���ȴ�ϵͳ��λ���,�����ٽ���cf��λ���
            ResetCFDevice();
            break;
        case ERR_CF_NOT_IN:
            // cf����λ״̬�Ѿ��ڼ��ʱ�������,�˴����м��ȷ��
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
        //zqh 2015��10��16�� add�� ��ӵ������Ƿ��������ر�־λ��
        if(CFCardDetectStatus == ERR_CF_MOUNT_OK){
            SETBIT(pUISystem->SysState.DevState, SYS_BIT_CFCard_Mount_Flag);
        }else{
            CLEARBIT(pUISystem->SysState.DevState, SYS_BIT_CFCard_Mount_Flag);
        }

        usleep(2000000);
    }

    CF_Mount_Runing = 0;

    CFCardDetectStatus = ERR_CF_MOUNT_OK;// ֹͣ��⣬��ΪCF��һֱ��λ

    FUNC_EXIT;

    pthread_exit(0);
}

void MountCF_Thread_S(void)
{
    int ret;

    CF_Mount_Runing = 1;
    CF_Mount_Loop_En = 1;

    {
        // ���cf����λ״̬,������ϵͳ״̬
        // ��ϵͳ�����̵�timer��,�����쳣ϵͳ״̬,��ʾ��ͬ�Ľ�����Ϣ
        ret = DoCFCardDetect();
        switch(ret)
        {
        case ERR_CF_MOUNT_OK:
            break;
        case ERR_CF_RESET:
            // ��λcf����������hub,��Ҫע�����,�������θ�λ֮���Լ�ⲻ��cf��,�ͻ����ϵͳ��λ
            // ������ϵͳ��λ֮��,���ȴ�ϵͳ��λ���,�����ٽ���cf��λ���
            ResetCFDevice();
            break;
        case ERR_CF_NOT_IN:
            // cf����λ״̬�Ѿ��ڼ��ʱ�������,�˴����м��ȷ��
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
 * �˺�������MountCFThread�̶߳�cf�����м�⼰���ء�
 * @return
 */
int StartMountCF_Thread(void)
{
    int ret=0;

    if(CF_Mount_Runing)//cf���Ѽ��أ�������������
    {
        return 0;
    }

    FUNC_ENTER;

#if USE_MOUNT_CF_THREAD//����MountCFThread�߳�
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
 * ֹͣcf���ļ�⣬��ѭ����־λCF_Mount_Loop_En���㼴�ɡ�
 */
void StopMountCF_Thread(void)
{
    int ret;

    if(CF_Mount_Runing)
    {
        START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_DELETE_FILE);
        CF_Mount_Loop_En = 0;//ѭ�����cf���Ƿ���λ�ı�־λ����void MountCFThread()�����г�ʼ���ġ�
#if USE_MOUNT_CF_THREAD
        pthread_join(CF_Mount_thread,&ret);	//SACH:W10849 �����䶯
#endif

        STOP_WATCHDOG_TIMER_SIG();
    }
}

int CheckMountFlags(void)
{
    // cf����λ��cf������ʱ,����ʾ���ڼ�����Ϣ
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

