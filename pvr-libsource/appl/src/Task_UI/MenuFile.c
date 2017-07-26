#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h> 

#include <pvr_types.h>
#include <config.h>	//Config for platform

#include <UI_Message.h>	//UI message
#include <Task_UI/task_ui_var.h>
#include <Task_UI/pvr_ui_func.h>
#include <GUI.H>
#include <Key.h>
#include "../../avi/decoder.h"
extern const char version_string[];


/*File Browser: List File info
 *--------------- MenuFile.c
 */
/*File Browser: change to next/prev page
 *--------------- MenuFile.c
 *Parameter	:	@left_or_right: 
 *					按了左键/前翻页--(-1)
 *					按了右键/后翻页--(+1)
 */
int MenuFile_PageChange(UI_Menu_t *pMenu, int adjustval)
{
    if(pUISystem->FileBrowserdirlist.nCount <= FILES_PER_BLOCK)//判断cf卡里是否多于十个文件
		return 0;

	if(adjustval>0)	{	//后翻页
		if((pUISystem->FileBrowserIndex + FILES_PER_BLOCK) < pUISystem->FileBrowserdirlist.nCount) {
			pUISystem->FileBrowserIndex += FILES_PER_BLOCK;
		} else {
			pUISystem->FileBrowserIndex = 0;
		}
    } else if(adjustval<0)	{  //前翻页
		if((pUISystem->FileBrowserIndex  - FILES_PER_BLOCK) >= 0) {
			pUISystem->FileBrowserIndex -= FILES_PER_BLOCK;
		} else {
			pUISystem->FileBrowserIndex = ((pUISystem->FileBrowserdirlist.nCount-1) / FILES_PER_BLOCK) * FILES_PER_BLOCK;
		}
	}
	pMenu->nButtonIdx = 0;
	return 1;
}

/*File Browser: change to next/prev single item
 *--------------- MenuFile.c
 *Parameter	:	@left_or_right: 
 *					按了上键/上一个--(-1)
 *					按了下键/下一个--(+1)
 */
int MenuFile_SingleChange(UI_Menu_t *pMenu, int adjustval)
{
	int index;		//总的Index(0---TotalNum-1)


    if(pUISystem->FileBrowserdirlist.nCount <= 1) {//判断cf卡里是否只有一个文件
		return 0;
	}
	
	index = pUISystem->FileBrowserIndex + pMenu->nButtonIdx;
	
	if(adjustval>0)	{	//下一个
		if((index + 1) < pUISystem->FileBrowserdirlist.nCount) {
			index++;
		} else {
			index = 0;
		}
	} else if(adjustval<0)	{			//上一个
		if(index <= 0) {
			index = pUISystem->FileBrowserdirlist.nCount - 1;
		} else {
			index --;
		}
	}
	pUISystem->FileBrowserIndex = (index / FILES_PER_BLOCK) * FILES_PER_BLOCK;
	pMenu->nButtonIdx = index - pUISystem->FileBrowserIndex;
	return 1;
}

/**
 * @brief GetCurrentFileFsInfo
 * 获得当前选中的文件的系统信息
 * @param nPageOffset
 * 页的偏移量
 * @param nOffset
 * 在当前页的偏移量
 * @param pDir
 * 目录结构体
 * @return
 */

struct FileAttrib *GetCurrentFileFsInfo(int nPageOffset,int nOffset,struct FileListBlock *pDir){
	int nFileIdx=nPageOffset+nOffset;
	if ((nFileIdx>=pDir->nCount)||(pDir->attribute==NULL)){
		return NULL;
	}
	return &(pDir->attribute[nFileIdx]);
}

/**
 * @brief GetCurrentFileName
 * 获得当前选中文件的名字
 * @param nPageOffset
 * 页偏移，比如有25个视频文件，每一页只显示10个文件，则页偏移的最大值为3
 * @param nOffset
 * 页偏移的内部偏移量，值为0~FILES_PER_BLOCK-1
 * @param pDir
 * 目录结构体
 * @return
 * 返回空则表示未获取到文件名。
 * 否则获取文件名成功
 */
char *GetCurrentFileName(int nPageOffset,int nOffset,struct FileListBlock *pDir){
	struct FileAttrib *pFsInfo=GetCurrentFileFsInfo(nPageOffset,nOffset,pDir);
	if (pFsInfo!=NULL){
		return pFsInfo->shortname;
	}
	return NULL;
}

/**
 * @brief GetCurrentFullPath
 * 获取当前文件的全路径，如/media/cf/20100622034355.avi
 * @param nPageOffset
 * 页偏移量
 * @param nOffset
 * 页本身的偏移量，范围0~FILES_PER_BLOCK-1
 * @param pDir
 * 目录结构体
 * @param sFullPath
 * 将获得的全路径名保存在此变量中
 * @return
 * 1：成功
 * -1：失败
 */
int GetCurrentFullPath(int nPageOffset,int nOffset,struct FileListBlock *pDir,char *sFullPath){
	struct FileAttrib *pFsInfo=GetCurrentFileFsInfo(nPageOffset,nOffset,pDir);
	if (pFsInfo!=NULL){
		snprintf(sFullPath,MAX_FS_PATH,"%s%s",pUISystem->Vol_Disk[pUISystem->nFileSelectDisk],pFsInfo->shortname);
        //LOGT("Vol_Disk : %s\n",pUISystem->Vol_Disk[pUISystem->nFileSelectDisk]);
        //LOGT("shortname : %s\n",pFsInfo->shortname);
        //LOGT("%s = %s\n",__func__,sFullPath);
		return 1;
	}
	return -1;
}

/*File Operations*/
/**
 * @brief DeleteSingleFile
 * 删除单个文件
 * @param filepath
 * 删除文件的全路径
 * @return
 */

int DeleteSingleFile(char *filepath)
{
	int index;
	int ret=-1;

	START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_DELETE_FILE);

	if (filepath){//SACH:W10889
		if (remove(filepath)==0){  //unlink return 0 is delete ok , return -1 is delete failure.
			*filepath=0;//Do set file name to NULL
			if(pUISystem->FileBrowserdirlist.nCount <= 1) {
				pUISystem->FileBrowserSelFile=0;
				pUISystem->FileBrowserIndex = 0;

				ret = 1;
			}
			index = pUISystem->FileBrowserIndex + pUISystem->FileBrowserSelFile;
			if(index>0)
				index--;
			pUISystem->FileBrowserIndex = (index / FILES_PER_BLOCK) * FILES_PER_BLOCK;
			pUISystem->FileBrowserSelFile = index - pUISystem->FileBrowserIndex;
		}
		ret = 1;
	}else {
		ret = -1;
	}

	sync();
	sync();
	STOP_WATCHDOG_TIMER_SIG();

	return ret;
}

//Message Box Call back
/**
 * @brief Delete_Callback
 * 删除单个文件的回调函数
 * @param pUser
 * 要删除的文件,例如/media/cf/20100622034355.avi
 * @return
 */
int Delete_Callback(void *pUser){
	int retval;
	Pvr_Waiting(TRUE);

    retval = DeleteSingleFile(pUser);//调用删除单个文件的函数

	return retval;
}

/**
 * @brief DeleteAllFile_Callback
 * 删除cf卡上的所有.avi文件的回调函数，在这个int DoDeleteAllFile(void)函数中调用
 * @return
 */
int DeleteAllFile_Callback(void)  //haiqing
{
	struct FileAttrib *pFsInfo=NULL;
	char sFullPath[MAX_FS_PATH];
	int cnt;
	int i;

    if(pUISystem->FileBrowserdirlist.nCount<=0)//检测cf卡上没有.avi文件，函数直接返回
	{
		pUISystem->FileBrowserdirlist.nCount = 0;
		if (!Scandir(&(pUISystem->FileBrowserdirlist), VIDEO_FILES, pUISystem->allData.CurSortMethod)) 
		{
			LOGE("%s: Scandir failed\n",__func__);
			ReleaseList(&(pUISystem->FileBrowserdirlist));
			return -1;
		}
	}

	Pvr_Waiting(TRUE);

    cnt = pUISystem->FileBrowserdirlist.nCount;//获取当前cf卡上.avi文件的个数

	for(i=0;i<cnt;i++)
	{
		pFsInfo = &(pUISystem->FileBrowserdirlist.attribute[i]);
		if(strncmp(pFsInfo->shortname,RESERVED_HEADER_STR,RESERVED_HEADER_LEN)==0)
		{
			LOGT("reserved file,do not delete\r\n");
			continue;
		}

		snprintf(sFullPath,MAX_FS_PATH,"%s%s",pUISystem->Vol_Disk[pUISystem->nFileSelectDisk],pFsInfo->shortname);
		LOGT("Delete file %s\r\n",sFullPath);
		if(DeleteSingleFile(sFullPath)<0)
		{
			LOGT("delete file %s error\r\n",sFullPath);
		}
	}

#if NEED_UMOUNT_CF
    SystemUmountDev(CF_MOUNT_DIR);

#if !(USE_MOUNT_CF_THREAD) && !(USE_MOUNT_CF_THREAD_S)
    // 使用线程处理加载时,不在此处进行加载操作
	SystemMountCFCard();
#endif

#endif

	return 0;
}

