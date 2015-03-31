/**
*
*/

#include <stdlib.h>
#include <stdio.h>
//#include <csl_tsc.h>
#include <track_students.h>



/**
* @ ���ƻ��ߵ�����
*/
typedef enum _stu_draw_line_type
{
	DRAW_NO = 0,			//������
	DRAW_TRIGGER_TRACK,		//��ʾ�����ٿ�ʹ�����,�޸���,�͸��������
	DRAW_SLANT_LINE,		//��б�߱�ʾ��45��,135��,90��,180����
	DRAW_MODEL,				//��ģ��
	DRAW_SKIN_COLOR,		//����������ͷ��ķ�ɫ����
	DRAW_TRACK_TRAJECTORY,	//�����ٹ켣��
	DRAW_TRACK_DIRECT,		//�����ٷ�����
	DRAW_TRACK_SPEED,		//�������ٶ���
	MAX_NUM_DRAW
}stu_draw_line_type_e;

#define	INTER_PROG_RATIO	(1)		//1//1��ʾ����,2��ʾ����
#define YUV422_OR_YUV420	(2)		//1//1��ʾ422,2��ʾ420


#define min(a,b) (((a)>(b))?(b):(a))
#define max(a,b) (((a)<(b))?(b):(a))
#define MAXVIDEOWIDTH 1920
#define MAXVIDEOHEIGHT 1088
#define STU_FIND_MV_DELAY 60//���ڼ�⵽�ƶ���60֡�ڲ��ټ��
#define STU_MVDET_DELAY 600//ѧ��������600֡�������ƶ����

//CSL_Uint64 counterVal;
//CSL_Uint64 counterVal_end=0;

/* p, q is on the same of line l */
static int is_same(const vertex_t *l_start, const vertex_t *l_end,
                   const vertex_t *p,
                   const vertex_t *q)
				  
{

	float_t dx = l_end->x - l_start->x;  	//���εı�
	float_t dy = l_end->y - l_start->y;

	float_t dx1= p->x - l_start->x;    //p(������)
	float_t dy1= p->y - l_start->y;

	float_t dx2= q->x - l_end->x;      //q(�����������յ�)
	float_t dy2= q->y - l_end->y; 

	if ((dx * dy1-dy * dx1) * (dx * dy2-dy * dx2) > 0)
	{
		return 0;
	}
	else
	{
		return 1;
	}
} 

/* 2 line segments (s1, s2) are intersect? */ 
static int is_intersect(const vertex_t *s1_start, const vertex_t *s1_end,  
                        const vertex_t *s2_start, const vertex_t *s2_end) 
{ 
     int i,j;
     i = is_same(s1_start, s1_end, s2_start, s2_end);
     j = is_same(s2_start, s2_end, s1_start, s1_end);
	 if ((i&&j) == 0)
	    return 0;
     else
	    return 1;
    
} 

/**
* @ �жϵ��Ƿ�������������ڣ����������������ڣ��������һ������
*/
static void vertices_get_extent(const vertex_t *vl, int np, /* in vertices */
                          rect_t *rc /* out extent*/ )
{
     short int i;
   	 i = vl[0].x;
     if (np > 0)
        {
         rc->min_x = rc->max_x = vl[0].x;
         rc->min_y = rc->max_y = vl[0].y;
        }
     
     else
        {
         rc->min_x = rc->min_y = rc->max_x = rc->max_y = 0; // =0 ? no vertices at all 
        }

     for(i=1; i<np; i++)
     {
         if(vl[i].x < rc->min_x) 
           rc->min_x = vl[i].x;
         if(vl[i].y < rc->min_y)
           rc->min_y = vl[i].y;
         if(vl[i].x > rc->max_x)
           rc->max_x = vl[i].x;
         if(vl[i].y > rc->max_y)
           rc->max_y = vl[i].y;
     }
}


/**
* @	������0����1����0��ʾ�ڼ�������⣬��1��ʾ�ڼ��������
*/
static int32_t pt_in_poly( const vertex_t *vl, int np,  /* polygon vl with np vertices  */ 
                 const vertex_t *v) 
{ 
	int i, j, k1,k2, c; 
	rect_t rc;  
	vertex_t w; 
	if (np < 3) 
	{
		return 0;  
	}
      
	vertices_get_extent(vl, np, &rc); 
	
	if (v->x < rc.min_x || v->x > rc.max_x || v->y < rc.min_y || v->y > rc.max_y) 
	{
		return 0; 
	}

	/* Set a horizontal beam l(*v, w) from v to the ultra right */    
	w.x = rc.max_x + 1; 
	w.y = v->y; 
	c = 0;        /* Intersection points counter */ 
	
	for(i=0; i<np; i++) 
	{ 
		j = (i+1) % np; 

		if (vl[i].y != vl[j].y)
		{
			if (is_intersect(vl+i, vl+j, v, &w)==1) 
			{
				c++; 
			}
		} 

		if(vl[i].y == w.y) 
		{ 
			if (vl[i].x >= v->x)
			{
				k1 =(np+i-1)%np;
				k2 =(np+i+1)%np;
				if (vl[k1].y > w.y)
				{
					if (vl[k2].y < w.y)
					{
						c++;
					}
				}

				if (vl[k2].y > w.y)
				{
					if (vl[k1].y < w.y)
					{
						c++;
					}
				}


			}
		} 
	} 

	return   c%2; 
} 
   

/**
* @ ��������: students_area_check()
* @	��������: �趨��������
* @ �������: input_param -- Ӧ�ò����õĲ���
* @ �������: ��
* @ ����ֵ:   ��
*/
static int32_t students_area_check(StuTrackHand *inst)
{
	StuITRACK_Params *input_param=&(inst->input_param);
    int32_t	 	k				= 0;	
	int16_t		video_width		= input_param->static_param.video_width;
	int16_t		video_height	= input_param->static_param.video_height;

	int32_t		m				= 0;
	int32_t		n				= 0;
	vertex_t	point_y_value	= {0};
	uint8_t *Students_Area_vertex=inst->t_stu_global_obj.Students_Area_vertex; 

	for (k = 0; k < video_width * video_height; k++)  
	{
		m = k%video_width;     //��
		n = k/video_width;    //��

		point_y_value.x = m;
		point_y_value.y = n;

		if (pt_in_poly ((const vertex_t *)input_param->dynamic_param.track_point, input_param->dynamic_param.track_point_num,  
			&point_y_value)==1)
		{
		#if 0
			if (n%2 == 0)
			{
				*((uint8_t *)Students_Area_vertex + n/2 * video_width + m) = 255;
			}
			else
			{
				*((uint8_t *)Students_Area_vertex + (n/2 + video_height/2)  * video_width + m) = 255;
			}
		#endif	
			*((uint8_t *)Students_Area_vertex + n * video_width + m) = 255;
		}
		else
		{
		#if 0
			if (n%2 == 0)
			{
				*((uint8_t *)Students_Area_vertex + n/2 * video_width + m) = 0;
			}
			else
			{
				*((uint8_t *)Students_Area_vertex + (n/2 + video_height/2) * video_width + m) = 0;
			}
		#endif	
			*((uint8_t *)Students_Area_vertex + n * video_width + m) = 0;
		}
	}
	return 0;
}


/**
* @ ��������: draw_point()
* @	��������: ����Ĺ���
* @	�������: buffer -- ��ͼ��buffer
* @			  yuv_value -- ���ߵ�Y��Cb��Cr����ɫֵ
* @			  line_info -- ���ߵ���ʼ������
* @ 		  static_param -- �ϲ㴫�����ľ�̬����
* @ �������: ��
* @ ����ֵ:   ��
*/
static int32_t	draw_point(StuTrackHand *inst,int8_t *buffer, students_yuv_value_info_t *yuv_value,
	int32_t	point_x, int32_t point_y)
{ 
	int nSrcBufWidth=inst->t_stu_global_obj.nSrcBufWidth;
	int nSrcBufHeight=inst->t_stu_global_obj.nSrcBufHeight;
	if((point_y % 2) == 0)
	{
		*(int8_t *)(buffer + point_y*nSrcBufWidth + point_x) = yuv_value->y_value;
		*(int8_t *)(buffer + (point_y + 1)*nSrcBufWidth + point_x) = yuv_value->y_value;
	}
	else
	{
		*(int8_t *)(buffer + (point_y)*nSrcBufWidth + point_x) = yuv_value->y_value;
		*(int8_t *)(buffer + (point_y - 1)*nSrcBufWidth + point_x) = yuv_value->y_value;

	}

	if((point_x % 2) == 0)
	{
		*(int8_t *)(buffer + (nSrcBufWidth* nSrcBufHeight) + (point_y/2)*nSrcBufWidth + point_x) = yuv_value->cb_value;
		*(int8_t *)(buffer + (nSrcBufWidth* nSrcBufHeight) + (point_y/2)*nSrcBufWidth + point_x+1) = yuv_value->cr_value;
	}
	else
	{
		*(int8_t *)(buffer + (nSrcBufWidth* nSrcBufHeight) + (point_y/2)*nSrcBufWidth + point_x - 1) = yuv_value->cb_value;
		*(int8_t *)(buffer + (nSrcBufWidth* nSrcBufHeight) + (point_y/2)*nSrcBufWidth + point_x) = yuv_value->cr_value;
	}
	return 0;
}

/**
* @ ��������: draw_line()
* @	��������: ����ֱ�ߵĹ���
* @	�������: buffer -- ��ͼ��buffer
* @			  yuv_value -- ���ߵ�Y��Cb��Cr����ɫֵ
* @			  line_info -- ���ߵ���ʼ������
* @ 		  static_param -- �ϲ㴫�����ľ�̬����
* @ �������: ��
* @ ����ֵ:   ��
*/
static void	draw_line(StuTrackHand *inst,int8_t *buffer,students_yuv_value_info_t *yuv_value,
	line_info_t *line_info)
{
	int32_t dx =line_info->end_x - line_info->start_x;
	int32_t	dy =line_info->end_y - line_info->start_y;
	int32_t	steps 	= 0;
	int32_t	k		= 0;
	int32_t tx,ty;
	float_t xIn, yIn; 
	
	float_t	x = line_info->start_x; 
	float_t	y = line_info->start_y;
	
    if (abs(dx) > abs(dy))
	{
		steps = abs(dx);
	}
    else
	{
		steps = abs(dy);
	}
       
    xIn = (float_t)dx / (float_t)steps;
    yIn = (float_t)dy / (float_t)steps;
 
 
    draw_point(inst,buffer,yuv_value,x,y);
    for(k=0;k<steps;k++)
	{
		x+=xIn;
		y+=yIn;
		tx = x + 0.5;
		ty = y + 0.5;
		draw_point(inst,buffer,yuv_value,tx,ty);
	}
}

/**
* @	��������: draw_rectangle()
* @	��������: �����ο��ڴ���Ҫ�ǻ��㣬�����ڴ���ʱ���õ����Ź����ͼ�񣬶����ߵ�
* @ 		  buffer������Դͼ���ϴ���ģ�����Ҫ�ѵ㻭��һ��С��
* @	�������: buffer -- ��ͼ��buffer
* @			  yuv_value -- ���ߵ�Y��Cb��Cr����ɫֵ
* @			  line_info -- ���ߵ���ʼ������
* @ 		  static_param -- �ϲ㴫�����ľ�̬����
* @ �������: ��
* @ ����ֵ:	  ��
*/
static void draw_rectangle(StuTrackHand *inst,int8_t *buffer,students_yuv_value_info_t *yuv_value,
	line_info_t *line_info)
{
    int32_t i,j;
	int32_t left_up_x 		= line_info->start_x;
	int32_t left_up_y 		= line_info->start_y;
	int32_t right_down_x 	= line_info->end_x;
	int32_t right_down_y 	= line_info->end_y;

    
	//������
    for(i=left_up_y;i<left_up_y+1;i++)  
	{
	    for(j=left_up_x;j<right_down_x;j++) 
	    {
	    	draw_point(inst,buffer,yuv_value,j,i);
	    }
	}	
	//������
    for(i=right_down_y;i<right_down_y+1;i++)
	{
	    for(j=left_up_x;j<right_down_x;j++) 
	    {
	    	draw_point(inst,buffer,yuv_value,j,i);
	    }
	}
	
	//�����
    for(i=left_up_y;i<right_down_y;i++)
	{
	    for(j=left_up_x;j<left_up_x+1;j++) 
	    {
	    	draw_point(inst,buffer,yuv_value,j,i);
	    }
	}
	
	//���ұ�
    for(i=left_up_y;i<right_down_y+1;i++)
	{
	    for(j=right_down_x;j<right_down_x+1;j++) 
	    {
	    	draw_point(inst,buffer,yuv_value,j,i);
	    }
	}

} 

