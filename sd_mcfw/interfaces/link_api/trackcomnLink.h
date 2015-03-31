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

#ifndef _TRACk_COMN_LINK_H_
#define _TRACk_COMN_LINK_H_


#include <stdint.h>

#define ITRACK_EOK      (0)
#define ITRACK_EFAIL    (-1)
#define ITRACK_ERUNTIME (-2)

/**
* @	һ��ͷ��ֵ�ģ��ģ��������ֵ
*/
#define MODEL_NUM_MAX	(50)


/**
* @	ģ���������ѡ����������ֵ
*/
#define MODEL_DISTANCE_CHOSE_NUM	(2)


/**
* @	��������ͷ������ֽ���
*/
#define CTL_DATA_LEN			(16)

/**
* @ ȡ��ĽǶȵĸ�������90�ȣ�180�ȵȵ�ȡ��
*/
#define	POINT_VIEW_NUM			(4)

/**
* @	������������õ�������
*/
#define	TRACK_AREA_POINT_MAX	(10)


#define ITRACK_SETPARAMS 0       /* only cmd we support */

#define		TRIGGER_MAX_NUM				(100)		//һ����������ڰ��������ڷֵĶ���
#define		TRIGGER_POINT_NUM		(30)		//һ��ˢ���������ܹ���⵽�Ĵ�������
#define		DETECTION_SECTION_NUM	(50)		//����֡Ϊһ������


#define     TEACH_DETECTION_TARGET   		(100)        //���ٸ�Ŀ��
#define		TEACH_DETECTION_FRAMES_SECTION	(20)	  		   //ÿ�ζ���֡
#define		TEACH_DETECTION_SECTION_NUM		(400)


typedef float float_t;

/**
* @	���ص�����
*/
typedef struct _pixel_point_info
{	
	UInt16	x;		//x����
	UInt16	y;		//y����
}vertex_t;

/**
* @	��������Ϣ
*/
typedef struct _out_trigger_point_info
{	
	int16_t	x0;		//x����(���϶���)
	int16_t	y0;		//y����(���϶���)
	int16_t	x1;		//x����(���϶���)
	int16_t	y1;		//y����(���϶���)
}out_trigger_point_info_t;

/**
* @	����controlʱ�Ƿ���Ҫ���³�ʼ����������
*/
typedef enum _control_init_type
{
	NO_INIT = 0,		//�������control
	RE_INIT,			//��Ҫ��ʼ�����ֲ���
	RE_START,			//��Ҫ��ʼ�����в���
	RE_INIT_NUM
}control_init_type_e;


/**
* @ ���������Ϣ
*/
typedef struct _trigger_point_info
{
	int		x0;
	int		y0;
	int		x1;
	int		y1;
	int		trigger_flag;		//����Ĵ�����־
}trigger_point_info_t;
/**
* @	����ͷ�ƶ��ķ���ĺ�
*/
typedef enum _camera_move_direction
{
	CAMERA_MOVE_NOT = 0,		//���ƶ�
	CAMERA_MOVE_LEFT,			//�����ƶ�		
	CAMERA_MOVE_RIGHT,			//�����ƶ�
	CAMERA_MOVE_UP,				//�����ƶ�
	CAMERA_MOVE_DOWN,			//�����ƶ�
	CAMERA_MOVE_STOP,			//ֹͣ�ƶ�
	CAMERA_MOVE_NUM
}camera_move_direction_e;

/**
* @	��������ͷ����������	
*/
typedef enum _cam_ctrl_type
{
	CAM_CTRL_NOT = 0,		//����������ͷ����
	CAM_CTRL_POSITION,		//��ת��Ԥ��λ����
	CAM_CTRL_ROTATION,		//��������ͷת����������
	CAM_CTRL_STOP,			//��������ͷֹͣת������
	CAM_CTRL_ZOOM,			//��������ͷ����仯
	CAM_CTRL_NUM
}cam_ctrl_type_e;


