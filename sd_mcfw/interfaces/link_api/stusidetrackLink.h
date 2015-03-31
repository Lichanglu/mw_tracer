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

#ifndef _STUSIDETRACk_LINK_H_
#define _STUSIDETRACk_LINK_H_


#include <mcfw/interfaces/link_api/trackcomnLink.h>
/**
    \brief Max supported TRACE channels
*/
#define ALG_LINK_STUSIDETRACK_MAX_CH      (1)


#define ALG_LINK_STUSIDETRACK_MAX_TO_CH      (8)



/**
* @	跟踪区域点设置的最大点数
*/
#define	TRACK_STUDENTS_RIGHT_SIDE_AREA_POINT_MAX	(10)




/**
* @ 存放自动跟踪的静态参数
*/
typedef struct 	_ITRACK_STUDENTS_RIGHT_SIDE_static_param_t
{
	int32_t	size;
	int16_t	video_width;			//输入视频的宽
	int16_t	video_height;			//输入视频的高
	int16_t	pic_width;				//输入缩放后图像的宽
	int16_t	pic_height;				//输入缩放后的图像的高
}ITRACK_STUDENTS_RIGHT_SIDE_static_param_t;

/**
* @ 存放自动跟踪的动态参数
*/
typedef struct 	_ITRACK_STUDENTS_RIGHT_SIDE_dynamic_params_t
{
	int32_t	size;
	int16_t	trigger_sum;		//触发点总数
	int16_t	track_mode;			//跟踪模式，0表示和采集好的原始帧比较，1表示和上帧比较看是否有变化
	int16_t	start_track_time;	//阀值时间，触发后多长时间开始跟踪,单位是帧
	int16_t	reset_time;			//跟踪丢失后到下次开始触发的时间
	int16_t	control_mode;	//控制类型,0为自动,1为手动
	int16_t	sens;			//
	int16_t	message;		//主要用来设定画那些线，比如是否画人得轮廓线等,1:显示所有边缘点状态；
							//2:符合45度，90度，135度，180度的线画出来；3:画模版;4:画跟踪框;5:画跟踪框内的肤色点
	int16_t	track_point_num;//跟踪框的设置，最大有几个点，(可由这些点来连接成框，组成了跟踪区域)
	vertex_t	track_point[TRACK_STUDENTS_RIGHT_SIDE_AREA_POINT_MAX];	//跟踪区域点的x，y坐标描述,是相对于缩放之前图像所说
	control_init_type_e	reset_level;	//0表示不要重新初始话,1表示需要重新初始化
}ITRACK_STUDENTS_RIGHT_SIDE_dynamic_params_t;


/**
* @ 存放自动跟踪的输入参数
*/
typedef struct 	_StuSideITRACK_Params
{
	int32_t	size;
	ITRACK_STUDENTS_RIGHT_SIDE_static_param_t 	static_param;
	ITRACK_STUDENTS_RIGHT_SIDE_dynamic_params_t	dynamic_param;
}StuSideITRACK_Params;


/**
* @ 存放自动跟踪的输出参数
*/
typedef struct	_StuSideITRACK_OutArgs
{
	int32_t	size;
	int32_t	server_cmd;						//发送给服务器用于切换服务器是播放学生机还是老师机的命令
	int32_t	cam_position;					//控制摄像头转到某个预置位的地方
	int32_t	move_distance;					//摄像头移动距离
	int32_t	move_direction;					//摄像头移动方向
	int32_t	cmd_type;						//为1表示站立，2表示坐下。
	int32_t	track_status;					//跟踪状态
	
	uint32_t	reserve1;
	uint32_t	reserve2;
	uint32_t	reserve3;
	uint32_t	reserve4;
}StuSideITRACK_OutArgs;


