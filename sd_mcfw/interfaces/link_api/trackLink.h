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

#ifndef _TRACk_LINK_H_
#define _TRACk_LINK_H_

#include <mcfw/interfaces/link_api/trackcomnLink.h>
/**
    \brief Max supported TRACE channels
*/
#define ALG_LINK_TRACK_MAX_CH      (1)

#define ALG_LINK_TRACK_MAX_TO_CH      (8)



/**
* @	跟踪框最大个数
*/
#define	TRACK_TRIGGER_MAX_NUM 	(2)	


#define ITRACK_SETPARAMS 0       /* only cmd we support */


/**
* @	触发框信息的结构体
*/
typedef struct _trigger_info
{
	int16_t	trigger_x0;		//表示触发框的左上顶点坐标的x值(以源图像为基准的)
	int16_t	trigger_y0;		//表示触发框的左上顶点坐标的y值(以源图像为基准的)
	int16_t	trigger_x1;		//表示触发框的右下顶点坐标的x值(以源图像为基准的)
	int16_t	trigger_y1;		//表示触发框的右下顶点坐标的y值(以源图像为基准的)
}trigger_info_t;


/**
* @ 存放自动跟踪的静态参数
*/
typedef struct 	_track_static_param
{
	UInt32	size;
	UInt16	video_width;			//输入视频的宽
	UInt16	video_height;			//输入视频的高
	UInt16	pic_width;				//输入缩放后图像的宽
	UInt16	pic_height;				//输入缩放后的图像的高
}track_static_param_t;


/**
* @ 存放自动跟踪的动态参数,新添加的部分 
*/
typedef struct 	_track_other_param
{
	trigger_info_t blackboard_trigger[2];//黑板区域触发框 
	trigger_info_t meanshift_trigger;//meanshfit 跟踪框 
	trigger_info_t pos1_trigger;//全景预置位触发框
	int16_t	pos1_trigger_num;//全景预置位触发点数，超过这个点数表示 触发
	int16_t meanshift_flag;//meanshift 开关
	int16_t dst_trigger_x;//触发的参考x,选择离这个框最近的目标作为跟踪目标	
	int16_t dst_trigger_y;//触发的参考y,选择离这个框最近的目标作为跟踪目标
	int16_t	nTrackSwitchType;			//只切不跟，跟踪类型，0为跟踪，1为只切不跟
}track_other_param_t;


/**
* @ 存放自动跟踪的动态参数
*/
typedef struct 	_track_dynamic_param
{
	UInt32	size;
	UInt16	track_mode;
	UInt16	start_track_time;	//阀值时间，触发后多长时间开始跟踪,单位是帧
	UInt16	reset_time;			//跟踪丢失后到下次开始触发的时间
	UInt8	zoom_distance;	//近景的焦距远近选择,值越大,焦距拉的越远,即0为焦距最近
	UInt16	x_offset;		//图像的x坐标缩放倍数,是IMG_X_offset_0
	UInt16	y_offset;		//图像的y坐标缩放倍数,是IMG_Y_offset_0
	UInt16	trigger_sum;	//表示在触发区域有几个点被触发了,是个阀值,当触发点阀值大于这个值时,认为被触发了T_p_sum
	trigger_info_t trigger[TRACK_TRIGGER_MAX_NUM];
	UInt16	control_mode;	//控制类型,0为自动,1为手动
	UInt16	sens;			//
	UInt16	message;		//主要用来设定画那些线，比如是否画人得轮廓线等,1:显示所有边缘点状态；
							//2:符合45度，90度，135度，180度的线画出来；3:画模版;4:画跟踪框;5:画跟踪框内的肤色点
	UInt16	model_sum;		//要匹配的模版总个数
	UInt16 track_sel;		//在有多个目标(人)的时候的选择策略，为1的时候选择最高的，2的时候选择最后面那个
	UInt16 process_type;	//为0时进行算法处理,为1时只画触发区域,跟踪区域和跟踪框
	UInt16	mid_x;			//设定的中心位置x的值(是相对于720缩放4倍后说的，也就是相对于180来算的)
	UInt16	mid_y;			//设定的中心位置y的值(是相对于576缩放4倍后说的，也就是相对于144来算的)
	UInt16	track_point_num;//跟踪框的设置，最大有几个点，(可由这些点来连接成框，组成了跟踪区域)
	vertex_t	track_point[TRACK_AREA_POINT_MAX];	//跟踪区域点的x，y坐标描述,是相对于缩放之前图像所说
	UInt16	limit_height;	//限制线，人如果在这条线之下，将不予跟踪(以源图像为基准的)
	UInt16  model_multiple;	//
	UInt16	model_level;	//
	control_init_type_e	reset_level;	//0表示不要重新初始话,1表示需要重新初始化
//	track_other_param_t *ptTrackOtherParam;
	track_other_param_t TrackOtherParam;
}ITRACK_DynamicParams;