static void draw_trigger_rect(StuTrackHand *inst,int8_t *buffer)
{
	StuITRACK_Params *input_param=&(inst->input_param);
	line_info_t 		line_info	= {0};
	students_yuv_value_info_t 	yuv_value	= {0};
	int32_t				index		= 0;
	int32_t				m 			= 0;
	int32_t				n 			= 0;
	

	for(index = 0; index < STUDENTS_TRIGGER_NUM; index++)
	{
		m = input_param->dynamic_param.trigger_info[index].x;
		n = input_param->dynamic_param.trigger_info[index].y;

		if((0 != m) || (0 != n))
		{
			line_info.start_x = m;
			line_info.start_y = n;
			line_info.end_x = m + input_param->dynamic_param.trigger_info[index].width;
			line_info.end_y = n + input_param->dynamic_param.trigger_info[index].height;

			yuv_value.y_value = 255;
			yuv_value.cb_value	= 0;
			yuv_value.cr_value	= 255;

			draw_rectangle(inst,buffer, &yuv_value, &line_info);
		}
	}
}
/**
* @	��������: draw_rectangle()
* @	��������: �����ο��ڴ���Ҫ�ǻ��㣬�����ڴ���ʱ���õ����Ź����ͼ�񣬶����ߵ�
* @ 		  buffer������Դͼ���ϴ���ģ�����Ҫ�ѵ㻭��һ��С��
* @	�������: buffer -- ��ͼ��buffer
* @			  yuv_value -- ���ߵ�Y��Cb��Cr����ɫֵ
* @			  line_info -- ���ߵ���ʼ������
* @ 		  static_param -- �ϲ㴫�����ľ�̬����
* @ �������: ��
* @ ����ֵ:	  ��
*/
static void fill_rectangle(StuTrackHand *inst,int8_t *buffer,students_yuv_value_info_t *yuv_value,
	line_info_t *line_info)
{
	int nSrcBufWidth=inst->t_stu_global_obj.nSrcBufWidth;
	int nSrcBufHeight=inst->t_stu_global_obj.nSrcBufHeight;
    int32_t i,j;
	int32_t left_up_x 		= line_info->start_x;
	int32_t left_up_y 		= line_info->start_y;
	int32_t right_down_x 	= line_info->end_x;
	int32_t right_down_y 	= line_info->end_y;

    for(i=left_up_y;i<right_down_y;i++)  
	{
	    for(j=left_up_x;j<right_down_x;j++) 
	    {
	    	buffer[i*nSrcBufWidth+j]=yuv_value->y_value;
	    	buffer[nSrcBufWidth*nSrcBufHeight+i/2*nSrcBufWidth+(j/2)*2]=yuv_value->cb_value;
	    	buffer[nSrcBufWidth*nSrcBufHeight+i/2*nSrcBufWidth+(j/2)*2+1]=yuv_value->cr_value;
	    }
	}	
} 

//�����ο�
static void draw_shield_rect(StuTrackHand *inst,int8_t *buffer)
{
	StuITRACK_Params *input_param=&(inst->input_param);
	line_info_t 		line_info	= {0};
	students_yuv_value_info_t 	yuv_value	= {0};
	int32_t				index		= 0;
	int32_t				m 			= 0;
	int32_t				n 			= 0;
	

	for(index = 0; index < STUDENTS_SHIELD_NUM; index++)
	{
		m = input_param->dynamic_param.shield_info[index].x;
		n = input_param->dynamic_param.shield_info[index].y;

		if((0 != m) || (0 != n))
		{
			line_info.start_x = m;
			line_info.start_y = n;
			line_info.end_x = m + input_param->dynamic_param.shield_info[index].width;
			line_info.end_y = n + input_param->dynamic_param.shield_info[index].height;

			yuv_value.y_value = 100;
			yuv_value.cb_value	= 0;
			yuv_value.cr_value	= 100;

			draw_rectangle(inst,buffer, &yuv_value, &line_info);
		}
	}
}


//����Ŀ����̨����
static void draw_multitarget_rect(StuTrackHand *inst,int8_t *buffer)
{
	StuITRACK_Params *input_param=&(inst->input_param);
	line_info_t 		line_info	= {0};
	students_yuv_value_info_t 	yuv_value	= {0};
	int32_t				index		= 0;
	int32_t				m 			= 0;
	int32_t				n 			= 0;
	

	for(index = 0; index < STUDENTS_MULTITARGET_NUM; index++)
	{
		m = input_param->dynamic_param.multitarget_info[index].x;
		n = input_param->dynamic_param.multitarget_info[index].y;

		if((0 != m) || (0 != n))
		{
			line_info.start_x = m;
			line_info.start_y = n;
			line_info.end_x = m + input_param->dynamic_param.multitarget_info[index].width;
			line_info.end_y = n + input_param->dynamic_param.multitarget_info[index].height;

			yuv_value.y_value = 100;
			yuv_value.cb_value	= 100;
			yuv_value.cr_value	= 0;

			draw_rectangle(inst,buffer, &yuv_value, &line_info);
		}
	}
}


/**
* @	��������: draw_track_rect()
* @ ��������: �����������ʹ�����
* @	�������: buffer -- ��ͼ��buffer
* @			  yuv_value -- ���ߵ�Y��Cb��Cr����ɫֵ
* @			  line_info -- ���ߵ���ʼ������
* @ 		  input_param -- �ϲ㴫�������������
* @	�������: ��
* @ ����ֵ:	  ��
*/
static void draw_track_rect(StuTrackHand *inst,int8_t *buffer)
{
	StuITRACK_Params *input_param=&(inst->input_param);
	line_info_t 		line_info	= {0};
	students_yuv_value_info_t 	yuv_value	= {0};
	int16_t				index		= 0;
	int16_t				j			= 0;
	int16_t				point_num	= input_param->dynamic_param.track_point_num;

	yuv_value.y_value = 255;
	yuv_value.cb_value = 0;
	yuv_value.cr_value = 255;

	//�����ٿ�
    for(index=0; index<point_num; index++) 
	{ 
		j = (index+1) % point_num; 

		line_info.start_x 	= input_param->dynamic_param.track_point[index].x;
		line_info.start_y 	= input_param->dynamic_param.track_point[index].y/INTER_PROG_RATIO;
		line_info.end_x 	= input_param->dynamic_param.track_point[j].x;
		line_info.end_y 	= input_param->dynamic_param.track_point[j].y/INTER_PROG_RATIO;
		draw_line(inst,buffer,&yuv_value,&line_info);
	}

	//����������
	draw_trigger_rect(inst,buffer);

	//��������
	draw_shield_rect(inst,buffer);

	//����Ŀ����̨�����
	draw_multitarget_rect(inst,buffer);

}

/**
* @ ��������: 	check_trigger_number()
* @ ��������: 	����ÿ�����ص������Ĵ�������
* @ �������: 	input_param -- �㷨���������
* @				m -- x����
* @				n -- y����
* @ �������: 	��
* @	����ֵ:   	(x,y)��Ӧ�Ĵ�������,����-1˵��û���ҵ�
*/
static int32_t	check_trigger_number(StuTrackHand *inst, int16_t m, int16_t	n)
{
	StuITRACK_Params *input_param=&(inst->input_param);
	int16_t			i 				= 0;
	int16_t			trigger_number	= input_param->dynamic_param.trigger_num;
	int16_t			x_offset		= input_param->dynamic_param.x_offset;
	int16_t			y_offset		= input_param->dynamic_param.y_offset;
	
	for(i = 0; i < trigger_number; i++)
	{
		if(m >= input_param->dynamic_param.trigger_info[i].x/x_offset
			&& (m <= (input_param->dynamic_param.trigger_info[i].x/x_offset + input_param->dynamic_param.trigger_info[i].width/x_offset))
			&& (n >= input_param->dynamic_param.trigger_info[i].y/y_offset)
			&& (n <= (input_param->dynamic_param.trigger_info[i].y/y_offset + input_param->dynamic_param.trigger_info[i].height/y_offset)))
		return i;
	}
	return 0;
}

