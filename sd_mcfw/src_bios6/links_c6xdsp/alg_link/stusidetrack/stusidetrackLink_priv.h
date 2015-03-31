/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _ALG_LINK_STUSIDETRACK_PRIV_H_
#define _ALG_LINK_STUSIDETRACK_PRIV_H_

#include <mcfw/src_bios6/utils/utils.h>
#include <mcfw/src_bios6/links_c6xdsp/system/system_priv_c6xdsp.h>
#include <mcfw/interfaces/link_api/algLink.h>



//#define MINRECTFILLSIZE (20)
#define MINRECTWIDTH 	(10)
#define MINRECTHEIGHT 	(10)
#define	SHORT_MAX		(65535)
#define	MAXRECTNUM		(1000)		//��⵽�Ŀ���Ϊһ���ŵĸ���
#define	MAX_TARGET_NUM	(100)		//��⵽�ı仯������



/**
* @	�����ڲ���ȫ�ֱ���
*/
typedef struct _T_STU_SIDE_GLOBAL_OBJ
{
	uint8_t *Area_vertex; //��ʾ���Ƿ����ڼ������
	uint8_t *gray_buf;  //���͸�ʴ֮���
	uint8_t *mid_buf;   //ԭʼ
	uint16_t *marked_buf;
	T_Rect	g_out_rect[MAX_TARGET_NUM];
	
	uint8_t *InBuffer; //
	uint8_t *src_y_buf; //���Ҫ���������Դybuf����
	uint8_t *tmp_src_y_buf; //���Ҫ���������Դybuf����
	
	uint8_t *dst_y_buf; //���ÿ�θ��ٺ�ĵ�һ֡ybuf����
	
	int32_t nSrcBufWidth;//�ɼ�ԭʼͼ������ݰڷ�buf�Ŀ��
	int32_t nSrcBufHeight;//�ɼ�ԭʼͼ������ݰڷ�buf�Ŀ��
	int16_t	turn_flag;			//Ϊ0ʱ��ʾ����Ҫ���¸��٣�Ϊ1ʱ��ʾ����Ŀ�꣬���ø��ٱ�־��Ϊ2ʱ��ʾ����Ŀ�궪ʧ
	int16_t	turn_time;			//��λ��Ŀ���,��Ҫ�ȴ�����ͷ����Ԥ��λ��ʱ��
	int16_t	track_start;		//��ʾ�Ƿ������Ŀ�꣬0��ʾδ�����ϣ�1��ʾ��������Ŀ��
	int32_t	first_time;			//��ʼ�����㷨��֡��,Ŀǰ��ǰʮ֡��������
	int32_t	last_move_flag;		//�����ƶ���־,Ϊ0��ʾδ�ƶ�����,Ϊ1��ʾ�ƶ��˷���
	int32_t	last_track_status;	//�ϴθ���״̬
	int32_t	skin_colour_num;	//������⵽��ɫ��֡��
	uint64_t	frame_num;		//�㷨֡��
	
	T_Rect 					tInRect;
	
} T_STU_SIDE_GLOBAL_OBJ;


typedef struct _StuSideTrackHand
{
	StuSideITRACK_Params	input_param;	//�������
	T_STU_SIDE_GLOBAL_OBJ t_global_obj;
} StuSideTrackHand;


typedef struct _AlgLink_StuSideTrackChObj
{

	UInt8 chId;
	UInt32 rtParamUpdatePerFrame;
	UInt8  *StuSideAddr[2][2];
	StuSideTrackHand stusidetrackhandle;
	
} AlgLink_StuSideTrackChObj;

typedef struct _AlgLink_StuSieTrackObj
{
	System_LinkQueInfo * inQueInfo;
	
	AlgLink_StuSideTrackChObj chObj[ALG_LINK_STUSIDETRACK_MAX_CH];
	
	AlgLink_StuSideTrackCreateParams  stusidetrackChCreateParams[ALG_LINK_STUSIDETRACK_MAX_CH];
	
	Int32 ChToStream[ALG_LINK_STUSIDETRACK_MAX_TO_CH];
	
	Utils_QueHandle stusidetrackoutFrameBufQue;
	
	FVID2_Frame        *queueMem[100];
	
	Int32 ProcessFlag;   //0 free 1 process
	
	PrintState algstate;
} AlgLink_StuSideTrackObj;

Int32 AlgLink_StuSideTrackalgCreate(AlgLink_StuSideTrackObj * pObj);
Int32 AlgLink_StuSideTrackalgDelete(AlgLink_StuSideTrackObj * pObj);
Int32 AlgLink_StuSideTrackalgProcessFrame(AlgLink_StuSideTrackObj * pObj, FVID2_Frame *pFrame);

#endif