/**
* @ ����λ��
*/
typedef enum _trigger_position
{
	NOT_TRIGGER = 0,		//������
	TRIGGER_POSITION1,		//����ȫ��
	TRIGGER_POSITION2,		//����Ԥ��λ2
	TRIGGER_POSITION3,		//����Ԥ��λ3
	TRIGGER_POSITION4,		//����Ԥ��λ4
	TRIGGER_POSITION5,		//����Ԥ��λ5
	TRIGGER_POSITION6,		//����Ԥ��λ6
	TRIGGER_POSITION7,		//����Ԥ��λ7
	TRIGGER_POSITION8,		//����Ԥ��λ8
	TRIGGER_POSITION9,		//����Ԥ��λ9
	TRIGGER_POSITION10,		//����Ԥ��λ10
	TRIGGER_POSITION11,		//����Ԥ��λ11
	TRIGGER_POSITION12,		//����Ԥ��λ12
	TRIGGER_POSITION13,		//����Ԥ��λ13
	TRIGGER_POSITION14,		//����Ԥ��λ14
	TRIGGER_POSITION15,		//����Ԥ��λ15
	TRIGGER_POSITION16,		//����Ԥ��λ16
	TRIGGER_POSITION17,		//����Ԥ��λ17
	TRIGGER_POSITION18,		//����Ԥ��λ18
	TRIGGER_POSITION19,		//����Ԥ��λ19
	TRIGGER_POSITION20,		//����Ԥ��λ20
	TRIGGER_POSITION21,		//����Ԥ��λ21
	TRIGGER_POSITION22,		//����Ԥ��λ22
	TRIGGER_POSITION23,		//����Ԥ��λ23
	TRIGGER_POSITION24,		//����Ԥ��λ24
	TRIGGER_POSITION25,		//����Ԥ��λ25
	TRIGGER_POSITION26,		//����Ԥ��λ26
	TRIGGER_POSITION27,		//����Ԥ��λ27
	TRIGGER_POSITION28,		//����Ԥ��λ28
	TRIGGER_POSITION29,		//����Ԥ��λ29
	TRIGGER_POSITION30,		//����Ԥ��λ30
	TRIGGER_POSITION31,		//����Ԥ��λ31
	TRIGGER_POSITION32,		//����Ԥ��λ32
	TRIGGER_POSITION33,		//����Ԥ��λ33
	TRIGGER_POSITION34,		//����Ԥ��λ34
	TRIGGER_POSITION35,		//����Ԥ��λ35
	TRIGGER_POSITION36,		//����Ԥ��λ36
	TRIGGER_POSITION37,		//����Ԥ��λ37
	TRIGGER_POSITION38,		//����Ԥ��λ38
	TRIGGER_POSITION39,		//����Ԥ��λ39
	TRIGGER_POSITION40,		//����Ԥ��λ40
	TRIGGER_POSITION41,		//����Ԥ��λ41
	TRIGGER_POSITION42		//����Ԥ��λ42	
}trigger_position_e;


/**
* @ Y,Cb,Crֵ�ṹ��
*/
typedef struct 	_yuv_value_info
{
	UInt16	y_value;		//y��ֵ
	UInt16	cb_value;		//cb��ֵ
	UInt16	cr_value;		//cr��ֵ
}yuv_value_info_t;

/**
* @ Y,Cb,Crֵ�ṹ��
*/
typedef struct 	_students_yuv_value_info
{
	UInt16	y_value;		//y��ֵ
	UInt16	cb_value;		//cb��ֵ
	UInt16	cr_value;		//cr��ֵ
}students_yuv_value_info_t;

/**
* @ ���ߵ���Ϣ
*/
typedef struct _line_info
{
	UInt32	start_x;	//�ߵ���ʼ��x����
	UInt32	start_y;	//�ߵ���ʼ��y����
	UInt32	end_x;		//�ߵĽ�����x����
	UInt32	end_y;		//�ߵĽ�����y����
}line_info_t;

typedef struct
{
  UInt16 min_x;
  UInt16 min_y;
  UInt16 max_x;
  UInt16 max_y;
} rect_t;

typedef struct TRect
{
	int nTop;
	int nBottom;
	int nLeft;
	int nRight;
	int nFillSize;
	int nMarkVal;
}T_Rect;


#define ALG_LINK_TRACK_CMD_SET_CHANNEL_WIN_PRM          (0x8001)

#define ALG_LINK_STUTRACK_CMD_SET_CHANNEL_WIN_PRM        (0x8002)

#define ALG_LINK_TRACK_CMD_PRINT_STATE        (0x8003)

#define ALG_LINK_STUSIDETRACK_CMD_SET_CHANNEL_WIN_PRM        (0x8004)

#define ALG_LINK_STUSIDETRACK_CMD_SAVE_VIEW (0x8005)

#define ALG_LINK_TRACK_CMD_SAVE_VIEW (0x8006)


#define ALG_LINK_STUDOUBLESIDETRACK_CMD_SET_CHANNEL_WIN_PRM        (0x8007)

#define ALG_LINK_STUDOUBLESIDETRACK_CMD_SAVE_VIEW (0x8008)

#define ALG_LINK_CEIL_TRACK_CMD_SET_CHANNEL_WIN_PRM          (0x8009)

#define ALG_LINK_CEIL_TRACK_CMD_SAVE_VIEW (0x800a)

#define ALG_LINK_BLACKBOARD_CMD_SET_CHANNEL_WIN_PRM (0x800b)
#endif

/*@}*/