/**
* @ 存放自动跟踪的输入参数
*/
typedef struct 	ITRACK_Params
{
	UInt32	size;
	track_static_param_t 	static_param;
	ITRACK_DynamicParams	dynamic_param;
}ITRACK_Params;



/**
* @ 存放自动跟踪的输出参数
*/
typedef struct	_track_output_param
{
	Int32	size;
	Int32	server_cmd;						//发送给服务器用于切换服务器是播放学生机还是老师机的命令
	Int32	cam_position;					//控制摄像头转到某个预置位的地方
	Int32	move_distance;					//摄像头移动距离
	Int32	move_direction;					//摄像头移动方向
	Int32	cmd_type;						//命令类型
	Int32	track_status;					//跟踪状态
	Int32	position1_mv_status;			//预置位为1 ，全景时的运动状态 
	UInt16	reserve1;
	UInt64	reserve2;
	UInt64	reserve3;
	UInt64	reserve4;
}ITRACK_OutArgs;



/**
    \brief OSD channel create time configuration parameters
*/
typedef struct
{
	UInt8 chId;

	ITRACK_Params TrackParms;
    /**< Initial window params for all channels */

	Int32 ChToStream[ALG_LINK_TRACK_MAX_TO_CH];//指定哪路流走哪路通道

	UInt8  *InBufferAddr[2][2];
	Int32  SaveViewFlag;
} AlgLink_TrackCreateParams;

typedef struct
{
    UInt32 chId;
    /**< SCD channel number on which tamper event was detected

        Valid values, 0..ALG_LINK_SCD_MAX_CH-1
    */
	ITRACK_OutArgs args;

} AlgLink_TrackChStatus;

typedef struct
{
    UInt32 chId;
   
} AlgLink_TrackSaveView;