/**
* @ ��������: 	nearest_point()
* @ ��������: 	�������ص����Ǹ����ٿ����
* @ �������: 	input_param -- �㷨���������
* @ �������: 	��
* @	����ֵ:   	
*/
static int32_t	nearest_point(StuTrackHand *inst, int32_t m, int32_t n)  
{ 
	StuITRACK_Params *input_param=&(inst->input_param);
	int32_t i,temp,nFlag=0;

	int16_t			x_offset		= input_param->dynamic_param.x_offset;
	int16_t			y_offset		= input_param->dynamic_param.y_offset;
	int32_t			value			= 0;
	int16_t			width			= 0;
	int16_t			height			= 0;

	int16_t *k_Nearest= inst->t_stu_global_obj.k_Nearest;		//����ĵ�

	temp = input_param->static_param.pic_width * input_param->static_param.pic_width
			+ input_param->static_param.pic_height * input_param->static_param.pic_height;
	k_Nearest[0] = 0; 
	for (i = 0;i < STUDENTS_TRIGGER_NUM;i++)            
	{
		if( (input_param->dynamic_param.trigger_info[i].width==0)||(input_param->dynamic_param.trigger_info[i].height==0))
		{
			continue;
		}
		if(input_param->dynamic_param.trigger_info[i].y<n*y_offset)
		{
			continue;
		}
		width = abs(m-(input_param->dynamic_param.trigger_info[i].x + input_param->dynamic_param.trigger_info[i].width/2)/x_offset);
		height = abs(n-(input_param->dynamic_param.trigger_info[i].y + input_param->dynamic_param.trigger_info[i].height/2)/y_offset);
		value = width * width + height *height;
	
		if (temp > value)
		{
			temp = value;
			k_Nearest[0] = i;
			nFlag=1;
		}
	}
    if(nFlag==0)
    {
		for (i = 0;i < STUDENTS_TRIGGER_NUM;i++)			
		{
			if( (input_param->dynamic_param.trigger_info[i].width==0)||(input_param->dynamic_param.trigger_info[i].height==0))
			{
				continue;
			}
			if(input_param->dynamic_param.trigger_info[i].y>=n*y_offset)
			{
				continue;
			}
			width = abs(m-(input_param->dynamic_param.trigger_info[i].x + input_param->dynamic_param.trigger_info[i].width/2)/x_offset);
			height = abs(n-(input_param->dynamic_param.trigger_info[i].y + input_param->dynamic_param.trigger_info[i].height/2)/y_offset);
			value = width * width + height *height;
		
			if (temp > value)
			{
				temp = value;
				k_Nearest[0] = i;
			}
		}
	}
	temp = input_param->static_param.pic_width * input_param->static_param.pic_width
			+ input_param->static_param.pic_height * input_param->static_param.pic_height;
	k_Nearest[1] = 0; 
	for (i = 0;i < STUDENTS_TRIGGER_NUM;i++)            
	{
		if (i == k_Nearest[0])
			continue;
		if( (input_param->dynamic_param.trigger_info[i].width==0)||(input_param->dynamic_param.trigger_info[i].height==0))
		{
			continue;
		}

		width = abs(m-(input_param->dynamic_param.trigger_info[i].x + input_param->dynamic_param.trigger_info[i].width/2)/x_offset);
		height = abs(n-(input_param->dynamic_param.trigger_info[i].y + input_param->dynamic_param.trigger_info[i].height/2)/y_offset);
		value = width * width + height *height;

		if (temp > value)
		{
			temp = value;
			k_Nearest[1] = i;
		} 

	}
	temp = input_param->static_param.pic_width * input_param->static_param.pic_width
			+ input_param->static_param.pic_height * input_param->static_param.pic_height;
	k_Nearest[2] = 0; 
	for (i = 0;i < STUDENTS_TRIGGER_NUM;i++)            
	{
		if (i == k_Nearest[0]||i == k_Nearest[1])
			continue;

		if( (input_param->dynamic_param.trigger_info[i].width==0)||(input_param->dynamic_param.trigger_info[i].height==0))
		{
			continue;
		}
		width = abs(m-(input_param->dynamic_param.trigger_info[i].x + input_param->dynamic_param.trigger_info[i].width/2)/x_offset);
		height = abs(n-(input_param->dynamic_param.trigger_info[i].y + input_param->dynamic_param.trigger_info[i].height/2)/y_offset);

		value = width * width + height *height;

		if (temp > value)
		{
			temp = value;
			k_Nearest[2] = i;
		} 
	}
	return k_Nearest[0];
}
/**
* @	��������: 	data_copy()
* @ ��������: 	��ͼ��������ţ���ȥ����
* @	�������: 	input_param -- �ⲿ���õ��������
* @	�������: 	��
* @ ����ֵ:	  	��
*/
static int32_t data_copy(StuTrackHand *inst, int8_t *output_buf)
{
	StuITRACK_Params *input_param=&(inst->input_param);
	int16_t			pic_width		= 0;
	int16_t			pic_width_4		= 0;
	int16_t			pic_height		= 0;
	int16_t			video_width		= input_param->static_param.video_width;
	int16_t			video_height	= input_param->static_param.video_height;
	int16_t			x_offset		= input_param->dynamic_param.x_offset;
	int16_t			y_offset		= input_param->dynamic_param.y_offset;
	int32_t			i 				= 0;
	int32_t			j 				= 0;
	int32_t			nSrcShift 				= 0;	
	int32_t			nDstShift 				= 0;

	uint32_t *			restrict pDst;
	uint8_t *			restrict pSrc;
	uint8_t *			restrict pDst_other;

	uint32_t			nVal0,nVal1;
	uint32_t			nVal2,nVal3;

	uint32_t			nDstVal;
	int k=0;
	uint8_t **students_dst_y_buf = inst->t_stu_global_obj.students_dst_y_buf;
	int nSrcBufWidth=inst->t_stu_global_obj.nSrcBufWidth;
	int nSrcBufHeight=inst->t_stu_global_obj.nSrcBufHeight;

	pic_width = video_width / x_offset;
	pic_height = video_height / y_offset;

	pic_width_4 = pic_width>>2;
#if 1

//����ͼ��Y����
for(i = 0; i < pic_height; i++)
	{
		for(j = 0; j < pic_width; j++)
		{

			*(students_dst_y_buf[inst->t_stu_global_obj.nCopyFramNum]+k) = *(output_buf + i*y_offset*nSrcBufWidth + j*x_offset);

			k++;
		}
	}

#else
	if(x_offset==4)
	{
		for(i = 0; i < pic_height; i++)
		{
			pSrc=output_buf+i*y_offset*nSrcBufWidth ;
			pDst=(uint32_t *)(students_dst_y_buf[nCopyFramNum]+i*pic_width);
			nSrcShift = 0;	
			nDstShift 	= 0;

			for(j = 0; j < pic_width_4; j++)
			{
				nVal0=_lo(_memd8(pSrc+nSrcShift));
				nVal1=_hi(_memd8(pSrc+nSrcShift));
				nVal2=_lo(_memd8(pSrc+nSrcShift+8));
				nVal3=_hi(_memd8(pSrc+nSrcShift+8));

				nDstVal=_packl4(_packl4(nVal3,nVal2),_packl4(nVal1,nVal0));
				//_mem4(pDst+ nDstShift) = nDstVal;
				*(pDst+ nDstShift) = nDstVal;

				nSrcShift+=16;
				nDstShift+=1;
				
			}
		}
	}
	else	if(x_offset==2)
	{
		for(i = 0; i < pic_height; i++)
		{			
			pSrc=output_buf+i*y_offset*nSrcBufWidth ;
			pDst=(uint32_t *)(students_dst_y_buf[nCopyFramNum]+i*pic_width);
			nSrcShift = 0;	
			nDstShift 	= 0;

			for(j = 0; j < pic_width_4; j++)
			{
				nVal0=_lo(_memd8(pSrc+nSrcShift));
				nVal1=_hi(_memd8(pSrc+nSrcShift));
				nDstVal=_packl4(nVal1,nVal0);
				//_mem4(pDst+ nDstShift) = nDstVal;
				*(pDst+ nDstShift) = nDstVal;
				nSrcShift+=8;
				nDstShift+=1;
			}
		}
	}
	else	if(x_offset==1)
	{
		for(i = 0; i < pic_height; i++)
		{
			pSrc=output_buf+i*y_offset*nSrcBufWidth ;
			pDst=(uint32_t *)(students_dst_y_buf[nCopyFramNum]+i*pic_width);
			nSrcShift = 0;	
			nDstShift 	= 0;

			for(j = 0; j < pic_width_4; j++)
			{
				nVal0=_mem4(pSrc+nSrcShift);
				
				//_memd8(pDst+ nDstShift) = _itod(nVal1,nVal0);
				*(pDst+ nDstShift) = nVal0;
				nSrcShift+=4;
				nDstShift+=4;
				
			}
		}
	}
	else
	{
		for(i = 0; i < pic_height; i++)
		{	
			pSrc=output_buf+i*y_offset*nSrcBufWidth ;
			pDst_other=students_dst_y_buf[nCopyFramNum]+i*pic_width;
			nSrcShift = 0;	
			nDstShift 	= 0;

			for(j = 0; j < pic_width; j++)
			{
				*(pDst_other+ nDstShift) = *(pSrc + nSrcShift);	
				nSrcShift+=x_offset;
				nDstShift++;

			}
			
		}
	}
#endif
	inst->t_stu_global_obj.nCopyFramNum++;
	if(inst->t_stu_global_obj.nCopyFramNum==3)
	{
		inst->t_stu_global_obj.nCopyFramNum=0;
	}
	return 0;
}

static int32_t img_compare_y(StuTrackHand *inst,int8_t *output_buf)
{
	StuITRACK_Params *input_param=&(inst->input_param);
  	int32_t k=0,T1,T2,T3;

	int16_t			pic_width		= 0;
	int16_t			pic_height		= 0;
	int16_t			video_width		= input_param->static_param.video_width;
	int16_t			video_height	= input_param->static_param.video_height;

	int16_t			x_offset		= input_param->dynamic_param.x_offset;
	int16_t			y_offset		= input_param->dynamic_param.y_offset;

	int16_t			sens			= input_param->dynamic_param.sens;
	
	int16_t			m 				= 0;
	int16_t			n 				= 0;
	int16_t			m_shift =20/input_param->dynamic_param.x_offset;
	int16_t			n_shift =20/input_param->dynamic_param.y_offset;
 	uint8_t *pTemp;
	uint8_t **students_dst_count=inst->t_stu_global_obj.students_dst_count;//ʵ�ʲ����ڴ�ָ�룬��students_time_count���ʹ�ã�Ϊ�˼����ڴ濽��
	uint8_t **students_dst_y_buf=inst->t_stu_global_obj.students_dst_y_buf;		 //�����һ֡Ybuffer����
	uint8_t *Students_Area_vertex=inst->t_stu_global_obj.Students_Area_vertex;
 	pic_width = video_width / x_offset;
	pic_height = video_height / y_offset;

	
  	pTemp=students_dst_count[8];
	students_dst_count[8]=students_dst_count[7];
	students_dst_count[7]=students_dst_count[6];
	students_dst_count[6]=students_dst_count[5];
	students_dst_count[5]=students_dst_count[4];
	students_dst_count[4]=students_dst_count[3];
	students_dst_count[3]=students_dst_count[2];
	students_dst_count[2]=students_dst_count[1];
	students_dst_count[1]=students_dst_count[0];
	students_dst_count[0]=pTemp;
	for(n = n_shift; n < pic_height-n_shift; n++)
	{
		for(m = m_shift; m < pic_width-m_shift; m++)
		{
			k=n*pic_width+m;
			if (*((uint8_t *)Students_Area_vertex + (n * y_offset * video_width)+ m * x_offset) == 0)
			{
				*((uint8_t *)students_dst_count[0] + k) = 0;
			}
			else
			{
				T1 = abs(*((uint8_t *)students_dst_y_buf[0] + k) - *((uint8_t *)students_dst_y_buf[1] + k));
				T2 = abs(*((uint8_t *)students_dst_y_buf[1] + k) - *((uint8_t *)students_dst_y_buf[2] + k));
				T3 = abs(*((uint8_t *)students_dst_y_buf[0] + k) - *((uint8_t *)students_dst_y_buf[2] + k));		
				if (T1>sens||T2>sens||T3>sens)                                           
				//if (T1>sens||T3>sens)                                           
				{
					*((uint8_t *)students_dst_count[0] + k) = 1;
				} 
				else
				{
					*((uint8_t *)students_dst_count[0] + k) = 0;
				} 
			}
		}
	}

	return 0;
}


//0 ��ͨ�仯��  1 ͷ���仯�� 2 ��Ե�Ƕ����仯��
static int32_t img_compare_y0(StuTrackHand *inst,int8_t *output_buf)
{
	StuITRACK_Params *input_param=&(inst->input_param);

  	int32_t 		i,j,k;
	int16_t			pic_width		= 0;
	int16_t			pic_height		= 0;
	int16_t			video_width		= input_param->static_param.video_width;
	int16_t			video_height	= input_param->static_param.video_height;

	int16_t			x_offset		= input_param->dynamic_param.x_offset;
	int16_t			y_offset		= input_param->dynamic_param.y_offset;

	int32_t			pic_all_pixel	= 0;

	int16_t			m 				= 0;
	int16_t			n 				= 0;
  
  	uint8_t *students_dst_count=inst->t_stu_global_obj.students_dst_count[0];//ʵ�ʲ����ڴ�ָ�룬��students_time_count���ʹ�ã�Ϊ�˼����ڴ濽��
	int nSrcBufWidth=inst->t_stu_global_obj.nSrcBufWidth;
	int nSrcBufHeight=inst->t_stu_global_obj.nSrcBufHeight;
 	pic_width = video_width / input_param->dynamic_param.x_offset;
    pic_height = video_height / input_param->dynamic_param.y_offset;

	pic_all_pixel = pic_width * pic_height;	
	
	for (k = 0; k < pic_all_pixel; k++)  
	{
		n = k/pic_width;
		m = k%pic_width;
		if (*((uint8_t *)students_dst_count  + k) == 1)
		{
			//�����ĵ��ʾΪ0������ͳ��
			if ((*((uint8_t *)students_dst_count  + (n-1)*pic_width + m -1) == 0)
				&&(*((uint8_t *)students_dst_count  + (n-1)*pic_width + m) == 0)
				&&(*((uint8_t *)students_dst_count  + (n-1)*pic_width + m +1) == 0)
				&&(*((uint8_t *)students_dst_count  + n*pic_width + m -1) == 0)
				&&(*((uint8_t *)students_dst_count  + n*pic_width + m +1) == 0)
				&&(*((uint8_t *)students_dst_count  + (n+1)*pic_width + m -1) == 0)
				&&(*((uint8_t *)students_dst_count  + (n+1)*pic_width + m) == 0)
				&&(*((uint8_t *)students_dst_count  + (n+1)*pic_width + m +1) == 0))
			{
				*((uint8_t *)students_dst_count  + k) = 0; 
			}

			//�ҵ�ͷ���ϵı仯��
			else
			{
				//ǰ���Ѿ��б���������Ҫ�ټӱ���
				for (i= - 2;i< 0;i++)				   
				{
					for (j=  -2;j<=  2;j++)
					{
						if (*((uint8_t *)students_dst_count  + (n+i)*pic_width + m + j) != 0)
						{
							*((uint8_t *)students_dst_count  + k) = 2; 
							output_buf[n * y_offset*nSrcBufWidth+m*x_offset]=0;
							output_buf[nSrcBufWidth*nSrcBufHeight+n * y_offset/2*nSrcBufWidth+m*x_offset/2*2]=0;
							output_buf[nSrcBufWidth*nSrcBufHeight+n * y_offset/2*nSrcBufWidth+m*x_offset/2*2+1]=0;
						}
				
					}  
				} 

			}

		}

	}
	return 0;
} 
static int32_t up_getC(StuITRACK_Params *input_param, uint8_t* inBuf,int16_t m,int16_t n,int16_t inPreC,int16_t *outC)
{
	int32_t 	i;

	int16_t			pic_width		= 0;
	int16_t			video_width		= input_param->static_param.video_width;
	int16_t			x_offset		= input_param->dynamic_param.x_offset;
	int16_t			y_offset		= input_param->dynamic_param.y_offset;
	uint32_t	nData0,nData1,nData2,nData3;
	
 	pic_width = video_width / x_offset;

	if(y_offset<=2)
	{
	
		for(i=1;i<=4;i++)
		{
			nData0=_lo(_memd8(inBuf + (n+inPreC-i)*pic_width + m -3));
			nData1=_hi(_memd8(inBuf + (n+inPreC-i)*pic_width + m -3));
			
			if(((nData0&0x01010101)!=0)||((nData1&0x01010101)!=0))
			{
				return -1;//goto C_Up_End_Pro;
			}
		}
		for(i=0;i<20/y_offset;i++)
		{
			/*nData0=_lo(_memd8(inBuf + (n+inPreC+i)*pic_width + m - 8));
			//nData1=_hi(_memd8(inBuf + (n+inPreC+i)*pic_width + m - 8));
			//nData2=_lo(_memd8(inBuf + (n+inPreC+i)*pic_width + m));
			//nData3=_hi(_memd8(inBuf + (n+inPreC+i)*pic_width + m));
			nData0=nData0&0x01010101;
			nData1=nData1&0x01010101;
			nData2=nData2&0x01010101;
			nData3=nData3&0x01010101;
			if((nData0!=0)||(nData1!=0)||(nData2!=0)||(nData3!=0))*/
			nData0=_lo(_memd8(inBuf + (n+inPreC+i)*pic_width + m - 3));
			nData1=_hi(_memd8(inBuf + (n+inPreC+i)*pic_width + m - 3));
			nData0=nData0&0x01010101;
			nData1=nData1&0x00010101;
			if((nData0!=0)||(nData1!=0))
			{
				*outC = i;
				return 1;//goto C2_Pro_Up;
			}
		}

	}
	else
	{
		
		for(i=1;i<=2;i++)
		{
			nData0=_lo(_memd8(inBuf + (n+inPreC-i)*pic_width + m - 3));
			nData1=_hi(_memd8(inBuf + (n+inPreC-i)*pic_width + m - 3));
			
			if(((nData0&0x01010101)!=0)||((nData1&0x01010101)!=0))
			{
				return -1;//goto C_Up_End_Pro;
			}
		}
		for(i=0;i<5;i++)
		{
			nData0=_lo(_memd8(inBuf + (n+inPreC+i)*pic_width + m -2));
			nData1=_hi(_memd8(inBuf + (n+inPreC+i)*pic_width + m -2));
			nData0=nData0&0x01010101;
			nData1=nData1&0x00000001;
			if((nData0!=0)||(nData1!=0))
			{
				*outC = i;
				return 1;//goto C2_Pro_Up;
			}
		}
	}
	return 0;
}
static int32_t check_up(StuTrackHand *inst, int16_t m, int16_t n,int16_t trigger_width,int16_t trigger_height)
{
	StuITRACK_Params *input_param=&(inst->input_param);
	int32_t 	loop,loopmin=8,loopmax=1,looplast=0;
	int16_t 	C_H=0;
	int16_t	C[9]={0,-1,-1,-1,-1,-1,-1,-1,-1};
	int16_t	C_COUNT=0;

	int16_t			x_offset		= input_param->dynamic_param.x_offset;
	int16_t			y_offset		= input_param->dynamic_param.y_offset;
	int32_t			index	= 0;
	int32_t	nRet,nPreCAll=0;
	int32_t nSrcMultNo=2,nDstMultNo=3;
  	uint8_t **students_dst_count=inst->t_stu_global_obj.students_dst_count;//ʵ�ʲ����ڴ�ָ�룬��students_time_count���ʹ�ã�Ϊ�˼����ڴ濽��
	if(y_offset>2)
	{
		if(trigger_height*y_offset*16<576)
		{
			nSrcMultNo=2;
			nDstMultNo=5;
		}
	}
	for(loop=1;loop<9;loop++)
	{
		nRet=up_getC(input_param, (uint8_t*)students_dst_count[loop],m,n,nPreCAll,&C[loop]);
		if (nRet<0)
		{
			break;//goto C_Up_End_Pro;
		}
		if ((C[loop]==-1)&&(C[loop-1]==-1))
		//if (C[loop]==-1)
		{
			break;//goto C_Up_End_Pro;
		}
		else
		{
			if(C[loop]>0)
			{
				nPreCAll+=C[loop];
			}
		}
	}
//C_Up_End_Pro: 
	for(loop=1;loop<9;loop++)
	{
		if (C[loop]>0)
		{
			C_COUNT ++;
			C_H = C_H + C[loop];
			loopmin=min(loopmin,loop);
			loopmax=max(loopmax,loop);
			if(looplast+2<loop)
			{
				//return 0;
			}
			looplast=loop;
		}
	}
	if((loopmax-loopmin+1)>C_COUNT+2)
	{
		//return 0;
	}
               
	if (C_H*nDstMultNo > trigger_height*nSrcMultNo && C_COUNT>2)
	{  //�����ϵĶ���
		for(index = 0; index < STUDENTS_SHIELD_NUM; index++)
		{
			if((m > input_param->dynamic_param.shield_info[index].x/x_offset)
				&& (m < input_param->dynamic_param.shield_info[index].x/x_offset + input_param->dynamic_param.shield_info[index].width/x_offset)
				&& (n > input_param->dynamic_param.shield_info[index].y/y_offset)
				&& (n < input_param->dynamic_param.shield_info[index].y/y_offset + input_param->dynamic_param.shield_info[index].height/y_offset))
			{
				return 0;
			}
		}
		return 1;
	} 
	else
	{//û���ҵ����ϵĶ���
		return 0;
	}
}

