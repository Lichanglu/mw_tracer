/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _ALG_LINK_STUDOUBLESIDETRACK_PRIV_H_
#define _ALG_LINK_STUDOUBLESIDETRACK_PRIV_H_

#include <mcfw/src_bios6/utils/utils.h>
#include <mcfw/src_bios6/links_c6xdsp/system/system_priv_c6xdsp.h>
#include <mcfw/interfaces/link_api/algLink.h>



//#define MINRECTFILLSIZE (20)
#define MINRECTWIDTH 	(10)//(10)
#define MINRECTHEIGHT 	(10)//(10)
#define	SHORT_MAX		(65535)
#define	MAXRECTNUM		(1000)		//检测到的可能为一个团的个数
#define	MAX_TARGET_NUM	(100)		//检测到的变化的团数


/**
* @	用于内部的全局变量
*/
typedef struct _T_STU_DOUBLE_SIDE_GLOBAL_OBJ
{
	uint8_t *Area_vertex; //表示点是否属于检测区域
	uint8_t *gray_buf;  //膨胀腐蚀之后的
	uint8_t *mid_buf;   //原始
	uint16_t *marked_buf;
	T_Rect	g_out_rect[MAX_TARGET_NUM];

	uint8_t *InBuffer; //
	uint8_t *src_y_buf; //存放要进行运算的源ybuf数据
	uint8_t *src_cbcr_buf;//存放要进行运算的源cbcrbuf数据
	
	uint8_t *tmp_src_y_buf; //存放要进行运算的源ybuf数据
	uint8_t *tmp_src_cbcr_buf; //存放要进行运算的源ybuf数据
	
	uint8_t *dst_y_buf; //存放每次跟踪后的第一帧ybuf数据
	uint8_t *dst_cbcr_buf;//存放每次跟踪后的第一帧cbcrbuf数据
	
	int32_t nSrcBufWidth;//采集原始图像的数据摆放buf的宽高
	int32_t nSrcBufHeight;//采集原始图像的数据摆放buf的宽高
	int16_t	turn_flag;			//为0时表示不需要重新跟踪，为1时表示捕获到目标，设置跟踪标志，为2时表示跟踪目标丢失
	int16_t	turn_time;			//定位到目标后,需要等待摄像头调用预置位的时间
	int16_t	track_start;		//表示是否跟踪上目标，0表示未跟踪上，1表示跟踪上了目标
	int32_t	first_time;			//开始进入算法的帧数,目前是前十帧不做处理
	int32_t	last_move_flag;		//方向移动标志,为0表示未移动方向,为1表示移动了方向
	int32_t	last_track_status;	//上次跟踪状态
	int32_t	skin_colour_num;	//连续检测到肤色的帧数
	uint64_t	frame_num;		//算法帧数

	T_Rect 					tInRect;

	int32_t debugValue0;
	int32_t debugValue1;
	int32_t debugValue2;

}T_STU_DOUBLE_SIDE_GLOBAL_OBJ;


typedef struct _StuDoubleSideTrackHand {
    StuDoubleSideITRACK_Params	input_param;	//输入参数
    T_STU_DOUBLE_SIDE_GLOBAL_OBJ t_global_obj;
	void *pBackGroundModel;
}StuDoubleSideTrackHand;


typedef struct _AlgLink_StuDoubleSideTrackChObj{

	UInt8 chId;
	UInt32 rtParamUpdatePerFrame;
	UInt8  *StuDoubleSideAddr[2][2];
	StuDoubleSideTrackHand studoublesidetrackhandle;
} AlgLink_StuDoubleSideTrackChObj;

typedef struct _AlgLink_StuDoubleSieTrackObj{
    System_LinkQueInfo * inQueInfo;

    AlgLink_StuDoubleSideTrackChObj chObj[ALG_LINK_STUDOUBLESIDETRACK_MAX_CH];

    AlgLink_StuDoubleSideTrackCreateParams  studoublesidetrackChCreateParams[ALG_LINK_STUDOUBLESIDETRACK_MAX_CH];

	Int32 StreamId[ALG_LINK_STUDOUBLESIDETRACK_MAX_CH];

	Utils_QueHandle studoublesidetrackoutFrameBufQue;

	FVID2_Frame        *queueMem[100];
	
	Int32 ProcessFlag;   //0 free 1 process

	PrintState algstate;
} AlgLink_StuDoubleSideTrackObj;

Int32 AlgLink_StuDoubleSideTrackalgCreate(AlgLink_StuDoubleSideTrackObj * pObj);
Int32 AlgLink_StuDoubleSideTrackalgDelete(AlgLink_StuDoubleSideTrackObj * pObj);
Int32 AlgLink_StuDoubleSideTrackalgProcessFrame(AlgLink_StuDoubleSideTrackObj * pObj, FVID2_Frame *pFrame);

#endif
