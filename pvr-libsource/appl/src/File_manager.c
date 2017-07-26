/*
----------------------------------------------------------------------
File		:	File_manager.c
Purpose	:
----------------------------------------------------------------------
Version-Date			:	2006.12.22
Author-Explanation		:	Sean.Yu
----------------------------------------------------------------------
*/

//#include <config.h>	//Config for platform
//#include <UI_Message.h>	//UI message
#include "File_manager.h"

#include "UCS2GB2312.h"
#include "Task_UI/task_ui_var.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include "../../pvr_debug.h"

/**
 * @brief scan_dir_internal
 * 检索文件夹内的文件列表函数
 * @param fullpath
 * 文件夹路径
 * @param user_ptr
 * 文件检索过滤回调函数指针
 * @return
 */
static int scan_dir_internal(char *fullpath,int (*scan_callback)(char *full_entry_path,struct dirent *fentry,struct stat *fstat,void *user_ptr),void *user_ptr)
{
	struct stat statbuf;
	struct dirent *dirp;
	char  full_path_local[MAX_FS_PATH];
	DIR *dp;
	char *ptr;
	int ret = -1;

	if(fullpath==NULL)
		return -1;
	if(lstat(fullpath,&statbuf)<0) {
//stat error
		//LOGT("%s :lstat \n",__func__);
		return -1;
	}
	if(S_ISDIR(statbuf.st_mode)==0) {
//not a directory	
		//LOGT("%s :S_ISDIR \n",__func__);
		return -1;
	}
	//ptr point to the last element of the fullpath
	ptr = fullpath + strlen(fullpath)-1;
	strcpy(full_path_local,fullpath);
	if(*ptr!='/') {
		//append '/' at the end of the path
		full_path_local[strlen(fullpath)]='/';
		full_path_local[strlen(fullpath)+1]=0;
		ptr=&full_path_local[strlen(fullpath)+1];
	}else
		ptr = &full_path_local[strlen(fullpath)];
	//ptr point to the end of full_path_local
	if((dp=opendir(full_path_local))==NULL) {
		//LOGT("%s :opendir %s \n",__func__,full_path_local);
		return -1;
	}		
	while((dirp=readdir(dp))!=NULL) {
		if(strcmp(dirp->d_name,".")==0)
			continue;
		if(strcmp(dirp->d_name,"..")==0)
			continue;
		strcpy(ptr,dirp->d_name);
		if(lstat(full_path_local,&statbuf)<0)
			goto out;
		if(scan_callback!=NULL)
			if(scan_callback(full_path_local,dirp,&statbuf,user_ptr)<0) {
				//LOGT("%s: callback return failed",__func__);
				goto out;
			}
	}
	ret = 1;
out:
	if(dp!=NULL) {
		if(closedir(dp)<0) {
			//LOGT("%s :closedir \n",__func__);			
			return -1;
		}	
	}	
	return ret;
}

//! 目录里avi文件数，用于检索目录时使用
static int dir_file_count=0;
//return 1 to continue scan the dir
//return -1 to stop the scan
/**
 * @brief FilterFile_count
 * 根据filter检索目录里文件数，检索到的文件名符合filter格式时，检索到的文件数加1
 * @param full_entry_path
 * 目录路径，未使用
 * @param fentry
 * 输入参数，目录信息结构体指针
 * @param fstat
 * 输入参数，文件信息结构体指针
 * @param user_ptr
 * 输入参数，文件后缀名filter
 * @return
 * 1 继续检索目录
 * -1 停止检索目录
 */
static int FilterFile_count(char *full_entry_path,struct dirent *fentry,struct stat *fstat,void *user_ptr)
{
	char *filter=(char *)user_ptr;
	char *ptr=filter, *ext=filter, ch;
	int  len;

	//LOGT("%s : fentry->d_name=%s\r\n",__func__,fentry->d_name);
	if(S_ISDIR(fstat->st_mode)) {
		if(pUISystem->list_contain & CONTAIN_DIR) {
			dir_file_count++;
		}
		return 1;
	}		
	if( !(pUISystem->list_contain & CONTAIN_FILE))
		return 1;
		
	// pass all, when filter=NULL,"\0", or a single "*"
	if(!filter) {
		dir_file_count++;
		return 1;
	}	
		
	if(!strlen(filter) || !strcmp(filter, "*")){
		dir_file_count++;
		return 1;
	}	

	// filter file by ext name
	while( (ch = *ptr++) ) 
	{
		if(ch == '|') 
		{
			len = ptr-1 - ext;
			if (!strncasecmp((char *)(&fentry->d_name[strlen(fentry->d_name)-len]), ext, len)) {
				dir_file_count++;
				return 1;
			}
			ext = ptr;
		}
	}
	return 1;
}

