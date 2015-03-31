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

#ifndef _STUTRACk_LINK_H_
#define _STUTRACk_LINK_H_


#include <mcfw/interfaces/link_api/trackcomnLink.h>
/**
    \brief Max supported TRACE channels
*/
#define ALG_LINK_STUTRACK_MAX_CH      (1)


#define ALG_LINK_STUTRACK_MAX_TO_CH      (8)
/* @} */

/* Control Commands */

/**
    \ingroup ALG_LINK_API_CMD
    \addtogroup ALG_OSD_LINK_API_CMD  ALG Link: OSD API Control Commands

    @{
*/

/**
    \brief Link CMD: Configure Osd params

    SUPPORTED in ALL platforms

    \param AlgLink_OsdChWinParams * [IN] OSD Window parameters
*/
//#define ALG_LINK_OSD_CMD_SET_CHANNEL_WIN_PRM            (0x6001)


/**
    \brief Link CMD: Set blind window params

    ONLY SUPPORTED in DM810x

    \param AlgLink_OsdChBlindWinParams * [IN] Blind window parameters
*/
//#define ALG_LINK_OSD_CMD_SET_CHANNEL_BLIND_WIN_PRM      (0x6002)



//typedef unsigned char uint8_t;
//typedef float		float_t;
//typedef unsigned short uint16_t;
//typedef unsigned int	uint32_t;
//typedef unsigned long long	uint64_t;



/**
* @ ѧ�����ٴ���������ֵ
*/
#define STUDENTS_TRIGGER_NUM	(35)

/**
* @ ѧ���������ο�����ֵ
*/
#define STUDENTS_SHIELD_NUM	(20)

/**
* @ ѧ�����ٶ�Ŀ����̨��������ֵ
*/
#define STUDENTS_MULTITARGET_NUM	(10)

/**
* @ ѧ������Ԥ��λ�����ֵ,����Ҫ��һ��Ԥ��λ��ȫ��,����Ҫ��1
*/
#define STUDENTS_PRESET_NUM	(STUDENTS_TRIGGER_NUM + 1)


#define ITRACK_SETPARAMS 			0       //���ö�̬����,���˴�������Ϣ
#define ITRACK_SETPARAMS_TRIGGER1 	1       //����ǰ5����������Ϣ
#define ITRACK_SETPARAMS_TRIGGER2 	2       //���õڶ���5����������Ϣ
#define ITRACK_SETPARAMS_TRIGGER3 	3       //���õ�����5����������Ϣ
#define ITRACK_SETPARAMS_TRIGGER4 	4       //���õ��ĸ�5����������Ϣ
#define ITRACK_SETPARAMS_TRIGGER5 	5       //���õ����5����������Ϣ
#define ITRACK_SETPARAMS_TRIGGER6 	6       //���õ�����5����������Ϣ
#define ITRACK_SETPARAMS_TRIGGER7 	7       //���õ��߸5�����������Ϣ

#define ITRACK_SETPARAMS_SHIELD1 	21      //����ǰ5�����ο���Ϣ
#define ITRACK_SETPARAMS_SHIELD2 	22      //���õڶ���5�����ο���Ϣ
#define ITRACK_SETPARAMS_SHIELD3 	23      //���õ�����5�����ο���Ϣ
#define ITRACK_SETPARAMS_SHIELD4 	24      //���õ��ĸ�5�����ο���Ϣ


/**
* @	��������Ϣ�Ľṹ��
*/
typedef struct _student_trigger_info
{
	int16_t	x;
	int16_t	y;	
	int16_t	width;
	int16_t	height;
}student_trigger_info_t;

/**
* @ ����Զ����ٵľ�̬����
*/
typedef struct 	_stutrack_static_param
{
	UInt32	size;
	UInt16	video_width;			//������Ƶ�Ŀ�
	UInt16	video_height;			//������Ƶ�ĸ�
	UInt16	pic_width;				//�������ź�ͼ��Ŀ�
	UInt16	pic_height;				//�������ź��ͼ��ĸ�
}stutrack_static_param_t;

