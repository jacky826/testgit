#ifndef _SYSTEM_CONFIG_HEAD_
#define _SYSTEM_CONFIG_HEAD_

/*
 * ����Ӧ�ò�������������Ϣ
 * ��Ҫ���ڿ���һЩ�������ú͵��Կ���
 */

/*�汾����*/
#define PVR_VERSION				(4.0)	//ϵͳ�汾
#define PVR_DEBUG				(1)		//�汾����--1: ���԰汾;0:�����汾

/*���ܿ��ƿ���*/
/*�Ƿ�򿪱���� ����*/
#define ENCODE_DECODE_ENABLE
/*�Ƿ��USB Host ����*/
#define USB_ENABLE	

#define FLASH_ENABLE
#define FLASHMAGICNUMBER 	(0xDEADFACE)

//#define DEBUG_MESSAGEBOX


/*��������*/
/*��������*/
#define SUPPORT_DISK_CNT		(5)//(3)
#define DISK_CF					(0)
#define DISK_USB				(1)
#define DISK_SD					(2)


/*��ʾ����*/
#define FILES_PER_BLOCK			(10)		//File browser��ÿҳ��ʾ���ļ���


#define UI_OSTTIMEDLY			(1)		//������Ӧʱ��(1 = 100ms) 
										//OSD ��С��ʱ��λ
/*From pvr_config.h*/
#define FILE_COPY_BUFFER_LENGTH		(0x10000)

//--------------------------------------------------------------------------//
//	Purpose	:	ģ�������趨
//--------------------------------------------------------------------------//
//
#define FILENAME_MAINLEN		(9)		//8+3���ļ�������

#define LANGUAGE_CNT			(3)		//ϵͳ֧������:  Ӣ��\��������\��������
#define REC_FORMAT_CNT			(9)		//ϵͳ֧�ֵ����¼��ģʽ:  PSP\PMP\iPod\Pocket PC\D Cam\Mobile

#define SCHEDULE_NAME_LEN		(12)		//¼���ļ����������Ϊ8���ַ�,��:Rec00000,".avi\0"�Ⱥ�׺����������
#define ACTUALSCHEDULE_NAME_LEN	(9)	//ʵ����󳤶ȣ�������β0



//--------------------------------------------------------------------------//
//	Purpose	:	 ���궨�� 
//--------------------------------------------------------------------------//
//
#define COORDINATE_X0_PROG_NUM		(550)
#define COORDINATE_Y0_PROG_NUM		(60)
#define COORDINATE_XSIZE_PROG_NUM	(160)
#define COORDINATE_YSIZE_PROG_NUM	(80)

//--------------------------------------------------------------------------//
//	Purpose	:	 �ļ�ϵͳ������ض��� 
//--------------------------------------------------------------------------//
#define EXTNAME_LEN					(8)	//�ļ���չ����󳤶�

//--------------------------------------------------------------------------//
//	Purpose	:	Ĭ������
//--------------------------------------------------------------------------//
#define DEFAULT_YEAR	(2010)
#define DEFAULT_MONTH	(2)
#define DEFAULT_DAY	(14)
#define DEFAULT_HOUR	(12)
#define DEFAULT_MINUTE	(30)
#define DEFAULT_SECOND	(30)

#define DECODER_USE_SYNCHRONOUS_LOAD

#define MAX_PATH	256

#define AVI_SYNC_TMP_FILE "/mnt/ram/tail.dat"
#define CONFIG_FILE_NAME	"/opt/.pvr_config.cfg"
//#define RECORD_FILE_PATH	SD_MOUNT_DIR     //changed by ryan

#define ENABLE_RENAME_FILE
#define RECORD_FILE_EXT			".avi"


#define USE_EFLAG_HD_INPUT
//#define LIMIT_HD_HEIGHT
#define EXIT_WHEN_FINISH

//#define USE_DEFAULT_CONFIG

//#define ENABLE_SERIAL_USER_MSG

#define USE_SERIAL_MSG_232          0
#define USE_SERIAL_MSG_422          1
#define ENABLE_422_KEY_MSG          1//rs422 ���հ�����Ϣ�������¸�ʽ��״̬��Ϣ