/*
Junr item5:
convert to fat time according to lt
*/
/**
 * @brief linuxt_2_fatt
 * 将linux时间格式转换成fat文件系统中文件时间格式
 * @param lt
 * linux格式的时间指针
 * @param fatt
 * fat格式的时间指针
 */
static void linuxt_2_fatt(time_t *lt,struct fatfs_file_time_t *fatt)
{
	struct tm *tm_time;
	if(fatt!=NULL && lt !=NULL) {
		tm_time = localtime(lt);
		fatt->Day = tm_time->tm_mday;
		fatt->Hour = tm_time->tm_hour;
		fatt->Minute=tm_time->tm_min;
		fatt->Month =tm_time->tm_mon+1;
		fatt->Second=tm_time->tm_sec;
		fatt->Year=tm_time->tm_year+1900;
	}
}

//! 目录中检索到的文件索引
static int cur_entry_index=0;

//! 文件信息结构体
struct FileListBlock_add{
	char *filter;
	struct FileListBlock *pflb;
};
//return 1 to continue scan
//return -1 to stop scan
/**
 * @brief FilterFile_moreinfo
 * 获取目录中检索到的文件的信息，将信息通过user_ptr返回
 * @param full_entry_path
 * 输入参数，当前目录路径
 * @param fentry
 * 输入参数，目录信息结构体指针
 * @param fstat
 * 输入参数，文件信息结构体指针
 * @param user_ptr
 * 输出参数，获取到的文件信息列表结构体指针
 * @return
 * 1：继续检索下一个文件
 * -1：停止
 */
static int FilterFile_moreinfo(char *full_entry_path,struct dirent *fentry,struct stat *fstat,void *user_ptr)
{
	int len;
	struct FileListBlock * dirlist = ((struct FileListBlock_add *)user_ptr)->pflb;
	char *pfilter = ((struct FileListBlock_add *)user_ptr)->filter;
	int last_filter_count = dir_file_count;

	//LOGT("%s : fentry->d_name=%s\r\n",__func__,fentry->d_name);
	
	if(FilterFile_count(full_entry_path,fentry,fstat,pfilter)>0) {
		if((last_filter_count+1)==dir_file_count) {//valid entry
			strcpy(dirlist->attribute[cur_entry_index].shortname, fentry->d_name);//JunR note: over flow protection needed
			//--长文件名
//			ucs2gb_int(fentry->d_name, dirlist->attribute[cur_entry_index].longname, LONG_FILENAME_LEN);	//国标(因为只有字符显示)
			strcpy(dirlist->attribute[cur_entry_index].longname, fentry->d_name);
			//--文件类型(选择性调用)
			len = strlen(dirlist->attribute[cur_entry_index].shortname);
			memcpy(dirlist->attribute[cur_entry_index].suffix, &dirlist->attribute[cur_entry_index].shortname[len - 3], 3);
			dirlist->attribute[cur_entry_index].suffix[3] = '\0';
			linuxt_2_fatt(&fstat->st_ctime,&dirlist->attribute[cur_entry_index].timeinfo.ftCreationTime);
			linuxt_2_fatt(&fstat->st_atime,&dirlist->attribute[cur_entry_index].timeinfo.ftLastAccessTime);
			linuxt_2_fatt(&fstat->st_mtime,&dirlist->attribute[cur_entry_index].timeinfo.ftLastWriteTime);
			dirlist->attribute[cur_entry_index].size=fstat->st_size;
			if(S_ISDIR(fstat->st_mode)) {
				dirlist->attribute[cur_entry_index].flag |=FILE_DIR_MASK;
			}	
			cur_entry_index++;
		}
		return 1;
	}else
		return -1;//stop scan;
}

/**
 * @brief GetFileCount
 * 获取符合filter的文件个数
 * @param path
 * @param filter
 * @return
 */
