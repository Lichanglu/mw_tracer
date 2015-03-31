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

#ifndef _TRACk_COMN_LINK_H_
#define _TRACk_COMN_LINK_H_


#include <stdint.h>

#define ITRACK_EOK      (0)
#define ITRACK_EFAIL    (-1)
#define ITRACK_ERUNTIME (-2)

/**
* @	一个头像分的模版模版个数最大值
*/
#define MODEL_NUM_MAX	(50)


/**
* @	模版近景焦距选择个数的最大值
*/
#define MODEL_DISTANCE_CHOSE_NUM	(2)


/**
* @	控制摄像头命令的字节数
*/
#define CTL_DATA_LEN			(16)

/**
* @ 取点的角度的个数，如90度，180度等的取点
*/
#define	POINT_VIEW_NUM			(4)

/**
* @	跟踪区域点设置的最大点数
*/
#define	TRACK_AREA_POINT_MAX	(10)


#define ITRACK_SETPARAMS 0       /* only cmd we support */

#define		TRIGGER_MAX_NUM				(100)		//一个检测周期内把所有周期分的段数
#define		TRIGGER_POINT_NUM		(30)		//一个刷新周期内总共检测到的触发点数
#define		DETECTION_SECTION_NUM	(50)		//多少帧为一个检测段


#define     TEACH_DETECTION_TARGET   		(100)        //多少个目标
#define		TEACH_DETECTION_FRAMES_SECTION	(20)	  		   //每段多少帧
#define		TEACH_DETECTION_SECTION_NUM		(400)


typedef float float_t;

/**
* @	像素点描述
*/
typedef struct _pixel_point_info
{	
	UInt16	x;		//x坐标
	UInt16	y;		//y坐标
}vertex_t;

/**
* @	触发点信息
*/
typedef struct _out_trigger_point_info
{	
	int16_t	x0;		//x坐标(左上顶点)
	int16_t	y0;		//y坐标(左上顶点)
	int16_t	x1;		//x坐标(右上顶点)
	int16_t	y1;		//y坐标(右上顶点)
}out_trigger_point_info_t;

/**
* @	调用control时是否需要重新初始化参数类型
*/
typedef enum _control_init_type
{
	NO_INIT = 0,		//无需调用control
	RE_INIT,			//需要初始化部分参数
	RE_START,			//需要初始化所有参数
	RE_INIT_NUM
}control_init_type_e;


/**
* @ 点的坐标信息
*/
typedef struct _trigger_point_info
{
	int		x0;
	int		y0;
	int		x1;
	int		y1;
	int		trigger_flag;		//本点的触发标志
}trigger_point_info_t;
/**
* @	摄像头移动的方向的宏
*/
typedef enum _camera_move_direction
{
	CAMERA_MOVE_NOT = 0,		//不移动
	CAMERA_MOVE_LEFT,			//向左移动		
	CAMERA_MOVE_RIGHT,			//向右移动
	CAMERA_MOVE_UP,				//向上移动
	CAMERA_MOVE_DOWN,			//向下移动
	CAMERA_MOVE_STOP,			//停止移动
	CAMERA_MOVE_NUM
}camera_move_direction_e;

/**
* @	控制摄像头的命令类型	
*/
typedef enum _cam_ctrl_type
{
	CAM_CTRL_NOT = 0,		//不进行摄像头控制
	CAM_CTRL_POSITION,		//跳转到预置位命令
	CAM_CTRL_ROTATION,		//控制摄像头转动方向命令
	CAM_CTRL_STOP,			//控制摄像头停止转动命令
	CAM_CTRL_ZOOM,			//控制摄像头焦距变化
	CAM_CTRL_NUM
}cam_ctrl_type_e;


