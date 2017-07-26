/*
----------------------------------------------------------------------
File		:	File_manager.h
Purpose	:
----------------------------------------------------------------------
Version-Date			:	2006.12.22
Author-Explanation		:	Sean.Yu
----------------------------------------------------------------------
*/

#ifndef  __FILE_MANAGER_DEFINED
#define __FILE_MANAGER_DEFINED

#include <time.h>
#include "pvr_types.h"
#include "config.h"

/**
 * @brief The fatfs_file_time_t struct
 * 时间结构体，文件属性中使用
 */
struct fatfs_file_time_t {
    WORD	Year;
	WORD	Month;
	WORD	Day;
	WORD	Hour;
	WORD	Minute;
	WORD	Second;
};

/**
 * @brief The fatfs_file_time_info_s struct
 * 保存文件属性中文件时间的结构体
 */
struct fatfs_file_time_info_s {
    struct fatfs_file_time_t	ftCreationTime;     /// 创建时间
    struct fatfs_file_time_t	ftLastAccessTime;   /// 最后访问时间
    struct fatfs_file_time_t	ftLastWriteTime;    /// 最后修改时间
};

#define SHORT_FILENAME_LEN      (64)                /// 短文件名属性中，文件名的长度限制
#define LONG_FILENAME_LEN       (64)                /// 长文件名属性中，文件名的长度限制
#define MAX_FS_PATH             (266)               /// 文件系统路径的最大长度


#define FILE_DIR_MASK			0x00000001          /// 目录属性掩码，标明文件为目录
#define FILE_TYPE_MASK			0x00000006          /// 文件类型掩码，目前未使用


/**
 * @brief The FileAttrib struct
 * 文件属性结构体
 */
struct FileAttrib
{
    unsigned long flag;                         /// 文件属性标识，用于表示文件是否为目录
    char shortname[SHORT_FILENAME_LEN];         /// 短文件名属性
    char longname[LONG_FILENAME_LEN];           /// 长文件名属性
    struct fatfs_file_time_info_s timeinfo;     /// 文件的时间属性
    char suffix[4];                             /// 文件后缀（avi\0）
    unsigned long size;                         /// 文件大小属性
    unsigned long duration;                     /// 文件播放时间属性
};

#define CONTAIN_FILE			0x00000001      /// 检索时包含文件标识
#define CONTAIN_DIR				0x00000002      /// 检索时包含目录标识

/**
 * @brief The FileListBlock struct
 * 检索到的文件属性列表结构体
 */
struct FileListBlock
{
    int  nCount;                    /// 检索到的文件个数
    unsigned long contain;          /// 检索到的文件包含文件
    char directory[MAX_FS_PATH];    /// 要检索的文件目录
    struct FileAttrib * attribute;  /// 检索到的文件属性列表起始指针
};

BOOL Scandir(struct FileListBlock * dirlist, char *filter, int sortMethod);
int FillFileList(char *list[FILES_PER_BLOCK], struct FileListBlock * dirlist, int nPage);
void ReleaseList(struct FileListBlock * listhdr);

#endif

