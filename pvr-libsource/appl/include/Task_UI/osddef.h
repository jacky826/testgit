#ifndef _OSDS_DEF_HEADER_
#define _OSDS_DEF_HEADER_

/*Postiion Definetion*/
//	3	4	1
//	5	0	6
//	7	2	8	
#define OSDPOS_BASEMENU		(0)
#define OSDPOS_RIGHTTOP		(OSDPOS_BASEMENU+1)
#define OSDPOS_MIDBOTTOM		(OSDPOS_RIGHTTOP+1)//Now we use

#define OSDPOS_COUNT			(OSDPOS_MIDBOTTOM+1)


#define VALIDPOSITION(nPos)		(nPos<OSDPOS_COUNT)


/*----------------------Staitc OSD-------------------------------*/

#define DVR_OSD_DIS						(-1)			// 使不能定时器 

//Blank Stat
#define OSD_BLANK_DISPLAY				(0)			//	Display Status
//Blank Recording
#define OSD_BLANK_RECORD			(OSD_BLANK_DISPLAY+1)		//	 Reocrding Stat	
//Playback Postion Bar
#define OSD_PLAY_DISPLAY				(OSD_BLANK_RECORD+1)		//	Display Status
/*Static OSD Count*/
#define UI_OSD_COUNT					(OSD_PLAY_DISPLAY+1) //(OSD_PLAY_AB+1)	//The Final one //Grifin

#define VALIDOSDIDX(x)					((x>=OSD_BLANK_DISPLAY)&&(x<UI_OSD_COUNT))

//OSD Functions

/* Blank Display */
#define OSD_BLANK_DISPLAY_POS			(OSDPOS_RIGHTTOP)
#define OSD_BLANK_DISPLAY_INTERVAL	(1)// 1 seconds 
int DoDisplayCurStatRefresh(void *user);
//Close: NULL
int DoDisplayCurStatRestore(void *user);

/* Blank Recording */
#define OSD_BLANK_RECORD_POS			(OSDPOS_RIGHTTOP)
#define OSD_BLANK_RECORD_INTERVAL	(0)//NO refresh
int DoDisplayRecStaus(void *user);

/*Playback Display*/
#define OSD_PLAY_DISPLAY_POS			(OSDPOS_MIDBOTTOM)
#define OSD_PLAY_DISPLAY_INTERVAL	(1)// 1 seconds 
int DoDisplayCurStatRefresh(void *user);
//Close: NULL
int DoDisplayCurStatRestore(void *user);


/*----------------------Temp OSD-------------------------------*/
//Display & Auto Close

#define OSD_VOLUMN_POS				(OSDPOS_MIDBOTTOM)
#define OSD_CHANNEL_POS			(OSDPOS_MIDBOTTOM)
#define OSD_IGNOREKEY_POS			(OSDPOS_RIGHTTOP)
#define OSD_PLAY_CONTROL_POS		(OSDPOS_RIGHTTOP)
#define OSD_PLAY_STOP_POS			(OSDPOS_RIGHTTOP)
#define OSD_PLAY_PAUSE_POS		(OSDPOS_RIGHTTOP)
#define OSD_TVAV_POS				(OSDPOS_RIGHTTOP)

#endif
