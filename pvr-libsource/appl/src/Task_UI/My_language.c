/*
----------------------------------------------------------------------
File		:	My_language.c
Purpose	:
----------------------------------------------------------------------
Version-Date			:	2006.12.12
Author-Explanation		:	Sean.Yu
----------------------------------------------------------------------
*/

#include <config.h>
#include <my_language.h>
#include <GUI.H>

/*
Notes:
	(1)
		En--English
		SiCn--Simple Chinese
		TrCn--Traditional Chinese
	(2)
		����Unicode���е�һ���ֽ���00������,
		ֱ����д�����ַ�����,
		������1��д"1",����д��Ӧ��Unicode��"\x00\x31"
	(3)
		ģ������
//--------------------------------------------------------------------------//

const char _En[] = 
{
	//
	""
};

const char _SiCn[] = 
{
	//
	GUI_UC_START
	""
	GUI_UC_END
	"\x0"
};

const char _TrCn[] = 
{
	//
	GUI_UC_START
	""
	GUI_UC_END
	"\x0"
};

const char *[LANGUAGE_CNT] = 
{
	_En, 
	_SiCn, 
	_TrCn
};

//--------------------------------------------------------------------------//

*/

//--------------------------------------------------------------------------//
//	Position	:	Main Menu
//--------------------------------------------------------------------------//
//

/*
    2015��3��20�� zqh add
    �޸�ӛ䛣�
    �����С�CF������Ϊ�������̡�

    ��������U�̡���Ϊ��U��δ���ӡ�
    �����Ͽ�U�̡���Ϊ��U�������ӡ�
    ����ϵͳ��������Ϊ���ȴ�ϵͳ������


*/

//zqh 2015��3��26�� add,
#define ESCKEY_SiCn     "\x90\x00\x51\xfa"//�˳�
#define ESCKEY_TrCn     "\x90\x00\x51\xfa"//�˳�
#define ENTERKEY_SiCn   "\x78\x6e\x8b\xa4"//ȷ��
#define ENTERKEY_TrCn   "\x78\xba\x8a\x8d"//�_�J
#define PLAYKEY_SiCn    "\x64\xad\x65\x3e"//����
#define PLAYKEY_TrCn    "\x64\xad\x65\x3e"//����

#if 0
#define TABLEKEY_EN     "Rec"
#define TABLEKEY_SiCn   "\x5f\x55\x53\xd6"//¼ȡ
#define TABLEKEY_TrCn   "\x93\x04\x53\xd6"//�ȡ
#else
#define TABLEKEY_EN     "Rec"
#define TABLEKEY_SiCn   "\x8b\xb0\x5f\x55"//��¼
#define TABLEKEY_TrCn   "\x8a\x18\x93\x04"//ӛ�
#endif
const char MainMenu_En[] = 
{
	//Main Menu
	"Main Menu"
};

const char MainMenu_SiCn[] = 
{
	//���˵�
	GUI_UC_START
	"\x4E\x3B\x83\xDC\x53\x55"
	GUI_UC_END
	"\x0"
};

const char MainMenu_TrCn[] = 
{
	//���ˆ�
	GUI_UC_START
	"\x4E\x3B\x83\xDC\x55\xAE"
	GUI_UC_END
	"\x0"
};

const char *MainMenu[LANGUAGE_CNT] = 
{
	MainMenu_En, 
	MainMenu_SiCn, 
	MainMenu_TrCn
};

//--------------------------------------------------------------------------//

const char Schedule_En[] = 
{
	//Schedule
	"Schedule"
};

const char Schedule_SiCn[] = 
{
	//¼�Ƽƻ�
	GUI_UC_START
	"\x5F\x55\x52\x36\x8B\xA1\x52\x12"
	GUI_UC_END
	"\x0"
};

const char Schedule_TrCn[] = 
{
	//��uӋ��
	GUI_UC_START
	"\x93\x04\x88\xFD\x8A\x08\x75\x6B"
	GUI_UC_END
	"\x0"
};

const char *Schedule[LANGUAGE_CNT] = 
{
	Schedule_En, 
	Schedule_SiCn, 
	Schedule_TrCn
};

//--------------------------------------------------------------------------//

const char ChannelSetting_En[] = 
{
	//Channel setting
	"Channel setting"
};

const char ChannelSetting_SiCn[] = 
{
	//Ƶ������
	GUI_UC_START
	"\x98\x91\x90\x53\x8B\xBE\x7F\x6E"
	GUI_UC_END
	"\x0"
};

const char ChannelSetting_TrCn[] = 
{
	//�l���O��
	GUI_UC_START
	"\x98\x3B\x90\x53\x8A\x2D\x7F\x6E"
	GUI_UC_END
	"\x0"
};

const char *ChannelSetting[LANGUAGE_CNT] = 
{
	ChannelSetting_En, 
	ChannelSetting_SiCn, 
	ChannelSetting_TrCn
};

//--------------------------------------------------------------------------//

const char ChannelScan_En[] = 
{
	//Channel scan
	"Channel scan"
};

const char ChannelScan_SiCn[] = 
{
	//Ƶ������
	GUI_UC_START
	"\x98\x91\x90\x53\x64\x1C\x7D\x22"
	GUI_UC_END
	"\x0"
};

const char ChannelScan_TrCn[] = 
{
	//�l������
	GUI_UC_START
	"\x98\x3B\x90\x53\x64\x1C\x7D\x22"
	GUI_UC_END
	"\x0"
};

const char *ChannelScan[LANGUAGE_CNT] = 
{
	ChannelScan_En, 
	ChannelScan_SiCn, 
	ChannelScan_TrCn
};

//--------------------------------------------------------------------------//

const char AutoScan_En[] = 
{
	//Auto scan
	"Auto scan"
};

const char AutoScan_SiCn[] = 
{
	//�Զ�����
	GUI_UC_START
	"\x81\xEA\x52\xA8\x64\x1C\x7D\x22"
	GUI_UC_END
	"\x0"
};

const char AutoScan_TrCn[] = 
{
	//�Ԅ�����
	GUI_UC_START
	"\x81\xEA\x52\xD5\x64\x1C\x7D\x22"
	GUI_UC_END
	"\x0"
};

const char *AutoScan[LANGUAGE_CNT] = 
{
	AutoScan_En, 
	AutoScan_SiCn, 
	AutoScan_TrCn
};

//--------------------------------------------------------------------------//

const char ManualScan_En[] = 
{
	//Manual scan
	"Manual scan"
};

const char ManualScan_SiCn[] = 
{
	//�ֶ�����
	GUI_UC_START
	"\x62\x4B\x52\xA8\x64\x1C\x7D\x22"
	GUI_UC_END
	"\x0"
};

const char ManualScan_TrCn[] = 
{
	//�ք�����
	GUI_UC_START
	"\x62\x4B\x52\xD5\x64\x1C\x7D\x22"
	GUI_UC_END
	"\x0"
};

const char *ManualScan[LANGUAGE_CNT] = 
{
	ManualScan_En, 
	ManualScan_SiCn, 
	ManualScan_TrCn
};

//--------------------------------------------------------------------------//

const char RecordSetting_En[] = 
{
	//Record setting
	"Record setting"
};

const char RecordSetting_SiCn[] = 
{
	//¼������
	GUI_UC_START
	"\x5F\x55\x52\x36\x8B\xBE\x7F\x6E"
	GUI_UC_END
	"\x0"
};

const char RecordSetting_TrCn[] = 
{
	//��u�O��
	GUI_UC_START
	"\x93\x04\x88\xFD\x8A\x2D\x7F\x6E"
	GUI_UC_END
	"\x0"
};

const char *RecordSetting[LANGUAGE_CNT] = 
{
	RecordSetting_En, 
	RecordSetting_SiCn, 
	RecordSetting_TrCn
};

//zqh 2015��10��29�� add��
const char EnterFileBrowser_En[] =
{
    //Entering file browser menu,\nplsase wait..."
    "Entering file browser menu,\nPlsase wait..."
};

const char EnterFileBrowser_SiCn[] =
{
    //���ڽ����ļ�������棬���Ժ�...
    GUI_UC_START
    "\x6b\x63\x57\x28\x8f\xdb\x51\x65\x65\x87\x4e\xf6\x6d\x4f\x89\xc8\x75\x4c\x97\x62\xff\x0c"
    GUI_UC_END

    "\n"

    GUI_UC_START
    "\x8b\xf7\x7a\x0d\x54\x0e"
    GUI_UC_END
    "..."
    "\x0"
};

const char EnterFileBrowser_TrCn[] =
{
    //�����M���ļ��g�[���棬Ո����...
    GUI_UC_START
    "\x6b\x63\x57\x28\x90\x32\x51\x65\x65\x87\x4e\xf6\x70\x0f\x89\xbd\x75\x4c\x97\x62\xff\x0c"
    GUI_UC_END
    "\n"
    GUI_UC_START
    "\x8a\xcb\x7a\x0d\x5f\x8c"
    GUI_UC_END
    "..."
    "\x0"
};

const char *EnterFileBrowser[LANGUAGE_CNT] =
{
    EnterFileBrowser_En,
    EnterFileBrowser_SiCn,
    EnterFileBrowser_TrCn
};


//--------------------------------------------------------------------------//

const char FileBrowser_En[] = 
{
    //File Browser
    "File Browser"
};

const char FileBrowser_SiCn[] = 
{
	//�ļ����
	GUI_UC_START
	"\x65\x87\x4E\xF6\x6D\x4F\x89\xC8"
	GUI_UC_END
	"\x0"
};

const char FileBrowser_TrCn[] = 
{
	//�n���[
	GUI_UC_START
	"\x6A\x94\x6D\x41\x89\xBD"
	GUI_UC_END
	"\x0"
};

const char *FileBrowser[LANGUAGE_CNT] = 
{
	FileBrowser_En, 
	FileBrowser_SiCn, 
	FileBrowser_TrCn
};

//--------------------------------------------------------------------------//

const char SystemSetting_En[] = 
{
	//System setting
	"System setting"
};

const char SystemSetting_SiCn[] = 
{
	//ϵͳ����
	GUI_UC_START
	"\x7C\xFB\x7E\xDF\x8B\xBE\x7F\x6E"
	GUI_UC_END
	"\x0"
};

const char SystemSetting_TrCn[] = 
{
	//ϵ�y�O��
	GUI_UC_START
	"\x7C\xFB\x7D\x71\x8A\x2D\x7F\x6E"
	GUI_UC_END
	"\x0"
};

const char *SystemSetting[LANGUAGE_CNT] = 
{
	SystemSetting_En, 
	SystemSetting_SiCn, 
	SystemSetting_TrCn
};

//--------------------------------------------------------------------------//

const char TVSetting_En[] = 
{
	//TV setting
	"TV setting"
};

const char TVSetting_SiCn[] = 
{
	//��������
	GUI_UC_START
	"\x75\x35\x89\xC6\x8B\xBE\x7F\x6E"
	GUI_UC_END
	"\x0"
};

const char TVSetting_TrCn[] = 
{
	//�ҕ�O��
	GUI_UC_START
	"\x96\xFB\x89\x96\x8A\x2D\x7F\x6"
	GUI_UC_END
	"\x0"
};

const char *TVSetting[LANGUAGE_CNT] = 
{
	TVSetting_En, 
	TVSetting_SiCn, 
	TVSetting_TrCn
};

//--------------------------------------------------------------------------//
//	Position	:	TV Setting Menu
//--------------------------------------------------------------------------//
//
const char Contrast_En[] = 
{
	//Contrast
	"Contrast"
};

const char Contrast_SiCn[] = 
{
	//�Աȶ�
	GUI_UC_START
	"\x5B\xF9\x6B\xD4\x5E\xA6"
	GUI_UC_END
	"\x0"
};

const char Contrast_TrCn[] = 
{
	//���ȶ�
	GUI_UC_START
	"\x5C\x0D\x6B\xD4\x5E\xA6"
	GUI_UC_END
	"\x0"
};

const char *Contrast[LANGUAGE_CNT] = 
{
	Contrast_En, 
	Contrast_SiCn, 
	Contrast_TrCn
};

//--------------------------------------------------------------------------//

const char Brightness_En[] = 
{
	//Brightness
	"Brightness"
};

const char Brightness_SiCn[] = 
{
	//����
	GUI_UC_START
	"\x4E\xAE\x5E\xA6"
	GUI_UC_END
	"\x0"
};

const char Brightness_TrCn[] = 
{
	//����
	GUI_UC_START
	"\x4E\xAE\x5E\xA6"
	GUI_UC_END
	"\x0"
};

const char *Brightness[LANGUAGE_CNT] = 
{
	Brightness_En, 
	Brightness_SiCn, 
	Brightness_TrCn
};

//--------------------------------------------------------------------------//
//	Position	:	System Setting Menu
//--------------------------------------------------------------------------//
//
const char SystemDate_En[] = 
{
	//System date
	"System date"
};

const char SystemDate_SiCn[] = 
{
	//ϵͳ����
	GUI_UC_START
	"\x7C\xFB\x7E\xDF\x65\xE5\x67\x1F"
	GUI_UC_END
	"\x0"
};

const char SystemDate_TrCn[] = 
{
	//ϵ�y����
	GUI_UC_START
	"\x7C\xFB\x7D\x71\x65\xE5\x67\x1F"
	GUI_UC_END
	"\x0"
};

const char *SystemDate[LANGUAGE_CNT] = 
{
	SystemDate_En, 
	SystemDate_SiCn, 
	SystemDate_TrCn
};

//--------------------------------------------------------------------------//

const char WeekDay_En[] = 
{
	//Week day
	"Week day"
};

const char WeekDay_SiCn[] = 
{
	//����
	GUI_UC_START
	"\x66\x1F\x67\x1F"
	GUI_UC_END
	"\x0"
};

const char WeekDay_TrCn[] = 
{
	//����
	GUI_UC_START
	"\x66\x1F\x67\x1F"
	GUI_UC_END
	"\x0"
};

const char *WeekDay[LANGUAGE_CNT] = 
{
	WeekDay_En, 
	WeekDay_SiCn, 
	WeekDay_TrCn
};

//--------------------------------------------------------------------------//

const char SystemClock_En[] = 
{
	//System clock
	"System clock"
};

const char SystemClock_SiCn[] = 
{
	//ϵͳʱ��
	GUI_UC_START
	"\x7C\xFB\x7E\xDF\x65\xF6\x94\x9F"
	GUI_UC_END
	"\x0"
};

const char SystemClock_TrCn[] = 
{
	//ϵ�y�r� 
	GUI_UC_START
	"\x7C\xFB\x7D\x71\x66\x42\x94\x18"
	GUI_UC_END
	"\x0"
};

const char *SystemClock[LANGUAGE_CNT] = 
{
	SystemClock_En, 
	SystemClock_SiCn, 
	SystemClock_TrCn
};

//--------------------------------------------------------------------------//

const char VideoMode_En[] = 
{
	//Video mode
	"Video mode"
};

const char VideoMode_SiCn[] = 
{
	//��Ƶ��ʽ
	GUI_UC_START
	"\x89\xC6\x98\x91\x52\x36\x5F\x0F"
	GUI_UC_END
	"\x0"
};

const char VideoMode_TrCn[] = 
{
	//ҕ�l��ʽ
	GUI_UC_START
	"\x89\x96\x98\x3B\x52\x36\x5F\x0F"
	GUI_UC_END
	"\x0"
};

const char *VideoMode[LANGUAGE_CNT] = 
{
	VideoMode_En, 
	VideoMode_SiCn, 
	VideoMode_TrCn
};

//--------------------------------------------------------------------------//

const char Language_En[] = 
{
	//Language
    "Language Setting"
};

const char Language_SiCn[] = 
{
    //��������(\x8bbe\x7f6e)
	GUI_UC_START
    "\x8B\xED\x8A\x00\x8b\xbe\x7f\x6e"
	GUI_UC_END
	"\x0"
};

const char Language_TrCn[] = 
{
    //�Z���O��(\x8a2d\x7f6e)
	GUI_UC_START
    "\x8A\x9E\x8A\x00\x8a\x2d\x7f\x6e"
	GUI_UC_END
	"\x0"
};

const char *Language[LANGUAGE_CNT] = 
{
	Language_En, 
	Language_SiCn, 
	Language_TrCn
};

//--------------------------------------------------------------------------//

const char InputSelect_En[] = 
{
	//Input select
	"Input select"
};

const char InputSelect_SiCn[] = 
{
	//��������
	GUI_UC_START
	"\x8F\x93\x51\x65\x8B\xBE\x7F\x6E"
	GUI_UC_END
	"\x0"
};

const char InputSelect_TrCn[] = 
{
	//ݔ���O��
	GUI_UC_START
	"\x8F\x38\x51\x65\x8A\x2D\x7F\x6E"
	GUI_UC_END
	"\x0"
};

const char *InputSelect[LANGUAGE_CNT] = 
{
	InputSelect_En, 
	InputSelect_SiCn, 
	InputSelect_TrCn
};

//--------------------------------------------------------------------------//

const char OutputSelect_En[] = 
{
	//Output select
	"Output select"
};

const char OutputSelect_SiCn[] = 
{
	//�������
	GUI_UC_START
	"\x8F\x93\x51\xFA\x8B\xBE\x7F\x6E"
	GUI_UC_END
	"\x0"
};

const char OutputSelect_TrCn[] = 
{
	//ݔ���O��
	GUI_UC_START
	"\x8F\x38\x51\xFA\x8A\x2D\x7F\x6E"
	GUI_UC_END
	"\x0"
};

const char *OutputSelect[LANGUAGE_CNT] = 
{
	OutputSelect_En, 
	OutputSelect_SiCn, 
	OutputSelect_TrCn
};

//--------------------------------------------------------------------------//

const char Reset_En[] = 
{
	//Reset
	"Reset"
};

const char Reset_SiCn[] = 
{
	//��λ
	GUI_UC_START
	"\x59\x0D\x4F\x4D"
	GUI_UC_END
	"\x0"
};

const char Reset_TrCn[] = 
{
	//��λ
	GUI_UC_START
	"\x5F\xA9\x4F\x4D"
	GUI_UC_END
	"\x0"
};

const char *Reset[LANGUAGE_CNT] = 
{
	Reset_En, 
	Reset_SiCn, 
	Reset_TrCn
};

//--------------------------------------------------------------------------//

const char StorageInfo_En[] = 
{
	//Storage info
	"Storage info"
};

const char StorageInfo_SiCn[] = 
{
	//�洢��Ϣ
	GUI_UC_START
	"\x5B\x58\x50\xA8\x4F\xE1\x60\x6F"
	GUI_UC_END
	"\x0"
};

const char StorageInfo_TrCn[] = 
{
	//�惦�YӍ
	GUI_UC_START
	"\x5B\x58\x51\x32\x8C\xC7\x8A\x0A"
	GUI_UC_END
	"\x0"
};

const char *StorageInfo[LANGUAGE_CNT] = 
{
	StorageInfo_En, 
	StorageInfo_SiCn, 
	StorageInfo_TrCn
};

//--------------------------------------------------------------------------//

const char Format_En[] = 
{
	//Format
	"Format"
};

const char Format_SiCn[] = 
{
	//��ʽ��
	GUI_UC_START
	"\x68\x3C\x5F\x0F\x53\x16"
	GUI_UC_END
	"\x0"
};

const char Format_TrCn[] = 
{
	//��ʽ��
	GUI_UC_START
	"\x68\x3C\x5F\x0F\x53\x16"
	GUI_UC_END
	"\x0"
};

const char *Format[LANGUAGE_CNT] = 
{
	Format_En, 
	Format_SiCn, 
	Format_TrCn
};

//zqh 2015��3��31�� add����ӡ�����ѡ��˵�����ѡ���У��������ġ�Ӣ�ġ���������
//--------------------------------------------------------------------------//

const char English_En[] =
{
    //English
    "English"
};

const char SampleChinese_SiCn[] =
{
    //��������
    GUI_UC_START
    "\x7B\x80\x4F\x53\x4E\x2D\x65\x87"
    GUI_UC_END
    "\x0"
};

const char TraditionChinese_TrCn[] =
{
    //���w����
    GUI_UC_START
    "\x7E\x41\x9A\xD4\x4E\x2D\x65\x87"
    GUI_UC_END
    "\x0"
};

//--------------------------------------------------------------------------//

const char SelectLanguage_En[] = 
{
	//English
	"English"
};

const char SelectLanguage_SiCn[] = 
{
	//��������
	GUI_UC_START
	"\x7B\x80\x4F\x53\x4E\x2D\x65\x87"
	GUI_UC_END
	"\x0"
};

const char SelectLanguage_TrCn[] = 
{
	//���w����
	GUI_UC_START
	"\x7E\x41\x9A\xD4\x4E\x2D\x65\x87"
	GUI_UC_END
	"\x0"
};

const char *SelectLanguage[LANGUAGE_CNT] = 
{
	SelectLanguage_En, 
	SelectLanguage_SiCn, 
	SelectLanguage_TrCn
};

//--------------------------------------------------------------------------//

const char Sunday_En[] = 
{
	//Sunday
	"Sunday"
};

const char Sunday_SiCn[] = 
{
	//������
	GUI_UC_START
	"\x66\x1F\x67\x1F\x65\xE5"
	GUI_UC_END
	"\x0"
};

const char Sunday_TrCn[] = 
{
	//������
	GUI_UC_START
	"\x66\x1F\x67\x1F\x65\xE5"
	GUI_UC_END
	"\x0"
};

const char *Sunday[LANGUAGE_CNT] = 
{
	Sunday_En, 
	Sunday_SiCn, 
	Sunday_TrCn
};

//--------------------------------------------------------------------------//

const char Monday_En[] = 
{
	//Monday
	"Monday"
};

const char Monday_SiCn[] = 
{
	//����һ
	GUI_UC_START
	"\x66\x1F\x67\x1F\x4E\x00"
	GUI_UC_END
	"\x0"
};

const char Monday_TrCn[] = 
{
	//����һ
	GUI_UC_START
	"\x66\x1F\x67\x1F\x4E\x00"
	GUI_UC_END
	"\x0"
};

const char *Monday[LANGUAGE_CNT] = 
{
	Monday_En, 
	Monday_SiCn, 
	Monday_TrCn
};

//--------------------------------------------------------------------------//

const char Tuesday_En[] = 
{
	//Tuesday
	"Tuesday"
};

const char Tuesday_SiCn[] = 
{
	//���ڶ�
	GUI_UC_START
	"\x66\x1F\x67\x1F\x4E\x8C"
	GUI_UC_END
	"\x0"
};

const char Tuesday_TrCn[] = 
{
	//���ڶ�
	GUI_UC_START
	"\x66\x1F\x67\x1F\x4E\x8C"
	GUI_UC_END
	"\x0"
};

const char *Tuesday[LANGUAGE_CNT] = 
{
	Tuesday_En, 
	Tuesday_SiCn, 
	Tuesday_TrCn
};

//--------------------------------------------------------------------------//

const char Wednesday_En[] = 
{
	//Wednesday
	"Wednesday"
};

const char Wednesday_SiCn[] = 
{
	//������
	GUI_UC_START
	"\x66\x1F\x67\x1F\x4E\x09"
	GUI_UC_END
	"\x0"
};

const char Wednesday_TrCn[] = 
{
	//������
	GUI_UC_START
	"\x66\x1F\x67\x1F\x4E\x09"
	GUI_UC_END
	"\x0"
};

const char *Wednesday[LANGUAGE_CNT] = 
{
	Wednesday_En, 
	Wednesday_SiCn, 
	Wednesday_TrCn
};

//--------------------------------------------------------------------------//

const char Thursday_En[] = 
{
	//Thursday
	"Thursday"
};

const char Thursday_SiCn[] = 
{
	//������
	GUI_UC_START
	"\x66\x1F\x67\x1F\x56\xDB"
	GUI_UC_END
	"\x0"
};

const char Thursday_TrCn[] = 
{
	//������
	GUI_UC_START
	"\x66\x1F\x67\x1F\x56\xDB"
	GUI_UC_END
	"\x0"
};

const char *Thursday[LANGUAGE_CNT] = 
{
	Thursday_En, 
	Thursday_SiCn, 
	Thursday_TrCn
};

//--------------------------------------------------------------------------//

const char Friday_En[] = 
{
	//Friday
	"Friday"
};

const char Friday_SiCn[] = 
{
	//������
	GUI_UC_START
	"\x66\x1F\x67\x1F\x4E\x94"
	GUI_UC_END
	"\x0"
};

const char Friday_TrCn[] = 
{
	//������
	GUI_UC_START
	"\x66\x1F\x67\x1F\x4E\x94"
	GUI_UC_END
	"\x0"
};

const char *Friday[LANGUAGE_CNT] = 
{
	Friday_En, 
	Friday_SiCn, 
	Friday_TrCn
};

//--------------------------------------------------------------------------//

const char Saturday_En[] = 
{
	//Saturday
	"Saturday"
};

const char Saturday_SiCn[] = 
{
	//������
	GUI_UC_START
	"\x66\x1F\x67\x1F\x51\x6D"
	GUI_UC_END
	"\x0"
};

const char Saturday_TrCn[] = 
{
	//������
	GUI_UC_START
	"\x66\x1F\x67\x1F\x51\x6D"
	GUI_UC_END
	"\x0"
};

const char *Saturday[LANGUAGE_CNT] = 
{
	Saturday_En, 
	Saturday_SiCn, 
	Saturday_TrCn
};

//--------------------------------------------------------------------------//

const char SUN_En[] = 
{
	//SUN
	"SUN"
};

const char SUN_SiCn[] = 
{
	//����
	GUI_UC_START
	"\x54\x68\x65\xE5"
	GUI_UC_END
	"\x0"
};

const char SUN_TrCn[] = 
{
	//�L ��
	GUI_UC_START
	"\x90\x31\x65\xE5"
	GUI_UC_END
	"\x0"
};

const char *SUN[LANGUAGE_CNT] = 
{
	SUN_En, 
	SUN_SiCn, 
	SUN_TrCn
};

//--------------------------------------------------------------------------//

const char MON_En[] = 
{
	//MON
	"MON"
};

const char MON_SiCn[] = 
{
	//��һ
	GUI_UC_START
	"\x54\x68\x4E\x00"
	GUI_UC_END
	"\x0"
};

const char MON_TrCn[] = 
{
	//�L һ
	GUI_UC_START
	"\x90\x31\x4E\x00"
	GUI_UC_END
	"\x0"
};

const char *MON[LANGUAGE_CNT] = 
{
	MON_En, 
	MON_SiCn, 
	MON_TrCn
};

//--------------------------------------------------------------------------//

const char TUE_En[] = 
{
	//TUE
	"TUE"
};

const char TUE_SiCn[] = 
{
	//�ܶ�
	GUI_UC_START
	"\x54\x68\x4E\x8C"
	GUI_UC_END
	"\x0"
};

const char TUE_TrCn[] = 
{
	//�L ��
	GUI_UC_START
	"\x90\x31\x4E\x8C"
	GUI_UC_END
	"\x0"
};

const char *TUE[LANGUAGE_CNT] = 
{
	TUE_En, 
	TUE_SiCn, 
	TUE_TrCn
};

//--------------------------------------------------------------------------//

const char WED_En[] = 
{
	//WED
	"WED"
};

const char WED_SiCn[] = 
{
	//����
	GUI_UC_START
	"\x54\x68\x4E\x09"
	GUI_UC_END
	"\x0"
};

const char WED_TrCn[] = 
{
	//�L ��
	GUI_UC_START
	"\x90\x31\x4E\x09"
	GUI_UC_END
	"\x0"
};

const char *WED[LANGUAGE_CNT] = 
{
	WED_En, 
	WED_SiCn, 
	WED_TrCn
};

//--------------------------------------------------------------------------//

const char THU_En[] = 
{
	//THU
	"THU"
};

const char THU_SiCn[] = 
{
	//����
	GUI_UC_START
	"\x54\x68\x56\xDB"
	GUI_UC_END
	"\x0"
};

const char THU_TrCn[] = 
{
	//�L ��
	GUI_UC_START
	"\x90\x31\x56\xDB"
	GUI_UC_END
	"\x0"
};

const char *THU[LANGUAGE_CNT] = 
{
	THU_En, 
	THU_SiCn, 
	THU_TrCn
};

//--------------------------------------------------------------------------//

const char FRI_En[] = 
{
	//FRI
	"FRI"
};

const char FRI_SiCn[] = 
{
	//����
	GUI_UC_START
	"\x54\x68\x4E\x94"
	GUI_UC_END
	"\x0"
};

const char FRI_TrCn[] = 
{
	//�L ��
	GUI_UC_START
	"\x90\x31\x4E\x94"
	GUI_UC_END
	"\x0"
};

const char *FRI[LANGUAGE_CNT] = 
{
	FRI_En, 
	FRI_SiCn, 
	FRI_TrCn
};

//--------------------------------------------------------------------------//

const char SAT_En[] = 
{
	//SAT
	"SAT"
};

const char SAT_SiCn[] = 
{
	//����
	GUI_UC_START
	"\x54\x68\x51\x6D"
	GUI_UC_END
	"\x0"
};

const char SAT_TrCn[] = 
{
	//�L ��
	GUI_UC_START
	"\x90\x31\x51\x6D"
	GUI_UC_END
	"\x0"
};

const char *SAT[LANGUAGE_CNT] = 
{
	SAT_En, 
	SAT_SiCn, 
	SAT_TrCn
};

//--------------------------------------------------------------------------//

const char SVideo_En[] = 
{
	//S-video
	"S-video"
};

const char SVideo_SiCn[] = 
{
	//S-video
	"S-video"
};

const char SVideo_TrCn[] = 
{
	//S-video
	"S-video"
};

const char *SVideo[LANGUAGE_CNT] = 
{
	SVideo_En, 
	SVideo_SiCn, 
	SVideo_TrCn
};

//--------------------------------------------------------------------------//

const char Cvbs_En[] = 
{
	//CVBS
	"CVBS"
};

const char Cvbs_SiCn[] = 
{
	//CVBS
	"CVBS"
};