/**
 * @brief DeleteMessage_Deinit
 * 删除文件后，对窗口进行反初始化
 * @param nOKFail
 * 忽略
 * @return
 */
int DeleteMessage_Deinit(int nOKFail)
{
	SystemChangeWindow(MENUFILEBROWSER,1,1,0);
#if USE_MOUNT_CF_THREAD_S
    SystemMountCFCard();
#endif
    
	return 1;
}

/**
 * @brief DoPlayFile
 * 播放视频文件
 * @return
 */

int DoPlayFile(void){
	if(GetCurrentFullPath(pUISystem->FileBrowserIndex,
		pUISystem->FileBrowserSelFile,&pUISystem->FileBrowserdirlist,pUISystem->sRecFilename)>0){
		return SystemChangeWindow(MENUPLAYBACK,0,1,1);//No close file,especial for File Browser 
	}
	return -1;
	
}

/**
 * @brief DoDeleteFile
 * 删除文件
 * @return
 */

int DoDeleteFile(void)
{
	int ret;
	char *filename = NULL;
    filename=GetCurrentFileName(pUISystem->FileBrowserIndex,pUISystem->FileBrowserSelFile,&pUISystem->FileBrowserdirlist);//获取当前选中的文件名
	if(filename == NULL)
	{
		LOGT("can't get current file name\r\n");
		return -1;
	}

    //LOGT("delete file %s\r\n",filename);
    if(strncmp(filename,RESERVED_HEADER_STR,RESERVED_HEADER_LEN)==0)//检测删除的文件是否为特定文件
	{
		return MessageBox(SpecificFile[pUISystem->allData.Language],0,
							NULL,NULL,NULL,NULL,NULL,NULL);
	}
	else
	{
        snprintf(pUISystem->sRecFilename,MAX_FS_PATH,"%s%s",//生成文件的全路径，如/media/cf/20100622034355.avi
					pUISystem->Vol_Disk[pUISystem->nFileSelectDisk],filename);
		return MessageBox(MsgBox30[pUISystem->allData.Language],CODEC_MESSAGE_DELAY,Delete_Callback,
			NULL,pUISystem->sRecFilename,MsgBox23[pUISystem->allData.Language],
			MsgBox24[pUISystem->allData.Language],DeleteMessage_Deinit);
	}
}

//haiqing 
/**
 * @brief DoDeleteAllFile
 * 删除cf卡上全部.avi文件时，生成信息弹窗
 * @return
 */
int DoDeleteAllFile(void)
{
    return MessageBox(MsgBox46[pUISystem->allData.Language],CODEC_MESSAGE_DELAY,
            DeleteAllFile_Callback,NULL,NULL,MsgBox23[pUISystem->allData.Language],
            MsgBox24[pUISystem->allData.Language], DeleteMessage_Deinit);
}

/**
 * @brief MenuFileBrowser_Init
 * 文件浏览界面之前的一些初始化工作，比如：获得cf卡里有多少个avi文件，设置初始化选中button
 * @param pMenu
 * 当前的活动窗口
 * @return
 */

int MenuFileBrowser_Init(UI_Menu_t *pMenu)
{
    int i;

    FUNC_ENTER;

    // Set FileBrowserdirlist directory and contain file type
    //获取cf卡里.avi文件的个数
    strcpy(pUISystem->FileBrowserdirlist.directory,pUISystem->Vol_Disk[pUISystem->nFileSelectDisk]);
    pUISystem->FileBrowserdirlist.contain = CONTAIN_FILE;
    pUISystem->FileBrowserdirlist.nCount = 0;

    //Start Scan
    if (!Scandir(&(pUISystem->FileBrowserdirlist), VIDEO_FILES, pUISystem->allData.CurSortMethod))//获得cf卡中有多少个.avi文件，并以指定的方法排序
    {
        LOGE("%s: Scandir failed\n",__func__);
        ReleaseList(&(pUISystem->FileBrowserdirlist));
        //		system("echo 1 > /sys/class/davinci_display/ch0/lvdsswitch");  //swtich lvds to bypass mode
        SetLvdsSwitch(1);

        LOGT("MenuFile exit(0)\n");
        request_exit_main_task = 1;
        exit_code = EXIT_CODE_PVR_ERROR;

        return -1;
    }


    MenuButtonMaskBitmap[pMenu->nMenuIdx]=0;//默认使能所有button

    if(pUISystem->FileBrowserdirlist.nCount==0)//判断cf卡里avi文件个数是否为0
    {
        for (i=0;i<BUTTON_CNT_FILEBROWSER;i++)
        {
            MenuButtonMaskBitmap[pMenu->nMenuIdx]|=(1<<i);//将10个button屏蔽掉
        }
        pUISystem->FileBrowserIndex = 0;
        pUISystem->FileBrowserSelFile = 0;
        pMenu->nButtonIdx=-1;	//set to invalid button

        return 1;
    }

    //假如cf卡里文件个数少于10个，假如有6个avi文件，则屏蔽掉第6-9（从0开始）个文件的button（屏蔽即不在界面上绘制他们的button）
    if(pUISystem->FileBrowserdirlist.nCount<BUTTON_CNT_FILEBROWSER_FILELIST)
    {
        for (i=pUISystem->FileBrowserdirlist.nCount;i<BUTTON_CNT_FILEBROWSER;i++)
        {
            MenuButtonMaskBitmap[pMenu->nMenuIdx]|=(1<<i);
        }
    }
    else
    {//cf卡里大于等于10个文件，则绘制10个button
        for (i=BUTTON_CNT_FILEBROWSER_FILELIST;i<BUTTON_CNT_FILEBROWSER;i++){
            MenuButtonMaskBitmap[pMenu->nMenuIdx]|=(1<<i);
        }
    }

    //! check current page start index & current select file index
    while(pUISystem->FileBrowserIndex >= pUISystem->FileBrowserdirlist.nCount)
    {
        pUISystem->FileBrowserIndex -= BUTTON_CNT_FILEBROWSER_FILELIST;
        if(pUISystem->FileBrowserIndex < 0)
        {
            pUISystem->FileBrowserIndex = 0;
            break;
        }
    }

    //删除一个文件后切换到文件浏览窗口时，使选中的button上移
    while((pUISystem->FileBrowserSelFile + pUISystem->FileBrowserIndex)
          >= pUISystem->FileBrowserdirlist.nCount)
    {
        pUISystem->FileBrowserSelFile--;
        if(pUISystem->FileBrowserSelFile < 0)
        {
            pUISystem->FileBrowserSelFile = 0;
            break;
        }
    }

    pMenu->nButtonIdx=pUISystem->FileBrowserSelFile;

    return 1;
}

/**
 * @brief MenuFileBrowser_Close
 * 释放pUISystem->FileBrowserdirlist内存
 * @param pMenu
 * 当前活动窗口
 * @return
 */

int MenuFileBrowser_Close(UI_Menu_t *pMenu)
{
//	system("echo 1 > /sys/class/davinci_display/ch0/lvdsswitch");  //swtich lvds to bypass mode

	ReleaseList(&(pUISystem->FileBrowserdirlist));
	return 1;
}

/**
 * @brief MenuFileBrowser_RefreshItemEx
 * 画文件浏览界面主窗体的函数
 * @param dirlist
 * 传入参数：文件列表块
 * @param Index
 * 传入参数：页内偏移
 * @param PageIndex
 * 传入参数：页的值。比如：有25个文件，则此变量取值范围为0-2
 * @param nIdx
 * 传入参数：button值
 * @param nFocus
 * 传入参数：是否是当前选中的button
 * @return
 */

