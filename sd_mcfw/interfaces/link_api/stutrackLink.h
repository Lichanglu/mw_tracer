/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
    \ingroup ALG_LINK_API
    \defgroup ALG_OSD_LINK_API ALG Link: OSD API

    @{
*/

/**
    \file osdLink.h
    \brief ALG Link: OSD API
*/

#ifndef _STUTRACk_LINK_H_
#define _STUTRACk_LINK_H_


#include <mcfw/interfaces/link_api/trackcomnLink.h>
/**
    \brief Max supported TRACE channels
*/
#define ALG_LINK_STUTRACK_MAX_CH      (1)


#define ALG_LINK_STUTRACK_MAX_TO_CH      (8)
/* @} */

/* Control Commands */

/**
    \ingroup ALG_LINK_API_CMD
    \addtogroup ALG_OSD_LINK_API_CMD  ALG Link: OSD API Control Commands

    @{
*/

/**
    \brief Link CMD: Configure Osd params

    SUPPORTED in ALL platforms

    \param AlgLink_OsdChWinParams * [IN] OSD Window parameters
*/
//#define ALG_LINK_OSD_CMD_SET_CHANNEL_WIN_PRM            (0x6001)


/**
    \brief Link CMD: Set blind window params

    ONLY SUPPORTED in DM810x

    \param AlgLink_OsdChBlindWinParams * [IN] Blind window parameters
*/
//#define ALG_LINK_OSD_CMD_SET_CHANNEL_BLIND_WIN_PRM      (0x6002)



//typedef unsigned char uint8_t;
//typedef float		float_t;
//typedef unsigned short uint16_t;
//typedef unsigned int	uint32_t;
//typedef unsigned long long	uint64_t;



/**
* @ Ñ§Éú¸ú×Ù´¥·¢¿òµÄ×î´óÖµ
*/
#define STUDENTS_TRIGGER_NUM	(35)

/**
* @ Ñ§Éú¸ú×ÙÆÁ±Î¿òµÄ×î´óÖµ
*/
#define STUDENTS_SHIELD_NUM	(20)

/**
* @ Ñ§Éú¸ú×Ù¶àÄ¿±êÉÏÌ¨¼ì²â¿òµÄ×î´óÖµ
*/
#define STUDENTS_MULTITARGET_NUM	(10)

/**
* @ Ñ§Éú¸ú×ÙÔ¤ÖÃÎ»µÄ×î´óÖµ,ÓÉÓÚÒªÁôÒ»¸öÔ¤ÖÃÎ»¸øÈ«¾°,ËùÒÔÒª¼Ó1
*/
#define STUDENTS_PRESET_NUM	(STUDENTS_TRIGGER_NUM + 1)


#define ITRACK_SETPARAMS 			0       //ÉèÖÃ¶¯Ì¬²ÎÊı,³ıÁË´¥·¢¿òĞÅÏ¢
#define ITRACK_SETPARAMS_TRIGGER1 	1       //ÉèÖÃÇ°5¸ö´¥·¢¿òĞÅÏ¢
#define ITRACK_SETPARAMS_TRIGGER2 	2       //ÉèÖÃµÚ¶ş¸ö5¸ö´¥·¢¿òĞÅÏ¢
#define ITRACK_SETPARAMS_TRIGGER3 	3       //ÉèÖÃµÚÈı¸ö5¸ö´¥·¢¿òĞÅÏ¢
#define ITRACK_SETPARAMS_TRIGGER4 	4       //ÉèÖÃµÚËÄ¸ö5¸ö´¥·¢¿òĞÅÏ¢
#define ITRACK_SETPARAMS_TRIGGER5 	5       //ÉèÖÃµÚÎå¸ö5¸ö´¥·¢¿òĞÅÏ¢
#define ITRACK_SETPARAMS_TRIGGER6 	6       //ÉèÖÃµÚÁù¸ö5¸ö´¥·¢¿òĞÅÏ¢
#define ITRACK_SETPARAMS_TRIGGER7 	7       //ÉèÖÃµÚÆß¸5¸öö´¥·¢¿òĞÅÏ¢

#define ITRACK_SETPARAMS_SHIELD1 	21      //ÉèÖÃÇ°5¸öÆÁ±Î¿òĞÅÏ¢
#define ITRACK_SETPARAMS_SHIELD2 	22      //ÉèÖÃµÚ¶ş¸ö5¸öÆÁ±Î¿òĞÅÏ¢
#define ITRACK_SETPARAMS_SHIELD3 	23      //ÉèÖÃµÚÈı¸ö5¸öÆÁ±Î¿òĞÅÏ¢
#define ITRACK_SETPARAMS_SHIELD4 	24      //ÉèÖÃµÚËÄ¸ö5¸öÆÁ±Î¿òĞÅÏ¢


/**
* @	´¥·¢¿òĞÅÏ¢µÄ½á¹¹Ìå
*/
typedef struct _student_trigger_info
{
	int16_t	x;
	int16_t	y;	
	int16_t	width;
	int16_t	height;
}student_trigger_info_t;

/**
* @ ´æ·Å×Ô¶¯¸ú×ÙµÄ¾²Ì¬²ÎÊı
*/
typedef struct 	_stutrack_static_param
{
	UInt32	size;
	UInt16	video_width;			//ÊäÈëÊÓÆµµÄ¿í
	UInt16	video_height;			//ÊäÈëÊÓÆµµÄ¸ß
	UInt16	pic_width;				//ÊäÈëËõ·ÅºóÍ¼ÏñµÄ¿í
	UInt16	pic_height;				//ÊäÈëËõ·ÅºóµÄÍ¼ÏñµÄ¸ß
}stutrack_static_param_t;

/**
* @ ´æ·Å×Ô¶¯¸ú×ÙµÄ¶¯Ì¬²ÎÊı
*/
typedef struct 	_stutrack_dynamic_param
{
	int32_t	size;
	int16_t	x_offset;		//Í¼ÏñµÄx×ø±êËõ·Å±¶Êı,ÊÇIMG_X_offset_0
	int16_t	y_offset;		//Í¼ÏñµÄy×ø±êËõ·Å±¶Êı,ÊÇIMG_Y_offset_0
	int16_t	reset_time;		//Ñ§Éú´¥·¢ºó¶à³¤Ê±¼äÃ»ÓĞ×øÏÂµÄ´¥·¢¾ÍÈÏÎªÊÇ×øÏÂÁËµÄÊ±¼ä
	int16_t	trigger_num;	//±íÊ¾´¥·¢¿ò¸öÊı
	//char	*pps;			//²âÊÔ
	student_trigger_info_t	trigger_info[STUDENTS_TRIGGER_NUM];	//Ñ§Éú»ú¸ú×ÙµÄ´¥·¢¿òĞÅÏ¢
	student_trigger_info_t	shield_info[STUDENTS_SHIELD_NUM];	//Ñ§Éú»úÆÁ±Î¿òĞÅÏ¢
	student_trigger_info_t	multitarget_info[STUDENTS_MULTITARGET_NUM];	//Ñ§Éú»úÆÁ±Î¿òĞÅÏ¢
	int16_t	control_mode;	//¿ØÖÆÀàĞÍ,0Îª×Ô¶¯,1ÎªÊÖ¶¯
	int16_t	sens;			//
	int16_t	message;		//Ö÷ÒªÓÃÀ´Éè¶¨»­ÄÇĞ©Ïß£¬±ÈÈçÊÇ·ñ»­ÈËµÃÂÖÀªÏßµÈ,1:ÏÔÊ¾ËùÓĞ±ßÔµµã×´Ì¬£»
							//2:·ûºÏ45¶È£¬90¶È£¬135¶È£¬180¶ÈµÄÏß»­³öÀ´£»3:»­Ä£°æ;4:»­¸ú×Ù¿ò;5:»­¸ú×Ù¿òÄÚµÄ·ôÉ«µã
	int16_t process_type;	//Îª0Ê±½øĞĞËã·¨´¦Àí,Îª1Ê±Ö»»­´¥·¢ÇøÓò,¸ú×ÙÇøÓòºÍ¸ú×Ù¿ò
	int16_t	track_point_num;//¸ú×Ù¿òµÄÉèÖÃ£¬×î´óÓĞ¼¸¸öµã£¬(¿ÉÓÉÕâĞ©µãÀ´Á¬½Ó³É¿ò£¬×é³ÉÁË¸ú×ÙÇøÓò)
	vertex_t	track_point[TRACK_AREA_POINT_MAX];	//¸ú×ÙÇøÓòµãµÄx£¬y×ø±êÃèÊö,ÊÇÏà¶ÔÓÚËõ·ÅÖ®Ç°Í¼ÏñËùËµ
	control_init_type_e	reset_level;	//0±íÊ¾²»ÒªÖØĞÂ³õÊ¼»°,1±íÊ¾ĞèÒªÖØĞÂ³õÊ¼»¯
	int16_t	nTrackSwitchType;			//¸ú×ÙÇĞ»»ÀàĞÍ,0±íÊ¾¸ú×Ù,1±íÊ¾Ö»ÇĞ»»
	int16_t	nStrategyNo;			//»úÎ»ĞÅÏ¢,ÓÃÓÚÖ»ÇĞ²»¸úµÄ·½Ê½Ñ¡Ôñ
}StuITRACK_DynamicParams;


/**
* @ ´æ·Å×Ô¶¯¸ú×ÙµÄÊäÈë²ÎÊı
*/
typedef struct 	_StuITRACK_Params
{
	UInt32	size;
	stutrack_static_param_t 	static_param;
	StuITRACK_DynamicParams	dynamic_param;
}StuITRACK_Params;




/**
* @ ´æ·Å×Ô¶¯¸ú×ÙµÄÊä³ö²ÎÊı
*/
typedef struct	_stutrack_output_param
{
	int32_t	size;
	int32_t	server_cmd;						//·¢ËÍ¸ø·şÎñÆ÷ÓÃÓÚÇĞ»»·şÎñÆ÷ÊÇ²¥·ÅÑ§Éú»ú»¹ÊÇÀÏÊ¦»úµÄÃüÁî
	int32_t	cam_position;					//¿ØÖÆÉãÏñÍ·×ªµ½Ä³¸öÔ¤ÖÃÎ»µÄµØ·½
	int32_t	cmd_type;						//Îª0±íÊ¾Ã»ÓĞÈÎºÎÑ§ÉúÕ¾Æğ,Îª1±íÊ¾ÓĞ1¸ö»ò¶à¸öÈËÕ¾Æğ

	uint16_t	nStandUpPos[4];//ÇøÓòÆğÁ¢´ÎÊı
	uint16_t	nFindMoveUpNum;//ÉÏÌ¨´ÎÊı
	uint16_t	nFindMoveDownNum;//ÏÂÌ¨´ÎÊı

	int32_t	k_0;
	int32_t	k_1;
	int32_t	k_2;
	int32_t	k_3;
	int32_t	k_4;
	int32_t	k_5;
	int32_t	k_6;
	int32_t	k_7;
	int32_t	k_8;
	
	int32_t	c_x;
	int32_t	c_y;
}StuITRACK_OutArgs;


/**
    \brief OSD channel create time configuration parameters
*/
typedef struct
{
	UInt8 chId;

	StuITRACK_Params StuTrackParms;

	Int32 ChToStream[ALG_LINK_STUTRACK_MAX_TO_CH];//Ö¸¶¨ÄÄÂ·Á÷×ßÄÄÂ·Í¨µÀ
    /**< Initial window params for all channels */
} AlgLink_StuTrackCreateParams;


typedef struct
{
    UInt32 chId;
    /**< SCD channel number on which tamper event was detected

        Valid values, 0..ALG_LINK_SCD_MAX_CH-1
    */
	StuITRACK_OutArgs args;

} AlgLink_StuTrackChStatus;


/*==============================================================================
    º¯Êı: <AlgLink_TrackalgInit>
    ¹¦ÄÜ: ´´½¨²ÎÊı³õÊ¼»¯(Ö÷Òª¸øÓ¦ÓÃÌá¹©)
    ²ÎÊı: 
    ·µ»ØÖµ: 
    Created By Ğì³ç 2013.04.11 11:12:51 For Web
==============================================================================*/
static inline void AlgLink_StuTrackalgInit(AlgLink_StuTrackCreateParams * pChPrm)
{

	Int32 chid = 0;
	StuITRACK_Params *pTrackParms = &pChPrm->StuTrackParms;
	pChPrm->chId = 0;
	memset(pTrackParms, 0x0, sizeof(StuITRACK_Params));
	pTrackParms->size = 0;
	pTrackParms->static_param.size = 0;
	pTrackParms->static_param.pic_width    = 160;
	pTrackParms->static_param.pic_height   = 90;
	pTrackParms->static_param.video_width  = 1280;
	pTrackParms->static_param.video_height = 720;

	pTrackParms->dynamic_param.size = 0;
	pTrackParms->dynamic_param.x_offset = 8;
	pTrackParms->dynamic_param.y_offset = 8;
	pTrackParms->dynamic_param.reset_time = 100;
	pTrackParms->dynamic_param.trigger_num = 2;
	pTrackParms->dynamic_param.control_mode = 0;
	pTrackParms->dynamic_param.sens = 20;
	pTrackParms->dynamic_param.message = 1;
	pTrackParms->dynamic_param.process_type = 0;
	pTrackParms->dynamic_param.track_point_num = 8;
	pTrackParms->dynamic_param.reset_level = RE_START;


#if 1
	pTrackParms->dynamic_param.track_point[0].x = 211;
	pTrackParms->dynamic_param.track_point[0].y = 24;
	pTrackParms->dynamic_param.track_point[1].x = 607;
	pTrackParms->dynamic_param.track_point[1].y = 25;
	pTrackParms->dynamic_param.track_point[2].x = 608;
	pTrackParms->dynamic_param.track_point[2].y = 109;
	pTrackParms->dynamic_param.track_point[3].x = 685;
	pTrackParms->dynamic_param.track_point[3].y = 110;
	pTrackParms->dynamic_param.track_point[4].x = 685;
	pTrackParms->dynamic_param.track_point[4].y = 286;
	pTrackParms->dynamic_param.track_point[5].x = 111;
	pTrackParms->dynamic_param.track_point[5].y = 288;
	pTrackParms->dynamic_param.track_point[6].x = 111;
	pTrackParms->dynamic_param.track_point[6].y = 107;
	pTrackParms->dynamic_param.track_point[7].x = 211;
	pTrackParms->dynamic_param.track_point[7].y = 107;

	memset(pTrackParms->dynamic_param.shield_info, 0x0, sizeof(student_trigger_info_t)*STUDENTS_SHIELD_NUM);
	memset(pTrackParms->dynamic_param.trigger_info, 0x0, sizeof(student_trigger_info_t)*STUDENTS_TRIGGER_NUM);
	memset(pTrackParms->dynamic_param.multitarget_info, 0x0, sizeof(student_trigger_info_t)*STUDENTS_MULTITARGET_NUM);
#endif

	#if 1
	pTrackParms->dynamic_param.trigger_info[0].x = 605;
	pTrackParms->dynamic_param.trigger_info[0].y = 126;
	pTrackParms->dynamic_param.trigger_info[0].width  = 34;
	pTrackParms->dynamic_param.trigger_info[0].height = 31;

	pTrackParms->dynamic_param.trigger_info[1].x = 523;
	pTrackParms->dynamic_param.trigger_info[1].y = 124;
	pTrackParms->dynamic_param.trigger_info[1].width  = 34;
	pTrackParms->dynamic_param.trigger_info[1].height = 29;

	pTrackParms->dynamic_param.trigger_info[2].x = 430;
	pTrackParms->dynamic_param.trigger_info[2].y = 124;
	pTrackParms->dynamic_param.trigger_info[2].width  = 35;
	pTrackParms->dynamic_param.trigger_info[2].height = 30;

	pTrackParms->dynamic_param.trigger_info[3].x = 326;
	pTrackParms->dynamic_param.trigger_info[3].y = 125;
	pTrackParms->dynamic_param.trigger_info[3].width  = 34;
	pTrackParms->dynamic_param.trigger_info[3].height = 29;

	pTrackParms->dynamic_param.trigger_info[4].x = 243;
	pTrackParms->dynamic_param.trigger_info[4].y = 125;
	pTrackParms->dynamic_param.trigger_info[4].width  = 34;
	pTrackParms->dynamic_param.trigger_info[4].height = 29;

	pTrackParms->dynamic_param.trigger_info[5].x = 154;
	pTrackParms->dynamic_param.trigger_info[5].y = 126;
	pTrackParms->dynamic_param.trigger_info[5].width  = 34;
	pTrackParms->dynamic_param.trigger_info[5].height = 30;

	pTrackParms->dynamic_param.trigger_info[7].x = 566;
	pTrackParms->dynamic_param.trigger_info[7].y = 79;
	pTrackParms->dynamic_param.trigger_info[7].width  = 32;
	pTrackParms->dynamic_param.trigger_info[7].height = 30;

	pTrackParms->dynamic_param.trigger_info[8].x = 495;
	pTrackParms->dynamic_param.trigger_info[8].y = 76;
	pTrackParms->dynamic_param.trigger_info[8].width  = 33;
	pTrackParms->dynamic_param.trigger_info[8].height = 29;

	pTrackParms->dynamic_param.trigger_info[9].x = 420;
	pTrackParms->dynamic_param.trigger_info[9].y = 75;
	pTrackParms->dynamic_param.trigger_info[9].width  = 33;
	pTrackParms->dynamic_param.trigger_info[9].height = 28;

	pTrackParms->dynamic_param.trigger_info[10].x = 352;
	pTrackParms->dynamic_param.trigger_info[10].y = 75;
	pTrackParms->dynamic_param.trigger_info[10].width  = 32;
	pTrackParms->dynamic_param.trigger_info[10].height = 29;

	#endif

	for(chid = 0;chid < ALG_LINK_STUTRACK_MAX_TO_CH; chid++)
	{
		pChPrm->ChToStream[chid] = -1;
	}
}


#endif

/*@}*/

