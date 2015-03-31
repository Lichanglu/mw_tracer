/**
*
*/

#include "track_students_right_side.h"

/**
* @ ���ƻ��ߵ�����
*/
typedef enum _draw_line_type
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
}draw_line_type_e;


/**
* @ ��������ͷ��ʽ
*/
typedef enum _track_mode_enum
{
	CLASS_VIEW_FORWARD = 0,		//��ǰȡ��
	CLASS_VIEW,					//����ȡ��
	MAX_NUM_MODE
}track_mode_e;


#define	INTER_PROG_RATIO	(1)		//1//1��ʾ����,2��ʾ����
#define YUV422_OR_YUV420	(2)		//1//1��ʾ422,2��ʾ420



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
* @ ��������: track_students_right_side_area_check()
* @	��������: �趨��������
* @ �������: input_param -- Ӧ�ò����õĲ���
* @ �������: ��
* @ ����ֵ:   ��
*/
void track_students_right_side_area_check(StuSideTrackHand *inst)
{
	StuSideITRACK_Params *input_param;
	uint8_t *Area_vertex; 
    int32_t	 	k				= 0;	
	int16_t		video_width;
	int16_t		video_height;

	int32_t		m				= 0;
	int32_t		n				= 0;
	vertex_t	point_y_value	= {0};
	if(NULL==inst)
	{
		return;
	}
	input_param=&(inst->input_param);
	Area_vertex=inst->t_global_obj.Area_vertex; 
	video_width		= input_param->static_param.video_width;
	video_height	= input_param->static_param.video_height;
	
	for (k = 0; k < video_width * video_height; k++)  
	{
		m = k%video_width;     //��
		n = k/video_width;    //��

		point_y_value.x = m;
		point_y_value.y = n;

		if (pt_in_poly ((const vertex_t *)input_param->dynamic_param.track_point, input_param->dynamic_param.track_point_num,  
			&point_y_value)==1)
		{
			*((Uint8 *)Area_vertex + n * video_width + m) = 255;
		}
		else
		{
			*((Uint8 *)Area_vertex + n * video_width + m) = 0;
		}
	}
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
static int32_t	draw_point(StuSideTrackHand *inst,int8_t *buffer, yuv_value_info_t *yuv_value,
	int32_t	point_x, int32_t point_y)
{
	int nSrcBufWidth=inst->t_global_obj.nSrcBufWidth;
	int nSrcBufHeight=inst->t_global_obj.nSrcBufHeight;
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
static int32_t	draw_line(StuSideTrackHand *inst,int8_t *buffer,yuv_value_info_t *yuv_value,
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
	
    if (fabs(dx) > fabs(dy))
	{
		steps = fabs(dx);
	}
    else
	{
		steps = fabs(dy);
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
	return 0;
}

/**
* @	��������: draw_rectangle()
* @	��������: �����ο��ڴ���Ҫ�ǻ��㣬�����ڴ���ʱ���õ����Ź����ͼ�񣬶����ߵ�
* @ 		  buffer������Դͼ���ϴ����ģ�����Ҫ�ѵ㻭��һ��С��
* @	�������: buffer -- ��ͼ��buffer
* @			  yuv_value -- ���ߵ�Y��Cb��Cr����ɫֵ
* @			  line_info -- ���ߵ���ʼ������
* @ 		  static_param -- �ϲ㴫�����ľ�̬����
* @ �������: ��
* @ ����ֵ:	  ��
*/
static void draw_rectangle(StuSideTrackHand *inst,int8_t *buffer,yuv_value_info_t *yuv_value,
	line_info_t *line_info)
{
    int32_t i,j;
	int32_t left_up_x 		= line_info->start_x;
	int32_t left_up_y 		= line_info->start_y;
	int32_t right_down_x 	= line_info->end_x;
	int32_t right_down_y 	= line_info->end_y;

    
	//��������
    for(i=left_up_y;i<left_up_y+1;i++)  
	{
	    for(j=left_up_x;j<right_down_x;j++) 
	    {
	    	draw_point(inst,buffer,yuv_value,j,i);
	    }
	}	

	//��������
    for(i=right_down_y;i<right_down_y+1;i++)
	{
	    for(j=left_up_x;j<right_down_x;j++) 
	    {
	    	draw_point(inst,buffer,yuv_value,j,i);
	    }
	}

	//�������
    for(i=left_up_y;i<right_down_y;i++)
	{
	    for(j=left_up_x;j<left_up_x+1;j++) 
	    {
	    	draw_point(inst,buffer,yuv_value,j,i);
	    }
	}

	//���ұ���
    for(i=left_up_y;i<right_down_y+1;i++)
	{
	    for(j=right_down_x;j<right_down_x+1;j++) 
	    {
	    	draw_point(inst,buffer,yuv_value,j,i);
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
static void draw_track_rect(StuSideTrackHand *inst,int8_t *buffer)
{
	StuSideITRACK_Params *input_param=&(inst->input_param);
	line_info_t 		line_info	= {0};
	yuv_value_info_t 	yuv_value	= {0};
	int16_t				index		= 0;
	int16_t				j			= 0;
	int16_t				point_num	= input_param->dynamic_param.track_point_num;

	yuv_value.y_value = 255;
	yuv_value.cb_value = 0;
	yuv_value.cr_value = 255;

    for(index=0; index<point_num; index++) 
	{ 
		j = (index+1) % point_num; 

		line_info.start_x 	= input_param->dynamic_param.track_point[index].x;
		line_info.start_y 	= input_param->dynamic_param.track_point[index].y/INTER_PROG_RATIO;
		line_info.end_x 	= input_param->dynamic_param.track_point[j].x;
		line_info.end_y 	= input_param->dynamic_param.track_point[j].y/INTER_PROG_RATIO;
		draw_line(inst,buffer,&yuv_value,&line_info);
	}
}

static int32_t move_check_data_copy(int8_t *output_buf, StuSideTrackHand *inst)
{
	StuSideITRACK_Params *input_param=&(inst->input_param);

	//int16_t	video_width			= input_param->static_param.video_width;
	
	//int16_t	video_height		= input_param->static_param.video_height;
	int32_t			i = 0,j		= 0;
	int32_t			a = 0;
	uint8_t *src_y_buf    = inst->t_global_obj.tmp_src_y_buf; //���Ҫ���������Դybuf����	
	//uint8_t *dst_y_buf    = inst->t_global_obj.dst_y_buf; //���ÿ�θ��ٺ�ĵ�һ֡ybuf����
	int32_t nSrcBufHeight = inst->t_global_obj.nSrcBufHeight;
	int32_t nSrcBufWidth  = inst->t_global_obj.nSrcBufWidth;

	int32_t 		left			= inst->t_global_obj.tInRect.nLeft;
	int32_t 		top				= inst->t_global_obj.tInRect.nTop;
	int32_t 		right			= inst->t_global_obj.tInRect.nRight;
	int32_t 		bottom			= inst->t_global_obj.tInRect.nBottom;
	int32_t			index           = 0;
	int32_t         len = right - left + 1;


	#if 0
	for(i = 0; i < video_height; i++)
	{
		for(j = 0; j < video_width; j++)
		{
			*(src_y_buf + a) = *(output_buf + i*nSrcBufWidth + j);
			a++;	
		}
	}
	#endif
	
	
	for(i=top; i<= bottom; i++)
	{
		memcpy(src_y_buf, output_buf + i*nSrcBufWidth + left, len);
		src_y_buf = src_y_buf + len;
	}
	return 0;
}	


int32_t check_track_point(int8_t *output_buf, StuSideTrackHand *inst)
{
	StuSideITRACK_Params 	*input_param	= &(inst->input_param);
	uint8_t 		*gray_buf		= inst->t_global_obj.gray_buf;
	int32_t	m 				= 0;
	int32_t	n				= 0;
	
	int16_t	video_width		= input_param->static_param.video_width;
	int16_t	video_height	= input_param->static_param.video_height;

	uint8_t *src_y_buf		= inst->t_global_obj.src_y_buf; //���Ҫ���������Դybuf����	
	uint8_t *dst_y_buf		= inst->t_global_obj.dst_y_buf; //���ÿ�θ��ٺ�ĵ�һ֡ybuf����

	uint8_t *Area_vertex	= inst->t_global_obj.Area_vertex;
	int32_t nSrcBufWidth  = inst->t_global_obj.nSrcBufWidth;
	yuv_value_info_t		yuv_value 	= {0};

	int32_t 		left			= inst->t_global_obj.tInRect.nLeft;
	int32_t 		top				= inst->t_global_obj.tInRect.nTop;
	int32_t 		right			= inst->t_global_obj.tInRect.nRight;
	int32_t 		bottom			= inst->t_global_obj.tInRect.nBottom;
	int32_t			index           = 0;
	int32_t         len = right - left + 1;


	int32_t	ret				= 0;

	float_t a 	= 0.0;
	float_t	b 	= 0.0;
	float_t	c	= 0.0;

	#if 0
	for(m = 0; m <= video_width; m++)
	{
		for(n = 0; n <= video_height; n++)
		{
			if (*((uint8_t *)Area_vertex + n*video_width+ m) == 0)
			{
				continue;
			}
			
			a = *((uint8_t *)src_y_buf+ video_width * n + m);
			b = *((uint8_t *)dst_y_buf+  video_width * n + m);

			//12��8�Ǹ�����ֵ����ʾ�иı䣬���д���
			if(b != 0)
			{
				c = a/b;
			}
			else
			{
				c = 1;
			}
			
			//if (c>1.2||c<0.8)
			if(c>1.3||c<0.7)
			{
				yuv_value.y_value = 255;
				yuv_value.cb_value = 0;
				yuv_value.cr_value = 255;
				draw_point(inst,output_buf,&yuv_value,m,n);
				*((uint8_t *)gray_buf  +  video_width * n + m) = 1;
				
			}	  
			else
			{
				*((uint8_t *)gray_buf  +  video_width * n + m) = 0;
			}
			
		}
	}
	#endif

	for(m = left; m <= right; m++)
	{
		for(n = top; n <= bottom; n++)
		{
			if (*((uint8_t *)Area_vertex + n*video_width+ m) == 0)
			{
				continue;
			}
			
			a = *((uint8_t *)output_buf +  nSrcBufWidth * n + m);
			//a = *((uint8_t *)src_y_buf+  len * (n - top) + (m - left));
			b = *((uint8_t *)dst_y_buf+  len * (n - top) + (m - left));

			//12��8�Ǹ�����ֵ����ʾ�иı䣬���д���
			if(b != 0)
			{
				c = a/b;
			}
			else
			{
				c = 1;
			}
			
			//if (c>1.2||c<0.8)
			if(c>1.3||c<0.7)
			{
				yuv_value.y_value = 255;
				yuv_value.cb_value = 0;
				yuv_value.cr_value = 255;
				draw_point(inst,output_buf,&yuv_value,m,n);
				*((uint8_t *)gray_buf  +  video_width * n + m) = 1;
			}	  
			else
			{
				*((uint8_t *)gray_buf  +  video_width * n + m) = 0;
			}
			
		}
	}
	
	
	return ret;
}

int32_t set_check_max_rect(StuSideTrackHand *inst)
{
	StuSideITRACK_Params 	*input_param	= &(inst->input_param);
	int32_t		index	= 0;
	int32_t		min_x	= inst->t_global_obj.nSrcBufWidth;
	int32_t		min_y	= inst->t_global_obj.nSrcBufHeight;
	int32_t		max_x	= 0;
	int32_t		max_y	= 0;

	for(index = 0; index < input_param->dynamic_param.track_point_num; index++)
	{
		if(input_param->dynamic_param.track_point[index].x < min_x)
		{
			min_x = input_param->dynamic_param.track_point[index].x;
		}

		if(input_param->dynamic_param.track_point[index].x > max_x)
		{
			max_x = input_param->dynamic_param.track_point[index].x;
		}

		if(input_param->dynamic_param.track_point[index].y < min_y)
		{
			min_y = input_param->dynamic_param.track_point[index].y;
		}

		
		if(input_param->dynamic_param.track_point[index].y > max_y)
		{
			max_y = input_param->dynamic_param.track_point[index].y;
		}
	}


	inst->t_global_obj.tInRect.nLeft 	= min_x;
	inst->t_global_obj.tInRect.nRight	= max_x;
	inst->t_global_obj.tInRect.nTop   	= min_y;
	inst->t_global_obj.tInRect.nBottom  = max_y;


	//Vps_printf("%d %d %d %d\n",min_x,max_x,min_y,max_y);
	return 0;
}

int32_t get_max_rect(StuSideITRACK_Params *pParams, T_Rect *prect)
{
	StuSideITRACK_Params 	*input_param	= pParams;
	int32_t		index	= 0;
	int32_t		min_x	= 1920;
	int32_t		min_y	= 1080;
	int32_t		max_x	= 0;
	int32_t		max_y	= 0;

	for(index = 0; index < input_param->dynamic_param.track_point_num; index++)
	{
		if(input_param->dynamic_param.track_point[index].x < min_x)
		{
			min_x = input_param->dynamic_param.track_point[index].x;
		}

		if(input_param->dynamic_param.track_point[index].x > max_x)
		{
			max_x = input_param->dynamic_param.track_point[index].x;
		}

		if(input_param->dynamic_param.track_point[index].y < min_y)
		{
			min_y = input_param->dynamic_param.track_point[index].y;
		}

		
		if(input_param->dynamic_param.track_point[index].y > max_y)
		{
			max_y = input_param->dynamic_param.track_point[index].y;
		}
	}


	prect->nLeft 	= min_x;
	prect->nRight	= max_x;
	prect->nTop   	= min_y;
	prect->nBottom  = max_y;


	//Vps_printf("%d %d %d %d\n",min_x,max_x,min_y,max_y);
	return 0;
}

//void GrayClose(uint8_t *pGray,uint8_t *pMidBuf,int32_t nWidth,T_Rect tInRect)
static int32_t gray_close(StuSideTrackHand *inst)
{
	StuSideITRACK_Params 	*input_param	= &(inst->input_param);
	uint8_t *Area_vertex	= inst->t_global_obj.Area_vertex;
	int32_t 		nWidth			= input_param->static_param.video_width;
	uint8_t 		*pGray			= inst->t_global_obj.gray_buf;
	uint8_t 		*pMidBuf		= inst->t_global_obj.mid_buf;
	int 			i				= 0;
	int 			j				= 0;
	int 			index			= 0;
	uint8_t 		pixel			= 0;
	uint8_t 		temp[9];
	int32_t 		left			= inst->t_global_obj.tInRect.nLeft;
	int32_t 		top				= inst->t_global_obj.tInRect.nTop;
	int32_t 		right			= inst->t_global_obj.tInRect.nRight;
	int32_t 		bottom			= inst->t_global_obj.tInRect.nBottom;
	
	memset(pMidBuf,0,(right-left+1)*(bottom-top+1));
	
	//����
	for(i=top+1;i<bottom;i++)
	{
		for(j=left+1;j<right;j++)
		{
			index=i*nWidth+j;

			if (*((uint8_t *)Area_vertex + i*nWidth+ j) == 0)
			{
				continue;
			}
			temp[0]=pGray[index-nWidth-1];
			temp[1]=pGray[index-nWidth];
			temp[2]=pGray[index-nWidth+1];
			temp[3]=pGray[index-1];
			temp[4]=pGray[index];
			temp[5]=pGray[index+1];
			temp[6]=pGray[index+nWidth-1];
			temp[7]=pGray[index+nWidth];
			temp[8]=pGray[index+nWidth+1];
			pixel=temp[0]|temp[1]|temp[2]|temp[3]|temp[4]|temp[5]|temp[6]|temp[7]|temp[8];
			pMidBuf[index]=pixel;
		}
	}
	#if 0
	//����
	for(i=top+1;i<bottom;i++)
	{
		for(j=left+1;j<right;j++)
		{
			index=i*nWidth+j;
			if (*((uint8_t *)Area_vertex + i*nWidth+ j) == 0)
			{
				continue;
			}
			
			temp[0]=pMidBuf[index-nWidth-1];
			temp[1]=pMidBuf[index-nWidth];
			temp[2]=pMidBuf[index-nWidth+1];
			temp[3]=pMidBuf[index-1];
			temp[4]=pMidBuf[index];
			temp[5]=pMidBuf[index+1];
			temp[6]=pMidBuf[index+nWidth-1];
			temp[7]=pMidBuf[index+nWidth];
			temp[8]=pMidBuf[index+nWidth+1];
			pixel=temp[0]|temp[1]|temp[2]|temp[3]|temp[4]|temp[5]|temp[6]|temp[7]|temp[8];
			pGray[index]=pixel;
		}
	}
	
	//��ʴ
	for(i=top+1;i<bottom;i++)
	{
		for(j=left+1;j<right;j++)
		{
			index=i*nWidth+j;
			if (*((uint8_t *)Area_vertex + i*nWidth+ j) == 0)
			{
				continue;
			}
			
			temp[0]=pGray[index-nWidth-1];
			temp[1]=pGray[index-nWidth];
			temp[2]=pGray[index-nWidth+1];
			temp[3]=pGray[index-1];
			temp[4]=pGray[index];
			temp[5]=pGray[index+1];
			temp[6]=pGray[index+nWidth-1];
			temp[7]=pGray[index+nWidth];
			temp[8]=pGray[index+nWidth+1];
			pixel=temp[0]&temp[1]&temp[2]&temp[3]&temp[4]&temp[5]&temp[6]&temp[7]&temp[8];
			pMidBuf[index]=pixel;
		}
	}
	#endif
	//��ʴ
	for(i=top+1;i<bottom;i++)
	{
		for(j=left+1;j<right;j++)
		{
			index=i*nWidth+j;
			if (*((uint8_t *)Area_vertex + i*nWidth+ j) == 0)
			{
				continue;
			}
			
			temp[0]=pMidBuf[index-nWidth-1];
			temp[1]=pMidBuf[index-nWidth];
			temp[2]=pMidBuf[index-nWidth+1];
			temp[3]=pMidBuf[index-1];
			temp[4]=pMidBuf[index];
			temp[5]=pMidBuf[index+1];
			temp[6]=pMidBuf[index+nWidth-1];
			temp[7]=pMidBuf[index+nWidth];
			temp[8]=pMidBuf[index+nWidth+1];
			pixel=temp[0]&temp[1]&temp[2]&temp[3]&temp[4]&temp[5]&temp[6]&temp[7]&temp[8];
			pGray[index]=pixel;
		}
	}
}

static void connect(int tab[],int i, int j)
{
	int mid;
	while(1)
	{
		if(i==j)
		{
			return;
		}
		if(tab[i]==i)
		{
			tab[i]=j;
		}
		if(tab[i]>j)
		{
			i=tab[i];
		}
		else if(tab[i]<j)
		{
			mid=i;
			i=j;
			j=tab[mid];
		}
		else
		{
			return;
		}
	}
}
static int Mark_value(int tab[],int i)
{
	while(1)
	{
		if(tab[i]==i)
		{
			return i;
		}
		else
		{
			i=tab[i];
		}
	}
}

static void MergerTable(int tab[],int mark_num)
{
	int j=0,temp=0;
	for(j=mark_num;j>0;j--)
	{
		temp=Mark_value(tab,j);
		tab[j]=temp;
		
	}
}

static int32_t find_block_kernel(StuSideTrackHand *inst)
{
	StuSideITRACK_Params 	*input_param			= &(inst->input_param);
	uint8_t 		*Area_vertex			= inst->t_global_obj.Area_vertex;
	uint8_t 		*pGray					= inst->t_global_obj.gray_buf;
	uint32_t 		*pMidBuf				= (int32_t *)inst->t_global_obj.mid_buf;
	uint16_t 		*pMarkedBuf				= inst->t_global_obj.marked_buf;
	int32_t 		nWidth					= input_param->static_param.video_width;
	int32_t 		nHeight					= input_param->static_param.video_height;
	T_Rect 			*tOutRects				= (inst->t_global_obj.g_out_rect);
	
	int32_t 		nObjectCount			= 0;
	T_Rect 			rect					= {SHORT_MAX,SHORT_MAX,0,0};
	int32_t 		left			= inst->t_global_obj.tInRect.nLeft;
	int32_t 		top				= inst->t_global_obj.tInRect.nTop;
	int32_t 		right			= inst->t_global_obj.tInRect.nRight;
	int32_t 		bottom			= inst->t_global_obj.tInRect.nBottom;
	int32_t 		nIndex					= 0;
	T_Rect 			*tObjectRects;
	int32_t 		*tab,*marks;

	int32_t 		mark_min				= 0;
	int32_t 		mark_num				= 0;
	int32_t 		mark_count				= 0;
	int32_t 		rect_count				= 0;
	int32_t			bFound					= 0;
	int32_t 		mark_new				= -1;
	int32_t 		mark_old				= -1;
	int32_t 		nRectWidth				= 0;
	int32_t 		nRectHeight				= 0;
	
	int32_t 		x1=0,x2=0;
	int32_t 		xL=0,xR=0;
	int32_t 		i=0,j=0,k=0,n=0;
	
	if(pGray == NULL)
	{
		return -1;
	}
	
	tab				= pMidBuf;
	marks			= tab+4096;
	tObjectRects	= (T_Rect*)(marks+1024);
	
	memset(tab,0,4096*4);
	memset(marks,0,1024*4);
	nIndex=top*nWidth;

	//Vps_printf("%d %d %d %d\n",left,right,top,bottom);
	for(i=top;i<=bottom;i++)
	{
		n=0;
		for(j=left;j<=right;j++)
		{

			if (*((uint8_t *)Area_vertex + i*nWidth+ j) == 0)
			{
				continue;
			}
			
			pMarkedBuf[nIndex+j]=0;
			if(pGray[nIndex+j]!=0)
			{
				if(0==n)
				{
					x1=j;
					x2=j;
				}
				else
				{
					x2=j;
				}
				n++;
			}
			if((pGray[nIndex+j]==0)||(j==right))
			{
				if(n>0)
				{
				
					bFound=0;
					mark_min=SHORT_MAX;
					if(i>top)
					{
						mark_new=-1;
						mark_old=-1;
						//xL=(x1-1);
						xL=(x1);
						xR=(x2+1);
						if(xL<left)
						{
							xL=left;
						}
						
						if(xR>right)
						{
							xR=right;
						}
						mark_count=0;
						for(k=xL;k<=xR;k++)
						{
							if(pGray[nIndex-nWidth+k]!=0)
							{
								bFound=1;
								mark_new=pMarkedBuf[nIndex-nWidth+k];
								if(mark_new!=mark_old)
								{
									mark_old=mark_new;
									marks[mark_count]=mark_new;
									mark_count++;
								}
								if(mark_new<mark_min)
								{
									mark_min=mark_new;
								}
							}
						}
						for(k=0;k<mark_count;k++)
						{
							connect(tab,marks[k],mark_min);
						}
					}
					if(!bFound)
					{
						mark_num++;
						tab[mark_num]=mark_num;
						for(k=x1;k<=x2;k++)
						{

							pMarkedBuf[nIndex+k]=mark_num;
						}
					}
					else
					{
						for(k=x1;k<=x2;k++)
						{

							pMarkedBuf[nIndex+k]=mark_min;
						}
					}
				}
				n=0;
			}
		}
		nIndex+=nWidth;
	}
	MergerTable(tab,mark_num);
	for(i=0;i<MAXRECTNUM;i++)
	{
		tObjectRects[i].nLeft=SHORT_MAX;
		tObjectRects[i].nTop=SHORT_MAX;
		tObjectRects[i].nRight=0;
		tObjectRects[i].nBottom=0;
		tObjectRects[i].nFillSize=0;
	}

	nIndex=top*nWidth;
	for(i=top;i<=bottom;i++)
	{
		//nIndex=i*nWidth+left;
		for(j=left;j<=right;j++)
		{
			if (*((uint8_t *)Area_vertex + i*nWidth+ j) == 0)
			{
				continue;
			}
			
			k=tab[pMarkedBuf[nIndex+j]];
			if((0==k) || (k >= MAXRECTNUM))
			{
				//nIndex++;
				continue;
			}
			rect.nLeft=SHORT_MAX;
			rect.nTop=SHORT_MAX;
			rect.nRight=0;
			rect.nBottom=0;
			pMarkedBuf[nIndex+j]=k;
			if(k>rect_count)
			{
				rect_count=k;
			}
			rect=tObjectRects[k];
			rect.nFillSize++;
			if(i<rect.nTop)
			{
				rect.nTop=i;
			}
			if(i>rect.nBottom)
			{
				rect.nBottom=i;
			}
			if(j<rect.nLeft)
			{
				rect.nLeft=j;
			}
			if(j>rect.nRight)
			{
				rect.nRight=j;
			}
			tObjectRects[k]=rect;
			//nIndex++;
		}

		nIndex+=nWidth;
	}
	nObjectCount=0;
	for(i=0;i<=rect_count;i++)
	{
		rect=tObjectRects[i];
		rect.nMarkVal=i;
		//�ų��Ǿ�������
		if(SHORT_MAX==rect.nLeft)
		{
			continue;
		}
		#if 1
		//�ų������������ľ���
		nRectWidth=rect.nRight-rect.nLeft+1;
		nRectHeight=rect.nBottom-rect.nTop+1;

		//Vps_printf("trigger_sum = %d rect.nFillSize = %d %d %d %d %d\n",input_param->dynamic_param.trigger_sum,rect.nFillSize,nRectWidth,nRectHeight,rect_count,mark_num);
		if((rect.nFillSize<input_param->dynamic_param.trigger_sum)
			||nRectWidth<MINRECTWIDTH||nRectHeight<MINRECTHEIGHT)
		{
			for(k=rect.nTop;k<=rect.nBottom;k++)
			{
				for(j=rect.nLeft;j<=rect.nRight;j++)
				{
					if(pMarkedBuf[k*nWidth+j]==i)
					{
						pGray[k*nWidth+j]=0;
						pMarkedBuf[k*nWidth+j]=0;
					}
				}

			}
			continue;
		}
		#endif
		tOutRects[nObjectCount]=rect;
		nObjectCount++;
	}
	return nObjectCount;
}


static Int32 Draw(StuSideTrackHand *inst,int8_t *input_buf, int8_t *output_buf)
{
	
	char *p = output_buf;
	char *y	= input_buf;


	int height = inst->input_param.static_param.video_height;
	int width  = inst->input_param.static_param.video_width;

	
	int i = 0;
	for(i = 0; i<height; i++)
	{
		memcpy(p,y,width);
		p = p + inst->t_global_obj.nSrcBufWidth;
		y = y + width;
	}


	return;
}

static Int32 Draw_1(StuSideTrackHand *inst,int8_t *input_buf, int8_t *output_buf)
{
	
	char *p = output_buf;
	char *y	= input_buf;


	int height = inst->input_param.static_param.video_height;
	int width  = inst->input_param.static_param.video_width;

	int32_t 		left			= inst->t_global_obj.tInRect.nLeft;
	int32_t 		top				= inst->t_global_obj.tInRect.nTop;
	int32_t 		right			= inst->t_global_obj.tInRect.nRight;
	int32_t 		bottom			= inst->t_global_obj.tInRect.nBottom;
	int32_t          len = right - left + 1; 
	int i = 0;


	//Vps_printf("%d %d %d %d\n",left,right,top,bottom);
	//memset(y,0x0,len*(bottom - top));
	for(i=top; i< bottom; i++)
	{
		memcpy(p + inst->t_global_obj.nSrcBufWidth*i + left, y + (i - top)*len, len);
	
	}

	p = output_buf + inst->t_global_obj.nSrcBufWidth*inst->t_global_obj.nSrcBufHeight;
	
	for(i=top; i< bottom/2; i++)
	{
	//	memset( p + inst->t_global_obj.nSrcBufWidth*i + left,0x0,len);
	}
	
	
	return;
}

int32_t	track_students_right_side_process(StuSideTrackHand *inst,int8_t *input_buf, int8_t *output_buf, 
	StuSideITRACK_OutArgs *output_param)
{
	StuSideITRACK_Params 	*input_param 	= &(inst->input_param);

	yuv_value_info_t		yuv_value 	= {0};
	line_info_t 			line_info	= {0};
	int32_t					index		= 0;

	int32_t					min_x		= 704;
	int32_t					max_x		= 0;
	uint8_t                 *tmpbuffer = NULL;
//	UInt32 time0 = Utils_getCurTimeInMsec();;

	if((NULL == inst)||(NULL == output_buf)||(NULL == output_param) || (NULL == input_buf))
	{
		return -1;
	}

	int32_t bufferlen = inst->t_global_obj.nSrcBufWidth * inst->t_global_obj.nSrcBufHeight*2;
	int32_t len  = (inst->t_global_obj.tInRect.nRight - inst->t_global_obj.tInRect.nLeft + 1)*(inst->t_global_obj.tInRect.nBottom - inst->t_global_obj.tInRect.nTop + 1);
		
	//Vps_printf("wxd %dx%d %dx%d\n",inst->t_global_obj.nSrcBufWidth,inst->t_global_obj.nSrcBufHeight,input_param->static_param.video_width,input_param->static_param.video_height);
	
	inst->t_global_obj.frame_num ++;

	
	if(CLASS_VIEW == inst->input_param.dynamic_param.track_mode)
	{
		inst->t_global_obj.dst_y_buf = inst->t_global_obj.src_y_buf;
		//memcpy(inst->t_global_obj.dst_y_buf, inst->t_global_obj.src_y_buf, len);
	}
	else
	{
		inst->t_global_obj.dst_y_buf = input_buf;
		//memcpy(inst->t_global_obj.dst_y_buf, input_buf, len);

	}

//	Vps_printf("time 0 = %d\n",Utils_getCurTimeInMsec() - time0);

	move_check_data_copy(output_buf,inst);
	

	check_track_point(output_buf,inst);


	#if 1
//	Vps_printf("time 1 = %d\n",Utils_getCurTimeInMsec() - time0);

	
	//Vps_printf("time 2 = %d\n",Utils_getCurTimeInMsec() - time0);

	//Draw_1(inst,inst->t_global_obj.dst_y_buf,output_buf);

	//Cache_wbInv(output_buf,bufferlen,Cache_Type_ALL,TRUE);
//	return 0;
	gray_close(inst);


	//Vps_printf("time 3 = %d\n",Utils_getCurTimeInMsec() - time0);
	output_param->cmd_type = find_block_kernel(inst);

	//Vps_printf("time 4 = %d\n",Utils_getCurTimeInMsec() - time0);
	//Draw(inst,inst->t_global_obj.src_y_buf,output_buf);
	//Cache_wbInv(output_buf,bufferlen,Cache_Type_ALL,TRUE);

	//return 0;

	for(index = 0; index < output_param->cmd_type; index++)
	{	
		
		if(inst->t_global_obj.g_out_rect[index].nLeft < min_x)
		{
			min_x = inst->t_global_obj.g_out_rect[index].nLeft;
		}

		if(inst->t_global_obj.g_out_rect[index].nLeft > max_x)
		{
			max_x = inst->t_global_obj.g_out_rect[index].nLeft;
		}
		

		yuv_value.y_value = 255;
		yuv_value.cr_value = 0;
		yuv_value.cb_value = 255;

		line_info.start_x 	= inst->t_global_obj.g_out_rect[index].nLeft;
		line_info.start_y 	= inst->t_global_obj.g_out_rect[index].nTop;
		line_info.end_x 	= inst->t_global_obj.g_out_rect[index].nRight;
		line_info.end_y		= inst->t_global_obj.g_out_rect[index].nBottom;
		
		draw_rectangle(inst,output_buf, &yuv_value, &line_info);
	
	}

	

	if(output_param->cmd_type > 0)
	{
		output_param->cmd_type = 1;
	}
	else
	{
		output_param->cmd_type = 2;
	}

	draw_track_rect(inst,output_buf);

	#endif
	tmpbuffer = inst->t_global_obj.src_y_buf;
	inst->t_global_obj.src_y_buf = inst->t_global_obj.tmp_src_y_buf;
	inst->t_global_obj.tmp_src_y_buf = tmpbuffer;

	Cache_wbInv(output_buf,bufferlen,Cache_Type_ALL,TRUE);

	return 0;
}