static int32_t down_getC(StuITRACK_Params *input_param, uint8_t* inBuf,int16_t m,int16_t n,int16_t inPreC,int16_t *outC)
{
	int32_t 	i;

	int16_t			pic_width		= 0;
	int16_t			video_width		= input_param->static_param.video_width;
	int16_t			x_offset		= input_param->dynamic_param.x_offset;
	int16_t			y_offset		= input_param->dynamic_param.y_offset;
	uint32_t	nData0,nData1,nData2,nData3;

 	pic_width = video_width / x_offset;
	if(y_offset<=2)
	{
	
		for(i=1;i<=4;i++)
		{
			nData0=_lo(_memd8(inBuf + (n-inPreC+i)*pic_width + m -3));
			nData1=_hi(_memd8(inBuf + (n-inPreC+i)*pic_width + m -3));
			
			if(((nData0&0x01010101)!=0)||((nData1&0x01010101)!=0))
			{
				return -1;//goto C_Up_End_Pro;
			}
		}
		for(i=0;i<20/y_offset;i++)
		{
			nData0=_lo(_memd8(inBuf + (n-inPreC-i)*pic_width + m - 8));
			nData1=_hi(_memd8(inBuf + (n-inPreC-i)*pic_width + m - 8));
			nData2=_lo(_memd8(inBuf + (n-inPreC-i)*pic_width + m));
			nData3=_hi(_memd8(inBuf + (n-inPreC-i)*pic_width + m));
			nData0=nData0&0x01010101;
			nData1=nData1&0x01010101;
			nData2=nData2&0x01010101;
			nData3=nData3&0x01010101;
			if((nData0!=0)||(nData1!=0)||(nData2!=0)||(nData3!=0))
			{
				*outC = i;
				return 1;//goto C2_Pro_Up;
			}
		}

	}
	else
	{
		
		for(i=1;i<=2;i++)
		{
			nData0=_lo(_memd8(inBuf + (n-inPreC+i)*pic_width + m - 3));
			nData1=_hi(_memd8(inBuf + (n-inPreC+i)*pic_width + m - 3));
			
			if(((nData0&0x01010101)!=0)||((nData1&0x01010101)!=0))
			{
				return -1;//goto C_Up_End_Pro;
			}
		}
		for(i=0;i<5;i++)
		{
			nData0=_lo(_memd8(inBuf + (n-inPreC-i)*pic_width + m -3));
			nData1=_hi(_memd8(inBuf + (n-inPreC-i)*pic_width + m -3));
			nData0=nData0&0x01010101;
			nData1=nData1&0x01010101;
			if((nData0!=0)||(nData1!=0))
			{
				*outC = i;
				return 1;//goto C2_Pro_Up;
			}
		}
	}
	return 0;
}

static int32_t check_down(StuTrackHand *inst, int16_t m, int16_t n,int16_t trigger_width,int16_t trigger_height)
{
	StuITRACK_Params *input_param=&(inst->input_param);
	int32_t 	loop;
	int16_t 	C_H=0;
	int16_t	C[9]={0,-1,-1,-1,-1,-1,-1,-1,-1};
	int16_t	C_COUNT=0;
	int16_t			y_offset		= input_param->dynamic_param.y_offset;
	int16_t			x_offset		= input_param->dynamic_param.x_offset;
	int32_t	nRet,nPreCAll=0;
	int32_t nSrcMultNo=2,nDstMultNo=3;
  	uint8_t **students_dst_count=inst->t_stu_global_obj.students_dst_count;//ʵ�ʲ����ڴ�ָ�룬��students_time_count���ʹ�ã�Ϊ�˼����ڴ濽��
	int16_t *Preset_Location=inst->t_stu_global_obj.Preset_Location;
	int16_t *k_Nearest= inst->t_stu_global_obj.k_Nearest;		//����ĵ�
	student_trigger_info_t	trigger_info;	//ѧ�������ٵĴ�������Ϣ

	if(y_offset>2)
	{
		if(trigger_height*y_offset*16<576)
		{
			nSrcMultNo=2;
			nDstMultNo=5;
		}
	}

      //���µļ��
	for(loop=1;loop<9;loop++)
	{
		nRet=down_getC(input_param, (uint8_t*)students_dst_count[loop],m,n,nPreCAll,&C[loop]);
		if (nRet<0)
		{
			break;//goto C_Dn_End_Pro;
		}
		if ((C[loop]==-1)&&(C[loop-1]==-1))
		{
			break;//goto C_Dn_End_Pro;
		}
		else
		{
			if(C[loop]>0)
			{
				nPreCAll+=C[loop];
			}
		}
	}
//	C_Dn_End_Pro: 
	for(loop=1;loop<9;loop++)
	{
		if (C[loop]>0)
		{
			C_COUNT ++;
			C_H = C_H + C[loop];
		}
		//Vps_printf("c[%d] = %d %d\n",loop, C[loop],trigger_height);
	}
	if (C_H*nDstMultNo > trigger_height*nSrcMultNo && C_COUNT>2)
	{  
		m = m;
		n = n-C_H;
		nearest_point(inst,m, n); 
		//������Ŀ�
		trigger_info=input_param->dynamic_param.trigger_info[k_Nearest[0]];
		if((abs(trigger_info.x+trigger_info.width/2-m*x_offset)<6*trigger_width*x_offset)
		&&(abs(trigger_info.y+trigger_info.height/2-n*y_offset)<6*trigger_height*y_offset))
		{
			Preset_Location[k_Nearest[0]+1]=0;//���־λ����ʾ�Ѿ�����
		}

		//��ڶ����Ŀ�
		trigger_info=input_param->dynamic_param.trigger_info[k_Nearest[1]];
		if((abs(trigger_info.x+trigger_info.width/2-m*x_offset)<6*trigger_width*x_offset)
		&&(abs(trigger_info.y+trigger_info.height/2-n*y_offset)<6*trigger_height*y_offset))
		{
			Preset_Location[k_Nearest[1]+1]=0;//���־λ����ʾ�Ѿ�����
		}

		//��������Ŀ�
		trigger_info=input_param->dynamic_param.trigger_info[k_Nearest[2]];
		if((abs(trigger_info.x+trigger_info.width/2-m*x_offset)<6*trigger_width*x_offset)
		&&(abs(trigger_info.y+trigger_info.height/2-n*y_offset)<6*trigger_height*y_offset))
		{
			Preset_Location[k_Nearest[2]+1]=0;//���־λ����ʾ�Ѿ�����
		}
		return 1;
	}
	return 0;
}

