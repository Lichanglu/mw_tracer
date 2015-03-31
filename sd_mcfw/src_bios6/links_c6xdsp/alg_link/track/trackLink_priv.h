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
* @	�����ڲ���ȫ�ֱ���
*/
typedef struct _T_GLOBAL_OBJ
{
	uint8_t *Area_vertex; //��ʾ���Ƿ����ڼ������
	
	uint8_t *src_y_buf; //���Ҫ���������Դybuf����
	uint8_t *src_cb_buf; //���Ҫ���������Դcbbuf����
	uint8_t *src_cr_buf; //���Ҫ���������Դcrbuf����
	
	uint8_t *dst_y_buf; //���ÿ�θ��ٺ�ĵ�һ֡ybuf����
	uint8_t *dst_cb_buf; //���ÿ�θ��ٺ�ĵ�һ֡cbbuf����
	uint8_t *dst_cr_buf; //���ÿ�θ��ٺ�ĵ�һ֡crbuf����
	
	/*�ڽ��������buffer�����У�ÿ�����ֵ��ʾ��������ĸ��Ƕȵ�ֵ������Ϊ1����ʾ��45�ȣ�Ϊ2��ʾ135�ȣ��ȵ�*/
	uint8_t *img_coordinate;
	uint8_t *img_coordinate_g;
	
	/*��������45�ȣ�90�ȣ�135�ȣ�180�ȵ��x,y*/
	uint8_t *Point_Buffer;
	
	/*�ֱ�����45�ȣ�90�ȣ�135�ȣ�180�ȵ�����ԣ�����Ҫ���ĵ�Ĵ�С*/
	uint8_t *T_L_Buffer;
	uint8_t *T_R_Buffer;
	uint8_t *T_U_Buffer;
	uint8_t *T_P_Buffer;
	
	/*���ڴ��ԭʼbuf������*/
	uint8_t *output_cb_buf;
	uint8_t *output_cr_buf;
	
	int32_t m_offset;		//Ŀ�������λ��
	int32_t n_offset;		//Ŀ�������λ��
	
	/*��ʾÿ�ֽǶȵĵ�ĸ���*/
	int32_t Point_180_Inc;
	int32_t Point_90_Inc;
	int32_t Point_45_Inc;
	int32_t Point_135_Inc;
	int32_t Point_Inc;

	int32_t nSrcBufWidth;//�ɼ�ԭʼͼ������ݰڷ�buf�Ŀ��
	int32_t nSrcBufHeight;//�ɼ�ԭʼͼ������ݰڷ�buf�Ŀ��
	int16_t	turn_flag;			//Ϊ0ʱ��ʾ����Ҫ���¸��٣�Ϊ1ʱ��ʾ����Ŀ�꣬���ø��ٱ�־��Ϊ2ʱ��ʾ����Ŀ�궪ʧ
	int16_t	turn_time;			//��λ��Ŀ���,��Ҫ�ȴ�����ͷ����Ԥ��λ��ʱ��
	int16_t	track_start;		//��ʾ�Ƿ������Ŀ�꣬0��ʾδ�����ϣ�1��ʾ��������Ŀ��
	int32_t	first_time;			//��ʼ�����㷨��֡��,Ŀǰ��ǰʮ֡��������
	int32_t	last_move_flag;		//�����ƶ���־,Ϊ0��ʾδ�ƶ�����,Ϊ1��ʾ�ƶ��˷���
	int32_t	last_track_status;	//�ϴθ���״̬	
	int32_t	skin_colour_num;	//������⵽��ɫ��֡��
	/**
	* @ ��Ÿ��ٴ�������
	*/
	int16_t 	model_v_cnt[MAX_HEAD_NUM];
	uint16_t	model_v_mnt[MAX_HEAD_NUM];
	uint16_t	model_v_pnt[MAX_HEAD_NUM];
	
	/**
	* @ ���ģ��
	*/
	uint8_t 	model[MODEL_DISTANCE_CHOSE_NUM][MODEL_NUM_MAX];
	/**
	* @ ���ٴ���Ľṹ��,���ڴ�Ÿ�����Ҫ������
	*/
	STR_OBJECTS_DETECTED pst_det_obj;
	
	/**
	* @ ���ٴ���Ľṹ��,���ڴ�Ÿ��ٴ�������
	*/
	STR_OBJECT_TRAKING pst_trak_obj;

	t_TrackingObject tAutoTrackObj;
	int32_t nLastTrackState;
	int32_t nTrackType;//��������,0Ϊ��������,1Ϊmeanshift����,2Ϊ������meanshiftͬʱ����
	int32_t nTargetFlag;//��ʾ�����ҵ�Ŀ��
	int32_t nTrackStartFlag;//��ʾԤ������ʱ����,��ʼ���ٱ�־,������û���ҵ�Ŀ��
	int32_t nMeanShiftFrame;//meanshift����ʱ��֡ͳ�ƣ�ÿ10֡���һ������
	int32_t nMeanShiftFaceLost;//meanshift����ͬʱ�������������û�ҵ��Ĵ���
	int32_t nPos1MvFlag;//Ԥ��λ1ʱȫ����⵽�˶���־
	int32_t nPos1MvLost;//Ԥ��λ1ʱȫ��δ��⵽�˶�ʱ��
	int32_t nLostNum;//Ԥ��λ1ʱȫ��δ��⵽�˶�ʱ��

	
	int32_t debugValue0;
	int32_t debugValue1;
	int32_t debugValue2;

}T_GLOBAL_OBJ;


typedef struct _TrackHand {
    ITRACK_Params	input_param;	//�������
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