/**
* @ ����Զ����ٵĶ�̬����
*/
typedef struct 	_stutrack_dynamic_param
{
	int32_t	size;
	int16_t	x_offset;		//ͼ���x�������ű���,��IMG_X_offset_0
	int16_t	y_offset;		//ͼ���y�������ű���,��IMG_Y_offset_0
	int16_t	reset_time;		//ѧ��������೤ʱ��û�����µĴ�������Ϊ�������˵�ʱ��
	int16_t	trigger_num;	//��ʾ���������
	//char	*pps;			//����
	student_trigger_info_t	trigger_info[STUDENTS_TRIGGER_NUM];	//ѧ�������ٵĴ�������Ϣ
	student_trigger_info_t	shield_info[STUDENTS_SHIELD_NUM];	//ѧ�������ο���Ϣ
	student_trigger_info_t	multitarget_info[STUDENTS_MULTITARGET_NUM];	//ѧ�������ο���Ϣ
	int16_t	control_mode;	//��������,0Ϊ�Զ�,1Ϊ�ֶ�
	int16_t	sens;			//
	int16_t	message;		//��Ҫ�����趨����Щ�ߣ������Ƿ��˵������ߵ�,1:��ʾ���б�Ե��״̬��
							//2:����45�ȣ�90�ȣ�135�ȣ�180�ȵ��߻�������3:��ģ��;4:�����ٿ�;5:�����ٿ��ڵķ�ɫ��
	int16_t process_type;	//Ϊ0ʱ�����㷨����,Ϊ1ʱֻ����������,��������͸��ٿ�
	int16_t	track_point_num;//���ٿ�����ã�����м����㣬(������Щ�������ӳɿ�����˸�������)
	vertex_t	track_point[TRACK_AREA_POINT_MAX];	//����������x��y��������,�����������֮ǰͼ����˵
	control_init_type_e	reset_level;	//0��ʾ��Ҫ���³�ʼ��,1��ʾ��Ҫ���³�ʼ��
	int16_t	nTrackSwitchType;			//�����л�����,0��ʾ����,1��ʾֻ�л�
	int16_t	nStrategyNo;			//��λ��Ϣ,����ֻ�в����ķ�ʽѡ��
}StuITRACK_DynamicParams;


/**
* @ ����Զ����ٵ��������
*/
typedef struct 	_StuITRACK_Params
{
	UInt32	size;
	stutrack_static_param_t 	static_param;
	StuITRACK_DynamicParams	dynamic_param;
}StuITRACK_Params;




/**
* @ ����Զ����ٵ��������
*/
typedef struct	_stutrack_output_param
{
	int32_t	size;
	int32_t	server_cmd;						//���͸������������л��������ǲ���ѧ����������ʦ��������
	int32_t	cam_position;					//��������ͷת��ĳ��Ԥ��λ�ĵط�
	int32_t	cmd_type;						//Ϊ0��ʾû���κ�ѧ��վ��,Ϊ1��ʾ��1��������վ��

	uint16_t	nStandUpPos[4];//������������
	uint16_t	nFindMoveUpNum;//��̨����
	uint16_t	nFindMoveDownNum;//��̨����

	int32_t	k_0;
	int32_t	k_1;
	int32_t	k_2;
	int32_t	k_3;
	int32_t	k_4;
	int32_t	k_5;
	int32_t	k_6;
	int32_t	k_7;
	int32_t	k_8;
	
	int32_t	c_x;
	int32_t	c_y;
}StuITRACK_OutArgs;


/**
    \brief OSD channel create time configuration parameters
*/
typedef struct
{
	UInt8 chId;

	StuITRACK_Params StuTrackParms;

	Int32 ChToStream[ALG_LINK_STUTRACK_MAX_TO_CH];//ָ����·������·ͨ��
    /**< Initial window params for all channels */
} AlgLink_StuTrackCreateParams;


typedef struct
{
    UInt32 chId;
    /**< SCD channel number on which tamper event was detected

        Valid values, 0..ALG_LINK_SCD_MAX_CH-1
    */
	StuITRACK_OutArgs args;

} AlgLink_StuTrackChStatus;


