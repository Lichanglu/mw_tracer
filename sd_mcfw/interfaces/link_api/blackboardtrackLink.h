/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


#ifndef _STU_BLACKBOARD_TRACk_LINK_H_
#define _STU_BLACKBOARD_TRACk_LINK_H_


#include <mcfw/interfaces/link_api/trackcomnLink.h>
/**
    \brief Max supported TRACE channels
*/
#define ALG_LINK_BLACKBOARDTRACK_MAX_CH      (1)   //�˺겻���޸�


//#define ALG_LINK_STUDOUBLESIDETRACK_MAX_CH      (8)



/**
* @	������������õ�������
*/
#define	BLACKBOARDTRACK_AREA_POINT_MAX	(10)

#define TRIGGER_NUM_MAX (11)

#define	TRACK_TRIGGER_MAX_NUM 	(2)	

/**
* @ ����Զ����ٵľ�̬����
*/
typedef struct
{
	int32_t	size;
	int16_t	video_width;			//������Ƶ�Ŀ�
	int16_t	video_height;			//������Ƶ�ĸ�
	int16_t	pic_width;				//�������ź�ͼ��Ŀ�
	int16_t	pic_height;				//�������ź��ͼ��ĸ�
}ITRACK_Blackboardtrack_static_param_t;

/**
* @ ����Զ����ٵĶ�̬����
*/

typedef struct
{
	int32_t	size;
	int16_t	trigger_sum;		//����������
	int16_t	track_mode;			//����ģʽ��0��ʾ�Ͳɼ��õ�ԭʼ֡�Ƚϣ�1��ʾ����֡�ȽϿ��Ƿ��б仯
	int16_t	start_track_time;	//��ֵʱ�䣬������೤ʱ�俪ʼ����,��λ��֡
	int16_t	reset_time;			//���ٶ�ʧ���´ο�ʼ������ʱ��
	int16_t	control_mode;	//��������,0Ϊ�Զ�,1Ϊ�ֶ�
	int16_t	sens;			//
	int16_t	message;		//��Ҫ�����趨����Щ�ߣ������Ƿ��˵������ߵ�,1:��ʾ���б�Ե��״̬��
							//2:����45�ȣ�90�ȣ�135�ȣ�180�ȵ��߻�������3:��ģ��;4:�����ٿ�;5:�����ٿ��ڵķ�ɫ��
	int16_t	track_point_num;//���ٿ�����ã�����м����㣬(������Щ�������ӳɿ�����˸�������)
	vertex_t	track_point[BLACKBOARDTRACK_AREA_POINT_MAX];	//����������x��y��������,�����������֮ǰͼ����˵
	trigger_info_t trigger[TRACK_TRIGGER_MAX_NUM];
	control_init_type_e	reset_level;	//0��ʾ��Ҫ���³�ʼ��,1��ʾ��Ҫ���³�ʼ��
}ITRACK_Blackboardtrack_dynamic_params_t;


/**
* @ ����Զ����ٵ��������
*/
typedef struct
{
	int32_t	size;
	ITRACK_Blackboardtrack_static_param_t 	static_param;
	ITRACK_Blackboardtrack_dynamic_params_t	dynamic_param;
	//ITRACK_DynamicParams dynamic_param;
}BlackboardTrack_Params;


/**
* @ ����Զ����ٵ��������
*/
typedef struct
{
	int32_t	size;
	int32_t	server_cmd;						//���͸������������л��������ǲ���ѧ����������ʦ��������
	int32_t	cam_position;					//��������ͷת��ĳ��Ԥ��λ�ĵط�
	int32_t	move_distance;					//����ͷ�ƶ�����
	int32_t	move_direction;					//����ͷ�ƶ�����
	int32_t	cmd_type;						//2Ϊ����վ����Ϊ1��ʾ����վ����0��ʾ���¡�
	int32_t	track_status;					//����״̬
	int32_t	trigger_num;					//��������
	//vertex_t	trigger[TRIGGER_NUM_MAX];	//����������
	vertex_t	trigger[TRIGGER_NUM_MAX];	//����������
	out_trigger_point_info_t	trigger_point[TRIGGER_NUM_MAX];	//����Ӧ�����Զ�ˢ�±�����
	int32_t     IsTrigger;
	uint32_t	reserve1;
	uint32_t	reserve2;
	uint32_t	reserve3;
	uint32_t	reserve4;
}BlackboardTrack_OutArgs;


