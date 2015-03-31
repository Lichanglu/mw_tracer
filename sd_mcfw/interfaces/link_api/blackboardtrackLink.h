/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


#ifndef _STU_BLACKBOARD_TRACk_LINK_H_
#define _STU_BLACKBOARD_TRACk_LINK_H_


#include <mcfw/interfaces/link_api/trackcomnLink.h>
/**
    \brief Max supported TRACE channels
*/
#define ALG_LINK_BLACKBOARDTRACK_MAX_CH      (1)   //此宏不可修改


//#define ALG_LINK_STUDOUBLESIDETRACK_MAX_CH      (8)



/**
* @	跟踪区域点设置的最大点数
*/
#define	BLACKBOARDTRACK_AREA_POINT_MAX	(10)

#define TRIGGER_NUM_MAX (11)

#define	TRACK_TRIGGER_MAX_NUM 	(2)	

/**
* @ 存放自动跟踪的静态参数
*/
typedef struct
{
	int32_t	size;
	int16_t	video_width;			//输入视频的宽
	int16_t	video_height;			//输入视频的高
	int16_t	pic_width;				//输入缩放后图像的宽
	int16_t	pic_height;				//输入缩放后的图像的高
}ITRACK_Blackboardtrack_static_param_t;

/**
* @ 存放自动跟踪的动态参数
*/

typedef struct
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
	vertex_t	track_point[BLACKBOARDTRACK_AREA_POINT_MAX];	//跟踪区域点的x，y坐标描述,是相对于缩放之前图像所说
	trigger_info_t trigger[TRACK_TRIGGER_MAX_NUM];
	control_init_type_e	reset_level;	//0表示不要重新初始话,1表示需要重新初始化
}ITRACK_Blackboardtrack_dynamic_params_t;


/**
* @ 存放自动跟踪的输入参数
*/
typedef struct
{
	int32_t	size;
	ITRACK_Blackboardtrack_static_param_t 	static_param;
	ITRACK_Blackboardtrack_dynamic_params_t	dynamic_param;
	//ITRACK_DynamicParams dynamic_param;
}BlackboardTrack_Params;


/**
* @ 存放自动跟踪的输出参数
*/
typedef struct
{
	int32_t	size;
	int32_t	server_cmd;						//发送给服务器用于切换服务器是播放学生机还是老师机的命令
	int32_t	cam_position;					//控制摄像头转到某个预置位的地方
	int32_t	move_distance;					//摄像头移动距离
	int32_t	move_direction;					//摄像头移动方向
	int32_t	cmd_type;						//2为多人站立，为1表示单人站立，0表示坐下。
	int32_t	track_status;					//跟踪状态
	int32_t	trigger_num;					//触发个数
	//vertex_t	trigger[TRIGGER_NUM_MAX];	//触发点坐标
	vertex_t	trigger[TRIGGER_NUM_MAX];	//触发点坐标
	out_trigger_point_info_t	trigger_point[TRIGGER_NUM_MAX];	//告诉应用做自动刷新背景用
	int32_t     IsTrigger;
	uint32_t	reserve1;
	uint32_t	reserve2;
	uint32_t	reserve3;
	uint32_t	reserve4;
}BlackboardTrack_OutArgs;


typedef struct
{
    UInt32 chId;
	UInt32 SaveViewFlag;
	UInt32 SaveType;
//	trigger_point_info_t	g_trigger_info[TRIGGER_POINT_NUM];
} AlgLink_BlackboardTrackSaveView;

/**
    \brief OSD channel create time configuration parameters
*/
typedef struct
{
	UInt8 chId;

	BlackboardTrack_Params BlackboardTrackParms;
	UInt8  *BlackboardTrackAddr[2][2];
	//AlgLink_BlackboardTrackSaveView  SaveView;
	Int32 StreamId;//指定哪路流走哪路通道
 
} AlgLink_BlackboardTrackCreateParams;


typedef struct
{
    UInt32 chId;
    /**< StuSideTrack channel number on which tamper event was detected

        Valid values, 0..-1
    */
	BlackboardTrack_OutArgs args;

} AlgLink_BlackboardTrackChStatus;



/*==============================================================================
    函数: <AlgLink_BlackboardTrackalgInit>
    功能: 
    参数: 
    返回值: 
    Created By 徐崇 2014.03.03 10:49:25 For Web
==============================================================================*/
static inline void AlgLink_BlackboardTrackalgInit(AlgLink_BlackboardTrackCreateParams * pChPrm)
{
	BlackboardTrack_Params *pTrackParms = &pChPrm->BlackboardTrackParms;

	pChPrm->BlackboardTrackAddr[0][0] = NULL;
	pChPrm->BlackboardTrackAddr[0][1] = NULL;
	pChPrm->BlackboardTrackAddr[1][0] = NULL;
	pChPrm->BlackboardTrackAddr[1][1] = NULL;
	
	//memset(&pChPrm->SaveView, 0x0, sizeof(AlgLink_BlackboardTrackSaveView));
	memset(pTrackParms, 0x0, sizeof(BlackboardTrack_Params));
	pTrackParms->size = 0;
	pTrackParms->static_param.size = 0;
	pTrackParms->static_param.pic_width    = 180;
	pTrackParms->static_param.pic_height   = 120;
	pTrackParms->static_param.video_width  = 704;
	pTrackParms->static_param.video_height = 576;

	pTrackParms->dynamic_param.size = 0;
	pTrackParms->dynamic_param.trigger_sum = 30;
	pTrackParms->dynamic_param.track_mode = 1;			//跟踪模式，0表示和采集好的原始帧比较，1表示和上帧比较看是否有变化
	pTrackParms->dynamic_param.start_track_time = 0;	//阀值时间，触发后多长时间开始跟踪,单位是帧
	pTrackParms->dynamic_param.reset_time = 0;			//跟踪丢失后到下次开始触发的时间
	pTrackParms->dynamic_param.control_mode = 0;	//控制类型,0为自动,1为手动
	pTrackParms->dynamic_param.sens = 30;			//
	pTrackParms->dynamic_param.message = 0;		//主要用来设定画那些线，比如是否画人得轮廓线等,1:显示所有边缘点状态；
										//2:符合45度，90度，135度，180度的线画出来；3:画模版;4:画跟踪框;5:画跟踪框内的肤色点
	pTrackParms->dynamic_param.track_point_num  =  4;//跟踪框的设置，最大有几个点，(可由这些点来连接成框，组成了跟踪区域)
	pTrackParms->dynamic_param.track_point[0].x = 60;	
	pTrackParms->dynamic_param.track_point[0].y = 48;	
	pTrackParms->dynamic_param.track_point[1].x = 97;	
	pTrackParms->dynamic_param.track_point[1].y = 59;
	pTrackParms->dynamic_param.track_point[2].x = 97;	
	pTrackParms->dynamic_param.track_point[2].y = 59;	
	pTrackParms->dynamic_param.track_point[3].x = 64;	
	pTrackParms->dynamic_param.track_point[4].y = 64;	


	pTrackParms->dynamic_param.reset_level = RE_INIT;	//0表示不要重新初始话,1表示需要重新初始化
	

	pChPrm->StreamId = -1;
	
}


#endif

/*@}*/