/**
* @ 触发位置
*/
typedef enum _trigger_position
{
	NOT_TRIGGER = 0,		//不控制
	TRIGGER_POSITION1,		//调用全景
	TRIGGER_POSITION2,		//调用预置位2
	TRIGGER_POSITION3,		//调用预置位3
	TRIGGER_POSITION4,		//调用预置位4
	TRIGGER_POSITION5,		//调用预置位5
	TRIGGER_POSITION6,		//调用预置位6
	TRIGGER_POSITION7,		//调用预置位7
	TRIGGER_POSITION8,		//调用预置位8
	TRIGGER_POSITION9,		//调用预置位9
	TRIGGER_POSITION10,		//调用预置位10
	TRIGGER_POSITION11,		//调用预置位11
	TRIGGER_POSITION12,		//调用预置位12
	TRIGGER_POSITION13,		//调用预置位13
	TRIGGER_POSITION14,		//调用预置位14
	TRIGGER_POSITION15,		//调用预置位15
	TRIGGER_POSITION16,		//调用预置位16
	TRIGGER_POSITION17,		//调用预置位17
	TRIGGER_POSITION18,		//调用预置位18
	TRIGGER_POSITION19,		//调用预置位19
	TRIGGER_POSITION20,		//调用预置位20
	TRIGGER_POSITION21,		//调用预置位21
	TRIGGER_POSITION22,		//调用预置位22
	TRIGGER_POSITION23,		//调用预置位23
	TRIGGER_POSITION24,		//调用预置位24
	TRIGGER_POSITION25,		//调用预置位25
	TRIGGER_POSITION26,		//调用预置位26
	TRIGGER_POSITION27,		//调用预置位27
	TRIGGER_POSITION28,		//调用预置位28
	TRIGGER_POSITION29,		//调用预置位29
	TRIGGER_POSITION30,		//调用预置位30
	TRIGGER_POSITION31,		//调用预置位31
	TRIGGER_POSITION32,		//调用预置位32
	TRIGGER_POSITION33,		//调用预置位33
	TRIGGER_POSITION34,		//调用预置位34
	TRIGGER_POSITION35,		//调用预置位35
	TRIGGER_POSITION36,		//调用预置位36
	TRIGGER_POSITION37,		//调用预置位37
	TRIGGER_POSITION38,		//调用预置位38
	TRIGGER_POSITION39,		//调用预置位39
	TRIGGER_POSITION40,		//调用预置位40
	TRIGGER_POSITION41,		//调用预置位41
	TRIGGER_POSITION42		//调用预置位42	
}trigger_position_e;


/**
* @ Y,Cb,Cr值结构体
*/
typedef struct 	_yuv_value_info
{
	UInt16	y_value;		//y的值
	UInt16	cb_value;		//cb的值
	UInt16	cr_value;		//cr的值
}yuv_value_info_t;

/**
* @ Y,Cb,Cr值结构体
*/
typedef struct 	_students_yuv_value_info
{
	UInt16	y_value;		//y的值
	UInt16	cb_value;		//cb的值
	UInt16	cr_value;		//cr的值
}students_yuv_value_info_t;

/**
* @ 画线的信息
*/
typedef struct _line_info
{
	UInt32	start_x;	//线的起始点x坐标
	UInt32	start_y;	//线的起始点y坐标
	UInt32	end_x;		//线的结束点x坐标
	UInt32	end_y;		//线的结束点y坐标
}line_info_t;

typedef struct
{
  UInt16 min_x;
  UInt16 min_y;
  UInt16 max_x;
  UInt16 max_y;
} rect_t;

typedef struct TRect
{
	int nTop;
	int nBottom;
	int nLeft;
	int nRight;
	int nFillSize;
	int nMarkVal;
}T_Rect;


#define ALG_LINK_TRACK_CMD_SET_CHANNEL_WIN_PRM          (0x8001)

#define ALG_LINK_STUTRACK_CMD_SET_CHANNEL_WIN_PRM        (0x8002)

#define ALG_LINK_TRACK_CMD_PRINT_STATE        (0x8003)

#define ALG_LINK_STUSIDETRACK_CMD_SET_CHANNEL_WIN_PRM        (0x8004)

#define ALG_LINK_STUSIDETRACK_CMD_SAVE_VIEW (0x8005)

#define ALG_LINK_TRACK_CMD_SAVE_VIEW (0x8006)


#define ALG_LINK_STUDOUBLESIDETRACK_CMD_SET_CHANNEL_WIN_PRM        (0x8007)

#define ALG_LINK_STUDOUBLESIDETRACK_CMD_SAVE_VIEW (0x8008)

#define ALG_LINK_CEIL_TRACK_CMD_SET_CHANNEL_WIN_PRM          (0x8009)

#define ALG_LINK_CEIL_TRACK_CMD_SAVE_VIEW (0x800a)

#define ALG_LINK_BLACKBOARD_CMD_SET_CHANNEL_WIN_PRM (0x800b)
#endif

/*@}*/