static int GetFileCount(char * path, char *filter)
{
	int local_dir_file_count = -1;
	dir_file_count = 0;
	if(scan_dir_internal(path,FilterFile_count,filter)>0) {
		//LOGT("%s: dir_file_count = %d\r\n",__func__,dir_file_count);
		local_dir_file_count = dir_file_count;
	}
	dir_file_count = 0;
	return local_dir_file_count;
}

/**
 * @brief NameSort
 * 按文件名排序
 * @param attrib_a
 * @param attrib_b
 * @return
 */
static int NameSort(struct FileAttrib *attrib_a , struct FileAttrib *attrib_b)
{
	//return strcmp(attrib_a->longname, attrib_b->longname);
	return strcmp(attrib_b->longname, attrib_a->longname);
}

/**
 * @brief TypeSort
 * 按文件类型名排序
 * @param attrib_a
 * @param attrib_b
 * @return
 */
static int TypeSort(struct FileAttrib *attrib_a , struct FileAttrib *attrib_b)
{
	return strcmp(attrib_a->suffix, attrib_b->suffix);
}

/**
 * @brief SizeSort
 * 按文件大小排序
 * @param attrib_a
 * @param attrib_b
 * @return
 */
static int SizeSort(struct FileAttrib *attrib_a , struct FileAttrib *attrib_b)
{
	if(attrib_a->size > attrib_b->size)
	{
		return 1;
	}
	else if(attrib_a->size == attrib_b->size)
	{
		return 0;
	}
	else 
	{
		return -1;
	}
}

/**
 * @brief DateSort
 * 按文件时间排序
 * @param attrib_a
 * @param attrib_b
 * @return
 */
static int DateSort(struct FileAttrib *attrib_a , struct FileAttrib *attrib_b)
{
	int cnt;
	char a_buf[16] = "";
	char b_buf[16] = "";

	//attribute a
	cnt = sprintf(a_buf, "%d", attrib_a->timeinfo.ftLastWriteTime.Year);
	cnt += sprintf(a_buf + cnt, "%d", attrib_a->timeinfo.ftLastWriteTime.Month);
	if(cnt == 5)
	{
		a_buf[6] = 0;
		a_buf[5] = a_buf[4];
		a_buf[4] = '0';
		cnt++;
	}
	cnt += sprintf(a_buf + cnt, "%d", attrib_a->timeinfo.ftLastWriteTime.Day);
	if(cnt == 7)
	{
		a_buf[8] = 0;
		a_buf[7] = a_buf[6];
		a_buf[6] = '0';
		cnt++;
	}
	cnt += sprintf(a_buf + cnt, "%d", attrib_a->timeinfo.ftLastWriteTime.Hour);
	if(cnt == 9)
	{
		a_buf[10] = 0;
		a_buf[9] = a_buf[8];
		a_buf[8] = '0';
		cnt++;
	}
	cnt += sprintf(a_buf + cnt, "%d", attrib_a->timeinfo.ftLastWriteTime.Minute);
	if(cnt == 11)
	{
		a_buf[12] = 0;
		a_buf[11] = a_buf[10];
		a_buf[10] = '0';
		cnt++;
	}

	//attribute b
	cnt = sprintf(b_buf, "%d", attrib_b->timeinfo.ftLastWriteTime.Year);
	cnt += sprintf(b_buf + cnt, "%d", attrib_b->timeinfo.ftLastWriteTime.Month);
	if(cnt == 5)
	{
		b_buf[6] = 0;
		b_buf[5] = b_buf[4];
		b_buf[4] = '0';
		cnt++;
	}
	cnt += sprintf(b_buf + cnt, "%d", attrib_b->timeinfo.ftLastWriteTime.Day);
	if(cnt == 7)
	{
		b_buf[8] = 0;
		b_buf[7] = b_buf[6];
		b_buf[6] = '0';
		cnt++;
	}
	cnt += sprintf(b_buf + cnt, "%d", attrib_b->timeinfo.ftLastWriteTime.Hour);
	if(cnt == 9)
	{
		b_buf[10] = 0;
		b_buf[9] = b_buf[8];
		b_buf[8] = '0';
		cnt++;
	}
	cnt += sprintf(b_buf + cnt, "%d", attrib_b->timeinfo.ftLastWriteTime.Minute);
	if(cnt == 11)
	{
		b_buf[12] = 0;
		b_buf[11] = b_buf[10];
		b_buf[10] = '0';
		cnt++;
	}

	//return strcmp(a_buf, b_buf);
	return strcmp(b_buf,a_buf);
}

