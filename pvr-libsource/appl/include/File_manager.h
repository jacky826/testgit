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
 * ʱ��ṹ�壬�ļ�������ʹ��
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
 * �����ļ��������ļ�ʱ��Ľṹ��
 */
struct fatfs_file_time_info_s {
    struct fatfs_file_time_t	ftCreationTime;     /// ����ʱ��
    struct fatfs_file_time_t	ftLastAccessTime;   /// ������ʱ��
    struct fatfs_file_time_t	ftLastWriteTime;    /// ����޸�ʱ��
};

#define SHORT_FILENAME_LEN      (64)                /// ���ļ��������У��ļ����ĳ�������
#define LONG_FILENAME_LEN       (64)                /// ���ļ��������У��ļ����ĳ�������
#define MAX_FS_PATH             (266)               /// �ļ�ϵͳ·������󳤶�


#define FILE_DIR_MASK			0x00000001          /// Ŀ¼�������룬�����ļ�ΪĿ¼
#define FILE_TYPE_MASK			0x00000006          /// �ļ��������룬Ŀǰδʹ��


/**
 * @brief The FileAttrib struct
 * �ļ����Խṹ��
 */
struct FileAttrib
{
    unsigned long flag;                         /// �ļ����Ա�ʶ�����ڱ�ʾ�ļ��Ƿ�ΪĿ¼
    char shortname[SHORT_FILENAME_LEN];         /// ���ļ�������
    char longname[LONG_FILENAME_LEN];           /// ���ļ�������
    struct fatfs_file_time_info_s timeinfo;     /// �ļ���ʱ������
    char suffix[4];                             /// �ļ���׺��avi\0��
    unsigned long size;                         /// �ļ���С����
    unsigned long duration;                     /// �ļ�����ʱ������
};

#define CONTAIN_FILE			0x00000001      /// ����ʱ�����ļ���ʶ
#define CONTAIN_DIR				0x00000002      /// ����ʱ����Ŀ¼��ʶ

/**
 * @brief The FileListBlock struct
 * ���������ļ������б�ṹ��
 */
struct FileListBlock
{
    int  nCount;                    /// ���������ļ�����
    unsigned long contain;          /// ���������ļ������ļ�
    char directory[MAX_FS_PATH];    /// Ҫ�������ļ�Ŀ¼
    struct FileAttrib * attribute;  /// ���������ļ������б���ʼָ��
};

BOOL Scandir(struct FileListBlock * dirlist, char *filter, int sortMethod);
int FillFileList(char *list[FILES_PER_BLOCK], struct FileListBlock * dirlist, int nPage);
void ReleaseList(struct FileListBlock * listhdr);

#endif