const char Cvbs_TrCn[] = 
{
	//CVBS
	"CVBS"
};

const char *Cvbs[LANGUAGE_CNT] = 
{
	Cvbs_En, 
	Cvbs_SiCn, 
	Cvbs_TrCn
};

//--------------------------------------------------------------------------//

const char Cable_En[] = 
{
	//Cable
	"TV"
};

const char Cable_SiCn[] = 
{
	//Cable
	"TV"
};

const char Cable_TrCn[] = 
{
	//Cable
	"TV"
};

const char *Cable[LANGUAGE_CNT] = 
{
	Cable_En, 
	Cable_SiCn, 
	Cable_TrCn
};

//--------------------------------------------------------------------------//

const char Component_En[] = 
{
	//YCbCr
	"YCbCr"
};

const char Component_SiCn[] = 
{
	//ɫ��
	GUI_UC_START
	"\x82\x72\x5D\xEE"
	GUI_UC_END
	"\x0"
};

const char Component_TrCn[] = 
{
	//ɫ��
	GUI_UC_START
	"\x82\x72\x5D\xEE"
	GUI_UC_END
	"\x0"
};

const char *Component[LANGUAGE_CNT] = 
{
	Component_En, 
	Component_SiCn, 
	Component_TrCn
};

//--------------------------------------------------------------------------//

const char PAL_DK_En[] = 
{
	//PAL_DK
	"PAL_DK"
};

const char PAL_DK_SiCn[] = 
{
	//PAL_DK
	"PAL_DK"
};

const char PAL_DK_TrCn[] = 
{
	//PAL_DK
	"PAL_DK"
};

const char *PAL_DK[LANGUAGE_CNT] = 
{
	PAL_DK_En, 
	PAL_DK_SiCn, 
	PAL_DK_TrCn
};

//--------------------------------------------------------------------------//

const char PAL_I_En[] = 
{
	//PAL_I
	"PAL_I"
};

const char PAL_I_SiCn[] = 
{
	//PAL_I
	"PAL_I"
};

const char PAL_I_TrCn[] = 
{
	//PAL_I
	"PAL_I"
};

const char *PAL_I[LANGUAGE_CNT] = 
{
	PAL_I_En, 
	PAL_I_SiCn, 
	PAL_I_TrCn
};

//--------------------------------------------------------------------------//

const char PAL_BG_En[] = 
{
	//PAL_BG
	"PAL_BG"
};

const char PAL_BG_SiCn[] = 
{
	//PAL_BG
	"PAL_BG"
};

const char PAL_BG_TrCn[] = 
{
	//PAL_BG
	"PAL_BG"
};

const char *PAL_BG[LANGUAGE_CNT] = 
{
	PAL_BG_En, 
	PAL_BG_SiCn, 
	PAL_BG_TrCn
};

//--------------------------------------------------------------------------//

const char NTSC_En[] = 
{
	//NTSC
	"NTSC"
};

const char NTSC_SiCn[] = 
{
	//NTSC
	"NTSC"
};

const char NTSC_TrCn[] = 
{
	//NTSC
	"NTSC"
};

const char *NTSC[LANGUAGE_CNT] = 
{
	NTSC_En, 
	NTSC_SiCn, 
	NTSC_TrCn
};

//--------------------------------------------------------------------------//

const char SECAM_En[] = 
{
	//SECAM
	"SECAM"
};

const char SECAM_SiCn[] = 
{
	//SECAM
	"SECAM"
};

const char SECAM_TrCn[] = 
{
	//SECAM
	"SECAM"
};

const char *SECAM[LANGUAGE_CNT] = 
{
	SECAM_En, 
	SECAM_SiCn, 
	SECAM_TrCn
};

//--------------------------------------------------------------------------//

const char Hour24_En[] = 
{
	//24Hour
	"24Hour"
};

const char Hour24_SiCn[] = 
{
	//24Сʱ
	"24"
	GUI_UC_START
	"\x5C\x0F\x65\xF6"
	GUI_UC_END
	"\x0"
};

const char Hour24rCn[] = 
{
	//24С�r
	"24"
	GUI_UC_START
	"\x5C\x0F\x66\x42"
	GUI_UC_END
	"\x0"
};

const char *Hour24[LANGUAGE_CNT] = 
{
	Hour24_En, 
	Hour24_SiCn, 
	Hour24rCn
};

//--------------------------------------------------------------------------//

const char Hour12_En[] = 
{
	//12Hour
	"12Hour"
};

const char Hour12_SiCn[] = 
{
	//12Сʱ
	"12"
	GUI_UC_START
	"\x5C\x0F\x65\xF6"
	GUI_UC_END
	"\x0"
};

const char Hour12_TrCn[] = 
{
	//12С�r
	"12"
	GUI_UC_START
	"\x5C\x0F\x66\x42"
	GUI_UC_END
	"\x0"
};

const char *Hour12[LANGUAGE_CNT] = 
{
	Hour12_En, 
	Hour12_SiCn, 
	Hour12_TrCn
};

//--------------------------------------------------------------------------//

const char Year_En[] = 
{
	//Year
	"Year"
};

const char Year_SiCn[] = 
{
	//��
	GUI_UC_START
	"\x5E\x74"
	GUI_UC_END
	"\x0"
};

const char Year_TrCn[] = 
{
	//��
	GUI_UC_START
	"\x5E\x74"
	GUI_UC_END
	"\x0"
};

const char *Year[LANGUAGE_CNT] = 
{
	Year_En, 
	Year_SiCn, 
	Year_TrCn
};

//--------------------------------------------------------------------------//

const char Month_En[] = 
{
	//Month
	"Month"
};

const char Month_SiCn[] = 
{
	//��
	GUI_UC_START
	"\x67\x08"
	GUI_UC_END
	"\x0"
};

const char Month_TrCn[] = 
{
	//��
	GUI_UC_START
	"\x67\x08"
	GUI_UC_END
	"\x0"
};

const char *Month[LANGUAGE_CNT] = 
{
	Month_En, 
	Month_SiCn, 
	Month_TrCn
};

//--------------------------------------------------------------------------//

const char Day_En[] = 
{
	//Day
	"Day"
};

const char Day_SiCn[] = 
{
	//��
	GUI_UC_START
	"\x65\xE5"
	GUI_UC_END
	"\x0"
};

const char Day_TrCn[] = 
{
	//��
	GUI_UC_START
	"\x65\xE5"
	GUI_UC_END
	"\x0"
};

const char *Day[LANGUAGE_CNT] = 
{
	Day_En, 
	Day_SiCn, 
	Day_TrCn
};

//--------------------------------------------------------------------------//

const char Hour_En[] = 
{
	//Hour
	"Hour"
};

const char Hour_SiCn[] = 
{
	//Сʱ
	GUI_UC_START
	"\x5C\x0F\x65\xF6"
	GUI_UC_END
	"\x0"
};

const char Hour_TrCn[] = 
{
	//С�r
	GUI_UC_START
	"\x5C\x0F\x66\x42"
	GUI_UC_END
	"\x0"
};

const char *Hour[LANGUAGE_CNT] = 
{
	Hour_En, 
	Hour_SiCn, 
	Hour_TrCn
};

//--------------------------------------------------------------------------//

const char Minute_En[] = 
{
	//Minute
	"Minute"
};

const char Minute_SiCn[] = 
{
	//����
	GUI_UC_START
	"\x52\x06\x94\x9F"
	GUI_UC_END
	"\x0"
};

const char Minute_TrCn[] = 
{
	//��� 
	GUI_UC_START
	"\x52\x06\x94\x18"
	GUI_UC_END
	"\x0"
};

const char *Minute[LANGUAGE_CNT] = 
{
	Minute_En, 
	Minute_SiCn, 
	Minute_TrCn
};

//--------------------------------------------------------------------------//

const char Second_En[] = 
{
	//Second
	"Second"
};

const char Second_SiCn[] = 
{
	//��
	GUI_UC_START
	"\x79\xD2"
	GUI_UC_END
	"\x0"
};

const char Second_TrCn[] = 
{
	//��
	GUI_UC_START
	"\x79\xD2"
	GUI_UC_END
	"\x0"
};

const char *Second[LANGUAGE_CNT] = 
{
	Second_En, 
	Second_SiCn, 
	Second_TrCn
};

//--------------------------------------------------------------------------//

const char HDD_En[] = 
{
	//Hard disk
	"Hard disk"
};

const char HDD_SiCn[] = 
{
	//Ӳ��
	GUI_UC_START
	"\x78\x6C\x76\xD8"
	GUI_UC_END
	"\x0"
};

const char HDD_TrCn[] = 
{
	//Ӳ��
	GUI_UC_START
	"\x78\x6C\x78\x9F"
	GUI_UC_END
	"\x0"
};

const char *HDD[LANGUAGE_CNT] = 
{
	HDD_En, 
	HDD_SiCn, 
	HDD_TrCn
};

//--------------------------------------------------------------------------//

const char SM_En[] = 
{
	//SM card
	"SM card"
};

const char SM_SiCn[] = 
{
	//SM��
	"SM "
	GUI_UC_START
	"\x53\x61"
	GUI_UC_END
	"\x0"
};

const char SM_TrCn[] = 
{
	//SM��
	"SM "
	GUI_UC_START
	"\x53\x61"
	GUI_UC_END
	"\x0"
};

const char *SM[LANGUAGE_CNT] = 
{
	SM_En, 
	SM_SiCn, 
	SM_TrCn
};

//--------------------------------------------------------------------------//

const char MS_En[] = 
{
	//MS card
	"MS card"
};

const char MS_SiCn[] = 
{
	//MS��
	"MS "
	GUI_UC_START
	"\x53\x61"
	GUI_UC_END
	"\x0"
};

const char MS_TrCn[] = 
{
	//MS��
	"MS "
	GUI_UC_START
	"\x53\x61"
	GUI_UC_END
	"\x0"
};

const char *MS[LANGUAGE_CNT] = 
{
	MS_En, 
	MS_SiCn, 
	MS_TrCn
};

//--------------------------------------------------------------------------//

const char SD_En[] = 
{
	//SD card
	"SD card"
};

const char SD_SiCn[] = 
{
	//SD��
	"SD "
	GUI_UC_START
	"\x53\x61"
	GUI_UC_END
	"\x0"
};

const char SD_TrCn[] = 
{
	//SD��
	"SD "
	GUI_UC_START
	"\x53\x61"
	GUI_UC_END
	"\x0"
};

const char *SD[LANGUAGE_CNT] = 
{
	SD_En, 
	SD_SiCn, 
	SD_TrCn
};

//--------------------------------------------------------------------------//

const char CF_En[] = 
{
	//CF card
	"CF card"
};

const char CF_SiCn[] = 
{
    //zqh 2015��3��19�� modify,����CF������Ϊ�������̡�
    //������
	GUI_UC_START
    "\x75\x35\x5b\x50\x76\xd8"
	GUI_UC_END
	"\x0"
};

const char CF_TrCn[] = 
{
    //zqh 2015��3��19�� modify������CF������Ϊ����ӱP��
    //��ӱP
	GUI_UC_START
    "\x96\xfb\x5b\x50\x76\xe4"
	GUI_UC_END
	"\x0"
};

const char *CF[LANGUAGE_CNT] = 
{
	CF_En, 
	CF_SiCn, 
	CF_TrCn
};

//--------------------------------------------------------------------------//

const char HDDInfo_En[] = 
{
	//Hard disk info
	"Hard disk info"
};

const char HDDInfo_SiCn[] = 
{
	//Ӳ����Ϣ
	GUI_UC_START
	"\x78\x6C\x76\xD8\x4F\xE1\x60\x6F"
	GUI_UC_END
	"\x0"
};

const char HDDInfo_TrCn[] = 
{
	//Ӳ���YӍ
	GUI_UC_START
	"\x78\x6C\x78\x9F\x8C\xC7\x8A\x0A"
	GUI_UC_END
	"\x0"
};

const char *HDDInfo[LANGUAGE_CNT] = 
{
	HDDInfo_En, 
	HDDInfo_SiCn, 
	HDDInfo_TrCn
};

//--------------------------------------------------------------------------//

const char SMInfo_En[] = 
{
	//SM card info
	"SM card info"
};

const char SMInfo_SiCn[] = 
{
	//SM����Ϣ
	"SM"
	GUI_UC_START
	"\x53\x61\x4F\xE1\x60\x6F"
	GUI_UC_END
	"\x0"
};

const char SMInfo_TrCn[] = 
{
	//SM���YӍ
	"SM "
	GUI_UC_START
	"\x53\x61\x8C\xC7\x8A\x0A"
	GUI_UC_END
	"\x0"
};

const char *SMInfo[LANGUAGE_CNT] = 
{
	SMInfo_En, 
	SMInfo_SiCn, 
	SMInfo_TrCn
};

//--------------------------------------------------------------------------//

const char MSInfo_En[] = 
{
	//MS card info
	"MS card info"
};

const char MSInfo_SiCn[] = 
{
	//MS����Ϣ
	"MS"
	GUI_UC_START
	"\x53\x61\x4F\xE1\x60\x6F"
	GUI_UC_END
	"\x0"
};

const char MSInfo_TrCn[] = 
{
	//MS���YӍ
	"MS "
	GUI_UC_START
	"\x53\x61\x8C\xC7\x8A\x0A"
	GUI_UC_END
	"\x0"
};

const char *MSInfo[LANGUAGE_CNT] = 
{
	MSInfo_En, 
	MSInfo_SiCn, 
	MSInfo_TrCn
};

//--------------------------------------------------------------------------//

const char SDInfo_En[] = 
{
	//SD card info
	"SD card info"
};

const char SDInfo_SiCn[] = 
{
	//SD����Ϣ
	"SD"
	GUI_UC_START
	"\x53\x61\x4F\xE1\x60\x6F"
	GUI_UC_END
	"\x0"
};

const char SDInfo_TrCn[] = 
{
	//SD���YӍ
	"SD "
	GUI_UC_START
	"\x53\x61\x8C\xC7\x8A\x0A"
	GUI_UC_END
	"\x0"
};

const char *SDInfo[LANGUAGE_CNT] = 
{
	SDInfo_En, 
	SDInfo_SiCn, 
	SDInfo_TrCn
};

//--------------------------------------------------------------------------//

const char CFInfo_En[] = 
{
	//CF card info
	"CF card info"
};

const char CFInfo_SiCn[] = 
{
    //zqh 2015��3��19�� modify
    //�����̿���Ϣ
	GUI_UC_START
    "\x75\x35\x5b\x50\x76\xd8\x4F\xE1\x60\x6F"
	GUI_UC_END
	"\x0"
};

const char CFInfo_TrCn[] = 
{
    //zqh 2015��3��19�� modify
    //��ӱP�YӍ
	GUI_UC_START
    "\x96\xfb\x5b\x50\x76\xe4\x8c\xc7\x8A\x0A"
	GUI_UC_END
	"\x0"
};

const char *CFInfo[LANGUAGE_CNT] = 
{
	CFInfo_En, 
	CFInfo_SiCn, 
	CFInfo_TrCn
};

//--------------------------------------------------------------------------//

const char TotalSpace_En[] = 
{
	//Total space:
	"Total space:"
};

const char TotalSpace_SiCn[] = 
{
	//�ܿռ䣺
	GUI_UC_START
	"\x60\x3B\x7A\x7A\x95\xF4\xFF\x1A"
	GUI_UC_END
	"\x0"
};

const char TotalSpace_TrCn[] = 
{
	//�����g��
	GUI_UC_START
	"\x7E\x3D\x7A\x7A\x95\x93\xFF\x1A"
	GUI_UC_END
	"\x0"
};

const char *TotalSpace[LANGUAGE_CNT] = 
{
	TotalSpace_En, 
	TotalSpace_SiCn, 
	TotalSpace_TrCn
};

//--------------------------------------------------------------------------//

const char UsedSpace_En[] = 
{
	//Used space:
	"Used space:"
};

const char UsedSpace_SiCn[] = 
{
	//ռ�ÿռ䣺
	GUI_UC_START
	"\x53\x60\x75\x28\x7A\x7A\x95\xF4\xFF\x1A"
	GUI_UC_END
	"\x0"
};

const char UsedSpace_TrCn[] = 
{
	//���ÿ��g��
	GUI_UC_START
	"\x4F\x54\x75\x28\x7A\x7A\x95\x93\xFF\x1A"
	GUI_UC_END
	"\x0"
};

const char *UsedSpace[LANGUAGE_CNT] = 
{
	UsedSpace_En, 
	UsedSpace_SiCn, 
	UsedSpace_TrCn
};

//--------------------------------------------------------------------------//

const char FreeSpace_En[] = 
{
	//Free space:
	"Free space:"
};

const char FreeSpace_SiCn[] = 
{
	//ʣ��ռ䣺
	GUI_UC_START
	"\x52\x69\x4F\x59\x7A\x7A\x95\xF4\xFF\x1A"
	GUI_UC_END
	"\x0"
};

const char FreeSpace_TrCn[] = 
{
	//ʣ�N���g��
	GUI_UC_START
	"\x52\x69v99\x18\x7A\x7A\x95\x93\xFF\x1A"
	GUI_UC_END
	"\x0"
};

const char *FreeSpace[LANGUAGE_CNT] = 
{
	FreeSpace_En, 
	FreeSpace_SiCn, 
	FreeSpace_TrCn
};

//--------------------------------------------------------------------------//

const char Number_En[] = 
{
	//No.
	"No."
};

const char Number_SiCn[] = 
{
	//��
	GUI_UC_START
	"\x5E\x8F"
	GUI_UC_END
	"\x0"
};

const char Number_TrCn[] = 
{
	//��
	GUI_UC_START
	"\x5E\x8F"
	GUI_UC_END
	"\x0"
};

const char *Number[LANGUAGE_CNT] = 
{
	Number_En, 
	Number_SiCn, 
	Number_TrCn
};

//--------------------------------------------------------------------------//

const char Source_En[] = 
{
	//Source
	"Source"
};

const char Source_SiCn[] = 
{
	//Դͨ��
	GUI_UC_START
	"\x6E\x90\x90\x1A\x90\x53"
	GUI_UC_END
	"\x0"
};

const char Source_TrCn[] = 
{
	//Դͨ��
	GUI_UC_START
	"\x6E\x90\x90\x1A\x90\x53"
	GUI_UC_END
	"\x0"
};

const char *Source[LANGUAGE_CNT] = 
{
	Source_En, 
	Source_SiCn, 
	Source_TrCn
};

//--------------------------------------------------------------------------//

const char Qual_En[] = 
{
	//Qual
	"Qual"
};

const char Qual_SiCn[] = 
{
	//����
	GUI_UC_START
	"\x8D\x28\x91\xCF"
	GUI_UC_END
	"\x0"
};

const char Qual_TrCn[] = 
{
	//Ʒ�|
	GUI_UC_START
	"\x54\xC1\x8C\xEA"
	GUI_UC_END
	"\x0"
};

const char *Qual[LANGUAGE_CNT] = 
{
	Qual_En, 
	Qual_SiCn, 
	Qual_TrCn
};

//--------------------------------------------------------------------------//

const char Quality_En[] = 
{
	//Quality
	"Quality"
};

const char Quality_SiCn[] = 
{
	//����
	GUI_UC_START
	"\x8D\x28\x91\xCF"
	GUI_UC_END
	"\x0"
};

const char Quality_TrCn[] = 
{
	//Ʒ�|
	GUI_UC_START
	"\x54\xC1\x8C\xEA"
	GUI_UC_END
	"\x0"
};

const char *Quality[LANGUAGE_CNT] = 
{
	Quality_En, 
	Quality_SiCn, 
	Quality_TrCn
};

//--------------------------------------------------------------------------//

const char Date_En[] = 
{
	//Date
	"Date"
};

const char Date_SiCn[] = 
{
	//����
	GUI_UC_START
	"\x65\xE5\x67\x1F"
	GUI_UC_END
	"\x0"
};

const char Date_TrCn[] = 
{
	//����
	GUI_UC_START
	"\x65\xE5\x67\x1F"
	GUI_UC_END
	"\x0"
};

const char *Date[LANGUAGE_CNT] = 
{
	Date_En, 
	Date_SiCn, 
	Date_TrCn
};

//--------------------------------------------------------------------------//

const char Start_En[] = 
{
	//Start
	"Start"
};

const char Start_SiCn[] = 
{
	//��ʼ
	GUI_UC_START
	"\x5F\x00\x59\xCB"
	GUI_UC_END
	"\x0"
};

const char Start_TrCn[] = 
{
	//�_ʼ 
	GUI_UC_START
	"\x95\x8B\x59\xCB"
	GUI_UC_END
	"\x0"
};

const char *Start[LANGUAGE_CNT] = 
{
	Start_En, 
	Start_SiCn, 
	Start_TrCn
};

//--------------------------------------------------------------------------//

const char End_En[] = 
{
	//End
	"End"
};

const char End_SiCn[] = 
{
	//����
	GUI_UC_START
	"\x7E\xD3\x67\x5F"
	GUI_UC_END
	"\x0"
};

const char End_TrCn[] = 
{
	//�Y��
	GUI_UC_START
	"\x7D\x50\x67\x5F"
	GUI_UC_END
	"\x0"
};

const char *End[LANGUAGE_CNT] = 
{
	End_En, 
	End_SiCn, 
	End_TrCn
};

//--------------------------------------------------------------------------//

const char RecordTo_En[] = 
{
	//Rec To
	"RecTo"
};

const char RecordTo_SiCn[] = 
{
	//¼��
	GUI_UC_START
	"\x5F\x55\x52\x30"
	GUI_UC_END
	"\x0"
};

const char RecordTo_TrCn[] = 
{
	//䛵�
	GUI_UC_START
	"\x93\x04\x52\x30"
	GUI_UC_END
	"\x0"
};

const char *RecordTo[LANGUAGE_CNT] = 
{
	RecordTo_En, 
	RecordTo_SiCn, 
	RecordTo_TrCn
};

//--------------------------------------------------------------------------//

const char Mode_En[] = 
{
	//Mode
	"Mode"
};

const char Mode_SiCn[] = 
{
	//ģʽ
	GUI_UC_START
	"\x6A\x21\x5F\x0F"
	GUI_UC_END
	"\x0"
};

const char Mode_TrCn[] = 
{
	//ģʽ
	GUI_UC_START
	"\x6A\x21\x5F\x0F"
	GUI_UC_END
	"\x0"
};

const char *Mode[LANGUAGE_CNT] = 
{
	Mode_En, 
	Mode_SiCn, 
	Mode_TrCn
};

//--------------------------------------------------------------------------//

const char Status_En[] = 
{
	//State
	"State"
};

const char Status_SiCn[] = 
{
	//״̬
	GUI_UC_START
	"\x72\xB6\x60\x01"
	GUI_UC_END
	"\x0"
};

const char Status_TrCn[] = 
{
	//��B
	GUI_UC_START
	"\x72\xC0\x61\x4B"
	GUI_UC_END
	"\x0"
};

const char *Status[LANGUAGE_CNT] = 
{
	Status_En, 
	Status_SiCn, 
	Status_TrCn
};

//--------------------------------------------------------------------------//

const char ScheduleDetails_En[] = 
{
	//Schedule details
	"Schedule details"
};

const char ScheduleDetails_SiCn[] = 
{
	//¼������
	GUI_UC_START
	"\x5F\x55\x52\x36\x8B\xE6\x60\xC5"
	GUI_UC_END
	"\x0"
};

const char ScheduleDetails_TrCn[] = 
{
	//��uԔ�� 
	GUI_UC_START
	"\x93\x04\x88\xFD\x8A\x73\x60\xC5"
	GUI_UC_END
	"\x0"
};

const char *ScheduleDetails[LANGUAGE_CNT] = 
{
	ScheduleDetails_En, 
	ScheduleDetails_SiCn, 
	ScheduleDetails_TrCn
};
//--------------------------------------------------------------------------//
const char Title_En[] = 
{
	//Title
	"Title"
};

const char Title_SiCn[] = 
{
	//����
	GUI_UC_START
	"\x68\x07\x98\x98"
	GUI_UC_END
	"\x0"
};

const char Title_TrCn[] = 
{
	//���}
	GUI_UC_START
	"\x6A\x19\x98\x4C"
	GUI_UC_END
	"\x0"
};

const char *Title[LANGUAGE_CNT] = 
{
	Title_En, 
	Title_SiCn, 
	Title_TrCn
};

//--------------------------------------------------------------------------//
const char AutoMarkerTime_En[] = 
{
	//Auto Marker
	"Auto Marker"
};

const char AutoMarkerTime_SiCn[] = 
{
	//�Զ��½ڻ���
	GUI_UC_START
	"\x7A\xE0\x82\x82\x81\xEA\x52\xA8\x52\x12\x52\x06"
	GUI_UC_END
	"\x0"
};

const char AutoMarkerTime_TrCn[] = 
{
	//
	GUI_UC_START
	"\x7A\xE0\x7B\xC0\x81\xEA\x52\xD5\x52\x83\x52\x06"
	GUI_UC_END
	"\x0"
};

const char *AutoMarkerTime[LANGUAGE_CNT] = 
{
	AutoMarkerTime_En, 
	AutoMarkerTime_SiCn, 
	AutoMarkerTime_TrCn
};

//--------------------------------------------------------------------------//
const char NicamMode_En[] = 
{
	//NicamMode
	"NicamMode"
};

const char NicamMode_SiCn[] = 
{
	//����ģʽ
	GUI_UC_START
	"\x58\xF0\x90\x53\x6A\x21\x5F\x0F"
	GUI_UC_END
	"\x0"
};

const char NicamMode_TrCn[] = 
{
	//��ģʽ
	GUI_UC_START
	"\x80\x72\x90\x53\x6A\x21\x5F\x0F"
	GUI_UC_END
	"\x0"
};

const char *NicamMode[LANGUAGE_CNT] = 
{
	NicamMode_En, 
	NicamMode_SiCn, 
	NicamMode_TrCn
};

//--------------------------------------------------------------------------//
const char FileFormat_En[] = 
{
	//FileFormat
	"FileFormat"
};

const char FileFormat_SiCn[] = 
{
	//�ļ���ʽ
	GUI_UC_START
	"\x65\x87\x4E\xF6\x68\x3C\x5F\x0F"
	GUI_UC_END
	"\x0"
};

const char FileFormat_TrCn[] = 
{
	//�n��ʽ
	GUI_UC_START
	"\x6A\x94\x68\x3C\x5F\x0F"
	GUI_UC_END
	"\x0"
};

const char *FileFormat[LANGUAGE_CNT] = 
{
	FileFormat_En, 
	FileFormat_SiCn, 
	FileFormat_TrCn
};

//--------------------------------------------------------------------------//
const char Resolution_En[] = 
{
	//Resolution
	"Resolution"
};

const char Resolution_SiCn[] = 
{
	//�ֱ���
	GUI_UC_START
	"\x52\x06\x8F\xA8\x73\x87"
	GUI_UC_END
	"\x0"
};

const char Resolution_TrCn[] = 
{
	//������
	GUI_UC_START
	"\x89\xE3\x67\x90\x5E\xA6"
	GUI_UC_END
	"\x0"
};

const char *Resolution[LANGUAGE_CNT] = 
{
	Resolution_En, 
	Resolution_SiCn, 
	Resolution_TrCn
};

//--------------------------------------------------------------------------//

const char Enable_En[] = 
{
	//Enable
	"Enable"
};

const char Enable_SiCn[] = 
{
	//��Ч
	GUI_UC_START
	"\x67\x09\x65\x48"
	GUI_UC_END
	"\x0"
};

const char Enable_TrCn[] = 
{
	//��Ч
	GUI_UC_START
	"\x67\x09\x65\x48"
	GUI_UC_END
	"\x0"
};

const char *Enable[LANGUAGE_CNT] = 
{
	Enable_En, 
	Enable_SiCn, 
	Enable_TrCn
};

//--------------------------------------------------------------------------//

const char Disable_En[] = 
{
	//Disable
	"Disable"
};

const char Disable_SiCn[] = 
{
	//��Ч
	GUI_UC_START
	"\x65\xE0\x65\x48"
	GUI_UC_END
	"\x0"
};

const char Disable_TrCn[] = 
{
	//�oЧ 
	GUI_UC_START
	"\x71\x21\x65\x48"
	GUI_UC_END
	"\x0"
};

const char *Disable[LANGUAGE_CNT] = 
{
	Disable_En, 
	Disable_SiCn, 
	Disable_TrCn
};

//--------------------------------------------------------------------------//

const char Delete_En[] = 
{
	//Delete File
	"Delete File"
};

const char Delete_SiCn[] = 
{
	//ɾ����(6587)��(4EF6)
	GUI_UC_START
	"\x52\x20\x96\x64\x65\x87\x4e\xf6"
	GUI_UC_END
	"\x0"
};

const char Delete_TrCn[] = 
{
	//�h����(6587)�n(6A94)
	GUI_UC_START
	"\x52\x2A\x96\x64\x65\x87\x6a\x94"
	GUI_UC_END
	"\x0"
};

const char *Delete[LANGUAGE_CNT] = 
{
	Delete_En, 
	Delete_SiCn,
	Delete_TrCn
};
const char DeleteAllFile_En[]=
{
	//delete all files
	"Delete All Files"
};
const char DeleteAllFile_SiCn[]=
{
	//ɾ(5220)��(9664)��(6240)��(6709)��(6587)��(4EF6)
	GUI_UC_START
	"\x52\x20\x96\x64\x62\x40\x67\x09\x65\x87\x4E\xF6"
	GUI_UC_END
	"\x0"
};
const char DeleteAllFile_TrCn[]=
{
	//�h(522A)��(9664)��(6240)��(6709)��(6587)��(4EF6)
	GUI_UC_START
	"\x52\x2A\x96\x64\x62\x40\x67\x09\x65\x87\x4E\xF6"
	GUI_UC_END
	"\x0"
};
const char *DeleteAllFile[LANGUAGE_CNT]=
{
	DeleteAllFile_En,
	DeleteAllFile_SiCn,
	DeleteAllFile_TrCn
};  //haiqing
	
