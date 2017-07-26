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
 *					�������/ǰ��ҳ--(-1)
 *					�����Ҽ�/��ҳ--(+1)
 */
int MenuFile_PageChange(UI_Menu_t *pMenu, int adjustval)
{
    if(pUISystem->FileBrowserdirlist.nCount <= FILES_PER_BLOCK)//�ж�cf�����Ƿ����ʮ���ļ�
		return 0;

	if(adjustval>0)	{	//��ҳ
		if((pUISystem->FileBrowserIndex + FILES_PER_BLOCK) < pUISystem->FileBrowserdirlist.nCount) {
			pUISystem->FileBrowserIndex += FILES_PER_BLOCK;
		} else {
			pUISystem->FileBrowserIndex = 0;
		}
    } else if(adjustval<0)	{  //ǰ��ҳ
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
 *					�����ϼ�/��һ��--(-1)
 *					�����¼�/��һ��--(+1)
 */
int MenuFile_SingleChange(UI_Menu_t *pMenu, int adjustval)
{
	int index;		//�ܵ�Index(0---TotalNum-1)


    if(pUISystem->FileBrowserdirlist.nCount <= 1) {//�ж�cf�����Ƿ�ֻ��һ���ļ�
		return 0;
	}
	
	index = pUISystem->FileBrowserIndex + pMenu->nButtonIdx;
	
	if(adjustval>0)	{	//��һ��
		if((index + 1) < pUISystem->FileBrowserdirlist.nCount) {
			index++;
		} else {
			index = 0;
		}
	} else if(adjustval<0)	{			//��һ��
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
 * ��õ�ǰѡ�е��ļ���ϵͳ��Ϣ
 * @param nPageOffset
 * ҳ��ƫ����
 * @param nOffset
 * �ڵ�ǰҳ��ƫ����
 * @param pDir
 * Ŀ¼�ṹ��
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
 * ��õ�ǰѡ���ļ�������
 * @param nPageOffset
 * ҳƫ�ƣ�������25����Ƶ�ļ���ÿһҳֻ��ʾ10���ļ�����ҳƫ�Ƶ����ֵΪ3
 * @param nOffset
 * ҳƫ�Ƶ��ڲ�ƫ������ֵΪ0~FILES_PER_BLOCK-1
 * @param pDir
 * Ŀ¼�ṹ��
 * @return
 * ���ؿ����ʾδ��ȡ���ļ�����
 * �����ȡ�ļ����ɹ�
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
 * ��ȡ��ǰ�ļ���ȫ·������/media/cf/20100622034355.avi
 * @param nPageOffset
 * ҳƫ����
 * @param nOffset
 * ҳ�����ƫ��������Χ0~FILES_PER_BLOCK-1
 * @param pDir
 * Ŀ¼�ṹ��
 * @param sFullPath
 * ����õ�ȫ·���������ڴ˱�����
 * @return
 * 1���ɹ�
 * -1��ʧ��
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
 * ɾ�������ļ�
 * @param filepath
 * ɾ���ļ���ȫ·��
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
 * ɾ�������ļ��Ļص�����
 * @param pUser
 * Ҫɾ�����ļ�,����/media/cf/20100622034355.avi
 * @return
 */
int Delete_Callback(void *pUser){
	int retval;
	Pvr_Waiting(TRUE);

    retval = DeleteSingleFile(pUser);//����ɾ�������ļ��ĺ���

	return retval;
}

/**
 * @brief DeleteAllFile_Callback
 * ɾ��cf���ϵ�����.avi�ļ��Ļص������������int DoDeleteAllFile(void)�����е���
 * @return
 */
int DeleteAllFile_Callback(void)  //haiqing
{
	struct FileAttrib *pFsInfo=NULL;
	char sFullPath[MAX_FS_PATH];
	int cnt;
	int i;

    if(pUISystem->FileBrowserdirlist.nCount<=0)//���cf����û��.avi�ļ�������ֱ�ӷ���
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

    cnt = pUISystem->FileBrowserdirlist.nCount;//��ȡ��ǰcf����.avi�ļ��ĸ���

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
    // ʹ���̴߳������ʱ,���ڴ˴����м��ز���
	SystemMountCFCard();
#endif

#endif

	return 0;
}

/**
 * @brief DeleteMessage_Deinit
 * ɾ���ļ��󣬶Դ��ڽ��з���ʼ��
 * @param nOKFail
 * ����
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
 * ������Ƶ�ļ�
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
 * ɾ���ļ�
 * @return
 */

int DoDeleteFile(void)
{
	int ret;
	char *filename = NULL;
    filename=GetCurrentFileName(pUISystem->FileBrowserIndex,pUISystem->FileBrowserSelFile,&pUISystem->FileBrowserdirlist);//��ȡ��ǰѡ�е��ļ���
	if(filename == NULL)
	{
		LOGT("can't get current file name\r\n");
		return -1;
	}

    //LOGT("delete file %s\r\n",filename);
    if(strncmp(filename,RESERVED_HEADER_STR,RESERVED_HEADER_LEN)==0)//���ɾ�����ļ��Ƿ�Ϊ�ض��ļ�
	{
		return MessageBox(SpecificFile[pUISystem->allData.Language],0,
							NULL,NULL,NULL,NULL,NULL,NULL);
	}
	else
	{
        snprintf(pUISystem->sRecFilename,MAX_FS_PATH,"%s%s",//�����ļ���ȫ·������/media/cf/20100622034355.avi
					pUISystem->Vol_Disk[pUISystem->nFileSelectDisk],filename);
		return MessageBox(MsgBox30[pUISystem->allData.Language],CODEC_MESSAGE_DELAY,Delete_Callback,
			NULL,pUISystem->sRecFilename,MsgBox23[pUISystem->allData.Language],
			MsgBox24[pUISystem->allData.Language],DeleteMessage_Deinit);
	}
}

//haiqing 
/**
 * @brief DoDeleteAllFile
 * ɾ��cf����ȫ��.avi�ļ�ʱ��������Ϣ����
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
 * �ļ��������֮ǰ��һЩ��ʼ�����������磺���cf�����ж��ٸ�avi�ļ������ó�ʼ��ѡ��button
 * @param pMenu
 * ��ǰ�Ļ����
 * @return
 */

int MenuFileBrowser_Init(UI_Menu_t *pMenu)
{
    int i;

    FUNC_ENTER;

    // Set FileBrowserdirlist directory and contain file type
    //��ȡcf����.avi�ļ��ĸ���
    strcpy(pUISystem->FileBrowserdirlist.directory,pUISystem->Vol_Disk[pUISystem->nFileSelectDisk]);
    pUISystem->FileBrowserdirlist.contain = CONTAIN_FILE;
    pUISystem->FileBrowserdirlist.nCount = 0;

    //Start Scan
    if (!Scandir(&(pUISystem->FileBrowserdirlist), VIDEO_FILES, pUISystem->allData.CurSortMethod))//���cf�����ж��ٸ�.avi�ļ�������ָ���ķ�������
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


    MenuButtonMaskBitmap[pMenu->nMenuIdx]=0;//Ĭ��ʹ������button

    if(pUISystem->FileBrowserdirlist.nCount==0)//�ж�cf����avi�ļ������Ƿ�Ϊ0
    {
        for (i=0;i<BUTTON_CNT_FILEBROWSER;i++)
        {
            MenuButtonMaskBitmap[pMenu->nMenuIdx]|=(1<<i);//��10��button���ε�
        }
        pUISystem->FileBrowserIndex = 0;
        pUISystem->FileBrowserSelFile = 0;
        pMenu->nButtonIdx=-1;	//set to invalid button

        return 1;
    }

    //����cf�����ļ���������10����������6��avi�ļ��������ε���6-9����0��ʼ�����ļ���button�����μ����ڽ����ϻ������ǵ�button��
    if(pUISystem->FileBrowserdirlist.nCount<BUTTON_CNT_FILEBROWSER_FILELIST)
    {
        for (i=pUISystem->FileBrowserdirlist.nCount;i<BUTTON_CNT_FILEBROWSER;i++)
        {
            MenuButtonMaskBitmap[pMenu->nMenuIdx]|=(1<<i);
        }
    }
    else
    {//cf������ڵ���10���ļ��������10��button
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

    //ɾ��һ���ļ����л����ļ��������ʱ��ʹѡ�е�button����
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
 * �ͷ�pUISystem->FileBrowserdirlist�ڴ�
 * @param pMenu
 * ��ǰ�����
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
 * ���ļ��������������ĺ���
 * @param dirlist
 * ����������ļ��б��
 * @param Index
 * ���������ҳ��ƫ��
 * @param PageIndex
 * ���������ҳ��ֵ�����磺��25���ļ�����˱���ȡֵ��ΧΪ0-2
 * @param nIdx
 * ���������buttonֵ
 * @param nFocus
 * ����������Ƿ��ǵ�ǰѡ�е�button
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
		//�����ļ��б��
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

	//��Button
	//File list & File desc
	if(nIdx<BUTTON_CNT_FILEBROWSER_FILELIST)
	{
		//��ǰѡ�е��ļ�������һ���ǵ�ǰ��ý���Ŀؼ�
		if(nIdx == pUISystem->FileBrowserSelFile)
		{
			// 1.�����ļ���ϸ��Ϣ
			//�����ӱ���
			GUI_DrawBitmap(pBmp_FdBg, bmp_FdBG_x0, bmp_FdBG_y0);

			GUI_SetColor(GUI_WHITE);
			//��д�ӱ�����Ϣ
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

			//--��д�ӱ����е�File name
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

			//--��д�ӱ����е�Date
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

			//--��д�ֱ����е�Frame rate:fps
			//--��д�ӱ����е�Duration: **:**:**
			//----����ļ���Ϣ:֡�ʣ�������ʱ��
			if (Player_GetInfo(sFullPath,&fileInfo)!=ERR_OK){
				//----��ʾFrame rate
				snprintf(DispBuf,DISPBUF_LEN, "-- --");
				GUI_DispStringAt((const char *)DispBuf, (bmp_FdBG_x0 + OFFSET_H_FILL + BUTTON_H_INTERVAL_COMMON * 2), (bmp_FirstListOnSub_y0 + (GUI_Font20_SongTi_Unicode.YSize + tmp_interval) * 2));
				
				//----��ʾDuration
				snprintf(DispBuf,DISPBUF_LEN, "--:--:--");
				GUI_DispStringAt((const char *)DispBuf, (bmp_FdBG_x0 + OFFSET_H_FILL + BUTTON_H_INTERVAL_COMMON * 2), (bmp_FirstListOnSub_y0 + (GUI_Font20_SongTi_Unicode.YSize + tmp_interval) * 3));
				
				goto START_DRAW_TABLE;
			}

			//----��ʾFrame rate
			nAppix=GetResolutionDesAppix(fileInfo.xdist,fileInfo.ydist);
			snprintf(DispBuf,DISPBUF_LEN, "%s,%03d*%03d%c",(fileInfo.vcodec==ENC_H264)?"H.264":"MPEG4",fileInfo.xdist,fileInfo.ydist<<nAppix,
				((nAppix==1)?'i':' '));
			GUI_DispStringAt((const char *)DispBuf, (bmp_FdBG_x0 + OFFSET_H_FILL + BUTTON_H_INTERVAL_COMMON * 2), (bmp_FirstListOnSub_y0 + (GUI_Font20_SongTi_Unicode.YSize + tmp_interval) * 2));

			//----��ʾDuration
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
			
			//--��д�ӱ����е�Size:****MB/****KB
			tmpval=0;
			if((dirlist->attribute[PageIndex+pUISystem->FileBrowserSelFile].size>>20) > 0){
				tmpval=snprintf(DispBuf,DISPBUF_LEN,"%ld MB ", (dirlist->attribute[PageIndex+pUISystem->FileBrowserSelFile].size)>>20);
			}
			if((((dirlist->attribute[PageIndex+pUISystem->FileBrowserSelFile].size>>10)%1024) > 0)||(tmpval==0)){
				snprintf(DispBuf+tmpval,DISPBUF_LEN-tmpval,"%ld KB", ((dirlist->attribute[PageIndex+pUISystem->FileBrowserSelFile].size>>10)%1024));
			}
			GUI_DispStringAt((const char *)DispBuf, (bmp_FdBG_x0 + OFFSET_H_FILL + BUTTON_H_INTERVAL_COMMON * 2), (bmp_FirstListOnSub_y0 + (GUI_Font20_SongTi_Unicode.YSize + tmp_interval) * 4));
			
			//--��д�ӱ����е�Page����1/23
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

		//��д�ļ���Ϣ
		//--��дTitle
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
	
	//������Ϣ��(��BUTTON_CNT_SCHEDULE��,����Ϊ0 ~ (BUTTON_CNT_SCHEDULE - 1))
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
 * ϵͳ���»ص����������л�����ϵͳ���´��ڡ�
 * @return
 */
int systemupdatecallback()
{
	SystemChangeWindow(MENUSYSTEMUPDATE,0,1,1);   //haiqing
	return 1;
}

/**
 * @brief MenuFileBrowser_ChangeFocus
 * ��Ӧ������Ϣ����ʵ���ļ�ѡ��״̬���л�
 * @param pMenu
 * ��ǰ�����
 * @param nMsgData
 * ������Ϣ
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
// ���Ӵ�U�̿�����CF���Ĳ���
/**
 * @brief MenuFileBrowser_MsgHandle
 * ���ļ�������棬��������Ϣ�ĺ���������:�л���ϵͳ��������´��ڡ������ļ������ȴ���
 * @param pMenu
 * ��ǰ�����
 * @param nMsgType
 * ��Ϣ����
 * @param nMsgData
 * ��Ϣ����
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
    if(pUISystem->FileBrowserdirlist.nCount<=0)//cf����û��.avi�ļ�
	{
		switch(nMsgData)
		{
        case KEY_ESC://�˳�������Ϣ
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
        case KEY_UPDATE://�л���������´���
            sprintf(tmp_str,"Ver:%s",version_string);
			MessageBox(tmp_str,0,NULL,NULL,NULL,
				NULL,NULL,systemupdatecallback);
            CLEARBIT(pUISystem->SysState.CurState, SYS_BIT_FILEBROSWER);
            SETBIT(pUISystem->SysState.CurState, SYS_BIT_SYSTEMUPDATE);//����ϵͳ����״̬λ
			break;
        case KEY_SYSTEM://�л���ϵͳ����
			SystemChangeWindow(MENUSYSTEMFUNC,0,1,1);
            CLEARBIT(pUISystem->SysState.CurState, SYS_BIT_FILEBROSWER);//����ļ����״̬Ϊ
            SETBIT(pUISystem->SysState.CurState, SYS_BIT_SYSTEMFUNCTION);//����ϵͳ����״̬λ
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
    case KEY_DELETE://ɾ����Ƶ�ļ�
        if(CheckMountFlags() < 0)
        {
        	retval=MsgNeedDispatch;
            break;
        }
		if(nButtonIdx<BUTTON_CNT_FILEBROWSER_FILELIST)
			DoDeleteFile();
		break;
    case KEY_PLAY://������Ƶ�ļ�
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
        SETBIT(pUISystem->SysState.CurState, SYS_BIT_SYSTEMUPDATE);//����ϵͳ����״̬λ

		break;
	case KEY_SYSTEM:
		SystemChangeWindow(MENUSYSTEMFUNC,0,1,1);
        CLEARBIT(pUISystem->SysState.CurState, SYS_BIT_FILEBROSWER);
        SETBIT(pUISystem->SysState.CurState, SYS_BIT_SYSTEMFUNCTION);//����ϵͳ����״̬λ
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
//	Purpose		:	�����ļ��������
//	Parameter	:	@*dirlist - FileListBlock�ṹ��ָ�룬�洢��ǰ·���ļ�ϵͳ��Ϣ��
//					@Index - ��ѡ�е��ļ���������0 ~ (BUTTON_CNT_FILEBROWSER_FILELIST - 1)
//					@DiskIndex - ����������Ϣ0:HDD; 1:SM; 2:MS; 3:SD; 4:CF
//					@PageIndex - ��ǰҳ�����������б��е�����0 ~ TotalNum-1��
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

	//��������
	Index = (Index > (BUTTON_CNT_FILEBROWSER - 1)) ? 0 : Index;
	DiskIndex = (DiskIndex > (SUPPORT_DISK_CNT- 1)) ? 0 : DiskIndex;
	
	//���Ʊ���
	GUI_DrawBitmap(pBmp_BG, 
							AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize), 
							AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize));

	//��дTitle:FileBrowser - 
	GUI_SetTextMode(GUI_TM_TRANS);
	GUI_DispStringHCenterAt(FileBrowserStorageList[DiskIndex], 
							(LCD_WIDTH / 2), 
							(AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_BELOW_HEIGHT));

	//�����·���ʾ��Ϣ
	bmp_x0 = AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize);
	bmp_y0 = AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + pBmp_BG->YSize - GUI_Font20_SongTi_Unicode.YSize - BUTTON_V_INTERVAL_COMMON;

    //zqh 2015��5��25�� add������Ӣ�ĵ���ʾ��Ϣ̫��(�����ַ��߿�)�����Խ�HelpTip11�������С��
    if(pUISystem->allData.Language == ENGLISH)
        GUI_SetFont(&GUI_Font8x16);

    GUI_DispStringAt(HelpTip11[pUISystem->allData.Language], bmp_x0, bmp_y0);

    if(pUISystem->allData.Language == ENGLISH)
        GUI_SetFont(&GUI_Font20_SongTi_Unicode);

    GUI_DrawBitmap(pBmp_logo_207, logo207_x, logo207_y);

	//������Ϣ��
	MenuFileBrowser_RefreshPageEx(dirlist,Index,PageIndex,1);

	//�趨ˢ������
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
 * �ļ����������ƺ���������ʵ���ǵ���Pvr_FileBrowser_FileInfo_Button_SetFocus()�������
 * @param pMenu
 * ��ǰ�����
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
 * ���ó�ʼ��ѡ�е�button�����л����ļ�������ʱ����ô˺���
 * @param pMenu
 * ��ǰ�����
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
 * ���ļ���ɾ�����������طŵȹ���
 * @param pMenu
 * ��ǰ�����
 * @param nMsgType
 * ��Ϣ����
 * @param nMsgData
 * ��Ϣ����
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
                case 2://copy�л�����
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
 * �����ļ��������ڽ���
 * @param pMenu
 * ��ǰ�����
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
	
	//��д��ǰ�������ļ����ļ���
	//����Button(��BUTTON_CNT_FILEOPERATION��,����Ϊ0 ~ (BUTTON_CNT_FILEOPERATION - 1))
	pBmp_BG = (&bmMainMenuBG_360_320);
	pBmp_UnSel = (&bmButton_Blue_785C4C_200_29_UP);
	pBmp_Sel = (&bmButton_Yellow_2060F0_200_29_UP);
	pBmp_Invalid = (&bmButton_Invalid_200_29);

	tmp_interval = (pBmp_BG->YSize - TITLE_ALL_HEIGHT - pBmp_Sel->YSize * (BUTTON_CNT_FILEOPERATION + 1)) / (BUTTON_CNT_FILEOPERATION + 1 + 1);
	bmp_FirstList_x0 = AUTO_GET_X0_FROM_WIDTH(pBmp_Sel->XSize);
	bmp_FirstList_y0 = AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_ALL_HEIGHT + tmp_interval;
	
	bmp_x0 = bmp_FirstList_x0;
	bmp_y0 = bmp_FirstList_y0 + (pBmp_Sel->YSize + tmp_interval) * (nIdx + 1);

		//��Button
	if((1<<nIdx)&MenuButtonMaskBitmap[pUISystem->nMenuIdx]){	//������Ҫ��������Ŀ
			GUI_DrawBitmap(pBmp_Invalid, bmp_x0, bmp_y0);
		}else{
		if(nFocus){	//���Ƹ���ѡ����Ŀ
			GUI_DrawBitmap(pBmp_Sel, bmp_x0, bmp_y0);
			}else{			//���ƷǸ�����Ŀ
			GUI_DrawBitmap(pBmp_UnSel, bmp_x0, bmp_y0);
		}
		}
	if((1<<nIdx)&MenuButtonMaskBitmap[pUISystem->nMenuIdx])
	{	//������Ҫ��������Ŀ
		GUI_SetColor(GUI_GRAY);

		//дContent
		GUI_DispStringHCenterAt(FileOperationList[nIdx], 
								(bmp_x0 + pBmp_UnSel->XSize / 2),
								(bmp_y0 + BUTTON_V_OFFSET_DISPLAY));
		GUI_SetColor(GUI_WHITE);

	}
	else
	{
		//дContent
		GUI_DispStringHCenterAt(FileOperationList[nIdx], 
								(bmp_x0 + pBmp_UnSel->XSize / 2),
								(bmp_y0 + BUTTON_V_OFFSET_DISPLAY));
	}

    // NOTE: Do not SetRefreshArea should notify
	return 1;
}

//--------------------------------------------------------------------------//
//	Name		:	Pvr_FileBrowser_Operation_Button_SetFocus
//	Purpose		:	�����ļ�����ѡ�����
//	Parameter	:	@Index - ��ѡ�е��ļ���������0 ~ (BUTTON_CNT_FILEOPERATION - 1)
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
	
	//��������
	Index = (Index > (BUTTON_CNT_FILEOPERATION - 1)) ? 0 : Index;

	//���Ʊ���
	pBmp_BG = (&bmMainMenuBG_360_320);
	GUI_DrawBitmap(pBmp_BG, 
							AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize), 
							AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize));

	//��дTitle:File operation
	GUI_SetTextMode(GUI_TM_TRANS);
	GUI_DispStringHCenterAt(FileOperation[pUISystem->allData.Language], 
							(LCD_WIDTH / 2), 
							(AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_BELOW_HEIGHT));

	//��õ�ǰ�������ļ����ļ���
	filename=GetCurrentFileName(pUISystem->FileBrowserIndex,pUISystem->FileBrowserSelFile,&pUISystem->FileBrowserdirlist);

	//��д��ǰ�������ļ����ļ���
	//����Button(��BUTTON_CNT_FILEOPERATION��,����Ϊ0 ~ (BUTTON_CNT_FILEOPERATION - 1))
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

	//�趨ˢ������
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
 * �ļ�����������ƺ��������磺��ʾ�������ļ���ɾ���ļ�����Ϣ��
 * @param pMenu
 * ��ǰ�����
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
 * ��cf���ļ�������u��
 * @return
 */
int DoFileCopyTO(void)
{
	char *destPath;
	char sorPath[MAX_FS_PATH];
	int ret;
	struct FileAttrib *pFsInfo;

    ret = DoUDiskDetect();//��������Ƿ����
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
 * �ļ�������U�̵Ĵ�����
 * @param pMenu
 * ��ǰ�����
 * @param nMsgType
 * ��Ϣ����
 * @param nMsgData
 * ��Ϣ����
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
 * �������ļ�ʱ�����ô˺��������ơ��ļ����ơ���Ϣ����
 * @param pMenu
 * ��ǰ�����
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
	//	Purpose	:	���Ʊ���
	//--------------------------------------------------------------------------//
	GUI_DrawBitmap(pBmp_BG, 
				AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize), 
				AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize));
	GUI_DrawBitmap(pBmp_SubBG, 
				AUTO_GET_X0_FROM_WIDTH(pBmp_SubBG->XSize), 
				(AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_ALL_HEIGHT + BUTTON_V_INTERVAL_COMMON));

	//--------------------------------------------------------------------------//
	//	Purpose	:	��дTitle
	//--------------------------------------------------------------------------//
	GUI_SetTextMode(GUI_TM_TRANS);
	GUI_DispStringHCenterAt(Message[pUISystem->allData.Language], 
							(LCD_WIDTH / 2), 
							(AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + TITLE_BELOW_HEIGHT));

	//--------------------------------------------------------------------------//
	//	Purpose	:	��д��ʾ����
	//	Purpose	:	�����·���ʾ��Ϣ
	//--------------------------------------------------------------------------//

	Rect.x0 = AUTO_GET_X0_FROM_WIDTH(pBmp_SubBG->XSize);
	Rect.y0 = AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize);
	Rect.x1 = AUTO_GET_X0_FROM_WIDTH(pBmp_SubBG->XSize) + pBmp_SubBG->XSize;
	Rect.y1 = AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + pBmp_BG->YSize;
	
    MessageDisp=FileCopyHelpInfo[pUISystem->allData.Language];
	//zqh 2015��3��27��modify����HelpTip1��ΪHelpTip8
	MessageHelp=HelpTip8[pUISystem->allData.Language];
	
    //��ʾ��ʾ��Ϣ���硰�����ļ����Ƿ��������
	GUI_DispStringInRect(MessageDisp, &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);

    //��ʾ�������֣�������ϵġ��ǣ���ȷ�������񣺰�ȡ������
    //zqh 2015��5��26�� add����ϵͳ����ΪӢ��ʱ���������С��
    if(pUISystem->allData.Language == ENGLISH)
        GUI_SetFont(&GUI_Font8x16);

	GUI_DispStringAt(MessageHelp, 
					AUTO_GET_X0_FROM_WIDTH(pBmp_SubBG->XSize), 
					(AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize) + pBmp_BG->YSize - GUI_Font20_SongTi_Unicode.YSize - PROCESS_V_OFFSET_BG_BOTTON));
	//temp--�趨ˢ������
	SetRefreshAreaPos(OSDPOS_BASEMENU, 
					AUTO_GET_X0_FROM_WIDTH(pBmp_BG->XSize), 
					AUTO_GET_Y0_FROM_HEIGHT(pBmp_BG->YSize), 
					pBmp_BG->XSize, 
					pBmp_BG->YSize, 
					ALPHA_COMMON,
					TRUE);
	return 1;
}