int MenuFileBrowser_RefreshItemEx(struct FileListBlock *dirlist,int Index,int PageIndex,int nIdx,int nFocus)
{
	char DispBuf[DISPBUF_LEN];
	int tmpval;//,sizeMval,sizeKval;
	char sFullPath[MAX_FS_PATH];
	INT32U tmp_interval;
	INT32U CurPage;
	INT32U TotalPage;
	INT32U bmp_x0;
	INT32U bmp_y0;
	INT32U bmp_FdBG_x0;
	INT32U bmp_FdBG_y0;
	INT32U bmp_FirstList_x0;
	INT32U bmp_FirstList_y0;
	INT32U bmp_FirstListOnSub_x0;
	INT32U bmp_FirstListOnSub_y0;

	INT32U tmp_hour;
	INT32U tmp_minute;
	INT32U tmp_second;
	char *filename;
	struct FileAttrib *pFsInfo;
	PlayerInfo fileInfo;

//	DATE_TIME tmp_date;

	GUI_BITMAP *pBmp_BG = &bmBG_DBlue_620_455;
	GUI_BITMAP *pBmp_FdBg = &bmBG_DBlue_316_156;
	GUI_BITMAP *pBmp_Sel = &bmFileList_Flat_Orange_240_30;
	GUI_BITMAP *pBmp_UnSel = &bmFileList_Flat_GBlue_240_30;
	GUI_BITMAP *pBmp_FLstBg = &bmBG_DBlue_241_366;

//System Setting
	INT8U nIdx_Setting;
//	INT8U cnt;

	GUI_BITMAP *pBmp_Setting_Sel = &bmButton_Flat_DBlue_148_30;
	GUI_BITMAP *pBmp_Combox_Sel = &bmButton_Combox_Orange_148_30;
//	GUI_BITMAP *pBmp_Combox_UnSel = &bmButton_Combox_DBlue_148_30;

	INT32U bmp_set_x0;
	INT32U bmp_set_y0;

	int nAppix;
	bmp_FirstList_x0 = AUTO_GET_X0_FROM_WIDTH(
				pBmp_FdBg->XSize+
				BUTTON_H_INTERVAL_COMMON*3+
				pBmp_FLstBg->XSize);
	bmp_FirstList_y0 = AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) +
				TITLE_ALL_HEIGHT + 
				BUTTON_V_INTERVAL_COMMON;
	bmp_x0 = bmp_FirstList_x0;
	bmp_y0 = bmp_FirstList_y0 +BUTTON_V_INTERVAL_COMMON+
		((pBmp_BG->YSize - TITLE_ALL_HEIGHT - BUTTON_V_INTERVAL_COMMON * 2) /
		(BUTTON_CNT_FILEBROWSER_FILELIST+ 1)) * (nIdx);

//File Info
	bmp_FdBG_x0 = bmp_FirstList_x0+pBmp_FLstBg->XSize+BUTTON_H_INTERVAL_COMMON*3;
	bmp_FdBG_y0 = bmp_FirstList_y0;

	tmp_interval = (pBmp_FdBg->YSize - GUI_Font20_SongTi_Unicode.YSize * BUTTON_CNT_FILESUBINFO) /
		(BUTTON_CNT_FILESUBINFO + 1);

	bmp_FirstListOnSub_x0=bmp_FdBG_x0+BUTTON_H_INTERVAL_COMMON;
	bmp_FirstListOnSub_y0 = bmp_FdBG_y0 + tmp_interval;

