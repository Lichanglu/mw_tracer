/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _ALG_LINK_TRACK_PRIV_H_
#define _ALG_LINK_TRACK_PRIV_H_

#include <mcfw/src_bios6/utils/utils.h>
#include <mcfw/src_bios6/links_c6xdsp/system/system_priv_c6xdsp.h>
#include <mcfw/interfaces/link_api/algLink.h>

#include "tracking.h"
#include "AutoTrack.h"

#define MAX_HEAD_NUM 600
/**
* @	用于内部的全局变量
*/
typedef struct _T_GLOBAL_OBJ
{
	uint8_t *Area_vertex; //表示点是否属于检测区域
	
	uint8_t *src_y_buf; //存放要进行运算的源ybuf数据
	uint8_t *src_cb_buf; //存放要进行运算的源cbbuf数据
	uint8_t *src_cr_buf; //存放要进行运算的源crbuf数据
	
	uint8_t *dst_y_buf; //存放每次跟踪后的第一帧ybuf数据
	uint8_t *dst_cb_buf; //存放每次跟踪后的第一帧cbbuf数据
	uint8_t *dst_cr_buf; //存放每次跟踪后的第一帧crbuf数据
	
	/*在进行运算的buffer区域中，每个点的值表示这个点是哪个角度的值，比如为1，表示是45度，为2表示135度，等等*/
	uint8_t *img_coordinate;
	uint8_t *img_coordinate_g;
	
	/*描述所有45度，90度，135度，180度点的x,y*/
	uint8_t *Point_Buffer;
	
	/*分别描述45度，90度，135度，180度点的属性，包括要画的点的大小*/
	uint8_t *T_L_Buffer;
	uint8_t *T_R_Buffer;
	uint8_t *T_U_Buffer;
	uint8_t *T_P_Buffer;
	
	/*用于存放原始buf的数据*/
	uint8_t *output_cb_buf;
	uint8_t *output_cr_buf;
	
	int32_t m_offset;		//目标的左右位置
	int32_t n_offset;		//目标的上下位置
	
	/*表示每种角度的点的个数*/
	int32_t Point_180_Inc;
	int32_t Point_90_Inc;
	int32_t Point_45_Inc;
	int32_t Point_135_Inc;
	int32_t Point_Inc;

	int32_t nSrcBufWidth;//采集原始图像的数据摆放buf的宽高
	int32_t nSrcBufHeight;//采集原始图像的数据摆放buf的宽高
	int16_t	turn_flag;			//为0时表示不需要重新跟踪，为1时表示捕获到目标，设置跟踪标志，为2时表示跟踪目标丢失
	int16_t	turn_time;			//定位到目标后,需要等待摄像头调用预置位的时间
	int16_t	track_start;		//表示是否跟踪上目标，0表示未跟踪上，1表示跟踪上了目标
	int32_t	first_time;			//开始进入算法的帧数,目前是前十帧不做处理
	int32_t	last_move_flag;		//方向移动标志,为0表示未移动方向,为1表示移动了方向
	int32_t	last_track_status;	//上次跟踪状态	
	int32_t	skin_colour_num;	//连续检测到肤色的帧数
	/**
	* @ 存放跟踪处理结果的
	*/
	int16_t 	model_v_cnt[MAX_HEAD_NUM];
	uint16_t	model_v_mnt[MAX_HEAD_NUM];
	uint16_t	model_v_pnt[MAX_HEAD_NUM];
	
	/**
	* @ 存放模版
	*/
	uint8_t 	model[MODEL_DISTANCE_CHOSE_NUM][MODEL_NUM_MAX];
	/**
	* @ 跟踪处理的结构体,用于存放跟踪需要的数据
	*/
	STR_OBJECTS_DETECTED pst_det_obj;
	
	/**
	* @ 跟踪处理的结构体,用于存放跟踪处理结果的
	*/
	STR_OBJECT_TRAKING pst_trak_obj;

	t_TrackingObject tAutoTrackObj;
	int32_t nLastTrackState;
	int32_t nTrackType;//跟踪类型,0为轮廓跟踪,1为meanshift跟踪,2为轮廓和meanshift同时跟踪
	int32_t nTargetFlag;//表示轮廓找到目标
	int32_t nTrackStartFlag;//表示预跟踪延时结束,开始跟踪标志,但可能没有找到目标
	int32_t nMeanShiftFrame;//meanshift跟踪时的帧统计，每10帧检测一次轮廓
	int32_t nMeanShiftFaceLost;//meanshift跟踪同时检测轮廓，轮廓没找到的次数
	int32_t nPos1MvFlag;//预置位1时全景检测到运动标志
	int32_t nPos1MvLost;//预置位1时全景未检测到运动时间
	int32_t nLostNum;//预置位1时全景未检测到运动时间

	
	int32_t debugValue0;
	int32_t debugValue1;
	int32_t debugValue2;

}T_GLOBAL_OBJ;


typedef struct _TrackHand {
    ITRACK_Params	input_param;	//输入参数
    T_GLOBAL_OBJ t_global_obj;
}TrackHand;	


typedef struct _AlgLink_TrackChObj
{

	UInt8 chId;
	UInt32 rtParamUpdatePerFrame;
	TrackHand trackhandle;
	UInt8  *InBufferAddr[2][2];
} AlgLink_TrackChObj;

typedef struct _AlgLink_TrackObj
{
	System_LinkQueInfo * inQueInfo;
	
	AlgLink_TrackChObj chObj[ALG_LINK_TRACK_MAX_CH];
	
	AlgLink_TrackCreateParams  trackChCreateParams[ALG_LINK_TRACK_MAX_CH];

	Int32 ChToStream[ALG_LINK_TRACK_MAX_TO_CH];

	Utils_QueHandle trackoutFrameBufQue;

	FVID2_Frame        *queueMem[100];

	Int32 ProcessFlag;   //0 free 1 process

	PrintState algstate;
} AlgLink_TrackObj;

Int32 AlgLink_TrackalgCreate(AlgLink_TrackObj * pObj);
Int32 AlgLink_TrackalgDelete(AlgLink_TrackObj * pObj);
Int32 AlgLink_TrackalgProcessFrame(AlgLink_TrackObj * pObj, FVID2_Frame *pFrame);

#endif