/**
    \brief OSD channel create time configuration parameters
*/
typedef struct
{
	UInt8 chId;

	StuSideITRACK_Params StuSideTrackParms;
	UInt8  *StuSideAddr[2][2];
	Int32 SaveViewFlag;
	Int32 ChToStream[ALG_LINK_STUSIDETRACK_MAX_TO_CH];//指定哪路流走哪路通道
 
} AlgLink_StuSideTrackCreateParams;


typedef struct
{
    UInt32 chId;
    /**< StuSideTrack channel number on which tamper event was detected

        Valid values, 0..-1
    */
	StuSideITRACK_OutArgs args;

} AlgLink_StuSideTrackChStatus;

typedef struct
{
    UInt32 chId;
   
} AlgLink_StuSideTrackSaveView;



/*==============================================================================
    函数: <AlgLink_StuSideTrackalgInit>
    功能: 
    参数: 
    返回值: 
    Created By 徐崇 2013.10.16 11:54:57 For Web
==============================================================================*/
static inline void AlgLink_StuSideTrackalgInit(AlgLink_StuSideTrackCreateParams * pChPrm)
{

	Int32 chid = 0;
	StuSideITRACK_Params *pTrackParms = &pChPrm->StuSideTrackParms;

	pChPrm->chId= 0;
	pChPrm->StuSideAddr[0][0] = NULL;
	pChPrm->StuSideAddr[0][1] = NULL;
	pChPrm->StuSideAddr[1][0] = NULL;
	pChPrm->StuSideAddr[1][1] = NULL;
	pChPrm->SaveViewFlag = 0;
	memset(pTrackParms, 0x0, sizeof(StuSideITRACK_Params));
	pTrackParms->size = 0;
	pTrackParms->static_param.size = 0;
	pTrackParms->static_param.pic_width    = 160;
	pTrackParms->static_param.pic_height   = 90;
	pTrackParms->static_param.video_width  = 1280;
	pTrackParms->static_param.video_height = 720;

	pTrackParms->dynamic_param.size = 0;
	pTrackParms->dynamic_param.trigger_sum = 0;
	pTrackParms->dynamic_param.track_mode = 0;			//跟踪模式，0表示和采集好的原始帧比较，1表示和上帧比较看是否有变化
	pTrackParms->dynamic_param.start_track_time = 0;	//阀值时间，触发后多长时间开始跟踪,单位是帧
	pTrackParms->dynamic_param.reset_time = 0;			//跟踪丢失后到下次开始触发的时间
	pTrackParms->dynamic_param.control_mode = 0;	//控制类型,0为自动,1为手动
	pTrackParms->dynamic_param.sens = 20;			//
	pTrackParms->dynamic_param.message = 0;		//主要用来设定画那些线，比如是否画人得轮廓线等,1:显示所有边缘点状态；
										//2:符合45度，90度，135度，180度的线画出来；3:画模版;4:画跟踪框;5:画跟踪框内的肤色点
	pTrackParms->dynamic_param.track_point_num  =  4;//跟踪框的设置，最大有几个点，(可由这些点来连接成框，组成了跟踪区域)
	pTrackParms->dynamic_param.track_point[0].x = 10;	
	pTrackParms->dynamic_param.track_point[0].y = 10;	
	pTrackParms->dynamic_param.track_point[1].x = 30;	
	pTrackParms->dynamic_param.track_point[1].y = 10;
	pTrackParms->dynamic_param.track_point[2].x = 30;	
	pTrackParms->dynamic_param.track_point[2].y = 10;	
	pTrackParms->dynamic_param.track_point[3].x = 30;	
	pTrackParms->dynamic_param.track_point[4].y = 30;	


	pTrackParms->dynamic_param.reset_level = RE_INIT;	//0表示不要重新初始话,1表示需要重新初始化
	
	for(chid = 0;chid < ALG_LINK_STUSIDETRACK_MAX_TO_CH; chid++)
	{
		pChPrm->ChToStream[chid] = -1;
	}
}


#endif

/*@}*/

