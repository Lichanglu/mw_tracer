/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _ALG_LINK_STUTRACK_PRIV_H_
#define _ALG_LINK_STUTRACK_PRIV_H_

#include <mcfw/src_bios6/utils/utils.h>
#include <mcfw/src_bios6/links_c6xdsp/system/system_priv_c6xdsp.h>
#include <mcfw/interfaces/link_api/algLink.h>


#define STU_MAX_CHECKNUM 20
#define STU_MAX_MVDET_TRIGGER	10 //学生上讲台入口，最多10个
typedef struct tstucheck_point
{
	int16_t	nCheckFlag;
	int16_t	nCheckTime;
	int16_t	nCheckX;
	int16_t	nCheckY;
}TStuCheckPoint;

/**
* @ 移动侦测信息结构体
*/
typedef struct _mvdet_info
{
	int nMoveDetectFlag;//启动移动侦测标志
	int nMoveDetectDelay;//检测到起立后启动移动侦测延时
	int nMoveDetectNum;
	int nFindMoveFlag[STUDENTS_MULTITARGET_NUM];//检测到移动目标标志
	int nFindMoveDelay[STUDENTS_MULTITARGET_NUM];//检测到移动目标后延时，在延时范围内该区域不再侦测移动目标
	int nFindMoveFlag_down[STUDENTS_MULTITARGET_NUM];//当前帧对应框下讲台标志
	int nFindMoveDelay_down[STUDENTS_MULTITARGET_NUM];//检测到下讲台后延时，在延时范围内该区域不再侦测下讲台
}t_mvdet_info;

typedef struct _T_STU_GLOBAL_OBJ {
	
	uint8_t *students_src_y_buf[2];   //存放缩放后Ybuffer数据,每10帧存放一次原始数据 
	
	uint8_t *students_dst_y_buf[3] ;		 //存放上一帧Ybuffer数据
	
	uint8_t *students_time_count[9];//0存放当前帧，n存放向前第n帧
	uint8_t *students_dst_count[9];//实际操作内存指针，与students_time_count配合使用，为了减少内存拷贝
	uint8_t *Students_Area_vertex; //表示点是否属于检测区域
	
	int32_t nSrcBufWidth;//采集原始图像的数据摆放buf的宽高
	int32_t nSrcBufHeight;//采集原始图像的数据摆放buf的宽高
	int16_t video_switch_stop;	//为1表示老师下讲台不跟踪,只切换到学生全景,为0时表示学生跟踪生效
	int32_t nFrameNum;
	int32_t nCopyPos;
	int32_t nCopy10FrameNum;
	int32_t nCopyFramNum;
	
	int16_t Preset_Location[STUDENTS_PRESET_NUM];	//1~36
	int16_t Preset_Delay[STUDENTS_PRESET_NUM];
	int16_t Last_Location[STUDENTS_PRESET_NUM];	//1~36,上一次的触发框
	int16_t k_Nearest[3];		//最近的点
	int32_t nMvXPos[STU_MAX_MVDET_TRIGGER][9];
	int32_t nMvYPos[STU_MAX_MVDET_TRIGGER][9];
	t_mvdet_info tMoveInfo;
	TStuCheckPoint t_check_point[STU_MAX_CHECKNUM];

}T_STU_GLOBAL_OBJ;

typedef struct _StuTrackHand {
    StuITRACK_Params	input_param;	//输入参数
    T_STU_GLOBAL_OBJ t_stu_global_obj;
}StuTrackHand;


typedef struct _AlgLink_StuTrackChObj{

	UInt8 chId;
	UInt32 rtParamUpdatePerFrame;
	
	StuTrackHand stutrackhandle;
		
} AlgLink_StuTrackChObj;

typedef struct _AlgLink_StuTrackObj{
    System_LinkQueInfo * inQueInfo;

    AlgLink_StuTrackChObj chObj[ALG_LINK_STUTRACK_MAX_CH];

    AlgLink_StuTrackCreateParams  stutrackChCreateParams[ALG_LINK_STUTRACK_MAX_CH];

	Int32 ChToStream[ALG_LINK_STUTRACK_MAX_TO_CH];

	Utils_QueHandle stutrackoutFrameBufQue;

	FVID2_Frame        *queueMem[100];
	
	Int32 ProcessFlag;   //0 free 1 process

	PrintState algstate;
} AlgLink_StuTrackObj;

Int32 AlgLink_StuTrackalgCreate(AlgLink_StuTrackObj * pObj);
Int32 AlgLink_StuTrackalgDelete(AlgLink_StuTrackObj * pObj);
Int32 AlgLink_StuTrackalgProcessFrame(AlgLink_StuTrackObj * pObj, FVID2_Frame *pFrame);

#endif