/*
two buffers for osd0 window, two buffers for attribute window.
one seperate buffer for ucgui which will be copied to osd0 buffer when 
enable_osd_dis() is called
*/
//this macro is not used anymore whose function is on by default commented by jason.ren@2011/11/20 
//#define USE_SEPERATE_UCGUI_BUFFER

#define USE_ENCODELIB_VER02300001

#define IDR_FRAME_INTERVAL 15
#define FRAME_CAPTURE_RATE 30
//#define CONFGI_SAVE_DEC_IMAGE

/*<2010-12-5 renjun add begin: flush loader's cache if play speed or play orient changed*/		
//following macro is different method to enhance decoding effect when speed change, use one of them
//#define FLUSH_LOADERCACHE_WHEN_SPEED_CHANGE
//#define SKIP_FRAME_DELAY_WHEN_SPEED_CHANGE
#define INPUT_SINGLE_FRAME
/*2010-12-5 renjun add end>*/

//auto scan SDCard Add new timer and timeout fn

#define PROC_MOUNTS_FILE		"/proc/mounts"
#define PROC_SCSI_FILE			"/proc/scsi/scsi"
#define PROC_PARTITIONS_FILE	"/proc/partitions"

#define PROC_PARSE_SD			"/dev/mmcblk"
#define PROC_PARSE_UDISK		"/dev/sd"
#define PROC_PARSE_CFCARD		"/dev/"

//SD����CF����U�� ��mount���̶�Ŀ¼��
#define SD_MOUNT_DIR			"/media/sd"
#define USB_MOUNT_DIR			"/media/usb"
#define CF_MOUNT_DIR			"/media/cf"

#define FAKE_DEV_STAT_FILE		".fakefile"

#define MIN_WARNING_FREE_SPACE_MB 500//zqh 2015��10��18�� add�� ��Ӳ�̿ռ�С��500M�ǿ�ʼ����
#define MIN_FREE_SPACE_MB		  100
#define MAX_RECORDFILE_SIZE_MB    1500

#define FPGA_PATH_LED2			"/sys/class/davinci_display/ch0/toggle_led2"
#define FPGA_PATH_LED3			"/sys/class/davinci_display/ch0/toggle_led3"
#define FPGA_LED2				0
#define FPGA_LED3				1
#define FPGA_LED_ON				0
#define FPGA_LED_OFF			1

#define UPDATELOADMODULESSCRIPT   1//���ڴ��Ƿ�����optĿ¼�µ�loadmodules-ef.sh�ļ��Ĺ��ܣ�0Ϊ�����ã�1Ϊ����

#if  UPDATELOADMODULESSCRIPT
#define UPDATE_LOAD_MODULE_SH_NAME    "loadmodules-ef.sh"//zqh add
#endif

#define UPDATE_FILE_NAME		"pvr"
#define UPDATE_DAEMON_NAME		"daemon"
#define UPDATE_KERNEL_NAME		"uImage"	//�ں��ļ���

#define UPDATE_KERNEL_CMD1		"flash_eraseall -q /dev/mtd2"
#define UPDATE_KERNEL_CMD2		"nandwrite -m -n -p -q /dev/mtd2 " //���滹��Ҫ����ں��ļ�����ȫ·��

#define UPDATE_SRC_FILE_PATH	( pUISystem->Vol_Disk[DISK_USB] )
#define UPDATE_DST_FILE_PATH	"/opt/"
#define UPDATE_BACK_FILE_DES	".tmp" 

#define RFS_MOUNT_NOD			"/dev/root"
#define RFS_MOUNT_DST			"/"

#define DISPBUF_LEN				100

#define ENABLE_FILE_SWITCH

// Disable software watchdog
//#define ENABLE_SOFTWARE_WATCHDOG

// debug only disable hardware watchdog
#define ENABLE_HARDWARE_WATCHDOG

//Reserved file header
#define RESERVED_HEADER_STR		"KXLH"
#define RESERVED_HEADER_LEN		4