//--------------------------------------------------------------------------//

const char StartTime_En[] = 
{
	//Start time
	"Start time"
};

const char StartTime_SiCn[] = 
{
	//��ʼʱ��
	GUI_UC_START
	"\x5F\x00\x59\xCB\x65\xF6\x95\xF4"
	GUI_UC_END
	"\x0"
};

const char StartTime_TrCn[] = 
{
	//�_ʼ �r�g
	GUI_UC_START
	"\x95\x8B\x59\xCB\x66\x42\x95\x93"
	GUI_UC_END
	"\x0"
};

const char *StartTime[LANGUAGE_CNT] = 
{
	StartTime_En, 
	StartTime_SiCn, 
	StartTime_TrCn
};

//--------------------------------------------------------------------------//

const char EndTime_En[] = 
{
	//End time
	"End time"
};

const char EndTime_SiCn[] = 
{
	//����ʱ��
	GUI_UC_START
	"\x7E\xD3\x67\x5F\x65\xF6\x95\xF4"
	GUI_UC_END
	"\x0"
};

const char EndTime_TrCn[] = 
{
	//�Y���r�g
	GUI_UC_START
	"\x7D\x50\x67\x5F\x66\x42\x95\x93"
	GUI_UC_END
	"\x0"
};

const char *EndTime[LANGUAGE_CNT] = 
{
	EndTime_En, 
	EndTime_SiCn, 
	EndTime_TrCn
};

//--------------------------------------------------------------------------//

const char Weekly_En[] = 
{
	//Weekly
	"Weekly"
};

const char Weekly_SiCn[] = 
{
	//ÿ��
	GUI_UC_START
	"\x6B\xCF\x54\x68"
	GUI_UC_END
	"\x0"
};

const char Weekly_TrCn[] = 
{
	//ÿ�L 
	GUI_UC_START
	"\x6B\xCF\x90\x31"
	GUI_UC_END
	"\x0"
};

const char *Weekly[LANGUAGE_CNT] = 
{
	Weekly_En, 
	Weekly_SiCn, 
	Weekly_TrCn
};

//--------------------------------------------------------------------------//

const char Daily_En[] = 
{
	//Daily
	"Daily"
};

const char Daily_SiCn[] = 
{
	//ÿ��
	GUI_UC_START
	"\x6B\xCF\x59\x29"
	GUI_UC_END
	"\x0"
};

const char Daily_TrCn[] = 
{
	//ÿ��
	GUI_UC_START
	"\x6B\xCF\x59\x29"
	GUI_UC_END
	"\x0"
};

const char *Daily[LANGUAGE_CNT] = 
{
	Daily_En, 
	Daily_SiCn, 
	Daily_TrCn
};

//--------------------------------------------------------------------------//

const char Once_En[] = 
{
	//Once
	"Once"
};

const char Once_SiCn[] = 
{
	//��һ��
	GUI_UC_START
	"\x4E\xC5\x4E\x00\x6B\x21"
	GUI_UC_END
	"\x0"
};

const char Once_TrCn[] = 
{
	//�Hһ��
	GUI_UC_START
	"\x50\xC5\x4E\x00\x6B\x21"
	GUI_UC_END
	"\x0"
};

const char *Once[LANGUAGE_CNT] = 
{
	Once_En, 
	Once_SiCn, 
	Once_TrCn
};

//--------------------------------------------------------------------------//

const char StateEn_En[] = 
{
	//En
	"En"
};

const char StateEn_SiCn[] = 
{
	//��Ч
	GUI_UC_START
	"\x67\x09\x65\x48"
	GUI_UC_END
	"\x0"
};

const char StateEn_TrCn[] = 
{
	//��Ч
	GUI_UC_START
	"\x67\x09\x65\x48"
	GUI_UC_END
	"\x0"
};

const char *StateEn[LANGUAGE_CNT] = 
{
	StateEn_En, 
	StateEn_SiCn, 
	StateEn_TrCn
};

//--------------------------------------------------------------------------//

const char StateDis_En[] = 
{
	//Dis
	"Dis"
};

const char StateDis_SiCn[] = 
{
	//��Ч
	GUI_UC_START
	"\x65\xE0\x65\x48"
	GUI_UC_END
	"\x0"
};

const char StateDis_TrCn[] = 
{
	//�oЧ 
	GUI_UC_START
	"\x71\x21\x65\x48"
	GUI_UC_END
	"\x0"
};

const char *StateDis[LANGUAGE_CNT] = 
{
	StateDis_En, 
	StateDis_SiCn, 
	StateDis_TrCn
};

//--------------------------------------------------------------------------//

const char StateDoing_En[] = 
{
	//Doing
	"Doing"
};

const char StateDoing_SiCn[] = 
{
	//¼��
	GUI_UC_START
	"\x5F\x55\x52\x36"
	GUI_UC_END
	"\x0"
};

const char StateDoing_TrCn[] = 
{
	//��u
	GUI_UC_START
	"\x93\x04\x88\xFD"
	GUI_UC_END
	"\x0"
};

const char *StateDoing[LANGUAGE_CNT] = 
{
	StateDoing_En, 
	StateDoing_SiCn, 
	StateDoing_TrCn
};

//--------------------------------------------------------------------------//

const char StateDone_En[] = 
{
	//Done
	"Done"
};

const char StateDone_SiCn[] = 
{
	//¼��
	GUI_UC_START
	"\x5F\x55\x5B\x8C"
	GUI_UC_END
	"\x0"
};

const char StateDone_TrCn[] = 
{
	//���
	GUI_UC_START
	"\x93\x04\x5B\x8C"
	GUI_UC_END
	"\x0"
};

const char *StateDone[LANGUAGE_CNT] = 
{
	StateDone_En, 
	StateDone_SiCn, 
	StateDone_TrCn
};

//--------------------------------------------------------------------------//

const char Message_En[] = 
{
	//Message
	"Message"
};

const char Message_SiCn[] = 
{
	//��ʾ��Ϣ
	GUI_UC_START
	"\x63\xD0\x79\x3A\x4F\xE1\x60\x6F"
	GUI_UC_END
	"\x0"
};

const char Message_TrCn[] = 
{
	//��ʾ�YӍ
	GUI_UC_START
	"\x63\xD0\x79\x3A\x8C\xC7\x8A\x0A"
	GUI_UC_END
	"\x0"
};

const char *Message[LANGUAGE_CNT] = 
{
	Message_En, 
	Message_SiCn, 
	Message_TrCn
};
//-------------------------------------------------------------------------//
//haiqing add
const char UDiskLinkFail_EN[]=
{
     //mount failed
     "Mount UDisk failed"
};
const char UDiskLinkFail_SiCn[]=
{
      //U��(76D8)��(8FDE)��(63A5)ʧ(5931)��(8D25)
      "U"
      GUI_UC_START
      "\x76\xD8\x8F\xDE\x63\xA5\x59\x31\x8D\x25"
      GUI_UC_END
      "\x0"
};
const char UDiskLinkFail_TrCn[]=
{
      //U�P(76E4)�B(9023)��(63A5)ʧ(5931)��(6557)
      "U"
      GUI_UC_START
      "\x76\xE4\x90\x23\x63\xA5\x59\x31\x65\x57"
      GUI_UC_END
      "\x0"
};
const char *UDiskLinkFail[LANGUAGE_CNT]=
{
      UDiskLinkFail_EN,
      UDiskLinkFail_SiCn,
      UDiskLinkFail_TrCn
};
//--------------------------------------------------------------------//
//haiqing add
const char UDiskBreakFail_EN[]=
{
      //Unmount UDisk failed
      "Unmount UDisk failed!"
};
const char UDiskBreakFail_SiCn[]=
{
      //U��(76D8)��(65AD)��(5F00)ʧ(5931)��(8D25)��
      "U"
      GUI_UC_START
      "\x76\xD8\x65\xAD\x5F\x00\x59\x31\x8D\x25\XFF\X01"
      GUI_UC_END
      "\x0"
};
const char UDiskBreakFail_TrCn[]=
{
       //U�P(76E4)��(65B7)�_(958B)ʧ(5931)��(6557)!
       "U"
       GUI_UC_START
       "\x76\xE4\x65\xB7\x95\x8B\x59\x31\x65\x57\xff\x01"
       GUI_UC_END
       "\x0"
};
const char *UDiskBreakFail[LANGUAGE_CNT]=
{
       UDiskBreakFail_EN,
       UDiskBreakFail_SiCn,
       UDiskBreakFail_TrCn
};
//--------------------------------------------------------------------------//

const char TextInput_En[] = 
{
	//Text input
	"Text input"
};

const char TextInput_SiCn[] = 
{
	//�ı�����
	GUI_UC_START
	"\x65\x87\x67\x2C\x8F\x93\x51\x65"
	GUI_UC_END
	"\x0"
};

const char TextInput_TrCn[] = 
{
	//�ı�ݔ��
	GUI_UC_START
	"\x65\x87\x67\x2C\x8F\x38\x51\x65"
	GUI_UC_END
	"\x0"
};

const char *TextInput[LANGUAGE_CNT] = 
{
	TextInput_En, 
	TextInput_SiCn, 
	TextInput_TrCn
};

//--------------------------------------------------------------------------//

const char Input_En[] = 
{
	//Please input:
	"Please input:"
};

const char Input_SiCn[] = 
{
	//������:
	GUI_UC_START
	"\x8B\xF7\x8F\x93\x51\x65"
	GUI_UC_END
	":"
	"\x0"
};

const char Input_TrCn[] = 
{
	//Ոݔ��:
	GUI_UC_START
	"\x8A\xCB\x8F\x38\x51\x65"
	GUI_UC_END
	":"
	"\x0"
};

const char *Input[LANGUAGE_CNT] = 
{
	Input_En, 
	Input_SiCn, 
	Input_TrCn
};

//--------------------------------------------------------------------------//

const char FreeCharacter_En[] = 
{
	//Free count:
	"Free count:"
};

const char FreeCharacter_SiCn[] = 
{
	//ʣ���ַ�����
	GUI_UC_START
	"\x52\x69\x4F\x59\x5B\x57\x7B\x26\x65\x70\xFF\x1A"
	GUI_UC_END
	"\x0"
};

const char FreeCharacter_TrCn[] = 
{
	//ʣ�N��Ԫ����
	GUI_UC_START
	"\x52\x69\x99\x18\x5B\x57\x51\x43\x65\x78\xFF\x1A"
	GUI_UC_END
	"\x0"
};

const char *FreeCharacter[LANGUAGE_CNT] = 
{
	FreeCharacter_En, 
	FreeCharacter_SiCn, 
	FreeCharacter_TrCn
};

//--------------------------------------------------------------------------//

const char Wait_En[] = 
{
	//Please wait...
	"Please wait..."
};

const char Wait_SiCn[] = 
{
	//���Ժ�...
	GUI_UC_START
	"\x8B\xF7\x7A\x0D\x50\x19"
	GUI_UC_END
	"..."
	"\x0"
};

const char Wait_TrCn[] = 
{
	//Ո�Ժ�...
	GUI_UC_START
	"\x8A\xCB\x7A\x0D\x50\x19"
	GUI_UC_END
	"..."
	"\x0"
};

const char *Wait[LANGUAGE_CNT] = 
{
	Wait_En, 
	Wait_SiCn, 
	Wait_TrCn
};

//--------------------------------------------------------------------------//

const char FileBrowser_HDD_En[] = 
{
	//File browser - HDD
	"File browser"
};

const char FileBrowser_HDD_SiCn[] = 
{
	//�ļ���� - HDD
	GUI_UC_START
	"\x65\x87\x4E\xF6\x6D\x4F\x89\xC8"
	GUI_UC_END
	"\x0"
};

const char FileBrowser_HDD_TrCn[] = 
{
	//�n���[ - HDD
	GUI_UC_START
	"\x6A\x94\x6D\x41\x89\xBD"
	GUI_UC_END
	"\x0"
};

const char *FileBrowser_HDD[LANGUAGE_CNT] = 
{
	FileBrowser_HDD_En, 
	FileBrowser_HDD_SiCn, 
	FileBrowser_HDD_TrCn
};

//--------------------------------------------------------------------------//

const char FileBrowser_SM_En[] = 
{
	//File browser - SM
	"File browser - SM"
};

const char FileBrowser_SM_SiCn[] = 
{
	//�ļ����- SM
	GUI_UC_START
	"\x65\x87\x4E\xF6\x6D\x4F\x89\xC8"
	GUI_UC_END
	" - SM"
	"\x0"
};

const char FileBrowser_SM_TrCn[] = 
{
	//�n���[ - SM
	GUI_UC_START
	"\x6A\x94\x6D\x41\x89\xBD"
	GUI_UC_END
	" - SM"
	"\x0"
};

const char *FileBrowser_SM[LANGUAGE_CNT] = 
{
	FileBrowser_SM_En, 
	FileBrowser_SM_SiCn, 
	FileBrowser_SM_TrCn
};

//--------------------------------------------------------------------------//

const char FileBrowser_MS_En[] = 
{
	//File browser - MS
	"File browser - MS"
};

const char FileBrowser_MS_SiCn[] = 
{
	//�ļ���� - MS
	GUI_UC_START
	"\x65\x87\x4E\xF6\x6D\x4F\x89\xC8"
	GUI_UC_END
	" - MS"
	"\x0"
};

const char FileBrowser_MS_TrCn[] = 
{
	//�n���[ - MS
	GUI_UC_START
	"\x6A\x94\x6D\x41\x89\xBD"
	GUI_UC_END
	" - MS"
	"\x0"
};

const char *FileBrowser_MS[LANGUAGE_CNT] = 
{
	FileBrowser_MS_En, 
	FileBrowser_MS_SiCn, 
	FileBrowser_MS_TrCn
};

//--------------------------------------------------------------------------//

const char FileBrowser_SD_En[] = 
{
	//File browser - SD
	"File browser - SD"
};

const char FileBrowser_SD_SiCn[] = 
{
	//�ļ���� - SD
	GUI_UC_START
	"\x65\x87\x4E\xF6\x6D\x4F\x89\xC8"
	GUI_UC_END
	" - SD"
	"\x0"
};

const char FileBrowser_SD_TrCn[] = 
{
	//�n���[ - SD
	GUI_UC_START
	"\x6A\x94\x6D\x41\x89\xBD"
	GUI_UC_END
	" - SD"
	"\x0"
};

const char *FileBrowser_SD[LANGUAGE_CNT] = 
{
	FileBrowser_SD_En, 
	FileBrowser_SD_SiCn, 
	FileBrowser_SD_TrCn
};

//--------------------------------------------------------------------------//
//2015��5��19�գ�zqh modify����" - CF"���� - �����̡�ȥ��
const char FileBrowser_CF_En[] = 
{
    //File browser
    "File Browser"
};

const char FileBrowser_CF_SiCn[] = 
{
    //zqh 2015��3��19�� modify
    //�ļ���� - ������
	GUI_UC_START
	"\x65\x87\x4E\xF6\x6D\x4F\x89\xC8"
	GUI_UC_END

    #if 0
    " - "
    GUI_UC_START
    "\x75\x35\x5b\x50\x76\xd8" //"������"
    GUI_UC_END
    #endif

    "\x0"
};

const char FileBrowser_CF_TrCn[] = 
{
    //zqh 2015��3��19�� modify
    //�n�����[ - ��ӱP
	GUI_UC_START
	"\x6A\x94\x6D\x41\x89\xBD"
	GUI_UC_END

    #if 0
    " - "
    GUI_UC_START
    "\x96\xfb\x5b\x50\x76\xe4" //"��ӱP"
    GUI_UC_END
    #endif

    "\x0"
};

const char *FileBrowser_CF[LANGUAGE_CNT] = 
{
	FileBrowser_CF_En, 
	FileBrowser_CF_SiCn, 
	FileBrowser_CF_TrCn
};

//--------------------------------------------------------------------------//

const char FileOperation_En[] = 
{
	//File operation
    "File Operation"
};

const char FileOperation_SiCn[] = 
{
	//�ļ�����
	GUI_UC_START
	"\x65\x87\x4E\xF6\x64\xCD\x4F\x5C"
	GUI_UC_END
	"\x0"
};

const char FileOperation_TrCn[] = 
{
    //�ęn����
	GUI_UC_START
    "\x65\x87\x6A\x94\x64\xCD\x4F\x5C"
	GUI_UC_END
	"\x0"
};

const char *FileOperation[LANGUAGE_CNT] = 
{
	FileOperation_En, 
	FileOperation_SiCn, 
	FileOperation_TrCn
};

//--------------------------------------------------------------------------//

const char Play_En[] = 
{
	//PlayFile
	"Play File"
};

const char Play_SiCn[] = 
{
	//������(6587)��(4EF6)
	GUI_UC_START
	"\x64\xAD\x65\x3E\x65\x87\x4e\xf6"
	GUI_UC_END
	"\x0"
};

const char Play_TrCn[] = 
{
	//������(6587)�n(6A94)
	GUI_UC_START
	"\x64\xAD\x65\x3E\x65\x87\x6a\x94"
	GUI_UC_END
	"\x0"
};

const char *Play[LANGUAGE_CNT] = 
{
	Play_En, 
	Play_SiCn, 
	Play_TrCn
};

//--------------------------------------------------------------------------//

const char Copy_En[] = 
{
	//Copy
	"Copy File"
};

const char Copy_SiCn[] = 
{
    //zqh 2015��3��24�� modify�����������ļ�����Ϊ�������ļ���
    //������(6587)��(4EF6)
	GUI_UC_START
    "\x59\x0D\x52\x36\x65\x87\x4e\xf6"
	GUI_UC_END
	"\x0"
};

const char Copy_TrCn[] = 
{
    //zqh 2015��3��24�� modify��������ؐ�ęn���Ğ顰�}�u�ęn��
    //�}�u��(6587)�n(6A94)
	GUI_UC_START
    "\x89\x07\x88\xFD\x65\x87\x6a\x94"
	GUI_UC_END
	"\x0"
};

const char *Copy[LANGUAGE_CNT] = 
{
	Copy_En, 
	Copy_SiCn, 
	Copy_TrCn
};

//--------------------------------------------------------------------------//

const char FileRename_En[] = 
{
	//Rename
	"Rename"
};

const char FileRename_SiCn[] = 
{
	//������
	GUI_UC_START
	"\x91\xCD\x54\x7D\x54\x0D"
	GUI_UC_END
	"\x0"
};

const char FileRename_TrCn[] = 
{
	//������
	GUI_UC_START
	"\x91\xCD\x54\x7D\x54\x0D"
	GUI_UC_END
	"\x0"
};

const char *FileRename[LANGUAGE_CNT] = 
{
	FileRename_En, 
	FileRename_SiCn, 
	FileRename_TrCn
};

//--------------------------------------------------------------------------//

const char Convert_En[] = 
{
	//Convert
	"Convert"
};

const char Convert_SiCn[] = 
{
	//ת��
	GUI_UC_START
	"\x8F\x6C\x63\x62"
	GUI_UC_END
	"\x0"
};

const char Convert_TrCn[] = 
{
	//�D�Q
	GUI_UC_START
	"\x8F\x49\x63\xDB"
	GUI_UC_END
	"\x0"
};

const char *Convert[LANGUAGE_CNT] = 
{
	Convert_En, 
	Convert_SiCn, 
	Convert_TrCn
};

//--------------------------------------------------------------------------//

const char Sort_En[] = 
{
	//Sort by
	"Sort by"
};

const char Sort_SiCn[] = 
{
	//����
	GUI_UC_START
	"\x63\x92\x5E\x8F"
	GUI_UC_END
};

const char Sort_TrCn[] = 
{
	//����
	GUI_UC_START
	"\x63\x92\x5E\x8F"
	GUI_UC_END
};

const char *Sort[LANGUAGE_CNT] = 
{
	Sort_En, 
	Sort_SiCn, 
	Sort_TrCn
};

//--------------------------------------------------------------------------//

const char FileName_En[] = 
{
    //File Name
    "File Name"
};

const char FileName_SiCn[] = 
{
	//�ļ���
	GUI_UC_START
	"\x65\x87\x4E\xF6\x54\x0D"
	GUI_UC_END
	"\x0"
};

const char FileName_TrCn[] = 
{
	//�n����
	GUI_UC_START
	"\x6A\x94\x68\x48\x54\x0D"
	GUI_UC_END
	"\x0"
};

const char *FileName[LANGUAGE_CNT] = 
{
	FileName_En, 
	FileName_SiCn, 
	FileName_TrCn
};

//--------------------------------------------------------------------------//

const char FrameRate_En[] = 
{
	//Frame rate
	"FPS"
};

const char FrameRate_SiCn[] = 
{
	//֡��
	GUI_UC_START
	"\x5E\x27\x73\x87"
	GUI_UC_END
	"\x0"
};

const char FrameRate_TrCn[] = 
{
	//����
	GUI_UC_START
	"\x5E\x40\x73\x87"
	GUI_UC_END
	"\x0"
};

const char *FrameRate[LANGUAGE_CNT] = 
{
	FrameRate_En, 
	FrameRate_SiCn, 
	FrameRate_TrCn
};
//--------------------------------------------------------------------------//
const char RecordInfo_En[] = 
{
	//Format
	"Format"
};

const char RecordInfo_SiCn[] = 
{
	//��ʽ
	GUI_UC_START
	"\x68\x3C\x5F\x0F"
	GUI_UC_END
	"\x0"
};

const char RecordInfo_TrCn[] = 
{
	//��ʽ
	GUI_UC_START
	"\x68\x3C\x5F\x0F"
	GUI_UC_END
	"\x0"
};

const char *RecordInfo[LANGUAGE_CNT] = 
{
	RecordInfo_En, 
	RecordInfo_SiCn, 
	RecordInfo_TrCn
};

//--------------------------------------------------------------------------//

const char Duration_En[] = 
{
	//Duration
	"Duration"
};

const char Duration_SiCn[] = 
{
	//ʱ��
	GUI_UC_START
	"\x65\xF6\x95\x7F"
	GUI_UC_END
	"\x0"
};

const char Duration_TrCn[] = 
{
	//�r�L
	GUI_UC_START
	"\x66\x42\x95\x77"
	GUI_UC_END
	"\x0"
};

const char *Duration[LANGUAGE_CNT] = 
{
	Duration_En, 
	Duration_SiCn, 
	Duration_TrCn
};

//--------------------------------------------------------------------------//

const char Size_En[] = 
{
	//Size
	"Size"
};

const char Size_SiCn[] = 
{
	//��С
	GUI_UC_START
	"\x59\x27\x5C\x0F"
	GUI_UC_END
	"\x0"
};

const char Size_TrCn[] = 
{
	//��С
	GUI_UC_START
	"\x59\x27\x5C\x0F"
	GUI_UC_END
	""
};

const char *Size[LANGUAGE_CNT] = 
{
	Size_En, 
	Size_SiCn, 
	Size_TrCn
};

//--------------------------------------------------------------------------//

const char Page_En[] = 
{
	//Page
	"Page"
};

const char Page_SiCn[] = 
{
	//ҳ
	GUI_UC_START
	"\x98\x75"
	GUI_UC_END
	"\x0"
};

const char Page_TrCn[] = 
{
	//�
	GUI_UC_START
	"\x98\x01"
	GUI_UC_END
	"\x0"
};

const char *Page[LANGUAGE_CNT] = 
{
	Page_En, 
	Page_SiCn, 
	Page_TrCn
};

//--------------------------------------------------------------------------//

const char Time_En[] = 
{
	//Time
	"Time"
};

const char Time_SiCn[] = 
{
	//ʱ��
	GUI_UC_START
	"\x65\xF6\x95\xF4"
	GUI_UC_END
	"\x0"
};

const char Time_TrCn[] = 
{
	//�r�g
	GUI_UC_START
	"\x66\x42\x95\x93"
	GUI_UC_END
	"\x0"
};

const char *Time[LANGUAGE_CNT] = 
{
	Time_En, 
	Time_SiCn, 
	Time_TrCn
};

//--------------------------------------------------------------------------//

const char Name_En[] = 
{
	//Name
	"Name"
};

const char Name_SiCn[] = 
{
	//����
	GUI_UC_START
	"\x54\x0D\x79\xF0"
	GUI_UC_END
	"\x0"
};

const char Name_TrCn[] = 
{
	//���Q
	GUI_UC_START
	"\x54\x0D\x7A\x31"
	GUI_UC_END
	"\x0"
};

const char *Name[LANGUAGE_CNT] = 
{
	Name_En, 
	Name_SiCn, 
	Name_TrCn
};

//--------------------------------------------------------------------------//

const char Type_En[] = 
{
	//Type
	"Type"
};

const char Type_SiCn[] = 
{
	//����
	GUI_UC_START
	"\x7C\x7B\x57\x8B"
	GUI_UC_END
	"\x0"
};

const char Type_TrCn[] = 
{
	//���
	GUI_UC_START
	"\x98\x5E\x57\x8B"
	GUI_UC_END
	"\x0"
};

const char *Type[LANGUAGE_CNT] = 
{
	Type_En, 
	Type_SiCn, 
	Type_TrCn
};

//--------------------------------------------------------------------------//

const char Default_En[] = 
{
	//Default
	"Default"
};

const char Default_SiCn[] = 
{
	//Ĭ��
	GUI_UC_START
	"\x9E\xD8\x8B\xA4"
	GUI_UC_END
	"\x0"
};

const char Default_TrCn[] = 
{
	//Ĭ�J 
	GUI_UC_START
	"\x9E\xD8\x8A\x8D"
	GUI_UC_END
	"\x0"
};

const char *Default[LANGUAGE_CNT] = 
{
	Default_En, 
	Default_SiCn, 
	Default_TrCn
};

//--------------------------------------------------------------------------//

const char PSP_En[] = 
{
	//PSP
	"PSP"
};

const char PSP_SiCn[] = 
{
	//PSP
	"PSP"
};

const char PSP_TrCn[] = 
{
	//PSP
	"PSP"
};

const char *PSP[LANGUAGE_CNT] = 
{
	PSP_En, 
	PSP_SiCn, 
	PSP_TrCn
};

//--------------------------------------------------------------------------//

const char PMP_En[] = 
{
	//PMP
	"PMP"
};

const char PMP_SiCn[] = 
{
	//PMP
	"PMP"
};

const char PMP_TrCn[] = 
{
	//PMP
	"PMP"
};

const char *PMP[LANGUAGE_CNT] = 
{
	PMP_En, 
	PMP_SiCn, 
	PMP_TrCn
};

//--------------------------------------------------------------------------//

const char IPOD_En[] = 
{
	//iPOD
	"iPod"
};

const char IPOD_SiCn[] = 
{
	//iPOD
	"iPod"
};

const char IPOD_TrCn[] = 
{
	//iPOD
	"iPod"
};

const char *IPOD[LANGUAGE_CNT] = 
{
	IPOD_En, 
	IPOD_SiCn, 
	IPOD_TrCn
};

//--------------------------------------------------------------------------//

const char PocketPC_En[] = 
{
	//Pocket PC
	"Pocket PC"
};

const char PocketPC_SiCn[] = 
{
	//Pocket PC
	"Pocket PC"
};

const char PocketPC_TrCn[] = 
{
	//Pocket PC
	"Pocket PC"
};

const char *PocketPC[LANGUAGE_CNT] = 
{
	PocketPC_En, 
	PocketPC_SiCn, 
	PocketPC_TrCn
};

//--------------------------------------------------------------------------//

const char DCam_En[] = 
{
	//D Cam
	"D Cam"
};

const char DCam_SiCn[] = 
{
	//D Cam
	"D Cam"
};

const char DCam_TrCn[] = 
{
	//D Cam
	"D Cam"
};

const char *DCam[LANGUAGE_CNT] = 
{
	DCam_En, 
	DCam_SiCn, 
	DCam_TrCn
};

//--------------------------------------------------------------------------//

const char Mobile_En[] = 
{
	//Mobile
	"Mobile"
};

const char Mobile_SiCn[] = 
{
	//Mobile
	"Mobile"
};

const char Mobile_TrCn[] = 
{
	//Mobile
	"Mobile"
};

const char *Mobile[LANGUAGE_CNT] = 
{
	Mobile_En, 
	Mobile_SiCn, 
	Mobile_TrCn
};

//--------------------------------------------------------------------------//

const char GotoTimePlay_En[] = 
{
	//"Set time to play"
	"Set time to play"
};

const char GotoTimePlay_SiCn[] = 
{
	//ָ��ʱ�䲥��
	GUI_UC_START
	"\x63\x07\x5B\x9A\x65\xF6\x95\xF4\x64\xAD\x65\x3E"
	GUI_UC_END
	"\x0"
};

const char GotoTimePlay_TrCn[] = 
{
	//ָ���r�g����
	GUI_UC_START
	"\x63\x07\x5B\x9A\x66\x42\x95\x93\x64\xAD\x65\x3E"
	GUI_UC_END
	"\x0"
};

const char *GotoTimePlay[LANGUAGE_CNT] = 
{
	GotoTimePlay_En, 
	GotoTimePlay_SiCn, 
	GotoTimePlay_TrCn
};

//--------------------------------------------------------------------------//

const char MovieLen_En[] = 
{
	//Movie length:
	"Movie length:"
};

const char MovieLen_SiCn[] = 
{
	//ӰƬ����
	GUI_UC_START
	"\x5F\x71\x72\x47\x95\x7F\x5E\xA6"
	GUI_UC_END
	"\x0"
};

const char MovieLen_TrCn[] = 
{
	//ӰƬ�L��
	GUI_UC_START
	"\x5F\x71\x72\x47\x95\x77\x5E\xA6"
	GUI_UC_END
	"\x0"
};

const char *MovieLen[LANGUAGE_CNT] = 
{
	MovieLen_En, 
	MovieLen_SiCn, 
	MovieLen_TrCn
};

//--------------------------------------------------------------------------//

const char ScanWait_En[] = 
{
	//System is scanning for channels, please wait...
	"System is scanning for channels, please wait..."
};

