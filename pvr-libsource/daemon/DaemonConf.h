#ifndef __DAEMON_CONFIG__
#define __DAEMON_CONFIG__

#if 0
//PATH
//#define PVREXE "/opt/pvr" 

//#define PVRBACK "/opt/pvr.tmp"

// get exit code in daemon
#define EXIT_CODE_PVR_ERROR		(2)
#define EXIT_CODE_NORMAL		(1)
#define EXIT_CODE_UPDATE		(10)
#define EXIT_CODE_REBOOT		(11)	// Add 0520-2012 , reboot system after update daemon & kernel

// care debug_v3 conditions
#define RFS_MOUNT_TYPE		"yaffs2" //"jffs2"	//"yaffs2"

// TODO: update lock file. To indicate update is in progress
#define UPDATE_LOCK_FILE		"/opt/pvr.lock"

#endif
#include "../appl/include/config.h"

// LOG
#define LOGE(...) fprintf(stderr,"E:" __VA_ARGS__);fflush(stderr)
#define LOGW(...) fprintf(stderr,"W:" __VA_ARGS__);fflush(stderr)
#define LOGT(...) fprintf(stderr,"T:" __VA_ARGS__);fflush(stderr)


#define UPDATE_TMP_DAEMON		"/opt/daemon.t"
#define UPDATE_TMP_PVR			"/opt/pvr.t"

#endif