/*==============================================================================
    ����: <AlgLink_TrackalgInit>
    ����: ����������ʼ��(��Ҫ��Ӧ���ṩ)
    ����: 
    ����ֵ: 
    Created By ��� 2013.04.11 11:12:51 For Web
==============================================================================*/
static inline void AlgLink_StuTrackalgInit(AlgLink_StuTrackCreateParams * pChPrm)
{

	Int32 chid = 0;
	StuITRACK_Params *pTrackParms = &pChPrm->StuTrackParms;
	pChPrm->chId = 0;
	memset(pTrackParms, 0x0, sizeof(StuITRACK_Params));
	pTrackParms->size = 0;
	pTrackParms->static_param.size = 0;
	pTrackParms->static_param.pic_width    = 160;
	pTrackParms->static_param.pic_height   = 90;
	pTrackParms->static_param.video_width  = 1280;
	pTrackParms->static_param.video_height = 720;

	pTrackParms->dynamic_param.size = 0;
	pTrackParms->dynamic_param.x_offset = 8;
	pTrackParms->dynamic_param.y_offset = 8;
	pTrackParms->dynamic_param.reset_time = 100;
	pTrackParms->dynamic_param.trigger_num = 2;
	pTrackParms->dynamic_param.control_mode = 0;
	pTrackParms->dynamic_param.sens = 20;
	pTrackParms->dynamic_param.message = 1;
	pTrackParms->dynamic_param.process_type = 0;
	pTrackParms->dynamic_param.track_point_num = 8;
	pTrackParms->dynamic_param.reset_level = RE_START;


#if 1
	pTrackParms->dynamic_param.track_point[0].x = 211;
	pTrackParms->dynamic_param.track_point[0].y = 24;
	pTrackParms->dynamic_param.track_point[1].x = 607;
	pTrackParms->dynamic_param.track_point[1].y = 25;
	pTrackParms->dynamic_param.track_point[2].x = 608;
	pTrackParms->dynamic_param.track_point[2].y = 109;
	pTrackParms->dynamic_param.track_point[3].x = 685;
	pTrackParms->dynamic_param.track_point[3].y = 110;
	pTrackParms->dynamic_param.track_point[4].x = 685;
	pTrackParms->dynamic_param.track_point[4].y = 286;
	pTrackParms->dynamic_param.track_point[5].x = 111;
	pTrackParms->dynamic_param.track_point[5].y = 288;
	pTrackParms->dynamic_param.track_point[6].x = 111;
	pTrackParms->dynamic_param.track_point[6].y = 107;
	pTrackParms->dynamic_param.track_point[7].x = 211;
	pTrackParms->dynamic_param.track_point[7].y = 107;

	memset(pTrackParms->dynamic_param.shield_info, 0x0, sizeof(student_trigger_info_t)*STUDENTS_SHIELD_NUM);
	memset(pTrackParms->dynamic_param.trigger_info, 0x0, sizeof(student_trigger_info_t)*STUDENTS_TRIGGER_NUM);
	memset(pTrackParms->dynamic_param.multitarget_info, 0x0, sizeof(student_trigger_info_t)*STUDENTS_MULTITARGET_NUM);
#endif

	#if 1
	pTrackParms->dynamic_param.trigger_info[0].x = 605;
	pTrackParms->dynamic_param.trigger_info[0].y = 126;
	pTrackParms->dynamic_param.trigger_info[0].width  = 34;
	pTrackParms->dynamic_param.trigger_info[0].height = 31;

	pTrackParms->dynamic_param.trigger_info[1].x = 523;
	pTrackParms->dynamic_param.trigger_info[1].y = 124;
	pTrackParms->dynamic_param.trigger_info[1].width  = 34;
	pTrackParms->dynamic_param.trigger_info[1].height = 29;

	pTrackParms->dynamic_param.trigger_info[2].x = 430;
	pTrackParms->dynamic_param.trigger_info[2].y = 124;
	pTrackParms->dynamic_param.trigger_info[2].width  = 35;
	pTrackParms->dynamic_param.trigger_info[2].height = 30;

	pTrackParms->dynamic_param.trigger_info[3].x = 326;
	pTrackParms->dynamic_param.trigger_info[3].y = 125;
	pTrackParms->dynamic_param.trigger_info[3].width  = 34;
	pTrackParms->dynamic_param.trigger_info[3].height = 29;

	pTrackParms->dynamic_param.trigger_info[4].x = 243;
	pTrackParms->dynamic_param.trigger_info[4].y = 125;
	pTrackParms->dynamic_param.trigger_info[4].width  = 34;
	pTrackParms->dynamic_param.trigger_info[4].height = 29;

	pTrackParms->dynamic_param.trigger_info[5].x = 154;
	pTrackParms->dynamic_param.trigger_info[5].y = 126;
	pTrackParms->dynamic_param.trigger_info[5].width  = 34;
	pTrackParms->dynamic_param.trigger_info[5].height = 30;

	pTrackParms->dynamic_param.trigger_info[7].x = 566;
	pTrackParms->dynamic_param.trigger_info[7].y = 79;
	pTrackParms->dynamic_param.trigger_info[7].width  = 32;
	pTrackParms->dynamic_param.trigger_info[7].height = 30;

	pTrackParms->dynamic_param.trigger_info[8].x = 495;
	pTrackParms->dynamic_param.trigger_info[8].y = 76;
	pTrackParms->dynamic_param.trigger_info[8].width  = 33;
	pTrackParms->dynamic_param.trigger_info[8].height = 29;

	pTrackParms->dynamic_param.trigger_info[9].x = 420;
	pTrackParms->dynamic_param.trigger_info[9].y = 75;
	pTrackParms->dynamic_param.trigger_info[9].width  = 33;
	pTrackParms->dynamic_param.trigger_info[9].height = 28;

	pTrackParms->dynamic_param.trigger_info[10].x = 352;
	pTrackParms->dynamic_param.trigger_info[10].y = 75;
	pTrackParms->dynamic_param.trigger_info[10].width  = 32;
	pTrackParms->dynamic_param.trigger_info[10].height = 29;

	#endif

	for(chid = 0;chid < ALG_LINK_STUTRACK_MAX_TO_CH; chid++)
	{
		pChPrm->ChToStream[chid] = -1;
	}
}


#endif

/*@}*/