static int32_t CheckHeadPoint(StuTrackHand *inst, int8_t *output_buf, int16_t m, int16_t n,int16_t trigger_width,int16_t trigger_height,int16_t* npHeadShift)
{
	StuITRACK_Params *input_param=&(inst->input_param);
	int32_t 	i,j,k;
	int16_t 	T_0_Count= 0,T_1_Count= 0,T_2_Count= 0,T_3_Count= 0;

	int16_t 		pic_width		= 0;
	int16_t 		video_width 	= input_param->static_param.video_width;
	int16_t 		video_height	= input_param->static_param.video_height;
	int16_t 		x_offset		= input_param->dynamic_param.x_offset;
	int16_t 		y_offset		= input_param->dynamic_param.y_offset;
	int32_t	min_x,min_y,max_x,max_y,max_width,nValWidth,max_headwidth;
	int32_t	nSrcMultNo=1,nDstMultNo=2;
	int32_t	nHeadFlag=0,nHeadShift=0;
	int32_t head_width;
	line_info_t 		line_info	= {0};
	students_yuv_value_info_t	yuv_value	= {0};
  	uint8_t *students_dst_count=inst->t_stu_global_obj.students_dst_count[0];//ʵ�ʲ����ڴ�ָ�룬��students_time_count���ʹ�ã�Ϊ�˼����ڴ濽��
	int nSrcBufWidth=inst->t_stu_global_obj.nSrcBufWidth;
	int nSrcBufHeight=inst->t_stu_global_obj.nSrcBufHeight;
	
	int16_t trigger_width_cur=trigger_width;
	int16_t	trigger_width_step=trigger_width>>2;
	int16_t	trigger_width_max=trigger_width;

	int16_t loop=0;
	int16_t nFindHead=0,nFindHeadInUpArea=0;

	if(trigger_width_step==0)
	{
		trigger_width_step=2;
	}
	trigger_width_max=trigger_width*2;
	while(trigger_width_cur<=trigger_width_max)
	{
		head_width=trigger_width_cur*2/3;
		if(head_width<2)
		{
			head_width=2;
		}
		if(head_width*x_offset*20>704)
		{//1/3
			nSrcMultNo=1;
			nDstMultNo=2;
		}
		else if(head_width*x_offset*28>704)
		{//1/2
			nSrcMultNo=1;
			nDstMultNo=2;
		}
		else if(head_width*x_offset*32>704)
		{//3/5
			nSrcMultNo=3;
			nDstMultNo=5;
		}
		else //if(head_width*x_offset*32<704)
		{//2/3
			nSrcMultNo=2;
			nDstMultNo=3;
		}
		
		pic_width = video_width /x_offset;
		nHeadFlag=0;
		 for(k= -1;k<=1;k++)
		 {
			 T_0_Count=0;
			 min_x=video_width;
			 max_x=0;
			 min_y=video_height;
			 max_y=0;
			 max_width=0;
			 nValWidth=0;
			nHeadShift=k*head_width/2;
			 //head   k=1ʱ����ͷ������ʵ��ͷ������ߣ�k=0ʱ���м䣬k=-1ʱ����ͷ������ʵ��ͷ�����ұ�
			for (i= 0;i<=head_width;i++) 	  
			{
				min_x=video_width;
				max_x=0;

				for (j= -head_width*3/4+nHeadShift;j<= head_width*3/4+nHeadShift;j++)
				{
					if (*((uint8_t*)students_dst_count + (n+i)*pic_width + m + j) != 0)
					{
						T_0_Count++; 
						min_x=min(min_x,j);
						max_x=max(max_x,j);
						min_y=min(min_y,i);
						max_y=max(max_y,i);
						nValWidth=max_x-min_x;
					}
				}  
				max_width=max(max_width,nValWidth);
			}  
			max_headwidth=max_width;
			if ((T_0_Count*nDstMultNo > head_width*head_width*nSrcMultNo )
				&&(max_width*5>head_width*3)&&((max_y-min_y)*5>head_width*3)
				)
			{
				nHeadFlag=1;
				break;
			}
		}
		 if(nHeadFlag==0)
		 {
			 trigger_width_cur+=trigger_width_step;
			 continue;
			 //*((uint8_t*)students_dst_count + n*pic_width + m ) = 2;
			 
			 //output_buf[n * y_offset*nSrcBufWidth+m*x_offset]=0;
			 //output_buf[nSrcBufWidth*nSrcBufHeight+n * y_offset/2*nSrcBufWidth+m*x_offset/2*2]=0;
			 //output_buf[nSrcBufWidth*nSrcBufHeight+n * y_offset/2*nSrcBufWidth+m*x_offset/2*2+1]=0;
			// return 0;
		 }
		//		 ͷ���µĵڶ� �����ڱ仯����� �����岿��Ҳ�б仯	
		T_1_Count=0;
		min_x=video_width;
		max_x=0;
		min_y=video_height;
		max_y=0;
		max_width=0;
		nValWidth=0;
		for (i= head_width;i<=head_width*2;i++)
		{
			min_x=video_width;
			max_x=0;
			for (j= -head_width+nHeadShift;j<= head_width+nHeadShift;j++)
			{
				if (*((uint8_t*)students_dst_count + (n+i)*pic_width + m + j) != 0)
				{
						T_1_Count++; 
						min_x=min(min_x,j);
						max_x=max(max_x,j);
						min_y=min(min_y,i);
						max_y=max(max_y,i);
						nValWidth=max_x-min_x;
				}
			}  
			max_width=max(max_width,nValWidth);
		}  
		if ((T_1_Count*nDstMultNo< head_width*head_width*nSrcMultNo )
			||(max_width<max_headwidth)||((max_y-min_y)*2<head_width*1)
			)
		{
			trigger_width_cur+=trigger_width_step;
			continue;
			//*((uint8_t*)students_dst_count + n*pic_width + m ) = 2;
			
			//output_buf[n * y_offset*nSrcBufWidth+m*x_offset]=0;
			//output_buf[nSrcBufWidth*nSrcBufHeight+n * y_offset/2*nSrcBufWidth+m*x_offset/2*2]=0;
			//output_buf[nSrcBufWidth*nSrcBufHeight+n * y_offset/2*nSrcBufWidth+m*x_offset/2*2+1]=0;
			//return 0;
		}
		
#if 1
		T_2_Count=0;
		T_3_Count=0;
		for (i= 0;i<=head_width;i++)
		{
			for (j= -head_width*3/2+nHeadShift;j<= -head_width/2+nHeadShift;j++)
			{
				if (*((uint8_t*)students_dst_count + (n+i)*pic_width + m + j) != 0)
				{
						T_2_Count++; 
				}
			}  
			for (j= head_width/2+nHeadShift;j<= head_width*3/2+nHeadShift;j++)
			{
				if (*((uint8_t*)students_dst_count + (n+i)*pic_width + m + j) != 0)
				{
						T_3_Count++; 
				}
			}  
		}  
		if ((T_2_Count*5>= T_0_Count*4)||(T_2_Count*5>= T_1_Count*4)||(T_3_Count*5>= T_0_Count*4)||(T_3_Count*5>= T_1_Count*4))
		{
			trigger_width_cur+=trigger_width_step;
			continue;
			//*((uint8_t*)students_dst_count + n*pic_width + m ) = 2;
			
			//output_buf[n * y_offset*nSrcBufWidth+m*x_offset]=0;
			//output_buf[nSrcBufWidth*nSrcBufHeight+n * y_offset/2*nSrcBufWidth+m*x_offset/2*2]=0;
			//output_buf[nSrcBufWidth*nSrcBufHeight+n * y_offset/2*nSrcBufWidth+m*x_offset/2*2+1]=0;
			//return 0;
		}
#endif
		*npHeadShift=nHeadShift;
		for (i= -trigger_width_cur;i< 0;i++)		//					   
		{
			for (j=  -trigger_width_cur;j <= trigger_width_cur;j++)
			{
				if (*((uint8_t*)students_dst_count + (n+i)*pic_width + m + j)== 1)
				{
					nFindHeadInUpArea=1;
				}
			} 
		}  
		nFindHead=1;
		break;
	}
	if(nFindHeadInUpArea==1)
	{
		nFindHead=0;
	}
	if(nFindHead==0)
	{
		*((uint8_t*)students_dst_count + n*pic_width + m ) = 2;
	}
	return nFindHead;
}
//�Ƚ�ǰ10֡ͼ�񣬿��仯����Ϣ 
static int32_t CmpBefore10Frame(StuTrackHand *inst, int8_t *output_buf, int16_t m, int16_t n,int16_t trigger_width,int16_t trigger_height,int32_t nHeadShift)
{
	StuITRACK_Params *input_param=&(inst->input_param);
	int32_t 	i,j;
	int16_t 		pic_width		= 0;
	int16_t			sens			= input_param->dynamic_param.sens;
	int16_t 		x_offset		= input_param->dynamic_param.x_offset;
	int16_t 		y_offset		= input_param->dynamic_param.y_offset;
	int32_t	min_y,max_y,max_width,max_height,nValWidth,max_width_now;
	int32_t T_Count,T_Count_More=0;
	line_info_t 		line_info	= {0};
	students_yuv_value_info_t	yuv_value	= {0};
  	uint8_t *students_dst_count=inst->t_stu_global_obj.students_dst_count[0];//ʵ�ʲ����ڴ�ָ�룬��students_time_count���ʹ�ã�Ϊ�˼����ڴ濽��
	uint8_t **students_src_y_buf=inst->t_stu_global_obj.students_src_y_buf;   //������ź�Ybuffer����,ÿ10֡���һ��ԭʼ���� 
	int nSrcBufWidth=inst->t_stu_global_obj.nSrcBufWidth;
	int nSrcBufHeight=inst->t_stu_global_obj.nSrcBufHeight;

	//������ǰ�����ݱȽϣ��仯����Ϣͳ��
	T_Count=0;
	T_Count_More=0;
	if(trigger_height<trigger_width)
	{
		trigger_height=trigger_width;
	}
	max_width=0;
	for (i= 0;i<=trigger_height;i++)
	{
		nValWidth=0;
		max_width_now=0;
		for (j= -trigger_width/2+nHeadShift;j<= trigger_width/2+nHeadShift;j++)
		{
			if(abs(*((uint8_t *)output_buf + (n+i)*y_offset*nSrcBufWidth + (m + j)*x_offset)-*(students_src_y_buf[inst->t_stu_global_obj.nCopyPos]+(n+i)*pic_width+m + j))>sens) 
			{
					T_Count++; 
					min_y=min(min_y,i);
					max_y=max(max_y,i);
					nValWidth++;
			}
			else
			{
				max_width_now=max(max_width_now,nValWidth);
				nValWidth=0;
			}
		}  
		max_width=max(max_width,max_width_now);
		if(max_width_now*5>trigger_width*4)
		{
			T_Count_More++;
		}
	}  
	max_height=max_y-min_y;
	if ((T_Count*5< trigger_width*trigger_height*3 )
		//||(max_width*5<trigger_width*4)
		||(max_height*5<trigger_height*4)
		||(T_Count*2<max_width*max_height*1)
		||(T_Count_More*5<trigger_height)
		)
	{
		*((uint8_t*)students_dst_count + n*pic_width + m ) = 2;
		
		output_buf[n * y_offset*nSrcBufWidth+m*x_offset]=0;
		output_buf[nSrcBufWidth*nSrcBufHeight+n * y_offset/2*nSrcBufWidth+m*x_offset/2*2]=0;
		output_buf[nSrcBufWidth*nSrcBufHeight+n * y_offset/2*nSrcBufWidth+m*x_offset/2*2+1]=0;
		return 0;
	}
	 
	return 1;
}