//System Setting
	nIdx_Setting = nIdx-BUTTON_CNT_FILEBROWSER_FILELIST;
	bmp_set_x0 = bmp_FdBG_x0+
		(pBmp_FdBg->XSize -BUTTON_H_INTERVAL_COMMON -pBmp_Combox_Sel->XSize) *
		(nIdx_Setting%2);

	bmp_set_y0 = bmp_FdBG_y0+pBmp_FdBg->YSize+BUTTON_H_INTERVAL_COMMON+
		((pBmp_BG->YSize - TITLE_ALL_HEIGHT -pBmp_FdBg->YSize - BUTTON_V_INTERVAL_COMMON * 3) / 
		((BUTTON_CNT_FILEBROWSER_SET_LIST)/2+ 1)) *
		(nIdx_Setting/2);

	if(nIdx == 0)
	{
		//绘制文件列表框
		GUI_DrawBitmap(pBmp_FLstBg,
						AUTO_GET_X0_FROM_WIDTH(pBmp_FdBg->XSize+BUTTON_H_INTERVAL_COMMON*3+pBmp_FLstBg->XSize),
						AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_ALL_HEIGHT + BUTTON_V_INTERVAL_COMMON);

        // TODO: add CFCard status here
#if 0
		if(TESTBIT(pUISystem->SysState.DevState,SYS_BIT_CFCard_REMOVE))
		{
//			GUI_DispStringAt(CFCardERR[pUISystem->allData.Language],
//							bmp_set_x0+(pBmp_FdBg->XSize -BUTTON_H_INTERVAL_COMMON -pBmp_Combox_Sel->XSize),
//							bmp_set_y0);
			GUI_DispStringHCenterAt(CFCardERR[pUISystem->allData.Language], 
							(bmp_set_x0+(pBmp_FdBg->XSize -BUTTON_H_INTERVAL_COMMON -pBmp_Combox_Sel->XSize)+
										pBmp_Combox_Sel->XSize / 2),
							(bmp_set_y0 + BUTTON_V_OFFSET_DISPLAY));
		}
		else
		{
//			GUI_DispStringAt(CFCardOK[pUISystem->allData.Language],
//							bmp_set_x0+(pBmp_FdBg->XSize -BUTTON_H_INTERVAL_COMMON -pBmp_Combox_Sel->XSize),
//							bmp_set_y0);
			GUI_DispStringHCenterAt(CFCardOK[pUISystem->allData.Language], 
							(bmp_set_x0+(pBmp_FdBg->XSize -BUTTON_H_INTERVAL_COMMON -pBmp_Combox_Sel->XSize)+
										pBmp_Combox_Sel->XSize / 2),
							(bmp_set_y0 + BUTTON_V_OFFSET_DISPLAY));
		}
#endif
	}

	//画Button
	//File list & File desc
	if(nIdx<BUTTON_CNT_FILEBROWSER_FILELIST)
	{
		//当前选中的文件，但不一定是当前获得焦点的控件
		if(nIdx == pUISystem->FileBrowserSelFile)
		{
			// 1.绘制文件详细信息
			//绘制子背景
			GUI_DrawBitmap(pBmp_FdBg, bmp_FdBG_x0, bmp_FdBG_y0);

			GUI_SetColor(GUI_WHITE);
			//填写子标题信息
			GUI_DispStringAt(FileName[pUISystem->allData.Language], bmp_FirstListOnSub_x0, bmp_FirstListOnSub_y0);
			GUI_DispStringAt(Date[pUISystem->allData.Language], bmp_FirstListOnSub_x0, (bmp_FirstListOnSub_y0 + GUI_Font20_SongTi_Unicode.YSize + tmp_interval));
			GUI_DispStringAt(RecordInfo[pUISystem->allData.Language], bmp_FirstListOnSub_x0, (bmp_FirstListOnSub_y0 + (GUI_Font20_SongTi_Unicode.YSize + tmp_interval) * 2));
			GUI_DispStringAt(Duration[pUISystem->allData.Language], bmp_FirstListOnSub_x0, (bmp_FirstListOnSub_y0 + (GUI_Font20_SongTi_Unicode.YSize + tmp_interval) * 3));
			GUI_DispStringAt(Size[pUISystem->allData.Language], bmp_FirstListOnSub_x0, (bmp_FirstListOnSub_y0 + (GUI_Font20_SongTi_Unicode.YSize + tmp_interval) * 4));
			GUI_DispStringAt(Page[pUISystem->allData.Language], bmp_FirstListOnSub_x0, (bmp_FirstListOnSub_y0 + (GUI_Font20_SongTi_Unicode.YSize + tmp_interval) * 5));

			GUI_DispStringAt(":", (bmp_FdBG_x0 + OFFSET_H_FILL), bmp_FirstListOnSub_y0);
			GUI_DispStringAt(":", (bmp_FdBG_x0 + OFFSET_H_FILL), (bmp_FirstListOnSub_y0 + GUI_Font20_SongTi_Unicode.YSize + tmp_interval));
			GUI_DispStringAt(":", (bmp_FdBG_x0 + OFFSET_H_FILL), (bmp_FirstListOnSub_y0 + (GUI_Font20_SongTi_Unicode.YSize + tmp_interval) * 2));
			GUI_DispStringAt(":", (bmp_FdBG_x0 + OFFSET_H_FILL), (bmp_FirstListOnSub_y0 + (GUI_Font20_SongTi_Unicode.YSize + tmp_interval) * 3));
			GUI_DispStringAt(":", (bmp_FdBG_x0 + OFFSET_H_FILL), (bmp_FirstListOnSub_y0 + (GUI_Font20_SongTi_Unicode.YSize + tmp_interval) * 4));
			GUI_DispStringAt(":", (bmp_FdBG_x0 + OFFSET_H_FILL), (bmp_FirstListOnSub_y0 + (GUI_Font20_SongTi_Unicode.YSize + tmp_interval) * 5));

//Tag
			if(dirlist->nCount<=0)
			{
				GUI_DispStringAt("0/0", (bmp_FdBG_x0 + OFFSET_H_FILL + BUTTON_H_INTERVAL_COMMON * 4), (bmp_FirstListOnSub_y0 + (GUI_Font20_SongTi_Unicode.YSize + tmp_interval) * 5));
				return 1;
			}

			//--填写子背景中的File name
			if ((pFsInfo=GetCurrentFileFsInfo(PageIndex,pUISystem->FileBrowserSelFile,dirlist))==NULL){
				//Never should happen
				goto START_DRAW_TABLE;		
			}
			if ((filename=GetCurrentFileName(PageIndex,pUISystem->FileBrowserSelFile,dirlist))==NULL){
				//Never should happen
				goto START_DRAW_TABLE;
			}
			if(GetCurrentFullPath(PageIndex,pUISystem->FileBrowserSelFile,dirlist,sFullPath) < 0){
				//Never should happen
				goto START_DRAW_TABLE;
			}
			
			GUI_DispStringAt(filename,
				(bmp_FdBG_x0 + OFFSET_H_FILL + BUTTON_H_INTERVAL_COMMON * 2),
				bmp_FirstListOnSub_y0);

			//--填写子背景中的Date
			memcpy(DispBuf,filename,4);
			DispBuf[4]='/';
			memcpy(DispBuf+5,filename+4,2);
			DispBuf[7]='/';
			memcpy(DispBuf+8,filename+6,2);
			DispBuf[10]=' ';
			memcpy(DispBuf+11,filename+8,2);
			DispBuf[13]=':';
			memcpy(DispBuf+14,filename+10,2);
			DispBuf[16]=':';
			memcpy(DispBuf+17,filename+12,2);
			DispBuf[19] = 0;	
			GUI_DispStringAt((const char *)DispBuf, (bmp_FdBG_x0 + OFFSET_H_FILL + BUTTON_H_INTERVAL_COMMON * 2), (bmp_FirstListOnSub_y0 + GUI_Font20_SongTi_Unicode.YSize + tmp_interval));

			//--填写字标题中的Frame rate:fps
			//--填写子标题中的Duration: **:**:**
			//----获得文件信息:帧率，播放总时间
			if (Player_GetInfo(sFullPath,&fileInfo)!=ERR_OK){
				//----显示Frame rate
				snprintf(DispBuf,DISPBUF_LEN, "-- --");
				GUI_DispStringAt((const char *)DispBuf, (bmp_FdBG_x0 + OFFSET_H_FILL + BUTTON_H_INTERVAL_COMMON * 2), (bmp_FirstListOnSub_y0 + (GUI_Font20_SongTi_Unicode.YSize + tmp_interval) * 2));
				
				//----显示Duration
				snprintf(DispBuf,DISPBUF_LEN, "--:--:--");
				GUI_DispStringAt((const char *)DispBuf, (bmp_FdBG_x0 + OFFSET_H_FILL + BUTTON_H_INTERVAL_COMMON * 2), (bmp_FirstListOnSub_y0 + (GUI_Font20_SongTi_Unicode.YSize + tmp_interval) * 3));
				
				goto START_DRAW_TABLE;
			}

			//----显示Frame rate
			nAppix=GetResolutionDesAppix(fileInfo.xdist,fileInfo.ydist);
			snprintf(DispBuf,DISPBUF_LEN, "%s,%03d*%03d%c",(fileInfo.vcodec==ENC_H264)?"H.264":"MPEG4",fileInfo.xdist,fileInfo.ydist<<nAppix,
				((nAppix==1)?'i':' '));
			GUI_DispStringAt((const char *)DispBuf, (bmp_FdBG_x0 + OFFSET_H_FILL + BUTTON_H_INTERVAL_COMMON * 2), (bmp_FirstListOnSub_y0 + (GUI_Font20_SongTi_Unicode.YSize + tmp_interval) * 2));

			//----显示Duration
//			tmp_hour =(fileInfo.playtime>>10)/ 3600;
//			tmp_minute = ((fileInfo.playtime>>10) % 3600) / 60;
//			tmp_second = (fileInfo.playtime>>10) % 60;
            //LOGT("%s fileInfo.playtime=%d\r\n",sFullPath,fileInfo.playtime);
			tmpval = fileInfo.playtime/1000;
			tmp_hour =tmpval/ 3600;
			tmp_minute = (tmpval % 3600)/ 60;
			tmp_second = tmpval % 60;

			snprintf(DispBuf,DISPBUF_LEN, "%02d:%02d:%02d", tmp_hour,tmp_minute,tmp_second);
			GUI_DispStringAt((const char *)DispBuf, (bmp_FdBG_x0 + OFFSET_H_FILL + BUTTON_H_INTERVAL_COMMON * 2), (bmp_FirstListOnSub_y0 + (GUI_Font20_SongTi_Unicode.YSize + tmp_interval) * 3));
			
START_DRAW_TABLE://From get select file Info failed
			
			//--填写子背景中的Size:****MB/****KB
			tmpval=0;
			if((dirlist->attribute[PageIndex+pUISystem->FileBrowserSelFile].size>>20) > 0){
				tmpval=snprintf(DispBuf,DISPBUF_LEN,"%ld MB ", (dirlist->attribute[PageIndex+pUISystem->FileBrowserSelFile].size)>>20);
			}
			if((((dirlist->attribute[PageIndex+pUISystem->FileBrowserSelFile].size>>10)%1024) > 0)||(tmpval==0)){
				snprintf(DispBuf+tmpval,DISPBUF_LEN-tmpval,"%ld KB", ((dirlist->attribute[PageIndex+pUISystem->FileBrowserSelFile].size>>10)%1024));
			}
			GUI_DispStringAt((const char *)DispBuf, (bmp_FdBG_x0 + OFFSET_H_FILL + BUTTON_H_INTERVAL_COMMON * 2), (bmp_FirstListOnSub_y0 + (GUI_Font20_SongTi_Unicode.YSize + tmp_interval) * 4));
			
			//--填写子背景中的Page，如1/23
			CurPage = PageIndex / BUTTON_CNT_FILEBROWSER_FILELIST+1;
			TotalPage = dirlist->nCount / BUTTON_CNT_FILEBROWSER_FILELIST+ (((dirlist->nCount) % BUTTON_CNT_FILEBROWSER_FILELIST> 0) ? 1 : 0);
			snprintf(DispBuf,DISPBUF_LEN,"%d/%d",CurPage,TotalPage);
			GUI_DispStringAt((const char *)DispBuf, (bmp_FdBG_x0 + OFFSET_H_FILL + BUTTON_H_INTERVAL_COMMON * 2), (bmp_FirstListOnSub_y0 + (GUI_Font20_SongTi_Unicode.YSize + tmp_interval) * 5));

			if(nFocus)
			{
				GUI_DrawBitmap(pBmp_Sel, bmp_x0, bmp_y0);
			}else{
				GUI_DrawBitmap(pBmp_UnSel, bmp_x0, bmp_y0);
			}
		}

		if ((nIdx+PageIndex)>=dirlist->nCount){
			return 1;
		}

		//填写文件信息
		//--填写Title
		GUI_DispStringHCenterAt(dirlist->attribute[nIdx+PageIndex].longname, 
			(bmp_x0 + pBmp_Sel->XSize/2),
			(bmp_y0 + BUTTON_V_OFFSET_DISPLAY));
	}
	//File Browser Setting Section
	else
	{
		if(nFocus)
		{
			GUI_DrawBitmap(pBmp_Combox_Sel, bmp_set_x0, bmp_set_y0);
		}
		else
		{
			GUI_DrawBitmap(pBmp_Setting_Sel, bmp_set_x0, bmp_set_y0);
		}
		switch(nIdx)
		{
		case MenuFileBrowser_Button_ID_MountUdisk:
			if(TESTBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_Mount_Flag))
			{
				GUI_DispStringHCenterAt(UMountUDisk[pUISystem->allData.Language], 
										(bmp_set_x0 + pBmp_Combox_Sel->XSize / 2),
										(bmp_set_y0 + BUTTON_V_OFFSET_DISPLAY));
			}
			else
			{
				GUI_DispStringHCenterAt(MountUDisk[pUISystem->allData.Language], 
										(bmp_set_x0 + pBmp_Combox_Sel->XSize / 2),
										(bmp_set_y0 + BUTTON_V_OFFSET_DISPLAY));
			}
			break;
		default:
			ERR("Error button id:nIdx=%d\r\n",nIdx);
			return -1;
#if 0
			GUI_DispStringHCenterAt(FileBrowserSetList[nIdx_Setting],
								(bmp_set_x0 + pBmp_Combox_Sel->XSize / 2),
								(bmp_set_y0 + BUTTON_V_OFFSET_DISPLAY));
			break;
#endif
		}
	}

	return 1;
}

int MenuFileBrowser_RefreshItem(struct UI_Menu_tag *pMenu,int nIdx,int nFocus)
{
	return MenuFileBrowser_RefreshItemEx(&(pUISystem->FileBrowserdirlist),
				pMenu->nButtonIdx,pUISystem->FileBrowserIndex,nIdx,nFocus);
}

int MenuFileBrowser_RefreshPageEx(struct FileListBlock *dirlist,int Index,int PageIndex,int nRefresh)
{
	INT8U i;
	int nFocus;
	
	//绘制信息条(共BUTTON_CNT_SCHEDULE个,索引为0 ~ (BUTTON_CNT_SCHEDULE - 1))
	for(i = 0; i < BUTTON_CNT_FILEBROWSER; i++)
	{
		if(i==Index)
			nFocus=1;
		else
			nFocus=0;
		MenuFileBrowser_RefreshItemEx(dirlist,Index,PageIndex,i,nFocus);
	}

	if (nRefresh){
		RefreshOSDDisplay();
	}
	return 1;
}