const char ScanWait_SiCn[] = 
{
	//������̨����ȴ�...
	GUI_UC_START
	"\x6B\x63\x57\x28\x64\x1C\x53\xF0\xFF\x0C\x8B\xF7\x7B\x49\x5F\x85"
	GUI_UC_END
	"..."
	"\x0"
};

const char ScanWait_TrCn[] = 
{
	//������̨��Ո�ȴ�...
	GUI_UC_START
	"\x6B\x63\x57\x28\x64\x1C\x53\xF0\xFF\x0C\x8A\xCB\x7B\x49\x5F\x85"
	GUI_UC_END
	"..."
	"\x0"
};

const char *ScanWait[LANGUAGE_CNT] = 
{
	ScanWait_En, 
	ScanWait_SiCn, 
	ScanWait_TrCn
};

//--------------------------------------------------------------------------//

const char ProgramNum_En[] = 
{
	//Program num
	"Program num"
};

const char ProgramNum_SiCn[] = 
{
	//��Ŀ���
	GUI_UC_START
	"\x82\x82\x76\xEE\x5E\x8F\x53\xF7"
	GUI_UC_END
	"\x0"
};

const char ProgramNum_TrCn[] = 
{
	//��Ŀ��̖
	GUI_UC_START
	"\x7B\xC0\x76\xEE\x5E\x8F\x86\x5F"
	GUI_UC_END
	"\x0"
};

const char *ProgramNum[LANGUAGE_CNT] = 
{
	ProgramNum_En, 
	ProgramNum_SiCn, 
	ProgramNum_TrCn
};

//--------------------------------------------------------------------------//

const char ChannelName_En[] = 
{
	//Channel name
	"Channel name"
};

const char ChannelName_SiCn[] = 
{
	//Ƶ������
	GUI_UC_START
	"\x98\x91\x90\x53\x54\x0D\x79\xF0"
	GUI_UC_END
	"\x0"
};

const char ChannelName_TrCn[] = 
{
	//�l�����Q
	GUI_UC_START
	"\x98\x3B\x90\x53\x54\x0D\x7A\x31"
	GUI_UC_END
	"\x0"
};

const char *ChannelName[LANGUAGE_CNT] = 
{
	ChannelName_En, 
	ChannelName_SiCn, 
	ChannelName_TrCn
};

//--------------------------------------------------------------------------//

const char Band_En[] = 
{
	//Band
	"Band"
};

const char Band_SiCn[] = 
{
	//Ƶ��
	GUI_UC_START
	"\x98\x91\x6B\xB5"
	GUI_UC_END
	"\x0"
};

const char Band_TrCn[] = 
{
	//�l��
	GUI_UC_START
	"\x98\x3B\x6B\xB5"
	GUI_UC_END
	"\x0"
};

const char *Band[LANGUAGE_CNT] = 
{
	Band_En, 
	Band_SiCn, 
	Band_TrCn
};

//--------------------------------------------------------------------------//

const char Search_En[] = 
{
	//Search
	"Search"
};

const char Search_SiCn[] = 
{
	//����
	GUI_UC_START
	"\x64\x1C\x7D\x22"
	GUI_UC_END
	"\x0"
};

const char Search_TrCn[] = 
{
	//����
	GUI_UC_START
	"\x64\x1C\x7D\x22"
	GUI_UC_END
	"\x0"
};

const char *Search[LANGUAGE_CNT] = 
{
	Search_En, 
	Search_SiCn, 
	Search_TrCn
};

//--------------------------------------------------------------------------//

const char FineTuning_En[] = 
{
	//Fine tuning
	"Fine tuning"
};

const char FineTuning_SiCn[] = 
{
	//Ƶ��΢��
	GUI_UC_START
	"\x98\x91\x90\x53\x5F\xAE\x8C\x03"
	GUI_UC_END
	"\x0"
};

const char FineTuning_TrCn[] = 
{
	//�l��΢�{
	GUI_UC_START
	"\x98\x3B\x90\x53\x5F\xAE\x8A\xBF"
	GUI_UC_END
	"\x0"
};

const char *FineTuning[LANGUAGE_CNT] = 
{
	FineTuning_En, 
	FineTuning_SiCn, 
	FineTuning_TrCn
};

//--------------------------------------------------------------------------//

const char Skip_En[] = 
{
	//Skip
	"Skip"
};

const char Skip_SiCn[] = 
{
	//����
	GUI_UC_START
	"\x8D\xF3\x8F\xC7"
	GUI_UC_END
	"\x0"
};

const char Skip_TrCn[] = 
{
	//���^
	GUI_UC_START
	"\x8D\xF3\x90\x4E"
	GUI_UC_END
	"\x0"
};

const char *Skip[LANGUAGE_CNT] = 
{
	Skip_En, 
	Skip_SiCn, 
	Skip_TrCn
};

//--------------------------------------------------------------------------//

const char GotoTime_En[] = 
{
	//Goto:
	"Goto:"
};

const char GotoTime_SiCn[] = 
{
	//��ת��:
	GUI_UC_START
	"\x8D\xF3\x8F\x6C\x52\x30"
	GUI_UC_END
	":\x0"
};

const char GotoTime_TrCn[] = 
{
	//���D��:
	GUI_UC_START
	"\x8D\xF3\x8F\x49\x52\x30"
	GUI_UC_END
	":\x0"
};

const char *GotoTime[LANGUAGE_CNT] = 
{
	GotoTime_En, 
	GotoTime_SiCn, 
	GotoTime_TrCn
};

//--------------------------------------------------------------------------//

const char TotalTime_En[] = 
{
	//"Total time:"
	"Total time:"
};

const char TotalTime_SiCn[] = 
{
	//��ʱ��:
	GUI_UC_START
	"\x60\x3B\x65\xF6\x95\xF4"
	GUI_UC_END
	":\x0"
};

const char TotalTime_TrCn[] = 
{
	//���r�g:
	GUI_UC_START
	"\x7E\x3D\x66\x42\x95\x93"
	GUI_UC_END
	":\x0"
};

const char *TotalTime[LANGUAGE_CNT] = 
{
	TotalTime_En, 
	TotalTime_SiCn, 
	TotalTime_TrCn
};

//--------------------------------------------------------------------------//

const char HelpTip0_En[] = 
{
	//""
	""
};

const char HelpTip0_SiCn[] = 
{
	//""
	""
};

const char HelpTip0_TrCn[] = 
{
	//""
	""
};

const char *HelpTip0[LANGUAGE_CNT] = 
{
	HelpTip0_En, 
	HelpTip0_SiCn, 
	HelpTip0_TrCn
};

//--------------------------------------------------------------------------//

const char HelpTip1_En[] = 
{
	//"Enter":OK;"Back":Cancel
	"\"Enter\":OK;\"Back\":Cancel"
};

const char HelpTip1_SiCn[] = 
{
	//"Enter":ȷ��;"Back":ȡ��
	"\"Enter\":"
	GUI_UC_START
	"\x78\x6E\x5B\x9A"
	GUI_UC_END
	";\"Back\":"
	GUI_UC_START
	"\x53\xD6\x6D\x88"
	GUI_UC_END
	"\x0"
};

const char HelpTip1_TrCn[] = 
{
	//"Enter":�_��;"Back":ȡ��
	"\"Enter\":"
	GUI_UC_START
	"\x78\xBA\x5B\x9A"
	GUI_UC_END
	";\"Back\":"
	GUI_UC_START
	"\x53\xD6\x6D\x88"
	GUI_UC_END
	"\x0"
};

const char *HelpTip1[LANGUAGE_CNT] = 
{
	HelpTip1_En, 
	HelpTip1_SiCn, 
	HelpTip1_TrCn
};

//--------------------------------------------------------------------------//

const char HelpTip2_En[] = 
{
    //Press any key to exit.
    "Press any key to exit."
};

const char HelpTip2_SiCn[] = 
{
	//��������˳���
	GUI_UC_START
	"\x63\x09\x4E\xFB\x61\x0F\x95\x2E\x90\x00\x51\xFA\x30\x02"
	GUI_UC_END
	"\x0"
};

const char HelpTip2_TrCn[] = 
{
	//�������I�˳���
	GUI_UC_START
	"\x63\x09\x4E\xFB\x61\x0F\x93\x75\x90\x00\x51\xFA\x30\x02"
	GUI_UC_END
	"\x0"
};

const char *HelpTip2[LANGUAGE_CNT] = 
{
	HelpTip2_En, 
	HelpTip2_SiCn, 
	HelpTip2_TrCn
};

//--------------------------------------------------------------------------//

const char HelpTip3_En[] = 
{
	//"Up/Down":Select one schedule;"Enter":Enter setup menu.
	"\"Up/Down\":Select one schedule;\"Enter\":Enter setup menu."
};

const char HelpTip3_SiCn[] = 
{
	//"Up/Down":ѡ��һ���ƻ���"Enter":�������ý��档
	"\"Up/Down\":"
	GUI_UC_START
	"\x90\x09\x62\xE9\x4E\x00\x4E\x2A\x8B\xA1\x52\x12\xFF\x1B"
	GUI_UC_END
	"\"Enter\":"
	GUI_UC_START
	"\x8F\xDB\x51\x65\x8B\xBE\x7F\x6E\x75\x4C\x97\x62\x30\x02"
	GUI_UC_END
	"\x0"
};

const char HelpTip3_TrCn[] = 
{
	//"Up/Down":�x��һ��Ӌ����"Enter":�M���O�ý��档
	"\"Up/Down\":"
	GUI_UC_START
	"\x90\x78\x64\xC7\x4E\x00\x50\x0B\x8A\x08\x75\x6B\xFF\x1B"
	GUI_UC_END
	"\"Enter\":"
	GUI_UC_START
	"\x90\x32\x51\x65\x8A\x2D\x7F\x6E\x4E\xCB\x97\x62\x30\x02"
	GUI_UC_END
	"\x0"
};

const char *HelpTip3[LANGUAGE_CNT] = 
{
	HelpTip3_En, 
	HelpTip3_SiCn, 
	HelpTip3_TrCn
};

//--------------------------------------------------------------------------//

const char HelpTip4_En[] = 
{
	//"Enter":OK;"Back":Cancel;\n"<":Backspace;">":Clear.
	"\"Enter\":OK;\"Back\":Cancel;\n\"<\":Backspace;\">\":Clear."
};

const char HelpTip4_SiCn[] = 
{
	//"Enter":ȷ��;"Back":ȡ��;\n"<":�˸�;">":���.
	"\"Enter\":"
	GUI_UC_START
	"\x78\x6E\x5B\x9A"
	GUI_UC_END
	";\"Back\":"
	GUI_UC_START
	"\x53\xD6\x6D\x88"
	GUI_UC_END
	";\n\"<\":"
	GUI_UC_START
	"\x90\x00\x68\x3C"
	GUI_UC_END
	";\">\":"
	GUI_UC_START
	"\x6E\x05\x7A\x7A"
	GUI_UC_END
	"."
	"\x0"
};

const char HelpTip4_TrCn[] = 
{
	//"Enter":�_��;"Back":ȡ��;\n"<":�˸�;">":���.
	"\"Enter\":"
	GUI_UC_START
	"\x78\xBA\x5B\x9A"
	GUI_UC_END
	";\"Back\":"
	GUI_UC_START
	"\x53\xD6\x6D\x88"
	GUI_UC_END
	";\n\"<\":"
	GUI_UC_START
	"\x90\x00\x68\x3C"
	GUI_UC_END
	";\">\":"
	GUI_UC_START
	"\x6E\x05\x7A\x7A"
	GUI_UC_END
	"."
	"\x0"
};

const char *HelpTip4[LANGUAGE_CNT] = 
{
	HelpTip4_En, 
	HelpTip4_SiCn, 
	HelpTip4_TrCn
};

//--------------------------------------------------------------------------//

const char HelpTip5_En[] = 
{
	//"Up/Down":Select one file;"Enter":Enter operation menu.
	"\""
	GUI_UC_START
	"\x25\xB2"
	GUI_UC_END
	"/"
	GUI_UC_START
	"\x25\xBC"
	GUI_UC_END
	"\""
	":Select one file; \"Enter\":Enter operation menu."
};

const char HelpTip5_SiCn[] = 
{
    //"Up/Down":ѡ��һ���ļ���"Enter":����������档
	//"\'Up/Down':"
	GUI_UC_START
	"\x25\xB2\x25\xBC\x90\x09\x62\xE9\x4E\x00\x4E\x2A\x65\x87\x4E\xF6\xFF\x1B"
	GUI_UC_END
	"\'Enter\':"
	GUI_UC_START
	"\x8F\xDB\x51\x65\x64\xCD\x4F\x5C\x75\x4C\x97\x62\x30\x02"
	GUI_UC_END
	"\x0"
};

const char HelpTip5_TrCn[] = 
{
	//"Up/Down":�x��һ���n��"Enter":�M��������档
	"\"Up/Down\":"
	GUI_UC_START
	"\x90\x78\x64\xC7\x4E\x00\x50\x0B\x6A\x94\xFF\x1B"
	GUI_UC_END
	"\"Enter\":"
	GUI_UC_START
	"\x90\x32\x51\x65\x64\xCD\x4F\x5C\x4E\xCB\x97\x62\x30\x02"
	GUI_UC_END
	"\x0"
};

const char *HelpTip5[LANGUAGE_CNT] = 
{
	HelpTip5_En, 
	HelpTip5_SiCn, 
	HelpTip5_TrCn
};

//--------------------------------------------------------------------------//

const char HelpTip6_En[] = 
{
	//"Enter:"YES;"Other Keys":NO
	"\"Enter\":YES;\"Other Keys\":NO"
};

const char HelpTip6_SiCn[] = 
{
	//"Enter:"��;"Other Keys":��
	"\"Enter\":"
	GUI_UC_START
	"\x66\x2F"
	GUI_UC_END
	"\"Other Keys\":"
	GUI_UC_START
	"\x54\x26"
	GUI_UC_END
	"\x0"
};

const char HelpTip6_TrCn[] = 
{
	//"Enter:"��;"Other Keys":��
	"\"Enter\":"
	GUI_UC_START
	"\x66\x2F"
	GUI_UC_END
	"\"Other Keys\":"
	GUI_UC_START
	"\x54\x26"
	GUI_UC_END
	"\x0"
};

const char *HelpTip6[LANGUAGE_CNT] = 
{
	HelpTip6_En, 
	HelpTip6_SiCn, 
	HelpTip6_TrCn
};

//zqh 2015��3��27�� add
//--------------------------------------------------------------------------//
//�� ESCKEY_SiCn �˳�
const char HelpTip7_En[] =
{
    //Press Esc Key To exit.
    "Press\"Esc\"key to exit."
};

const char HelpTip7_SiCn[] =
{
    //��(6309) ESCKEY_SiCn ��(9000)��(51fa)��
    GUI_UC_START
    "\x63\x09"//��
    GUI_UC_END
    "\""
    GUI_UC_START
    ESCKEY_SiCn//�˳�
    GUI_UC_END
    "\""
    GUI_UC_START
    "\x95\x2e"//��
    GUI_UC_END

    GUI_UC_START
    "\x90\x00\x51\xfa\x30\x02"//�˳���
    GUI_UC_END
    "\x0"
};

const char HelpTip7_TrCn[] =
{
    //��(6309) ESCKEY_TrCn ��(9000)��(51fa)��
    GUI_UC_START
    "\x63\x09"//��
    GUI_UC_END
    "\""
    GUI_UC_START
    ESCKEY_TrCn//�˳�
    GUI_UC_END
    "\""
    GUI_UC_START
    "\x93\x75"//�I
    GUI_UC_END

    GUI_UC_START
    "\x90\x00\x51\xfa\x30\x02"//�˳���
    GUI_UC_END
    "\x0"
};

const char *HelpTip7[LANGUAGE_CNT] =
{
    HelpTip7_En,
    HelpTip7_SiCn,
    HelpTip7_TrCn
};


//zqh 2015��3��27�� add
//--------------------------------------------------------------------------//
//�ǣ���ȷ�������񣺰��˳���
const char HelpTip8_En[] =
{
    //Yes:press\"Enter\"key;No:press\"Esc\"key.
    "Yes:press\"Enter\"key;No:press\"Esc\"key."
};

const char HelpTip8_SiCn[] =
{
    //��(662f)����ENTERKEY_SiCn����(5426)����ESCKEY_SiCn��
    GUI_UC_START
    "\x66\x2f"//��
    GUI_UC_END
    ":"

    GUI_UC_START
    "\x63\x09"//��
    GUI_UC_END
    "\""
    GUI_UC_START
    ENTERKEY_SiCn//ȷ��
    GUI_UC_END
    "\""
    GUI_UC_START
    "\x95\x2e"//��
    GUI_UC_END
    ";"

    GUI_UC_START
    "\x54\x26"//��
    GUI_UC_END
    ":"

    GUI_UC_START
    "\x63\x09"//��
    GUI_UC_END
    "\""
    GUI_UC_START
    ESCKEY_SiCn//�˳�
    GUI_UC_END
    "\""
    GUI_UC_START
    "\x95\x2e\x30\x02"//����
    GUI_UC_END
    "\x0"
};

const char HelpTip8_TrCn[] =
{
    //��(662f)����ENTERKEY_TrCn����(5426)����ESCKEY_TrCn��
    GUI_UC_START
    "\x66\x2f"//��
    GUI_UC_END
    ":"

    GUI_UC_START
    "\x63\x09"//��
    GUI_UC_END
    "\""
    GUI_UC_START
    ENTERKEY_TrCn//ȷ��
    GUI_UC_END
    "\""
    GUI_UC_START
    "\x93\x75"//�I
    GUI_UC_END
    ";"

    GUI_UC_START
    "\x54\x26"//��
    GUI_UC_END
    ":"

    GUI_UC_START
    "\x63\x09"//��
    GUI_UC_END
    "\""
    GUI_UC_START
    ESCKEY_TrCn//�˳�
    GUI_UC_END
    "\""
    GUI_UC_START
    "\x93\x75\x30\x02"//�I��
    GUI_UC_END
    "\x0"
};

const char *HelpTip8[LANGUAGE_CNT] =
{
    HelpTip8_En,
    HelpTip8_SiCn,
    HelpTip8_TrCn
};


//zqh 2015��3��27�� add
//--------------------------------------------------------------------------//
//�� ����� ����ϵͳ�����˵�(\u8fdb\u5165\u7cfb\u7edf\u5347\u7ea7\u83dc\u5355)
const char HelpTip9_En[] =
{
    //Press any key to enter the system update menu.
    "Press any key to enter system update menu."
};

const char HelpTip9_SiCn[] =
{
    //�����������ϵͳ�����˵���
    GUI_UC_START
    "\x63\x09"//��
    GUI_UC_END
    GUI_UC_START
    "\x4E\xFB\x61\x0F\x95\x2E"//�����
    GUI_UC_END
    GUI_UC_START
    "\x8f\xdb\x51\x65\x7c\xfb\x7e\xdf\x53\x47\x7e\xa7\x83\xdc\x53\x55\x30\x02"//����ϵͳ�����˵���
    GUI_UC_END
    "\x0"
};

const char HelpTip9_TrCn[] =
{
    //��(6309)�����I�M��ϵ�y�����ˆ�(\x90\x32\x51\x65\x7c\xfb\x7d\x71\x53\x47\x7d\x1a\x83\xdc\x55\xae)��
    GUI_UC_START
    "\x63\x09"//��
    GUI_UC_END
    GUI_UC_START
    "\x4E\xFB\x61\x0F\x93\x75"//�����I
    GUI_UC_END
    GUI_UC_START
    "\x90\x32\x51\x65\x7c\xfb\x7d\x71\x53\x47\x7d\x1a\x83\xdc\x55\xae\x30\x02"//�M��ϵ�y�����ˆΡ�
    GUI_UC_END
    "\x0"
};

const char *HelpTip9[LANGUAGE_CNT] =
{
    HelpTip9_En,
    HelpTip9_SiCn,
    HelpTip9_TrCn
};


//zqh 2015��3��27�� add
//zqh 2015��5��21�� modify���������£�����ϵͳ���ԡ���Ϊ����ȷ�ϡ�������ϵͳ����.��
//--------------------------------------------------------------------------//
//up/down:set system  language
const char HelpTip10_En[] =
{
    //Press\"Enter\"key to set system language.
    "Press\"Enter\"key to set system language."
};

const char HelpTip10_SiCn[] =
{
    //����ȷ�ϡ�������ϵͳ����(\x8b\xbe\x7f\x6e\x7c\xfb\x7e\xdf\x8b\xed\x8a\x00)��
    //�����˳������˳�
    GUI_UC_START
    "\x63\x09"//��
    GUI_UC_END
    "\""
    GUI_UC_START
    ENTERKEY_SiCn//ȷ��
    GUI_UC_END
    "\""
    GUI_UC_START
    "\x95\x2e\x8b\xbe\x7f\x6e\x7c\xfb\x7e\xdf\x8b\xed\x8a\x00\x30\x02"//������ϵͳ���ԡ�
    GUI_UC_END
    "\x0"
};

const char HelpTip10_TrCn[] =
{
    //"�_�J"�I�O��ϵ�y�Z��(\x8a2d\x7f6e\x7cfb\x7d71\x8a9e\x8a00)��
    GUI_UC_START
    "\x63\x09"//��
    GUI_UC_END
    "\""
    GUI_UC_START
    ENTERKEY_TrCn//�_�J
    GUI_UC_END
    "\""
    GUI_UC_START
    "\x93\x75\x8a\x2d\x7f\x6e\x7c\xfb\x7d\x71\x8a\x9e\x8a\x00\x30\x02"//�I�O��ϵ�y�Z�ԡ�
    GUI_UC_END
    "\x0"
};

const char *HelpTip10[LANGUAGE_CNT] =
{
    HelpTip10_En,
    HelpTip10_SiCn,
    HelpTip10_TrCn
};

//zqh 2015��4��1�� add���޸���������ʾ��Ϣ
//--------------------------------------------------------------------------//
//��ʾ��Ϣ��??\??:�ļ�ǰ��ҳ;¼ȡ:"�ļ����\U������"���л�;����:ѡ���ļ�
//�޸�Ϊ����ʾ��������¼�������ڡ��ļ�������롰U�����ӡ����й��ܼ��л�
//����¼�ơ���Ϊ����¼��
const char HelpTip11_En[] =
{
    //??\??:Before And After The Page;Rec:Switch Between "File Browser\UDisk connected"
    #if 0
    GUI_UC_START
    "\x25\xc0\x25\xc0"
    GUI_UC_END
    "\\"
    GUI_UC_START
    "\x25\xb6\x25\xb6"
    GUI_UC_END
    ":Before And After The Page;"
    #endif
    "Note:Press\""
    TABLEKEY_EN
    "\"key to switch between \"file browser"
    "\\"
    "U disk connect\"."
};

const char HelpTip11_SiCn[] =
{
    //��ʾ������¼ȡ�������ڡ��ļ�������롰U�����ӡ����й��ܼ��л�.
    //����¼ȡ����Ϊ����¼��
    GUI_UC_START
    "\x63\xD0\x79\x3A\xFF\x1A\x63\x09\x20\x1C"
    GUI_UC_END

    GUI_UC_START
    TABLEKEY_SiCn
    GUI_UC_END

    GUI_UC_START
    "\x20\x1D\x95\x2E\x53\xEF\x57\x28\x20\x1C\x65\x87\x4E\xF6\x6D\x4F\x89\xC8\x20\x1D\x4E\x0E\x20\x1C"
    GUI_UC_END
    "U"
    GUI_UC_START
    "\x76\xD8\x8F\xDE\x63\xA5\x20\x1D\x8F\xDB\x88\x4C\x52\x9F\x80\xFD\x95\xF4\x52\x07\x63\x62\x30\x02"
    GUI_UC_END
    "\x0"
};

const char HelpTip11_TrCn[] =
{
    //��ʾ�������ȡ���I���ڡ��ļ��g�[���c��U�P�B�ӡ��M�й����g�ГQ��
    GUI_UC_START
    "\x63\xD0\x79\x3A\xFF\x1A\x63\x09\x20\x1C"
    GUI_UC_END

    GUI_UC_START
    TABLEKEY_TrCn
    GUI_UC_END

    GUI_UC_START
    "\x20\x1D\x93\x75\x53\xEF\x57\x28\x20\x1C\x65\x87\x4E\xF6\x70\x0F\x89\xBD\x20\x1D\x82\x07\x20\x1C"
    GUI_UC_END

    "U"
    GUI_UC_START
    "\x76\xE4\x90\x23\x63\xA5\x20\x1D\x90\x32\x88\x4C\x52\x9F\x80\xFD\x95\x93\x52\x07\x63\xDB\x30\x02"
    GUI_UC_END
    "\x0"
};