static int32_t img_compare_y1(StuTrackHand *inst, int8_t *output_buf)
{
	StuITRACK_Params *input_param=&(inst->input_param);
	int32_t 	i,k;

	int16_t			m 				= 0;
	int16_t			n 				= 0;

	int16_t			pic_width		= 0;
	int16_t			pic_height		= 0;
	int16_t			video_width		= input_param->static_param.video_width;
	int16_t			video_height	= input_param->static_param.video_height;

	int16_t			x_offset		= input_param->dynamic_param.x_offset;
	int16_t			y_offset		= input_param->dynamic_param.y_offset;

	int32_t			pic_all_pixel	= 0;

	int16_t			trigger_width	= 0;
	int16_t			trigger_height	= 0;
	int16_t			trigger_number	= 0;

	line_info_t 		line_info	= {0};
	students_yuv_value_info_t 	yuv_value	= {0};
  	uint8_t *students_dst_count=inst->t_stu_global_obj.students_dst_count[0];//ʵ�ʲ����ڴ�ָ�룬��students_time_count���ʹ�ã�Ϊ�˼����ڴ濽��
	uint8_t *Students_Area_vertex=inst->t_stu_global_obj.Students_Area_vertex;
	TStuCheckPoint *t_check_point=inst->t_stu_global_obj.t_check_point;

	int32_t	nRet;
	int16_t nHeadShift=0;
 	pic_width = video_width / x_offset;
    pic_height = video_height /y_offset;

	pic_all_pixel = pic_width * pic_height;	
	
	for (k = 0; k < pic_all_pixel; k++)  
	{
	    n = k/pic_width;
	    m = k%pic_width;
		if (*((uint8_t *)Students_Area_vertex + n* y_offset*video_width+ m* x_offset) == 0)
		{
			continue;
		}
		if (*((uint8_t*)students_dst_count + k) == 1)
		{
			trigger_number = nearest_point(inst, m, n);
			trigger_width = input_param->dynamic_param.trigger_info[trigger_number].width/x_offset;
			trigger_height = input_param->dynamic_param.trigger_info[trigger_number].height/y_offset;

			if(trigger_width<2)
			{
				trigger_width=2;
			}
			if(trigger_height<2)
			{
				trigger_height=2;
			}
			nHeadShift=0;
			nRet=CheckHeadPoint(inst, output_buf,  m, n, trigger_width,trigger_height,&nHeadShift);
			if(nRet==0)
			{
				continue;
			}
			
			line_info.start_x =m*x_offset;
			line_info.start_y=n*y_offset;
			
			line_info.end_x=line_info.start_x+4;
			line_info.end_y=line_info.start_y+4;
			
			yuv_value.y_value = 255;
			yuv_value.cb_value	= 128;
			yuv_value.cr_value	= 0;
			fill_rectangle(inst,output_buf, &yuv_value, &line_info);
			nRet=check_up(inst, m, n, trigger_width, trigger_height);
			if(nRet==1)
			{
				//nRet=CmpBefore10Frame(inst, output_buf,m,n,trigger_width,trigger_height,nHeadShift);
				//if(nRet==1)
				{
					line_info.start_x =m* x_offset-trigger_width*x_offset/2;//-trigger_width/4;
					line_info.start_y=n*y_offset;
					
					line_info.end_x=line_info.start_x+trigger_width*x_offset;//+trigger_width/2;
					line_info.end_y=line_info.start_y+trigger_width*y_offset*2;
					
					yuv_value.y_value = 255;
					yuv_value.cb_value	= 255;
					yuv_value.cr_value	= 255;
					fill_rectangle(inst,output_buf, &yuv_value, &line_info);
				//�����ϵĶ���
			//�ж�һ����Χ���ǲ����Ѿ��д����㣬������򲻸���
					for (i=0;i<STU_MAX_CHECKNUM;i++) 
					{
						if(t_check_point[i].nCheckFlag==1)
						{
							if((abs(m-t_check_point[i].nCheckX)<trigger_width)
								&&(abs(n-t_check_point[i].nCheckY)<trigger_width))
							{
								t_check_point[i].nCheckTime = 12;//��ʱ��ԭ���㷨����һ��
								t_check_point[i].nCheckX=(t_check_point[i].nCheckX+m)/2;
								t_check_point[i].nCheckY=(t_check_point[i].nCheckY+n)/2;
								break;
							}
						}
					}
					//��һ���µĴ�����
					if(i==STU_MAX_CHECKNUM)
					{
						for (i=0;i<STU_MAX_CHECKNUM-1;i++) 
						{
							t_check_point[i] = t_check_point[i+1];
						}
						t_check_point[STU_MAX_CHECKNUM-1].nCheckFlag=1;//��־λ��1��ʾ�д�����
						t_check_point[STU_MAX_CHECKNUM-1].nCheckTime = 12;//��ʱ
						t_check_point[STU_MAX_CHECKNUM-1].nCheckX= m;//λ��
						t_check_point[STU_MAX_CHECKNUM-1].nCheckY= n;//λ��

					}
				}

			}
			else
			{
			//û���ҵ����ϵĶ������������Ƿ������µĶ���
				nRet=check_down(inst,m, n, trigger_width, trigger_height);
			
				if(nRet==1)
				{
					line_info.start_x =m* x_offset-trigger_width*x_offset/2;//-trigger_width/4;
					line_info.start_y=n*y_offset;
					
					line_info.end_x=line_info.start_x+trigger_width*x_offset;//+trigger_width/2;
					line_info.end_y=line_info.start_y+trigger_width*y_offset;
					
					yuv_value.y_value = 255;
					yuv_value.cb_value	= 128;
					yuv_value.cr_value	= 128;
					fill_rectangle(inst,output_buf, &yuv_value, &line_info);
				}
			}
			
		}
	}
	return 0;
}
//���������λ�������� 
static int32_t GetStandUpPos(StuTrackHand *inst, int16_t nTriggerNo)
{
	StuITRACK_Params *input_param=&(inst->input_param);
	int16_t				x_offset	= input_param->dynamic_param.x_offset;
	int16_t				y_offset	= input_param->dynamic_param.y_offset;

	int16_t m,n;
	int32_t i,index; 
	int32_t nMaxTriggerY=0,nMinTriggerY=input_param->static_param.video_height;
	int32_t nMaxTriggerX=0,nMinTriggerX=input_param->static_param.video_width;
	int32_t nTempX=0,nTempY=0;
	int32_t nPos=0;
	m=input_param->dynamic_param.trigger_info[nTriggerNo].x+input_param->dynamic_param.trigger_info[nTriggerNo].width/2;
	n=input_param->dynamic_param.trigger_info[nTriggerNo].y+input_param->dynamic_param.trigger_info[nTriggerNo].height/2;
	for(index = 0; index < input_param->dynamic_param.track_point_num; index++)
	{
		nMaxTriggerX=max(nMaxTriggerX,input_param->dynamic_param.track_point[index].x);
		nMinTriggerX=min(nMinTriggerX,input_param->dynamic_param.track_point[index].x);
		nMaxTriggerY=max(nMaxTriggerY,input_param->dynamic_param.track_point[index].y);
		nMinTriggerY=min(nMinTriggerY,input_param->dynamic_param.track_point[index].y);
	}
	nTempX=nMinTriggerX+(nMaxTriggerX-nMinTriggerX)/2;
	nTempY=nMinTriggerY+(nMaxTriggerY-nMinTriggerY)*2/5;
	if(n>=nTempY)
	{
		if(m<=nTempX)
		{//��ǰ��
			nPos=1;
		}
		else
		{//��ǰ��
			nPos=0;
		}

	}
	else
	{
		if(m<=nTempX)
		{//���
			nPos=3;
		}
		else
		{//�Һ�
			nPos=2;
		}

	}
	return nPos;
}	

static int32_t img_process_y(StuTrackHand *inst, int8_t *output_buf,StuITRACK_OutArgs *output_param)
{
	StuITRACK_Params *input_param=&(inst->input_param);
	line_info_t 		line_info	= {0};
	students_yuv_value_info_t 	yuv_value	= {0};
	int16_t				x_offset	= input_param->dynamic_param.x_offset;
	int16_t				y_offset	= input_param->dynamic_param.y_offset;

	int16_t				m 			= 0;
	int16_t				n			= 0;
	int32_t i; 
	int32_t nPos=0;
	
	TStuCheckPoint *t_check_point=inst->t_stu_global_obj.t_check_point;
	
	int16_t *Preset_Location=inst->t_stu_global_obj.Preset_Location;	//1~36
	int16_t *Preset_Delay=inst->t_stu_global_obj.Preset_Delay;
	int16_t *k_Nearest=inst->t_stu_global_obj.k_Nearest;		//����ĵ�
	int16_t nTriggerNo;
	t_mvdet_info *tMoveInfo=&(inst->t_stu_global_obj.tMoveInfo);
	for (i=0;i<STU_MAX_CHECKNUM;i++) 
	{
		if (t_check_point[i].nCheckFlag== 1)
		{
			t_check_point[i].nCheckTime--;
			if (t_check_point[i].nCheckTime == 0)
			{
				t_check_point[i].nCheckFlag= 0;
			}

			if ( t_check_point[i].nCheckTime==8)
			{
				m = t_check_point[i].nCheckX;
				n = t_check_point[i].nCheckY;
				nTriggerNo=nearest_point(inst, m,n); 
			//�������λ��
				//nPos=GetStandUpPos(inst, m, n);
				nPos=GetStandUpPos(inst, nTriggerNo);
				output_param->nStandUpPos[nPos]++;
				
				Preset_Location[k_Nearest[0]+1] = 1;
				Preset_Delay[k_Nearest[0]+1] = input_param->dynamic_param.reset_time; 
				tMoveInfo->nMoveDetectFlag=1;
				tMoveInfo->nMoveDetectDelay=STU_MVDET_DELAY;
				tMoveInfo->nMoveDetectNum++;

				line_info.start_x = m * x_offset;
				line_info.start_y = n * y_offset;
				line_info.end_x = line_info.start_x + x_offset * 8;
				line_info.end_y = line_info.start_y + y_offset * 8;
				
				yuv_value.y_value = 255;
				yuv_value.cb_value	= 255;
				yuv_value.cr_value	= 0;
				
				if(DRAW_TRIGGER_TRACK == input_param->dynamic_param.message)
				{
					//draw_rectangle(inst,output_buf, &yuv_value, &line_info);
				}
			}

		} 
	}
	return 0;
}	
static int32_t MoveDetect(StuTrackHand *inst, int8_t *output_buf,StuITRACK_OutArgs *output_param)
{
	StuITRACK_Params *input_param=&(inst->input_param);
	int32_t 	i;

	int16_t			m 				= 0;
	int16_t			n 				= 0;

	int16_t			pic_width		= 0;
	int16_t			pic_height		= 0;
	int16_t			video_width		= input_param->static_param.video_width;
	int16_t			video_height	= input_param->static_param.video_height;

	int16_t			x_offset		= input_param->dynamic_param.x_offset;
	int16_t			y_offset		= input_param->dynamic_param.y_offset;

	int16_t			trigger_width	= 0;
	int16_t			trigger_height	= 0;
	int16_t			trigger_number	= 0;

	line_info_t 		line_info	= {0};
	students_yuv_value_info_t 	yuv_value	= {0};
	t_mvdet_info *tMoveInfo=&(inst->t_stu_global_obj.tMoveInfo);
  	uint8_t *students_dst_count=inst->t_stu_global_obj.students_dst_count[0];//ʵ�ʲ����ڴ�ָ�룬��students_time_count���ʹ�ã�Ϊ�˼����ڴ濽��
	int32_t (*nMvXPos)[9]=inst->t_stu_global_obj.nMvXPos;
	int32_t (*nMvYPos)[9]=inst->t_stu_global_obj.nMvYPos;


	int32_t			index	= 0;
	int32_t	nDifNum=0,nSumX=0,nSumY=0,nCenterX=0,nCenterY=0,nDifY=0,nCount=0,nDifY_down=0,nCount_down=0;
	int32_t nMaxY=0,nFlag=0;
	int32_t nShiftH;
	int32_t	x0,x1,y0,y1;
	int32_t nFindMoveUpNum=0,nFindMoveDownNum=0;
 	pic_width = video_width / input_param->dynamic_param.x_offset;
    pic_height = video_height / input_param->dynamic_param.y_offset;

  	for(index = 0; index < STUDENTS_MULTITARGET_NUM; index++)
	{
		//������Ч�����
		if(input_param->dynamic_param.multitarget_info[index].width==0)
		{
			continue;
		}
		//�ж���̨����־
		if(tMoveInfo->nFindMoveFlag[index]==1)
		{
			tMoveInfo->nFindMoveDelay[index]--;
			if(tMoveInfo->nFindMoveDelay[index]==0)
			{
				tMoveInfo->nFindMoveFlag[index]=0;
			}
		}
		//�ж���̨����־
		if(tMoveInfo->nFindMoveFlag_down[index]==1)
		{
			tMoveInfo->nFindMoveDelay_down[index]--;
			if(tMoveInfo->nFindMoveDelay_down[index]==0)
			{
				tMoveInfo->nFindMoveFlag_down[index]=0;
			}
		}
		//�Ͻ�̨�½�̨����־��Ϊ1ʱ����
		if((tMoveInfo->nFindMoveFlag[index]==1)&&(tMoveInfo->nFindMoveFlag_down[index]==1))
		{
			continue;
		}
		x0=input_param->dynamic_param.multitarget_info[index].x/x_offset;
		x1=x0+input_param->dynamic_param.multitarget_info[index].width/x_offset;
		y0=input_param->dynamic_param.multitarget_info[index].y/y_offset;
		y1=y0+input_param->dynamic_param.multitarget_info[index].height/y_offset;

		nDifNum=0;
		nSumX=0;
		nSumY=0;
		nCenterX=0;
		nCenterY=0;
		for(n=y0;n<y1;n++)
		{
			nFlag=0;
			for(m=x0;m<x1;m++)
			{
				if(*((uint8_t*)students_dst_count + n*pic_width+m) != 0)
				{
					nDifNum++;
					nSumX+=m;
					nSumY+=n;
					nFlag=1;
				}
			}
			if(nFlag==1)
			{
				nMaxY=n;
			}

		}
		if(nDifNum!=0)
		{
			nCenterX=nSumX/nDifNum;
			nCenterY=nSumY/nDifNum;
			trigger_number = nearest_point(inst, nCenterX, nCenterY);
			trigger_width = input_param->dynamic_param.trigger_info[trigger_number].width/x_offset;
			trigger_height = input_param->dynamic_param.trigger_info[trigger_number].height/y_offset;
			nShiftH = trigger_height*5/3;
			if(nDifNum*3<trigger_width*trigger_width*2)
			{
				nCenterX=nMvXPos[index][0];
				nCenterY=nMvYPos[index][0];
				nMaxY=nMvYPos[index][0];
				memset(&nMvYPos[index],0,9*sizeof(int32_t));
				continue;
			}
			nMvXPos[index][8]=nMvXPos[index][7];
			nMvXPos[index][7]=nMvXPos[index][6];
			nMvXPos[index][6]=nMvXPos[index][5];
			nMvXPos[index][5]=nMvXPos[index][4];
			nMvXPos[index][4]=nMvXPos[index][3];
			nMvXPos[index][3]=nMvXPos[index][2];
			nMvXPos[index][2]=nMvXPos[index][1];
			nMvXPos[index][1]=nMvXPos[index][0];
			nMvXPos[index][0]=nCenterX;
			
			nMvYPos[index][8]=nMvYPos[index][7];
			nMvYPos[index][7]=nMvYPos[index][6];
			nMvYPos[index][6]=nMvYPos[index][5];
			nMvYPos[index][5]=nMvYPos[index][4];
			nMvYPos[index][4]=nMvYPos[index][3];
			nMvYPos[index][3]=nMvYPos[index][2];
			nMvYPos[index][2]=nMvYPos[index][1];
			nMvYPos[index][1]=nMvYPos[index][0];
			nMvYPos[index][0]=nMaxY;//nCenterY;
			//�½�̨ÿ�ζ���
			
			if(tMoveInfo->nFindMoveFlag_down[index]==0)
			{		
				nDifY_down=0;
				nCount_down=0;
				for(i=1;i<9;i++)
				{
					if((nMvYPos[index][i-1]<nMvYPos[index][i]))
					{
						nCount_down++;
						nDifY_down+=(nMvYPos[index][i]-nMvYPos[index][i-1]);
					}
				}
			
				if((nCount_down>4)&&(nDifY_down>nShiftH))
				{
					tMoveInfo->nFindMoveFlag_down[index]=1;
					tMoveInfo->nFindMoveDelay_down[index]=STU_FIND_MV_DELAY;
					nFindMoveDownNum++;
					line_info.start_x = nCenterX * x_offset;
					line_info.start_y = nMaxY* y_offset;
					line_info.end_x = line_info.start_x + x_offset * trigger_width*2;
					line_info.end_y = line_info.start_y + y_offset * trigger_width*2;
					
					yuv_value.y_value = 255;
					yuv_value.cb_value	= 128;
					yuv_value.cr_value	= 128;
					
					fill_rectangle(inst,output_buf, &yuv_value, &line_info);
					memset(&nMvYPos[index],0,9*sizeof(int32_t));
				}
			}
			//�Ͻ�̨����������ʱ�ż��
			if((inst->t_stu_global_obj.tMoveInfo.nMoveDetectFlag==1)&&(tMoveInfo->nFindMoveFlag[index]==0))
			{
				nDifY=0;
				nCount=0;
				for(i=1;i<9;i++)
				{
					if((nMvYPos[index][i-1]>nMvYPos[index][i]))
					{
						nCount++;
						nDifY+=(nMvYPos[index][i-1]-nMvYPos[index][i]);
					}
				}
				if((nCount>4)&&(nDifY>nShiftH))
				{
					tMoveInfo->nFindMoveFlag[index]=1;
					tMoveInfo->nFindMoveDelay[index]=STU_FIND_MV_DELAY;
					nFindMoveUpNum++;
					tMoveInfo->nMoveDetectNum--;
					if(tMoveInfo->nMoveDetectNum<=0)
					{
						tMoveInfo->nMoveDetectFlag=0;
						tMoveInfo->nMoveDetectDelay=0;
					}
					line_info.start_x = nCenterX * x_offset;
					line_info.start_y = nMaxY* y_offset;
					line_info.end_x = line_info.start_x + x_offset * trigger_width*2;
					line_info.end_y = line_info.start_y + y_offset * trigger_width*2;
					
					yuv_value.y_value = 255;
					yuv_value.cb_value	= 0;
					yuv_value.cr_value	= 255;
					
					fill_rectangle(inst,output_buf, &yuv_value, &line_info);
					memset(&nMvYPos[index],0,9*sizeof(int32_t));
				}
			}
				
		}
		
		
	}
	output_param->nFindMoveUpNum=nFindMoveUpNum;
	
	output_param->nFindMoveDownNum=nFindMoveDownNum;
	return 0;
}
static int32_t	delay_dec(StuTrackHand *inst)
{
	int16_t *Preset_Location=inst->t_stu_global_obj.Preset_Location;
	int16_t *Preset_Delay=inst->t_stu_global_obj.Preset_Delay;
	int i;

	//Ԥ��λ��ʱ�ݼ�
	for (i = 0;i < STUDENTS_TRIGGER_NUM;i++)
	{
		if (Preset_Location[i+1]== 1)     
		{
			Preset_Delay[i+1]--;
			if (Preset_Delay[i+1] == 0)
			{
				Preset_Delay[i+1]= 1;
				Preset_Location[i+1] = 0;
			}
		}
	}    
	//�˴�ȥ�������´���λ��ʱ�ݼ�
	return 0;
}  