/*==============================================================================
    函数: <AlgLink_TrackalgInit>
    功能: 创建参数初始化(主要给应用提供)
    参数: 
    返回值: 
    Created By 徐崇 2013.04.11 11:12:51 For Web
==============================================================================*/
static inline void AlgLink_TrackalgInit(AlgLink_TrackCreateParams * pChPrm)
{
	UInt32 chid = 0;
	ITRACK_Params *pTrackParms = &pChPrm->TrackParms;

	pChPrm->chId = 0;
	pChPrm->InBufferAddr[0][0] = NULL;
	pChPrm->InBufferAddr[0][1] = NULL;
	pChPrm->InBufferAddr[1][0] = NULL;
	pChPrm->InBufferAddr[1][1] = NULL;
	pChPrm->SaveViewFlag = 0;
	memset(pChPrm, 0x0, sizeof(ITRACK_Params));
	pTrackParms->size = 0;
	pTrackParms->static_param.size = 0;
	pTrackParms->static_param.pic_width    = 80;
	pTrackParms->static_param.pic_height   = 45;
	pTrackParms->static_param.video_width  = 1280;
	pTrackParms->static_param.video_height = 720;

	pTrackParms->dynamic_param.zoom_distance = 0;
	pTrackParms->dynamic_param.start_track_time = 50;
	pTrackParms->dynamic_param.reset_time = 80;
	pTrackParms->dynamic_param.limit_height = 480;
	pTrackParms->dynamic_param.x_offset = 4;
	pTrackParms->dynamic_param.y_offset = 4;
	pTrackParms->dynamic_param.trigger_sum = 3;

	memset(pTrackParms->dynamic_param.trigger, 0x0, sizeof(pTrackParms->dynamic_param.trigger));
	
	pTrackParms->dynamic_param.trigger[0].trigger_x0  = 768;
	pTrackParms->dynamic_param.trigger[0].trigger_y0  = 414;
	pTrackParms->dynamic_param.trigger[0].trigger_x1  = 852;
	pTrackParms->dynamic_param.trigger[0].trigger_y1  = 447;
	
	pTrackParms->dynamic_param.control_mode  = 0;
	pTrackParms->dynamic_param.sens  		= 56;
	pTrackParms->dynamic_param.message  		= 1;
	pTrackParms->dynamic_param.model_sum  	= 10;
	pTrackParms->dynamic_param.track_sel     = 1;
	pTrackParms->dynamic_param.mid_x     = 80;
	pTrackParms->dynamic_param.mid_y     = 45;
	pTrackParms->dynamic_param.model_multiple     = 1;
	pTrackParms->dynamic_param.model_level     = 24;
	pTrackParms->dynamic_param.track_point_num     = 4;

	#if 1
	pTrackParms->dynamic_param.track_point[0].x =0;
	pTrackParms->dynamic_param.track_point[0].y =100;
	pTrackParms->dynamic_param.track_point[1].x =0;
	pTrackParms->dynamic_param.track_point[1].y =600;
	
	pTrackParms->dynamic_param.track_point[2].x =1280;
	pTrackParms->dynamic_param.track_point[2].y =600;
	pTrackParms->dynamic_param.track_point[3].x =1280;
	pTrackParms->dynamic_param.track_point[3].y =100;

	#if 0
	pTrackParms->dynamic_param.track_point[4].x =640;
	pTrackParms->dynamic_param.track_point[4].y =82;
	pTrackParms->dynamic_param.track_point[5].x =808;
	pTrackParms->dynamic_param.track_point[5].y =146;
	pTrackParms->dynamic_param.track_point[6].x =1190;
	pTrackParms->dynamic_param.track_point[6].y =119;
	pTrackParms->dynamic_param.track_point[7].x =0;
	pTrackParms->dynamic_param.track_point[7].y =0;
	pTrackParms->dynamic_param.track_point[8].x =0;
	pTrackParms->dynamic_param.track_point[8].y =0;
	pTrackParms->dynamic_param.track_point[9].x =0;
	pTrackParms->dynamic_param.track_point[9].y =0;
	#endif
	#endif

	for(chid = 0;chid < ALG_LINK_TRACK_MAX_TO_CH; chid++)
	{
		pChPrm->ChToStream[chid] = -1;
	}
	pTrackParms->dynamic_param.TrackOtherParam.meanshift_flag = 1;
	pTrackParms->dynamic_param.TrackOtherParam.pos1_trigger_num = 0;
	pTrackParms->dynamic_param.TrackOtherParam.pos1_trigger.trigger_x0 = 0;
	pTrackParms->dynamic_param.TrackOtherParam.pos1_trigger.trigger_y0 = 0;
	pTrackParms->dynamic_param.TrackOtherParam.pos1_trigger.trigger_x1 = 0;
	pTrackParms->dynamic_param.TrackOtherParam.pos1_trigger.trigger_y1 = 0;
	pTrackParms->dynamic_param.TrackOtherParam.meanshift_trigger.trigger_x0 = 37;
	pTrackParms->dynamic_param.TrackOtherParam.meanshift_trigger.trigger_y0 = 416;
	pTrackParms->dynamic_param.TrackOtherParam.meanshift_trigger.trigger_x1 = 694;
	pTrackParms->dynamic_param.TrackOtherParam.meanshift_trigger.trigger_y1 = 450;
}


#endif

/*@}*/

