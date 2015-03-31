/**
*
*/

#include "blackboardtrack.h"


#define min(a,b) (((a)>(b))?(b):(a))
#define max(a,b) (((a)<(b))?(b):(a))
/**
* @ øÿ÷∆ª≠œﬂµƒ¿‡–Õ
*/
typedef enum _draw_line_type
{
	DRAW_NO = 0,			//≤ªª≠œﬂ
	DRAW_TRIGGER_TRACK,		//±Ì æª≠∏˙◊ŸøÚ∫Õ¥•∑¢øÚ,œﬁ∏ﬂœﬂ,∫Õ∏˙◊Ÿ«¯”ÚøÚ
	DRAW_SLANT_LINE,		//ª≠–±œﬂ±Ì æª≠45∂»,135∂»,90∂»,180∂»œﬂ
	DRAW_MODEL,				//ª≠ƒ£∞Ê
	DRAW_SKIN_COLOR,		//ª≠ºÏ≤‚«¯”ÚµƒÕ∑œÒµƒ∑Ù…´≤ø∑÷
	DRAW_TRACK_TRAJECTORY,	//ª≠∏˙◊ŸπÏº£œﬂ
	DRAW_TRACK_DIRECT,		//ª≠∏˙◊Ÿ∑ΩœÚœﬂ
	DRAW_TRACK_SPEED,		//ª≠∏˙◊ŸÀŸ∂»œﬂ
	MAX_NUM_DRAW
}draw_line_type_e;


/**
* @ øÿ÷∆…„œÒÕ∑∑Ω Ω
*/
typedef enum _track_mode_enum
{
	CLASS_VIEW_FORWARD = 0,		//øŒ«∞»°æ∞
	CLASS_VIEW,					//øŒ÷–»°æ∞
	MAX_NUM_MODE
}track_mode_e;


#define	INTER_PROG_RATIO	(1)		//1//1±Ì æ÷––,2±Ì æ∏Ù––
#define YUV422_OR_YUV420	(2)		//1//1±Ì æ422,2±Ì æ420



/* p, q is on the same of line l */
static int is_same(const vertex_t *l_start, const vertex_t *l_end,
                   const vertex_t *p,
                   const vertex_t *q)
				  