static int32_t	send_code(StuTrackHand *inst, int8_t *output_buf,StuITRACK_OutArgs *output_param)
{
	StuITRACK_Params *input_param=&(inst->input_param);
	int16_t *Preset_Location=inst->t_stu_global_obj.Preset_Location;
	int16_t *Preset_Delay=inst->t_stu_global_obj.Preset_Delay;
	uint8_t	i,temp,delay_max=0;
	int16_t position_num=42;
	int16_t preset_num=0;
	line_info_t 		line_info	= {0};
	students_yuv_value_info_t 	yuv_value	= {0};
	output_param->cmd_type = 0;
	if (inst->t_stu_global_obj.video_switch_stop == 1)
	{	
		output_param->cam_position = position_num;
		return 0;
	}
	for (i = 1;i < STUDENTS_PRESET_NUM;i++)  //ͳ�ƴ�������
	{
		if (Preset_Location[i] == 1)
		{
			preset_num++;             //ͳ�ƴ���������
			if (delay_max <= Preset_Delay[i])
			{
				delay_max = Preset_Delay[i];   //����ʱ���ֵ
			}

			
			line_info.start_x =input_param->dynamic_param.trigger_info[i-1].x;
			line_info.start_y=input_param->dynamic_param.trigger_info[i-1].y;
			
			line_info.end_x=line_info.start_x+input_param->dynamic_param.trigger_info[i-1].width;
			line_info.end_y=line_info.start_y+input_param->dynamic_param.trigger_info[i-1].height;
			
			yuv_value.y_value = 255;
			yuv_value.cb_value	= 0;
			yuv_value.cr_value	= 255;
			fill_rectangle(inst,output_buf, &yuv_value, &line_info);
		}
	}
	if (preset_num == 0)
	{
		position_num = 42;
		output_param->cam_position = position_num;
		return 0;
	}
#if 0
	if (preset_num == 1)
	{
		for (i = 1;i < STUDENTS_PRESET_NUM;i++)
		{
			if (Preset_Location[i] == 1)
			{
				position_num = i;
				output_param->cam_position = position_num;
				return 0;
			}
		} 
	}
	if(preset_num > 1)
	{
		position_num = 42;
		output_param->cam_position = position_num;
		return 0;
	}
#else

#if 0
	if (preset_num > 0)
	{
		
		if (delay_max > input_param->dynamic_param.reset_time - 3)
		{
			//position_num = 42;
			//output_param->cam_position = position_num;
			return 0;
		}
	}
#endif
    
	if (preset_num == 1)
	{
		for (i = 1;i < STUDENTS_PRESET_NUM;i++)
		{
			if (Preset_Location[i] == 1)
			{
				output_param->cmd_type = 1;
				position_num = i;
				output_param->cam_position = position_num;
				return 0;
			}
		} 
	}
	if (preset_num > 1)
	{
		output_param->cmd_type = 1;
		position_num = 42;
		temp = 0;
		if (Preset_Location[1] == 1)
			temp++;
		if (Preset_Location[2] == 1)
			temp++;
		if (Preset_Location[3] == 1)
			temp++;
		if (Preset_Location[8] == 1)
			temp++;
		if (Preset_Location[9] == 1)
			temp++;
		if (Preset_Location[10] == 1)
			temp++;  
		if (Preset_Location[15] == 1)
			temp++;
		if (Preset_Location[16] == 1)
			temp++;
		if (Preset_Location[17] == 1)
			temp++;
		if (temp ==  preset_num)
		{
			position_num = 36;
			output_param->cam_position = position_num;
			return 0;
		}   

		temp = 0;
		if (Preset_Location[3] == 1)
			temp++;
		if (Preset_Location[4] == 1)
			temp++;
		if (Preset_Location[5] == 1)
			temp++;
		if (Preset_Location[10] == 1)
			temp++;
		if (Preset_Location[11] == 1)
			temp++;
		if (Preset_Location[12] == 1)
			temp++;  
		if (Preset_Location[17] == 1)
			temp++;
		if (Preset_Location[18] == 1)
			temp++;
		if (Preset_Location[19] == 1)
			temp++; 
		if (temp ==  preset_num)
		{
			position_num = 37;
			output_param->cam_position = position_num;
			return 0;
		}        

		temp = 0;         
		if (Preset_Location[5] == 1)
			temp++;
		if (Preset_Location[6] == 1)
			temp++;
		if (Preset_Location[7] == 1)
			temp++;
		if (Preset_Location[12] == 1)
			temp++;
		if (Preset_Location[13] == 1)
			temp++;
		if (Preset_Location[14] == 1)
			temp++;  
		if (Preset_Location[19] == 1)
			temp++;
		if (Preset_Location[20] == 1)
			temp++;
		if (Preset_Location[21] == 1)
			temp++; 
		if (temp ==  preset_num)
		{
			position_num = 38;
			output_param->cam_position = position_num;
			return 0;
		}    

		temp = 0;   
		if (Preset_Location[15] == 1)
			temp++;
		if (Preset_Location[16] == 1)
			temp++;
		if (Preset_Location[17] == 1)
			temp++;
		if (Preset_Location[22] == 1)
			temp++;
		if (Preset_Location[23] == 1)
			temp++;
		if (Preset_Location[24] == 1)
			temp++;  
		if (Preset_Location[29] == 1)
			temp++;
		if (Preset_Location[30] == 1)
			temp++;
		if (Preset_Location[31] == 1)
			temp++; 
		if (temp ==  preset_num)
		{
			position_num = 39;
			output_param->cam_position = position_num;
			return 0;
		}      

		temp = 0;
		if (Preset_Location[17] == 1)
			temp++;
		if (Preset_Location[18] == 1)
			temp++;
		if (Preset_Location[19] == 1)
			temp++;
		if (Preset_Location[24] == 1)
			temp++;
		if (Preset_Location[25] == 1)
			temp++;
		if (Preset_Location[26] == 1)
			temp++;  
		if (Preset_Location[31] == 1)
			temp++;
		if (Preset_Location[32] == 1)
			temp++;
		if (Preset_Location[33] == 1)
			temp++; 
		if (temp ==  preset_num)
		{
			position_num = 40;
			output_param->cam_position = position_num;
			return 0;
		}  

		temp = 0;
		if (Preset_Location[19] == 1)
			temp++;
		if (Preset_Location[20] == 1)
			temp++;
		if (Preset_Location[21] == 1)
			temp++;
		if (Preset_Location[26] == 1)
			temp++;
		if (Preset_Location[27] == 1)
			temp++;
		if (Preset_Location[28] == 1)
			temp++;  
		if (Preset_Location[33] == 1)
			temp++;
		if (Preset_Location[34] == 1)
			temp++;
		if (Preset_Location[35] == 1)
			temp++; 
		if (temp ==  preset_num)
		{
			position_num = 41;
			output_param->cam_position = position_num;
			return 0;
		}  
	}
#endif		
	output_param->cam_position = position_num;
	return 0;
}
static int32_t	OnlySwitch_2stu_send_code(StuTrackHand *inst, int8_t *output_buf,StuITRACK_OutArgs *output_param)
{
	StuITRACK_Params *input_param=&(inst->input_param);
	int16_t *Preset_Location=inst->t_stu_global_obj.Preset_Location;
	int16_t *Preset_Delay=inst->t_stu_global_obj.Preset_Delay;
	int16_t	i,temp,delay_max=0;
	int16_t position_num=42;
	int16_t preset_num=0;
	int16_t delay_max_no=42;
	line_info_t 		line_info	= {0};
	students_yuv_value_info_t 	yuv_value	= {0};
	output_param->cmd_type = 0;
	if (inst->t_stu_global_obj.video_switch_stop == 1)
	{	
		output_param->cam_position = position_num;
		return 0;
	}
	for (i = 1;i < STUDENTS_PRESET_NUM;i++)  //ͳ�ƴ�������
	{
		if (Preset_Location[i] == 1)
		{
			preset_num++;             //ͳ�ƴ���������
			if (delay_max <= Preset_Delay[i])
			{
				delay_max = Preset_Delay[i];   //����ʱ���ֵ
				delay_max_no=i;
			}
			line_info.start_x =input_param->dynamic_param.trigger_info[i-1].x;
			line_info.start_y=input_param->dynamic_param.trigger_info[i-1].y;
			
			line_info.end_x=line_info.start_x+input_param->dynamic_param.trigger_info[i-1].width;
			line_info.end_y=line_info.start_y+input_param->dynamic_param.trigger_info[i-1].height;
			
			yuv_value.y_value = 255;
			yuv_value.cb_value	= 0;
			yuv_value.cr_value	= 255;
			fill_rectangle(inst,output_buf, &yuv_value, &line_info);
		}
	}
	if (preset_num == 0)
	{
		output_param->cam_position = position_num;
		return 0;
	}
	
	if (preset_num > 1)
	{//��Ŀ������ֱ���л�����λ����ͷ
		output_param->cam_position = 1;
	}
	else
	{
		output_param->cam_position = delay_max_no;
	}
	output_param->cmd_type = 1;
	return 0;
}
	

