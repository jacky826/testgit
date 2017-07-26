#ifndef _SYSTEM_CONFIG_HEAD_
#define _SYSTEM_CONFIG_HEAD_

/*
 * 定义应用层程序基本配置信息
 * 主要用于控制一些功能配置和调试开关
 */

/*版本控制*/
#define PVR_VERSION				(4.0)	//系统版本
#define PVR_DEBUG				(1)		//版本类型--1: 调试版本;0:发布版本

/*功能控制开关*/
/*是否打开编解码 功能*/
#define ENCODE_DECODE_ENABLE
/*是否打开USB Host 功能*/
#define USB_ENABLE	

#define FLASH_ENABLE
#define FLASHMAGICNUMBER 	(0xDEADFACE)

//#define DEBUG_MESSAGEBOX


/*功能配置*/
/*分区数量*/
#define SUPPORT_DISK_CNT		(5)//(3)
#define DISK_CF					(0)
#define DISK_USB				(1)
#define DISK_SD					(2)


/*显示设置*/
#define FILES_PER_BLOCK			(10)		//File browser中每页显示的文件数


#define UI_OSTTIMEDLY			(1)		//按键响应时间(1 = 100ms) 
										//OSD 最小延时单位
/*From pvr_config.h*/
#define FILE_COPY_BUFFER_LENGTH		(0x10000)

//--------------------------------------------------------------------------//
//	Purpose	:	模块容量设定
//--------------------------------------------------------------------------//
//
#define FILENAME_MAINLEN		(9)		//8+3主文件名长度

#define LANGUAGE_CNT			(3)		//系统支持语言:  英文\简体中文\繁体中文
#define REC_FORMAT_CNT			(9)		//系统支持的最大录制模式:  PSP\PMP\iPod\Pocket PC\D Cam\Mobile

#define SCHEDULE_NAME_LEN		(12)		//录像文件的名字最多为8个字符,如:Rec00000,".avi\0"等后缀不包含在内
#define ACTUALSCHEDULE_NAME_LEN	(9)	//实际最大长度，包括结尾0



//--------------------------------------------------------------------------//
//	Purpose	:	 坐标定义 
//--------------------------------------------------------------------------//
//
#define COORDINATE_X0_PROG_NUM		(550)
#define COORDINATE_Y0_PROG_NUM		(60)
#define COORDINATE_XSIZE_PROG_NUM	(160)
#define COORDINATE_YSIZE_PROG_NUM	(80)

//--------------------------------------------------------------------------//
//	Purpose	:	 文件系统函数相关定义 
//--------------------------------------------------------------------------//
#define EXTNAME_LEN					(8)	//文件扩展名最大长度

//--------------------------------------------------------------------------//
//	Purpose	:	默认设置
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
#define ENABLE_422_KEY_MSG          1//rs422 接收按键消息并返回新格式的状态消息

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

//SD卡、CF卡、U盘 均mount到固定目录下
#define SD_MOUNT_DIR			"/media/sd"
#define USB_MOUNT_DIR			"/media/usb"
#define CF_MOUNT_DIR			"/media/cf"

#define FAKE_DEV_STAT_FILE		".fakefile"

#define MIN_WARNING_FREE_SPACE_MB 500//zqh 2015年10月18日 add， 当硬盘空间小于500M是开始警告
#define MIN_FREE_SPACE_MB		  100
#define MAX_RECORDFILE_SIZE_MB    1500

#define FPGA_PATH_LED2			"/sys/class/davinci_display/ch0/toggle_led2"
#define FPGA_PATH_LED3			"/sys/class/davinci_display/ch0/toggle_led3"
#define FPGA_LED2				0
#define FPGA_LED3				1
#define FPGA_LED_ON				0
#define FPGA_LED_OFF			1

#define UPDATELOADMODULESSCRIPT   1//用于打开是否升级opt目录下的loadmodules-ef.sh文件的功能，0为不启用，1为启用

#if  UPDATELOADMODULESSCRIPT
#define UPDATE_LOAD_MODULE_SH_NAME    "loadmodules-ef.sh"//zqh add
#endif

#define UPDATE_FILE_NAME		"pvr"
#define UPDATE_DAEMON_NAME		"daemon"
#define UPDATE_KERNEL_NAME		"uImage"	//内核文件名

#define UPDATE_KERNEL_CMD1		"flash_eraseall -q /dev/mtd2"
#define UPDATE_KERNEL_CMD2		"nandwrite -m -n -p -q /dev/mtd2 " //后面还需要添加内核文件名的全路径

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
#define WRITE_LOG_TO_CF			0//zqh 2015年10月18日 modify， 将日志保存在电子盘功能去掉
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

#define MAX_LOG_FILE_SIZE		10	// 最多保存两个日志，当前日志最大大小 10Mbyte

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

// 目前通过检查usb的中断数，并不能定性的发现是否是usb不正常
#define CHECK_USB_EN            0

// file copy thread enabled ,used in v2.1 & v2.0
#define USE_FILE_COPY_THREAD    1

// 使用cf在位检测子线程处理cf卡加载的响应,子线程持续直到程序退出
#define USE_MOUNT_CF_THREAD             1

// 每次调用mount的时候只进行一次加载,加载成功就退出线程,失败根据情况重新加载或退出
#define USE_MOUNT_CF_THREAD_S           0


// 音频编解码相关
#define SUPPORT_AUDIO_ENC       1
#define AUDIO_TEST_TAG          1

#define APPEND_AVI_IDX          1
#endif

