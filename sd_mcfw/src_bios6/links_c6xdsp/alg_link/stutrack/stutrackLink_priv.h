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
#define STU_MAX_MVDET_TRIGGER	10 //ѧ���Ͻ�̨��ڣ����10��
typedef struct tstucheck_point
{
	int16_t	nCheckFlag;
	int16_t	nCheckTime;
	int16_t	nCheckX;
	int16_t	nCheckY;
}TStuCheckPoint;

/**
* @ �ƶ������Ϣ�ṹ��
*/
typedef struct _mvdet_info
{
	int nMoveDetectFlag;//�����ƶ�����־
	int nMoveDetectDelay;//��⵽�����������ƶ������ʱ
	int nMoveDetectNum;
	int nFindMoveFlag[STUDENTS_MULTITARGET_NUM];//��⵽�ƶ�Ŀ���־
	int nFindMoveDelay[STUDENTS_MULTITARGET_NUM];//��⵽�ƶ�Ŀ�����ʱ������ʱ��Χ�ڸ�����������ƶ�Ŀ��
	int nFindMoveFlag_down[STUDENTS_MULTITARGET_NUM];//��ǰ֡��Ӧ���½�̨��־
	int nFindMoveDelay_down[STUDENTS_MULTITARGET_NUM];//��⵽�½�̨����ʱ������ʱ��Χ�ڸ�����������½�̨
}t_mvdet_info;

typedef struct _T_STU_GLOBAL_OBJ {
	
	uint8_t *students_src_y_buf[2];   //������ź�Ybuffer����,ÿ10֡���һ��ԭʼ���� 
	
	uint8_t *students_dst_y_buf[3] ;		 //�����һ֡Ybuffer����
	
	uint8_t *students_time_count[9];//0��ŵ�ǰ֡��n�����ǰ��n֡
	uint8_t *students_dst_count[9];//ʵ�ʲ����ڴ�ָ�룬��students_time_count���ʹ�ã�Ϊ�˼����ڴ濽��
	uint8_t *Students_Area_vertex; //��ʾ���Ƿ����ڼ������
	
	int32_t nSrcBufWidth;//�ɼ�ԭʼͼ������ݰڷ�buf�Ŀ��
	int32_t nSrcBufHeight;//�ɼ�ԭʼͼ������ݰڷ�buf�Ŀ��
	int16_t video_switch_stop;	//Ϊ1��ʾ��ʦ�½�̨������,ֻ�л���ѧ��ȫ��,Ϊ0ʱ��ʾѧ��������Ч
	int32_t nFrameNum;
	int32_t nCopyPos;
	int32_t nCopy10FrameNum;
	int32_t nCopyFramNum;
	
	int16_t Preset_Location[STUDENTS_PRESET_NUM];	//1~36
	int16_t Preset_Delay[STUDENTS_PRESET_NUM];
	int16_t Last_Location[STUDENTS_PRESET_NUM];	//1~36,��һ�εĴ�����
	int16_t k_Nearest[3];		//����ĵ�
	int32_t nMvXPos[STU_MAX_MVDET_TRIGGER][9];
	int32_t nMvYPos[STU_MAX_MVDET_TRIGGER][9];
	t_mvdet_info tMoveInfo;
	TStuCheckPoint t_check_point[STU_MAX_CHECKNUM];

}T_STU_GLOBAL_OBJ;

typedef struct _StuTrackHand {
    StuITRACK_Params	input_param;	//�������
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
