/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _ALG_LINK_BLACKBOARD_TRACK_PRIV_H_
#define _ALG_LINK_BLACKBOARD_TRACK_PRIV_H_

#include <mcfw/src_bios6/utils/utils.h>
#include <mcfw/src_bios6/links_c6xdsp/system/system_priv_c6xdsp.h>
#include <mcfw/interfaces/link_api/algLink.h>



//#define MINRECTFILLSIZE (20)
#define MINRECTWIDTH 	(10)//(10)
#define MINRECTHEIGHT 	(10)//(10)
#define	SHORT_MAX		(65535)
#define	MAXRECTNUM		(1000)		//��⵽�Ŀ���Ϊһ���ŵĸ���
#define	MAX_TARGET_NUM	(100)		//��⵽�ı仯������


/**
* @	�����ڲ���ȫ�ֱ���
*/
typedef struct
{
	uint8_t *Area_vertex; //��ʾ���Ƿ����ڼ������
	uint8_t *gray_buf;  //���͸�ʴ֮���
	uint8_t *mid_buf;   //ԭʼ
	uint16_t *marked_buf;
	T_Rect	g_out_rect[MAX_TARGET_NUM];

	uint8_t *InBuffer; //
	uint8_t *src_y_buf; //���Ҫ���������Դybuf����
	uint8_t *src_cbcr_buf;//���Ҫ���������Դcbcrbuf����
	
	uint8_t *tmp_src_y_buf; //���Ҫ���������Դybuf����
	uint8_t *tmp_src_cbcr_buf; //���Ҫ���������Դybuf����
	
	uint8_t *dst_y_buf; //���ÿ�θ��ٺ�ĵ�һ֡ybuf����
	uint8_t *dst_cbcr_buf;//���ÿ�θ��ٺ�ĵ�һ֡cbcrbuf����

	uint8_t *pre_frame;//��һ֡
	
	int32_t nSrcBufWidth;//�ɼ�ԭʼͼ������ݰڷ�buf�Ŀ���
	int32_t nSrcBufHeight;//�ɼ�ԭʼͼ������ݰڷ�buf�Ŀ���
	int16_t	turn_flag;			//Ϊ0ʱ��ʾ����Ҫ���¸��٣�Ϊ1ʱ��ʾ����Ŀ�꣬���ø��ٱ�־��Ϊ2ʱ��ʾ����Ŀ�궪ʧ
	int16_t	turn_time;			//��λ��Ŀ���,��Ҫ�ȴ�����ͷ����Ԥ��λ��ʱ��
	int16_t	track_start;		//��ʾ�Ƿ������Ŀ�꣬0��ʾδ�����ϣ�1��ʾ��������Ŀ��
	int32_t	first_time;			//��ʼ�����㷨��֡��,Ŀǰ��ǰʮ֡��������
	int32_t	last_move_flag;		//�����ƶ���־,Ϊ0��ʾδ�ƶ�����,Ϊ1��ʾ�ƶ��˷���
	int32_t	last_track_status;	//�ϴθ���״̬
	int32_t	skin_colour_num;	//������⵽��ɫ��֡��
	uint64_t	frame_num;		//�㷨֡��

	T_Rect 					tInRect;
	int32_t	last_frame_status;

	int32_t debugValue0;
	int32_t debugValue1;
	int32_t debugValue2;

}T_BLACKBOARD_TRACK_GLOBAL_OBJ;


typedef struct 
{
    BlackboardTrack_Params	input_param;	//�������
    T_BLACKBOARD_TRACK_GLOBAL_OBJ t_global_obj;
}BlackBoardTrackHand;


typedef struct 
{

	UInt8 chId;
	UInt32 rtParamUpdatePerFrame;
	UInt8  *BlackboardTrackAddr[2][2];
	BlackBoardTrackHand blackboardtrackhandle;
		
} AlgLink_BlackboardTrackChObj;

typedef struct
{
    System_LinkQueInfo * inQueInfo;

    AlgLink_BlackboardTrackChObj chObj[ALG_LINK_BLACKBOARDTRACK_MAX_CH];

    AlgLink_BlackboardTrackCreateParams  blackboardtrackChCreateParams[ALG_LINK_BLACKBOARDTRACK_MAX_CH];

	Int32 StreamId[ALG_LINK_BLACKBOARDTRACK_MAX_CH];

	Utils_QueHandle blackboardtrackoutFrameBufQue;

	FVID2_Frame        *queueMem[100];
	
	Int32 ProcessFlag;   //0 free 1 process

	PrintState algstate;
} AlgLink_BlackboardTrackObj;

Int32 AlgLink_BlackBoardTrackalgCreate(AlgLink_BlackboardTrackObj * pObj);
Int32 AlgLink_BlackBoardTrackalgDelete(AlgLink_BlackboardTrackObj * pObj);
Int32 AlgLink_BlackBoardTrackalgProcessFrame(AlgLink_BlackboardTrackObj * pObj, FVID2_Frame *pFrame);

#endif