{

	float_t dx = l_end->x - l_start->x;  	//∂‡±ﬂŒµƒ±ﬂ
	float_t dy = l_end->y - l_start->y;

	float_t dx1= p->x - l_start->x;    //p(¥˝ºÏ≤‚µ„)
	float_t dy1= p->y - l_start->y;

	float_t dx2= q->x - l_end->x;      //q(¥˝ºÏ≤‚µ„…‰œﬂ÷’µ„)
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
* @ ≈–∂œµ„ «∑Ò¬‰‘⁄Õ‚∞¸æÿ’Ûƒ⁄£¨»Áπ˚¬‰‘⁄Õ‚∞¸æÿ’Ûƒ⁄£¨‘ÚΩ¯––œ¬“ª≤Ωº∆À„
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
* @	ºÏ≤‚µ„ «0ªπ «1£¨ «0±Ì æ‘⁄ºÏ≤‚«¯”ÚÕ‚£¨ «1±Ì æ‘⁄ºÏ≤‚«¯”Úƒ⁄
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
* @ ∫Ø ˝√˚≥∆: ceil_track_area_check()
* @	∫Ø ˝π¶ƒ‹: …Ë∂®∏˙◊Ÿ«¯”Ú
* @  ‰»Î≤Œ ˝: input_param -- ”¶”√≤„…Ë÷√µƒ≤Œ ˝
* @  ‰≥ˆ≤Œ ˝: Œﬁ
* @ ∑µªÿ÷µ:   Œﬁ
*/
void blackboard_track_area_check(BlackBoardTrackHand *inst)
{
	BlackboardTrack_Params *input_param;
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
		m = k%video_width;     //¡–
		n = k/video_width;    //––

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
* @ ∫Ø ˝√˚≥∆: draw_point()
* @	∫Ø ˝π¶ƒ‹: ª≠µ„µƒπ¶ƒ‹
* @	 ‰»Î≤Œ ˝: buffer -- ª≠Õºµƒbuffer
* @			  yuv_value -- ª≠œﬂµƒY£¨Cb£¨Crµƒ—’…´÷µ
* @			  line_info -- ª≠œﬂµƒ∆ ºµ„◊¯±Í
* @ 		  static_param -- …œ≤„¥´Ω¯¿¥µƒæ≤Ã¨≤Œ ˝
* @  ‰≥ˆ≤Œ ˝: Œﬁ
* @ ∑µªÿ÷µ:   Œﬁ
*/
static int32_t	draw_point(BlackBoardTrackHand *inst,int8_t *buffer, yuv_value_info_t *yuv_value,
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
* @ ∫Ø ˝√˚≥∆: draw_line()
* @	∫Ø ˝π¶ƒ‹: ª≠Ãı÷±œﬂµƒπ¶ƒ‹
* @	 ‰»Î≤Œ ˝: buffer -- ª≠Õºµƒbuffer
* @			  yuv_value -- ª≠œﬂµƒY£¨Cb£¨Crµƒ—’…´÷µ
* @			  line_info -- ª≠œﬂµƒ∆ ºµ„◊¯±Í
* @ 		  static_param -- …œ≤„¥´Ω¯¿¥µƒæ≤Ã¨≤Œ ˝
* @  ‰≥ˆ≤Œ ˝: Œﬁ
* @ ∑µªÿ÷µ:   Œﬁ
*/
static int32_t	draw_line(BlackBoardTrackHand *inst,int8_t *buffer,yuv_value_info_t *yuv_value,
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
* @	∫Ø ˝√˚≥∆: draw_rectangle()
* @	∫Ø ˝π¶ƒ‹: ª≠æÿ–ŒøÚ£¨‘⁄¥À÷˜“™ «ª≠µ„£¨”…”⁄‘⁄¥¶¿Ì ± «”√µƒÀı∑≈π˝∫ÛµƒÕºœÒ£¨∂¯ª≠œﬂµƒ
* @ 		  buffer‘Ú «‘⁄‘¥ÕºœÒ…œ¥¶¿Ìµƒ£¨À˘“‘“™∞—µ„ª≠≥…“ª∏ˆ–°øÚ
* @	 ‰»Î≤Œ ˝: buffer -- ª≠Õºµƒbuffer
* @			  yuv_value -- ª≠œﬂµƒY£¨Cb£¨Crµƒ—’…´÷µ
* @			  line_info -- ª≠œﬂµƒ∆ ºµ„◊¯±Í
* @ 		  static_param -- …œ≤„¥´Ω¯¿¥µƒæ≤Ã¨≤Œ ˝
* @  ‰≥ˆ≤Œ ˝: Œﬁ
* @ ∑µªÿ÷µ:	  Œﬁ
*/
static void draw_rectangle(BlackBoardTrackHand *inst,int8_t *buffer,yuv_value_info_t *yuv_value,
	line_info_t *line_info)
{
    int32_t i,j;
	int32_t left_up_x 		= line_info->start_x;
	int32_t left_up_y 		= line_info->start_y;
	int32_t right_down_x 	= line_info->end_x;
	int32_t right_down_y 	= line_info->end_y;

    
	//ª≠…œ√Êœﬂ
    for(i=left_up_y;i<left_up_y+1;i++)  
	{
	    for(j=left_up_x;j<right_down_x;j++) 
	    {
	    	draw_point(inst,buffer,yuv_value,j,i);
	    }
	}	

	//ª≠œ¬√Êœﬂ
    for(i=right_down_y;i<right_down_y+1;i++)
	{
	    for(j=left_up_x;j<right_down_x;j++) 
	    {
	    	draw_point(inst,buffer,yuv_value,j,i);
	    }
	}

	//ª≠◊Û±ﬂœﬂ
    for(i=left_up_y;i<right_down_y;i++)
	{
	    for(j=left_up_x;j<left_up_x+1;j++) 
	    {
	    	draw_point(inst,buffer,yuv_value,j,i);
	    }
	}

	//ª≠”“±ﬂœﬂ
    for(i=left_up_y;i<right_down_y+1;i++)
	{
	    for(j=right_down_x;j<right_down_x+1;j++) 
	    {
	    	draw_point(inst,buffer,yuv_value,j,i);
	    }
	}

} 

/**
* @	∫Ø ˝√˚≥∆: fill_rectangle()
* @	∫Ø ˝π¶ƒ‹: ª≠æÿ–ŒøÚ£¨‘⁄¥À÷˜“™ «ª≠µ„£¨”…”⁄‘⁄¥¶¿Ì ± «”√µƒÀı∑≈π˝∫ÛµƒÕºœÒ£¨∂¯ª≠œﬂµƒ
* @ 		  buffer‘Ú «‘⁄‘¥ÕºœÒ…œ¥¶¿Ìµƒ£¨À˘“‘“™∞—µ„ª≠≥…“ª∏ˆ–°øÚ
* @	 ‰»Î≤Œ ˝: buffer -- ª≠Õºµƒbuffer
* @			  yuv_value -- ª≠œﬂµƒY£¨Cb£¨Crµƒ—’…´÷µ
* @			  line_info -- ª≠œﬂµƒ∆ ºµ„◊¯±Í
* @ 		  static_param -- …œ≤„¥´Ω¯¿¥µƒæ≤Ã¨≤Œ ˝
* @  ‰≥ˆ≤Œ ˝: Œﬁ
* @ ∑µªÿ÷µ:	  Œﬁ
*/
static void fill_rectangle(BlackBoardTrackHand *inst,int8_t *buffer,yuv_value_info_t *yuv_value,
	line_info_t *line_info)
{
	int nSrcBufWidth=inst->t_global_obj.nSrcBufWidth;
	int nSrcBufHeight=inst->t_global_obj.nSrcBufHeight;
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




/**
* @	∫Ø ˝√˚≥∆: draw_track_rect()
* @ ∫Ø ˝π¶ƒ‹: ª≠∏˙◊Ÿ«¯”ÚøÚ∫Õ¥•∑¢øÚ
* @	 ‰»Î≤Œ ˝: buffer -- ª≠Õºµƒbuffer
* @			  yuv_value -- ª≠œﬂµƒY£¨Cb£¨Crµƒ—’…´÷µ
* @			  line_info -- ª≠œﬂµƒ∆ ºµ„◊¯±Í
* @ 		  input_param -- …œ≤„¥´Ω¯¿¥µƒ ‰»Î≤Œ ˝
* @	 ‰≥ˆ≤Œ ˝: Œﬁ
* @ ∑µªÿ÷µ:	  Œﬁ
*/
static void draw_track_rect(BlackBoardTrackHand *inst,int8_t *buffer)
{
	BlackboardTrack_Params *input_param=&(inst->input_param);
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

		/*≤‚ ‘*/
	for(index = 0; index < 35;index++)
	{
		yuv_value.cb_value = 255;
		yuv_value.cr_value = 0;
		yuv_value.y_value  = 255;

		if(index == 17)
		{
			
			yuv_value.cb_value = 255;
			yuv_value.cr_value = 255;
			yuv_value.y_value  = 255;
		}

		line_info.start_x = index * 20;
		line_info.start_y = 286;

		line_info.end_x = line_info.start_x + 4;
		line_info.end_y = 290;
		
		fill_rectangle(inst,buffer, &yuv_value, &line_info);
	}
}

static int32_t move_check_data_copy(int8_t *output_buf, BlackBoardTrackHand *inst)
{
	//BlackboardTrack_Params *input_param=&(inst->input_param);

	//int16_t	video_width			= input_param->static_param.video_width;
	
	//int16_t	video_height		= input_param->static_param.video_height;
	int32_t			i = 0;
	//int32_t			a = 0;
	uint8_t *src_y_buf    = inst->t_global_obj.tmp_src_y_buf; //¥Ê∑≈“™Ω¯––‘ÀÀ„µƒ‘¥ybuf ˝æ›	
	uint8_t *src_cbcr_buf    = inst->t_global_obj.tmp_src_cbcr_buf; //¥Ê∑≈“™Ω¯––‘ÀÀ„µƒ‘¥ybuf ˝æ›	
	
	//uint8_t *dst_y_buf    = inst->t_global_obj.dst_y_buf; //¥Ê∑≈√ø¥Œ∏˙◊Ÿ∫Ûµƒµ⁄“ª÷°ybuf ˝æ›
	int32_t nSrcBufHeight = inst->t_global_obj.nSrcBufHeight;
	int32_t nSrcBufWidth  = inst->t_global_obj.nSrcBufWidth;

	int32_t 		left			= inst->t_global_obj.tInRect.nLeft;
	int32_t 		top				= inst->t_global_obj.tInRect.nTop;
	int32_t 		right			= inst->t_global_obj.tInRect.nRight;
	int32_t 		bottom			= inst->t_global_obj.tInRect.nBottom;
//	int32_t			index           = 0;
	int32_t         len = right - left + 1;

	
	for(i=top; i<= bottom; i++)
	{
		memcpy(src_y_buf, output_buf + i*nSrcBufWidth + left, len);
		src_y_buf = src_y_buf + len;
	}

	uint8_t *src_cbcr = output_buf + nSrcBufHeight*nSrcBufWidth + left/2*2;
	
	for(i=top; i<= bottom; i=i+2)
	{
		memcpy(src_cbcr_buf, src_cbcr + i/2*nSrcBufWidth, len);
		src_cbcr_buf = src_cbcr_buf + len;
	}
	
	return 0;
}	




static int32_t check_track_point(int8_t *output_buf, BlackBoardTrackHand *inst)
{
	BlackboardTrack_Params 	*input_param	= &(inst->input_param);
	uint8_t 		*gray_buf		= inst->t_global_obj.gray_buf;
	int32_t	m 				= 0;
	int32_t	n				= 0;

	int32_t sens = input_param->dynamic_param.sens;
	int16_t	video_width		= input_param->static_param.video_width;
//	int16_t	video_height	= input_param->static_param.video_height;

//	uint8_t *src_y_buf		= inst->t_global_obj.src_y_buf; //¥Ê∑≈“™Ω¯––‘ÀÀ„µƒ‘¥ybuf ˝æ›	
	uint8_t *dst_y_buf		= inst->t_global_obj.dst_y_buf; //¥Ê∑≈√ø¥Œ∏˙◊Ÿ∫Ûµƒµ⁄“ª÷°ybuf ˝æ›
	uint8_t *dst_cbcr_buf		= inst->t_global_obj.dst_cbcr_buf; //¥Ê∑≈√ø¥Œ∏˙◊Ÿ∫Ûµƒµ⁄“ª÷°ybuf ˝æ›

	uint8_t *Area_vertex	= inst->t_global_obj.Area_vertex;
	int32_t nSrcBufWidth  = inst->t_global_obj.nSrcBufWidth;
	int32_t nSrcBufHeight  = inst->t_global_obj.nSrcBufHeight;
	yuv_value_info_t		yuv_value 	= {0};

	int32_t 		left			= inst->t_global_obj.tInRect.nLeft;
	int32_t 		top				= inst->t_global_obj.tInRect.nTop;
	int32_t 		right			= inst->t_global_obj.tInRect.nRight;
	int32_t 		bottom			= inst->t_global_obj.tInRect.nBottom;
	int32_t 		trigger_sum			= inst->input_param.dynamic_param.trigger_sum;
//	int32_t			index           = 0;
	int32_t         len = right - left + 1;

	int32_t a1 	=  0;
	int32_t	b1 	=  0;
	int32_t a2 	=  0;
	int32_t	b2 	=  0;
	int32_t a3	=  0;
	int32_t	b3 	=  0;
	int32_t	c1	=  0;
	int32_t	c2	=  0;
	int32_t	c3	=  0;
	int32_t offset = nSrcBufWidth*nSrcBufHeight;

	unsigned int			num 		= 0;

	//UInt32 time0= Utils_getCurTimeInMsec();

	int SrcRowOffset = 0;
	int DstRowOffset = 0;
	
	for(n = top; n <= bottom; n++)
	{
		SrcRowOffset = offset + n/2*nSrcBufWidth;
		DstRowOffset = (n - top)/2*len;
		for(m = left; m <= right; m++)
		{
			if (*((uint8_t *)Area_vertex + n*video_width+ m) == 0)
			{
				continue;
			}

			int src_offset = SrcRowOffset + m/2*2;
			int dst_offset = DstRowOffset  + (m - left)/2*2;
			
			//ºÏ≤‚y «∑Ò”–±‰ªØ
			a1 = *((uint8_t *)output_buf +  nSrcBufWidth * n + m);
			b1 = *((uint8_t *)dst_y_buf +  len * (n - top) + (m - left));


			if(sens == 0)
			{

				if(b1 != 0)
				{
					c1 = a1/b1;
				}
				else
				{
					c1 = 1;
				}
			
				if(c1>1.4||c1<0.6)
				{
					yuv_value.y_value = 255;
					yuv_value.cb_value = 0;
					yuv_value.cr_value = 255;
					draw_point(inst,output_buf,&yuv_value,m,n);
					*(gray_buf  +  video_width * n + m) = 1;
					num++;
					
				}	  
				else
				{
					*(gray_buf  +  video_width * n + m) = 0;
				}
			}
			else
			{


				c1 = abs(a1 - b1);
				if (c1 > sens)
				{
					yuv_value.y_value = 255;
					yuv_value.cb_value = 0;
					yuv_value.cr_value = 255;
					draw_point(inst,output_buf,&yuv_value,m,n);
					*(gray_buf  +  video_width * n + m) = 1;
					num++;
					continue;
				}

				
				#if 0
				//ºÏ≤Ècb «∑Ò”–±‰ªØ
				a2 = *((uint8_t *)output_buf + src_offset);
				b2 = *((uint8_t *)dst_cbcr_buf + dst_offset);
				c2 = abs(a2 - b2);
				if (c2 > sens)
				{
					yuv_value.y_value = 255;
					yuv_value.cb_value = 0;
					yuv_value.cr_value = 255;
					draw_point(inst,output_buf,&yuv_value,m,n);
					*(gray_buf  +  video_width * n + m) = 1;
					continue;
				}
			
				a3 = *((uint8_t *)output_buf + src_offset + 1);
				b3 = *((uint8_t *)dst_cbcr_buf + dst_offset + 1);
				c3 = abs(a3 - b3);
				if (c3 > sens)
				{
					yuv_value.y_value = 255;
					yuv_value.cb_value = 0;
					yuv_value.cr_value = 255;
					draw_point(inst,output_buf,&yuv_value,m,n);
					*(gray_buf  +  video_width * n + m) = 1;
					continue;
				}
				#endif

				*(gray_buf  +  video_width * n + m) = 0;
			}
		
		}
	}

	//Vps_printf("time 1 = %d\n",Utils_getCurTimeInMsec() - time0);

	if(num >= trigger_sum)
	{
		return 1;		//±Ì√˜”–¥•∑¢£¨‘⁄–¥∞Â È
	}
	else
	{
		return 0;		//√ª”–¥•∑¢£¨√ª”––¥∞Â È
	}
	
	return 1;
}



int32_t blackboard_track_set_check_max_rect(BlackBoardTrackHand *inst)
{
	BlackboardTrack_Params 	*input_param	= &(inst->input_param);
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

	if(max_x > input_param->static_param.video_width)
	{
		max_x = input_param->static_param.video_width;
	}

	if(max_y > input_param->static_param.video_height)
	{
		max_y = input_param->static_param.video_height;
	}

	inst->t_global_obj.tInRect.nLeft 	= min_x;
	inst->t_global_obj.tInRect.nRight	= max_x;
	inst->t_global_obj.tInRect.nTop   	= min_y;
	inst->t_global_obj.tInRect.nBottom  = max_y;


	Vps_printf("%d %d %d %d\n",min_x,max_x,min_y,max_y);
	return 0;
}

int32_t blackboard_track_get_max_rect(BlackboardTrack_Params *pParams, T_Rect *prect)
{
	BlackboardTrack_Params 	*input_param	= pParams;
	int32_t		index	= 0;
	int32_t		min_x	= 704;
	int32_t		min_y	= 576;
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
static int32_t gray_close(BlackBoardTrackHand *inst)
{
	BlackboardTrack_Params 	*input_param	= &(inst->input_param);
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
	
	//≈Ú’Õ
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
	//≈Ú’Õ
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
	
	//∏Ø ¥
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
	//∏Ø ¥
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
	return 0;
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

static int32_t find_block_kernel(BlackBoardTrackHand *inst)
{
	BlackboardTrack_Params 	*input_param			= &(inst->input_param);
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

	//Vps_printf(">>>>>>>>>>%d %d %d %d\n",left,right,top,bottom);
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
			//if((0==k))
			{
				//nIndex++;
				continue;
			}
			//rect.nLeft=SHORT_MAX;
			//rect.nTop=SHORT_MAX;
			//rect.nRight=0;
			//rect.nBottom=0;
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
	
		
			//Vps_printf("rect.nTop[%d] rect.nBottom[%d] rect.nLeft[%d] rect.nRight[%d] rect.nFillSize[%d][%d:%d][%d]\n",\
			//rect.nTop,rect.nBottom,rect.nLeft,rect.nRight,rect.nFillSize,i,j,k);
		
		//Vps_printf("11111 %d %d %d\n",k,nIndex+j,pMarkedBuf[nIndex+j]);
		}

		nIndex+=nWidth; //------
	}
	
	nObjectCount=0;
	
	for(i=0;i<=rect_count;i++)
	{
		rect=tObjectRects[i];
		rect.nMarkVal=i;
		//≈≈≥˝∑«æÿ–Œ«¯”Ú
		if(SHORT_MAX==rect.nLeft)
		{
			continue;
		}
		#if 1
		//≈≈≥˝≤ª¬˙◊„Ãıº˛µƒæÿ–Œ
		nRectWidth=rect.nRight-rect.nLeft+1;
		nRectHeight=rect.nBottom-rect.nTop+1;

		//Vps_printf("%d trigger_sum = %d rect.nFillSize = %d %d %d %d %d\n",i,input_param->dynamic_param.trigger_sum,rect.nFillSize,nRectWidth,nRectHeight,rect_count,mark_num);
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

#if 0
static Int32 Draw(BlackBoardTrackHand *inst,int8_t *input_buf, int8_t *output_buf)
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

static Int32 Draw_1(BlackBoardTrackHand *inst,int8_t *input_buf, int8_t *output_buf)
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


//ª≠UV
static Int32 Draw_2(BlackBoardTrackHand *inst,int8_t *input_buf, int8_t *output_buf)
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

	char *src_cbcr	= input_buf + 1280*720;

	//Vps_printf("%d %d %d %d\n",left,right,top,bottom);
	//memset(y,0x0,len*(bottom - top));
	for(i=top; i<= bottom; i++)
	{
		memcpy(p + inst->t_global_obj.nSrcBufWidth*i + left, y + (i - top)*len, len);
	
	}

	char *puv = output_buf + inst->t_global_obj.nSrcBufWidth*inst->t_global_obj.nSrcBufHeight;
	
	for(i=top; i<= bottom; i=i+2)
	{
		memcpy(puv + i/2*inst->t_global_obj.nSrcBufWidth + left/2*2, src_cbcr + (i - top)/2*len, len);
	}
	
	
	return;
}

//ª≠UV
static Int32 Draw_3(BlackBoardTrackHand *inst,int8_t *input_buf, int8_t *output_buf, int8_t *uv)
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

	char *src_cbcr	= uv;

	//Vps_printf("%d %d %d %d\n",left,right,top,bottom);
	//memset(y,0x0,len*(bottom - top));
	for(i=top; i<= bottom; i++)
	{
		memcpy(p + inst->t_global_obj.nSrcBufWidth*i + left, y + (i - top)*len, len);
	
	}

	char *puv = output_buf + inst->t_global_obj.nSrcBufWidth*inst->t_global_obj.nSrcBufHeight;
	
	for(i=top; i<= bottom; i=i+2)
	{
		memcpy(puv + i/2*inst->t_global_obj.nSrcBufWidth + left/2*2, src_cbcr + (i - top)/2*len, len);
	}
	
	
	return;
}
#endif

static int ClearRect(BlackBoardTrackHand *inst,int nTriggerWidth,BlackboardTrack_OutArgs *output_param, int8_t *output_buf)
{
	int nRectNum=output_param->trigger_num;
	int nOutNum=0,nRectNo=0;
	T_Rect rect[2];
	T_Rect *ptInRect=inst->t_global_obj.g_out_rect;
	T_Rect *ptMidRect=ptInRect+MAX_TARGET_NUM/2;//10∏ˆƒø±Íƒ⁄¥¶¿Ì ±”√µƒ¡Ÿ ±ø’º‰
	yuv_value_info_t		yuv_value 	= {0};
	line_info_t 			line_info	= {0};
	int i,j;

	int32_t					min_x		= SHORT_MAX;
	int32_t					max_x		= 0;
	int32_t					avg_width	= SHORT_MAX;
	int32_t					width0		= 0;
	int32_t					width1		= 0;
	int32_t maxindex,minindex,space_value;

	int clearflag=0;

	if(output_param->trigger_num == 0)
	{
		output_param->cmd_type = 0;
		return 0;
	}
	avg_width=0;
	for(i=0;i<nRectNum;i++)
	{
		width0	= ptInRect[i].nRight - ptInRect[i].nLeft;
		avg_width+=width0;
		if(ptInRect[i].nLeft < min_x)
		{
			min_x = ptInRect[i].nLeft;
		}
		if(ptInRect[i].nRight> max_x)
		{
			max_x = ptInRect[i].nRight;
		}
	}
	avg_width=avg_width/nRectNum;
	if((3*(max_x - min_x)) > nTriggerWidth)
	{//øÌ∂»¥Û”⁄1/3£¨»œŒ™ «∂‡∏ˆƒø±Í
		output_param->cmd_type = 2;
		return 0;
	}
	if(output_param->trigger_num == 1)
	{
		output_param->trigger[0].x = (inst->t_global_obj.g_out_rect[0].nLeft + inst->t_global_obj.g_out_rect[0].nRight)/2;
		output_param->trigger[0].y = (inst->t_global_obj.g_out_rect[0].nTop + inst->t_global_obj.g_out_rect[0].nBottom)/2;
		output_param->cmd_type = 1;
	}
	else if((output_param->trigger_num > 1)&&(output_param->trigger_num <= 10))
	{
		memcpy(ptMidRect,ptInRect,sizeof(T_Rect)*output_param->trigger_num);
		do
		{
			clearflag = 0;
			for(i=0;i<nRectNum;i++)
			{
				rect[0]=ptMidRect[i];
				if(rect[0].nLeft==SHORT_MAX)
				{
					continue;
				}
				width0	= rect[0].nRight - rect[0].nLeft;
				for(j=i+1;j<nRectNum;j++)
				{
					rect[1]=ptMidRect[j];
					if(rect[1].nLeft==SHORT_MAX)
					{
						continue;
					}
					width1	= rect[1].nRight - rect[1].nLeft;
					if(rect[1].nLeft >= rect[0].nLeft)
					{
						maxindex = 1;
						minindex = 0;
					}
					else
					{
						maxindex = 0;
						minindex = 1;	
					}
					space_value = abs(rect[maxindex].nLeft - rect[minindex].nRight);

					if((rect[maxindex].nLeft <= rect[minindex].nRight) || (space_value <= avg_width))
					{
						clearflag = 1;
						ptMidRect[j].nLeft=SHORT_MAX;
						ptMidRect[i].nLeft=min(rect[0].nLeft,rect[1].nLeft);
						ptMidRect[i].nRight=max(rect[0].nRight,rect[1].nRight);
						ptMidRect[i].nTop=min(rect[0].nTop,rect[1].nTop);
						ptMidRect[i].nBottom=max(rect[0].nBottom,rect[1].nBottom);
						rect[0]=ptMidRect[i];
						width0	= rect[0].nRight - rect[0].nLeft;
					}
				}
			}
		}while(clearflag==1);
		nOutNum=0;
		for(i=0;i<nRectNum;i++)
		{
			rect[0]=ptMidRect[i];
			if(rect[0].nLeft!=SHORT_MAX)
			{
				nRectNo=i;
				nOutNum++;
				yuv_value.y_value = 255;
				yuv_value.cr_value = 255;
				yuv_value.cb_value = 255;
				line_info.start_x	= rect[0].nLeft;
				line_info.start_y	= rect[0].nTop;
				line_info.end_x 	= rect[0].nRight;
				line_info.end_y 	= rect[0].nBottom;
				
				draw_rectangle(inst,output_buf, &yuv_value, &line_info);
			}
		}
		if(nOutNum==1)
		{
			output_param->cmd_type = 1;
			output_param->trigger[0].x = (ptMidRect[nRectNo].nLeft + ptMidRect[nRectNo].nRight)/2;
			output_param->trigger[0].y = (ptMidRect[nRectNo].nTop + ptMidRect[nRectNo].nBottom)/2;
		}
		else
		{
			output_param->cmd_type = 2;
		}
	}
	else if(output_param->trigger_num > 10)
	{
		output_param->cmd_type = 2;
	}

	return 0;
}

#if 0
int InterframeDetection(BlackBoardTrackHand *inst,uint8_t *input_buf)
{
	BlackboardTrack_Params 	*input_param 	= &(inst->input_param);
	int32_t nSrcBufWidth  = inst->t_global_obj.nSrcBufWidth;
	uint8_t *preframe = inst->t_global_obj.pre_frame;
	//uint8_t *preframe = inst->t_global_obj.mid_buf;
	int i,j;
	int top = input_param->dynamic_param.trigger[0].trigger_y0;
	int bottom = input_param->dynamic_param.trigger[0].trigger_y1;
	int left = input_param->dynamic_param.trigger[0].trigger_x0;
	int right = input_param->dynamic_param.trigger[0].trigger_x1;
	int a,b,c,num = 0;
//	yuv_value_info_t yuv_value;

	for(i= top; i < bottom;i++)
	{
		for(j = left; j < right; j++)
		{

			a = *((uint8_t *)preframe +  1280 * i + j);
			b = *((uint8_t *)input_buf +  nSrcBufWidth * i + j);
			c = abs(a-b);

			*((uint8_t *)preframe +  1280 * i + j) = *((uint8_t *)input_buf +  nSrcBufWidth * i + j);

			if(c > 20)
			{
				num++;
				*((uint8_t *)input_buf +  nSrcBufWidth * i + j) = 0;
			}
			
			//*((uint8_t *)input_buf +  nSrcBufWidth * i + j) = 0;
		}
	}

	if(num > 10)
	{
		return 1;
	}

	return 0;
}
#endif



int32_t	blackboard_track_process(BlackBoardTrackHand *inst,int8_t *input_buf, int8_t *output_buf, 
	BlackboardTrack_OutArgs *output_param)
{
	BlackboardTrack_Params 	*input_param 	= &(inst->input_param);

	yuv_value_info_t		yuv_value 	= {0};
	line_info_t 			line_info	= {0};
	int32_t					index		= 0;

	int32_t					min_x		= input_param->static_param.video_width;
	int32_t					max_x		= 0;
	uint8_t                 *tmpbuffer = NULL;

	int32_t					width0		= 0;
	int32_t					width1		= 0;
	int32_t					width2		= 0;

	T_Rect					tTempRect[MAX_TARGET_NUM] = {0};
	int32_t					width		= 0;
	int32_t					height		= 0;
	int32_t					space_value01	= 0;
	int32_t					space_value12	= 0;
	int32_t					space_value20	= 0;
	int32_t max,min,i,maxindex,minindex,midindex= 0;
	UInt32 time0 = Utils_getCurTimeInMsec();;

	if((NULL == inst)||(NULL == output_buf)||(NULL == output_param) || (NULL == input_buf))
	{
		return -1;
	}
	
	int32_t bufferlen = inst->t_global_obj.nSrcBufWidth * inst->t_global_obj.nSrcBufHeight*2;
	int32_t len  = (inst->t_global_obj.tInRect.nRight - inst->t_global_obj.tInRect.nLeft + 1)*(inst->t_global_obj.tInRect.nBottom - inst->t_global_obj.tInRect.nTop + 1);

	
	//Vps_printf("wxd %dx%d %dx%d\n",inst->t_global_obj.nSrcBufWidth,inst->t_global_obj.nSrcBufHeight,input_param->static_param.video_width,input_param->static_param.video_height);
	inst->t_global_obj.debugValue0 = 1;
	inst->t_global_obj.frame_num ++;

	
	if(CLASS_VIEW == inst->input_param.dynamic_param.track_mode)
	{
		inst->t_global_obj.dst_y_buf = inst->t_global_obj.src_y_buf;
		inst->t_global_obj.dst_cbcr_buf = inst->t_global_obj.src_cbcr_buf;
		//memcpy(inst->t_global_obj.dst_y_buf, inst->t_global_obj.src_y_buf, len);
	}
	else
	{
		inst->t_global_obj.dst_y_buf = input_buf;
		inst->t_global_obj.dst_cbcr_buf = input_buf + 1280*720;
	
		//inst->t_global_obj.dst_y_buf = inst->t_global_obj.backdrop_buf;
	//	inst->t_global_obj.dst_cbcr_buf = inst->t_global_obj.backdrop_buf; + 1280*720;
		//memcpy(inst->t_global_obj.dst_y_buf, input_buf, len);

	}

	inst->t_global_obj.debugValue0 = 2;
//	Vps_printf("time 0 = %d\n",Utils_getCurTimeInMsec() - time0);


	move_check_data_copy(output_buf,inst);
	inst->t_global_obj.debugValue0 = 3;

	//memcpy(input_buf+1280*720,inst->t_global_obj.tmp_src_cbcr_buf,len/2);
	//Draw_2(inst,input_buf,output_buf);
	//Cache_wbInv(output_buf,bufferlen,Cache_Type_ALL,TRUE);

	//return ;
	output_param->cmd_type	=	check_track_point(output_buf,inst);


	if(inst->t_global_obj.last_frame_status != output_param->cmd_type)
	{
		inst->t_global_obj.frame_num ++;

		if(output_param->cmd_type == 1)
		{
			if(inst->t_global_obj.frame_num > 10)
			{
				inst->t_global_obj.last_frame_status = output_param->cmd_type;
				inst->t_global_obj.frame_num = 0;
			}
			else
			{
				output_param->cmd_type = inst->t_global_obj.last_frame_status;
			}
			
		}
		else
		{
			if(inst->t_global_obj.frame_num > 50)
			{
				inst->t_global_obj.last_frame_status = output_param->cmd_type;
				inst->t_global_obj.frame_num = 0;
			}
			else
			{
				output_param->cmd_type = inst->t_global_obj.last_frame_status;
			}
		}
	
	}
	else
	{
		inst->t_global_obj.frame_num = 0;
	}



	inst->t_global_obj.debugValue0 = 6;
	draw_track_rect(inst,output_buf);
	inst->t_global_obj.debugValue0 = 7;

	tmpbuffer = inst->t_global_obj.src_y_buf;
	inst->t_global_obj.src_y_buf = inst->t_global_obj.tmp_src_y_buf;
	inst->t_global_obj.tmp_src_y_buf = tmpbuffer;


	tmpbuffer = inst->t_global_obj.src_cbcr_buf;
	inst->t_global_obj.src_cbcr_buf = inst->t_global_obj.tmp_src_cbcr_buf;
	inst->t_global_obj.tmp_src_cbcr_buf = tmpbuffer;
	

	Cache_wbInv(output_buf,bufferlen,Cache_Type_ALL,TRUE);
	inst->t_global_obj.debugValue0 = 8;
	return 0;
}

