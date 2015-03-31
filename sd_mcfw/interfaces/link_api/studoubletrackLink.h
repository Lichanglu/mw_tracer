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

#ifndef _STU_DOUBLE_SIDETRACk_LINK_H_
#define _STU_DOUBLE_SIDETRACk_LINK_H_


#include <mcfw/interfaces/link_api/trackcomnLink.h>
/**
    \brief Max supported TRACE channels
*/
#define ALG_LINK_STUDOUBLESIDETRACK_MAX_CH      (2)


//#define ALG_LINK_STUDOUBLESIDETRACK_MAX_CH      (8)



/**
* @	������������õ�������
*/
#define	TRACK_STUDENTS_DOUBLE_SIDE_AREA_POINT_MAX	(10)

#define TRIGGER_NUM_MAX (11)


/**
* @ ����Զ����ٵľ�̬����
*/
typedef struct 	_ITRACK_STUDENTS_DOUBLE_SIDE_static_param_t
{
	int32_t	size;
	int16_t	video_width;			//������Ƶ�Ŀ�
	int16_t	video_height;			//������Ƶ�ĸ�
	int16_t	pic_width;				//�������ź�ͼ��Ŀ�
	int16_t	pic_height;				//�������ź��ͼ��ĸ�
}ITRACK_STUDENTS_DOUBLE_SIDE_static_param_t;

/**
* @ ����Զ����ٵĶ�̬����
*/
typedef struct 	_ITRACK_STUDENTS_DOUBLE_SIDE_dynamic_params_t
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
	vertex_t	track_point[TRACK_STUDENTS_DOUBLE_SIDE_AREA_POINT_MAX];	//����������x��y��������,�����������֮ǰͼ����˵
	control_init_type_e	reset_level;	//0��ʾ��Ҫ���³�ʼ��,1��ʾ��Ҫ���³�ʼ��
	int32_t  refreshTime;
	int32_t  refreshType;
}ITRACK_STUDENTS_DOUBLE_SIDE_dynamic_params_t;


/**
* @ ����Զ����ٵ��������
*/
typedef struct 	_StuDoubleSideITRACK_Params
{
	int32_t	size;
	ITRACK_STUDENTS_DOUBLE_SIDE_static_param_t 	static_param;
	ITRACK_STUDENTS_DOUBLE_SIDE_dynamic_params_t	dynamic_param;
}StuDoubleSideITRACK_Params;


/**
* @ ����Զ����ٵ��������
*/
typedef struct	_StuDoubleSideITRACK_OutArgs
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
	
	uint32_t	reserve1;
	uint32_t	reserve2;
	uint32_t	reserve3;
	uint32_t	reserve4;
}StuDoubleSideITRACK_OutArgs;


typedef struct
{
    UInt32 chId;
	UInt32 SaveViewFlag;
	UInt32 SaveType;
	trigger_point_info_t	g_trigger_info[TRIGGER_POINT_NUM];
} AlgLink_StuDoubleSideTrackSaveView;

/**
    \brief OSD channel create time configuration parameters
*/
typedef struct
{
	UInt8 chId;

	StuDoubleSideITRACK_Params StuDoubleSideTrackParms;
	UInt8  *StuDoubleSideAddr[2][2];
	AlgLink_StuDoubleSideTrackSaveView  SaveView;
//	trigger_point_info_t	g_trigger_info[TRIGGER_POINT_NUM];
	Int32 StreamId;//ָ����·������·ͨ��
 
} AlgLink_StuDoubleSideTrackCreateParams;


typedef struct
{
    UInt32 chId;
    /**< StuSideTrack channel number on which tamper event was detected

        Valid values, 0..-1
    */
	StuDoubleSideITRACK_OutArgs args;

} AlgLink_StuDoubleSideTrackChStatus;



/*==============================================================================
    ����: <AlgLink_StuDoubleSideTrackalgInit>
    ����: 
    ����: 
    ����ֵ: 
    Created By ��� 2014.03.03 10:49:25 For Web
==============================================================================*/
static inline void AlgLink_StuDoubleSideTrackalgInit(AlgLink_StuDoubleSideTrackCreateParams * pChPrm)
{
	StuDoubleSideITRACK_Params *pTrackParms = &pChPrm->StuDoubleSideTrackParms;

	pChPrm->StuDoubleSideAddr[0][0] = NULL;
	pChPrm->StuDoubleSideAddr[0][1] = NULL;
	pChPrm->StuDoubleSideAddr[1][0] = NULL;
	pChPrm->StuDoubleSideAddr[1][1] = NULL;
	
	memset(&pChPrm->SaveView, 0x0, sizeof(AlgLink_StuDoubleSideTrackSaveView));
	memset(pTrackParms, 0x0, sizeof(StuDoubleSideITRACK_Params));
	pTrackParms->size = 0;
	pTrackParms->static_param.size = 0;
	pTrackParms->static_param.pic_width    = 180;
	pTrackParms->static_param.pic_height   = 120;
	pTrackParms->static_param.video_width  = 704;
	pTrackParms->static_param.video_height = 576;

	pTrackParms->dynamic_param.refreshTime = 0;
	pTrackParms->dynamic_param.size = 0;
	pTrackParms->dynamic_param.trigger_sum = 60;
	pTrackParms->dynamic_param.track_mode = 0;			//����ģʽ��0��ʾ�Ͳɼ��õ�ԭʼ֡�Ƚϣ�1��ʾ����֡�ȽϿ��Ƿ��б仯
	pTrackParms->dynamic_param.start_track_time = 0;	//��ֵʱ�䣬������೤ʱ�俪ʼ����,��λ��֡
	pTrackParms->dynamic_param.reset_time = 0;			//���ٶ�ʧ���´ο�ʼ������ʱ��
	pTrackParms->dynamic_param.control_mode = 0;	//��������,0Ϊ�Զ�,1Ϊ�ֶ�
	pTrackParms->dynamic_param.sens = 20;			//
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