const char *HelpTip11[LANGUAGE_CNT] =
{
    HelpTip11_En,
    HelpTip11_SiCn,
    HelpTip11_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox1_En[] = 
{
	//Reset all setting to default, continue?
    "Reset all setting to default, \nContinue?"
};

const char MsgBox1_SiCn[] = 
{
	//�ָ���Ĭ�����ã��Ƿ������
	GUI_UC_START
	"\x60\x62\x59\x0D\x52\x30\x9E\xD8\x8B\xA4\x8B\xBE\x7F\x6E\xFF\x0C\x66\x2F\x54\x26\x7E\xE7\x7E\xED\xFF\x1F"
	GUI_UC_END
	"\x0"
};

const char MsgBox1_TrCn[] = 
{
	//�֏͵�Ĭ�J�O�ã��Ƿ��^�m��
	GUI_UC_START
	"\x60\x62\x5F\xA9\x52\x30\x9E\xD8\x8A\x8D\x8A\x2D\x7F\x6E\xFF\x0C\x66\x2F\x54\x26\x7E\x7C\x7E\x8C\xFF\x1F"
	GUI_UC_END
	"\x0"
};

const char *MsgBox1[LANGUAGE_CNT] = 
{
	MsgBox1_En, 
	MsgBox1_SiCn, 
	MsgBox1_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox2_En[] = 
{
	//Mount HDD failed!
	"Mount HDD failed!"
};

const char MsgBox2_SiCn[] = 
{
	//�����ļ�ϵͳʧ�ܣ�
	GUI_UC_START
	"\x63\x02\x8F\x7D\x65\x87\x4E\xF6\x7C\xFB\x7E\xDF\x59\x31\x8D\x25\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char MsgBox2_TrCn[] = 
{
	//���d�nϵ�yʧ����
	GUI_UC_START
	"\x63\x9B\x8F\x09\x6A\x94\x7C\xFB\x7D\x71\x59\x31\x65\x57\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char *MsgBox2[LANGUAGE_CNT] = 
{
	MsgBox2_En, 
	MsgBox2_SiCn, 
	MsgBox2_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox3_En[] = 
{
	//Initialize HDD failed!
	"Initialize HDD failed!"
};

const char MsgBox3_SiCn[] = 
{
	//��ʼ��Ӳ��ʧ�ܣ�
	GUI_UC_START
	"\x52\x1D\x59\xCB\x53\x16\x78\x6C\x76\xD8\x59\x31\x8D\x25\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char MsgBox3_TrCn[] = 
{
	//��ʼ��Ӳ��ʧ����
	GUI_UC_START
	"\x52\x1D\x59\xCB\x53\x16\x78\x6C\x78\x9F\x59\x31\x65\x57\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char *MsgBox3[LANGUAGE_CNT] = 
{
	MsgBox3_En, 
	MsgBox3_SiCn, 
	MsgBox3_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox4_En[] = 
{
	//No device!
	"No device!"
};

const char MsgBox4_SiCn[] = 
{
	//���豸�����ڣ�
	GUI_UC_START
	"\x8B\xE5\x8B\xBE\x59\x07\x4E\x0D\x5B\x58\x57\x28\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char MsgBox4_TrCn[] = 
{
	//ԓ�O�䲻���ڣ� 
	GUI_UC_START
	"\x8A\x72\x8A\x2D\x50\x99\x4E\x0D\x5B\x58\x57\x28\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char *MsgBox4[LANGUAGE_CNT] = 
{
	MsgBox4_En, 
	MsgBox4_SiCn, 
	MsgBox4_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox5_En[] = 
{
	//Format hard disk, \ncontinue?
	"Format hard disk, \ncontinue?"
};

const char MsgBox5_SiCn[] = 
{
	//��ʽ��Ӳ�̣��Ƿ������
	GUI_UC_START
	"\x68\x3C\x5F\x0F\x53\x16\x78\x6C\x76\xD8\xFF\x0C\x66\x2F\x54\x26\x7E\xE7\x7E\xED\xFF\x1F"
	GUI_UC_END
	"\x0"
};

const char MsgBox5_TrCn[] = 
{
	//��ʽ��Ӳ�����Ƿ��^�m��
	GUI_UC_START
	"\x68\x3C\x5F\x0F\x53\x16\x78\x6C\x78\x9F\xFF\x0C\x66\x2F\x54\x26\x7E\x7C\x7E\x8C\xFF\x1F"
	GUI_UC_END
	"\x0"
};

const char *MsgBox5[LANGUAGE_CNT] = 
{
	MsgBox5_En, 
	MsgBox5_SiCn, 
	MsgBox5_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox6_En[] = 
{
	//Format SM card, \ncontinue?
	"Format SM card, \ncontinue?"
};

const char MsgBox6_SiCn[] = 
{
	//��ʽ��SM�����Ƿ������
	GUI_UC_START
	"\x68\x3C\x5F\x0F\x53\x16"
	GUI_UC_END
	"SM"
	GUI_UC_START
	"\x53\x61\xFF\x0C\x66\x2F\x54\x26\x7E\xE7\x7E\xED\xFF\x1F"
	GUI_UC_END
	"\x0"
};

const char MsgBox6_TrCn[] = 
{
	//��ʽ��SM�����Ƿ��^�m��
	GUI_UC_START
	"\x68\x3C\x5F\x0F\x53\x16"
	GUI_UC_END
	"SM"
	GUI_UC_START
	"\x53\x61\xFF\x0C\x66\x2F\x54\x26\x7E\x7C\x7E\x8C\xFF\x1F"
	GUI_UC_END
	"\x0"
};

const char *MsgBox6[LANGUAGE_CNT] = 
{
	MsgBox6_En, 
	MsgBox6_SiCn, 
	MsgBox6_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox7_En[] = 
{
	//Format MS card, \ncontinue?
	"Format MS card, \ncontinue?"
};

const char MsgBox7_SiCn[] = 
{
	//��ʽ��MS�����Ƿ������
	GUI_UC_START
	"\x68\x3C\x5F\x0F\x53\x16"
	GUI_UC_END
	"MS"
	GUI_UC_START
	"\x53\x61\xFF\x0C\x66\x2F\x54\x26\x7E\xE7\x7E\xED\xFF\x1F"
	GUI_UC_END
	"\x0"
};

const char MsgBox7_TrCn[] = 
{
	//��ʽ��MS�����Ƿ��^�m��
	GUI_UC_START
	"\x68\x3C\x5F\x0F\x53\x16"
	GUI_UC_END
	"MS"
	GUI_UC_START
	"\x53\x61\xFF\x0C\x66\x2F\x54\x26\x7E\x7C\x7E\x8C\xFF\x1F"
	GUI_UC_END
	"\x0"
};

const char *MsgBox7[LANGUAGE_CNT] = 
{
	MsgBox7_En, 
	MsgBox7_SiCn, 
	MsgBox7_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox8_En[] = 
{
	//Format SD card, \ncontinue?
	"Format SD card, \ncontinue?"
};

const char MsgBox8_SiCn[] = 
{
	//��ʽ��SD�����Ƿ������
	GUI_UC_START
	"\x68\x3C\x5F\x0F\x53\x16"
	GUI_UC_END
	"SD "
	GUI_UC_START
	"\x53\x61\xFF\x0C\x66\x2F\x54\x26\x7E\xE7\x7E\xED\xFF\x1F"
	GUI_UC_END
	"\x0"
};

const char MsgBox8_TrCn[] = 
{
	//��ʽ��SD�����Ƿ��^�m��
	GUI_UC_START
	"\x68\x3C\x5F\x0F\x53\x16"
	GUI_UC_END
	"SD "
	GUI_UC_START
	"\x53\x61\xFF\x0C\x66\x2F\x54\x26\x7E\x7C\x7E\x8C\xFF\x1F"
	GUI_UC_END
	"\x0"
};

const char *MsgBox8[LANGUAGE_CNT] = 
{
	MsgBox8_En, 
	MsgBox8_SiCn, 
	MsgBox8_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox9_En[] = 
{
	//Format CF card, \ncontinue?
    "Format electronic disk, \nContinue?"
};

const char MsgBox9_SiCn[] = 
{
    //zqh 2015��3��20�� modify, 2015��4��15���޸�bug���������̡�����ʾ����ȷ
    //��ʽ�������̣��Ƿ������
	GUI_UC_START
    "\x68\x3c\x5f\x0f\x53\x16\x75\x35\x5b\x50\x76\xd8\xff\x0c\x66\x2f\x54\x26\x7e\xe7\x7e\xed\xff\x1f"
    GUI_UC_END
	"\x0"
};

const char MsgBox9_TrCn[] = 
{
    //zqh 2015��3��20�� modify��2015��4��15���޸�bug������ӱP������ʾ����ȷ
    //��ʽ����ӱP���Ƿ��^�m��
	GUI_UC_START
    "\x68\x3c\x5f\x0f\x53\x16\x96\xfb\x5b\x50\x76\xe4\xff\x0c\x66\x2f\x54\x26\x7e\x7c\x7e\x8c\xff\x1f"
    GUI_UC_END
	"\x0"
};

const char *MsgBox9[LANGUAGE_CNT] = 
{
	MsgBox9_En, 
	MsgBox9_SiCn, 
	MsgBox9_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox11_En[] = 
{
	//Format hard disk success!
	"Format hard disk success!"
};

const char MsgBox11_SiCn[] = 
{
	//��ʽ��Ӳ�̳ɹ���
	GUI_UC_START
	"\x68\x3C\x5F\x0F\x53\x16\x78\x6C\x76\xD8\x62\x10\x52\x9F\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char MsgBox11_TrCn[] = 
{
	//��ʽ��Ӳ���ɹ���
	GUI_UC_START
	"\x68\x3C\x5F\x0F\x53\x16\x78\x6C\x78\x9F\x62\x10\x52\x9F\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char *MsgBox11[LANGUAGE_CNT] = 
{
	MsgBox11_En, 
	MsgBox11_SiCn, 
	MsgBox11_TrCn
};

//--------------------------------------------------------------------------//

 const char MsgBox12_En[] = 
{
	//Format SM card success!
	"Format SM card success!"
};

const char MsgBox12_SiCn[] = 
{
	//��ʽ��SM�� �ɹ���
	GUI_UC_START
	"\x68\x3C\x5F\x0F\x53\x16 "
	GUI_UC_END
	"SM"
	GUI_UC_START
	"\x53\x61\x62\x10\x52\x9F\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char MsgBox12_TrCn[] = 
{
	//��ʽ��SM�� �ɹ���
	GUI_UC_START
	"\x68\x3C\x5F\x0F\x53 "
	GUI_UC_END
	"SM"
	GUI_UC_START
	"\x53\x61\x62\x10\x52\x9F\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char *MsgBox12[LANGUAGE_CNT] = 
{
	MsgBox12_En, 
	MsgBox12_SiCn, 
	MsgBox12_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox13_En[] = 
{
	//Format MS card success!
	"Format MS card success!"
};

const char MsgBox13_SiCn[] = 
{
	//��ʽ��MS���ɹ���
	GUI_UC_START
	"\x68\x3C\x5F\x0F\x53\x16"
	GUI_UC_END
	"MS"
	GUI_UC_START
	"\x53\x61\x62\x10\x52\x9F\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char MsgBox13_TrCn[] = 
{
	//��ʽ��MS���ɹ���
	GUI_UC_START
	"\x68\x3C\x5F\x0F\x53\x16"
	GUI_UC_END
	"MS"
	GUI_UC_START
	"\x53\x61\x62\x10\x52\x9F\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char *MsgBox13[LANGUAGE_CNT] = 
{
	MsgBox13_En, 
	MsgBox13_SiCn, 
	MsgBox13_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox14_En[] = 
{
	//Format SD card success!
	"Format SD card success!"
};

const char MsgBox14_SiCn[] = 
{
	//��ʽ��SD���ɹ���
	GUI_UC_START
	"\x68\x3C\x5F\x0F\x53\x16"
	GUI_UC_END
	"SD "
	GUI_UC_START
	"\x53\x61\x62\x10\x52\x9F\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char MsgBox14_TrCn[] = 
{
	//��ʽ��SD���ɹ���
	GUI_UC_START
	"\x68\x3C\x5F\x0F\x53\x16"
	GUI_UC_END
	"SD "
	GUI_UC_START
	"\x53\x61\x62\x10\x52\x9F\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char *MsgBox14[LANGUAGE_CNT] = 
{
	MsgBox14_En, 
	MsgBox14_SiCn, 
	MsgBox14_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox15_En[] = 
{
	//Format CF card success!
    "Format electronic disk success!"
};

const char MsgBox15_SiCn[] = 
{
    //zqh 2015��3��20�� modify
    //��ʽ�������̳ɹ���
	GUI_UC_START
    "\x68\x3C\x5F\x0F\x53\x16\x75\x35\x5b\x50\x76\xd8\x62\x10\x52\x9F\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char MsgBox15_TrCn[] = 
{
    //zqh 2015��3��20�� modify
    //��ʽ����ӱP�ɹ���
	GUI_UC_START
    "\x68\x3C\x5F\x0F\x53\x16\x96\xfb\x5b\x50\x76\xe4\x62\x10\x52\x9F\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char *MsgBox15[LANGUAGE_CNT] = 
{
	MsgBox15_En, 
	MsgBox15_SiCn, 
	MsgBox15_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox16_En[] = 
{
	//Format hard disk failed!
	"Format hard disk failed!"
};

const char MsgBox16_SiCn[] = 
{
	//��ʽ��Ӳ��ʧ�ܣ�
	GUI_UC_START
	"\x68\x3C\x5F\x0F\x53\x16\x78\x6C\x76\xD8\x59\x31\x8D\x25\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char MsgBox16_TrCn[] = 
{
	//��ʽ��Ӳ��ʧ����
	GUI_UC_START
	"\x68\x3C\x5F\x0F\x53\x16\x78\x6C\x78\x9F\x59\x31\x65\x57\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char *MsgBox16[LANGUAGE_CNT] = 
{
	MsgBox16_En, 
	MsgBox16_SiCn, 
	MsgBox16_TrCn
};

//--------------------------------------------------------------------------//

 const char MsgBox17_En[] = 
{
	//Format SM card failed!
	"Format SM card failed!"
};

const char MsgBox17_SiCn[] = 
{
	//��ʽ��SM�� ʧ�ܣ�
	GUI_UC_START
	"\x68\x3C\x5F\x0F\x53\x16 "
	GUI_UC_END
	"SM"
	GUI_UC_START
	"\x53\x61\x59\x31\x8D\x25\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char MsgBox17_TrCn[] = 
{
	//��ʽ��SM�� ʧ����
	GUI_UC_START
	"\x68\x3C\x5F\x0F\x53 "
	GUI_UC_END
	"SM"
	GUI_UC_START
	"\x53\x61\x59\x31\x65\x57\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char *MsgBox17[LANGUAGE_CNT] = 
{
	MsgBox17_En, 
	MsgBox17_SiCn, 
	MsgBox17_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox18_En[] = 
{
	//Format MS card failed!
	"Format MS card failed!"
};

const char MsgBox18_SiCn[] = 
{
	//��ʽ��MS��ʧ�ܣ�
	GUI_UC_START
	"\x68\x3C\x5F\x0F\x53\x16"
	GUI_UC_END
	"MS"
	GUI_UC_START
	"\x53\x61\x59\x31\x8D\x25\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char MsgBox18_TrCn[] = 
{
	//��ʽ��MS��ʧ����
	GUI_UC_START
	"\x68\x3C\x5F\x0F\x53\x16"
	GUI_UC_END
	"MS"
	GUI_UC_START
	"\x53\x61\x59\x31\x65\x57\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char *MsgBox18[LANGUAGE_CNT] = 
{
	MsgBox18_En, 
	MsgBox18_SiCn, 
	MsgBox18_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox19_En[] = 
{
	//Format SD card failed!
	"Format SD card failed!"
};

const char MsgBox19_SiCn[] = 
{
	//��ʽ��SD��ʧ�ܣ�
	GUI_UC_START
	"\x68\x3C\x5F\x0F\x53\x16"
	GUI_UC_END
	"SD "
	GUI_UC_START
	"\x53\x61\x59\x31\x8D\x25\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char MsgBox19_TrCn[] = 
{
	//��ʽ��SD��ʧ����
	GUI_UC_START
	"\x68\x3C\x5F\x0F\x53\x16"
	GUI_UC_END
	"SD "
	GUI_UC_START
	"\x53\x61\x59\x31\x65\x57\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char *MsgBox19[LANGUAGE_CNT] = 
{
	MsgBox19_En, 
	MsgBox19_SiCn, 
	MsgBox19_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox20_En[] = 
{
	//Format CF card failed!
    "Format electronic disk failed!"
};

const char MsgBox20_SiCn[] = 
{
    //zqh 2015��3��20�� modify
    //��ʽ��������ʧ�ܣ�
	GUI_UC_START
    "\x68\x3c\x5f\x0f\x53\x16\x75\x35\x5b\x50\x76\xd8\x59\x31\x8d\x25\xff\x01"
	GUI_UC_END
    "\x0"
};

const char MsgBox20_TrCn[] = 
{
    //zqh 2015��3��20�� modify
    //��ʽ����ӱPʧ����
	GUI_UC_START
    "\x68\x3C\x5F\x0F\x53\x16\x96\xfb\x5b\x50\x76\xe4\x59\x31\x65\x57\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char *MsgBox20[LANGUAGE_CNT] = 
{
	MsgBox20_En, 
	MsgBox20_SiCn, 
	MsgBox20_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox21_En[] = 
{
	//Specified file's name is \nidentical to the existing one.\nPlease specify another one.
	"Specified file's name is \nidentical to the existing one.\nPlease specify another one."
};

const char MsgBox21_SiCn[] = 
{
    //ָ�����ļ��������ļ�������\n��ָ����һ�ļ�����
	GUI_UC_START
	"\x63\x07\x5B\x9A\x76\x84\x65\x87\x4E\xF6\x4E\x0E\x73\xB0\x67\x09\x65\x87\x4E\xF6\x91\xCD\x54\x0D\x30\x02"
	GUI_UC_END
	"\n"
	GUI_UC_START
	"\x8B\xF7\x63\x07\x5B\x9A\x53\xE6\x4E\x00\x65\x87\x4E\xF6\x54\x0D\x30\x02"
	GUI_UC_END
	"\x0"
};

const char MsgBox21_TrCn[] = 
{
	//ָ���ęn�c�F�Йn������\nՈָ����һ�n������
	GUI_UC_START
	"\x63\x07\x5B\x9A\x76\x84\x6A\x94\x82\x07\x73\xFE\x67\x09\x6A\x94\x91\xCD\x54\x0D\x30\x02"
	GUI_UC_END
	"\n"
	GUI_UC_START
	"\x8A\xCB\x63\x07\x5B\x9A\x53\xE6\x4E\x00\x6A\x94\x68\x48\x54\x0D\x30\x02"
	GUI_UC_END
	"\x0"
};

const char *MsgBox21[LANGUAGE_CNT] = 
{
	MsgBox21_En, 
	MsgBox21_SiCn, 
	MsgBox21_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox22_En[] = 
{
	//No video file!
	"No video file!"
};

const char MsgBox22_SiCn[] = 
{
	//û����Ƶ�ļ���
	GUI_UC_START
	"\x6C\xA1\x67\x09\x89\xC6\x98\x91\x65\x87\x4E\xF6\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char MsgBox22_TrCn[] = 
{
	//�]��ҕ�l�n��
	GUI_UC_START
	"\x6C\x92\x67\x09\x89\x96\x98\x3B\x6A\x94\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char *MsgBox22[LANGUAGE_CNT] = 
{
	MsgBox22_En, 
	MsgBox22_SiCn, 
	MsgBox22_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox23_En[] = 
{
	//Delete file success!
	"Delete file success!"
};

const char MsgBox23_SiCn[] = 
{
	//ɾ���ļ��ɹ���
	GUI_UC_START
	"\x52\x20\x96\x64\x65\x87\x4E\xF6\x62\x10\x52\x9F\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char MsgBox23_TrCn[] = 
{
	//�h���n�ɹ���
	GUI_UC_START
	"\x52\x2A\x96\x64\x6A\x94\x62\x10\x52\x9F\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char *MsgBox23[LANGUAGE_CNT] = 
{
	MsgBox23_En, 
	MsgBox23_SiCn, 
	MsgBox23_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox24_En[] = 
{
	//Delete file failed!
	"Delete file failed!"
};

const char MsgBox24_SiCn[] = 
{
	//ɾ���ļ�ʧ�ܣ�
	GUI_UC_START
	"\x52\x20\x96\x64\x65\x87\x4E\xF6\x59\x31\x8D\x25\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char MsgBox24_TrCn[] = 
{
	//�h���nʧ����
	GUI_UC_START
	"\x52\x2A\x96\x64\x6A\x94\x59\x31\x65\x57\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char *MsgBox24[LANGUAGE_CNT] = 
{
	MsgBox24_En, 
	MsgBox24_SiCn, 
	MsgBox24_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox25_En[] = 
{
	//Rename file success!
	"Rename file success!"
};

const char MsgBox25_SiCn[] = 
{
	//�������ļ��ɹ���
	GUI_UC_START
	"\x91\xCD\x54\x7D\x54\x0D\x65\x87\x4E\xF6\x62\x10\x52\x9F\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char MsgBox25_TrCn[] = 
{
	//�������n�ɹ���
	GUI_UC_START
	"\x91\xCD\x54\x7D\x54\x0D\x6A\x94\x62\x10\x52\x9F\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char *MsgBox25[LANGUAGE_CNT] = 
{
	MsgBox25_En, 
	MsgBox25_SiCn, 
	MsgBox25_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox26_En[] = 
{
	//Rename file success!
	"Rename file success!"
};

const char MsgBox26_SiCn[] = 
{
	//�������ļ�ʧ�ܣ�
	GUI_UC_START
	"\x91\xCD\x54\x7D\x54\x0D\x65\x87\x4E\xF6\x59\x31\x8D\x25\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char MsgBox26_TrCn[] = 
{
	//�������nʧ����
	GUI_UC_START
	"\x91\xCD\x54\x7D\x54\x0D\x6A\x94\x59\x31\x65\x57\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char *MsgBox26[LANGUAGE_CNT] = 
{
	MsgBox26_En, 
	MsgBox26_SiCn, 
	MsgBox26_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox27_En[] = 
{
	//Can't scan!\nplease change to Cable mode!
	"Can't scan!\nplease change to Cable mode!"
};

const char MsgBox27_SiCn[] = 
{
	//�޷���̨���뽫�����л���Cableģʽ��
	GUI_UC_START
	"\x65\xE0\x6C\xD5\x64\x1C\x53\xF0"
	GUI_UC_END
	",\n"
	GUI_UC_START
	"\x8B\xF7\x5C\x06\x8F\x93\x51\x65\x52\x07\x63\x62\x81\xF3"
	GUI_UC_END
	"Cable"
	GUI_UC_START
	"\x6A\x21\x5F\x0F\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char MsgBox27_TrCn[] = 
{
	//�o����̨��Ո��ݔ���ГQ��Cableģʽ��
	GUI_UC_START
	"\x71\x21\x6C\xD5\x64\x1C\x53\xF0"
	GUI_UC_END
	",\n"
	GUI_UC_START
	"\x8A\xCB\x5C\x07\x8F\x38\x51\x65\x52\x07\x63\xDB\x81\xF3"
	GUI_UC_END
	"Cable"
	GUI_UC_START
	"\x6A\x21\x5F\x0F\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char *MsgBox27[LANGUAGE_CNT] = 
{
	MsgBox27_En, 
	MsgBox27_SiCn, 
	MsgBox27_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox28_En[] = 
{
	//Auto Scan finished
	"Auto Scan finished"
};

const char MsgBox28_SiCn[] = 
{
	//��̨����
	GUI_UC_START
	"\x64\x1C\x53\xF0\x7E\xD3\x67\x5F"
	GUI_UC_END
	"\x0"
};

const char MsgBox28_TrCn[] = 
{
	//��̨�Y����
	GUI_UC_START
	"\x64\x1C\x53\xF0\x7D\x50\x67\x5F"
	GUI_UC_END
	"\x0"
};

const char *MsgBox28[LANGUAGE_CNT] = 
{
	MsgBox28_En, 
	MsgBox28_SiCn, 
	MsgBox28_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox29_En[] = 
{
	//Fixed time exceeds\nthe movie length!
	"Fixed time exceeds\nthe movie length!"
};

const char MsgBox29_SiCn[] = 
{
	//�趨ʱ�䳬��ӰƬ���ȣ�
	GUI_UC_START
	"\x8B\xBE\x5B\x9A\x65\xF6\x95\xF4\x8D\x85\x51\xFA\x5F\x71\x72\x47\x95\x7F\x5E\xA6\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char MsgBox29_TrCn[] = 
{
	//�O���r�g����ӰƬ�L�ȣ�
	GUI_UC_START
	"\x8A\x2D\x5B\x9A\x66\x42\x95\x93\x8D\x85\x51\xFA\x5F\x71\x72\x47\x95\x77\x5E\xA6\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char *MsgBox29[LANGUAGE_CNT] = 
{
	MsgBox29_En, 
	MsgBox29_SiCn, 
	MsgBox29_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox30_En[] = 
{
	//Delete file, continue?
	"Delete file, continue?"
};

const char MsgBox30_SiCn[] = 
{
	//ɾ���ļ����Ƿ������
	GUI_UC_START
	"\x52\x20\x96\x64\x65\x87\x4E\xF6\xFF\x0C\x66\x2F\x54\x26\x7E\xE7\x7E\xED\xFF\x1F"
	GUI_UC_END
	"\x0"
};

const char MsgBox30_TrCn[] = 
{
	//�h���n���Ƿ��^�m��
	GUI_UC_START
	"\x52\x2A\x96\x64\x6A\x94\xFF\x0C\x66\x2F\x54\x26\x7E\x7C\x7E\x8C\xFF\x1F"
	GUI_UC_END
	"\x0"
};

const char *MsgBox30[LANGUAGE_CNT] = 
{
	MsgBox30_En, 
	MsgBox30_SiCn, 
	MsgBox30_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox31_En[] = 
{
	//Ready to record, it will switch to\nthe according channel after 2 seconds!
	"Ready to record, it will switch to\nthe according channel after 2 seconds!"
};

const char MsgBox31_SiCn[] = 
{
	//¼�Ƽƻ�����������2����Զ��л���¼��Ƶ����
	GUI_UC_START
	"\x5F\x55\x52\x36\x8B\xA1\x52\x12\x53\x73\x5C\x06\x54\x2F\x52\xA8\xFF\x0C"
	GUI_UC_END
	"\n2"
	GUI_UC_START
	"\x79\xD2\x54\x0E\x81\xEA\x52\xA8\x52\x07\x63\x62\x81\xF3\x5F\x55\x52\x36\x98\x91\x90\x53\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char MsgBox31_TrCn[] = 
{
	//��uӋ���������ӣ�2�����Ԅ��ГQ����u�l����
	GUI_UC_START
	"\x79\xD2\x54\x0E\x81\xEA\x52\xA8\x52\x07\x63\x62\x81\xF3\x5F\x55\x52\x36\x98\x91\x90\x53\xFF\x01"
	GUI_UC_END
	"\n2"
	GUI_UC_START
	"\x79\xD2\x5F\x8C\x81\xEA\x52\xD5\x52\x07\x63\xDB\x81\xF3\x93\x04\x88\xFD\x98\x3B\x90\x53\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char *MsgBox31[LANGUAGE_CNT] = 
{
	MsgBox31_En, 
	MsgBox31_SiCn, 
	MsgBox31_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox32_En[] = 
{
	//Record start!
	"Record start!"
};

const char MsgBox32_SiCn[] = 
{
	//¼�ƿ�ʼ��
	GUI_UC_START
	"\x5F\x55\x52\x36\x5F\x00\x59\xCB\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char MsgBox32_TrCn[] = 
{
	//��u�_ʼ��
	GUI_UC_START
	"\x93\x04\x88\xFD\x95\x8B\x59\xCB\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char *MsgBox32[LANGUAGE_CNT] = 
{
	MsgBox32_En, 
	MsgBox32_SiCn, 
	MsgBox32_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox33_En[] = 
{
	//Record over!
	"Record over!"
};

const char MsgBox33_SiCn[] = 
{
	//¼�ƽ�����
	GUI_UC_START
	"\x5F\x55\x52\x36\x7E\xD3\x67\x5F\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char MsgBox33_TrCn[] = 
{
	//��u�Y��
	GUI_UC_START
	"\x93\x04\x88\xFD\x7D\x50\x67\x5F\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char *MsgBox33[LANGUAGE_CNT] = 
{
	MsgBox33_En, 
	MsgBox33_SiCn, 
	MsgBox33_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox34_En[] = 
{
	//The same title document exists , can't copy!
	"The same title document\nexists , can't copy!"
};

const char MsgBox34_SiCn[] = 
{
	//����ͬ���ļ����޷����ƣ�
	GUI_UC_START
	"\x5B\x58\x57\x28\x54\x0C\x54\x0D\x65\x87\x4E\xF6\xFF\x0C\x65\xE0\x6C\xD5\x59\x0D\x52\x36\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char MsgBox34_TrCn[] = 
{
	//����ͬ���n���o���}�u��
	GUI_UC_START
	"\x5B\x58\x57\x28\x54\x0C\x54\x0D\x6A\x94\xFF\x0C\x71\x21\x6C\xD5\x89\x07\x88\xFD\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char *MsgBox34[LANGUAGE_CNT] = 
{
	MsgBox34_En, 
	MsgBox34_SiCn, 
	MsgBox34_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox35_En[] = 
{
	//Copy success!
	"Copy success!"
};

const char MsgBox35_SiCn[] = 
{
	//�ļ����Ƴɹ���
	GUI_UC_START
	"\x65\x87\x4E\xF6\x59\x0D\x52\x36\x62\x10\x52\x9F\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char MsgBox35_TrCn[] = 
{
	//�n�}�u�ɹ���
	GUI_UC_START
	"\x6A\x94\x89\x07\x88\xFD\x62\x10\x52\x9F\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char *MsgBox35[LANGUAGE_CNT] = 
{
	MsgBox35_En, 
	MsgBox35_SiCn, 
	MsgBox35_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox36_En[] = 
{
	//Copy failed, please check disk space!
	"Copy failed!"
};

const char MsgBox36_SiCn[] = 
{
    //��(6587)��(4EF6)��(590D)��(5236)ʧ(5931)��(8D25)!
	GUI_UC_START
    "\x65\x87\x4E\xF6\x59\x0D\x52\x36\x59\x31\x8D\x25\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char MsgBox36_TrCn[] = 
{
    //��(6587)�n(6A94)��(590D)��(5236)ʧ(5931)��(8D25)��
	GUI_UC_START
    "\x65\x87\x6A\x94\x59\x0d\x52\x36\x59\x31\x8d\x25\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char *MsgBox36[LANGUAGE_CNT] = 
{
	MsgBox36_En, 
	MsgBox36_SiCn, 
	MsgBox36_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox37_En[] = 
{
	//Copy file, continue?
	"Copy file, continue?"
};

const char MsgBox37_SiCn[] = 
{
	//�ļ����ƣ��Ƿ������
	GUI_UC_START
	"\x65\x87\x4E\xF6\x59\x0D\x52\x36\xFF\x0C\x66\x2F\x54\x26\x7E\xE7\x7E\xED\xFF\x1F"
	GUI_UC_END
	"\x0"
};

const char MsgBox37_TrCn[] = 
{
	//�n�}�u���Ƿ��^�m��
	GUI_UC_START
	"\x6A\x94\x89\x07\x88\xFD\xFF\x0C\x66\x2F\x54\x26\x7E\x7C\x7E\x8C\xFF\x1F"
	GUI_UC_END
	"\x0"
};

const char *MsgBox37[LANGUAGE_CNT] = 
{
	MsgBox37_En, 
	MsgBox37_SiCn, 
	MsgBox37_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox38_En[] = 
{
	//Auto Scan will erase all channel info first
	"Auto Scan will erase all channel info first"
};

const char  MsgBox38_SiCn[] = 
{
	//�Զ���̨���������Ƶ����Ϣ
	GUI_UC_START
	"\x81\xEA\x52\xA8\x64\x1C\x53\xF0\x5C\x06\x99\x96\x51\x48\x6E\x05\x96\x64\x98\x91\x90\x53\x4F\xE1\x60\x6F"
	GUI_UC_END
	"\x0"
};

const char  MsgBox38_TrCn[] = 
{
	//�Ԅ���̨������l���YӍ
	GUI_UC_START
	"\x81\xEA\x52\xD5\x64\x1C\x53\xF0\x5C\x07\x99\x96\x51\x48\x6E\x05\x96\x64\x98\x3B\x90\x53\x8C\xC7\x8A\x0A"
	GUI_UC_END
	"\x0"
};

const char * MsgBox38[LANGUAGE_CNT] = 
{
	 MsgBox38_En, 
	 MsgBox38_SiCn, 
	 MsgBox38_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox39_En[] = 
{
	//Press \"Enter\" to start.
	"Press \"Enter\" to start."
};

const char MsgBox39_SiCn[] = 
{
	//��"Enter"��ʼ��̨
	GUI_UC_START
	"\x63\x09"
	GUI_UC_END
	"\"Enter\""
	GUI_UC_START
	"\x5F\x00\x59\xCB\x64\x1C\x53\xF0"
	GUI_UC_END
	"\x0"
};

const char MsgBox39_TrCn[] = 
{
	//��"Enter"�_ʼ��̨
	GUI_UC_START
	"\x63\x09"
	GUI_UC_END
	"\"Enter\""
	GUI_UC_START
	"\x95\x8B\x59\xCB\x64\x1C\x53\xF0"
	GUI_UC_END
	"\x0"
};

const char *MsgBox39[LANGUAGE_CNT] = 
{
	MsgBox39_En, 
	MsgBox39_SiCn, 
	MsgBox39_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox40_En[] = 
{
	//Current Band : 
	"Current Band : "
};

const char MsgBox40_SiCn[] = 
{
	//��ǰƵ�Σ�
	GUI_UC_START
	"\x5F\x53\x52\x4D\x98\x91\x6B\xB5\xFF\x1A"
	GUI_UC_END
	"\x0"
};

const char MsgBox40_TrCn[] = 
{
	//��ǰ�l�Σ�
	GUI_UC_START
	"\x75\x76\x52\x4D\x98\x3B\x6B\xB5\xFF\x1A"
	GUI_UC_END
	"\x0"
};

const char *MsgBox40[LANGUAGE_CNT] = 
{
	MsgBox40_En, 
	MsgBox40_SiCn, 
	MsgBox40_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox41_En[] = 
{
	//channels
	"channels"
};

const char MsgBox41_SiCn[] = 
{
	//��Ƶ��
	GUI_UC_START
	"\x4E\x2A\x98\x91\x90\x53"
	GUI_UC_END
	"\x0"
};

const char MsgBox41_TrCn[] = 
{
	//���l��
	GUI_UC_START
	"\x50\x0B\x98\x3B\x90\x53"
	GUI_UC_END
	"\x0"
};

const char *MsgBox41[LANGUAGE_CNT] = 
{
	MsgBox41_En, 
	MsgBox41_SiCn, 
	MsgBox41_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox42_En[] = 
{
	//Scanned channels: 
	"Scanned channels:"
};

const char MsgBox42_SiCn[] = 
{
	//��������̨����
	GUI_UC_START
	"\x64\x1C\x7D\x22\x52\x30\x76\x84\x53\xF0\x65\x70\xFF\x1A"
	GUI_UC_END
	"\x0"
};

const char MsgBox42_TrCn[] = 
{
	//��������̨����
	GUI_UC_START
	"\x64\x1C\x7D\x22\x52\x30\x76\x84\x53\xF0\x65\x78\xFF\x1A"
	GUI_UC_END
	"\x0"
};

const char *MsgBox42[LANGUAGE_CNT] = 
{
	MsgBox42_En, 
	MsgBox42_SiCn, 
	MsgBox42_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox43_En[] = 
{
	//Reset success
    "Reset success!"
};

const char MsgBox43_SiCn[] = 
{
    //�ָ�Ĭ�����óɹ�!
	GUI_UC_START
    "\x60\x62\x59\x0D\x9E\xD8\x8B\xA4\x8B\xBE\x7F\x6E\x62\x10\x52\x9F\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char MsgBox43_TrCn[] = 
{
    //�֏�Ĭ�J�O�óɹ�!
	GUI_UC_START
    "\x60\x62\x5F\xA9\x9E\xD8\x8A\x8D\x8A\x2D\x7F\x6E\x62\x10\x52\x9F\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char *MsgBox43[LANGUAGE_CNT] = 
{
	MsgBox43_En, 
	MsgBox43_SiCn, 
	MsgBox43_TrCn
};

//--------------------------------------------------------------------------//

const char MsgBox44_En[] = 
{
	//Reset failed
    "Reset failed!"
};

const char MsgBox44_SiCn[] = 
{
    //�ָ�Ĭ������ʧ��!
	GUI_UC_START
    "\x60\x62\x59\x0D\x9E\xD8\x8B\xA4\x8B\xBE\x7F\x6E\x59\x31\x8D\x25\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char MsgBox44_TrCn[] = 
{
    //�֏�Ĭ�J�O��ʧ��!
	GUI_UC_START
    "\x60\x62\x5F\xA9\x9E\xD8\x8A\x8D\x8A\x2D\x7F\x6E\x59\x31\x65\x57\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char *MsgBox44[LANGUAGE_CNT] = 
{
	MsgBox44_En, 
	MsgBox44_SiCn, 
	MsgBox44_TrCn
};

//--------------------------------------------------------------------------//
const char MsgBox45_En[] = 
{
	//Record will be started soon,\nwill you stop the playback?
	"Record will be started soon,\nwill you stop the playback?"
};

const char MsgBox45_SiCn[] = 
{
	//¼�Ƽ���������\n�Ƿ�ֹͣ��ǰ���ţ�
	GUI_UC_START
	"\x5F\x55\x52\x36\x53\x73\x5C\x06\x54\x2F\x52\xA8\xFF\x0C"
	GUI_UC_END
	"\n"
	GUI_UC_START
	"\x66\x2F\x54\x26\x50\x5C\x6B\x62\x5F\x53\x52\x4D\x64\xAD\x65\x3E\xFF\x1F"
	GUI_UC_END
	"\x0"
};

const char MsgBox45_TrCn[] = 
{
	//��u�������ӣ��Ƿ�ֹͣ��ǰ���ţ�
	GUI_UC_START
	"\x93\x04\x88\xFD\x53\x73\x5C\x07\x55\x5F\x52\xD5\xFF\x0C"
	GUI_UC_END
	"\n"
	GUI_UC_START
	"\x66\x2F\x54\x26\x50\x5C\x6B\x62\x75\x76\x52\x4D\x64\xAD\x65\x3E\xFF\x1F"
	GUI_UC_END
	"\x0"
};

const char *MsgBox45[LANGUAGE_CNT] = 
{
	MsgBox45_En, 
	MsgBox45_SiCn, 
	MsgBox45_TrCn
};

//zqh 2015��3��25�� add,��ɾ����������������Ƶ�ļ�ʱ����ʾ��Ϣ�����������
const char MsgBox46_En[] =
{
    //Delete file, continue?
    "Delete all file, continue?"
};

const char MsgBox46_SiCn[] =
{
    //ɾ�������ļ����Ƿ������
    GUI_UC_START
    "\x52\x20\x96\x64\x62\x40\x67\x09\x65\x87\x4E\xF6\xFF\x0C\x66\x2F\x54\x26\x7E\xE7\x7E\xED\xFF\x1F"
    GUI_UC_END
    "\x0"
};

const char MsgBox46_TrCn[] =
{
    //�h�������ęn���Ƿ��^�m��
    GUI_UC_START
    "\x52\x2A\x96\x64\x62\x40\x67\x09\x65\x87\x6A\x94\xFF\x0C\x66\x2F\x54\x26\x7E\x7C\x7E\x8C\xFF\x1F"
    GUI_UC_END
    "\x0"
};

const char *MsgBox46[LANGUAGE_CNT] =
{
    MsgBox46_En,
    MsgBox46_SiCn,
    MsgBox46_TrCn
};

//--------------------------------------------------------------------------//
const char MsgBox47_En[] =
{
    //Electronic disk inserted\nMounting
    "Check electronic disk\nFile system, continue?"
};

const char MsgBox47_SiCn[] =
{
    //zqh 2015��3��20�� modify�������������ļ�ϵͳ���Ƿ��������
    // ��(68C0)��(67E5)������ ��(6587)��(4EF6)ϵ(7CFB)ͳ(7EDF)?
    GUI_UC_START
    "\x68\xc0\x67\xe5\x75\x35\x5b\x50\x76\xd8\x65\x87\x4e\xf6\x7c\xfb\x7e\xdf\xff\x0c\x66\x2f\x54\x26\x7e\xe7\x7e\xed\xff\x1f"
    GUI_UC_END
    "\x0"
};

const char MsgBox47_TrCn[] =
{
    //zqh 2015��3��20��  modify,"�z����ӱP�ļ�ϵ�y���Ƿ��^�m��"
    // �z(6AA2)��(67E5)��ӱP()�n(6A94)��(6848)ϵ(7CFB)�y(7D71)
    GUI_UC_START
    "\x6a\xa2\x67\xe5\x96\xfb\x5b\x50\x76\xe4\x65\x87\x4e\xf6\x7c\xfb\x7d\x71\xff\x0c\x66\x2f\x54\x26\x7e\x7c\x7e\x8c\xff\x1f"
    GUI_UC_END
    "\x0"
};

const char *MsgBox47[LANGUAGE_CNT] =
{
    MsgBox47_En,
    MsgBox47_SiCn,
    MsgBox47_TrCn
};

//--------------------------------------------------------------------------//
//zqh 2015��5��21�� add�������ڡ�����ϵͳ����ʱ�������ʾ��Ϣ����
const char MsgBox48_En[] =
{
    //Set system language to English
    "Set system language to English"
};

const char MsgBox48_SiCn[] =
{
    //zqh 2015��5��21�� add����ϵͳ��������Ϊ�������ġ�
    // ϵͳ��������Ϊ��������
    GUI_UC_START
    "\x7c\xfb\x7e\xdf\x8b\xed\x8a\x00\x8b\xbe\x7f\x6e\x4e\x3a\x7b\x80\x4f\x53\x4e\x2d\x65\x87"
    GUI_UC_END
    "\x0"
};

const char MsgBox48_TrCn[] =
{
    //zqh 2015��5��21�� add����ϵ�y�Z���O�Þ鷱�w���ġ�
    // ϵ�y�Z���O�Þ鷱�w����
    GUI_UC_START
    "\x7c\xfb\x7d\x71\x8a\x9e\x8a\x00\x8a\x2d\x7f\x6e\x70\xba\x7e\x41\x9a\xd4\x4e\x2d\x65\x87"
    GUI_UC_END
    "\x0"
};

const char *MsgBox48[LANGUAGE_CNT] =
{
    MsgBox48_En,
    MsgBox48_SiCn,
    MsgBox48_TrCn
};


const char CodecSetting_En[] = 
{
	//CodecSetting
	"CodecSetting"
};

const char CodecSetting_SiCn[] = 
{
	//�������
	GUI_UC_START
	"\x7F\x16\x78\x01\x53\xC2\x65\x70"
	GUI_UC_END
	"\x0"
};

const char CodecSetting_TrCn[] = 
{
	//��(7DE8)�a(78BC)��(53C3)��(6578)
	GUI_UC_START
	"\x7D\xE8\x78\xBC\x53\xC3\x65\x78"
	GUI_UC_END
	"\x0"
};

const char *CodecSetting[LANGUAGE_CNT] = 
{
	CodecSetting_En, 
	CodecSetting_SiCn, 
	CodecSetting_TrCn
};
////////////////////////////////////////////////
const char FullDiskStrategy_En[] = 
{
	//FullDiskStrategy
	"FullDiskStrategy"
};

const char FullDiskStrategy_SiCn[] = 
{
	//Ӳ(786C)��(76D8)��(6EE1)��(7B56)��(7565)
	GUI_UC_START
	"\x78\x6C\x76\xD8\x6E\xE1\x7B\x56\x75\x65"
	GUI_UC_END
	"\x0"
};

const char FullDiskStrategy_TrCn[] = 
{
	//Ӳ(786C)��(789F)�M(6EFF)��(7B56)��(7565)
	GUI_UC_START
	"\x78\x6C\x78\x9F\x6E\xFF\x7B\x56\x75\x65"
	GUI_UC_END
	"\x0"
};

const char *FullDiskStrategy[LANGUAGE_CNT] = 
{
	FullDiskStrategy_En, 
	FullDiskStrategy_SiCn, 
	FullDiskStrategy_TrCn
};
////////////////////////////////////////////////
const char MountUDisk_En[] = 
{
	//MountUDisk
	"Mount UDisk"
};

const char MountUDisk_SiCn[] = 
{
    //zqh 2015��3��20�� modify
    //U��δ����
    "U"
    GUI_UC_START
    "\x76\xD8\x67\x2a\x8F\xde\x63\xA5"
    GUI_UC_END
	"\x0"
};

const char MountUDisk_TrCn[] = 
{
    //zqh 2015��3��20�� modify
    //U�Pδ�B��
	"U"
	GUI_UC_START
    "\x76\xE4\x67\x2A\x90\x23\x63\xa5"
	GUI_UC_END
	"\x0"
};

const char *MountUDisk[LANGUAGE_CNT] = 
{
	MountUDisk_En, 
	MountUDisk_SiCn, 
	MountUDisk_TrCn
};
//--------------------------------------------------------------------------//
const char UMountUDisk_En[] = 
{
	//UnMountUDisk
	"UnMount UDisk"
};

const char UMountUDisk_SiCn[] = 
{
    //zqh 2015��3��20�� modify
    //U��������
    "U"
    GUI_UC_START
    "\x76\xD8\x5d\xf2\x8f\xde\x63\xa5"
    GUI_UC_END
	"\x0"
};

const char UMountUDisk_TrCn[] = 
{
    //zqh 2015��3��20�� modify
    //U�����B��
	"U"
	GUI_UC_START
    "\x76\xE4\x5d\xf2\x90\x23\x63\xa5"
	GUI_UC_END
	"\x0"
};

const char *UMountUDisk[LANGUAGE_CNT] = 
{
	UMountUDisk_En, 
	UMountUDisk_SiCn, 
	UMountUDisk_TrCn
};
////////////////////////////////////////////////
const char FormatSDCard_En[] = 
{
    //Format SDCard
    "Format SDCard"
};

const char FormatSDCard_SiCn[] = 
{
	//��(683C)ʽ(5F0F)��(5316)SD��(5361)
	//��(6E05)��(7A7A)SD��(5361)
	GUI_UC_START
	"\x68\x3C\x5F\x0F\x53\x16"
	GUI_UC_END
	"SD"
	GUI_UC_START
	"\x53\x61"
	GUI_UC_END
	"\x0"
};

const char FormatSDCard_TrCn[] = 
{
	//��(683C)ʽ(5F0F)��(5316)SD��(5361)
	GUI_UC_START
	"\x68\x3C\x5F\x0F\x53\x16"
	GUI_UC_END
	"SD"
	GUI_UC_START
	"\x53\x61"
	GUI_UC_END
	"\x0"
};

const char *FormatSDCard[LANGUAGE_CNT] = 
{
	FormatSDCard_En, 
	FormatSDCard_SiCn, 
	FormatSDCard_TrCn
};
////////////////////////////////////////////////
const char RestoreSetting_En[] = 
{
	//RestoreSetting
	"RestoreSetting"
};

const char RestoreSetting_SiCn[] = 
{
	//��(6062)��(590D)��(8BBE)��(7F6E)
	GUI_UC_START
	"\x60\x62\x59\x0D\x8B\xBE\x7F\x6E"
	GUI_UC_END
	"\x0"
};

const char RestoreSetting_TrCn[] = 
{
	//��(6062)��(5FA9)�O(8A2D)��(7F6E)
	GUI_UC_START
	"\x60\x62\x5F\xA9\x8A\x2D\x7F\x6E"
	GUI_UC_END
	"\x0"
};

const char *RestoreSetting[LANGUAGE_CNT] = 
{
	RestoreSetting_En, 
	RestoreSetting_SiCn, 
	RestoreSetting_TrCn
};
////////////////////////////////////////////////
const char SystemUpdate_En[] = 
{
	//SystemUpdate
    "System Update"
};

const char SystemUpdate_SiCn[] = 
{
	//ϵ(7CFB)ͳ(7EDF)��(5347)��(7EA7)
	GUI_UC_START
	"\x7C\xFB\x7E\xDF\x53\x47\x7E\xA7"
	GUI_UC_END
	"\x0"
};

const char SystemUpdate_TrCn[] = 
{
	//ϵ(7CFB)�y(7D71)��(5347)��(7D1A)
	GUI_UC_START
	"\x7C\xFB\x7D\x71\x53\x47\x7D\x1A"
	GUI_UC_END
	"\x0"
};

const char *SystemUpdate[LANGUAGE_CNT] = 
{
	SystemUpdate_En, 
	SystemUpdate_SiCn, 
	SystemUpdate_TrCn
};

////////////////////////////////////////////////
const char CodecLevel_En[] = 
{
	//CodecLevel
	"CodecLevel"
};

const char CodecLevel_SiCn[] = 
{
	//��(7F16)��(7801)��(8D28)��(91CF)��(8BBE)��(5B9A)
	GUI_UC_START
	"\x7F\x16\x78\x01\x8D\x28\x91\xCF\x8B\xBE\x5B\x9A"
	GUI_UC_END
	"\x0"
};

const char CodecLevel_TrCn[] = 
{
	//��(7DE8)�a(78BC) Ʒ(54C1)�|(8CEA)�O(8A2D)��(5B9A)
	GUI_UC_START
	"\x7D\xE8\x78\xBC\x54\xC1\x8C\xEA\x8A\x2D\x5B\x9A"
	GUI_UC_END
	"\x0"
};

const char *CodecLevel[LANGUAGE_CNT] = 
{
	CodecLevel_En, 
	CodecLevel_SiCn, 
	CodecLevel_TrCn
};

////////////////////////////////////////////////
const char Chroma_En[] = 
{
	//PicBrightness
	"Chroma"
};

const char Chroma_SiCn[] = 
{
	//ɫ(8272)��(5EA6)��(8BBE)��(7F6E)
	GUI_UC_START
	"\x82\x72\x5E\xA6\x8B\xBE\x7F\x6E"
	GUI_UC_END
	"\x0"
};

const char Chroma_TrCn[] = 
{
	//ɫ(8272)��(5EA6)�O(8A2D)��(7F6E)
	GUI_UC_START
	"\x82\x72\x5E\xA6\x8A\x2D\x7F\x6E"
	GUI_UC_END
	"\x0"
};

const char *Chroma[LANGUAGE_CNT] = 
{
	Chroma_En, 
	Chroma_SiCn, 
	Chroma_TrCn
};

////////////////////////////////////////////////
const char DeleteOldFile_En[] = 
{
	//AutoDelete
	"Delete Old File"
};

const char DeleteOldFile_SiCn[] = 
{
	//ɾ(5220)��(9664)��(65E7)��(6587)��(4EF6)
	GUI_UC_START
	"\x52\x20\x96\x64\x65\xE7\x65\x87\x4E\xF6"
	GUI_UC_END
	"\x0"
};

const char DeleteOldFile_TrCn[] = 
{
	//�h(522A)��(9664)�f(820A)��(6587)��(4EF6)
	GUI_UC_START
	"\x52\x2A\x96\x64\x82\x0A\x65\x87\x4E\xF6"
	GUI_UC_END
	"\x0"
};

const char *DeleteOldFile[LANGUAGE_CNT] = 
{
	DeleteOldFile_En,
	DeleteOldFile_SiCn,
	DeleteOldFile_TrCn
};
////////////////////////////////////////////////
const char StopSave_En[] = 
{
	//StopSave
	"Stop Save"
};

const char StopSave_SiCn[] = 
{
	//ͣ(505C)ֹ(6B62)��(4FDD)��(5B58)
	GUI_UC_START
	"\x50\x5C\x6B\x62\x4F\xDD\x5B\x58"
	GUI_UC_END
	"\x0"
};

const char StopSave_TrCn[] = 
{
	//ͣ(505C)ֹ(6B62)��(4FDD)��(5B58)
	GUI_UC_START
	"\x50\x5C\x6B\x62\x4F\xDD\x5B\x58"
	GUI_UC_END
	"\x0"
};

const char *StopSave[LANGUAGE_CNT] = 
{
	StopSave_En,
	StopSave_SiCn,
	StopSave_TrCn
};
////////////////////////////////////////////////
const char HighestQuality_En[] = 
{
	//Highest Quality
	"Highest Quality"
};

const char HighestQuality_SiCn[] = 
{
	//��(6700)��(9AD8)��(8D28)��(91CF)
	GUI_UC_START
	"\x67\x00\x9A\xD8\x8D\x28\x91\xCF"
	GUI_UC_END
	"\x0"
};

const char HighestQuality_TrCn[] = 
{
	//��(6700)��(9AD8)Ʒ(54C1)�|(8CEA)
	GUI_UC_START
	"\x67\x00\x9A\xD8\x54\xC1\x8C\xEA"
	GUI_UC_END
	"\x0"
};

const char *HighestQuality[LANGUAGE_CNT] = 
{
	HighestQuality_En,
	HighestQuality_SiCn,
	HighestQuality_TrCn
};
////////////////////////////////////////////////
const char HighQuality_En[] = 
{
	//High Quality
	"High Quality"
};

const char HighQuality_SiCn[] = 
{
	//��(8F83)��(9AD8)��(8D28)��(91CF)
	GUI_UC_START
	"\x8F\x83\x9A\xD8\x8D\x28\x91\xCF"
	GUI_UC_END
	"\x0"
};

const char HighQuality_TrCn[] = 
{
	//�^(8F03)��(9AD8)Ʒ(54C1)�|(8CEA)
	GUI_UC_START
	"\x8F\x03\x9A\xD8\x54\xC1\x8C\xEA"
	GUI_UC_END
	"\x0"
};

const char *HighQuality[LANGUAGE_CNT] = 
{
	HighQuality_En,
	HighQuality_SiCn,
	HighQuality_TrCn
};
//--------------------------------------------------------------------------//
const char MediumQuality_En[] = 
{
	//MediumQuality
	"Medium Quality"
};

const char MediumQuality_SiCn[] = 
{
	//��(4E2D)��(7B49)��(8D28)��(91CF)
	GUI_UC_START
	"\x4E\x2D\x7B\x49\x8D\x28\x91\xCF"
	GUI_UC_END
	"\x0"
};

const char MediumQuality_TrCn[] = 
{
	//��(4E2D)��(7B49)Ʒ(54C1)�|(8CEA)
	GUI_UC_START
	"\x4E\x2D\x7B\x49\x54\xC1\x8C\xEA"
	GUI_UC_END
	"\x0"
};

const char *MediumQuality[LANGUAGE_CNT] = 
{
	MediumQuality_En, 
	MediumQuality_SiCn, 
	MediumQuality_TrCn
};
//--------------------------------------------------------------------------//
const char LowQuality_En[] = 
{
	//LowQuality
	"Low Quality"
};

const char LowQuality_SiCn[] = 
{
	//��(8F83)��(4F4E)��(8D28)��(91CF)
	GUI_UC_START
	"\x8F\x83\x4F\x4E\x8D\x28\x91\xCF"
	GUI_UC_END
	"\x0"
};

const char LowQuality_TrCn[] = 
{
	//�^(8F03)��(4F4E)Ʒ(54C1)�|(8CEA)
	GUI_UC_START
	"\x8F\x03\x4F\x4E\x54\xC1\x8C\xEA"
	GUI_UC_END
	"\x0"
};

const char *LowQuality[LANGUAGE_CNT] = 
{
	LowQuality_En, 
	LowQuality_SiCn, 
	LowQuality_TrCn
};

//--------------------------------------------------------------------------//
const char MsgBoxFmtUDisk_En[] = 
{
	//Format UDisk, \ncontinue?
    "Format UDisk, \nContinue?"
};

const char MsgBoxFmtUDisk_SiCn[] = 
{
	//��(683C)ʽ(5F0F)��(5316)U��(76D8)��(FF0C)��(662F)��(5426)��(7EE7)��(7EED)��FF1F
	GUI_UC_START
	"\x68\x3C\x5F\x0F\x53\x16"
	GUI_UC_END
	"U"
	GUI_UC_START
	"\x76\xD8\xFF\x0C\x66\x2F\x54\x26\x7E\xE7\x7E\xED\xFF\x1F"
	GUI_UC_END
	"\x0"
};

const char MsgBoxFmtUDisk_TrCn[] = 
{
	//��(683C)ʽ(5F0F)��(5316)U�P(76E4)��(FF0C)��(662F)��(5426)�^(7E7C)�m(7E8C)��FF1F
	GUI_UC_START
	"\x68\x3C\x5F\x0F\x53\x16"
	GUI_UC_END
	"U"
	GUI_UC_START
	"\x76\xE4\xFF\x0C\x66\x2F\x54\x26\x7E\x7C\x7E\x8C\xFF\x1F"
	GUI_UC_END
	"\x0"
};

const char *MsgBoxFmtUDisk[LANGUAGE_CNT] = 
{
	MsgBoxFmtUDisk_En, 
	MsgBoxFmtUDisk_SiCn, 
	MsgBoxFmtUDisk_TrCn
};
//--------------------------------------------------------------------------//
const char MsgBoxFmtUDiskSuc_En[] = 
{
	//Format UDisk Success!
    "Format UDisk success!"
};

const char MsgBoxFmtUDiskSuc_SiCn[] = 
{
    //��(683C)ʽ(5F0F)��(5316)U��(76D8)��(6210)��(529F)!(ff01)
	GUI_UC_START
	"\x68\x3C\x5F\x0F\x53\x16"
	GUI_UC_END
	"U"
	GUI_UC_START
    "\x76\xd8\x62\x10\x52\x9f\xff\x01"
	GUI_UC_END
	"\x0"
};

const char MsgBoxFmtUDiskSuc_TrCn[] = 
{
    //��(683C)ʽ(5F0F)��(5316)U�P(76E4)��(6210)��(529F)!
	GUI_UC_START
	"\x68\x3C\x5F\x0F\x53\x16"
	GUI_UC_END
	"U"
	GUI_UC_START
    "\x76\xe4\x62\x10\x52\x9f\xff\x01"
	GUI_UC_END
	"\x0"
};

const char *MsgBoxFmtUDiskSuc[LANGUAGE_CNT] = 
{
	MsgBoxFmtUDiskSuc_En, 
	MsgBoxFmtUDiskSuc_SiCn, 
	MsgBoxFmtUDiskSuc_TrCn
};
//--------------------------------------------------------------------------//
const char SDRemoved_En[] = 
{
	//SD Card Removed
	"SD Card Removed"
};

const char SDRemoved_SiCn[] = 
{
	//SD��(5361)��(5DF2)��(5F39)��(51FA)
	"SD"
	GUI_UC_START
	"\x53\x61\x5d\xf2\x5f\x39\x51\xfa"
	GUI_UC_END
	"\x0"
};

const char SDRemoved_TrCn[] = 
{
	//SD��(5361)��(5DF2)��(5F48)��(51FA)
	"SD"
	GUI_UC_START
	"\x53\x61\x5d\xf2\x5f\x48\x51\xfa"
	GUI_UC_END
	"\x0"
};

const char *SDRemoved[LANGUAGE_CNT] = 
{
	SDRemoved_En, 
	SDRemoved_SiCn, 
	SDRemoved_TrCn
};

//--------------------------------------------------------------------------//
const char SDDetected_En[] = 
{
	//SD Card Detected
	"SD Card Detected"
};

const char SDDetected_SiCn[] = 
{
	//��(68C0)��(6D4B)��(5230)SD��(5361)
	GUI_UC_START
	"\x68\xc0\x6d\x4b\x52\x30"
	GUI_UC_END
	"SD"
	GUI_UC_START
	"\x53\x61"
	GUI_UC_END
	"\x0"
};

const char SDDetected_TrCn[] = 
{
	//�z(6AA2)�y(6E2C)��(5230)SD��(5361)
	GUI_UC_START
	"\x6a\xa2\x6e\x2c\x52\x30"
	GUI_UC_END
	"SD"
	GUI_UC_START
	"\x53\x61"
	GUI_UC_END
	"\x0"
};

const char *SDDetected[LANGUAGE_CNT] = 
{
	SDDetected_En, 
	SDDetected_SiCn, 
	SDDetected_TrCn
};

//--------------------------------------------------------------------------//
const char UDiskRemoved_En[] = 
{
	//UDisk Removed
	"UDisk Removed"
};

const char UDiskRemoved_SiCn[] = 
{
	//U��(76D8)��(5DF2)��(62D4)��(51FA)
	"U"
	GUI_UC_START
	"\x76\xd8\x5d\xf2\x62\xD4\x51\xFA"
	GUI_UC_END
	"\x0"
};

const char UDiskRemoved_TrCn[] = 
{
	//U�P(76E4)��(5DF2)��(62D4)��(51FA)
	"U"
	GUI_UC_START
	"\x76\xe4\x5D\xF2\x62\xD4\x51\xFA"
	GUI_UC_END
	"\x0"
};

const char *UDiskRemoved[LANGUAGE_CNT] = 
{
	UDiskRemoved_En, 
	UDiskRemoved_SiCn, 
	UDiskRemoved_TrCn
};

//--------------------------------------------------------------------------//
const char UDiskDetected_En[] = 
{
    //UDisk Detected
	"UDisk Detected"
};

const char UDiskDetected_SiCn[] = 
{
	//U��(76D8)��(5DF2)��(52A0)��(8F7D)
	"U"
	GUI_UC_START
	"\x76\xd8\x5d\xf2\x52\xa0\x8f\x7d"
	GUI_UC_END
	"\x0"
};

const char UDiskDetected_TrCn[] = 
{
	//U�P(76E4)��(5DF2)��(52A0)�d(8F09)
	"U"
	GUI_UC_START
	"\x76\xe4\x5d\xf2\x52\xa0\x8f\x09"
	GUI_UC_END
	"\x0"
};

const char *UDiskDetected[LANGUAGE_CNT] = 
{
	UDiskDetected_En, 
	UDiskDetected_SiCn, 
	UDiskDetected_TrCn
};

//--------------------------------------------------------------------------//
const char HelpSDRemove_En[] = 
{
    //Please insert SDCard
    "Please insert SDCard"
};

const char HelpSDRemove_SiCn[] = 
{
	//��(8BF7)��(63D2)��(5165)SD��(5361)
	GUI_UC_START
	"\x8b\xf7\x63\xd2\x51\x65"
	GUI_UC_END
	"SD"
	GUI_UC_START
	"\x53\x61"
	GUI_UC_END
	"\x0"
};

const char HelpSDRemove_TrCn[] = 
{
	//Ո(8ACB)��(63D2)��(5165)SD��(5361)
	GUI_UC_START
	"\x8a\xcb\x63\xd2\x51\x65"
	GUI_UC_END
	"SD"
	GUI_UC_START
	"\x53\x61"
	GUI_UC_END
	"\x0"
};

const char *HelpSDRemove[LANGUAGE_CNT] = 
{
	HelpSDRemove_En, 
	HelpSDRemove_SiCn, 
	HelpSDRemove_TrCn
};

//--------------------------------------------------------------------------//
const char SDCardFull_En[] = 
{
	//SDCard Full
	"SDCard Full"
};

const char SDCardFull_SiCn[] = 
{
	//SD��(5361)��(5B58)��(50A8)��(5DF2)��(6EE1)
	"SD"
	GUI_UC_START
	"\x53\x61\x5b\x58\x50\xa8\x5d\xf2\x6e\xe1"
	GUI_UC_END
	"\x0"
};

const char SDCardFull_TrCn[] = 
{
	//SD��(5361)��(5B58)��(5132)��(5DF2)�M(6EFF)
	"SD"
	GUI_UC_START
	"\x53\x61\x5b\x58\x51\x32\x5d\xf2\x6e\xff"
	GUI_UC_END
	"\x0"
};

const char *SDCardFull[LANGUAGE_CNT] = 
{
	SDCardFull_En,
	SDCardFull_SiCn,
	SDCardFull_TrCn
};
//--------------------------------------------------------------------------//
const char UpdataSuc_En[] = 
{
	//Update Success!
    "Update success!"
};

const char UpdataSuc_SiCn[] = 
{
	//��(5347)��(7EA7)��(6210)��(529F)!
	GUI_UC_START
    "\x53\x47\x7E\xA7\x62\x10\x52\x9F\xff\x01"
	GUI_UC_END
	"\x0"
};

const char UpdataSuc_TrCn[] = 
{
	//��(5347)��(7D1A)��(6210)��(529F)!
	GUI_UC_START
    "\x53\x47\x7D\x1A\x62\x10\x52\x9F\xff\x01"
	GUI_UC_END
	"\x0"
};

const char *UpdataSuc[LANGUAGE_CNT] = 
{
	UpdataSuc_En, 
	UpdataSuc_SiCn, 
	UpdataSuc_TrCn
};

//--------------------------------------------------------------------------//
const char UpdataFailed_En[] = 
{
	//Update Failed!
	"Update Failed!"
};

const char UpdataFailed_SiCn[] = 
{
	//��(5347)��(7EA7)ʧ(5931)��(8D25)
	GUI_UC_START
    "\x53\x47\x7E\xA7\x59\x31\x8D\x25\xff\x01"
	GUI_UC_END
	"\x0"
};

const char UpdataFailed_TrCn[] = 
{
    //��(5347)��(7D1A)ʧ(5931)��(6557)!
	GUI_UC_START
    "\x53\x47\x7D\x1A\x59\x31\x65\x57\xff\x01"
	GUI_UC_END
	"\x0"
};

const char *UpdataFailed[LANGUAGE_CNT] = 
{
	UpdataFailed_En, 
	UpdataFailed_SiCn, 
	UpdataFailed_TrCn
};

//--------------------------------------------------------------------------//
const char HelpMountUDisk_En[] = 
{
    //zqh 2015��3��25�� modify�� ����Please mount UDisk first����Ϊ��UDisk is not connected Please mount UDisk first��
    //UDisk is not connected
	//Please mount UDisk first
    "UDisk is not connected\nPlease insert U disk and connect."
};

const char HelpMountUDisk_SiCn[] = 
{
    //zqh 2015��3��25�� modify���������Ȳ���U�̲����ӡ���Ϊ��U��δ�������Ȳ���U�̲����ӣ���
    //U��δ����
    "U"
    GUI_UC_START
    "\x76\xD8\x67\x2a\x8F\xde\x63\xA5"
    GUI_UC_END
    "\n"

    //��(8BF7)��(5148)��(52A0)��(8F7D)U��(76D8)
    //��(8BF7)��(5148)��(63D2)��(5165)U��(76D8)��(5E76)��(8FDE)��(63A5)��
	GUI_UC_START
	"\x8b\xf7\x51\x48\x63\xD2\x51\x65"
	GUI_UC_END
	"U"
	GUI_UC_START
    "\x76\xD8\x5E\x76\x8F\xDE\x63\xA5\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char HelpMountUDisk_TrCn[] = 
{
    //zqh 2015��3��25�� modify������Ո�ȳ���U�P�K�B�ӡ��Ğ顰U�Pδ�B�ӣ�Ո�Ȳ���U�P�K�B�ӣ���
    //U�Pδ�B��
    "U"
    GUI_UC_START
    "\x76\xE4\x67\x2A\x90\x23\x63\xa5"
    GUI_UC_END
    "\n"

    //Ո(8ACB)��(5148)��(52A0)�d(8F09)U�P(76E4)
	//Ո(8ACB)��(5148)��(63D2)��(5165)U�P(76E4)�K(4E26)�B(9023)��(63A5)
	GUI_UC_START
	"\x8a\xcb\x51\x48\x63\xD2\x51\x65"
	GUI_UC_END
	"U"
	GUI_UC_START
    "\x76\xe4\x4E\x26\x90\x23\x63\xA5\xFF\x01"
	GUI_UC_END
	"\x0"
};

const char *HelpMountUDisk[LANGUAGE_CNT] = 
{
	HelpMountUDisk_En, 
	HelpMountUDisk_SiCn, 
	HelpMountUDisk_TrCn
};

//--------------------------------------------------------------------------//
const char FileCopy_En[] = 
{
    #if 0
    //File Copy, Continue
    "File Copy, Continue"
    #else
    //File Copy
    "File Copy"
    #endif
};

const char FileCopy_SiCn[] = 
{
    #if 0
    //zqh 2015��3��24�� modify�� �����ļ����ơ���Ϊ�������ļ����Ƿ����?��
    //�����ļ����Ƿ������
    GUI_UC_START
    "\x59\x0D\x52\x36\x65\x87\x4E\xF6\xFF\x0C\x66\x2F\x54\x26\x7E\xE7\x7E\xED\xFF\x1F"
    GUI_UC_END
    "\x0"
    #else
    //zqh 2015��3��30�� modify
    //�ļ�����
    GUI_UC_START
    "\x65\x87\x4E\xF6\x59\x0D\x52\x36"
    GUI_UC_END
    "\x0"
    #endif
};

const char FileCopy_TrCn[] = 
{
    #if 0
    //zqh 2015��3��24�� modify�������n�}�u����Ϊ���}�u�n�����Ƿ��^�m����
    //�}�u�n�����Ƿ��^�m��
    GUI_UC_START
    "\x89\x07\x88\xFD\x6A\x94\x68\x48\xFF\x0C\x66\x2F\x54\x26\x7E\x7C\x7E\x8C\xFF\x1F"
    GUI_UC_END
    "\x0"
    #else
    //zqh 2015��3��24�� modify
    //�n���}�u
    GUI_UC_START
    "\x6A\x94\x68\x48\x89\x07\x88\xFD"
    GUI_UC_END
    "\x0"

    #endif
};

const char *FileCopy[LANGUAGE_CNT] = 
{
	FileCopy_En, 
	FileCopy_SiCn, 
	FileCopy_TrCn
};

/*
//zqh 2015��3��30�� add��
//--------------------------------------------------------------------------//
const char FileName_En[] =
{
    //File Name
    "File Name"
};

const char FileName_SiCn[] =
{
    //zqh 2015��3��24�� modify�� �����ļ����ơ���Ϊ�������ļ����Ƿ����?��
    //�ļ���(\x6587\x4ef6\x540d)
    GUI_UC_START
    "\x65\x87\x4e\xf6\x54\x0d"
    GUI_UC_END
    "\x0"
};

const char FileName_TrCn[] =
{
    //zqh 2015��3��24�� modify�������n�}�u����Ϊ���}�u�n�����Ƿ��^�m����
    //�n����(\x6a94\x6848\x540d)
    GUI_UC_START
    "\x6a\x94\x68\x48\x54\x0d"
    GUI_UC_END
    "\x0"
};

const char *FileName[LANGUAGE_CNT] =
{
    FileName_En,
    FileName_SiCn,
    FileName_TrCn
};
*/

//zqh 2015��3��30�� add��
//--------------------------------------------------------------------------//
const char FileCopyHelpInfo_En[] =
{
    //File Copy
    "File copy, continue?"
};

const char FileCopyHelpInfo_SiCn[] =
{
    //zqh 2015��3��24�� modify�� �����ļ����ơ���Ϊ�������ļ����Ƿ����?��
    //�����ļ����Ƿ������
    GUI_UC_START
    "\x59\x0D\x52\x36\x65\x87\x4E\xF6\xFF\x0C\x66\x2F\x54\x26\x7E\xE7\x7E\xED\xFF\x1F"
    GUI_UC_END
    "\x0"
};

const char FileCopyHelpInfo_TrCn[] =
{
    //zqh 2015��3��24�� modify�������n�}�u����Ϊ���}�u�n�����Ƿ��^�m����
    //�}�u�n�����Ƿ��^�m��
    GUI_UC_START
    "\x89\x07\x88\xFD\x6A\x94\x68\x48\xFF\x0C\x66\x2F\x54\x26\x7E\x7C\x7E\x8C\xFF\x1F"
    GUI_UC_END
    "\x0"
};

const char *FileCopyHelpInfo[LANGUAGE_CNT] =
{
    FileCopyHelpInfo_En,
    FileCopyHelpInfo_SiCn,
    FileCopyHelpInfo_TrCn
};

//--------------------------------------------------------------------------//
const char SystemInfo_En[] = 
{
	//System Infomation
	"System Infomation"
};

const char SystemInfo_SiCn[] = 
{
	//ϵ(7CFB)ͳ(7EDF)��(4FE1)Ϣ(606F)
	GUI_UC_START
	"\x7c\xfb\x7e\xdf\x4f\xe1\x60\x6f"
	GUI_UC_END
	"\x0"
};

const char SystemInfo_TrCn[] = 
{
	//ϵ(7CFB)�y(7D71)��(4FE1)Ϣ(606F)
	GUI_UC_START
	"\x7c\xfb\x7d\x71\x4f\xe1\x60\x6f"
	GUI_UC_END
	"\x0"
};

const char *SystemInfo[LANGUAGE_CNT] =
{
    SystemInfo_En,
    SystemInfo_SiCn,
    SystemInfo_TrCn
};

//zqh 2015��3��30�� add����ӡ�ϵͳ���ܡ���ʾ��Ϣ

//--------------------------------------------------------------------------//
const char SystemFunction_En[] =
{
    //System Function
    "System Function"
};

const char SystemFunction_SiCn[] =
{
    //ϵ(7CFB)ͳ(7EDF)��(529f)��(80fd)
    GUI_UC_START
    "\x7c\xfb\x7e\xdf\x52\x9f\x80\xfd"
    GUI_UC_END
    "\x0"
};

const char SystemFunction_TrCn[] =
{
    //ϵ(7CFB)�y(7D71)��(529f)��(80fd)
    GUI_UC_START
    "\x7c\xfb\x7d\x71\x52\x9f\x80\xfd"
    GUI_UC_END
    "\x0"
};

const char *SystemFunction[LANGUAGE_CNT] =
{
    SystemFunction_En,
    SystemFunction_SiCn,
    SystemFunction_TrCn
};

//zqh 2015��3��30�� add����ӡ�����汾����ʾ��Ϣ

//--------------------------------------------------------------------------//
const char SoftwareVersion_En[] =
{
    //Software Version
    "Software Version"
};

const char SoftwareVersion_SiCn[] =
{
    //����汾(\x8f6f\x4ef6\x7248\x672c)
    GUI_UC_START
    "\x8f\x6f\x4e\xf6\x72\x48\x67\x2c"
    GUI_UC_END
    "\x0"
};

const char SoftwareVersion_TrCn[] =
{
    //ܛ���汾(\x8edf\x4ef6\x7248\x672c)
    GUI_UC_START
    "\x8e\xdf\x4e\xf6\x72\x48\x67\x2c"
    GUI_UC_END
    "\x0"
};

const char *SoftwareVersion[LANGUAGE_CNT] =
{
    SoftwareVersion_En,
    SoftwareVersion_SiCn,
    SoftwareVersion_TrCn
};


//--------------------------------------------------------------------------//
const char KernelUpdata_En[] = 
{
	//Kernel Update
	"Kernel Update"
};

const char KernelUpdata_SiCn[] = 
{
	//��(5185)��(6838)��(5347)��(7EA7)
	GUI_UC_START
	"\x51\x85\x68\x38\x53\x47\x7E\xA7"
	GUI_UC_END
	"\x0"
};

const char KernelUpdata_TrCn[] = 
{
	//��(6838)��(5FC3)��(5347)��(7D1A)
	GUI_UC_START
	"\x68\x38\x5f\xc3\x53\x47\x7d\x1a"
	GUI_UC_END
	"\x0"
};

const char *KernelUpdata[LANGUAGE_CNT] = 
{
	KernelUpdata_En, 
	KernelUpdata_SiCn, 
	KernelUpdata_TrCn
};

//--------------------------------------------------------------------------//
const char ubootUpdate_En[] = 
{
    //Uboot Update
    "Uboot Update"
};

const char ubootUpdate_SiCn[] = 
{
	//uboot��(5347)��(7EA7)
    "Uboot"
	GUI_UC_START
	"\x53\x47\x7E\xA7"
	GUI_UC_END
	"\x0"
};

const char ubootUpdate_TrCn[] = 
{
	//uboot��(5347)��(7D1A)
    "Uboot"
	GUI_UC_START
	"\x53\x47\x7D\x1A"
	GUI_UC_END
	"\x0"
};

const char *ubootUpdate[LANGUAGE_CNT] = 
{
	ubootUpdate_En, 
	ubootUpdate_SiCn, 
	ubootUpdate_TrCn
};

//zqh 2015��3��30�� modify�������ļ�ϵ�y��������Ϊ������Pvr��

//--------------------------------------------------------------------------//
const char FileSystemUpdate_En[] = 
{
    #if 0
    //File System Update
	"File System Update"
    #else
    //Update Pvr
    "Update Pvr"
    #endif
};

const char FileSystemUpdate_SiCn[] = 
{
    #if 0
    //��(6587)��(4EF6)ϵ(7CFB)ͳ(7EDF)��(5347)��(7EA7)
	GUI_UC_START
	"\x65\x87\x4e\xf6\x7c\xfb\x7e\xdf\x53\x47\x7e\xa7"
	GUI_UC_END
	"\x0"
    #else
    //����Pvr
    GUI_UC_START
    "\x53\x47\x7e\xa7"
    GUI_UC_END
    "Pvr\x0"
    #endif
};

const char FileSystemUpdate_TrCn[] = 
{
    #if 0
    //�n(6A94)��(6848)ϵ(7CFB)�y(7D71)��(5347)��(7D1A)
	GUI_UC_START
	"\x6a\x94\x68\x48\x7c\xfb\x7d\x71\x53\x47\x7d\x1a"
	GUI_UC_END
	"\x0"
    #else

    //��(5347)��(7D1A)Pvr
    GUI_UC_START
    "\x53\x47\x7d\x1a"
    GUI_UC_END
    "Pvr\x0"

    #endif
};

const char *FileSystemUpdate[LANGUAGE_CNT] = 
{
	FileSystemUpdate_En, 
	FileSystemUpdate_SiCn, 
	FileSystemUpdate_TrCn
};

//--------------------------------------------------------------------------//
const char UpdateDaemon_En[] = 
{
	//Update Daemon
	"Update Daemon"
};

const char UpdateDaemon_SiCn[] = 
{
	//��(5347)��(7EA7)Daemon
	GUI_UC_START
	"\x53\x47\x7e\xa7"
	GUI_UC_END
	"Daemon\x0"
};

const char UpdateDaemon_TrCn[] = 
{
	//��(5347)��(7D1A)Daemon
	GUI_UC_START
	"\x53\x47\x7d\x1a"
	GUI_UC_END
	"Daemon\x0"
};

const char *UpdateDaemon[LANGUAGE_CNT] = 
{
	UpdateDaemon_En, 
	UpdateDaemon_SiCn, 
	UpdateDaemon_TrCn
};

//--------------------------------------------------------------------------//
const char NoNeedUpdate_En[] = 
{
    //Do not need update
    "Do not need update"
};

const char NoNeedUpdate_SiCn[] = 
{
	//��(4E0D)��(9700)Ҫ(8981)��(8FDB)��(884C)��(66F4)��(65B0)
	GUI_UC_START
	"\x4e\x0d\x97\x00\x89\x81\x8f\xdb\x88\x4c\x66\xf4\x65\xb0"
	GUI_UC_END
	"\x0"
};

const char NoNeedUpdate_TrCn[] = 
{
	//��(4E0D)��(9700)Ҫ(8981)�M(9032)��(884C)��(66F4)��(65B0) 
	GUI_UC_START
	"\x4e\x0d\x97\x00\x89\x81\x90\x32\x88\x4c\x66\xf4\x65\xb0"
	GUI_UC_END
	"\x0"
};

const char *NoNeedUpdate[LANGUAGE_CNT] = 
{
	NoNeedUpdate_En, 
	NoNeedUpdate_SiCn, 
	NoNeedUpdate_TrCn
};


const char PlayBackFinished_En[] = 
{
	//Play Back Finished
    "Play back finished!"
};

const char PlayBackFinished_SiCn[] = 
{
    //��(64AD)��(653E)��(7ED3)��(675F)!
	GUI_UC_START
    "\x64\xAD\x65\x3E\x7E\xD3\x67\x5F\xff\x01"
	GUI_UC_END
	"\x0"
};

const char PlayBackFinished_TrCn[] = 
{
    //��(64AD)��(653E)�Y(7D50)��(675F)!
	GUI_UC_START
    "\x64\xAD\x65\x3E\x7D\x50\x67\x5F\xff\x01"
	GUI_UC_END
	"\x0"
};

const char *PlayBackFinished[LANGUAGE_CNT] = 
{
	PlayBackFinished_En, 
	PlayBackFinished_SiCn, 
	PlayBackFinished_TrCn
};

const char PlayBackErr_En[] = 
{
	//Play Back Error
    "Play back error!"
};

const char PlayBackErr_SiCn[] = 
{
    //��(64AD)��(653E)��(9519)��(8BEF)!
	GUI_UC_START
    "\x64\xAD\x65\x3E\x95\x19\x8B\xEF\xff\x01"
	GUI_UC_END
	"\x0"
};

const char PlayBackErr_TrCn[] = 
{
    //��(64AD)��(653E)�e(932F)�`(8AA4)!
	GUI_UC_START
    "\x64\xAD\x65\x3E\x93\x2F\x8A\xA4\xff\x01"
	GUI_UC_END
	"\x0"
};

const char *PlayBackErr[LANGUAGE_CNT] = 
{
	PlayBackErr_En, 
	PlayBackErr_SiCn, 
	PlayBackErr_TrCn
};

const char RecordFailed_En[] = 
{
    //Record Failed!
    "Record failed!"
};

const char RecordFailed_SiCn[] = 
{
    //¼(5F55)��(5236)ʧ(5931)��(8D25)!
	GUI_UC_START
    "\x5F\x55\x52\x36\x59\x31\x8D\x25\xff\x01"
	GUI_UC_END
	"\x0"
};

const char RecordFailed_TrCn[] = 
{
    //�(9304)�u(88FD)ʧ(5931)��(6557)!
	GUI_UC_START
    "\x93\x04\x88\xFD\x59\x31\x65\x57\xff\x01"
	GUI_UC_END
	"\x0"
};

const char *RecordFailed[LANGUAGE_CNT] = 
{
	RecordFailed_En, 
	RecordFailed_SiCn, 
	RecordFailed_TrCn
};

//--------------------------------------------------------------------------//
const char CFRemoved_En[] = 
{
    //Electronic disk not found
    "Electronic disk not found!"
};

const char CFRemoved_SiCn[] = 
{
    //zqh 2015��3��20�� modify
    //δ(672A)��(68C0)��(6D4B)��(5230)������!
	GUI_UC_START
    "\x67\x2A\x68\xC0\x6d\x4b\x52\x30\x75\x35\x5b\x50\x76\xd8\xff\x01"
	GUI_UC_END
	"\x0"
};

const char CFRemoved_TrCn[] = 
{
    //zqh 2015��3��20�� modify
    //δ(672A)�z(6AA2)�y(6E2C)��(5230)��ӱP!
	GUI_UC_START
    "\x67\x2a\x6a\xa2\x6e\x2c\x52\x30\x96\xfb\x5b\x50\x76\xe4\xff\x01"
	GUI_UC_END
	"\x0"
};

const char *CFRemoved[LANGUAGE_CNT] = 
{
	CFRemoved_En, 
	CFRemoved_SiCn, 
	CFRemoved_TrCn
};
//--------------------------------------------------------------------------//
const char CFDetected_En[] = 
{
    //Electronic disk detected
    "Electronic disk detected!"
};

const char CFDetected_SiCn[] = 
{
    //zqh 2015��3��20�� modify
    //��(68C0)��(6D4B)��(5230)������!
	GUI_UC_START
    "\x68\xc0\x6d\x4b\x52\x30\x75\x35\x5b\x50\x76\xd8\xff\x01"
	GUI_UC_END
	"\x0"
};

const char CFDetected_TrCn[] = 
{
    //zqh 2015��3��20�� modify
    //�z(6AA2)�y(6E2C)��(5230)��ӱP!
	GUI_UC_START
    "\x6a\xa2\x6e\x2c\x52\x30\x96\xfb\x5b\x50\x76\xe4\xff\x01"
	GUI_UC_END
	"\x0"
};

const char *CFDetected[LANGUAGE_CNT] = 
{
	CFDetected_En, 
	CFDetected_SiCn, 
	CFDetected_TrCn
};

//--------------------------------------------------------------------------//
const char SpecificFile_En[] = 
{
    //Don't delete specific file
    "Don't delete specific file"
};

const char SpecificFile_SiCn[] = 
{
	//��(7279)��(5B9A)��(6587)��(4EF6)��(4E0D)��(80FD)ɾ(5220)��(9664)
	GUI_UC_START
	"\x72\x79\x5b\x9a\x65\x87\x4e\xf6\x4e\x0d\x80\xfd\x52\x20\x96\x64"
	GUI_UC_END
	"\x0"
};

const char SpecificFile_TrCn[] = 
{
	//��(7279)��(5B9A)��(6587)�n(6A94)��(4E0D)��(80FD)�h(522A)��(9664)
	GUI_UC_START
	"\x72\x79\x5b\x9a\x65\x87\x6a\x94\x4e\x0d\x80\xfd\x52\x2a\x96\x64"
	GUI_UC_END
	"\x0"
};

const char *SpecificFile[LANGUAGE_CNT] = 
{
	SpecificFile_En, 
	SpecificFile_SiCn, 
	SpecificFile_TrCn
};

//zqh 2015��3��25�� modify������û�������ļ�����Ϊ��û�������ļ��� uImage��
//--------------------------------------------------------------------------//
const char NoUpdateFileuImage_En[] =
{
	//No update file
    "No update file: uImage"
};

const char NoUpdateFileuImage_SiCn[] =
{
	//û(6CA1)��(6709)��(5347)��(7EA7)��(6587)��(4EF6)
	GUI_UC_START
	"\x6c\xa1\x67\x09\x53\x47\x7e\xa7\x65\x87\x4e\xf6"
	GUI_UC_END
    ": uImage"
	"\x0"
};

const char NoUpdateFileuImage_TrCn[] =
{
	//�](6C92)��(6709)��(5347)��(7D1A)��(6587)��(4EF6)
	GUI_UC_START
	"\x6c\x92\x67\x09\x53\x47\x7d\x1a\x65\x87\x4e\xf6"
	GUI_UC_END
    ": uImage"
	"\x0"
};

const char *NoUpdateFileuImage[LANGUAGE_CNT] =
{
    NoUpdateFileuImage_En,
    NoUpdateFileuImage_SiCn,
    NoUpdateFileuImage_TrCn
};

// zqh 2015��3��25�� modify�� �����]�������ļ����Ğ顰�]�������ļ��� pvr��
//--------------------------------------------------------------------------//
const char NoUpdateFilePvr_En[] =
{
    //No update file
    "No update file: pvr"
};

const char NoUpdateFilePvr_SiCn[] =
{
    //û(6CA1)��(6709)��(5347)��(7EA7)��(6587)��(4EF6)
    GUI_UC_START
    "\x6c\xa1\x67\x09\x53\x47\x7e\xa7\x65\x87\x4e\xf6"
    GUI_UC_END
    ": pvr"
    "\x0"
};

const char NoUpdateFilePvr_TrCn[] =
{
    //�](6C92)��(6709)��(5347)��(7D1A)��(6587)��(4EF6)
    GUI_UC_START
    "\x6c\x92\x67\x09\x53\x47\x7d\x1a\x65\x87\x4e\xf6"
    GUI_UC_END
    ": pvr"
    "\x0"
};

const char *NoUpdateFilePvr[LANGUAGE_CNT] =
{
    NoUpdateFilePvr_En,
    NoUpdateFilePvr_SiCn,
    NoUpdateFilePvr_TrCn
};
//

//zqh 2015��3��25�� modify������û�������ļ�����Ϊ��û�������ļ���daemon��
//--------------------------------------------------------------------------//
const char NoUpdateFileDaemon_En[] =
{
    //No update file
    "No update file: daemon"
};

const char NoUpdateFileDaemon_SiCn[] =
{
    //û(6CA1)��(6709)��(5347)��(7EA7)��(6587)��(4EF6)
    GUI_UC_START
    "\x6c\xa1\x67\x09\x53\x47\x7e\xa7\x65\x87\x4e\xf6"
    GUI_UC_END
    ": daemon"
    "\x0"
};

const char NoUpdateFileDaemon_TrCn[] =
{
    //�](6C92)��(6709)��(5347)��(7D1A)��(6587)��(4EF6)
    GUI_UC_START
    "\x6c\x92\x67\x09\x53\x47\x7d\x1a\x65\x87\x4e\xf6"
    GUI_UC_END
    ": daemon"
    "\x0"
};

const char *NoUpdateFileDaemon[LANGUAGE_CNT] =
{
    NoUpdateFileDaemon_En,
    NoUpdateFileDaemon_SiCn,
    NoUpdateFileDaemon_TrCn
};

//--------------------------------------------------------------------------//
const char DoKernelUpdate_En[] = 
{
	//Do Kernel Update?
    "Do kernel update?"
};

const char DoKernelUpdate_SiCn[] = 
{
	//ȷ(786E)��(5B9A)��(5347)��(7EA7)��(5185)��(6838)?
	GUI_UC_START
    "\x78\x6e\x5b\x9a\x53\x47\x7e\xa7\x51\x85\x68\x38\xff\x1f"
	GUI_UC_END
	"\x0"
};

const char DoKernelUpdate_TrCn[] = 
{
	//�_(78BA)��(5B9A)��(5347)��(7D1A)��(5167)��(6838)?
	GUI_UC_START
    "\x78\xba\x5b\x9a\x53\x47\x7d\x1a\x51\x67\x68\x38\xff\x1f"
	GUI_UC_END
	"\x0"
};

const char *DoKernelUpdate[LANGUAGE_CNT] = 
{
	DoKernelUpdate_En,
	DoKernelUpdate_SiCn,
	DoKernelUpdate_TrCn
};

//--------------------------------------------------------------------------//
const char FileBackUp_En[] = 
{
	//File Backup
    "File Backup"
};

const char FileBackUp_SiCn[] = 
{
	//��(5907)��(4EFD)��(6587)��(4EF6)
	GUI_UC_START
	"\x59\x07\x4E\xFD\x65\x87\x4E\xF6"
	GUI_UC_END
	"\x0"
};

const char FileBackUp_TrCn[] = 
{
	//��(5099)��(4EFD)��(6587)�n(6A94)
	GUI_UC_START
	"\x50\x99\x4e\xfd\x65\x87\x6a\x94"
	GUI_UC_END
	"\x0"
};

const char *FileBackUp[LANGUAGE_CNT] = 
{
	FileBackUp_En,
	FileBackUp_SiCn,
	FileBackUp_TrCn
};

//--------------------------------------------------------------------------//
const char FileCopyBack_En[] = 
{
	//File Copy Back
    "File copy back"
};

const char FileCopyBack_SiCn[] = 
{
	//��(6062)��(590D)��(6587)��(4EF6)
	GUI_UC_START
	"\x60\x62\x59\x0d\x65\x87\x4E\xF6"
	GUI_UC_END
	"\x0"
};

const char FileCopyBack_TrCn[] = 
{
	//��(6062)��(5FA9)��(6587)�n(6A94)
	GUI_UC_START
	"\x60\x62\x5f\xa9\x65\x87\x6a\x94"
	GUI_UC_END
	"\x0"
};

const char *FileCopyBack[LANGUAGE_CNT] = 
{
	FileCopyBack_En,
	FileCopyBack_SiCn,
	FileCopyBack_TrCn
};

//--------------------------------------------------------------------------//
const char FileBackUpSuc_En[] = 
{
	//File Backup Success
    "File backup success!"
};

const char FileBackUpSuc_SiCn[] = 
{
    //��(5907)��(4EFD)��(6587)��(4EF6)��(6210)��(529F)!
	GUI_UC_START
    "\x59\x07\x4E\xFD\x65\x87\x4E\xF6\x62\x10\x52\x9f\xff\x01"
	GUI_UC_END
	"\x0"
};

const char FileBackUpSuc_TrCn[] = 
{
    //��(5099)��(4EFD)��(6587)�n(6A94)��(6210)��(529F)!
	GUI_UC_START
    "\x50\x99\x4e\xfd\x65\x87\x6a\x94\x62\x10\x52\x9f\xff\x01"
	GUI_UC_END
	"\x0"
};

const char *FileBackUpSuc[LANGUAGE_CNT] = 
{
	FileBackUpSuc_En,
	FileBackUpSuc_SiCn,
	FileBackUpSuc_TrCn
};

//--------------------------------------------------------------------------//
const char FileBackUpFail_En[] = 
{
    //File Backup Failed!
    "File backup failed!"
};

const char FileBackUpFail_SiCn[] = 
{
    //��(5907)��(4EFD)��(6587)��(4EF6)ʧ(5931)��(8D25)!
	GUI_UC_START
    "\x59\x07\x4E\xFD\x65\x87\x4E\xF6\x59\x31\x8d\x25\xff\x1f"
	GUI_UC_END
	"\x0"
};

const char FileBackUpFail_TrCn[] = 
{
    //��(5099)��(4EFD)��(6587)�n(6A94)ʧ(5931)��(6557)!
	GUI_UC_START
    "\x50\x99\x4e\xfd\x65\x87\x6a\x94\x59\x31\x65\x57\xff\x1f"
	GUI_UC_END
	"\x0"
};

const char *FileBackUpFail[LANGUAGE_CNT] = 
{
	FileBackUpFail_En,
	FileBackUpFail_SiCn,
	FileBackUpFail_TrCn
};

//--------------------------------------------------------------------------//
const char FileCopyBackSuc_En[] = 
{
    //File copy back success
    "File copy back success!"
};

const char FileCopyBackSuc_SiCn[] = 
{
    //��(6062)��(590D)��(6587)��(4EF6)��(6210)��(529F)!
	GUI_UC_START
    "\x60\x62\x59\x0d\x65\x87\x4E\xF6\x62\x10\x52\x9f\xff\x1f"
	GUI_UC_END
	"\x0"
};

const char FileCopyBackSuc_TrCn[] = 
{
    //��(6062)��(5FA9)��(6587)�n(6A94)��(6210)��(529F)!
	GUI_UC_START
    "\x60\x62\x5f\xa9\x65\x87\x6a\x94\x62\x10\x52\x9f\xff\x1f"
	GUI_UC_END
	"\x0"
};

const char *FileCopyBackSuc[LANGUAGE_CNT] = 
{
	FileCopyBackSuc_En,
	FileCopyBackSuc_SiCn,
	FileCopyBackSuc_TrCn
};

//--------------------------------------------------------------------------//
const char FileCopyBackFail_En[] = 
{
    //File Copy Back Failed!
    "File copy back failed!"
};

const char FileCopyBackFail_SiCn[] = 
{
    //��(6062)��(590D)��(6587)��(4EF6)ʧ(5931)��(8D25)!
	GUI_UC_START
    "\x60\x62\x59\x0d\x65\x87\x4E\xF6\x59\x31\x8d\x25\xff\x1f"
	GUI_UC_END
	"\x0"
};

const char FileCopyBackFail_TrCn[] = 
{
    //��(6062)��(5FA9)��(6587)�n(6A94)ʧ(5931)��(6557)!
	GUI_UC_START
    "\x60\x62\x5f\xa9\x65\x87\x6a\x94\x59\x31\x65\x57\xff\x1f"
	GUI_UC_END
	"\x0"
};

const char *FileCopyBackFail[LANGUAGE_CNT] = 
{
	FileCopyBackFail_En,
	FileCopyBackFail_SiCn,
	FileCopyBackFail_TrCn
};

//--------------------------------------------------------------------------//
const char FileUpdate_En[] = 
{
	//File Update
    "File Update"
};

const char FileUpdate_SiCn[] = 
{
	//��(6587)��(4EF6)��(5347)��(7EA7)
	GUI_UC_START
	"\x65\x87\x4e\xf6\x53\x47\x7e\xa7"
	GUI_UC_END
	"\x0"
};

const char FileUpdate_TrCn[] = 
{
	//�n(6A94)��(6848)��(5347)��(7D1A)
	GUI_UC_START
	"\x6a\x94\x68\x48\x53\x47\x7d\x1a"
	GUI_UC_END
	"\x0"
};

const char *FileUpdate[LANGUAGE_CNT] = 
{
	FileUpdate_En, 
	FileUpdate_SiCn, 
	FileUpdate_TrCn
};

//--------------------------------------------------------------------------//
const char SystemReboot_En[] = 
{
    //zqh 2015��3��20�� modify������system reboot����Ϊ��wait for the system to reboot��
    //Wait For The System Reboot
    "Wait for the system reboot"
};

const char SystemReboot_SiCn[] = 
{
    //zqh 2015��3��20�� modify������ϵͳ��������Ϊ���ȴ�ϵͳ������
    //��(7B49)��(5F85)ϵ(7CFB)ͳ(7EDF)��(91CD)��(542F)
	GUI_UC_START
    "\x7b\x49\x5f\x85\x7c\xFB\x7E\xDF\x91\xcd\x54\x2f"
	GUI_UC_END
	"\x0"
};

const char SystemReboot_TrCn[] = 
{
    //zqh 2015��3��20�� modify������ϵ�y�؆����Ğ顰�ȴ�ϵ�y�؆���
    //��(7B49)��(5F85)ϵ(7CFB)�y(7D71)��(91CD)��(555F)
	GUI_UC_START
    "\x7b\x49\x5f\x85\x7c\xfb\x7d\x71\x91\xcd\x55\x5f"
	GUI_UC_END
	"\x0"
};

const char *SystemReboot[LANGUAGE_CNT] = 
{
	SystemReboot_En, 
	SystemReboot_SiCn, 
	SystemReboot_TrCn
};

//--------------------------------------------------------------------------//
const char CFCardFull_En[] = 
{
    //Electronic disk Full
    "Electronic disk full"
};

const char CFCardFull_SiCn[] = 
{
    //zqh 2015��3��20�� modify
    //������()��(5B58)��(50A8)��(5DF2)��(6EE1)
	GUI_UC_START
    "\x75\x35\x5b\x50\x76\xd8\x5b\x58\x50\xa8\x5d\xf2\x6e\xe1"
	GUI_UC_END
	"\x0"
};

const char CFCardFull_TrCn[] = 
{
    //zqh 2015��3��20�� modify
    //��ӱP��()��(5132)��(5DF2)�M(6EFF)
	GUI_UC_START
    "\x96\xfb\x5b\x50\x76\xe4\x5b\x58\x51\x32\x5d\xf2\x6e\xff"
	GUI_UC_END
	"\x0"
};

const char *CFCardFull[LANGUAGE_CNT] = 
{
	CFCardFull_En,
	CFCardFull_SiCn,
	CFCardFull_TrCn
};

//--------------------------------------------------------------------------//
const char CFCardERR_En[] = 
{
    //Electronic disk error
    "Electronic disk error!"
};

const char CFCardERR_SiCn[] = 
{
    //zqh 2015��3��20�� modify
    //������()��(5F02)��(5E38)!
	GUI_UC_START
    "\x75\x35\x5b\x50\x76\xd8\x5f\x02\x5e\x38\xff\x01"
	GUI_UC_END
	"\x0"
};

const char CFCardERR_TrCn[] = 
{
    //zqh 2015��3��20�� modify
    //��ӱP (7570)��(5E38)!
	GUI_UC_START
    "\x96\xfb\x5b\x50\x76\xe4\x75\x70\x5e\x38\xff\x01"
	GUI_UC_END
	"\x0"
};

const char *CFCardERR[LANGUAGE_CNT] = 
{
	CFCardERR_En,
	CFCardERR_SiCn,
	CFCardERR_TrCn
};

//--------------------------------------------------------------------------//
const char CFCardOK_En[] = 
{
    //Electronic disk ok!
    "Electronic disk ok!"
};

const char CFCardOK_SiCn[] = 
{
    //zqh 2015��3��20�� modify
    //������()��(6B63)��(5E38)!
	GUI_UC_START
    "\x75\x35\x5b\x50\x76\xd8\x6b\x63\x5e\x38\xff\x01"
	GUI_UC_END
	"\x0"
};

const char CFCardOK_TrCn[] = 
{
    //zqh 2015��3��20�� modify
    //��ӱP ��(6B63)��(5E38)!
	GUI_UC_START
    "\x96\xfb\x5b\x50\x76\xe4\x6b\x63\x5e\x38\xff\x01"
	GUI_UC_END
	"\x0"
};

const char *CFCardOK[LANGUAGE_CNT] = 
{
	CFCardOK_En,
	CFCardOK_SiCn,
	CFCardOK_TrCn
};

//--------------------------------------------------------------------------//
const char CopyToCFCard_En[] = 
{
    //Copy file to Electronic disk
    "Copy file to electronic disk,\nContinue?"
};

const char CopyToCFCard_SiCn[] = 
{
    // zqh 2015��3��20�� modify�����������ļ���CF������Ϊ�������ļ��������̣��Ƿ��������
    // ������(6587)��(4EF6)��(5230)������
	GUI_UC_START
    "\x59\x0D\x52\x36\x65\x87\x4E\xF6\x52\x30\x75\x35\x5b\x50\x76\xd8\xff\x0c\x66\x2f\x54\x26\x7e\xe7\x7e\xed\xff\x1f"
	GUI_UC_END
	"\x0"
};

const char CopyToCFCard_TrCn[] = 
{
    //zqh 2015��3��20�� modify��������ؐ�ęn��CF������Ϊ���}�u�ęn����ӱP���Ƿ��^�m����
    // �}�u��(6587)�n(6A94)��(5230)��ӱP
	GUI_UC_START
    "\x89\x07\x88\xFD\x65\x87\x6a\x94\x52\x30\x96\xfb\x5b\x50\x76\xe4\xff\x0c\x66\x2f\x54\x26\x7e\x7c\x7e\x8c\xff\x1f"
	GUI_UC_END
	"\x0"
};

const char *CopyToCFCard[LANGUAGE_CNT] = 
{
	CopyToCFCard_En,
	CopyToCFCard_SiCn,
	CopyToCFCard_TrCn
};

//zqh 2015��3��25�� modify����ϵͳ�����б��еġ������ļ���CF������Ϊ������U���ļ��������̡�
//--------------------------------------------------------------------------//
const char CopyToCFCardName_En[] =
{
    //Copy file from UDisk to Electronic Disk
    "Copy File From UDisk To\nElectronic Disk"
};

const char CopyToCFCardName_SiCn[] =
{
    // ����U����(6587)��(4EF6)��(5230)������
    GUI_UC_START
    "\x59\x0D\x52\x36"
    GUI_UC_END
    "U"
    GUI_UC_START
    "\x76\xd8\x65\x87\x4E\xF6\x52\x30\x75\x35\x5b\x50\x76\xd8"
    GUI_UC_END
    "\x0"
};

const char CopyToCFCardName_TrCn[] =
{
    // �}�uU�P��(6587)�n(6A94)��(5230)��ӱP
    GUI_UC_START
    "\x89\x07\x88\xFD"
    GUI_UC_END
    "U"
    GUI_UC_START
    "\x76\xe4\x65\x87\x6a\x94\x52\x30\x96\xfb\x5b\x50\x76\xe4"
    GUI_UC_END
    "\x0"
};

const char *CopyToCFCardName[LANGUAGE_CNT] =
{
    CopyToCFCardName_En,
    CopyToCFCardName_SiCn,
    CopyToCFCardName_TrCn
};


//--------------------------------------------------------------------------//
const char FormatCFCard_En[] = 
{
    //Format Electronic Disk
    "Format Electronic Disk"
};

const char FormatCFCard_SiCn[] = 
{
    //zqh 2015��3��20�� modify
    // ��(683C)ʽ(5F0F)��(5316)������
	GUI_UC_START
    "\x68\x3c\x5f\x0f\x53\x16\x75\x35\x5b\x50\x76\xd8"
	GUI_UC_END
	"\x0"
};

const char FormatCFCard_TrCn[] = 
{
    // zqh  2015��3��20��   modify
    // ��(683C)ʽ(5F0F)��(5316)��ӱP
	GUI_UC_START
    "\x68\x3c\x5f\x0f\x53\x16\x96\xfb\x5b\x50\x76\xe4"
	GUI_UC_END
	"\x0"
};

const char *FormatCFCard[LANGUAGE_CNT] = 
{
	FormatCFCard_En,
	FormatCFCard_SiCn,
	FormatCFCard_TrCn
};

//--------------------------------------------------------------------------//
const char UDiskFileList_En[] = 
{
    //UDisk File List
    "UDisk File List"
};

const char UDiskFileList_SiCn[] = 
{
	// U��(76D8)��(6587)��(4EF6)��(5217)��(8868)
	"U"
	GUI_UC_START
	"\x76\xD8\x65\x87\x4e\xf6\x52\x17\x88\x68"
	GUI_UC_END
	"\x0"
};

const char UDiskFileList_TrCn[] = 
{
	// U�P(76E4)��(6587)�n(6A94)��(5217)��(8868)
	"U"
	GUI_UC_START
	"\x76\xe4\x65\x87\x6a\x94\x52\x17\x88\x68"
	GUI_UC_END
	"\x0"
};

const char *UDiskFileList[LANGUAGE_CNT] = 
{
	UDiskFileList_En,
	UDiskFileList_SiCn,
	UDiskFileList_TrCn
};

//--------------------------------------------------------------------------//
const char CFCardInsert_En[] = 
{
    //Electronic disk inserted\nMounting
    "Electronic disk inserted\nMounting"
};

const char CFCardInsert_SiCn[] = 
{
    //zqh 2015��3��20��  modify
    //������  ��(5DF2)��(63D2)��(5165),��(52A0)��(8F7D)��(4E2D)
	GUI_UC_START
    "\x75\x35\x5b\x50\x76\xd8\x5d\xf2\x63\xD2\x51\x65\x52\xa0\x8f\x7d\x4\x2d"
	GUI_UC_END
	"\x0"
};

const char CFCardInsert_TrCn[] = 
{
    //zqh 2015��3��20�� modify
    //��ӱP  ��(5DF2)��(63D2)��(5165),��(639B)�d(8F09)��(4E2D)
	GUI_UC_START
    "\x96\xfb\x5b\x50\x76\xe4\x5d\xf2\x63\xd2\x51\x65\x63\x9b\x8f\x09\x4e\x2d"
	GUI_UC_END
	"\x0"
};

const char *CFCardInsert[LANGUAGE_CNT] = 
{
	CFCardInsert_En,
	CFCardInsert_SiCn,
	CFCardInsert_TrCn
};

//--------------------------------------------------------------------------//
const char CFCardNeedFormat_En[] = 
{
    //Electronic disk need format
    "Electronic disk need format!"
};

const char CFCardNeedFormat_SiCn[] = 
{
    //zqh 2015��3��20�� modify
    //������ ��(9700)Ҫ(8981)��(683C)ʽ(5F0F)��(5316)!
	GUI_UC_START
    "\x75\x35\x5b\x50\x76\xd8\x97\x00\x89\x81\x68\x3c\x5f\x0f\x53\x16\xff\x01"
	GUI_UC_END
	"\x0"
};

const char CFCardNeedFormat_TrCn[] = 
{
    //zqh 2015��3��20�� modify
    //��ӱP  ��(9700)Ҫ(8981)��(683C)ʽ(5F0F)��(5316)!
	GUI_UC_START
    "\x96\xfb\x5b\x50\x76\xe4\x97\x00\x89\x81\x68\x3c\x5f\x0f\x53\x16\xff\x01"
	GUI_UC_END
	"\x0"
};

const char *CFCardNeedFormat[LANGUAGE_CNT] = 
{
	CFCardInsert_En,
	CFCardInsert_SiCn,
	CFCardInsert_TrCn
};

//--------------------------------------------------------------------------//
const char CFCardFsck_En[] = 
{
    //Check electronic disk file system
    "Check Electronic Disk\nFile System"
};

const char CFCardFsck_SiCn[] = 
{
    //zqh 2015��3��20�� modify�������������ļ�ϵͳ��
    // ��(68C0)��(67E5)������ ��(6587)��(4EF6)ϵ(7CFB)ͳ(7EDF)
	GUI_UC_START
    "\x68\xc0\x67\xe5\x75\x35\x5b\x50\x76\xd8\x65\x87\x4e\xf6\x7c\xfb\x7e\xdf"
	GUI_UC_END
	"\x0"
};

const char CFCardFsck_TrCn[] = 
{
    //zqh 2015��3��20��  modify,"�z����ӱP�ļ�ϵ�y"
    // �z(6AA2)��(67E5)��ӱP()�n(6A94)��(6848)ϵ(7CFB)�y(7D71)
	GUI_UC_START
    "\x6a\xa2\x67\xe5\x96\xfb\x5b\x50\x76\xe4\x65\x87\x4e\xf6\x7c\xfb\x7d\x71"
	GUI_UC_END
	"\x0"
};

const char *CFCardFsck[LANGUAGE_CNT] = 
{
	CFCardFsck_En,
	CFCardFsck_SiCn,
	CFCardFsck_TrCn
};

//--------------------------------------------------------------------------//
const char CFCardFsckOK_En[] = 
{
    //Check electronic disk \nfile system complete
    "Check electronic disk \nFile system complete!"
};

const char CFCardFsckOK_SiCn[] = 
{
    //zqh 2015��3��20�� modify
    // ��(68C0)��(67E5)������()��(6587)��(4EF6)ϵ(7CFB)ͳ(7EDF)��(5B8C)��(6210)!
	GUI_UC_START
    "\x68\xc0\x67\xE5\x75\x35\x5b\x50\x76\xd8\x65\x87\x4e\xf6\x7c\xfb\x7e\xdf\x5b\x8c\x62\x10\xff\x01"
	GUI_UC_END
	"\x0"
};

const char CFCardFsckOK_TrCn[] = 
{
    //zqh 2015��3��20�� modify
    // �z(6AA2)��(67E5)��ӱP()�n(6A94)��(6848)ϵ(7CFB)�y(7D71)��(5B8C)��(6210)!
	GUI_UC_START
    "\x6a\xa2\x67\xe5\x96\xfb\x5b\x50\x76\xe4\x6a\x94\x68\x48\x7c\xfb\x7d\x71\x5b\x8c\x62\x10\xff\x01"
	GUI_UC_END
	"\x0"
};

const char *CFCardFsckOK[LANGUAGE_CNT] = 
{
	CFCardFsckOK_En,
	CFCardFsckOK_SiCn,
	CFCardFsckOK_TrCn
};

//--------------------------------------------------------------------------//
const char CFCardFsckFail_En[] = 
{
    //Check electronic disk file system failed
    "Check electronic disk \nFile system failed!"
};

const char CFCardFsckFail_SiCn[] = 
{
    //zqh 2015��3��20�� modify
    // ��(68C0)��(67E5)������()��(6587)��(4EF6)ϵ(7CFB)ͳ(7EDF)ʧ(5931)��(8D25)!
	GUI_UC_START
    "\x68\xc0\x67\xE5\x75\x35\x5b\x50\x76\xd8\x65\x87\x4e\xf6\x7c\xfb\x7e\xdf\x59\x31\x8d\x25\xff\x01"
	GUI_UC_END
	"\x0"
};

const char CFCardFsckFail_TrCn[] = 
{
    //zqh 2015��3��20��  modify
    // �z(6AA2)��(67E5)��ӱP ()�n(6A94)��(6848)ϵ(7CFB)�y(7D71)ʧ(5931)��(6557)!
	GUI_UC_START
    "\x6a\xa2\x67\xe5\x96\xfb\x5b\x50\x76\xe4\x6a\x94\x68\x48\x7c\xfb\x7d\x71\x59\x31\x65\x57\xff\x01"
	GUI_UC_END
	"\x0"
};

const char *CFCardFsckFail[LANGUAGE_CNT] = 
{
	CFCardFsckFail_En,
	CFCardFsckFail_SiCn,
	CFCardFsckFail_TrCn
};

//--------------------------------------------------------------------------//
const char UpdateSystemTools_En[] = 
{
    //Update system tools
    "Update system tools"
};

const char UpdateSystemTools_SiCn[] = 
{
	// ��(5347)��(7EA7)ϵ(7CFB)ͳ(7EDF)��(5DE5)��(5177)
	GUI_UC_START
	"\x53\x47\x7e\xa7\x7c\xfb\x7e\xdf\x5d\xe5\x51\x77"
	GUI_UC_END
	"\x0"
};

const char UpdateSystemTools_TrCn[] = 
{
	// ��(5347)��(7D1A)ϵ(7CFB)�y(7D71)��(5DE5)��(5177)
	GUI_UC_START
	"\x53\x47\x7d\x1a\x7c\xfb\x7d\x71\x5d\xe5\x51\x77"
	GUI_UC_END
	"\x0"
};

const char *UpdateSystemTools[LANGUAGE_CNT] = 
{
	UpdateSystemTools_En,
	UpdateSystemTools_SiCn,
	UpdateSystemTools_TrCn
};

//--------------------------------------------------------------------------//
const char UpdateSystemToolsOK_En[] = 
{
    //Update system tools complete
    "Update system tools complete!"
};

const char UpdateSystemToolsOK_SiCn[] = 
{
    // ��(5347)��(7EA7)ϵ(7CFB)ͳ(7EDF)��(5DE5)��(5177)��(5B8C)��(6210)!
	GUI_UC_START
    "\x53\x47\x7e\xa7\x7c\xfb\x7e\xdf\x5d\xe5\x51\x77\x5b\x8c\x62\x10\xff\x01"
	GUI_UC_END
	"\x0"
};

const char UpdateSystemToolsOK_TrCn[] = 
{
    // ��(5347)��(7D1A)ϵ(7CFB)�y(7D71)��(5DE5)��(5177)��(5B8C)��(6210)!
    GUI_UC_START
    "\x53\x47\x7d\x1a\x7c\xfb\x7d\x71\x5d\xe5\x51\x77\x5b\x8c\x62\x10\xff\x01"
	GUI_UC_END
	"\x0"
};

const char *UpdateSystemToolsOK[LANGUAGE_CNT] = 
{
	UpdateSystemToolsOK_En,
	UpdateSystemToolsOK_SiCn,
	UpdateSystemToolsOK_TrCn
};

//--------------------------------------------------------------------------//
const char UpdateSystemToolsFail_En[] = 
{
    //Update system tools failed
    "Update system tools failed!"
};

const char UpdateSystemToolsFail_SiCn[] = 
{
    // ��(5347)��(7EA7)ϵ(7CFB)ͳ(7EDF)��(5DE5)��(5177)ʧ(5931)��(8D25)!
	GUI_UC_START
    "\x53\x47\x7e\xa7\x7c\xfb\x7e\xdf\x5d\xe5\x51\x77\x59\x31\x8d\x25\xff\x01"
	GUI_UC_END
	"\x0"
};

const char UpdateSystemToolsFail_TrCn[] = 
{
    // ��(5347)��(7D1A)ϵ(7CFB)�y(7D71)��(5DE5)��(5177)ʧ(5931)��(6557)!
	GUI_UC_START
    "\x53\x47\x7d\x1a\x7c\xfb\x7d\x71\x5d\xe5\x51\x77\x59\x31\x65\x57\xff\x01"
	GUI_UC_END
	"\x0"
};

const char *UpdateSystemToolsFail[LANGUAGE_CNT] = 
{
	UpdateSystemToolsFail_En,
	UpdateSystemToolsFail_SiCn,
	UpdateSystemToolsFail_TrCn
};

//--------------------------------------------------------------------------//
const char GetSystemLog_En[] = 
{
    //Get system log
    "Get system log"
};

const char GetSystemLog_SiCn[] = 
{
	// ��(83B7)ȡ(53D6)ϵ(7CFB)ͳ(7EDF)��(65E5)־(5FD7)
	GUI_UC_START
	"\x83\xb7\x53\xd6\x7c\xfb\x7e\xdf\x65\xe5\x5f\xd7"
	GUI_UC_END
	"\x0"
};

const char GetSystemLog_TrCn[] = 
{
	// �@(7372)ȡ(53D6)ϵ(7CFB)�y(7D71)��(65E5)�I(8A8C)
	GUI_UC_START
	"\x73\x72\x53\xd6\x7c\xfb\x7d\x71\x65\xe5\x8a\x8c"
	GUI_UC_END
	"\x0"
};

const char *GetSystemLog[LANGUAGE_CNT] = 
{
	GetSystemLog_En,
	GetSystemLog_SiCn,
	GetSystemLog_TrCn
};
	
//--------------------------------------------------------------------------//
const char UDiskReadOnly_En[] = 
{
	// udisk read only
    "UDisk read only"
};

const char UDiskReadOnly_SiCn[] = 
{
	// U��(76D8)ϵ(7CFB)ͳ(7EDF)ֻ(53EA)��(8BFB)
	"U"
	GUI_UC_START
	"\x76\xd8\x7c\xfb\x7e\xdf\x53\xea\x8b\xfb"
	GUI_UC_END
	"\x0"
};

const char UDiskReadOnly_TrCn[] = 
{
	// U�P(76E4)ϵ(7CFB)�y(7D71)ֻ(53EA)�x(8B80)
	"U"
	GUI_UC_START
	"\x76\xe4\x7c\xfb\x7d\x71\x53\xea\x8b\x80"
	GUI_UC_END
	"\x0"
};

const char *UDiskReadOnly[LANGUAGE_CNT] = 
{
	UDiskReadOnly_En,
	UDiskReadOnly_SiCn,
	UDiskReadOnly_TrCn
};

//--------------------------------------------------------------------------//
const char EnableSystemLog_En[] = 
{
	// press enter to enable log
    "Press enter to enable log"
};

const char EnableSystemLog_SiCn[] = 
{
	// ��(6309)Enter��(5F00)��(542F)��(65E5)־(5FD7)
	GUI_UC_START
	"\x63\x09"
	GUI_UC_END
	"Enter"
	GUI_UC_START
	"\x5f\x00\x54\x2f\x65\xe5\x5f\xd7"
	GUI_UC_END
	"\x0"
};

const char EnableSystemLog_TrCn[] = 
{
	// ��(6309)Enter�_(958B)��(555F)��(65E5)�I(8A8C)
	GUI_UC_START
	"\x63\x09"
	GUI_UC_END
	"Enter"
	GUI_UC_START
	"\x95\x8b\x55\x5f\x65\xe5\x8a\x8c"
	GUI_UC_END
	"\x0"
};

const char *EnableSystemLog[LANGUAGE_CNT] = 
{
	EnableSystemLog_En,
	EnableSystemLog_SiCn,
	EnableSystemLog_TrCn
};

//--------------------------------------------------------------------------//
const char DisableSystemLog_En[] = 
{
	// press enter to disable log
	"Press Enter to disable log"
};

const char DisableSystemLog_SiCn[] = 
{
	// ��(6309)Enter��(5173)��(95ED)��(65E5)־(5FD7)
	GUI_UC_START
	"\x63\x09"
	GUI_UC_END
	"Enter"
	GUI_UC_START
	"\x51\x73\x95\xed\x65\xe5\x5f\xd7"
	GUI_UC_END
	"\x0"
};

const char DisableSystemLog_TrCn[] = 
{
	// ��(6309)Enter�P(95DC)�](9589)��(65E5)�I(8A8C)
	GUI_UC_START
	"\x63\x09"
	GUI_UC_END
	"Enter"
	GUI_UC_START
	"\x95\xdc\x95\x89\x65\xe5\x8a\x8c"
	GUI_UC_END
	"\x0"
};

const char *DisableSystemLog[LANGUAGE_CNT] = 
{
	DisableSystemLog_En,
	DisableSystemLog_SiCn,
	DisableSystemLog_TrCn
};

//--------------------------------------------------------------------------//
const char SetSystemLog_En[] = 
{
	// enable/disable log
	"Enable / Disable LOG"
};

const char SetSystemLog_SiCn[] = 
{
	// ��(5F00)��(542F)/��(5173)��(95ED)��(65E5)־(5FD7)
	GUI_UC_START
	"\x5f\x00\x54\x2f"
	GUI_UC_END
	"/"
	GUI_UC_START
	"\x51\x73\x95\xed\x65\xe5\x5f\xd7"
	GUI_UC_END
	"\x0"
};

const char SetSystemLog_TrCn[] = 
{
	// �_(958B)��(555F)/�P(95DC)�](9589)��(65E5)�I(8A8C)
	GUI_UC_START
	"\x95\x8b\x55\x5f"
	GUI_UC_END
	"/"
	GUI_UC_START
	"\x95\xdc\x95\x89\x65\xe5\x8a\x8c"
	GUI_UC_END
	"\x0"
};

const char *SetSystemLog[LANGUAGE_CNT] = 
{
	SetSystemLog_En,
	SetSystemLog_SiCn,
	SetSystemLog_TrCn
};

//--------------------------------------------------------------------------//
const char CheckingCFCard_En[] = 
{
    //Checking electronic disk\nPlease wait...
    "Checking electronic disk\nPlease wait..."
};

const char CheckingCFCard_SiCn[] = 
{
    //zqh 2015��3��20�� modify
    // ��(6B63)��(5728)��(68C0)��(67E5)������()\n��(8BF7)��(7A0D)��(5019)...
	GUI_UC_START
    "\x6b\x63\x57\x28\x68\xc0\x67\xe5\x75\x35\x5b\x50\x76\xd8"
	GUI_UC_END
	"\n"
	GUI_UC_START
	"\x8b\xf7\x7a\x0d\x50\x19"
	GUI_UC_END
	"..."
	"\x0"
};

const char CheckingCFCard_TrCn[] = 
{
    //zqh 2015��3��20��  modify
    // ��(6B63)��(5728)�z(6AA2)��(67E5)��ӱP ()\nՈ(8ACB)��(7A0D)��(5019)...
	GUI_UC_START
    "\x6b\x63\x57\x28\x6a\xa2\x67\xe5\x96\xfb\x5b\x50\x76\xe4"
	GUI_UC_END
	"\n"
	GUI_UC_START
	"\x8a\xcb\x7a\x0d\x50\x19"
	GUI_UC_END
	"..."
	"\x0"
};

const char *CheckingCFCard[LANGUAGE_CNT] = 
{
	CheckingCFCard_En,
	CheckingCFCard_SiCn,
	CheckingCFCard_TrCn
};

//--------------------------------------------------------------------------//
const char FormattingCFCard_En[] = 
{
    //Formatting electronic disk\nplease wait...
    "Formatting electronic disk\nPlease wait..."
};

const char FormattingCFCard_SiCn[] = 
{
    //zqh 2015��3��20��  modify
    // ��(6B63)��(5728)��(683C)ʽ(5F0F)��(5316)������()\n��(8BF7)��(7A0D)��(5019)...
	GUI_UC_START
    "\x6b\x63\x57\x28\x68\x3c\x5f\x0f\x53\x61\x75\x35\x5b\x50\x76\xd8"
	GUI_UC_END
	"\n"
	GUI_UC_START
	"\x8b\xf7\x7a\x0d\x50\x19"
	GUI_UC_END
	"..."
	"\x0"
};

const char FormattingCFCard_TrCn[] = 
{
    //zqh 2015��3��20��  modify
    // ��(6B63)��(5728)��(683C)ʽ(5F0F)��(5316)��ӱP ()\nՈ(8ACB)��(7A0D)��(5019)...
	GUI_UC_START
    "\x6b\x63\x57\x28\x68\x3c\x5f\x0f\x53\x16\x96\xfb\x5b\x50\x76\xe4"
	GUI_UC_END
	"\n"
	GUI_UC_START
	"\x8a\xcb\x7a\x0d\x50\x19"
	GUI_UC_END
	"..."
	"\x0"
};

const char *FormattingCFCard[LANGUAGE_CNT] = 
{
	FormattingCFCard_En,
	FormattingCFCard_SiCn,
	FormattingCFCard_TrCn
};

//--------------------------------------------------------------------------//

/*

const char MsgBox_En[] = 
{
	//
	""
};

const char MsgBox_SiCn[] = 
{
	//
	GUI_UC_START
	""
	GUI_UC_END
	"\x0"
};

const char MsgBox_TrCn[] = 
{
	//
	GUI_UC_START
	""
	GUI_UC_END
	"\x0"
};

const char *MsgBox[LANGUAGE_CNT] = 
{
	MsgBox_En, 
	MsgBox_SiCn, 
	MsgBox_TrCn
};

//--------------------------------------------------------------------------//
*/