typedef struct
{
    UInt32 chId;
	UInt32 SaveViewFlag;
	UInt32 SaveType;
//	trigger_point_info_t	g_trigger_info[TRIGGER_POINT_NUM];
} AlgLink_BlackboardTrackSaveView;

/**
    \brief OSD channel create time configuration parameters
*/
typedef struct
{
	UInt8 chId;

	BlackboardTrack_Params BlackboardTrackParms;
	UInt8  *BlackboardTrackAddr[2][2];
	//AlgLink_BlackboardTrackSaveView  SaveView;
	Int32 StreamId;//ָ����·������·ͨ��
 
} AlgLink_BlackboardTrackCreateParams;


typedef struct
{
    UInt32 chId;
    /**< StuSideTrack channel number on which tamper event was detected

        Valid values, 0..-1
    */
	BlackboardTrack_OutArgs args;

} AlgLink_BlackboardTrackChStatus;



/*==============================================================================
    ����: <AlgLink_BlackboardTrackalgInit>
    ����: 
    ����: 
    ����ֵ: 
    Created By ��� 2014.03.03 10:49:25 For Web
==============================================================================*/
static inline void AlgLink_BlackboardTrackalgInit(AlgLink_BlackboardTrackCreateParams * pChPrm)
{
	BlackboardTrack_Params *pTrackParms = &pChPrm->BlackboardTrackParms;

	pChPrm->BlackboardTrackAddr[0][0] = NULL;
	pChPrm->BlackboardTrackAddr[0][1] = NULL;
	pChPrm->BlackboardTrackAddr[1][0] = NULL;
	pChPrm->BlackboardTrackAddr[1][1] = NULL;
	
	//memset(&pChPrm->SaveView, 0x0, sizeof(AlgLink_BlackboardTrackSaveView));
	memset(pTrackParms, 0x0, sizeof(BlackboardTrack_Params));
	pTrackParms->size = 0;
	pTrackParms->static_param.size = 0;
	pTrackParms->static_param.pic_width    = 180;
	pTrackParms->static_param.pic_height   = 120;
	pTrackParms->static_param.video_width  = 704;
	pTrackParms->static_param.video_height = 576;

	pTrackParms->dynamic_param.size = 0;
	pTrackParms->dynamic_param.trigger_sum = 30;
	pTrackParms->dynamic_param.track_mode = 1;			//����ģʽ��0��ʾ�Ͳɼ��õ�ԭʼ֡�Ƚϣ�1��ʾ����֡�ȽϿ��Ƿ��б仯
	pTrackParms->dynamic_param.start_track_time = 0;	//��ֵʱ�䣬������೤ʱ�俪ʼ����,��λ��֡
	pTrackParms->dynamic_param.reset_time = 0;			//���ٶ�ʧ���´ο�ʼ������ʱ��
	pTrackParms->dynamic_param.control_mode = 0;	//��������,0Ϊ�Զ�,1Ϊ�ֶ�
	pTrackParms->dynamic_param.sens = 30;			//
	pTrackParms->dynamic_param.message = 0;		//��Ҫ�����趨����Щ�ߣ������Ƿ��˵������ߵ�,1:��ʾ���б�Ե��״̬��
										//2:����45�ȣ�90�ȣ�135�ȣ�180�ȵ��߻�������3:��ģ��;4:�����ٿ�;5:�����ٿ��ڵķ�ɫ��
	pTrackParms->dynamic_param.track_point_num  =  4;//���ٿ�����ã�����м����㣬(������Щ�������ӳɿ�����˸�������)
	pTrackParms->dynamic_param.track_point[0].x = 60;	
	pTrackParms->dynamic_param.track_point[0].y = 48;	
	pTrackParms->dynamic_param.track_point[1].x = 97;	
	pTrackParms->dynamic_param.track_point[1].y = 59;
	pTrackParms->dynamic_param.track_point[2].x = 97;	
	pTrackParms->dynamic_param.track_point[2].y = 59;	
	pTrackParms->dynamic_param.track_point[3].x = 64;	
	pTrackParms->dynamic_param.track_point[4].y = 64;	


	pTrackParms->dynamic_param.reset_level = RE_INIT;	//0��ʾ��Ҫ���³�ʼ��,1��ʾ��Ҫ���³�ʼ��
	

	pChPrm->StreamId = -1;
	
}


#endif

/*@}*/