/**
 * @brief Scandir
 * 检索目录接口函数，通过调用该函数，从目录中读取到符合filter的文件的文件数、文件属性，并按照
 * sortMethod将文件排序，保存到文件列表中
 * @param dirlist
 * 输入参数/输出参数，保存要检索的文件所在目录参数，并将检索到的文件信息放在该结构体
 * @param filter
 * 文件名过滤
 * @param sortMethod
 * 文件列表中文件排列方式
 * @return
 * 检索正常返回TRUE，发生异常返回FALSE
 */
BOOL Scandir(struct FileListBlock * dirlist, char *filter, int sortMethod)
{
	int count;
	struct FileListBlock_add fba;
	int len = sizeof(struct FileAttrib);//SACH:W10866
	
	pUISystem->list_contain = dirlist->contain;

	//LOGT("%s: dir: %s filter:%s sort %d\r\n",__func__,dirlist->directory,filter,sortMethod);
	
	if(dirlist->attribute) 
	{
		free(dirlist->attribute);
		dirlist->attribute = NULL;
	}

    // 获取文件数，在无文件时直接返回
	if(((count = GetFileCount(dirlist->directory, filter))) <= 0)//SACH:W10875
	{
		dirlist->nCount = 0;
		return TRUE;
	}

    // 申请文件列表空间，由于此处使用malloc，因此在需要再次检索目录信息之前，需要free
	dirlist->attribute = (struct FileAttrib *)malloc(sizeof(struct FileAttrib)*count);
	if (dirlist->attribute == NULL) {
        LOGE("mallco failed at %s\r\n",__func__);
//		request2reboot();
		return FALSE;
	}	
	pUISystem->max_filecount_ever = count;
	
	memset(dirlist->attribute, 0, len*count);//SACH:W10866
	dirlist->nCount = count;

    // 获取文件信息并将列表排序
	fba.pflb=dirlist;
	fba.filter=filter;
	cur_entry_index = 0;
	if(scan_dir_internal(dirlist->directory,FilterFile_moreinfo,&fba)<0) {
		return FALSE;
	}else {
		switch(sortMethod)
		{
			case SORT_METHOD_NAME:
				qsort(dirlist->attribute, count, sizeof(struct FileAttrib), (int(*)(const void*, const void*))NameSort);
				break;
			
			case SORT_METHOD_TYPE:
				qsort(dirlist->attribute, count, sizeof(struct FileAttrib), (int(*)(const void*, const void*))TypeSort);
				break;
				
			case SORT_METHOD_SIZE:
				qsort(dirlist->attribute, count, sizeof(struct FileAttrib), (int(*)(const void*, const void*))SizeSort);
				break;
			
			case SORT_METHOD_DATE:
				qsort(dirlist->attribute, count, sizeof(struct FileAttrib), (int(*)(const void*, const void*))DateSort);
				break;
				
			default:
				qsort(dirlist->attribute, count, sizeof(struct FileAttrib), (int(*)(const void*, const void*))DateSort);
				break;
		}
		return TRUE;		
	}
}

/**
 * @brief FillFileList
 * 按顺序将目录文件列表中的文件名添加到list里从nIndex开始的位置
 * @param list
 * 传入参数/传出参数，文件名列表
 * @param dirlist
 * 目录中的文件列表结构体指针
 * @param nIndex
 * 要拷贝的文件名在list中的开始项
 * @return
 * 最终拷贝的文件名数
 */
int FillFileList(char *list[FILES_PER_BLOCK], struct FileListBlock * dirlist, int nIndex)
{
	int i;
	for(i=nIndex;i-nIndex<FILES_PER_BLOCK && i<dirlist->nCount ;++i) {
		list[i-nIndex] = dirlist->attribute[i].longname;
	}
	return i-nIndex;
}

/**
 * @brief ReleaseList
 * 调用free释放目录文件列表结构体申请的资源
 * @param listhdr
 * 目录文件列表结构体
 */
void ReleaseList(struct FileListBlock * listhdr)
{
	if(listhdr->attribute) {
		free(listhdr->attribute);
		listhdr->attribute = NULL;
	}
	pUISystem->max_filecount_ever = 0;
}