/**
 * @brief systemupdatecallback
 * 系统更新回调函数，并切换至“系统更新窗口”
 * @return
 */
int systemupdatecallback()
{
	SystemChangeWindow(MENUSYSTEMUPDATE,0,1,1);   //haiqing
	return 1;
}

/**
 * @brief MenuFileBrowser_ChangeFocus
 * 响应按键消息，并实现文件选中状态的切换
 * @param pMenu
 * 当前活动窗口
 * @param nMsgData
 * 按键消息
 * @return
 */
int MenuFileBrowser_ChangeFocus(UI_Menu_t *pMenu, unsigned short nMsgData)
{
	int ret=0;
	switch(nMsgData)
	{
	case KEY_LEFT:
		ret=MenuFile_PageChange(pMenu, -1);
		break;
	case KEY_RIGHT:
		ret=MenuFile_PageChange(pMenu, 1);
		break;
	case KEY_UP:
		ret=MenuFile_SingleChange(pMenu, -1);
		break;
	case KEY_DOWN:
		ret=MenuFile_SingleChange(pMenu, 1);
		break;
	default:
		ERR("%s:Invalid KeyMsg\r\n",__func__);//should not happen
		break;		
	}
	if(ret)
	{
		pUISystem->FileBrowserSelFile=pMenu->nButtonIdx;
		MenuFileBrowser_RefreshPageEx(&(pUISystem->FileBrowserdirlist),pMenu->nButtonIdx,pUISystem->FileBrowserIndex,1);
	}
	return ret;
}  
// 增加从U盘拷贝到CF卡的操作
/**
 * @brief MenuFileBrowser_MsgHandle
 * 在文件浏览界面，处理按键消息的函数。比如:切换到系统、软件更新窗口。还有文件操作等窗口
 * @param pMenu
 * 当前活动窗口
 * @param nMsgType
 * 消息类型
 * @param nMsgData
 * 消息数据
 * @return
 */
TMsgHandleStat MenuFileBrowser_MsgHandle(UI_Menu_t *pMenu,unsigned short nMsgType,unsigned short nMsgData){
	TMsgHandleStat retval=MsgHandled;
	int nButtonIdx = pMenu->nButtonIdx;
//	int adjustval=0;
	int i;
	char tmp_str[MAX_FS_PATH];

	if (nMsgType!=MSG_KEY_EVENT){
		return MsgNocare;
	}

    // TODO: need merge these two branches
    if(pUISystem->FileBrowserdirlist.nCount<=0)//cf卡里没有.avi文件
	{
		switch(nMsgData)
		{
        case KEY_ESC://退出按键消息
            LOGT("MenuRestore\r\n");
            SystemMenuRestore();
            //SystemChangeWindow(MENUBLANK,1,0,0);
#ifdef EXIT_WHEN_FINISH
			goto Exit_MenuFileBrowser;
#endif
		case KEY_NEXT:
			MenuButtonMaskBitmap[pMenu->nMenuIdx]=0;
			for(i=0;i<BUTTON_CNT_FILEBROWSER_FILELIST;i++)
			{
				MenuButtonMaskBitmap[pMenu->nMenuIdx]|=(1<<i);
			}

			if(nButtonIdx<BUTTON_CNT_FILEBROWSER_FILELIST)
			{
				pMenu->nButtonIdx = BUTTON_CNT_FILEBROWSER_FILELIST;
			}
			else
			{
				//set to invalid button
				pMenu->nButtonIdx = -1;
			}
			MenuFileBrowser_RefreshPageEx(&(pUISystem->FileBrowserdirlist),
											pMenu->nButtonIdx,
											pUISystem->FileBrowserIndex,1);
			break;
		case KEY_ENTER:
			if(nButtonIdx<BUTTON_CNT_FILEBROWSER_FILELIST)
			{
				LOGT("No Video File Exist\r\n");
				MessageBox(MsgBox22[pUISystem->allData.Language],CODEC_MESSAGE_DELAY
					,NULL,NULL,NULL,NULL,NULL,NULL);
				break;
			}
			else
			{
				switch(nButtonIdx)
				{
                case MenuFileBrowser_Button_ID_MountUdisk:
					if(TESTBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_Mount_Flag))
					{
						DoUDiskDetect();
						SystemUmountDev(pUISystem->Vol_Disk[DISK_USB]);
						CLEARBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_Mount_Flag);
						MenuRefresh();
					}
					else
					{
						if(DoUDiskDetect()<0)
						{
							MessageBox(UDiskLinkFail[pUISystem->allData.Language],DEV_MESSAGE_DELAY,NULL,NULL,NULL,NULL,NULL,NULL);
							break;
						}

						if(!TESTBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_REMOVE))
						{
							SETBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_Mount_Flag);
							MenuRefresh();
							break;
						}

						if(SystemMountUDisk()<0)
						{
							MessageBox(UDiskLinkFail[pUISystem->allData.Language],DEV_MESSAGE_DELAY,NULL,NULL,NULL,NULL,NULL,NULL);
						}
						else
						{
							SETBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_Mount_Flag);
							MenuRefresh();
						}
					}
					break;
				default:
					ERR("%s:Invalid nIdx:%d\r\n",__func__,nButtonIdx);
					for(i=BUTTON_CNT_FILEBROWSER_FILELIST;i<BUTTON_CNT_FILEBROWSER;i++)
					{
						MenuButtonMaskBitmap[pMenu->nMenuIdx]|=(1<<i);
					}
					pMenu->nButtonIdx=pUISystem->FileBrowserSelFile;
					break;
				}
			}
			break;
        case KEY_UPDATE://切换至软件更新窗口
            sprintf(tmp_str,"Ver:%s",version_string);
			MessageBox(tmp_str,0,NULL,NULL,NULL,
				NULL,NULL,systemupdatecallback);
            CLEARBIT(pUISystem->SysState.CurState, SYS_BIT_FILEBROSWER);
            SETBIT(pUISystem->SysState.CurState, SYS_BIT_SYSTEMUPDATE);//设置系统更新状态位
			break;
        case KEY_SYSTEM://切换到系统窗口
			SystemChangeWindow(MENUSYSTEMFUNC,0,1,1);
            CLEARBIT(pUISystem->SysState.CurState, SYS_BIT_FILEBROSWER);//清楚文件浏览状态为
            SETBIT(pUISystem->SysState.CurState, SYS_BIT_SYSTEMFUNCTION);//设置系统功能状态位
			break;
		default:
//			LOGT("No Video File Exist\r\n");
//			MessageBox(MsgBox22[pUISystem->allData.Language],CODEC_MESSAGE_DELAY
//				,NULL,NULL,NULL,NULL,NULL,NULL);
			break;
		}
		return retval;
	}
	switch(nMsgData)
	{
	case KEY_LEFT:
	case KEY_RIGHT:
	case KEY_UP:
	case KEY_DOWN:
		if(nButtonIdx<BUTTON_CNT_FILEBROWSER_FILELIST)
		{
			MenuFileBrowser_ChangeFocus(pMenu,nMsgData);
		}
		break;
    case KEY_DELETE://删除视频文件
        if(CheckMountFlags() < 0)
        {
        	retval=MsgNeedDispatch;
            break;
        }
		if(nButtonIdx<BUTTON_CNT_FILEBROWSER_FILELIST)
			DoDeleteFile();
		break;
    case KEY_PLAY://播放视频文件
        if(CheckMountFlags() < 0)
        {
        	retval=MsgNeedDispatch;
            break;
        }
		if(nButtonIdx<BUTTON_CNT_FILEBROWSER_FILELIST)
			DoPlayFile();
		break;
	case KEY_ENTER:
		if(nButtonIdx<BUTTON_CNT_FILEBROWSER_FILELIST)
		{
			SystemChangeWindow(MENUFILEOPERATION,0,1,1);
		}
		else
		{
			switch(nButtonIdx)
			{
			case MenuFileBrowser_Button_ID_MountUdisk:
				if(TESTBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_Mount_Flag))
				{
					DoUDiskDetect();
					SystemUmountDev(pUISystem->Vol_Disk[DISK_USB]);
					CLEARBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_Mount_Flag);
					MenuRefresh();
				}
				else
				{
					if(DoUDiskDetect()<0)
					{
						MessageBox(UDiskLinkFail[pUISystem->allData.Language],DEV_MESSAGE_DELAY,NULL,NULL,NULL,NULL,NULL,NULL);
						break;
					}

					if(!TESTBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_REMOVE))
					{
						SETBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_Mount_Flag);
						MenuRefresh();
						break;
					}

					if(SystemMountUDisk()<0)
					{
						MessageBox(UDiskLinkFail[pUISystem->allData.Language],DEV_MESSAGE_DELAY,NULL,NULL,NULL,NULL,NULL,NULL);
					}
					else
					{
						SETBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_Mount_Flag);
						MenuRefresh();
					}
				}
				break;
			default:
				ERR("%s:Invalid nIdx:%d\r\n",__func__,nButtonIdx);
				for(i=BUTTON_CNT_FILEBROWSER_FILELIST;i<BUTTON_CNT_FILEBROWSER;i++)
				{
					MenuButtonMaskBitmap[pMenu->nMenuIdx]|=(1<<i);
				}
				pMenu->nButtonIdx=pUISystem->FileBrowserSelFile;
				break;
			}
		}
		break;
	case KEY_ESC:
        LOGT("MenuRestore\r\n");
        //SystemChangeWindow(MENUBLANK,1,0,0);
        SystemMenuRestore();
