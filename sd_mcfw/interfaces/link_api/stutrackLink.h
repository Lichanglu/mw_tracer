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
* @ 学生跟踪触发框的最大值
*/
#define STUDENTS_TRIGGER_NUM	(35)

/**
* @ 学生跟踪屏蔽框的最大值
*/
#define STUDENTS_SHIELD_NUM	(20)

/**
* @ 学生跟踪多目标上台检测框的最大值
*/
#define STUDENTS_MULTITARGET_NUM	(10)

/**
* @ 学生跟踪预置位的最大值,由于要留一个预置位给全景,所以要加1
*/
#define STUDENTS_PRESET_NUM	(STUDENTS_TRIGGER_NUM + 1)


#define ITRACK_SETPARAMS 			0       //设置动态参数,除了触发框信息
#define ITRACK_SETPARAMS_TRIGGER1 	1       //设置前5个触发框信息
#define ITRACK_SETPARAMS_TRIGGER2 	2       //设置第二个5个触发框信息
#define ITRACK_SETPARAMS_TRIGGER3 	3       //设置第三个5个触发框信息
#define ITRACK_SETPARAMS_TRIGGER4 	4       //设置第四个5个触发框信息
#define ITRACK_SETPARAMS_TRIGGER5 	5       //设置第五个5个触发框信息
#define ITRACK_SETPARAMS_TRIGGER6 	6       //设置第六个5个触发框信息
#define ITRACK_SETPARAMS_TRIGGER7 	7       //设置第七�5个龃シ⒖蛐畔�

#define ITRACK_SETPARAMS_SHIELD1 	21      //设置前5个屏蔽框信息
#define ITRACK_SETPARAMS_SHIELD2 	22      //设置第二个5个屏蔽框信息
#define ITRACK_SETPARAMS_SHIELD3 	23      //设置第三个5个屏蔽框信息
#define ITRACK_SETPARAMS_SHIELD4 	24      //设置第四个5个屏蔽框信息


/**
* @	触发框信息的结构体
*/
typedef struct _student_trigger_info
{
	int16_t	x;
	int16_t	y;	
	int16_t	width;
	int16_t	height;
}student_trigger_info_t;

/**
* @ 存放自动跟踪的静态参数
*/
typedef struct 	_stutrack_static_param
{
	UInt32	size;
	UInt16	video_width;			//输入视频的宽
	UInt16	video_height;			//输入视频的高
	UInt16	pic_width;				//输入缩放后图像的宽
	UInt16	pic_height;				//输入缩放后的图像的高
}stutrack_static_param_t;

/**
* @ 存放自动跟踪的动态参数
*/
typedef struct 	_stutrack_dynamic_param
{
	int32_t	size;
	int16_t	x_offset;		//图像的x坐标缩放倍数,是IMG_X_offset_0
	int16_t	y_offset;		//图像的y坐标缩放倍数,是IMG_Y_offset_0
	int16_t	reset_time;		//学生触发后多长时间没有坐下的触发就认为是坐下了的时间
	int16_t	trigger_num;	//表示触发框个数
	//char	*pps;			//测试
	student_trigger_info_t	trigger_info[STUDENTS_TRIGGER_NUM];	//学生机跟踪的触发框信息
	student_trigger_info_t	shield_info[STUDENTS_SHIELD_NUM];	//学生机屏蔽框信息
	student_trigger_info_t	multitarget_info[STUDENTS_MULTITARGET_NUM];	//学生机屏蔽框信息
	int16_t	control_mode;	//控制类型,0为自动,1为手动
	int16_t	sens;			//
	int16_t	message;		//主要用来设定画那些线，比如是否画人得轮廓线等,1:显示所有边缘点状态；
							//2:符合45度，90度，135度，180度的线画出来；3:画模版;4:画跟踪框;5:画跟踪框内的肤色点
	int16_t process_type;	//为0时进行算法处理,为1时只画触发区域,跟踪区域和跟踪框
	int16_t	track_point_num;//跟踪框的设置，最大有几个点，(可由这些点来连接成框，组成了跟踪区域)
	vertex_t	track_point[TRACK_AREA_POINT_MAX];	//跟踪区域点的x，y坐标描述,是相对于缩放之前图像所说
	control_init_type_e	reset_level;	//0表示不要重新初始话,1表示需要重新初始化
	int16_t	nTrackSwitchType;			//跟踪切换类型,0表示跟踪,1表示只切换
	int16_t	nStrategyNo;			//机位信息,用于只切不跟的方式选择
}StuITRACK_DynamicParams;


/**
* @ 存放自动跟踪的输入参数
*/
typedef struct 	_StuITRACK_Params
{
	UInt32	size;
	stutrack_static_param_t 	static_param;
	StuITRACK_DynamicParams	dynamic_param;
}StuITRACK_Params;




/**
* @ 存放自动跟踪的输出参数
*/
typedef struct	_stutrack_output_param
{
	int32_t	size;
	int32_t	server_cmd;						//发送给服务器用于切换服务器是播放学生机还是老师机的命令
	int32_t	cam_position;					//控制摄像头转到某个预置位的地方
	int32_t	cmd_type;						//为0表示没有任何学生站起,为1表示有1个或多个人站起

	uint16_t	nStandUpPos[4];//区域起立次数
	uint16_t	nFindMoveUpNum;//上台次数
	uint16_t	nFindMoveDownNum;//下台次数

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

	Int32 ChToStream[ALG_LINK_STUTRACK_MAX_TO_CH];//指定哪路流走哪路通道
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
    函数: <AlgLink_TrackalgInit>
    功能: 创建参数初始化(主要给应用提供)
    参数: 
    返回值: 
    Created By 徐崇 2013.04.11 11:12:51 For Web
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