//get exit code in daemon
#define EXIT_CODE_INVALID       (0)     // invalid exit code ,default exit code if pvr not exit normaly
#define EXIT_CODE_PVR_ERROR		(2)
#define EXIT_CODE_NORMAL		(1)
#define EXIT_CODE_UPDATE		(10)
#define EXIT_CODE_REBOOT		(11)	// Add 0520-2012 , reboot system after update daemon & kernel

extern int exit_code;
extern int request_exit_main_task;


#define DEV_PATH_LEN			20

#define DEBUG_WATCHDOG_THREAD

#define WATCHDOG_TO_DELETE_FILE		120		// 120  / 2 = 60 s
#define WATCHDOG_TO_KERNEL_UPDATE	600		// 600 / 2 = 300 s
#define WATCHDOG_TO_FORMAT_CF		1200		// 1200 / 2 = 600 s
#define WATCHDOG_TO_FSCK_CF			1200		// 1200 / 2 = 600 s

#define WATCHDOG_TO_MOVE_LOG		600		// 600 / 2 = 300 s
#define WATCHDOG_TO_MOUNT			1200		// 1200 / 2 = 600 s

// v3 hardware debug mark
#define DEBUG_V2_1				1

#define MOUNT_TYPE_UDISK		"vfat"

#define MOUNT_CF_EXT            1
#if MOUNT_CF_EXT// DEBUG_V2_1
#define MOUNT_TYPE_CF			"ext3"
#define MOUNT_DATA_CF			"data=ordered"
#else
#define MOUNT_TYPE_CF			"vfat"
#define MOUNT_DATA_CF			0
#endif

#if DEBUG_V2_1
#define RFS_MOUNT_TYPE			"yaffs2"
#else
#define RFS_MOUNT_TYPE			"jffs2"
#endif

#if DEBUG_V2_1
//#define UPDATE_SYSTEM_TOOLS_CNT		2
#else
//#define UPDATE_SYSTEM_TOOLS_CNT		2
#endif

// System log redirect
#define WRITE_LOG_TO_CF			0//zqh 2015��10��18�� modify�� ����־�����ڵ����̹���ȥ��
#define WRITE_LOG_EN_FILE       "/opt/.pvr_log_en"

#define LOG_PATH_CFCARD			pUISystem->Vol_Disk[DISK_CF]
#define LOG_PATH_TMP			"/tmp/"

#define LOG_FILE_LOG			"pvr_log.log"
#define LOG_FILE_LOG_OLD		"pvr_log.old.log"

#define LOG_FILE_STARTUP		"pvr_startup.log"

#define LOG_FILE_FMT			"pvr_fmt.log"
#define LOG_FILE_FSCK			"pvr_fsck.log"

#define LOG_FILE_KERNEL_START	"kernel_start.log"
#define LOG_FILE_KERNEL_LAST	"kernel_msg_last.log"

#define MAX_LOG_FILE_SIZE		10	// ��ౣ��������־����ǰ��־����С 10Mbyte

// update lock file. To indicate update is in progress
#define UPDATE_LOCK_FILE		"/opt/pvr.lock"


#if 0
// update copy to tmp file & rename
#if DEBUG_V2_1
#define UPDATE_TMP_FILE_DES		".t"
#else
#define UPDATE_TMP_FILE_DES		".t"
#endif
#endif

#define NEED_UMOUNT_CF			1

// Ŀǰͨ�����usb���ж����������ܶ��Եķ����Ƿ���usb������
#define CHECK_USB_EN            0

// file copy thread enabled ,used in v2.1 & v2.0
#define USE_FILE_COPY_THREAD    1

// ʹ��cf��λ������̴߳���cf�����ص���Ӧ,���̳߳���ֱ�������˳�
#define USE_MOUNT_CF_THREAD             1

// ÿ�ε���mount��ʱ��ֻ����һ�μ���,���سɹ����˳��߳�,ʧ�ܸ���������¼��ػ��˳�
#define USE_MOUNT_CF_THREAD_S           0


// ��Ƶ��������
#define SUPPORT_AUDIO_ENC       1
#define AUDIO_TEST_TAG          1

#define APPEND_AVI_IDX          1
#endif