#ifdef EXIT_WHEN_FINISH
		goto Exit_MenuFileBrowser;
#endif
		break;
	case KEY_NEXT:
		MenuButtonMaskBitmap[pMenu->nMenuIdx]=0;
		if(nButtonIdx<BUTTON_CNT_FILEBROWSER_FILELIST)
		{
			for(i=0;i<BUTTON_CNT_FILEBROWSER_FILELIST;i++)
			{
				MenuButtonMaskBitmap[pMenu->nMenuIdx]|=(1<<i);
			}
			pMenu->nButtonIdx=MenuSetUsefulButton(pMenu->nMenuIdx);
		}
		else
		{
			if(pUISystem->FileBrowserdirlist.nCount<BUTTON_CNT_FILEBROWSER_FILELIST)
			{
				for(i=pUISystem->FileBrowserdirlist.nCount;i<BUTTON_CNT_FILEBROWSER;i++)
				{
					MenuButtonMaskBitmap[pMenu->nMenuIdx]|=(1<<i);
				}
			}
			else
			{
				for(i=BUTTON_CNT_FILEBROWSER_FILELIST;i<BUTTON_CNT_FILEBROWSER;i++)
				{
					MenuButtonMaskBitmap[pMenu->nMenuIdx]|=(1<<i);
				}
			}
			pMenu->nButtonIdx=pUISystem->FileBrowserSelFile;
		}
		MenuFileBrowser_RefreshPageEx(&(pUISystem->FileBrowserdirlist),
										pMenu->nButtonIdx,
										pUISystem->FileBrowserIndex,1);
		break;
	case KEY_UPDATE:
        sprintf(tmp_str,"Ver:%s",version_string);
		MessageBox(tmp_str,0,NULL,NULL,NULL,
			NULL,NULL,systemupdatecallback);
        CLEARBIT(pUISystem->SysState.CurState, SYS_BIT_FILEBROSWER);
        SETBIT(pUISystem->SysState.CurState, SYS_BIT_SYSTEMUPDATE);//设置系统更新状态位

		break;
	case KEY_SYSTEM:
		SystemChangeWindow(MENUSYSTEMFUNC,0,1,1);
        CLEARBIT(pUISystem->SysState.CurState, SYS_BIT_FILEBROSWER);
        SETBIT(pUISystem->SysState.CurState, SYS_BIT_SYSTEMFUNCTION);//设置系统更新状态位
		break;
	default:
		retval=MsgNeedDispatch;
		break;
	}

	return retval;

#ifdef EXIT_WHEN_FINISH

Exit_MenuFileBrowser:
	CLEARBIT(pUISystem->SysState.CurState, SYS_BIT_FILEBROSWER);//clear filebrowser status bit

#if (USE_MOUNT_CF_THREAD) || (USE_MOUNT_CF_THREAD_S)
    START_WATCHDOG_TIMER_SIG(WATCHDOG_TO_DELETE_FILE);
    if(CF_Mount_Runing)
    {
        StopMountCF_Thread();
    }
    STOP_WATCHDOG_TIMER_SIG();
#endif
	SystemUmountDev(CF_MOUNT_DIR);
	SetLvdsSwitch(1);

	LOGT("MenuFile exit \r\n");
	request_exit_main_task = 1;
	exit_code = EXIT_CODE_NORMAL;

	return MsgHandled;
#endif

}

//--------------------------------------------------------------------------//
//	Name		:	Pvr_FileBrowser_FileInfo_Button_SetFocus
//	Purpose		:	绘制文件浏览界面
//	Parameter	:	@*dirlist - FileListBlock结构体指针，存储当前路径文件系统信息。
//					@Index - 所选中的文件的索引。0 ~ (BUTTON_CNT_FILEBROWSER_FILELIST - 1)
//					@DiskIndex - 磁盘索引信息0:HDD; 1:SM; 2:MS; 3:SD; 4:CF
//					@PageIndex - 当前页首行在整个列表中的索引0 ~ TotalNum-1。
//--------------------------------------------------------------------------//
void Pvr_FileBrowser_FileInfo_Button_SetFocus(struct FileListBlock *dirlist,int Index,int PageIndex, INT8U DiskIndex)
{
	INT32U bmp_x0;
	INT32U bmp_y0;
	INT32U bmp_SubBG_x0;
	INT32U bmp_SubBG_y0;
	INT32U tmp_interval;
	INT32U bmp_FirstListOnSub_x0;
	INT32U bmp_FirstListOnSub_y0;
	INT32U bmp_FdBG_x0;
	INT32U bmp_FdBG_y0;

    GUI_SetFont(&GUI_Font20_SongTi_Unicode);
    GUI_SetColor(GUI_WHITE);
    GUI_SetBkColor(GUI_BLACK);

	GUI_BITMAP *pBmp_BG = &bmBG_DBlue_620_455;
	GUI_BITMAP *pBmp_FdBg = &bmBG_DBlue_316_156;
	GUI_BITMAP *pBmp_FLstBg = &bmBG_DBlue_241_366;
//Logo
	INT32U logo207_x;
	INT32U logo207_y;
	GUI_BITMAP *pBmp_logo_207=&bm207;
	GUI_BITMAP *pBmp_Combox_UnSel = &bmButton_Combox_DBlue_148_30;

	bmp_FdBG_x0 = AUTO_GET_X0_FROM_WIDTH(
				pBmp_FdBg->XSize+
				BUTTON_H_INTERVAL_COMMON*3+
				pBmp_FLstBg->XSize)+
				pBmp_FLstBg->XSize+BUTTON_H_INTERVAL_COMMON*3;
	bmp_FdBG_y0 = AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) +
				TITLE_ALL_HEIGHT + 
				BUTTON_V_INTERVAL_COMMON;

	tmp_interval = (pBmp_FdBg->YSize - GUI_Font20_SongTi_Unicode.YSize * BUTTON_CNT_FILESUBINFO) /
		(BUTTON_CNT_FILESUBINFO + 1);

	bmp_FirstListOnSub_x0=bmp_FdBG_x0+BUTTON_H_INTERVAL_COMMON;
	bmp_FirstListOnSub_y0 = bmp_FdBG_y0 + tmp_interval;

//Logo
	logo207_x=bmp_FdBG_x0+40;
	logo207_y=bmp_FdBG_y0+pBmp_FdBg->YSize+BUTTON_H_INTERVAL_COMMON*3+pBmp_Combox_UnSel->YSize;

	//变量保护
	Index = (Index > (BUTTON_CNT_FILEBROWSER - 1)) ? 0 : Index;
	DiskIndex = (DiskIndex > (SUPPORT_DISK_CNT- 1)) ? 0 : DiskIndex;
	
	//绘制背景
	GUI_DrawBitmap(pBmp_BG, 
							AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize), 
							AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize));

	//填写Title:FileBrowser - 
	GUI_SetTextMode(GUI_TM_TRANS);
	GUI_DispStringHCenterAt(FileBrowserStorageList[DiskIndex], 
							(LCD_WIDTH / 2), 
							(AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_BELOW_HEIGHT));

	//绘制下方提示信息
	bmp_x0 = AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize);
	bmp_y0 = AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + pBmp_BG->YSize - GUI_Font20_SongTi_Unicode.YSize - BUTTON_V_INTERVAL_COMMON;

    //zqh 2015年5月25日 add，由于英文的提示信息太长(超过字符边框)，所以将HelpTip11的字体变小。
    if(pUISystem->allData.Language == ENGLISH)
        GUI_SetFont(&GUI_Font8x16);

    GUI_DispStringAt(HelpTip11[pUISystem->allData.Language], bmp_x0, bmp_y0);

    if(pUISystem->allData.Language == ENGLISH)
        GUI_SetFont(&GUI_Font20_SongTi_Unicode);

    GUI_DrawBitmap(pBmp_logo_207, logo207_x, logo207_y);

	//绘制信息条
	MenuFileBrowser_RefreshPageEx(dirlist,Index,PageIndex,1);

	//设定刷新区域
	SetRefreshAreaPos(OSDPOS_BASEMENU, 
					AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize), 
					AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize), 
					pBmp_BG->XSize, 
					pBmp_BG->YSize, 
					ALPHA_COMMON,
					TRUE);
}