static int32_t	OnlySwitch_1stu_send_code(StuTrackHand *inst, int8_t *output_buf,StuITRACK_OutArgs *output_param)
{
	StuITRACK_Params *input_param=&(inst->input_param);
	int16_t *Preset_Location=inst->t_stu_global_obj.Preset_Location;
	int16_t *Preset_Delay=inst->t_stu_global_obj.Preset_Delay;
	int16_t *Last_Location=inst->t_stu_global_obj.Last_Location;
	int16_t	i,temp,delay_max=0;
	int16_t position_num=42;
	int16_t preset_num=0;
	int16_t delay_max_no=42;
	int16_t nNewLocationFlag=0;
	line_info_t 		line_info	= {0};
	students_yuv_value_info_t 	yuv_value	= {0};
	if (inst->t_stu_global_obj.video_switch_stop == 1)
	{	
		output_param->cam_position = position_num;
		output_param->cmd_type = 0;
		return 0;
	}
	
	for (i = 1;i < STUDENTS_PRESET_NUM;i++)   //ͳ�ƴ�������
	{
		if (Preset_Location[i] == 1)
		{
			preset_num++;              //ͳ�ƴ���������
			if (delay_max <= Preset_Delay[i])
			{
				delay_max = Preset_Delay[i];    //����ʱ���ֵ
				delay_max_no=i;
			}
			line_info.start_x =input_param->dynamic_param.trigger_info[i-1].x;
			line_info.start_y=input_param->dynamic_param.trigger_info[i-1].y;
			
			line_info.end_x=line_info.start_x+input_param->dynamic_param.trigger_info[i-1].width;
			line_info.end_y=line_info.start_y+input_param->dynamic_param.trigger_info[i-1].height;
			
			yuv_value.y_value = 255;
			yuv_value.cb_value	= 0;
			yuv_value.cr_value	= 255;
			fill_rectangle(inst,output_buf, &yuv_value, &line_info);

			if(Last_Location[i] == 0)
			{
				nNewLocationFlag=1;
			}
		}	
		Last_Location[i]=Preset_Location[i];
	}
	if (preset_num == 0)
	{
		output_param->cam_position = position_num;
		output_param->cmd_type = 0;
		return 0;
	}
	if(nNewLocationFlag==1)
	{//���µĴ���
		output_param->cam_position = delay_max_no;
		output_param->cmd_type = 1;
	}
	else
	{//û���µĴ���
		output_param->cam_position = delay_max_no;
		output_param->cmd_type = 2;
	}
	return 0;
}
	
int32_t track_students_init(StuTrackHand *inst)
{
	StuITRACK_Params *input_param=&(inst->input_param);
	//if(RE_START == input_param->dynamic_param.reset_level)
	{
		students_area_check(inst);
	}
	//else if(RE_INIT == input_param->dynamic_param.reset_level)
	//{
	//}
	memset(&(inst->t_stu_global_obj.t_check_point),0,sizeof(TStuCheckPoint)*STU_MAX_CHECKNUM);
	memset(inst->t_stu_global_obj.Preset_Location, 0, sizeof(int16_t) * STUDENTS_PRESET_NUM);
	memset(inst->t_stu_global_obj.Preset_Delay, 0, sizeof(int16_t) * STUDENTS_PRESET_NUM);
	memset(inst->t_stu_global_obj.Last_Location, 0, sizeof(int16_t) * STUDENTS_PRESET_NUM);
	memset(&(inst->t_stu_global_obj.tMoveInfo),0,sizeof(t_mvdet_info));
	memset(&(inst->t_stu_global_obj.nMvXPos[0][0]),0,sizeof(int32_t)*STUDENTS_MULTITARGET_NUM*9);
	memset(&(inst->t_stu_global_obj.nMvYPos[0][0]),0,sizeof(int32_t)*STUDENTS_MULTITARGET_NUM*9);
	inst->t_stu_global_obj.students_dst_count[0]=inst->t_stu_global_obj.students_time_count[0];
	inst->t_stu_global_obj.students_dst_count[1]=inst->t_stu_global_obj.students_time_count[1];
	inst->t_stu_global_obj.students_dst_count[2]=inst->t_stu_global_obj.students_time_count[2];
	inst->t_stu_global_obj.students_dst_count[3]=inst->t_stu_global_obj.students_time_count[3];
	inst->t_stu_global_obj.students_dst_count[4]=inst->t_stu_global_obj.students_time_count[4];
	inst->t_stu_global_obj.students_dst_count[5]=inst->t_stu_global_obj.students_time_count[5];
	inst->t_stu_global_obj.students_dst_count[6]=inst->t_stu_global_obj.students_time_count[6];
	inst->t_stu_global_obj.students_dst_count[7]=inst->t_stu_global_obj.students_time_count[7];
	inst->t_stu_global_obj.students_dst_count[8]=inst->t_stu_global_obj.students_time_count[8];

	inst->t_stu_global_obj.nSrcBufWidth=MAXVIDEOWIDTH;
	inst->t_stu_global_obj.nSrcBufHeight=MAXVIDEOHEIGHT;
	inst->t_stu_global_obj.video_switch_stop	= 0;
	inst->t_stu_global_obj.nFrameNum=0;
	inst->t_stu_global_obj.nCopyPos=0;
	inst->t_stu_global_obj.nCopy10FrameNum=0;
	inst->t_stu_global_obj.nCopyFramNum  = 0;
	return 0;
}
static int32_t data_copy10(StuTrackHand *inst, int8_t *output_buf)
{
	StuITRACK_Params *input_param=&(inst->input_param);
	int16_t			pic_width		= 0;
	int16_t			pic_height		= 0;
	int16_t			video_width		= input_param->static_param.video_width;
	int16_t			video_height	= input_param->static_param.video_height;
	int16_t			x_offset		= input_param->dynamic_param.x_offset;
	int16_t			y_offset		= input_param->dynamic_param.y_offset;
	int32_t			i 				= 0;
	int32_t			j 				= 0;
	int16_t			sens			= input_param->dynamic_param.sens;

	int k=0;
	uint8_t **students_src_y_buf=inst->t_stu_global_obj.students_src_y_buf;   //������ź�Ybuffer����,ÿ10֡���һ��ԭʼ���� 
	int nSrcBufWidth=inst->t_stu_global_obj.nSrcBufWidth;
	pic_width = video_width / x_offset;
	pic_height = video_height / y_offset;
	if(inst->t_stu_global_obj.nCopy10FrameNum==0)
	{
		for(i = 0; i < pic_height; i++)
		{
			for(j = 0; j < pic_width; j++)
			{
		
				*(students_src_y_buf[inst->t_stu_global_obj.nCopyPos]+k) = *((uint8_t *)output_buf + i*y_offset*nSrcBufWidth + j*x_offset);
		
				k++;
			}
		}
		inst->t_stu_global_obj.nCopyPos=!inst->t_stu_global_obj.nCopyPos;
	}
	inst->t_stu_global_obj.nCopy10FrameNum++;
	if(inst->t_stu_global_obj.nCopy10FrameNum==10)
	{
		inst->t_stu_global_obj.nCopy10FrameNum=0;
	}
	
	for(i = 0; i < pic_height; i++)
	{
		for(j = 0; j < pic_width; j++)
		{

			if(abs(*((uint8_t *)output_buf + i*y_offset*nSrcBufWidth + j*x_offset)-*(students_src_y_buf[inst->t_stu_global_obj.nCopyPos]+i*pic_width+j))>sens)
			{
				//output_buf[i * y_offset*nSrcBufWidth+j*x_offset]=0;
				//output_buf[nSrcBufWidth*nSrcBufHeight+i * y_offset/2*nSrcBufWidth+j*x_offset/2*2]=0;
				//output_buf[nSrcBufWidth*nSrcBufHeight+i * y_offset/2*nSrcBufWidth+j*x_offset/2*2+1]=255;
	
			}
		}
	}

	return 0;
}
static Int32 Draw(StuTrackHand *inst,int8_t *addr)
{
	
	char *p = addr;
	char *y	= inst->t_stu_global_obj.students_dst_y_buf[0];

	int height= inst->input_param.static_param.pic_height;
	int width= inst->input_param.static_param.pic_width;

	//memset(pChObj->src_y_buf,0x0,160*90);
	//memset(pChObj->src_cb_buf,0x80,160*90/4);
	//memset(pChObj->src_cr_buf,0x80,160*90/4);

	
	int i = 0;
	for(i = 0; i<height; i++)
	{
		memcpy(p,y,width);
		p = p + inst->t_stu_global_obj.nSrcBufWidth;
		y = y + width;
	}

	p = p + inst->t_stu_global_obj.nSrcBufWidth*inst->t_stu_global_obj.nSrcBufHeight;
	int j = 0;
	for(i = 0; i<inst->t_stu_global_obj.nSrcBufHeight/2; i++)
	{
		for(j = 0; j <inst->t_stu_global_obj.nSrcBufWidth/2;j++)
		{
			if((i< height) && (j < width))
			{
				*p = 128;
				p++;
				*p = 128;
				// u++;
				// v++;
				 p++;
			}
			else
			{

				p++;
				p++;
			}
		}
		
	}

	return;
}

int32_t	track_students_process(StuTrackHand *inst, int8_t *output_buf, StuITRACK_OutArgs *output_param)
{
	StuITRACK_Params *input_param;
	unsigned int clock_start=0;
	if((NULL==inst)||(NULL==output_buf)||(NULL==output_param))
	{
		return -1;
	}

	int bufferlen = inst->t_stu_global_obj.nSrcBufWidth*inst->t_stu_global_obj.nSrcBufHeight*2;
	
	input_param=&(inst->input_param);
	//inst->t_stu_global_obj.nFrameNum++; 
	if(inst->t_stu_global_obj.tMoveInfo.nMoveDetectFlag==1)
	{
		inst->t_stu_global_obj.tMoveInfo.nMoveDetectDelay--;
		if(inst->t_stu_global_obj.tMoveInfo.nMoveDetectDelay==0)
		{
			inst->t_stu_global_obj.tMoveInfo.nMoveDetectFlag=0;
			inst->t_stu_global_obj.tMoveInfo.nMoveDetectNum=0;
		}
	}

	
 //   counterVal=CSL_tscRead(); 
	//clock_start=CLK_gethtime();
	memset(output_param, 0, sizeof(StuITRACK_OutArgs));
	
	//����ͼ��Y���� ÿ֡����
	data_copy(inst, output_buf);

	//����ͼ��Y���� 10֡����
	data_copy10(inst, output_buf);

	//Draw(inst, output_buf);

	//���㱣��仯��
	img_compare_y(inst,output_buf);

	//���㱣��ͷ���ı仯��
	img_compare_y0(inst,output_buf);
	
	//clock_copy+=(CLK_gethtime()-clock_start);
	img_compare_y1(inst, output_buf);

	
	img_process_y(inst, output_buf, output_param);
	delay_dec(inst);
	MoveDetect(inst, output_buf, output_param);
	if(0==input_param->dynamic_param.nTrackSwitchType)
	{
		send_code(inst, output_buf, output_param);
	}
	else if(1==input_param->dynamic_param.nTrackSwitchType)
	{
		if(100==input_param->dynamic_param.nStrategyNo)
		{
			OnlySwitch_1stu_send_code(inst, output_buf, output_param);
		}
		else if(101==input_param->dynamic_param.nStrategyNo)
		{//ѧ��һǰһ��,���Ŀ����������ǰ��,��������ǰ��,ż����������
			OnlySwitch_2stu_send_code(inst, output_buf, output_param);
		}
	}
	if(DRAW_TRIGGER_TRACK == input_param->dynamic_param.message)
	{
		draw_track_rect(inst,output_buf);	
	}
	

	Cache_wbInv(output_buf,bufferlen,Cache_Type_ALL,TRUE);
	return 0;
}

