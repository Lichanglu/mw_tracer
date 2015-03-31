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
* @	���ٿ�������
*/
#define	TRACK_TRIGGER_MAX_NUM 	(2)	


#define ITRACK_SETPARAMS 0       /* only cmd we support */


/**
* @	��������Ϣ�Ľṹ��
*/
typedef struct _trigger_info
{
	int16_t	trigger_x0;		//��ʾ����������϶��������xֵ(��Դͼ��Ϊ��׼��)
	int16_t	trigger_y0;		//��ʾ����������϶��������yֵ(��Դͼ��Ϊ��׼��)
	int16_t	trigger_x1;		//��ʾ����������¶��������xֵ(��Դͼ��Ϊ��׼��)
	int16_t	trigger_y1;		//��ʾ����������¶��������yֵ(��Դͼ��Ϊ��׼��)
}trigger_info_t;


/**
* @ ����Զ����ٵľ�̬����
*/
typedef struct 	_track_static_param
{
	UInt32	size;
	UInt16	video_width;			//������Ƶ�Ŀ�
	UInt16	video_height;			//������Ƶ�ĸ�
	UInt16	pic_width;				//�������ź�ͼ��Ŀ�
	UInt16	pic_height;				//�������ź��ͼ��ĸ�
}track_static_param_t;


/**
* @ ����Զ����ٵĶ�̬����,����ӵĲ��� 
*/
typedef struct 	_track_other_param
{
	trigger_info_t blackboard_trigger[2];//�ڰ����򴥷��� 
	trigger_info_t meanshift_trigger;//meanshfit ���ٿ� 
	trigger_info_t pos1_trigger;//ȫ��Ԥ��λ������
	int16_t	pos1_trigger_num;//ȫ��Ԥ��λ�����������������������ʾ ����
	int16_t meanshift_flag;//meanshift ����
	int16_t dst_trigger_x;//�����Ĳο�x,ѡ��������������Ŀ����Ϊ����Ŀ��	
	int16_t dst_trigger_y;//�����Ĳο�y,ѡ��������������Ŀ����Ϊ����Ŀ��
	int16_t	nTrackSwitchType;			//ֻ�в������������ͣ�0Ϊ���٣�1Ϊֻ�в���
}track_other_param_t;


/**
* @ ����Զ����ٵĶ�̬����
*/
typedef struct 	_track_dynamic_param
{
	UInt32	size;
	UInt16	track_mode;
	UInt16	start_track_time;	//��ֵʱ�䣬������೤ʱ�俪ʼ����,��λ��֡
	UInt16	reset_time;			//���ٶ�ʧ���´ο�ʼ������ʱ��
	UInt8	zoom_distance;	//�����Ľ���Զ��ѡ��,ֵԽ��,��������ԽԶ,��0Ϊ�������
	UInt16	x_offset;		//ͼ���x�������ű���,��IMG_X_offset_0
	UInt16	y_offset;		//ͼ���y�������ű���,��IMG_Y_offset_0
	UInt16	trigger_sum;	//��ʾ�ڴ��������м����㱻������,�Ǹ���ֵ,�������㷧ֵ�������ֵʱ,��Ϊ��������T_p_sum
	trigger_info_t trigger[TRACK_TRIGGER_MAX_NUM];
	UInt16	control_mode;	//��������,0Ϊ�Զ�,1Ϊ�ֶ�
	UInt16	sens;			//
	UInt16	message;		//��Ҫ�����趨����Щ�ߣ������Ƿ��˵������ߵ�,1:��ʾ���б�Ե��״̬��
							//2:����45�ȣ�90�ȣ�135�ȣ�180�ȵ��߻�������3:��ģ��;4:�����ٿ�;5:�����ٿ��ڵķ�ɫ��
	UInt16	model_sum;		//Ҫƥ���ģ���ܸ���
	UInt16 track_sel;		//���ж��Ŀ��(��)��ʱ���ѡ����ԣ�Ϊ1��ʱ��ѡ����ߵģ�2��ʱ��ѡ��������Ǹ�
	UInt16 process_type;	//Ϊ0ʱ�����㷨����,Ϊ1ʱֻ����������,��������͸��ٿ�
	UInt16	mid_x;			//�趨������λ��x��ֵ(�������720����4����˵�ģ�Ҳ���������180�����)
	UInt16	mid_y;			//�趨������λ��y��ֵ(�������576����4����˵�ģ�Ҳ���������144�����)
	UInt16	track_point_num;//���ٿ�����ã�����м����㣬(������Щ�������ӳɿ�����˸�������)
	vertex_t	track_point[TRACK_AREA_POINT_MAX];	//����������x��y��������,�����������֮ǰͼ����˵
	UInt16	limit_height;	//�����ߣ��������������֮�£����������(��Դͼ��Ϊ��׼��)
	UInt16  model_multiple;	//
	UInt16	model_level;	//
	control_init_type_e	reset_level;	//0��ʾ��Ҫ���³�ʼ��,1��ʾ��Ҫ���³�ʼ��
//	track_other_param_t *ptTrackOtherParam;
	track_other_param_t TrackOtherParam;
}ITRACK_DynamicParams;


/**
* @ ����Զ����ٵ��������
*/
typedef struct 	ITRACK_Params
{
	UInt32	size;
	track_static_param_t 	static_param;
	ITRACK_DynamicParams	dynamic_param;
}ITRACK_Params;



/**
* @ ����Զ����ٵ��������
*/
typedef struct	_track_output_param
{
	Int32	size;
	Int32	server_cmd;						//���͸������������л��������ǲ���ѧ����������ʦ��������
	Int32	cam_position;					//��������ͷת��ĳ��Ԥ��λ�ĵط�
	Int32	move_distance;					//����ͷ�ƶ�����
	Int32	move_direction;					//����ͷ�ƶ�����
	Int32	cmd_type;						//��������
	Int32	track_status;					//����״̬
	Int32	position1_mv_status;			//Ԥ��λΪ1 ��ȫ��ʱ���˶�״̬ 
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

	Int32 ChToStream[ALG_LINK_TRACK_MAX_TO_CH];//ָ����·������·ͨ��

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
    ����: <AlgLink_TrackalgInit>
    ����: ����������ʼ��(��Ҫ��Ӧ���ṩ)
    ����: 
    ����ֵ: 
    Created By ��� 2013.04.11 11:12:51 For Web
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