/**
 * @brief MenuFileBrowser_Refresh
 * 文件浏览界面绘制函数，具体实现是调用Pvr_FileBrowser_FileInfo_Button_SetFocus()这个函数
 * @param pMenu
 * 当前活动窗口
 * @return
 */

int MenuFileBrowser_Refresh(UI_Menu_t *pMenu){
	Pvr_FileBrowser_FileInfo_Button_SetFocus(&(pUISystem->FileBrowserdirlist),
		pMenu->nButtonIdx,pUISystem->FileBrowserIndex,pUISystem->nFileSelectDisk);
	return 1;
}

/*File Operation: List File Operation
 *--------------- MenuFile.c
 */
/**
 * @brief MenuFileOper_Init
 * 设置初始化选中的button，在切换到文件操作的时候调用此函数
 * @param pMenu
 * 当前活动窗口
 * @return
 */
int MenuFileOper_Init(UI_Menu_t *pMenu){

    GUI_SetBkColor(GUI_BLACK);
	//MenuButtonMaskBitmap[pMenu->nMenuIdx]|=(1<<3);
	MenuButtonMaskBitmap[pMenu->nMenuIdx]|=(1<<4);
	
	pMenu->nButtonIdx=0;
	return 1;
}

/*
 * @brief MenuFileOper_MsgHandle
 * 对文件的删除、拷贝、回放等功能
 * @param pMenu
 * 当前活动窗口
 * @param nMsgType
 * 消息类型
 * @param nMsgData
 * 消息数据
 * @return
 */

TMsgHandleStat MenuFileOper_MsgHandle(UI_Menu_t *pMenu,unsigned short nMsgType,unsigned short nMsgData){
	TMsgHandleStat retval=MsgHandled;
	if (nMsgType!=MSG_KEY_EVENT){
		return MsgNocare;
	}

	switch(nMsgData){
		case KEY_UP:
			MenuChangeFocus(-1);
			break;
		case KEY_DOWN:
			MenuChangeFocus(1);
			break;
		case KEY_ENTER:
		case KEY_RIGHT:
            if(CheckMountFlags() < 0)
            {
            	retval=MsgNeedDispatch;
                break;
            }

			switch (pMenu->nButtonIdx){
				case 0://Playback
					DoPlayFile();
					break;
				case 1://delete
					DoDeleteFile();
					break;
                case 2://copy切换窗口
					SystemChangeWindow(MENUFILECOPYTO,1,1,1);
					break;
                case 3://delete all files
					DoDeleteAllFile();
					break;
				default://should not happen
					pMenu->nButtonIdx=(MenuButtonCnt[pUISystem->nMenuIdx]-1);
					MenuChangeFocus(1);
					break;
			}
			break;
//		case KEY_MENU:
//			ReleaseList(&(pUISystem->FileBrowserdirlist));//Do Release BrowerMenu Resourse
//			SystemChangeWindow(MENUBLANK, 1, 1,0);
//			break;
		case KEY_ESC:
            LOGT("MenuRestore\r\n");
			SystemMenuRestore();
			break;
		default:
			retval=MsgNeedDispatch;
			break;
	}
	return retval;
}

/**
 * @brief MenuFileOper_RefreshItem
 * 绘制文件操作窗口界面
 * @param pMenu
 * 当前活动窗口
 * @param nIdx
 *
 * @param nFocus
 * @return
 */
int MenuFileOper_RefreshItem(struct UI_Menu_tag *pMenu,int nIdx,int nFocus)
{
	INT32U bmp_x0;
	INT32U bmp_y0;
	INT32U bmp_FirstList_x0;
	INT32U bmp_FirstList_y0;
	INT32U tmp_interval;
	GUI_BITMAP *pBmp_BG;
	GUI_BITMAP *pBmp_Sel;
	GUI_BITMAP *pBmp_UnSel;
	GUI_BITMAP *pBmp_Invalid;

    GUI_SetFont(&GUI_Font20_SongTi_Unicode);
    GUI_SetColor(GUI_WHITE);
    GUI_SetBkColor(GUI_BLACK);
	
	//填写当前所操作文件的文件名
	//绘制Button(共BUTTON_CNT_FILEOPERATION个,索引为0 ~ (BUTTON_CNT_FILEOPERATION - 1))
	pBmp_BG = (&bmMainMenuBG_360_320);
	pBmp_UnSel = (&bmButton_Blue_785C4C_200_29_UP);
	pBmp_Sel = (&bmButton_Yellow_2060F0_200_29_UP);
	pBmp_Invalid = (&bmButton_Invalid_200_29);

	tmp_interval = (pBmp_BG->YSize - TITLE_ALL_HEIGHT - pBmp_Sel->YSize * (BUTTON_CNT_FILEOPERATION + 1)) / (BUTTON_CNT_FILEOPERATION + 1 + 1);
	bmp_FirstList_x0 = AUTO_GET_X0_FROM_WIDTH(pBmp_Sel->XSize);
	bmp_FirstList_y0 = AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_ALL_HEIGHT + tmp_interval;
	
	bmp_x0 = bmp_FirstList_x0;
	bmp_y0 = bmp_FirstList_y0 + (pBmp_Sel->YSize + tmp_interval) * (nIdx + 1);

		//画Button
	if((1<<nIdx)&MenuButtonMaskBitmap[pUISystem->nMenuIdx]){	//绘制需要跳过的条目
			GUI_DrawBitmap(pBmp_Invalid, bmp_x0, bmp_y0);
		}else{
		if(nFocus){	//绘制高亮选中条目
			GUI_DrawBitmap(pBmp_Sel, bmp_x0, bmp_y0);
			}else{			//绘制非高亮条目
			GUI_DrawBitmap(pBmp_UnSel, bmp_x0, bmp_y0);
		}
		}
	if((1<<nIdx)&MenuButtonMaskBitmap[pUISystem->nMenuIdx])
	{	//绘制需要跳过的条目
		GUI_SetColor(GUI_GRAY);

		//写Content
		GUI_DispStringHCenterAt(FileOperationList[nIdx], 
								(bmp_x0 + pBmp_UnSel->XSize / 2),
								(bmp_y0 + BUTTON_V_OFFSET_DISPLAY));
		GUI_SetColor(GUI_WHITE);

	}
	else
	{
		//写Content
		GUI_DispStringHCenterAt(FileOperationList[nIdx], 
								(bmp_x0 + pBmp_UnSel->XSize / 2),
								(bmp_y0 + BUTTON_V_OFFSET_DISPLAY));
	}

    // NOTE: Do not SetRefreshArea should notify
	return 1;
}

//--------------------------------------------------------------------------//
//	Name		:	Pvr_FileBrowser_Operation_Button_SetFocus
//	Purpose		:	绘制文件操作选择界面
//	Parameter	:	@Index - 所选中的文件的索引。0 ~ (BUTTON_CNT_FILEOPERATION - 1)
//--------------------------------------------------------------------------//
void Pvr_FileBrowser_Operation_Button_SetFocus(INT8U Index)
{
	INT8U i;
	INT32U bmp_FirstList_x0;
	INT32U bmp_FirstList_y0;
	INT32U tmp_interval;
	GUI_BITMAP *pBmp_BG;
	GUI_BITMAP *pBmp_Sel;
	GUI_BITMAP *pBmp_UnSel;
	char *filename;

    GUI_SetFont(&GUI_Font20_SongTi_Unicode);
    GUI_SetColor(GUI_WHITE);
    GUI_SetBkColor(GUI_BLACK);
	
	//变量保护
	Index = (Index > (BUTTON_CNT_FILEOPERATION - 1)) ? 0 : Index;

	//绘制背景
	pBmp_BG = (&bmMainMenuBG_360_320);
	GUI_DrawBitmap(pBmp_BG, 
							AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize), 
							AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize));

	//填写Title:File operation
	GUI_SetTextMode(GUI_TM_TRANS);
	GUI_DispStringHCenterAt(FileOperation[pUISystem->allData.Language], 
							(LCD_WIDTH / 2), 
							(AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_BELOW_HEIGHT));

	//获得当前所操作文件的文件名
	filename=GetCurrentFileName(pUISystem->FileBrowserIndex,pUISystem->FileBrowserSelFile,&pUISystem->FileBrowserdirlist);

	//填写当前所操作文件的文件名
	//绘制Button(共BUTTON_CNT_FILEOPERATION个,索引为0 ~ (BUTTON_CNT_FILEOPERATION - 1))
	pBmp_UnSel = (&bmButton_Blue_785C4C_200_29_UP);
	pBmp_Sel = (&bmButton_Yellow_2060F0_200_29_UP);

	tmp_interval = (pBmp_BG->YSize - TITLE_ALL_HEIGHT - pBmp_Sel->YSize * (BUTTON_CNT_FILEOPERATION + 1)) / (BUTTON_CNT_FILEOPERATION + 1 + 1);
	bmp_FirstList_x0 = AUTO_GET_X0_FROM_WIDTH(pBmp_Sel->XSize);
	bmp_FirstList_y0 = AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_ALL_HEIGHT + tmp_interval;
	
 	GUI_DispStringHCenterAt((const char *)filename,
							(bmp_FirstList_x0 + pBmp_UnSel->XSize / 2),
							(bmp_FirstList_y0 + BUTTON_V_OFFSET_DISPLAY));
	
	for(i = 0; i < BUTTON_CNT_FILEOPERATION; i++)
	{
		MenuRefreshItemOnly(i);
	}

	//设定刷新区域
	SetRefreshAreaPos(OSDPOS_BASEMENU, 
					AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize), 
					AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize), 
					pBmp_BG->XSize, 
					pBmp_BG->YSize, 
					ALPHA_COMMON,
					TRUE);
}

/**
 * @brief MenuFileOper_Refresh
 * 文件操作界面绘制函数。比如：显示“播放文件、删除文件等信息”
 * @param pMenu
 * 当前活动窗口
 * @return
 */

int MenuFileOper_Refresh(UI_Menu_t *pMenu){
	Pvr_FileBrowser_Operation_Button_SetFocus(pMenu->nButtonIdx);
	return 1;
}

//JunR item7
/*File Copy TO: Copy to disk
 *--------------- MenuFile.c
 */
/**
 * @brief DoFileCopyTO
 * 将cf卡文件拷贝到u盘
 * @return
 */
int DoFileCopyTO(void)
{
	char *destPath;
	char sorPath[MAX_FS_PATH];
	int ret;
	struct FileAttrib *pFsInfo;

    ret = DoUDiskDetect();//检测优盘是否挂载
	if(TESTBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_REMOVE))
	{
		ERR("UDisk disconnect!\n");
		CLEARBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_Mount_Flag);
		MessageBox(UDiskRemoved[pUISystem->allData.Language],DEV_MESSAGE_DELAY,
			NULL,NULL,NULL,NULL,NULL,NULL);
		return -1;
	}

	if(!TESTBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_Mount_Flag))
	{
		ERR("Need Mount UDisk first!\n");
		MessageBox(HelpMountUDisk[pUISystem->allData.Language],DEV_MESSAGE_DELAY,
			NULL,NULL,NULL,NULL,NULL,NULL);
		return -1;
	}


	if(GetCurrentFullPath(pUISystem->FileBrowserIndex,
		pUISystem->FileBrowserSelFile,&pUISystem->FileBrowserdirlist,sorPath)<0){
		return -1;
	}
    destPath=GetCurrentFileName(pUISystem->FileBrowserIndex,pUISystem->FileBrowserSelFile,&pUISystem->FileBrowserdirlist);
	strncpy(pUISystem->sFileCopyFrom,sorPath,MAX_FS_PATH);
	strncpy(pUISystem->sFileCopyTo,pUISystem->Vol_Disk[DISK_USB],MAX_FS_PATH);
	strcat(pUISystem->sFileCopyTo,destPath);
	LOGT("%s:sorPath = %s\r\n",__func__,pUISystem->sFileCopyFrom);
	LOGT("%s:destpath = %s\r\n",__func__,pUISystem->sFileCopyTo);
	//printf("do file copy to ok!\n");
	return 1;
}

int MenuCopyTo_Init(UI_Menu_t *pMenu)
{
	return 1;
}

/**
 * @brief MenuCopyTo_MsgHandle
 * 文件拷贝到U盘的处理函数
 * @param pMenu
 * 当前活动窗口
 * @param nMsgType
 * 消息类型
 * @param nMsgData
 * 消息数据
 * @return
 */

TMsgHandleStat MenuCopyTo_MsgHandle(UI_Menu_t *pMenu,unsigned short nMsgType,unsigned short nMsgData)
{
	char destPath[MAX_FS_PATH];
	char sorPath[MAX_FS_PATH];
	char *filename;
	int ret;
	struct FileAttrib *pFsInfo;

	TMsgHandleStat retval=MsgHandled;
	const char *pStr;
	if (nMsgType!=MSG_KEY_EVENT){
		return MsgNocare;
	}

	switch(nMsgData){
		case KEY_ENTER:
            if(CheckMountFlags() < 0)
            {
            	retval=MsgNeedDispatch;
                break;
            }

			ret = DoUDiskDetect();
			if( (ret<0)
				||TESTBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_REMOVE)
				||(!TESTBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_Mount_Flag))
			  )
			{
				ERR("Need Mount UDisk first!\n");
				CLEARBIT(pUISystem->SysState.DevState,SYS_BIT_UDisk_Mount_Flag);
				MessageBox(HelpMountUDisk[pUISystem->allData.Language],DEV_MESSAGE_DELAY,
					NULL,NULL,NULL,NULL,NULL,NULL);
				return -1;
			}

			if(GetCurrentFullPath(pUISystem->FileBrowserIndex,pUISystem->FileBrowserSelFile,&pUISystem->FileBrowserdirlist,sorPath)<0)
			{
				return -1;
			}
		    filename=GetCurrentFileName(pUISystem->FileBrowserIndex,pUISystem->FileBrowserSelFile,&pUISystem->FileBrowserdirlist);
			snprintf(destPath,MAX_FS_PATH,"%s%s",pUISystem->Vol_Disk[DISK_USB],filename);
			pUISystem->nFileCopyToStage = MESSAGESTAGE_COPY_TO_UDISK;
			StartCopyFile(sorPath,destPath);

			break;
		case KEY_ESC:
            LOGT("MenuRestore\r\n");
			SystemMenuRestore();
			break;
		default:
			retval=MsgNeedDispatch;
			break;
	}
	return retval;
}
//hai qing add
int MenuCopyTo_RefreshItem(struct UI_Menu_tag *pMenu,int nIdx,int nFocus)
{
	return 0;
}

/**
 * @brief MenuCopyTo_Refresh
 * 当复制文件时，调用此函数，绘制“文件复制”消息弹窗
 * @param pMenu
 * 当前活动窗口
 * @return
 */


int MenuCopyTo_Refresh(UI_Menu_t *pMenu)
{
	GUI_BITMAP *pBmp_BG = (&bmMessageBoxBG_360_240);
	GUI_BITMAP *pBmp_SubBG = (&bmMessageRect_310_130);
	GUI_RECT Rect;
	const char *MessageDisp=NULL;
	const char *MessageHelp=NULL;

	GUI_SetFont(&GUI_Font20_SongTi_Unicode);
	GUI_SetColor(GUI_WHITE);
	GUI_SetBkColor(GUI_BLACK);
	//--------------------------------------------------------------------------//
	//	Purpose	:	绘制背景
	//--------------------------------------------------------------------------//
	GUI_DrawBitmap(pBmp_BG, 
				AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize), 
				AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize));
	GUI_DrawBitmap(pBmp_SubBG, 
				AUTO_GET_X0_FROM_WIDTH(pBmp_SubBG->XSize), 
				(AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_ALL_HEIGHT + BUTTON_V_INTERVAL_COMMON));

	//--------------------------------------------------------------------------//
	//	Purpose	:	填写Title
	//--------------------------------------------------------------------------//
	GUI_SetTextMode(GUI_TM_TRANS);
	GUI_DispStringHCenterAt(Message[pUISystem->allData.Language], 
							(LCD_WIDTH / 2), 
							(AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_BELOW_HEIGHT));

	//--------------------------------------------------------------------------//
	//	Purpose	:	填写显示内容
	//	Purpose	:	绘制下方提示信息
	//--------------------------------------------------------------------------//

	Rect.x0 = AUTO_GET_X0_FROM_WIDTH(pBmp_SubBG->XSize);
	Rect.y0 = AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize);
	Rect.x1 = AUTO_GET_X0_FROM_WIDTH(pBmp_SubBG->XSize) + pBmp_SubBG->XSize;
	Rect.y1 = AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + pBmp_BG->YSize;
	
    MessageDisp=FileCopyHelpInfo[pUISystem->allData.Language];
	//zqh 2015年3月27日modify，将HelpTip1改为HelpTip8
	MessageHelp=HelpTip8[pUISystem->allData.Language];
	
    //显示提示信息，如“复制文件，是否继续？”
	GUI_DispStringInRect(MessageDisp, &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);

    //显示帮助文字，如界面上的“是：按确定键；否：按取消键”
    //zqh 2015年5月26日 add，当系统语言为英文时，将字体变小。
    if(pUISystem->allData.Language == ENGLISH)
        GUI_SetFont(&GUI_Font8x16);

	GUI_DispStringAt(MessageHelp, 
					AUTO_GET_X0_FROM_WIDTH(pBmp_SubBG->XSize), 
					(AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + pBmp_BG->YSize - GUI_Font20_SongTi_Unicode.YSize - PROCESS_V_OFFSET_BG_BOTTON));
	//temp--设定刷新区域
	SetRefreshAreaPos(OSDPOS_BASEMENU, 
					AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize), 
					AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize), 
					pBmp_BG->XSize, 
					pBmp_BG->YSize, 
					ALPHA_COMMON,
					TRUE);
	return 1;
}


