/**
*
*/

#include "track.h"
#include "tracking.h"
#include "AutoTrack.h"

/**
* @ ¿ØÖÆ»­ÏßµÄÀàÐÍ
*/
typedef enum _draw_line_type
{
	DRAW_NO = 0,			//²»»­Ïß
	DRAW_TRIGGER_TRACK,		//±íÊ¾»­¸ú×Ù¿òºÍ´¥·¢¿ò,ÏÞ¸ßÏß,ºÍ¸ú×ÙÇøÓò¿ò
	DRAW_SLANT_LINE,		//»­Ð±Ïß±íÊ¾»­45¶È,135¶È,90¶È,180¶ÈÏß
	DRAW_MODEL,				//»­Ä£°æ
	DRAW_SKIN_COLOR,		//»­¼ì²âÇøÓòµÄÍ·ÏñµÄ·ôÉ«²¿·Ö
	DRAW_TRACK_TRAJECTORY,	//»­¸ú×Ù¹ì¼£Ïß
	DRAW_TRACK_DIRECT,		//»­¸ú×Ù·½ÏòÏß
	DRAW_TRACK_SPEED,		//»­¸ú×ÙËÙ¶ÈÏß
	MAX_NUM_DRAW
}draw_line_type_e;


#define	INTER_PROG_RATIO	(1)		//1//1±íÊ¾ÖðÐÐ,2±íÊ¾¸ôÐÐ
#define YUV422_OR_YUV420	(2)		//1//1±íÊ¾422,2±íÊ¾420

/**
* @	¿ØÖÆÉãÏñÍ·Ê±,Ëù¸ú×ÙÈËµÄÖÐÐÄµãµ½ÆÁÄ»ÖÐÐÄµãµÄ¾àÀëÐ¡ÓÚÕâ¸öºêÊ±,ÈÏÎªÒÑ¾­ÔÚÖÐÐÄµã 
*/
#define CENTER_DISTANC_MAX		(20)
#define CENTER_DISTANC_MIN		(8)

#if 1
int AnalysingVideoSharp(unsigned short usSrcWidth, unsigned short usSrcHeight,
	                                                  unsigned short usAreaWidth, unsigned short usAreaHeight,
	                                                  unsigned char* pcSrc, unsigned char* pcArea, unsigned char* pcDst)
{
	unsigned short  i, j;
	short sTemp = 0, sDiff = 0;
	unsigned char* pcPos = NULL;

    for(i = 1; i < usAreaHeight - 1; i++)
    {
	    for(j = 1; j < usAreaWidth - 1; j++)
	    {	 
		    if(*(pcArea + usAreaWidth * i + j) == 0)
	         	{continue;}
			
		    pcPos = pcSrc + usSrcWidth * i + j;
		    if((*pcPos) > *(pcPos - usSrcWidth) + 10
				&& (*pcPos) > *(pcPos + usSrcWidth) + 10
				&& (*pcPos) > *(pcPos + 1) + 10
				&& (*pcPos) > *(pcPos - 1) + 10)
		    {
		    	  *(pcDst + usAreaWidth * i + j) = (*pcPos);
		    	  continue;
		    }
		    sDiff = 8 *  (*pcPos);
		    sDiff = (sDiff - *(pcPos - usSrcWidth - 1) -  *(pcPos - usSrcWidth)
		       - *(pcPos - usSrcWidth + 1) - *(pcPos - 1) - *(pcPos + 1)
		       - *(pcPos + usSrcWidth - 1) -  *(pcPos + usSrcWidth) -  *(pcPos + usSrcWidth + 1));

	         if(sDiff > 24)
	         {
			    sTemp = (*pcPos) + (sDiff >> 1);
		         sTemp = sTemp > 255 ? 255 : sTemp;
		         *(pcDst + usAreaWidth * i + j) = sTemp;
	         }
	         else if(sDiff < -24)
	         {
			    sTemp = 32767 - (sDiff & 0x7fff);
			    sTemp = (sTemp >> 1);
		         sTemp = (*pcPos) - sTemp;
		         sTemp = sTemp < 0 ? 0 : sTemp;
		         *(pcDst + usAreaWidth * i + j) = sTemp;
	         }
			 else
			 {
			 *(pcDst + usAreaWidth * i + j) = (*pcPos);
			 }
	    }
    }

    for(i = 1; i < usAreaHeight - 1; i++)
    {
	    for(j = 1; j < usAreaWidth - 1; j++)
	    {	
	          if(*(pcArea + usAreaWidth * i + j) != 0)
	          {
	               *(pcSrc + usSrcWidth * i + j) = *(pcDst + usAreaWidth * i + j);
	          }
	    }
    }


	return 1;
}

#endif

/* p, q is on the same of line l */
static int is_same(const vertex_t *l_start, const vertex_t *l_end,
                   const vertex_t *p,
                   const vertex_t *q)
				  
{

	float_t dx = l_end->x - l_start->x;  	//¶à±ßÎµÄ±ß
	float_t dy = l_end->y - l_start->y;

	float_t dx1= p->x - l_start->x;    //p(´ý¼ì²âµã)
	float_t dy1= p->y - l_start->y;

	float_t dx2= q->x - l_end->x;      //q(´ý¼ì²âµãÉäÏßÖÕµã)
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
* @ ÅÐ¶ÏµãÊÇ·ñÂäÔÚÍâ°ü¾ØÕóÄÚ£¬Èç¹ûÂäÔÚÍâ°ü¾ØÕóÄÚ£¬Ôò½øÐÐÏÂÒ»²½¼ÆËã
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
* @	¼ì²âµãÊÇ0»¹ÊÇ1£¬ÊÇ0±íÊ¾ÔÚ¼ì²âÇøÓòÍâ£¬ÊÇ1±íÊ¾ÔÚ¼ì²âÇøÓòÄÚ
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
* @ º¯ÊýÃû³Æ: area_check()
* @	º¯Êý¹¦ÄÜ: Éè¶¨¸ú×ÙÇøÓò
* @ ÊäÈë²ÎÊý: input_param -- Ó¦ÓÃ²ãÉèÖÃµÄ²ÎÊý
* @ Êä³ö²ÎÊý: ÎÞ
* @ ·µ»ØÖµ:   ÎÞ
*/
void area_check(TrackHand *inst)
{
	ITRACK_Params *input_param;
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
		m = k%video_width;     //ÁÐ
		n = k/video_width;    //ÐÐ

		point_y_value.x = m;
		point_y_value.y = n;

		if (pt_in_poly ((const vertex_t *)input_param->dynamic_param.track_point, input_param->dynamic_param.track_point_num,  
			&point_y_value)==1)
		{
		#if 0
			if (n%2 == 0)
			{
				*((Uint8 *)Area_vertex + n/2 * video_width + m) = 255;
			}
			else
			{
				*((Uint8 *)Area_vertex + (n/2 + video_height/2)  * video_width + m) = 255;
			}
		#endif	
			*((Uint8 *)Area_vertex + n * video_width + m) = 255;
		}
		else
		{
		#if 0
			if (n%2 == 0)
			{
				*((Uint8 *)Area_vertex + n/2 * video_width + m) = 0;
			}
			else
			{
				*((Uint8 *)Area_vertex + (n/2 + video_height/2) * video_width + m) = 0;
			}
		#endif	
			*((Uint8 *)Area_vertex + n * video_width + m) = 0;
		}
	}
}

#if 1
//³õÊ¼»¯Ä£°æ
int32_t	init_model(TrackHand *inst)
{
	uint8_t	(*model)[MODEL_NUM_MAX] = inst->t_global_obj.model;
	//½ü¾°Ä£°æ
	model[0][0] = 0x11;
	model[0][1] = 0x88;
	model[0][2] = 0x2;
	model[0][3] = 5;
	model[0][4] = 5;
	
	model[0][5] = 0x14;
	model[0][6] = 0x8;
	model[0][7] = 0x2;
	model[0][8] = 5;
	model[0][9] = 5;
	
	model[0][10] = 0x17;
	model[0][11] = 0x89;
	model[0][12] = 0x8;
	model[0][13] = 5;
	model[0][14] = 5;
	
	model[0][15] = 0x18;
	model[0][16] = 0x9;
	model[0][17] = 0x8;
	model[0][18] = 5;
	model[0][19] = 5;
#if 0
	model[0][20] = 0xaa;
	model[0][21] = 0x0;
	model[0][22] = 0xe;
	model[0][23] = 4;
	model[0][24] = 4;
#else
/*
	model[0][20] = 0xaa;
	model[0][21] = 0x0;
	model[0][22] = 0xe;
	model[0][23] = 4;
	model[0][24] = 8;
*/
	model[0][20] = 0xaa;
	model[0][21] = 0x0;
	model[0][22] = 0xf;
	model[0][23] = 4;
	model[0][24] = 9;
#endif
	
	model[0][25] = 0x15;
	model[0][26] = 0x86;
	model[0][27] = 0x16;
	model[0][28] = 6;
	model[0][29] = 10;
	
	model[0][30] = 0x12;
	model[0][31] = 0x6;
	model[0][32] = 0x16;
	model[0][33] = 6;
	model[0][34] = 10;
	
	model[0][35] = 0xaf;
	model[0][36] = 0x0;
	model[0][37] = 0xa;
	model[0][38] = 14;
	model[0][39] = 14;
	
	model[0][40] = 0x11;
	model[0][41] = 0x94;
	model[0][42] = 0x26;
	model[0][43] = 16;
	model[0][44] = 8;
	
	model[0][45] = 0x14;
	model[0][46] = 0x14;
	model[0][47] = 0x26;
	model[0][48] = 16;
	model[0][49] = 8;
	
	//Ô¶¾°Ä£°æ
	model[1][0] = 0x11;
	model[1][1] = 0x84;
	model[1][2] = 0x1;
	model[1][3] = 4;
	model[1][4] = 4;
	
	model[1][5] = 0x14;
	model[1][6] = 0x4;
	model[1][7] = 0x1;
	model[1][8] = 4;
	model[1][9] = 4;
	
	model[1][10] = 0x17;
	model[1][11] = 0x85;
	model[1][12] = 0x4;
	model[1][13] = 4;
	model[1][14] = 4;
	
	model[1][15] = 0x18;
	model[1][16] = 0x5;
	model[1][17] = 0x4;
	model[1][18] = 4;
	model[1][19] = 4;

	model[1][20] = 0xaa;
	model[1][21] = 0x0;
	model[1][22] = 0x7;
	model[1][23] = 8;
	model[1][24] = 6;
	
	model[1][25] = 0x15;
	model[1][26] = 0x83;
	model[1][27] = 0xb;
	model[1][28] = 3;
	model[1][29] = 5;
	
	model[1][30] = 0x12;
	model[1][31] = 0x3;
	model[1][32] = 0xb;
	model[1][33] = 3;
	model[1][34] = 5;
	
	model[1][35] = 0xaf;
	model[1][36] = 0x0;
	model[1][37] = 0x5;
	model[1][38] = 7;
	model[1][39] = 7;
	
	model[1][40] = 0x11;
	model[1][41] = 0x8a;
	model[1][42] = 0x13;
	model[1][43] = 8;
	model[1][44] = 4;
	
	model[1][45] = 0x14;
	model[1][46] = 0x0a;
	model[1][47] = 0x13;
	model[1][48] = 8;
	model[1][49] = 4;
	return 0;

}
/*
//²âÊÔÄ£°æ
int32_t	(TrackHand *inst)
{
	uint8_t	(*model)[MODEL_NUM_MAX] = inst->t_global_obj.model;
	//Ô¶¾°µÄÄ£°æ
	model[0] = 0x11;
	model[1] = 0x84;
	model[2] = 0x1;
	model[3] = 4;
	model[4] = 4;
	
	model[5] = 0x14;
	model[6] = 0x4;
	model[7] = 0x1;
	model[8] = 4;
	model[9] = 4;
	
	model[10] = 0x17;
	model[11] = 0x84;
	model[12] = 0x5;
	model[13] = 4;
	model[14] = 4;
	
	model[15] = 0x18;
	model[16] = 0x4;
	model[17] = 0x5;
	model[18] = 4;
	model[19] = 4;

	model[20] = 0xaa;
	model[21] = 0x0;
	model[22] = 0x7;
	model[23] = 2;
	model[24] = 4;
	
	model[25] = 0x15;
	model[26] = 0x83;
	model[27] = 0xb;
	model[28] = 3;
	model[29] = 5;
	
	model[30] = 0x12;
	model[31] = 0x3;
	model[32] = 0xb;
	model[33] = 3;
	model[34] = 5;
	
	model[35] = 0xaf;
	model[36] = 0x0;
	model[37] = 0x5;
	model[38] = 7;
	model[39] = 7;
	
	model[40] = 0x11;
	model[41] = 0x8a;
	model[42] = 0x13;
	model[43] = 8;
	model[44] = 4;
	
	model[45] = 0x14;
	model[46] = 0x0a;
	model[47] = 0x13;
	model[48] = 8;
	model[49] = 4;
	return 0;

}
*/
#else
//½ü¾°µÄÄ£°æ
int32_t	init_model(TRACK_Obj *inst)
{
	uint8_t	(*model)[MODEL_NUM_MAX] = inst->t_global_obj.model;
	//³õÊ¼»¯Ä£°æ
	model[0][0] = 0x11;
	model[0][1] = 0x88;
	model[0][2] = 0x2;
	model[0][3] = 5;
	model[0][4] = 5;
	
	model[0][5] = 0x14;
	model[0][6] = 0x8;
	model[0][7] = 0x2;
	model[0][8] = 5;
	model[0][9] = 5;
	
	model[0][10] = 0x17;
	model[0][11] = 0x89;
	model[0][12] = 0x8;
	model[0][13] = 5;
	model[0][14] = 5;
	
	model[0][15] = 0x18;
	model[0][16] = 0x9;
	model[0][17] = 0x8;
	model[0][18] = 5;
	model[0][19] = 5;
#if 0
	model[0][20] = 0xaa;
	model[0][21] = 0x0;
	model[0][22] = 0xe;
	model[0][23] = 4;
	model[0][24] = 4;
#else
	model[0][20] = 0xaa;
	model[0][21] = 0x0;
	model[0][22] = 0xe;
	model[0][23] = 4;
	model[0][24] = 8;
#endif
	
	model[0][25] = 0x15;
	model[0][26] = 0x86;
	model[0][27] = 0x16;
	model[0][28] = 6;
	model[0][29] = 10;
	
	model[0][30] = 0x12;
	model[0][31] = 0x6;
	model[0][32] = 0x16;
	model[0][33] = 6;
	model[0][34] = 10;
	
	model[0][35] = 0xaf;
	model[0][36] = 0x0;
	model[0][37] = 0xa;
	model[0][38] = 14;
	model[0][39] = 14;
	
	model[0][40] = 0x11;
	model[0][41] = 0x94;
	model[0][42] = 0x26;
	model[0][43] = 16;
	model[0][44] = 8;
	
	model[0][45] = 0x14;
	model[0][46] = 0x14;
	model[0][47] = 0x26;
	model[0][48] = 16;
	model[0][49] = 8;
	return 0;

}
#endif

/**
* @ º¯ÊýÃû³Æ: draw_point()
* @	º¯Êý¹¦ÄÜ: »­µãµÄ¹¦ÄÜ
* @	ÊäÈë²ÎÊý: buffer -- »­Í¼µÄbuffer
* @			  yuv_value -- »­ÏßµÄY£¬Cb£¬CrµÄÑÕÉ«Öµ
* @			  line_info -- »­ÏßµÄÆðÊ¼µã×ø±ê
* @ 		  static_param -- ÉÏ²ã´«½øÀ´µÄ¾²Ì¬²ÎÊý
* @ Êä³ö²ÎÊý: ÎÞ
* @ ·µ»ØÖµ:   ÎÞ
*/
static int32_t	draw_point(TrackHand *inst,int8_t *buffer, yuv_value_info_t *yuv_value,
	int32_t	point_x, int32_t point_y)
{
	track_static_param_t *static_param=&(inst->input_param.static_param);
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
* @ º¯ÊýÃû³Æ: draw_line()
* @	º¯Êý¹¦ÄÜ: »­ÌõÖ±ÏßµÄ¹¦ÄÜ
* @	ÊäÈë²ÎÊý: buffer -- »­Í¼µÄbuffer
* @			  yuv_value -- »­ÏßµÄY£¬Cb£¬CrµÄÑÕÉ«Öµ
* @			  line_info -- »­ÏßµÄÆðÊ¼µã×ø±ê
* @ 		  static_param -- ÉÏ²ã´«½øÀ´µÄ¾²Ì¬²ÎÊý
* @ Êä³ö²ÎÊý: ÎÞ
* @ ·µ»ØÖµ:   ÎÞ
*/
static int32_t	draw_line(TrackHand *inst,int8_t *buffer,yuv_value_info_t *yuv_value,
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
* @	º¯ÊýÃû³Æ: draw_rectangle()
* @	º¯Êý¹¦ÄÜ: »­¾ØÐÎ¿ò£¬ÔÚ´ËÖ÷ÒªÊÇ»­µã£¬ÓÉÓÚÔÚ´¦ÀíÊ±ÊÇÓÃµÄËõ·Å¹ýºóµÄÍ¼Ïñ£¬¶ø»­ÏßµÄ
* @ 		  bufferÔòÊÇÔÚÔ´Í¼ÏñÉÏ´¦ÀíµÄ£¬ËùÒÔÒª°Ñµã»­³ÉÒ»¸öÐ¡¿ò
* @	ÊäÈë²ÎÊý: buffer -- »­Í¼µÄbuffer
* @			  yuv_value -- »­ÏßµÄY£¬Cb£¬CrµÄÑÕÉ«Öµ
* @			  line_info -- »­ÏßµÄÆðÊ¼µã×ø±ê
* @ 		  static_param -- ÉÏ²ã´«½øÀ´µÄ¾²Ì¬²ÎÊý
* @ Êä³ö²ÎÊý: ÎÞ
* @ ·µ»ØÖµ:	  ÎÞ
*/
static void draw_rectangle(TrackHand *inst,int8_t *buffer,yuv_value_info_t *yuv_value,
	line_info_t *line_info)
{
    int32_t i,j;
	int32_t left_up_x 		= line_info->start_x;
	int32_t left_up_y 		= line_info->start_y;
	int32_t right_down_x 	= line_info->end_x;
	int32_t right_down_y 	= line_info->end_y;
	//int32_t	video_hight		= inst->input_param.static_param.video_height;

    
	//»­ÉÏÃæÆæÊýÐÐ
    for(i=left_up_y;i<left_up_y+1;i++)  
	{
	    for(j=left_up_x;j<right_down_x;j++) 
	    {
	    	draw_point(inst,buffer,yuv_value,j,i);
	    }
	}	

	#if 0
	//»­ÉÏÃæÅ¼ÊýÐÐ
	for(i=video_hight/2+left_up_y;i<video_hight/2+left_up_y+1;i++)  
	{
	    for(j=left_up_x;j<right_down_x;j++) 
	    {
	    	draw_point(inst,buffer,yuv_value,j,i);
	    }
	}
	#endif
	//»­ÏÂÃæÆæÊýÐÐ
    for(i=right_down_y;i<right_down_y+1;i++)
	{
	    for(j=left_up_x;j<right_down_x;j++) 
	    {
	    	draw_point(inst,buffer,yuv_value,j,i);
	    }
	}

	#if 0
	//»­ÏÂÃæÅ¼ÊýÐÐ
	for(i=video_hight/2+right_down_y;i<video_hight/2+right_down_y+1;i++)
	{
	    for(j=left_up_x;j<right_down_x;j++) 
	    {
	    	draw_point(inst,buffer,yuv_value,j,i);
	    }
	}	
	#endif
	
	//»­×ó±ßÆæÊýÐÐ
    for(i=left_up_y;i<right_down_y;i++)
	{
	    for(j=left_up_x;j<left_up_x+1;j++) 
	    {
	    	draw_point(inst,buffer,yuv_value,j,i);
	    }
	}

	#if 0
	//»­×ó±ßÅ¼ÊýÐÐ
	for(i=video_hight/2+left_up_y;i<video_hight/2+right_down_y;i++)
	{
	    for(j=left_up_x;j<left_up_x+1;j++) 
	    {
	    	draw_point(inst,buffer,yuv_value,j,i);
	    }
	}
	#endif
	
	//»­ÓÒ±ßÆæÊýÐÐ
    for(i=left_up_y;i<right_down_y+1;i++)
	{
	    for(j=right_down_x;j<right_down_x+1;j++) 
	    {
	    	draw_point(inst,buffer,yuv_value,j,i);
	    }
	}

	#if 0
	//»­ÓÒ±ßÅ¼ÊýÐÐ
    for(i=video_hight/2+left_up_y;i<video_hight/2+right_down_y+1;i++)
	{
	    for(j=right_down_x;j<right_down_x+1;j++) 
	    {
	    	draw_point(inst,buffer,yuv_value,j,i);
	    }
	}
	#endif
} 

/**
* @	º¯ÊýÃû³Æ: draw_track_rect()
* @ º¯Êý¹¦ÄÜ: »­¸ú×ÙÇøÓò¿òºÍ´¥·¢¿ò
* @	ÊäÈë²ÎÊý: buffer -- »­Í¼µÄbuffer
* @			  yuv_value -- »­ÏßµÄY£¬Cb£¬CrµÄÑÕÉ«Öµ
* @			  line_info -- »­ÏßµÄÆðÊ¼µã×ø±ê
* @ 		  input_param -- ÉÏ²ã´«½øÀ´µÄÊäÈë²ÎÊý
* @	Êä³ö²ÎÊý: ÎÞ
* @ ·µ»ØÖµ:	  ÎÞ
*/
static void draw_track_rect(TrackHand *inst,int8_t *buffer)
{
	ITRACK_Params *input_param=&(inst->input_param);
	line_info_t 		line_info	= {0};
	yuv_value_info_t 	yuv_value	= {0};
	int16_t				index		= 0;
	int16_t				j			= 0;
	int16_t				point_num	= input_param->dynamic_param.track_point_num;
//	int16_t				limit_height	= input_param->dynamic_param.limit_height;
	trigger_info_t trigger[TRACK_TRIGGER_MAX_NUM];
	trigger_info_t meanshift_trigger;
	trigger_info_t pos1_trigger;
	int32_t	x_offset 	= input_param->dynamic_param.x_offset;
	int32_t	y_offset	= input_param->dynamic_param.y_offset;

	int32_t	mid_x		= input_param->dynamic_param.mid_x;
	int32_t	mid_y		= input_param->dynamic_param.mid_y;
	
	int8_t	zoom_distance	= input_param->dynamic_param.zoom_distance;
	int32_t m_offset=inst->t_global_obj.m_offset;		//Ä¿±êµÄ×óÓÒÎ»ÖÃ
	int32_t n_offset=inst->t_global_obj.n_offset;		//Ä¿±êµÄÉÏÏÂÎ»ÖÃ
	uint8_t	(*model)[MODEL_NUM_MAX] = inst->t_global_obj.model;
	int32_t nTrackSwitchType=input_param->dynamic_param.TrackOtherParam.nTrackSwitchType;
	if(zoom_distance < 0)
	{
		zoom_distance = 0;
	}
	if(zoom_distance >= MODEL_DISTANCE_CHOSE_NUM)
	{
		zoom_distance = MODEL_DISTANCE_CHOSE_NUM - 1;
	}
	yuv_value.y_value = 255;
	yuv_value.cb_value = 255;
	yuv_value.cr_value = 0;
	
	//»­´¥·¢¿ò
	trigger[0].trigger_x0 		= input_param->dynamic_param.trigger[0].trigger_x0;
	trigger[0].trigger_y0 		= input_param->dynamic_param.trigger[0].trigger_y0;
	trigger[0].trigger_x1 		= input_param->dynamic_param.trigger[0].trigger_x1;
	trigger[0].trigger_y1 		= input_param->dynamic_param.trigger[0].trigger_y1;
	trigger[1].trigger_x0 		= input_param->dynamic_param.trigger[1].trigger_x0;
	trigger[1].trigger_y0 		= input_param->dynamic_param.trigger[1].trigger_y0;
	trigger[1].trigger_x1 		= input_param->dynamic_param.trigger[1].trigger_x1;
	trigger[1].trigger_y1 		= input_param->dynamic_param.trigger[1].trigger_y1;
	for(j=0;j<TRACK_TRIGGER_MAX_NUM;j++)
	{
		line_info.start_x 	= trigger[j].trigger_x0;
		line_info.start_y 	= trigger[j].trigger_y0/INTER_PROG_RATIO;
		line_info.end_x 	= trigger[j].trigger_x1;
		line_info.end_y 	= trigger[j].trigger_y0/INTER_PROG_RATIO;
		draw_line(inst,buffer,&yuv_value,&line_info);

		line_info.start_x 	= trigger[j].trigger_x1;
		line_info.start_y 	= trigger[j].trigger_y0/INTER_PROG_RATIO;
		line_info.end_x 	= trigger[j].trigger_x1;
		line_info.end_y 	= trigger[j].trigger_y1 /INTER_PROG_RATIO;
		draw_line(inst,buffer,&yuv_value,&line_info);

		line_info.start_x 	= trigger[j].trigger_x0;
		line_info.start_y 	= trigger[j].trigger_y0/INTER_PROG_RATIO;
		line_info.end_x 	= trigger[j].trigger_x0;
		line_info.end_y 	= trigger[j].trigger_y1/INTER_PROG_RATIO;
		draw_line(inst,buffer,&yuv_value,&line_info);

		line_info.start_x 	= trigger[j].trigger_x0;
		line_info.start_y 	= trigger[j].trigger_y1/INTER_PROG_RATIO;
		line_info.end_x 	= trigger[j].trigger_x1;
		line_info.end_y 	= trigger[j].trigger_y1/INTER_PROG_RATIO;
		draw_line(inst,buffer,&yuv_value,&line_info);
	}
	if(nTrackSwitchType==0)
	{
		yuv_value.y_value = 255;
		yuv_value.cb_value = 0;
		yuv_value.cr_value = 255;
		for(index=0; index<point_num; index++) 
		{ 
			j = (index+1) % point_num; 
		
			line_info.start_x	= input_param->dynamic_param.track_point[index].x;
			line_info.start_y	= input_param->dynamic_param.track_point[index].y/INTER_PROG_RATIO;
			line_info.end_x 	= input_param->dynamic_param.track_point[j].x;
			line_info.end_y 	= input_param->dynamic_param.track_point[j].y/INTER_PROG_RATIO;
			draw_line(inst,buffer,&yuv_value,&line_info);
		}
		//ÏÔÊ¾¸ú×Ù¿ò×´Ì¬
		if(m_offset < model[zoom_distance][38])
		{
			m_offset = model[zoom_distance][38]+1;
		}
		line_info.start_x = (m_offset - model[zoom_distance][38]) * x_offset;
		line_info.start_y = n_offset * (y_offset/INTER_PROG_RATIO);
		line_info.end_x = (m_offset + model[zoom_distance][38]) * x_offset;
		line_info.end_y = (n_offset + model[zoom_distance][39] * 2) * (y_offset/INTER_PROG_RATIO);

		yuv_value.y_value = 0;
		yuv_value.cb_value	= 0;
		yuv_value.cr_value	= 255;
		if (m_offset == mid_x && n_offset == mid_y)
		{
			yuv_value.cb_value	= 255;
			yuv_value.cr_value	= 0;
		}
			
		draw_rectangle(inst,buffer, &yuv_value, &line_info);

		//»­meanshift ¼ì²â¿ò
		yuv_value.y_value = 255;
		yuv_value.cb_value = 128;
		yuv_value.cr_value = 128;
		meanshift_trigger = input_param->dynamic_param.TrackOtherParam.meanshift_trigger;
		line_info.start_x	= meanshift_trigger.trigger_x0;
		line_info.start_y	= meanshift_trigger.trigger_y0/INTER_PROG_RATIO;
		line_info.end_x 	= meanshift_trigger.trigger_x1;
		line_info.end_y 	= meanshift_trigger.trigger_y0/INTER_PROG_RATIO;
		draw_line(inst,buffer,&yuv_value,&line_info);

		line_info.start_x	= meanshift_trigger.trigger_x1;
		line_info.start_y	= meanshift_trigger.trigger_y0/INTER_PROG_RATIO;
		line_info.end_x 	= meanshift_trigger.trigger_x1;
		line_info.end_y 	= meanshift_trigger.trigger_y1 /INTER_PROG_RATIO;
		draw_line(inst,buffer,&yuv_value,&line_info);

		line_info.start_x	= meanshift_trigger.trigger_x0;
		line_info.start_y	= meanshift_trigger.trigger_y0/INTER_PROG_RATIO;
		line_info.end_x 	= meanshift_trigger.trigger_x0;
		line_info.end_y 	= meanshift_trigger.trigger_y1/INTER_PROG_RATIO;
		draw_line(inst,buffer,&yuv_value,&line_info);

		line_info.start_x	= meanshift_trigger.trigger_x0;
		line_info.start_y	= meanshift_trigger.trigger_y1/INTER_PROG_RATIO;
		line_info.end_x 	= meanshift_trigger.trigger_x1;
		line_info.end_y 	= meanshift_trigger.trigger_y1/INTER_PROG_RATIO;
		draw_line(inst,buffer,&yuv_value,&line_info);

	//»­pos1 ¼ì²â¿ò
	yuv_value.y_value = 0;
	yuv_value.cb_value = 128;
	yuv_value.cr_value = 128;
	pos1_trigger= input_param->dynamic_param.TrackOtherParam.pos1_trigger;
	line_info.start_x	= pos1_trigger.trigger_x0;
	line_info.start_y	= pos1_trigger.trigger_y0/INTER_PROG_RATIO;
	line_info.end_x 	= pos1_trigger.trigger_x1;
	line_info.end_y 	= pos1_trigger.trigger_y0/INTER_PROG_RATIO;
	draw_line(inst,buffer,&yuv_value,&line_info);

		line_info.start_x	= pos1_trigger.trigger_x1;
		line_info.start_y	= pos1_trigger.trigger_y0/INTER_PROG_RATIO;
		line_info.end_x 	= pos1_trigger.trigger_x1;
		line_info.end_y 	= pos1_trigger.trigger_y1 /INTER_PROG_RATIO;
		draw_line(inst,buffer,&yuv_value,&line_info);

		line_info.start_x	= pos1_trigger.trigger_x0;
		line_info.start_y	= pos1_trigger.trigger_y0/INTER_PROG_RATIO;
		line_info.end_x 	= pos1_trigger.trigger_x0;
		line_info.end_y 	= pos1_trigger.trigger_y1/INTER_PROG_RATIO;
		draw_line(inst,buffer,&yuv_value,&line_info);

		line_info.start_x	= pos1_trigger.trigger_x0;
		line_info.start_y	= pos1_trigger.trigger_y1/INTER_PROG_RATIO;
		line_info.end_x 	= pos1_trigger.trigger_x1;
		line_info.end_y 	= pos1_trigger.trigger_y1/INTER_PROG_RATIO;
		draw_line(inst,buffer,&yuv_value,&line_info);
	}
	else
	{
		//»­´¥·¢¿ò£¬°åÊé ÇøÓò 
		trigger[0].trigger_x0		= input_param->dynamic_param.TrackOtherParam.blackboard_trigger[0].trigger_x0;
		trigger[0].trigger_y0		= input_param->dynamic_param.TrackOtherParam.blackboard_trigger[0].trigger_y0;
		trigger[0].trigger_x1		= input_param->dynamic_param.TrackOtherParam.blackboard_trigger[0].trigger_x1;
		trigger[0].trigger_y1		= input_param->dynamic_param.TrackOtherParam.blackboard_trigger[0].trigger_y1;
		trigger[1].trigger_x0		= input_param->dynamic_param.TrackOtherParam.blackboard_trigger[1].trigger_x0;
		trigger[1].trigger_y0		= input_param->dynamic_param.TrackOtherParam.blackboard_trigger[1].trigger_y0;
		trigger[1].trigger_x1		= input_param->dynamic_param.TrackOtherParam.blackboard_trigger[1].trigger_x1;
		trigger[1].trigger_y1		= input_param->dynamic_param.TrackOtherParam.blackboard_trigger[1].trigger_y1;
		for(j=0;j<TRACK_TRIGGER_MAX_NUM;j++)
		{
			line_info.start_x	= trigger[j].trigger_x0;
			line_info.start_y	= trigger[j].trigger_y0/INTER_PROG_RATIO;
			line_info.end_x 	= trigger[j].trigger_x1;
			line_info.end_y 	= trigger[j].trigger_y0/INTER_PROG_RATIO;
			draw_line(inst,buffer,&yuv_value,&line_info);
	
			line_info.start_x	= trigger[j].trigger_x1;
			line_info.start_y	= trigger[j].trigger_y0/INTER_PROG_RATIO;
			line_info.end_x 	= trigger[j].trigger_x1;
			line_info.end_y 	= trigger[j].trigger_y1 /INTER_PROG_RATIO;
			draw_line(inst,buffer,&yuv_value,&line_info);
	
			line_info.start_x	= trigger[j].trigger_x0;
			line_info.start_y	= trigger[j].trigger_y0/INTER_PROG_RATIO;
			line_info.end_x 	= trigger[j].trigger_x0;
			line_info.end_y 	= trigger[j].trigger_y1/INTER_PROG_RATIO;
			draw_line(inst,buffer,&yuv_value,&line_info);
	
			line_info.start_x	= trigger[j].trigger_x0;
			line_info.start_y	= trigger[j].trigger_y1/INTER_PROG_RATIO;
			line_info.end_x 	= trigger[j].trigger_x1;
			line_info.end_y 	= trigger[j].trigger_y1/INTER_PROG_RATIO;
			draw_line(inst,buffer,&yuv_value,&line_info);
		}
	}
}

/**
* @	º¯ÊýÃû³Æ: draw_msg_info()
* @ º¯Êý¹¦ÄÜ: À´ÏûÏ¢Ê±,»­¶ÔÓ¦Ïß
* @ ÊäÈë²ÎÊý: output_buf -- ÓÃÓÚ»­ÏßµÄbuf
* @	ÊäÈë²ÎÊý: input_param -- Íâ²¿ÉèÖÃµÄÊäÈë²ÎÊý
* @ ·µ»ØÖµ:	  ÎÞ
*/

static void draw_msg_info(TrackHand *inst,int8_t *output_buf) 
{
	ITRACK_Params *input_param=&(inst->input_param);
   	int32_t i,j,w,m,n,m_0,n_0,model_inc;

	int32_t	x_offset		= input_param->dynamic_param.x_offset;
	int32_t	y_offset		= input_param->dynamic_param.y_offset;

	
	int16_t	video_width		= input_param->static_param.video_width;
	int16_t	video_height	= input_param->static_param.video_height;
	int16_t	pic_width		= video_width/x_offset;
	int16_t	pic_height		= video_height/y_offset;
	int16_t	message			= input_param->dynamic_param.message;

	int32_t temp			= 0;	

	int32_t	model_sum		= input_param->dynamic_param.model_sum;

	int32_t	mid_x			= input_param->dynamic_param.mid_x;
	int32_t	mid_y			= input_param->dynamic_param.mid_y;

	int8_t	zoom_distance		= input_param->dynamic_param.zoom_distance;
	
	int32_t intAPixels 		= 0;	//»­¾ØÐÎ¿ò×óÉÏ½ÇµãµÄx×ø±ê
	int32_t intALines 		= 0;	//»­¾ØÐÎ¿ò×óÉÏ½ÇµãµÄy×ø±ê
	int32_t intDPixels 		= 0;	//»­¾ØÐÎ¿òÓÒÏÂ½ÇµãµÄx×ø±ê
	int32_t intDLines 		= 0;	//»­¾ØÐÎ¿òÓÒÏÂ½ÇµãµÄy×ø±ê
	
	yuv_value_info_t		yuv_value 	= {0};
	line_info_t 			line_info 	= {0};
	uint8_t *Point_Buffer=inst->t_global_obj.Point_Buffer;
	int32_t Point_180_Inc=inst->t_global_obj.Point_180_Inc;
	int32_t Point_90_Inc=inst->t_global_obj.Point_90_Inc;
	int32_t Point_45_Inc=inst->t_global_obj.Point_45_Inc;
	int32_t Point_135_Inc=inst->t_global_obj.Point_135_Inc;
	int32_t Point_Inc=inst->t_global_obj.Point_Inc;
	uint8_t *T_L_Buffer=inst->t_global_obj.T_L_Buffer;
	uint8_t *T_R_Buffer=inst->t_global_obj.T_R_Buffer;
	uint8_t *T_U_Buffer=inst->t_global_obj.T_U_Buffer;
	uint8_t *T_P_Buffer=inst->t_global_obj.T_P_Buffer;
	uint8_t *img_coordinate=inst->t_global_obj.img_coordinate;
	uint8_t *img_coordinate_g=inst->t_global_obj.img_coordinate_g;
	uint8_t	(*model)[MODEL_NUM_MAX] = inst->t_global_obj.model;

	if(zoom_distance < 0)
	{
		zoom_distance = 0;
	}
	if(zoom_distance >= MODEL_DISTANCE_CHOSE_NUM)
	{
		zoom_distance = MODEL_DISTANCE_CHOSE_NUM - 1;
	}
	
	m 	= 0;
	n 	= 0;
	m_0 = mid_x;
	n_0 = mid_y;
	if (message == 0x5)
	{//»­ËùÓÐ½Ç¶Èµã£¬
	
		for(i= 0;i< pic_height;i++) 
		{
			for(j= 0;j< pic_width;j++) 
			{
	
				if (img_coordinate_g[i*pic_width+j]!=0)
				{
					line_info.start_x  = j * x_offset;
					line_info.start_y = i * (y_offset/INTER_PROG_RATIO);
					line_info.end_x = (j + 2) * x_offset;
					line_info.end_y = (i + 2) * (y_offset/INTER_PROG_RATIO);
					if (img_coordinate_g[i*pic_width+j]==1)
					{
						yuv_value.y_value = 255;
						yuv_value.cb_value	= 255;
						yuv_value.cr_value	= 255;
					}
					if (img_coordinate_g[i*pic_width+j]==2)
					{
						yuv_value.y_value = 255;
						yuv_value.cb_value	= 0;
						yuv_value.cr_value	= 255;
					}
					if (img_coordinate_g[i*pic_width+j]==3)
					{
						yuv_value.y_value = 255;
						yuv_value.cb_value	= 255;
						yuv_value.cr_value	= 0;
					}
					if (img_coordinate_g[i*pic_width+j]==4)
					{
						yuv_value.y_value = 0;
						yuv_value.cb_value	= 0;
						yuv_value.cr_value	= 0;
					}
					draw_rectangle(inst,output_buf, &yuv_value, &line_info);
				}
			}
		}
		}

	if (message == 0x6)
	{//»­ËùÓÐ½Ç¶Èµã£¬
		for(i= 0;i< Point_Inc;i++) 
		{
			w = *((Uint16 *)Point_Buffer + i);//µãÀàÐÍ
			i++;
			m = *((Uint16 *)Point_Buffer + i);
			i++;
			n = *((Uint16 *)Point_Buffer + i);

			line_info.start_x  = (m - 1) * x_offset;
			line_info.start_y = (n -1) * (y_offset/INTER_PROG_RATIO);
			line_info.end_x = (m + 1) * x_offset;
			line_info.end_y = (n + 1) * (y_offset/INTER_PROG_RATIO);
			w = w & 0x0f;
			if (w == 0x01)
			{
				//yuv_value.y_value 	= 128;
				yuv_value.y_value = 0;
				yuv_value.cb_value	= 0;
				yuv_value.cr_value	= 255;
				draw_rectangle(inst,output_buf, &yuv_value, &line_info);
			}
			if (w == 0x02)
			{
				//yuv_value.y_value 	= 128;
				yuv_value.y_value = 0;
				yuv_value.cb_value	= 255;
				yuv_value.cr_value	= 0;
				draw_rectangle(inst,output_buf, &yuv_value, &line_info);

			}
			if (w == 0x03)
			{
				//yuv_value.y_value 	= 255;
				yuv_value.y_value = 0;
				yuv_value.cb_value	= 128;
				yuv_value.cr_value	= 128;
				draw_rectangle(inst,output_buf, &yuv_value, &line_info);
			}
			if (w == 0x04)
			{
				//yuv_value.y_value 	= 128;
				yuv_value.y_value = 0;
				yuv_value.cb_value	= 0;
				yuv_value.cr_value	= 0;
				draw_rectangle(inst,output_buf, &yuv_value, &line_info);
			}  

		}
	}
	
	if (message == 0x7)
	{//»­ËùÓÐµã£¬Ï¸»­
		for(i= 0;i< Point_Inc;i++) 
		{
			w = *((Uint16 *)Point_Buffer + i);
			i++;
			m = *((Uint16 *)Point_Buffer + i);
			i++;
			n = *((Uint16 *)Point_Buffer + i);

			line_info.start_x = (m - 1) * x_offset;
			line_info.start_y = (n -1) * (y_offset/INTER_PROG_RATIO);
			line_info.end_x = (m + 1) * x_offset;
			line_info.end_y = (n + 1) * (y_offset/INTER_PROG_RATIO);

			if (w == 0x11)
			{
				yuv_value.y_value 	= 255;
				//yuv_value.y_value = 0;
				yuv_value.cb_value	= 0;
				yuv_value.cr_value	= 255;
				draw_rectangle(inst,output_buf, &yuv_value, &line_info);
			}
			if (w == 0x21)
			{
				//yuv_value.y_value 	= 128;
				yuv_value.y_value = 0;
				yuv_value.cb_value	= 0;
				yuv_value.cr_value	= 255;
				draw_rectangle(inst,output_buf, &yuv_value, &line_info);
			}
			if (w == 0x12)
			{
				yuv_value.y_value 	= 255;
				//yuv_value.y_value = 0;
				yuv_value.cb_value	= 255;
				yuv_value.cr_value	= 0;
				draw_rectangle(inst,output_buf, &yuv_value, &line_info);
			}
			if (w == 0x22)
			{
				//yuv_value.y_value 	= 128;
				yuv_value.y_value = 0;
				yuv_value.cb_value	= 255;
				yuv_value.cr_value	= 0;
				draw_rectangle(inst,output_buf, &yuv_value, &line_info);
			}
			if (w == 0x03)
			{
				//yuv_value.y_value 	= 255;
				yuv_value.y_value = 0;
				yuv_value.cb_value	= 128;
				yuv_value.cr_value	= 128;
				draw_rectangle(inst,output_buf, &yuv_value, &line_info);
			}
			if (w == 0x14)
			{
				yuv_value.y_value 	= 255;
				//yuv_value.y_value = 0;
				yuv_value.cb_value	= 0;
				yuv_value.cr_value	= 0;
				draw_rectangle(inst,output_buf, &yuv_value, &line_info);
			}  
			if (w == 0x24)
			{
				//yuv_value.y_value 	= 128;
				yuv_value.y_value = 0;
				yuv_value.cb_value	= 0;
				yuv_value.cr_value	= 0;
				draw_rectangle(inst,output_buf, &yuv_value, &line_info);
			}  
		}
	}

	//ÏÔÊ¾Í¬½Ç¶ÈµãÃæ»ý
	if (message == DRAW_SLANT_LINE)
	{
		for(i= 0;i< Point_45_Inc;i++) 
		{
			i++;
			i++;
			intAPixels = *((Uint16 *)T_L_Buffer + i);
			i++;
			intALines = *((Uint16 *)T_L_Buffer + i);
			i++;
			intDPixels = *((Uint16 *)T_L_Buffer + i);
			i++;
			intDLines = *((Uint16 *)T_L_Buffer + i);

			line_info.start_x	= intAPixels;
			line_info.start_y	= intDLines;
			line_info.end_x		= intDPixels;
			line_info.end_y		= intALines;
			
			//yuv_value.y_value 	= 128;
			yuv_value.y_value = 0;
			yuv_value.cb_value 	= 0;
			yuv_value.cr_value 	= 255;
			draw_line(inst,output_buf, &yuv_value, &line_info);
		}
		for(i= 0;i< Point_135_Inc;i++) 
		{
			i++;
			i++;
			intAPixels = *((Uint16 *)T_R_Buffer + i);
			i++;
			intALines = *((Uint16 *)T_R_Buffer + i);
			i++;
			intDPixels = *((Uint16 *)T_R_Buffer + i);
			i++;
			intDLines = *((Uint16 *)T_R_Buffer + i);

			line_info.start_x	= intAPixels;
			line_info.start_y	= intALines;
			line_info.end_x		= intDPixels;
			line_info.end_y		= intDLines;
			
			//yuv_value.y_value 	= 128;
			yuv_value.y_value = 0;
			yuv_value.cb_value 	= 0;
			yuv_value.cr_value 	= 255;
			draw_line(inst,output_buf, &yuv_value, &line_info);
			
		}

		for(i= 0;i< Point_180_Inc;i++) 
		{
			i++;
			i++;
			intAPixels = *((Uint16 *)T_U_Buffer + i);
			i++;
			intALines = *((Uint16 *)T_U_Buffer + i);
			i++;
			intDPixels = *((Uint16 *)T_U_Buffer + i);
			i++;
			intDLines = *((Uint16 *)T_U_Buffer + i);

			line_info.start_x	= intAPixels;
			line_info.start_y	= intALines+(intDLines-intALines)/2;
			line_info.end_x		= intDPixels;
			line_info.end_y		= intALines+(intDLines-intALines)/2;
			
			//yuv_value.y_value 	= 128;
			yuv_value.y_value = 0;
			yuv_value.cb_value 	= 0;
			yuv_value.cr_value 	= 255;
			draw_line(inst,output_buf, &yuv_value, &line_info);
			
		}
		for(i= 0;i< Point_90_Inc;i++) 
		{
			i++;
			i++;
			intAPixels = *((Uint16 *)T_P_Buffer + i);
			i++;
			intALines = *((Uint16 *)T_P_Buffer + i);
			i++;
			intDPixels = *((Uint16 *)T_P_Buffer + i);
			i++;
			intDLines = *((Uint16 *)T_P_Buffer + i);

			line_info.start_x	= intAPixels+(intDPixels-intAPixels)/2;
			line_info.start_y	= intALines;
			line_info.end_x		= intAPixels+(intDPixels-intAPixels)/2;
			line_info.end_y		= intDLines;
			
			//yuv_value.y_value 	= 128;
			yuv_value.y_value = 0;
			yuv_value.cb_value 	= 0;
			yuv_value.cr_value 	= 255;
			draw_line(inst,output_buf, &yuv_value, &line_info);
			
		}
	}

	//ÏÔÊ¾Ä£°æ
	if (message == DRAW_MODEL)
	{
		m = mid_x;
		n = mid_y;
		line_info.start_x 	= (m - 2) * x_offset;
		line_info.start_y 	= (n -2) * (y_offset/INTER_PROG_RATIO);
		line_info.end_x 	= (m + 2) * x_offset;
		line_info.end_y 	= (n + 2) * (y_offset/INTER_PROG_RATIO);

		//yuv_value.y_value 	= 255;
		yuv_value.y_value = 0;
		yuv_value.cb_value	= 128;
		yuv_value.cr_value	= 128;
		
		draw_rectangle(inst,output_buf, &yuv_value, &line_info);
		for (model_inc= 0;model_inc< model_sum;model_inc++)
		{
			temp = model[zoom_distance][model_inc*5+1] & 0x80;
			if (temp == 0x80)
			{
				m_0 = m - (model[zoom_distance][model_inc*5+1] & 0x7f);
			}
			else
			{
				m_0 = m + (model[zoom_distance][model_inc*5+1] & 0x7f);
			}

			temp = model[zoom_distance][model_inc*5+2] & 0x80;
			if (temp == 0x80) 
			{
				n_0 = n - (model[zoom_distance][model_inc*5+2] & 0x7f);
			}
			else
			{
				n_0 = n + (model[zoom_distance][model_inc*5+2] & 0x7f);
			}

			line_info.start_x = (m_0 - model[zoom_distance][model_inc*5+3]) * x_offset;        //1
			line_info.start_y = (n_0 - model[zoom_distance][model_inc*5+4]) * (y_offset/INTER_PROG_RATIO);
			line_info.end_x = (m_0 + model[zoom_distance][model_inc*5+3]) * x_offset;
			line_info.end_y = (n_0 + model[zoom_distance][model_inc*5+4]) * (y_offset/INTER_PROG_RATIO);

			//yuv_value.y_value 	= 255;
			yuv_value.y_value = 0;
			yuv_value.cb_value	= 255;
			yuv_value.cr_value	= model_inc*25;
			draw_rectangle(inst,output_buf, &yuv_value, &line_info);
		}

	}  
	return;
} 
static void draw_target(TrackHand *inst,int8_t *output_buf) 
{
	int i;
	yuv_value_info_t		yuv_value 	= {0};
	line_info_t 			line_info 	= {0};
	ITRACK_Params *input_param=&(inst->input_param);
	STR_OBJECTS_DETECTED *pst_det_obj=&(inst->t_global_obj.pst_det_obj);
	STR_OBJECT_TRAKING *pst_trak_obj=&(inst->t_global_obj.pst_trak_obj);
	int32_t	x_offset		= input_param->dynamic_param.x_offset;
	int32_t	y_offset		= input_param->dynamic_param.y_offset;
	int8_t	zoom_distance	= input_param->dynamic_param.zoom_distance;
	uint8_t	(*model)[MODEL_NUM_MAX] = inst->t_global_obj.model;
	for(i=0;i<pst_det_obj->ucNumber;i++)
	{
		line_info.start_x = pst_det_obj->astInfo[i].sX*x_offset;
		line_info.start_y = pst_det_obj->astInfo[i].sY *y_offset;
		line_info.end_x = line_info.start_x + 64;
		line_info.end_y = line_info.start_y + 64;
		yuv_value.y_value = 255;
		yuv_value.cb_value = 255;
		yuv_value.cr_value = 255;
		if(i==1)
		{
			yuv_value.cb_value = 128;
			yuv_value.cr_value = 128;
		}
		draw_rectangle(inst,output_buf, &yuv_value, &line_info);
		
	}

}
/**
* @	º¯ÊýÃû³Æ: data_copy()
* @ º¯Êý¹¦ÄÜ: ¶ÔÍ¼Ïñ½øÐÐËõ·Å£¬²¢È¥¸ôÐÐ
* @	ÊäÈë²ÎÊý: input_param -- Íâ²¿ÉèÖÃµÄÊäÈë²ÎÊý
* @	Êä³ö²ÎÊý: ÎÞ
* @ ·µ»ØÖµ:	  ÎÞ
*/
static int32_t data_copy(TrackHand *inst, int8_t *output_buf)
{
	ITRACK_Params *input_param=&(inst->input_param);
	int32_t k					= 0;
	int16_t pic_width			= 0;
	int16_t pic_height			= 0;
	int16_t video_width 		= input_param->static_param.video_width;
	int16_t video_height		= input_param->static_param.video_height;
	int32_t 		i = 0,j 	= 0;
	int32_t 		a = 0,b 	= 0;
	int32_t x_offset		= input_param->dynamic_param.x_offset;
	int32_t y_offset		= input_param->dynamic_param.y_offset;
	int16_t reset_time		= input_param->dynamic_param.reset_time;

	int32_t curoffset		= 0;
	yuv_value_info_t		yuv_value	= {0};
	line_info_t 			line_info	= {0};
	
	uint8_t *src_y_buf	= inst->t_global_obj.src_y_buf; //´æ·ÅÒª½øÐÐÔËËãµÄÔ´ybufÊý¾Ý
	uint8_t *src_cr_buf = inst->t_global_obj.src_cr_buf; //´æ·ÅÒª½øÐÐÔËËãµÄÔ´crbufÊý¾Ý
	uint8_t *src_cb_buf = inst->t_global_obj.src_cb_buf;
	uint8_t *dst_y_buf	= inst->t_global_obj.dst_y_buf; //´æ·ÅÃ¿´Î¸ú×ÙºóµÄµÚÒ»Ö¡ybufÊý¾Ý
	uint8_t *puv;
	int32_t nSrcBufWidth=inst->t_global_obj.nSrcBufWidth;
	int32_t nSrcBufHeight=inst->t_global_obj.nSrcBufHeight;
	UInt32 xoffset	= 0;
	UInt32 yoffset	= 0;
	int offset	= nSrcBufWidth* nSrcBufHeight;
	pic_width  = video_width / input_param->dynamic_param.x_offset;
	pic_height = video_height / input_param->dynamic_param.y_offset;

	//ÓÃÓÚ×öÈËÁ³·ôÉ«Ê¶±ðµÄbuf
	k = 0;
	a = 0;
	b = 0;
	
	//dstµÄbufÓÃÓÚ´¥·¢µÄÅÐ¶Ï
	if ((inst->t_global_obj.first_time < reset_time) || inst->t_global_obj.track_start == 0)
	{
		memcpy(dst_y_buf,src_y_buf,(pic_width* pic_height));
	}	

	i = 0;
	j = 0;
	k = 0;

	
   // UInt32 time0 = Utils_getCurTimeInMsec();
	for(i = 0; i < pic_height; i++)
	{
		yoffset = 0;
		for(j = 0; j < pic_width; j++)
		{
			*(src_y_buf + a) = *(output_buf + xoffset + yoffset);
			a++;

			yoffset = yoffset + x_offset;
		}
		xoffset = xoffset + y_offset*nSrcBufWidth;
	}

	xoffset = 0;
	k = 0;
	y_offset = y_offset/2;
	for(i = 0; i < pic_height; i++)
	{
		yoffset = 0;
		for(j = 0; j < pic_width; j++)
		{	
			*(src_cr_buf + k) = *(output_buf + offset + xoffset + yoffset + 1);
			k++;
			yoffset = yoffset + x_offset;
			
		}
		xoffset = xoffset + y_offset*nSrcBufWidth;
	}

	//Ç°30Ö¡²»×ö´¦Àí
	inst->t_global_obj.first_time++;
	if (inst->t_global_obj.first_time > reset_time)
	{
		inst->t_global_obj.first_time = reset_time;
	}
	return k;
}	
/**
* @	º¯ÊýÃû³Æ: image_y_value_compare_only_switch()
* @ º¯Êý¹¦ÄÜ: È¡´¥·¢µã
* @ ÊäÈë²ÎÊý: output_buf -- ÒªÔÚÉÏÃæ¸ú×Ù¿òµÄbuffer
* @			  input_param -- Íâ²¿ÉèÖÃµÄÊäÈë²ÎÊý
* @	Êä³ö²ÎÊý: output_param
* @ ·µ»ØÖµ:	  ÎªÉãÏñÍ·Ô¤ÖÃÎ»µÄ
*/
static int32_t image_y_value_compare_only_switch(TrackHand *inst,int8_t *input_buf,int8_t *output_buf,ITRACK_OutArgs *output_param)
{
	ITRACK_Params *input_param=&(inst->input_param);
	int32_t	trigger_num[TRACK_TRIGGER_MAX_NUM] 	={0, 0};
	int32_t	trigger_black_num[TRACK_TRIGGER_MAX_NUM] 	={0, 0};
	int32_t pos=0;
	int32_t	m_position		= 0;
	int32_t	m 				= 0;
	int32_t	n				= 0;
	int16_t	pic_width		= 0;
	int16_t	pic_height		= 0;
	
	int16_t	video_width		= input_param->static_param.video_width;
	int16_t	video_height	= input_param->static_param.video_height;

	trigger_info_t trigger[TRACK_TRIGGER_MAX_NUM];
	trigger_info_t trigger_black[TRACK_TRIGGER_MAX_NUM];

	int32_t	x_offset		= input_param->dynamic_param.x_offset;
	int32_t	y_offset		= input_param->dynamic_param.y_offset;

	int16_t	reset_time		= input_param->dynamic_param.reset_time;

	yuv_value_info_t		yuv_value 	= {0};
	line_info_t 			line_info	= {0};

	float_t a,b,c;
	uint8_t *src_y_buf=inst->t_global_obj.src_y_buf; //´æ·ÅÒª½øÐÐÔËËãµÄÔ´ybufÊý¾Ý
//	uint8_t *dst_y_buf=inst->t_global_obj.dst_y_buf; //´æ·ÅÃ¿´Î¸ú×ÙºóµÄµÚÒ»Ö¡ybufÊý¾Ý
	uint8_t *cmp_y_buf=(uint8_t *)input_buf;//±£´æµÄÊý¾Ý,Ò²¾ÍÊÇÐèÒª±È½ÏµÄ±³¾°Êý¾Ý

	pic_width = video_width / x_offset;
	pic_height = video_height / y_offset;

	trigger[0].trigger_x0 		= input_param->dynamic_param.trigger[0].trigger_x0;
	trigger[0].trigger_y0 		= input_param->dynamic_param.trigger[0].trigger_y0;
	trigger[0].trigger_x1 		= input_param->dynamic_param.trigger[0].trigger_x1;
	trigger[0].trigger_y1 		= input_param->dynamic_param.trigger[0].trigger_y1;
	trigger[1].trigger_x0 		= input_param->dynamic_param.trigger[1].trigger_x0;
	trigger[1].trigger_y0 		= input_param->dynamic_param.trigger[1].trigger_y0;
	trigger[1].trigger_x1 		= input_param->dynamic_param.trigger[1].trigger_x1;
	trigger[1].trigger_y1 		= input_param->dynamic_param.trigger[1].trigger_y1;

	trigger[0].trigger_x0 = trigger[0].trigger_x0/x_offset;
	trigger[0].trigger_y0 = trigger[0].trigger_y0/y_offset;
	trigger[0].trigger_x1 = trigger[0].trigger_x1/x_offset;
	trigger[0].trigger_y1 = trigger[0].trigger_y1/y_offset;
	trigger[1].trigger_x0 = trigger[1].trigger_x0/x_offset;
	trigger[1].trigger_y0 = trigger[1].trigger_y0/y_offset;
	trigger[1].trigger_x1 = trigger[1].trigger_x1/x_offset;
	trigger[1].trigger_y1 = trigger[1].trigger_y1/y_offset;

	for(pos=0;pos<2;pos++)
	{
		for (n = trigger[pos].trigger_y0; n < trigger[pos].trigger_y1; n++)  
		{
			for (m = trigger[pos].trigger_x0; m < trigger[pos].trigger_x1; m++)  
			{
				a = *((Uint8 *)src_y_buf+n*pic_width+m);
				b = *((Uint8 *)cmp_y_buf+n*y_offset*video_width+m*x_offset);
				//*((Uint8 *)dst_y_buf + n*pic_width+m)= a;
				if(b==0)
				{
					continue;
				}
				//12ºÍ8ÊÇ¸ö¾­ÑéÖµ£¬±íÊ¾ÓÐ¸Ä±ä£¬¼´ÓÐ´¥·¢
				c = a/b;
				if (c>1.2||c<0.8)
				{
					if(DRAW_TRIGGER_TRACK == input_param->dynamic_param.message)
					{
						line_info.start_x = m * x_offset;
						line_info.start_y = n * (y_offset/INTER_PROG_RATIO);
						line_info.end_x = line_info.start_x + x_offset;
						line_info.end_y = line_info.start_y + y_offset/INTER_PROG_RATIO;

						//»­¾ØÐÎ¿ò
						yuv_value.y_value 	= 0;
						yuv_value.cb_value 	= 0;
						yuv_value.cr_value	= 0;
						draw_rectangle(inst,output_buf, &yuv_value, &line_info);
					}

					trigger_num[pos]++;
				}
			}     
		}
	}

	trigger_black[0].trigger_x0 		= input_param->dynamic_param.TrackOtherParam.blackboard_trigger[0].trigger_x0;
	trigger_black[0].trigger_y0 		= input_param->dynamic_param.TrackOtherParam.blackboard_trigger[0].trigger_y0;
	trigger_black[0].trigger_x1 		= input_param->dynamic_param.TrackOtherParam.blackboard_trigger[0].trigger_x1;
	trigger_black[0].trigger_y1 		= input_param->dynamic_param.TrackOtherParam.blackboard_trigger[0].trigger_y1;
	trigger_black[1].trigger_x0 		= input_param->dynamic_param.TrackOtherParam.blackboard_trigger[1].trigger_x0;
	trigger_black[1].trigger_y0 		= input_param->dynamic_param.TrackOtherParam.blackboard_trigger[1].trigger_y0;
	trigger_black[1].trigger_x1 		= input_param->dynamic_param.TrackOtherParam.blackboard_trigger[1].trigger_x1;
	trigger_black[1].trigger_y1 		= input_param->dynamic_param.TrackOtherParam.blackboard_trigger[1].trigger_y1;

	trigger_black[0].trigger_x0 = trigger_black[0].trigger_x0/x_offset;
	trigger_black[0].trigger_y0 = trigger_black[0].trigger_y0/y_offset;
	trigger_black[0].trigger_x1 = trigger_black[0].trigger_x1/x_offset;
	trigger_black[0].trigger_y1 = trigger_black[0].trigger_y1/y_offset;
	trigger_black[1].trigger_x0 = trigger_black[1].trigger_x0/x_offset;
	trigger_black[1].trigger_y0 = trigger_black[1].trigger_y0/y_offset;
	trigger_black[1].trigger_x1 = trigger_black[1].trigger_x1/x_offset;
	trigger_black[1].trigger_y1 = trigger_black[1].trigger_y1/y_offset;

//Ä¿Ç°Ö»ÓÐÒ»¸ö°åÊé¾µÍ·
	for(pos=0;pos<1;pos++)
	{
		for (n = trigger_black[pos].trigger_y0; n < trigger_black[pos].trigger_y1; n++)  
		{
			for (m = trigger_black[pos].trigger_x0; m < trigger_black[pos].trigger_x1; m++)  
			{
				a = *((Uint8 *)src_y_buf+n*pic_width+m);
				b = *((Uint8 *)cmp_y_buf+n*y_offset*video_width+m*x_offset);
				//*((Uint8 *)dst_y_buf + n*pic_width+m)= a;
				if(b==0)
				{
					continue;
				}
				//12ºÍ8ÊÇ¸ö¾­ÑéÖµ£¬±íÊ¾ÓÐ¸Ä±ä£¬¼´ÓÐ´¥·¢
				c = a/b;
				if (c>1.2||c<0.8)
				{
					if(DRAW_TRIGGER_TRACK == input_param->dynamic_param.message)
					{
						line_info.start_x = m * x_offset;
						line_info.start_y = n * (y_offset/INTER_PROG_RATIO);
						line_info.end_x = line_info.start_x + x_offset;
						line_info.end_y = line_info.start_y + y_offset/INTER_PROG_RATIO;

						//»­¾ØÐÎ¿ò
						yuv_value.y_value 	= 255;
						yuv_value.cb_value 	= 255;
						yuv_value.cr_value	= 255;
						draw_rectangle(inst,output_buf, &yuv_value, &line_info);
					}
					trigger_black_num[pos]++;
				}
			}     
		}
	}	
	//T_p_sum±íÊ¾ÔÚ´¥·¢ÇøÓòÓÐ¼¸¸öµã±»´¥·¢ÁË£¬ÊÇ¸ö·§Öµ£¬µ±p´óÓÚÕâ¸öÖµÊ±£¬ÎÒÃÇ¾ÍÈÏÎªÒÑ¾­´¥·¢ÁË	
	if ((trigger_num[0] >= input_param->dynamic_param.trigger_sum)
		||(trigger_num[1] >= input_param->dynamic_param.trigger_sum))
	{ //·ÇºÚ°åÇøÓò´¥·¢ £¬ÎÞÂÛºÚ°åÊÇ·ñ´¥·¢¶¼ÏÔÊ¾Îª1
		output_param->cam_position= 1;
		output_param->track_status = 1;
		inst->t_global_obj.last_track_status=1;
		inst->t_global_obj.nLostNum=0;
	}
	else if (trigger_black_num[0] >= input_param->dynamic_param.trigger_sum)
	{ //ºÚ°åÇøÓò´¥·¢ 
		output_param->cam_position= 2;
		output_param->track_status = 1;
		inst->t_global_obj.last_track_status=1;
		inst->t_global_obj.nLostNum=0;
	}
	else
	{
		output_param->cam_position= 0;
		output_param->track_status = 0;
	}
	if(output_param->track_status==0)
	{
		if(inst->t_global_obj.last_track_status!=0)
		{
			inst->t_global_obj.nLostNum++;
			if(inst->t_global_obj.nLostNum>=TK_LOST_FRAME_LEVEL2)
			{//¶ªÊ§´ÎÊý´óÓÚ60f ÈÏÎªÊÇÕæÕý¶ªÁË
				inst->t_global_obj.last_track_status=0;
			}
			else
			{//ÔÚ60f ÄÚ²»Ëã¶ªÊ§
				output_param->track_status = 1;
			}
		}
	}
	output_param->move_direction	= CAMERA_MOVE_STOP;
	output_param->cmd_type			= CAMERA_MOVE_NOT;
	return 0;
}

/**
* @	º¯ÊýÃû³Æ: image_y_value_compare()
* @ º¯Êý¹¦ÄÜ: È¡´¥·¢µã
* @ ÊäÈë²ÎÊý: output_buf -- ÒªÔÚÉÏÃæ¸ú×Ù¿òµÄbuffer
* @			  input_param -- Íâ²¿ÉèÖÃµÄÊäÈë²ÎÊý
* @	Êä³ö²ÎÊý: output_param
* @ ·µ»ØÖµ:	  ÎªÉãÏñÍ·Ô¤ÖÃÎ»µÄ
*/
static int32_t image_y_value_compare(TrackHand *inst,int8_t *output_buf,ITRACK_OutArgs *output_param)
{
	ITRACK_Params *input_param=&(inst->input_param);
	int32_t	k 				= 0;
	int32_t	w 				= 0;
	int32_t	trigger_num[TRACK_TRIGGER_MAX_NUM] 	={0, 0};
	int32_t	MvNum[3] 	={0, 0, 0};
	int32_t pos=0;
	int32_t	m_position		= 0;
	int32_t	temp1			= 0;
	int32_t	temp2			= 0;
	int32_t	m 				= 0;
	int32_t	n				= 0;
	int16_t	pic_width		= 0;
	int16_t	pic_height		= 0;
	
	int16_t	video_width		= input_param->static_param.video_width;
	int16_t	video_height	= input_param->static_param.video_height;

	int32_t	CmpMvWidth		= 0;
	int32_t	CmpMvHeight		= 0;
	int32_t nCurPos1MvFlag =0;

	trigger_info_t trigger[TRACK_TRIGGER_MAX_NUM];
	trigger_info_t pos1_trigger=input_param->dynamic_param.TrackOtherParam.pos1_trigger;
	
	int32_t	x_offset		= input_param->dynamic_param.x_offset;
	int32_t	y_offset		= input_param->dynamic_param.y_offset;

	int16_t	reset_time		= input_param->dynamic_param.reset_time;

	yuv_value_info_t		yuv_value 	= {0};
	line_info_t 			line_info	= {0};

	int32_t	ret				= 0;

	float_t a,b,c;
	uint8_t *src_y_buf=inst->t_global_obj.src_y_buf; //´æ·ÅÒª½øÐÐÔËËãµÄÔ´ybufÊý¾Ý
	uint8_t *dst_y_buf=inst->t_global_obj.dst_y_buf; //´æ·ÅÃ¿´Î¸ú×ÙºóµÄµÚÒ»Ö¡ybufÊý¾Ý

	pic_width = video_width / x_offset;
	pic_height = video_height / y_offset;

	trigger[0].trigger_x0 		= input_param->dynamic_param.trigger[0].trigger_x0;
	trigger[0].trigger_y0 		= input_param->dynamic_param.trigger[0].trigger_y0;
	trigger[0].trigger_x1 		= input_param->dynamic_param.trigger[0].trigger_x1;
	trigger[0].trigger_y1 		= input_param->dynamic_param.trigger[0].trigger_y1;
	trigger[1].trigger_x0 		= input_param->dynamic_param.trigger[1].trigger_x0;
	trigger[1].trigger_y0 		= input_param->dynamic_param.trigger[1].trigger_y0;
	trigger[1].trigger_x1 		= input_param->dynamic_param.trigger[1].trigger_x1;
	trigger[1].trigger_y1 		= input_param->dynamic_param.trigger[1].trigger_y1;

	trigger[0].trigger_x0 = trigger[0].trigger_x0/x_offset;
	trigger[0].trigger_y0 = trigger[0].trigger_y0/y_offset;
	trigger[0].trigger_x1 = trigger[0].trigger_x1/x_offset;
	trigger[0].trigger_y1 = trigger[0].trigger_y1/y_offset;
	trigger[1].trigger_x0 = trigger[1].trigger_x0/x_offset;
	trigger[1].trigger_y0 = trigger[1].trigger_y0/y_offset;
	trigger[1].trigger_x1 = trigger[1].trigger_x1/x_offset;
	trigger[1].trigger_y1 = trigger[1].trigger_y1/y_offset;
	pos1_trigger.trigger_x0 = pos1_trigger.trigger_x0/x_offset;
	pos1_trigger.trigger_y0 = pos1_trigger.trigger_y0/y_offset;
	pos1_trigger.trigger_x1 = pos1_trigger.trigger_x1/x_offset;
	pos1_trigger.trigger_y1 = pos1_trigger.trigger_y1/y_offset;
	CmpMvWidth=(pos1_trigger.trigger_x1-pos1_trigger.trigger_x0)/3;
	CmpMvHeight=pos1_trigger.trigger_y1-pos1_trigger.trigger_y0;
	w = pic_width;
#if 1
	//Òò±¾º¯ÊýÖ÷ÒªÊÇÔÚ´¥·¢µãÄÚÕÒµã£¬ËùÒÔµ±ÒÑ¾­¸ú×ÙÉÏºó£¬´Ëº¯ÊýÖ±½Ó·µ»Ø
	if ((inst->t_global_obj.first_time < reset_time) || (inst->t_global_obj.track_start == 1) 
		|| (inst->t_global_obj.turn_flag != 0))
	{
		return NOT_TRIGGER;		       
	}
#endif
	for (n = 0; n < pic_height; n++)  
	{
		for (m = 0; m < pic_width; m++,k++)  
		{
			//ÅÐ¶ÏµãÊôÓÚÄÄ¸ö´¥·¢¿ò,Á½¸ö¶¼²»ÊôÓÚ¾Í·µ»Ø
			if (m >= trigger[0].trigger_x0 && m <= trigger[0].trigger_x1 && n >= trigger[0].trigger_y0 && n <= trigger[0].trigger_y1 )
			{
				pos=0;
			}
			else if(m >= trigger[1].trigger_x0 && m <= trigger[1].trigger_x1 && n >= trigger[1].trigger_y0 && n <= trigger[1].trigger_y1 )
			{
				pos=1;
			}
			else
			{
				continue;
			}
			a = *((Uint8 *)src_y_buf+ k);
			b = *((Uint8 *)dst_y_buf+ k);
			if(b==0)
			{
				continue;
			}
			//12ºÍ8ÊÇ¸ö¾­ÑéÖµ£¬±íÊ¾ÓÐ¸Ä±ä£¬¼´ÓÐ´¥·¢
			c = a/b;
			if (c>1.2||c<0.8)
			{
				if(DRAW_TRIGGER_TRACK == input_param->dynamic_param.message)
				{
					line_info.start_x = m * x_offset;
					line_info.start_y = n * (y_offset/INTER_PROG_RATIO);
					line_info.end_x = line_info.start_x + x_offset;
					line_info.end_y = line_info.start_y + y_offset/INTER_PROG_RATIO;
	
					//»­¾ØÐÎ¿ò
					yuv_value.y_value	= 0;
					yuv_value.cb_value	= 0;
					yuv_value.cr_value	= 0;
					draw_rectangle(inst,output_buf, &yuv_value, &line_info);
				}
	
				trigger_num[pos]++;
			}	  
		}
	}
	
	//T_p_sum±íÊ¾ÔÚ´¥·¢ÇøÓòÓÐ¼¸¸öµã±»´¥·¢ÁË£¬ÊÇ¸ö·§Öµ£¬µ±p´óÓÚÕâ¸öÖµÊ±£¬ÎÒÃÇ¾ÍÈÏÎªÒÑ¾­´¥·¢ÁË	
	if (trigger_num[0] >= input_param->dynamic_param.trigger_sum)
	{ 
		inst->t_global_obj.turn_flag = 1;
		output_param->cmd_type = CAM_CTRL_POSITION;
		if(trigger_num[1] >= input_param->dynamic_param.trigger_sum)
		{
			ret = TRIGGER_POSITION3;
		}
		else
		{
			if((trigger[0].trigger_y0<trigger[1].trigger_y0))
			{
				ret = TRIGGER_POSITION3;
			}
			else
			{
				ret = TRIGGER_POSITION2;
			}
		}
	}
	else
	{ 
		if(trigger_num[1] >= input_param->dynamic_param.trigger_sum)
		{
			inst->t_global_obj.turn_flag = 1;
			output_param->cmd_type = CAM_CTRL_POSITION;
			if((trigger[1].trigger_y0<trigger[0].trigger_y0))
			{
				ret = TRIGGER_POSITION3;
			}
			else
			{
				ret = TRIGGER_POSITION2;
			}
		}
		else
		{
			ret = NOT_TRIGGER;
		}
	}
	
	output_param->cam_position = ret;
	nCurPos1MvFlag=0;
	for(pos=0;pos<3;pos++)
	{
		for (n = pos1_trigger.trigger_y0; n < pos1_trigger.trigger_y1; n++)  
		{
			for (m = pos1_trigger.trigger_x0+pos*CmpMvWidth; m < pos1_trigger.trigger_x0+(pos+1)*CmpMvWidth; m++)  
			{
				k=n*pic_width+m;
				a = *((Uint8 *)src_y_buf+ k);
				b = *((Uint8 *)dst_y_buf+ k);
				if(b==0)
				{
					continue;
				}
				//12ºÍ8ÊÇ¸ö¾­ÑéÖµ£¬±íÊ¾ÓÐ¸Ä±ä£¬¼´ÓÐ´¥·¢
				c = a/b;
				if (c>1.2||c<0.8)
				{
					if(DRAW_TRIGGER_TRACK == input_param->dynamic_param.message)
					{
						line_info.start_x = m * x_offset;
						line_info.start_y = n * (y_offset/INTER_PROG_RATIO);
						line_info.end_x = line_info.start_x + x_offset;
						line_info.end_y = line_info.start_y + y_offset/INTER_PROG_RATIO;
		
						//»­¾ØÐÎ¿ò
						yuv_value.y_value	= 255;
						yuv_value.cb_value	= 127*pos;
						yuv_value.cr_value	= 127*pos;
						draw_rectangle(inst,output_buf, &yuv_value, &line_info);
					}
		
					MvNum[pos]++;
				}	  
			}
		}
		
		if(MvNum[pos]>= input_param->dynamic_param.TrackOtherParam.pos1_trigger_num)
		{
			nCurPos1MvFlag=1;
			inst->t_global_obj.nPos1MvFlag=1;
			inst->t_global_obj.nPos1MvLost=0;
			if(DRAW_TRIGGER_TRACK == input_param->dynamic_param.message)
			{
				line_info.start_x = (pos1_trigger.trigger_x0+pos*CmpMvWidth) * x_offset;
				line_info.start_y = (pos1_trigger.trigger_y0+4) * y_offset;
				line_info.end_x = line_info.start_x + 64;
				line_info.end_y = line_info.start_y + 64;
				
				//»­¾ØÐÎ¿ò
				yuv_value.y_value	= 255;
				yuv_value.cb_value	= 0;
				yuv_value.cr_value	= 128;
				draw_rectangle(inst,output_buf, &yuv_value, &line_info);
			}
			break;
		}
	}
	
	if(nCurPos1MvFlag==0)
	{
		inst->t_global_obj.nPos1MvLost++;
		if(inst->t_global_obj.nPos1MvLost>=150)
		{
			inst->t_global_obj.nPos1MvLost=0;
			inst->t_global_obj.nPos1MvFlag=0;
		}
	}
	return ret;
}


/**
* @	º¯ÊýÃû³Æ: check_image_key_point()
* @ º¯Êý¹¦ÄÜ: ÔÚÊäÈëÍ¼ÏñÖÐ²éµã£¬·Ö±ðÕÒ³ö45¶È£¬135¶È£¬90¶È£¬180¶È
* @ ÊäÈë²ÎÊý: in -- ÊäÈëµÄÍ¼ÏñÊý¾Ý
* @			  input_param Íâ²¿ÉèÖÃµÄÊäÈë²ÎÊý
* @			  img_flag	-- Í¼ÏñÀàÐÍ±êÖ¾£¬YÍ¼ÏñµÈÓÚ0£¬ÆäËüµÈÓÚ1
* @	Êä³ö²ÎÊý: ÎÞ
*/
static int32_t check_image_key_point(TrackHand *inst,const unsigned char *restrict in,int32_t	img_flag)
{
	ITRACK_Params *input_param=&(inst->input_param);
    int32_t j,G[POINT_VIEW_NUM];
    int32_t G0,G1_0,G2_0,G3_0,G4_0;
    int32_t  i,w,Grad;
    int32_t i00,      i02,      i04;
    int32_t      i11, i12, i13;
    int32_t i20, i21, i22, i23, i24;
	int32_t      i31, i32, i33;
	int32_t i40,      i42,      i44;
	float_t a,b,c;
	int16_t	pic_width		= 0;
	int16_t	pic_height		= 0;

	int32_t	video_width		= input_param->static_param.video_width;
	int32_t	video_height	= input_param->static_param.video_height;

	int32_t	x_offset		= input_param->dynamic_param.x_offset;
	int32_t	y_offset		= input_param->dynamic_param.y_offset;
	int32_t	sens			= input_param->dynamic_param.sens;
	
	int32_t		m			= 0;
	int32_t		n			= 0;
	uint8_t *Area_vertex=inst->t_global_obj.Area_vertex; 
	uint8_t *img_coordinate=inst->t_global_obj.img_coordinate;
	uint8_t *img_coordinate_g=inst->t_global_obj.img_coordinate_g;
	a = sens;
	
	if (1 == img_flag)
	{
		a = 10;
	}
	
	b = 100;
	c = a/b;
	
    pic_width = video_width/x_offset;
	pic_height = video_height/y_offset;
	
    w = pic_width;
    for (i = 0; i < pic_width*(pic_height-3) - 2; i++)
    {
	    m = i%w;
        n = i/w;

	    if (*((Uint8 *)Area_vertex + (n+3)* (y_offset/INTER_PROG_RATIO)*video_width+ (m+1)* x_offset) == 0)
	    {
			continue;
	    }
		
        if (*((Uint8 *)Area_vertex + (n+4)* (y_offset/INTER_PROG_RATIO)*video_width+ (m+2)* x_offset) == 0)
        {
			continue;
        }

        i00=in[i    ];                  i02=in[i    +2];                 i04=in[i    +4];
                       i11=in[i+  w+1]; i12=in[i+  w+2];i13=in[i+  w+3]; 
        i20=in[i+2*w]; i21=in[i+2*w+1]; i22=in[i+2*w+2];i23=in[i+2*w+3]; i24=in[i+2*w+4];
                       i31=in[i+3*w+1]; i32=in[i+3*w+2];i33=in[i+3*w+3]; 
	    i40=in[i+4*w];                  i42=in[i+4*w+2];                 i44=in[i+4*w+4];
		if (img_flag == 0)
		{
			*((Uint8*)img_coordinate +i+2*w+2) = 0;
			*((Uint8*)img_coordinate_g+i+2*w+2) = 0;
		}
		
        G[0] = abs(- 2*i11 - i12 - i21   // 45
                 + i32 + 2*i33 + i23);

        G[1] = abs(- 2*i13 - i12 - i23  // 135
                 + i21 + 2*i31 + i32);

        G[2] = abs(- 2*i12 - i11 - i13  // 180
                 + i31 + 2*i32 + i33);

        G[3] = abs(- 2*i23 - i13 - i33  //90
                 + i11 + 2*i21 + i31);


        G1_0 = abs(- 2*i00 + 2*i44); // 45
                
        G2_0 = abs(- 2*i04 + 2*i40);   // 135
                   
        G3_0 = abs(- 2*i02 + 2*i42);   // 180
                  
        G4_0 = abs(- 2*i20 + 2*i24);   // 90
                  
        Grad = 0;
        G0 = 0;
		for (j=0;j<POINT_VIEW_NUM;j++)
	    {
		 	if (G0 <= G[j])
		    {
				 G0 = G[j];
				 Grad = j+1;
            }
        }

		if (Grad > POINT_VIEW_NUM)
		{
			continue;
		}

		a = i22;
		b = a * c; 


		if (G0 < b)
		{
			continue;
		}
                       
		if (Grad == 1)   //45
		{  
			if (G1_0 > b)
			{
				*((Uint8 *)img_coordinate+i+2*w+2) = 1;
				*((Uint8 *)img_coordinate_g+i+2*w+2) = 1;
			}
		}
		if (Grad == 2)   //135
		{  
			if (G2_0 > b)
			{
				*((Uint8 *)img_coordinate+i+2*w+2) = 2;
				*((Uint8 *)img_coordinate_g+i+2*w+2) = 2;
			}
		}

		if (Grad == 3)   //180
		{  
			if (G3_0 > b)
			{
				*((Uint8 *)img_coordinate+i+2*w+2) = 3;
				*((Uint8 *)img_coordinate_g+i+2*w+2) = 3;
			}
		}
		if (Grad == 4)   //90
		{  
			if (G4_0 > b)
			{
				*((Uint8 *)img_coordinate+i+2*w+2) = 4;
				*((Uint8 *)img_coordinate_g+i+2*w+2) = 4;
			}
		}
                
    }
	
    return TRUE;
}


/**
* @	º¯ÊýÃû³Æ: image_g_extremum()
* @ º¯Êý¹¦ÄÜ: »­45¶È£¬90¶È£¬135¶È£¬180¶ÈµÄµã
* @ ÊäÈë²ÎÊý: output_buf -- ÒªÔÚÉÏÃæ¸ú×Ù¿òµÄbuffer
* @			  input_param -- Íâ²¿ÉèÖÃµÄÊäÈë²ÎÊý
* @	Êä³ö²ÎÊý: ÎÞ
* @ ·µ»ØÖµ:	  ÎÞ
*/
static void image_g_extremum(TrackHand *inst,int8_t *output_buf)
{
	ITRACK_Params *input_param=&(inst->input_param);
    int32_t	k, w;
	int16_t	pic_width		= 0;
	int16_t	pic_height		= 0;

	int32_t	video_width 	= input_param->static_param.video_width;
	int32_t	video_height	= input_param->static_param.video_height;

	int32_t	x_offset		= input_param->dynamic_param.x_offset;
	int32_t	y_offset		= input_param->dynamic_param.y_offset;

	int32_t	m 				= 0;
	int32_t	n 				= 0;

	int16_t	message			= input_param->dynamic_param.message;

	yuv_value_info_t		yuv_value 	= {0};
	line_info_t 			line_info	= {0};
	
	uint8_t *Area_vertex=inst->t_global_obj.Area_vertex; 
	uint8_t *img_coordinate=inst->t_global_obj.img_coordinate;
    pic_width = video_width/x_offset;
	pic_height = video_height/y_offset;
    w = pic_width;

	//messageµÄ¶¼ÊÇ»­¿ò£¬ÏÔÊ¾ËùÓÐ±ßÔµµã×´Ì¬
	if (message == 0xf1)
	{
		for (k = pic_width; k < pic_width*pic_height; k++)  
		{
			m = k%w;
			n = k/w;
			//Èç¹ûµã²»ÔÚ¸ú×ÙÇøÓòÔò·µ»Ø
			if (*((Uint8 *)Area_vertex + (n+1)* (y_offset/INTER_PROG_RATIO)*video_width+ (m+1)* x_offset) == 0)
				continue;
			
			
			//»­45¶ÈµÄµã£¬ÓÉÓÚm£¬n¶¼ÊÇÔÚËõ·Å¹ýµÄÍ¼ÏñÉÏÕÒµÄµã£¬¶øÕæÕý»­µÄµãÊÇÔÚÔ­Í¼ÉÏ
			//Òò´ËÒª°ÑÒ»¸öµã±ä³ÉÒ»¸ö¿ò
			if (*((Uint8 *)img_coordinate+k) == 1)
			{  
				line_info.start_x = m * x_offset;			//ÆðÊ¼µãÎªm£¬n
				line_info.start_y = n * (y_offset/INTER_PROG_RATIO);
				line_info.end_x = line_info.start_x + x_offset;		//ÖÕµãÎªm£¬n¼ÓÉÏËõ·ÅµÄ±¶Êý
				line_info.end_y = line_info.start_y + (y_offset/INTER_PROG_RATIO);	//ÓÉÓÚÊÇ¸ôÐÐµÄ£¬ËùÒÔyÖµ³ýÒÔ2

				//yuv_value.y_value 	= 128;
				yuv_value.y_value = 0;
				yuv_value.cb_value	= 0;
				yuv_value.cr_value	= 255;
				draw_rectangle(inst,output_buf, &yuv_value, &line_info);
			}

			//»­135¶ÈµÄµã
			if (*((Uint8 *)img_coordinate+k) == 2)
			{  
				line_info.start_x = m * x_offset;
				line_info.start_y = n * (y_offset/INTER_PROG_RATIO);
				line_info.end_x = line_info.start_x + x_offset;
				line_info.end_y = line_info.start_y + (y_offset/INTER_PROG_RATIO);

				//yuv_value.y_value 	= 128;
				yuv_value.y_value = 0;
				yuv_value.cb_value	= 255;
				yuv_value.cr_value	= 0;
				draw_rectangle(inst,output_buf, &yuv_value, &line_info);

			}

			//»­180¶ÈµÄµã
			if (*((Uint8 *)img_coordinate+k) == 3)
			{  
				line_info.start_x = m * x_offset;
				line_info.start_y = n * (y_offset/INTER_PROG_RATIO);
				line_info.end_x = line_info.start_x + x_offset;
				line_info.end_y = line_info.start_y + (y_offset/INTER_PROG_RATIO);

				//yuv_value.y_value 	= 255;
				yuv_value.y_value = 0;
				yuv_value.cb_value	= 128;
				yuv_value.cr_value	= 128;
				draw_rectangle(inst,output_buf, &yuv_value, &line_info);

			}

			//»­90¶ÈµÄµã
			if (*((Uint8 *)img_coordinate+k) == 4)
			{  
				line_info.start_x = m * x_offset;
				line_info.start_y = n * (y_offset/INTER_PROG_RATIO);
				line_info.end_x = line_info.start_x + x_offset;
				line_info.end_y = line_info.start_y + (y_offset/INTER_PROG_RATIO);

				//yuv_value.y_value 	= 128;
				yuv_value.y_value = 0;
				yuv_value.cb_value	= 255;
				yuv_value.cr_value	= 128;
				draw_rectangle(inst,output_buf, &yuv_value, &line_info);
			}
			
		}
	}
}

/**
* @	º¯ÊýÃû³Æ: image_arc_position()
* @ º¯Êý¹¦ÄÜ: ¼ÆËã³öÈËÍ·ÏñµÄ45¶È£¬90¶È£¬135¶È£¬180¶ÈµÄÏß
* @	ÊäÈë²ÎÊý: input_param -- Íâ²¿ÉèÖÃµÄÊäÈë²ÎÊý
* @	Êä³ö²ÎÊý: ÎÞ
* @ ·µ»ØÖµ:	  ÎÞ
*/
static void image_arc_position(TrackHand *inst)
{
  
  ITRACK_Params *input_param=&(inst->input_param);
    int32_t i,w,m,n,j,k,add_flag;


	int16_t	pic_width		= 0;
	int16_t	pic_height		= 0;

	int32_t	video_width 	= input_param->static_param.video_width;
	int32_t	video_height	= input_param->static_param.video_height;

	int32_t	x_offset		= input_param->dynamic_param.x_offset;
	int32_t	y_offset		= input_param->dynamic_param.y_offset;

	int8_t	zoom_distance	= input_param->dynamic_param.zoom_distance;

	int32_t	Total_Point_W_L_45		= 0;
    int32_t	Total_Point_W_R_45		= 0;
    int32_t	Total_Point_H_U_45		= 0;
    int32_t	Total_Point_H_D_45		= 0;

	int32_t	Total_Point_W_L_135		= 0;
    int32_t	Total_Point_W_R_135		= 0;
    int32_t	Total_Point_H_U_135		= 0;
    int32_t	Total_Point_H_D_135		= 0;

	int32_t	Total_Point_W_L_180		= 0;
    int32_t	Total_Point_W_R_180		= 0;
    int32_t	Total_Point_H_U_180		= 0;
    int32_t	Total_Point_H_D_180		= 0;

	int32_t	Total_Point_W_L_90		= 0;
    int32_t	Total_Point_W_R_90		= 0;
    int32_t	Total_Point_H_U_90		= 0;
    int32_t	Total_Point_H_D_90		= 0;

	int32_t D_45_Inc				= 0;
	int32_t	D_135_Inc				= 0;
	int32_t	D_180_Inc				= 0;
	int32_t	D_90_Inc				= 0;

	int32_t	Direction_45			= 0;
	int32_t	Direction_135			= 0;
	int32_t	Direction_90			= 0;

	int32_t intAPixels 		= 0;	//»­¾ØÐÎ¿ò×óÉÏ½ÇµãµÄx×ø±ê
	int32_t intALines 		= 0;	//»­¾ØÐÎ¿ò×óÉÏ½ÇµãµÄy×ø±ê
	int32_t intDPixels 		= 0;	//»­¾ØÐÎ¿òÓÒÏÂ½ÇµãµÄx×ø±ê
	int32_t intDLines 		= 0;	//»­¾ØÐÎ¿òÓÒÏÂ½ÇµãµÄy×ø±ê
  
  uint8_t *Area_vertex=inst->t_global_obj.Area_vertex; 
  uint8_t *img_coordinate=inst->t_global_obj.img_coordinate;
  uint8_t *img_coordinate_g=inst->t_global_obj.img_coordinate_g;

  uint8_t *Point_Buffer=inst->t_global_obj.Point_Buffer;
  int32_t Point_180_Inc=inst->t_global_obj.Point_180_Inc;
  int32_t Point_90_Inc=inst->t_global_obj.Point_90_Inc;
  int32_t Point_45_Inc=inst->t_global_obj.Point_45_Inc;
  int32_t Point_135_Inc=inst->t_global_obj.Point_135_Inc;
  int32_t Point_Inc=inst->t_global_obj.Point_Inc;
  uint8_t *T_L_Buffer=inst->t_global_obj.T_L_Buffer;
  uint8_t *T_R_Buffer=inst->t_global_obj.T_R_Buffer;
  uint8_t *T_U_Buffer=inst->t_global_obj.T_U_Buffer;
  uint8_t *T_P_Buffer=inst->t_global_obj.T_P_Buffer;
  uint8_t (*model)[MODEL_NUM_MAX] = inst->t_global_obj.model;
	pic_width = video_width/x_offset;
	pic_height = video_height/y_offset;
	
    w = pic_width;
       
    Point_45_Inc = 0;
    Point_135_Inc = 0;
    Point_180_Inc = 0; 
    Point_90_Inc = 0;   
    Point_Inc = 0; 
    for (k = pic_width; k < pic_width*pic_height; k++)  
	{
		m = k%w;
		n = k/w;

		//µã²»ÔÚ¸ú×ÙÇøÓò½«ÅÐ¶ÏÏÂÒ»¸öµã
		if (*((Uint8 *)Area_vertex + (n+1)* (y_offset/INTER_PROG_RATIO)*video_width+ (m+1)* x_offset) == 0)
		{
			continue;
		}
		
		if (*((Uint8 *)Area_vertex + (n+3)* (y_offset/INTER_PROG_RATIO)*video_width+ (m+3)* x_offset) == 0)
		{
			continue;
		}

		add_flag = 1;
		Total_Point_W_L_45 = 0;
		Total_Point_W_R_45 = 0;
		Total_Point_H_U_45 = 0;
		Total_Point_H_D_45 = 0;

		D_180_Inc = 0;
		D_90_Inc = 0; 

		//ÅÐ¶Ï45¶Èµã
		if (*((Uint8 *)img_coordinate+k) == 1)
		{
			//Èç¹û45¶ÈµãµÄÓÒ±ßµã»¹ÊÇ45¶Èµã£¬ÔòÈÏÎª´Ëµã²»ÊÇÈËÁ³µÄ×ó±ßµÄ45¶Èµã£¬¼ÌÐøÏòÏÂÑ°ÕÒ
			if (*((Uint8 *)img_coordinate + w * n + m +1) == 1)
			{
				continue;
			}
			

			//ÒªÅÐ¶Ï45¶ÈµãÓÒ±ßµÄ¹Ø¼üµãµÄ9¸öµãÊÇ·ñ¶¼ÊÇ180¶Èµã£¬ÊÇµÄ»°¾Í×ö¸öÍ³¼Æ£¬ÓÃÓÚ¸¨Öú
			//ÅÐ¶ÏÕâ¸ö45µãÊÇ·ñÊÇÕæÕýµÄ45¶Èµã
			if (*((Uint8 *)img_coordinate_g + w * n + m +1) == 3) 
				D_180_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * n + m +2) == 3) 
				D_180_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * n + m +3) == 3) 
				D_180_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n-1) + m +1) == 3) 
				D_180_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n-1) + m +2) == 3) 
				D_180_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n-1) + m +3) == 3) 
				D_180_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n+1) + m +1) == 3) 
				D_180_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n+1) + m +2) == 3) 
				D_180_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n+1) + m +3) == 3) 
				D_180_Inc++;

			//ÒªÅÐ¶Ï45¶ÈµãÓÒ±ßµÄ¹Ø¼üµãµÄ9¸öµãÊÇ·ñ¶¼ÊÇ90¶Èµã£¬ÊÇµÄ»°¾Í×ö¸öÍ³¼Æ£¬ÓÃÓÚ¸¨Öú
			//ÅÐ¶ÏÕâ¸ö45µãÊÇñÊÇÕæÕýµÄ45¶Èµã
			if (*((Uint8 *)img_coordinate_g + w * (n-1) + m) == 4) 
				D_90_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n-1) + m -1) == 4) 
				D_90_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n-1) + m +1) == 4) 
				D_90_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n-2) + m) == 4) 
				D_90_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n-2) + m -1) == 4) 
				D_90_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n-2) + m +1) == 4) 
				D_90_Inc++;
			if (*((Uint8 *)img_coordinate_g + w * (n-3) + m) == 4) 
				D_90_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n-3) + m -1) == 4) 
				D_90_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n-3) + m +1) == 4) 
				D_90_Inc++;

			Direction_45 = 0x00;
			if (D_180_Inc > 1 && D_180_Inc >  D_90_Inc)
			{
				Direction_45 = 0x10;
			}
			
			if (D_90_Inc > 1 && D_180_Inc <  D_90_Inc)
			{
				Direction_45 = 0x20;
			}

			i = n;
			j = m;
			Total_Point_W_L_45 = j;
			Total_Point_W_R_45 = j;
			Total_Point_H_U_45 = i;
			Total_Point_H_D_45 = i;
			
			while(add_flag == 1)
			{
				add_flag = 0;
				if (*((Uint8 *)img_coordinate + w * (i+1) + j - 1) == 1)
				{
					*((Uint8 *)img_coordinate + w * (i+1) + j - 1) = 5;

					add_flag = 1;
					i = i+1;
					j = j-1;
					if (Total_Point_W_L_45 > j) 
					{
						Total_Point_W_L_45 = j;
					}
					if (Total_Point_W_R_45 < j) 
					{
						Total_Point_W_R_45 = j;
					}

					if (Total_Point_H_U_45 > i) 
					{
						Total_Point_H_U_45 = i;
					}
					if (Total_Point_H_D_45 < i) 
					{
						Total_Point_H_D_45 = i;
					}
					continue;
				}

				if (*((Uint8 *)img_coordinate + w * i + j - 1) == 1)
				{
					*((Uint8 *)img_coordinate + w * i + j - 1) = 5;

					add_flag = 1;
					j = j-1;
					if (Total_Point_W_L_45 > j) 
					{
						Total_Point_W_L_45 = j;
					}
					if (Total_Point_W_R_45 < j) 
					{
						Total_Point_W_R_45 = j;
					}

					if (Total_Point_H_U_45 > i) 
					{
						Total_Point_H_U_45 = i;
					}
					if (Total_Point_H_D_45 < i) 
					{
						Total_Point_H_D_45 = i;
					}
					continue;
				}


				if (*((Uint8 *)img_coordinate + w * (i+1) + j) == 1)
				{
					*((Uint8 *)img_coordinate + w * (i+1) + j) = 5;

					add_flag = 1;
					i = i+1;
					if (Total_Point_W_L_45 > j) 
					{
						Total_Point_W_L_45 = j;
					}
					if (Total_Point_W_R_45 < j) 
					{
						Total_Point_W_R_45 = j;
					}

					if (Total_Point_H_U_45 > i) 
					{
						Total_Point_H_U_45 = i;
					}
					if (Total_Point_H_D_45 < i) 
					{
						Total_Point_H_D_45 = i;
					}
					continue;
				}

				if (*((Uint8 *)img_coordinate + w * (i+2) + j - 2) == 1)
				{
					*((Uint8 *)img_coordinate + w * (i+2) + j - 2) = 5;

					add_flag = 1;
					i = i+2;
					j = j-2;
					if (Total_Point_W_L_45 > j) 
					{
						Total_Point_W_L_45 = j;
					}
					if (Total_Point_W_R_45 < j)
					{
						Total_Point_W_R_45 = j;
					}

					if (Total_Point_H_U_45 > i) 
					{
						Total_Point_H_U_45 = i;
					}
					if (Total_Point_H_D_45 < i) 
					{
						Total_Point_H_D_45 = i;
					}
					continue;
				}

				if (*((Uint8 *)img_coordinate + w * (i+1) + j - 2) == 1)
				{
					*((Uint8 *)img_coordinate + w * (i+1) + j - 2) = 5;

					add_flag = 1;
					i = i+1;
					j = j-2;
					if (Total_Point_W_L_45 > j) 
					{
						Total_Point_W_L_45 = j;
					}
					if (Total_Point_W_R_45 < j) 
					{
						Total_Point_W_R_45 = j;
					}

					if (Total_Point_H_U_45 > i) 
					{
						Total_Point_H_U_45 = i;
					}
					if (Total_Point_H_D_45 < i) 
					{
						Total_Point_H_D_45 = i;
					}
					continue;
				}

				if (*((Uint8 *)img_coordinate + w * (i+2) + j-1) == 1)
				{
					*((Uint8 *)img_coordinate + w * (i+2) + j-1) = 5;

					add_flag = 1;
					i = i+2;
					j = j-1;
					if (Total_Point_W_L_45 > j) 
					{
						Total_Point_W_L_45 = j;
					}
					if (Total_Point_W_R_45 < j) 
					{
						Total_Point_W_R_45 = j;
					}

					if (Total_Point_H_U_45 > i) 
					{
						Total_Point_H_U_45 = i;
					}
					if (Total_Point_H_D_45 < i) 
					{
						Total_Point_H_D_45 = i;
					}
					continue;
				}
			}

		} 
		
		Total_Point_W_L_135 = 0;
		Total_Point_W_R_135 = 0;
		Total_Point_H_U_135 = 0;
		Total_Point_H_D_135 = 0; 

		D_180_Inc = 0;
		D_90_Inc = 0; 

		//ÅÐ¶Ï135¶Èµã
		if (*((Uint8 *)img_coordinate+k) == 2)
		{
			//Èç¹û135¶ÈµãµÄÓÒ±ßµã»¹ÊÇ135¶Èµã£¬ÔòÈÏÎª´Ëµã²»ÊÇÈËÁ³µÄ×ó±ßµÄ135¶Èµã£¬¼ÌÐøÏòÏÂÑ°ÕÒ
			if (*((Uint8 *)img_coordinate_g + w * n + m - 1) == 2)
			continue;

			//ÒªÅÐ¶Ï135¶ÈµãÓÒ±ßµÄ¹Ø¼üµãµÄ9¸öµãÊÇ·ñ¶¼ÊÇ180¶Èµã£¬ÊÇµÄ»°¾Í×ö¸öÍ³¼Æ£¬ÓÃÓÚ¸¨Öú
			//ÅÐ¶ÏÕâ¸ö135µãÊÇ·ñÊÇÕæÕýµÄ135¶Èµã
			if (*((Uint8 *)img_coordinate_g + w * n + m -1) == 3) 
				D_180_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * n + m -2) == 3) 
				D_180_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * n + m -3) == 3) 
				D_180_Inc++;
			if (*((Uint8 *)img_coordinate_g + w * (n-1) + m -1) == 3) 
				D_180_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n-1) + m -2) == 3) 
				D_180_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n-1) + m -3) == 3) 
				D_180_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n+1) + m -1) == 3) 
				D_180_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n+1) + m -2) == 3) 
				D_180_Inc++;
			if (*((Uint8 *)img_coordinate_g + w * (n+1) + m -3) == 3) 
				D_180_Inc++;    

			if (*((Uint8 *)img_coordinate_g + w * (n-1) + m) == 4) 
				D_90_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n-1) + m -1) == 4) 
				D_90_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n-1) + m +1) == 4) 
				D_90_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n-2) + m) == 4) 
				D_90_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n-2) + m -1) == 4) 
				D_90_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n-2) + m +1) == 4) 
				D_90_Inc++;
			if (*((Uint8 *)img_coordinate_g + w * (n-3) + m) == 4) 
				D_90_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n-3) + m -1) == 4) 
				D_90_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n-3) + m +1) == 4) 
				D_90_Inc++;

			Direction_135 = 0x00;
			if (D_180_Inc > 1 && D_180_Inc >  D_90_Inc)
			{
				Direction_135 = 0x10;
			}
			if (D_90_Inc > 1 && D_180_Inc <  D_90_Inc)
			{
				Direction_135 = 0x20;
			}

			i = n;
			j = m;
			Total_Point_W_L_135 = j;
			Total_Point_W_R_135 = j;
			Total_Point_H_U_135 = i;
			Total_Point_H_D_135 = i;

			while(add_flag == 1)
			{
				add_flag = 0;

				if (*((Uint8 *)img_coordinate + w * (i+1) + j + 1) == 2)
				{
					*((Uint8 *)img_coordinate + w * (i+1) + j + 1) = 6;

					add_flag = 1;
					i = i+1;
					j = j+1;
					if (Total_Point_W_L_135 > j) 
					{
						Total_Point_W_L_135 = j;
					}
					if (Total_Point_W_R_135 < j) 
					{
						Total_Point_W_R_135 = j;
					}

					if (Total_Point_H_U_135 > i) 
					{
						Total_Point_H_U_135 = i;
					}
					if (Total_Point_H_D_135 < i) 
					{
						Total_Point_H_D_135 = i;
					}
					continue;
				}

				if (*((Uint8 *)img_coordinate + w * i + j + 1) == 2)
				{
					*((Uint8 *)img_coordinate + w * i + j + 1) = 6;

					add_flag = 1;
					j = j+1;
					if (Total_Point_W_L_135 > j) 
					{
						Total_Point_W_L_135 = j;
					}
					if (Total_Point_W_R_135 < j) 
					{
						Total_Point_W_R_135 = j;
					}

					if (Total_Point_H_U_135 > i) 
					{
						Total_Point_H_U_135 = i;
					}
					if (Total_Point_H_D_135 < i) 
					{
						Total_Point_H_D_135 = i;
					}
					continue;
				}

				if (*((Uint8 *)img_coordinate + w * (i+1) + j) == 2)
				{
					*((Uint8 *)img_coordinate + w * (i+1) + j) = 6;

					add_flag = 1;
					i = i+1;
					if (Total_Point_W_L_135 > j) 
					{
						Total_Point_W_L_135 = j;
					}
					if (Total_Point_W_R_135 < j) 
					{
						Total_Point_W_R_135 = j;
					}

					if (Total_Point_H_U_135 > i) 
						Total_Point_H_U_135 = i;
					if (Total_Point_H_D_135 < i) 
						Total_Point_H_D_135 = i;
					continue;
				}

				if (*((Uint8 *)img_coordinate + w * (i+2) + j + 2) == 2)
				{
					*((Uint8 *)img_coordinate + w * (i+2) + j + 2) = 6;

					add_flag = 1;
					i = i+2;
					j = j+2;
					if (Total_Point_W_L_135 > j) 
					{
						Total_Point_W_L_135 = j;
					}
					if (Total_Point_W_R_135 < j) 
					{
						Total_Point_W_R_135 = j;
					}

					if (Total_Point_H_U_135 > i) 
					{
						Total_Point_H_U_135 = i;
					}
					if (Total_Point_H_D_135 < i) 
					{
						Total_Point_H_D_135 = i;
					}
					continue;
				}

				if (*((Uint8 *)img_coordinate + w * (i+1) + j + 2) == 2)
				{
					*((Uint8 *)img_coordinate + w * (i+1) + j + 2) = 6;

					add_flag = 1;
					i = i+1;
					j = j+2;
					if (Total_Point_W_L_135 > j) 
					{
						Total_Point_W_L_135 = j;
					}
					if (Total_Point_W_R_135 < j) 
					{
						Total_Point_W_R_135 = j;
					}

					if (Total_Point_H_U_135 > i) 
					{
						Total_Point_H_U_135 = i;
					}
					if (Total_Point_H_D_135 < i) 
					{
						Total_Point_H_D_135 = i;
					}
					continue;
				}

				if (*((Uint8 *)img_coordinate + w * (i+2) + j+1) == 2)
				{
					*((Uint8 *)img_coordinate + w * (i+2) + j+1) = 6;

					add_flag = 1;
					i = i+2;
					j = j+1;
					if (Total_Point_W_L_135 > j) 
					{
						Total_Point_W_L_135 = j;
					}
					if (Total_Point_W_R_135 < j) 
					{
						Total_Point_W_R_135 = j;
					}
					
					if (Total_Point_H_U_135 > i) 
					{
						Total_Point_H_U_135 = i;
					}
					if (Total_Point_H_D_135 < i) 
					{
						Total_Point_H_D_135 = i;
					}
					continue;
				}
			}

		} 
		Total_Point_W_L_180 = 0;
		Total_Point_W_R_180 = 0;
		Total_Point_H_U_180 = 0;
		Total_Point_H_D_180 = 0;

		//180¶Èµã
		if (*((Uint8 *)img_coordinate+k) == 3)
		{
			if (*((Uint8 *)img_coordinate + w * n + m - 1) == 3|| 
				*((Uint8 *)img_coordinate + w * (n+1) + m ) == 3||
				*((Uint8 *)img_coordinate + w * (n+1) + m - 1) == 3)
			{
				continue;
			}
			i = n;
			j = m;
			Total_Point_W_L_180 = j;
			Total_Point_W_R_180 = j;
			Total_Point_H_U_180 = i;
			Total_Point_H_D_180 = i;
			while(add_flag == 1)
			{
				add_flag = 0;

				if (*((Uint8 *)img_coordinate + w * (i-1) + j) == 3)
				{
					*((Uint8 *)img_coordinate + w * (i-1) + j) = 7;

					add_flag = 1;
					i = i-1;
					if (Total_Point_W_L_180 > j) 
						Total_Point_W_L_180 = j;
					if (Total_Point_W_R_180 < j) 
						Total_Point_W_R_180 = j;

					if (Total_Point_H_U_180 > i) 
						Total_Point_H_U_180 = i;
					if (Total_Point_H_D_180 < i) 
						Total_Point_H_D_180 = i;
					continue;
				}
				if (*((Uint8 *)img_coordinate + w * (i+1) + j) == 3)
				{
					*((Uint8 *)img_coordinate + w * (i+1) + j) = 7;

					add_flag = 1;
					i = i+1;
					if (Total_Point_W_L_180 > j) 
						Total_Point_W_L_180 = j;
					if (Total_Point_W_R_180 < j) 
						Total_Point_W_R_180 = j;

					if (Total_Point_H_U_180 > i) 
						Total_Point_H_U_180 = i;
					if (Total_Point_H_D_180 < i) 
						Total_Point_H_D_180 = i;
					continue;
				}

				if (*((Uint8 *)img_coordinate + w * i + j + 1) == 3)
				{
					*((Uint8 *)img_coordinate + w * i + j + 1) = 7;

					add_flag = 1;
					j = j+1;
					if (Total_Point_W_L_180 > j) 
						Total_Point_W_L_180 = j;
					if (Total_Point_W_R_180 < j) 
						Total_Point_W_R_180 = j;

					if (Total_Point_H_U_180 > i) 
						Total_Point_H_U_180 = i;
					if (Total_Point_H_D_180 < i) 
						Total_Point_H_D_180 = i;
					continue;
				} 
				if (*((Uint8 *)img_coordinate + w * (i-1) + j + 1) == 3)
				{
					*((Uint8 *)img_coordinate + w * (i-1) + j + 1) = 7;

					add_flag = 1;
					i = i-1;
					j = j+1;
					if (Total_Point_W_L_180 > j) 
						Total_Point_W_L_180 = j;
					if (Total_Point_W_R_180 < j) 
						Total_Point_W_R_180 = j;

					if (Total_Point_H_U_180 > i) 
						Total_Point_H_U_180 = i;
					if (Total_Point_H_D_180 < i) 
						Total_Point_H_D_180 = i;
					continue;
				}
				if (*((Uint8 *)img_coordinate + w * (i+1) + j + 1) == 3)
				{
					*((Uint8 *)img_coordinate + w * (i+1) + j + 1) = 7;

					add_flag = 1;
					i = i+1;
					j = j+1;
					if (Total_Point_W_L_180 > j) 
						Total_Point_W_L_180 = j;
					if (Total_Point_W_R_180 < j) 
						Total_Point_W_R_180 = j;

					if (Total_Point_H_U_180 > i) 
						Total_Point_H_U_180 = i;
					if (Total_Point_H_D_180 < i) 
						Total_Point_H_D_180 = i;
					continue;
				}


				if (*((Uint8 *)img_coordinate + w * i + j + 2) == 3)
				{
					*((Uint8 *)img_coordinate + w * i + j + 2) = 7;

					add_flag = 1;
					j = j+2;
					if (Total_Point_W_L_180 > j) 
						Total_Point_W_L_180 = j;
					if (Total_Point_W_R_180 < j) 
						Total_Point_W_R_180 = j;

					if (Total_Point_H_U_180 > i) 
						Total_Point_H_U_180 = i;
					if (Total_Point_H_D_180 < i) 
						Total_Point_H_D_180 = i;
					continue;
				}
				if (*((Uint8 *)img_coordinate + w * (i-1) + j + 2) == 3)
				{
					*((Uint8 *)img_coordinate + w * (i-1) + j + 2) = 7;

					add_flag = 1;
					i = i-1;
					j = j+2;
					if (Total_Point_W_L_180 > j) 
						Total_Point_W_L_180 = j;
					if (Total_Point_W_R_180 < j) 
						Total_Point_W_R_180 = j;

					if (Total_Point_H_U_180 > i) 
						Total_Point_H_U_180 = i;
					if (Total_Point_H_D_180 < i) 
						Total_Point_H_D_180 = i;
					continue;
				}
				if (*((Uint8 *)img_coordinate + w * (i+1) + j + 2) == 3)
				{
					*((Uint8 *)img_coordinate + w * (i+1) + j + 2) = 7;

					add_flag = 1;
					i = i+1;
					j = j+2;
					if (Total_Point_W_L_180 > j) 
						Total_Point_W_L_180 = j;
					if (Total_Point_W_R_180 < j) 
						Total_Point_W_R_180 = j;

					if (Total_Point_H_U_180 > i) 
						Total_Point_H_U_180 = i;
					if (Total_Point_H_D_180 < i) 
						Total_Point_H_D_180 = i;
					continue;
				}
			}
		}
		Total_Point_W_L_90 = 0;
		Total_Point_W_R_90 = 0;
		Total_Point_H_U_90 = 0;
		Total_Point_H_D_90 = 0;

		D_45_Inc = 0; 
		D_135_Inc = 0;

		//90¶Èµã
		if (*((Uint8 *)img_coordinate+k) == 4)
		{
			if (*((Uint8 *)img_coordinate + w * (n-1) + m ) == 4)
			{
				continue;
			}

			if (*((Uint8 *)img_coordinate_g + w * (n-1) + m) == 1) 
				D_45_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n-1) + m -1) == 1) 
				D_45_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n-1) + m +1) == 1) 
				D_45_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n-2) + m) == 1) 
				D_45_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n-2) + m -1) == 1) 
				D_45_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n-2) + m +1) == 1) 
				D_45_Inc++;
			if (*((Uint8 *)img_coordinate_g + w * (n-3) + m) == 1) 
				D_45_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n-3) + m -1) == 1) 
				D_45_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n-3) + m +1) == 1) 
				D_45_Inc++;

			if (*((Uint8 *)img_coordinate_g + w * (n-1) + m) == 2) 
				D_135_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n-1) + m -1) == 2) 
				D_135_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n-1) + m +1) == 2) 
				D_135_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n-2) + m) == 2) 
				D_135_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n-2) + m -1) == 2) 
				D_135_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n-2) + m +1) == 2) 
				D_135_Inc++;
			if (*((Uint8 *)img_coordinate_g + w * (n-3) + m) == 2) 
				D_135_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n-3) + m -1) == 2) 
				D_135_Inc++; 
			if (*((Uint8 *)img_coordinate_g + w * (n-3) + m +1) == 2) 
				D_135_Inc++;
			
			Direction_90 = 0x00;
			if (D_45_Inc > 1 && D_45_Inc > D_135_Inc)
			{
				Direction_90 = 0x10;
			}
			if (D_135_Inc > 1 && D_45_Inc <  D_135_Inc)
			{
				Direction_90 = 0x20;
			}

			i = n;
			j = m;
			Total_Point_W_L_90 = j;
			Total_Point_W_R_90 = j;
			Total_Point_H_U_90 = i;
			Total_Point_H_D_90 = i;
			
			while(add_flag == 1)
			{
				add_flag = 0;
				if (*((Uint8 *)img_coordinate + w * (i+1) + j) == 4)
				{
					*((Uint8 *)img_coordinate + w * (i+1) + j) = 8;

					add_flag = 1;
					i = i+1;
					if (Total_Point_W_L_90 > j) 
						Total_Point_W_L_90 = j;
					if (Total_Point_W_R_90 < j) 
						Total_Point_W_R_90 = j;

					if (Total_Point_H_U_90 > i) 
						Total_Point_H_U_90 = i;
					if (Total_Point_H_D_90 < i) 
						Total_Point_H_D_90 = i;
					continue;
				}
				
				if (*((Uint8 *)img_coordinate + w * (i+1) + j - 1) == 4)
				{
					*((Uint8 *)img_coordinate + w * (i+1) + j - 1) = 8;

					add_flag = 1;
					i = i+1;
					j = j-1;
					if (Total_Point_W_L_90 > j) 
						Total_Point_W_L_90 = j;
					if (Total_Point_W_R_90 < j) 
						Total_Point_W_R_90 = j;

					if (Total_Point_H_U_90 > i) 
						Total_Point_H_U_90 = i;
					if (Total_Point_H_D_90 < i) 
						Total_Point_H_D_90 = i;
					continue;
				}
				if (*((Uint8 *)img_coordinate + w * (i+1) + j + 1) == 4)
				{
					*((Uint8 *)img_coordinate + w * (i+1) + j + 1) = 8;

					add_flag = 1;
					i = i+1;
					j = j+1;
					if (Total_Point_W_L_90 > j) 
						Total_Point_W_L_90 = j;
					if (Total_Point_W_R_90 < j) 
						Total_Point_W_R_90 = j;

					if (Total_Point_H_U_90 > i) 
						Total_Point_H_U_90 = i;
					if (Total_Point_H_D_90 < i) 
						Total_Point_H_D_90 = i;
					continue;
				}

				if (*((Uint8 *)img_coordinate + w * (i+2) + j) == 4)
				{
					*((Uint8 *)img_coordinate + w * (i+2) + j) = 8;

					add_flag = 1;
					i = i+2;
					if (Total_Point_W_L_90 > j) 
						Total_Point_W_L_90 = j;
					if (Total_Point_W_R_90 < j) 
						Total_Point_W_R_90 = j;

					if (Total_Point_H_U_90 > i) 
						Total_Point_H_U_90 = i;
					if (Total_Point_H_D_90 < i) 
						Total_Point_H_D_90 = i;
					continue;
				}
				if (*((Uint8 *)img_coordinate + w * (i+2) + j - 1) == 4)
				{
					*((Uint8 *)img_coordinate + w * (i+2) + j - 1) = 8;

					add_flag = 1;
					i = i+2;
					j = j-1;
					if (Total_Point_W_L_90 > j) 
						Total_Point_W_L_90 = j;
					if (Total_Point_W_R_90 < j) 
						Total_Point_W_R_90 = j;

					if (Total_Point_H_U_90 > i) 
						Total_Point_H_U_90 = i;
					if (Total_Point_H_D_90 < i) 
						Total_Point_H_D_90 = i;
					continue;
				}
				if (*((Uint8 *)img_coordinate + w * (i+2) + j + 1) == 4)
				{
					*((Uint8 *)img_coordinate + w * (i+2) + j + 1) = 8;

					add_flag = 1;
					i = i+2;
					j = j+1;
					if (Total_Point_W_L_90 > j) 
						Total_Point_W_L_90 = j;
					if (Total_Point_W_R_90 < j) 
						Total_Point_W_R_90 = j;

					if (Total_Point_H_U_90 > i) 
						Total_Point_H_U_90 = i;
					if (Total_Point_H_D_90 < i) 
						Total_Point_H_D_90 = i;
					continue;
				}
			}
		} 

#if 1
		if (Total_Point_W_R_45 - Total_Point_W_L_45 > 0 && Total_Point_H_D_45 - Total_Point_H_U_45 > 1)// &&
		//    Total_Point_W_R_45 - Total_Point_W_L_45 < 20 && Total_Point_H_D_45 - Total_Point_H_U_45 < 20)
#else
		if ((Total_Point_W_R_45 - Total_Point_W_L_45 > 0) && (Total_Point_H_D_45 - Total_Point_H_U_45 > 1)
			&& (Total_Point_W_R_45 - Total_Point_W_L_45 < 20) && (Total_Point_H_D_45 - Total_Point_H_U_45 < 20))
#endif
		{
			//if(   (Total_Point_W_R_45-Total_Point_W_L_45   > model[zoom_distance][38]/5)
			//	&&(Total_Point_H_D_45 - Total_Point_H_U_45 > model[zoom_distance][39]/5)
			// )
			{
				intAPixels = Total_Point_W_L_45 * x_offset;
				intALines = Total_Point_H_U_45 * (y_offset/INTER_PROG_RATIO);
				intDPixels = Total_Point_W_R_45 * x_offset;
				intDLines = Total_Point_H_D_45 * (y_offset/INTER_PROG_RATIO);

				*((Uint16 *)T_L_Buffer + Point_45_Inc) = m - (Total_Point_W_R_45-Total_Point_W_L_45)/2;
				Point_45_Inc++;
				*((Uint16 *)T_L_Buffer + Point_45_Inc) = n + (Total_Point_H_D_45-Total_Point_H_U_45)/2;
				Point_45_Inc++;
				*((Uint16 *)T_L_Buffer + Point_45_Inc) = intAPixels;
				Point_45_Inc++;
				*((Uint16 *)T_L_Buffer + Point_45_Inc) = intALines;
				Point_45_Inc++;
				*((Uint16 *)T_L_Buffer + Point_45_Inc) = intDPixels;
				Point_45_Inc++;
				*((Uint16 *)T_L_Buffer + Point_45_Inc) = intDLines;
				Point_45_Inc++;

				*((Uint16 *)Point_Buffer + Point_Inc) = Direction_45|0x01;
				Point_Inc++; 
				*((Uint16 *)Point_Buffer + Point_Inc) = m - (Total_Point_W_R_45-Total_Point_W_L_45)/2;
				Point_Inc++;
				*((Uint16 *)Point_Buffer + Point_Inc) = n + (Total_Point_H_D_45-Total_Point_H_U_45)/2;
				Point_Inc++;
			}
		}
#if 1
		if (Total_Point_W_R_135 - Total_Point_W_L_135 > 0 && Total_Point_H_D_135 - Total_Point_H_U_135 > 1)// &&
			//  Total_Point_W_R_135 - Total_Point_W_L_135 < 20 && Total_Point_H_D_135 - Total_Point_H_U_135 < 20)
#else
		if ((Total_Point_W_R_135 - Total_Point_W_L_135 > 0) && (Total_Point_H_D_135 - Total_Point_H_U_135 > 1)
			&& (Total_Point_W_R_135 - Total_Point_W_L_135 < 20) && (Total_Point_H_D_135 - Total_Point_H_U_135 < 20))
#endif
		{
			//if(   (Total_Point_W_R_135 - Total_Point_W_L_135   > model[zoom_distance][38]/5)
			//	&&(Total_Point_H_D_135 - Total_Point_H_U_135   > model[zoom_distance][39]/5)
			//  )
			{
				intAPixels = Total_Point_W_L_135 * x_offset;
				intALines = Total_Point_H_U_135 * (y_offset/INTER_PROG_RATIO);
				intDPixels = Total_Point_W_R_135 * x_offset;
				intDLines = Total_Point_H_D_135 * (y_offset/INTER_PROG_RATIO);

				*((Uint16 *)T_R_Buffer + Point_135_Inc) = m + (Total_Point_W_R_135-Total_Point_W_L_135)/2;
				Point_135_Inc++;
				*((Uint16 *)T_R_Buffer + Point_135_Inc) = n + (Total_Point_H_D_135-Total_Point_H_U_135)/2;
				Point_135_Inc++;
				*((Uint16 *)T_R_Buffer + Point_135_Inc) = intAPixels;
				Point_135_Inc++;
				*((Uint16 *)T_R_Buffer + Point_135_Inc) = intALines;
				Point_135_Inc++;
				*((Uint16 *)T_R_Buffer + Point_135_Inc) = intDPixels;
				Point_135_Inc++;
				*((Uint16 *)T_R_Buffer + Point_135_Inc) = intDLines;
				Point_135_Inc++;

				*((Uint16 *)Point_Buffer + Point_Inc) = Direction_135|0x02;
				Point_Inc++;
				*((Uint16 *)Point_Buffer + Point_Inc) = m + (Total_Point_W_R_135-Total_Point_W_L_135)/2;
				Point_Inc++;
				*((Uint16 *)Point_Buffer + Point_Inc) = n + (Total_Point_H_D_135-Total_Point_H_U_135)/2;
				Point_Inc++;
			}
		}

#if 1
		if (Total_Point_W_R_180 - Total_Point_W_L_180 > 0 && Total_Point_H_D_180 - Total_Point_H_U_180 >= 0)// &&
		// Total_Point_W_R_180 - Total_Point_W_L_180 < 20 && Total_Point_H_D_180 - Total_Point_H_U_180 < 8)
#else
		if ((Total_Point_W_R_180 - Total_Point_W_L_180 > 0) && (Total_Point_H_D_180 - Total_Point_H_U_180 >= 0)
			&& (Total_Point_W_R_180 - Total_Point_W_L_180 < 20) && (Total_Point_H_D_180 - Total_Point_H_U_180 < 8))
#endif
		{

			intAPixels = Total_Point_W_L_180 * x_offset;
			intALines = Total_Point_H_U_180 * (y_offset/INTER_PROG_RATIO);
			intDPixels = Total_Point_W_R_180 * x_offset;
			intDLines = Total_Point_H_D_180 * (y_offset/INTER_PROG_RATIO);


			if (*((Uint8 *)Area_vertex + (n+model[zoom_distance][22]+model[zoom_distance][24])* (y_offset/INTER_PROG_RATIO)*video_width+ (m+3)* x_offset) != 0)
			{
				if((Total_Point_W_R_180-Total_Point_W_L_180 < model[zoom_distance][38]*2)
					//&& (Total_Point_W_R_180-Total_Point_W_L_180 > model[zoom_distance][38]/5)
					)
				{
					*((Uint16 *)T_U_Buffer + Point_180_Inc) = m + (Total_Point_W_R_180-Total_Point_W_L_180)/2;
					Point_180_Inc++;
					*((Uint16 *)T_U_Buffer + Point_180_Inc) = n + (Total_Point_H_D_180-Total_Point_H_U_180)/2;
					Point_180_Inc++;
					*((Uint16 *)T_U_Buffer + Point_180_Inc) = intAPixels;
					Point_180_Inc++;
					*((Uint16 *)T_U_Buffer + Point_180_Inc) = intALines;
					Point_180_Inc++;
					*((Uint16 *)T_U_Buffer + Point_180_Inc) = intDPixels;
					Point_180_Inc++;
					*((Uint16 *)T_U_Buffer + Point_180_Inc) = intDLines+2;
					Point_180_Inc++;

					*((Uint16 *)Point_Buffer + Point_Inc) = 3;
					Point_Inc++;
					*((Uint16 *)Point_Buffer + Point_Inc) = m + (Total_Point_W_R_180-Total_Point_W_L_180)/2;
					Point_Inc++;
					*((Uint16 *)Point_Buffer + Point_Inc) = n + (Total_Point_H_D_180-Total_Point_H_U_180)/2;
					Point_Inc++;
				}
			}
		}
#if 1
		if (Total_Point_W_R_90 - Total_Point_W_L_90 >= 0 && Total_Point_H_D_90 - Total_Point_H_U_90 > 1)// &&
		//  Total_Point_W_R_90 - Total_Point_W_L_90 < 8 && Total_Point_H_D_90 - Total_Point_H_U_90 < 20)
#else
		if ((Total_Point_W_R_90 - Total_Point_W_L_90 >= 0) && (Total_Point_H_D_90 - Total_Point_H_U_90 > 1)
			&& (Total_Point_W_R_90 - Total_Point_W_L_90 < 8) && (Total_Point_H_D_90 - Total_Point_H_U_90 < 20))
#endif
		{

			//if((Total_Point_H_D_90-Total_Point_H_U_90) < (model[zoom_distance][39] + model[zoom_distance][37])*3/4
		//	if((Total_Point_H_D_90-Total_Point_H_U_90) > (model[zoom_distance][39])/5 )
			{
				intAPixels = Total_Point_W_L_90 * x_offset;
				intALines = Total_Point_H_U_90 * (y_offset/INTER_PROG_RATIO);
				intDPixels = Total_Point_W_R_90 * x_offset;
				intDLines = Total_Point_H_D_90 * (y_offset/INTER_PROG_RATIO);


				*((Uint16 *)T_P_Buffer + Point_90_Inc) = m;
				Point_90_Inc++;
				*((Uint16 *)T_P_Buffer + Point_90_Inc) = n;
				Point_90_Inc++;
				*((Uint16 *)T_P_Buffer + Point_90_Inc) = intAPixels;
				Point_90_Inc++;
				*((Uint16 *)T_P_Buffer + Point_90_Inc) = intALines;
				Point_90_Inc++;
				*((Uint16 *)T_P_Buffer + Point_90_Inc) = intDPixels+2;
				Point_90_Inc++;
				*((Uint16 *)T_P_Buffer + Point_90_Inc) = intDLines;
				Point_90_Inc++;

				*((Uint16 *)Point_Buffer + Point_Inc) = Direction_90|0x04;
				Point_Inc++;
				*((Uint16 *)Point_Buffer + Point_Inc) = m;
				Point_Inc++;
				*((Uint16 *)Point_Buffer + Point_Inc) = n;
				Point_Inc++;
			}
		} 

	}	
	 
	 inst->t_global_obj.Point_180_Inc=Point_180_Inc;
	 inst->t_global_obj.Point_90_Inc=Point_90_Inc;
	 inst->t_global_obj.Point_45_Inc=Point_45_Inc;
	 inst->t_global_obj.Point_135_Inc=Point_135_Inc;
	 inst->t_global_obj.Point_Inc=Point_Inc;
}


static int32_t	merge_180_degree_lines(TrackHand *inst)
{
	int32_t		i 		= 0;
	int32_t		j 		= 0;
	int32_t Point_180_Inc=inst->t_global_obj.Point_180_Inc;
	uint8_t *T_U_Buffer=inst->t_global_obj.T_U_Buffer;

	//È¡ÓÒ¶¥µãµÄÖµ
	for(i = 0; i < Point_180_Inc; i++)
	{
		//È¡×ó¶¥µãµÄÖµ
		for(j = 0; j < Point_180_Inc; j++)
		{
			if(abs(*((Uint16 *)T_U_Buffer + i + 3) - *((Uint16 *)T_U_Buffer + j + 3)) < 9)
			{
				if(abs(*((Uint16 *)T_U_Buffer + i + 4) - *((Uint16 *)T_U_Buffer + j + 2)) < 25)
				{
					*((Uint16 *)T_U_Buffer + i) = *((Uint16 *)T_U_Buffer + i + 2) + (*((Uint16 *)T_U_Buffer + j + 4) - *((Uint16 *)T_U_Buffer + i + 2))/2;
					*((Uint16 *)T_U_Buffer + i + 1) = *((Uint16 *)T_U_Buffer + i + 1);
					*((Uint16 *)T_U_Buffer + i + 2) = *((Uint16 *)T_U_Buffer + i + 2);
					*((Uint16 *)T_U_Buffer + i + 3) = *((Uint16 *)T_U_Buffer + i + 3);
					*((Uint16 *)T_U_Buffer + i + 4) = *((Uint16 *)T_U_Buffer + j + 4);
					*((Uint16 *)T_U_Buffer + i + 5) = *((Uint16 *)T_U_Buffer + j + 5);
					break;
				}
			}

			j = j + 5;
		}
		i = i + 5;
	}
	return 0;
}
void CtrlCam(TrackHand *inst,ITRACK_OutArgs *output_param)
{
	ITRACK_Params *input_param=&(inst->input_param);
	int32_t	mid_x			= input_param->dynamic_param.mid_x;
	int32_t	left_distance	= 0;
	int32_t	right_distance	= 0;
	//Êä³ö¸øÓ¦ÓÃ²ã£¬ÉãÏñÍ·ÒÆ¶¯·½ÏòºÍÉãÏñÍ·ÒÆ¶¯¾àÀë
	if(inst->t_global_obj.m_offset < mid_x)
	{
		right_distance = mid_x - inst->t_global_obj.m_offset;
	}
	else
	{
		left_distance = inst->t_global_obj.m_offset - mid_x;
	}

	if(left_distance > CENTER_DISTANC_MAX)
	{

		output_param->move_distance 		= left_distance;
		output_param->move_direction  		= CAMERA_MOVE_LEFT;
		output_param->cmd_type				= CAM_CTRL_ROTATION;
		inst->t_global_obj.last_move_flag	= CAMERA_MOVE_LEFT;
	}
	else if(right_distance > CENTER_DISTANC_MAX)
	{
		output_param->move_distance 		= right_distance;
		output_param->move_direction  		= CAMERA_MOVE_RIGHT;
		output_param->cmd_type				= CAM_CTRL_ROTATION;
		inst->t_global_obj.last_move_flag	= CAMERA_MOVE_RIGHT;
	}
	else
	{
		if(CAMERA_MOVE_STOP != inst->t_global_obj.last_move_flag)
		{
			if((left_distance < CENTER_DISTANC_MIN) && (right_distance < CENTER_DISTANC_MIN))
			{
				output_param->move_distance 	= inst->t_global_obj.m_offset - mid_x;
				output_param->move_direction  	= CAMERA_MOVE_STOP;
				output_param->cmd_type			= CAM_CTRL_ROTATION;
				inst->t_global_obj.last_move_flag	= CAMERA_MOVE_STOP;
			}
			else
			{//²»Í£Ö¹,µ«ÊÇ¾àÀëÒ²ÉÏ±¨,ÈÃÓ¦ÓÃ¼õËÙ
				if(right_distance>0)
				{
					output_param->move_distance 		= right_distance;
					output_param->move_direction		= CAMERA_MOVE_RIGHT;
					output_param->cmd_type				= CAM_CTRL_ROTATION;
					inst->t_global_obj.last_move_flag	= CAMERA_MOVE_RIGHT;
				}
				else
				{
					output_param->move_distance 		= left_distance;
					output_param->move_direction		= CAMERA_MOVE_LEFT;
					output_param->cmd_type				= CAM_CTRL_ROTATION;
					inst->t_global_obj.last_move_flag	= CAMERA_MOVE_LEFT;
				}
			}
		}
	}

}
static int GetTrackType(TrackHand *inst)
{
	int i=0;
	int index;
	int nLastX=0;
	int nFindNearNum=0;
	ITRACK_Params *input_param=&(inst->input_param);
	int16_t	pic_width			= input_param->static_param.pic_width;
	int16_t	pic_height			= input_param->static_param.pic_height;
	STR_OBJECTS_DETECTED *pst_det_obj=&(inst->t_global_obj.pst_det_obj);
	STR_OBJECT_TRAKING *pst_trak_obj=&(inst->t_global_obj.pst_trak_obj);
	uint8_t	(*model)[MODEL_NUM_MAX] = inst->t_global_obj.model;
	int32_t x_offset		= input_param->dynamic_param.x_offset;
	int8_t zoom_distance	= input_param->dynamic_param.zoom_distance;
	int32_t nFaceWidth=model[zoom_distance][38]*4;
	index = (pst_trak_obj->uiPointNum - 1) & 0x7f;
	nLastX=inst->t_global_obj.m_offset;//pst_trak_obj.astPoint[index].sX;// = inst->t_global_obj.m_offset;
	if(pst_det_obj->ucNumber<2)
	{
		return 0;
	}
	else
	{
	//	return 1;
	}
	for(i=0;i<pst_det_obj->ucNumber;i++)
	{
		if(abs(pst_det_obj->astInfo[i].sX-nLastX)<nFaceWidth)
		{
			nFindNearNum++;
		}
	}
	if(nFindNearNum>1)
	{
		return 1;
	}
	return 0;
}
/**
* @	º¯ÊýÃû³Æ: image_arc_normalized()
* @ º¯Êý¹¦ÄÜ: ÓëÄ£°æÆ¥Åä£¬µÃµ½ÈËÍ·ÏñµÄ¸öÊý£¬»­³öÈËµÄÍ·Ïñ
* @ ÊäÈë²ÎÊý: output_buf -- ÓÃÓÚ»­ÏßµÄbuf
* @	ÊäÈë²ÎÊý: input_param -- Íâ²¿ÉèÖÃµÄÊäÈë²ÎÊý
* @	Êä³ö²ÎÊý: output_param -- Êä³ö²ÎÊý
* @ ·µ»ØÖµ:	  ÎÞ
*/

static void image_arc_normalized(TrackHand *inst,int8_t *output_buf, ITRACK_OutArgs *output_param) 
{
	ITRACK_Params *input_param=&(inst->input_param);
   	int32_t k,i,j,U,Q,w,m,n,m_0,n_0,m_1,n_1,Sum,temp_0,temp_1,model_inc,C_x;
   	int32_t Cb,Cr,C_inc,A_inc,A_inc_45_L,A_inc_135_L,A_inc_45_R,A_inc_135_R;

	int32_t	x_offset		= input_param->dynamic_param.x_offset;
	int32_t	y_offset		= input_param->dynamic_param.y_offset;
	unsigned char *pOutYUV[3];

	
	int16_t	message			= input_param->dynamic_param.message;

	int32_t cbcr_bit_flag 	= 0;
	int32_t temp			= 0;	

	int32_t	model_sum		= input_param->dynamic_param.model_sum;

	int32_t	mid_x			= input_param->dynamic_param.mid_x;
	int32_t	mid_y			= input_param->dynamic_param.mid_y;
	//int32_t	video_height	= input_param->static_param.video_height;
	
	int16_t	limit_height		= input_param->dynamic_param.limit_height;

	int16_t	pic_width			= input_param->static_param.pic_width;
	int16_t	pic_height			= input_param->static_param.pic_height;

	int8_t	zoom_distance		= input_param->dynamic_param.zoom_distance;
	

	int32_t	all_target_cnt	= 0;	//·¢ÏÖµÄÈËÁ³µÄ¸öÊý
	int32_t dst_target_cnt  = 0;
	int16_t all_target[MAX_HEAD_NUM];
	
	yuv_value_info_t		yuv_value 	= {0};
	line_info_t 			line_info 	= {0};

	int32_t	left_distance	= 0;
	int32_t	right_distance	= 0;

	unsigned char  ucCommand = 0;
	int32_t	index			= 0;
	int32_t	ret				= 0;
	int32_t nSrcBufWidth=inst->t_global_obj.nSrcBufWidth;
	int32_t nSrcBufHeight=inst->t_global_obj.nSrcBufHeight;

	uint8_t *Point_Buffer=inst->t_global_obj.Point_Buffer;
	int32_t Point_180_Inc=inst->t_global_obj.Point_180_Inc;
	int32_t Point_Inc=inst->t_global_obj.Point_Inc;
	uint8_t *T_U_Buffer=inst->t_global_obj.T_U_Buffer;

	int16_t 	*model_v_cnt=inst->t_global_obj.model_v_cnt;
	uint16_t	*model_v_mnt=inst->t_global_obj.model_v_mnt;
	uint16_t	*model_v_pnt=inst->t_global_obj.model_v_pnt;
	STR_OBJECTS_DETECTED *pst_det_obj=&(inst->t_global_obj.pst_det_obj);
	
	STR_OBJECT_TRAKING *pst_trak_obj=&(inst->t_global_obj.pst_trak_obj);
	uint8_t	(*model)[MODEL_NUM_MAX] = inst->t_global_obj.model;
	int32_t nFaceWidth=model[zoom_distance][38]*2*x_offset;
	int32_t	hei_number = 0;
	short nx,ny,nw,nh;
	pOutYUV[0]=(unsigned char *)output_buf;
	pOutYUV[1]=pOutYUV[0]+inst->t_global_obj.nSrcBufWidth*inst->t_global_obj.nSrcBufHeight;
	pOutYUV[2]=pOutYUV[1]+inst->t_global_obj.nSrcBufWidth*inst->t_global_obj.nSrcBufHeight/4;

	limit_height = limit_height/input_param->dynamic_param.y_offset;

	if(zoom_distance < 0)
	{
		zoom_distance = 0;
	}
	if(zoom_distance >= MODEL_DISTANCE_CHOSE_NUM)
	{
		zoom_distance = MODEL_DISTANCE_CHOSE_NUM - 1;
	}
	
	m 	= 0;
	n 	= 0;
	m_1 = 0;
	n_1 = 0;
	k 	= 0;
	m_0 = mid_x;
	n_0 = mid_y;

	memset(pst_det_obj, 0, sizeof(STR_OBJECTS_DETECTED));
	draw_msg_info(inst,output_buf);
	for(k= 0;k< MODEL_NUM_MAX;k++)                     //È·¶¨mÖµ
	{
		model_v_cnt[k] = 0;
		model_v_pnt[k] = 0;
		model_v_mnt[k] = 0;
	}
	
	k = 0;
	if (Point_180_Inc > 6*100-6)
	{
		Point_180_Inc = 6*100-6;
	}
	
	for(U= 0;U< Point_180_Inc;U++)
	{
		m = *((Uint16 *)T_U_Buffer + U);
		U++;
		n = *((Uint16 *)T_U_Buffer + U);
		U++;
		U++;
		U++;
		U++;
		A_inc = 0;
		A_inc_45_L = 0;
		A_inc_135_L = 0;
		A_inc_45_R = 0;
		A_inc_135_R = 0;
		for(model_inc= 0;model_inc< model_sum;model_inc++)
		{
			model_v_pnt[model_inc] = 0;
		} 

		for(model_inc= 0;model_inc< model_sum;model_inc++)
		{
			C_x =  model[zoom_distance][model_inc*5] & 0x0f;
			if (C_x == 0x0a)
			{
				temp = model[zoom_distance][model_inc*5+1] & 0x80;
				if (temp == 0x80)
				{
					m_0 = m - (model[zoom_distance][model_inc*5+1] & 0x7f);
				}
				else
				{
					m_0 = m + (model[zoom_distance][model_inc*5+1] & 0x7f);
				}

				temp = model[zoom_distance][model_inc*5+2] & 0x80;
				if (temp == 0x80) 
				{
					n_0 = n - (model[zoom_distance][model_inc*5+2] & 0x7f);
				}
				else
				{
					n_0 = n + (model[zoom_distance][model_inc*5+2] & 0x7f);
				}

				C_inc = 0; 
				cbcr_bit_flag = 0;
				for (i= (n_0 - model[zoom_distance][model_inc*5+4]) * y_offset/INTER_PROG_RATIO;i < (n_0 + model[zoom_distance][model_inc*5+4]) * y_offset/INTER_PROG_RATIO;i++)  
				{
					for (j= (m_0 - model[zoom_distance][model_inc*5+3]) * x_offset;j< (m_0 + model[zoom_distance][model_inc*5+3]) * x_offset;j++) 
					{

						cbcr_bit_flag ++;
						
						#if 0
						Cb = (*((Uint8 *)output_cb_buf + ((i/2)* video_width/2 + j/2)));
						Cr = (*((Uint8 *)output_cr_buf + ((i/2)* video_width/2 + j/2)));
						#else
						if(j%2)
						{
							Cb = *((Uint8 *)output_buf + (nSrcBufWidth* nSrcBufHeight) + (i/2)*nSrcBufWidth + j-1);
							Cr = *((Uint8 *)output_buf + (nSrcBufWidth* nSrcBufHeight) + (i/2)*nSrcBufWidth + j);
						}
						else
						{
							Cb = *((Uint8 *)output_buf + (nSrcBufWidth* nSrcBufHeight) + (i/2)*nSrcBufWidth + j);
							Cr = *((Uint8 *)output_buf + (nSrcBufWidth* nSrcBufHeight) + (i/2)*nSrcBufWidth + j + 1);
						}
						#endif

						if (Cb < 123 && Cb > 108 && Cr < 156 && Cr > 135)
						{
							C_inc++;
							
							if(DRAW_SKIN_COLOR == message)
							{
								line_info.start_x = j;
								line_info.start_y = i;
								line_info.end_x = line_info.start_x + 2;
								line_info.end_y = line_info.start_y + 2;

								//?-??D??¨°
								yuv_value.y_value	= 255;
								yuv_value.cb_value	= 128;
								yuv_value.cr_value	= 128;
								draw_rectangle(inst,output_buf, &yuv_value, &line_info);
							}
						}

						
						#if 0
						if (C_inc > 20)
						{
							temp = model[zoom_distance][model_inc*5] & 0xf0;
							temp = temp>>4;
							model_v_pnt[model_inc] = temp;
							goto next_0;
						}
						#else

						if(Cb < 156 && Cb > 100 && Cr < 156 && Cr > 100)
						{
							hei_number++;
							#if 0
							line_info.start_x = j;
							line_info.start_y = i;
							line_info.end_x = line_info.start_x + 2;
							line_info.end_y = line_info.start_y + 2;

							//?-??D??¨°
							yuv_value.y_value	= 255;
							yuv_value.cb_value	= 255;
							yuv_value.cr_value	= 255;
							draw_rectangle(inst,output_buf, &yuv_value, &line_info);
							#endif
							
						}

						if(2 == inst->t_global_obj.last_track_status)
						{
							

							if (C_inc > 20)
							{
								inst->t_global_obj.skin_colour_num = 0;
								temp = model[zoom_distance][model_inc*5] & 0xf0;
								temp = temp>>4;
								model_v_pnt[model_inc] = temp;
								goto next_0;
							}
							else
							{
								if(inst->t_global_obj.skin_colour_num < 400)
								{
									if(hei_number > 20)
									{
										temp = model[zoom_distance][model_inc*5] & 0xf0;
										temp = temp>>4;
										model_v_pnt[model_inc] = temp;
										goto next_0;
									}

								}
							}
								

						}
						else
						{
							if (C_inc > 20)
							{
								temp = model[zoom_distance][model_inc*5] & 0xf0;
								temp = temp>>4;
								model_v_pnt[model_inc] = temp;
								goto next_0;
							}
						}
#endif
						
						j++;
					}
					i++;
				}
			} 
		}

next_0:
		for(Q= 0;Q< Point_Inc;Q++)					   //
		{
			w = *((Uint16 *)Point_Buffer + Q);
			Q++;
			m_1 = *((Uint16 *)Point_Buffer + Q);
			Q++;
			n_1 = *((Uint16 *)Point_Buffer + Q);

			for(model_inc= 0;model_inc< model_sum;model_inc++)
			{
				C_x =  model[zoom_distance][model_inc*5] & 0x0f;
				if (w == 0x11 && C_x == 0x01)							   //45
				{
					temp = model[zoom_distance][model_inc*5+1] & 0x80;
					if (temp == 0x80)
					{
						m_0 = m - (model[zoom_distance][model_inc*5+1] & 0x7f);
					}
					else
					{
						m_0 = m + (model[zoom_distance][model_inc*5+1] & 0x7f);
					}

					temp = model[zoom_distance][model_inc*5+2] & 0x80;
					if (temp == 0x80) 
					{
						n_0 = n - (model[zoom_distance][model_inc*5+2] & 0x7f);
					}
					else
					{
						n_0 = n + (model[zoom_distance][model_inc*5+2] & 0x7f);
					}

					if (abs(m_0 - m_1) < model[zoom_distance][model_inc*5+3] && abs(n_0 - n_1) < model[zoom_distance][model_inc*5+4])
					{
						temp = model[zoom_distance][model_inc*5] & 0xf0;
						temp = temp>>4;
						model_v_pnt[model_inc] = temp;
					}
				}

				if (w == 0x21 && C_x == 0x02)							   //45
				{
					temp = model[zoom_distance][model_inc*5+1] & 0x80;
					if (temp == 0x80)
					{
						m_0 = m - (model[zoom_distance][model_inc*5+1] & 0x7f);
					}
					else
					{
						m_0 = m + (model[zoom_distance][model_inc*5+1] & 0x7f);
					}

					temp = model[zoom_distance][model_inc*5+2] & 0x80;
					if (temp == 0x80) 
					{
						n_0 = n - (model[zoom_distance][model_inc*5+2] & 0x7f);
					}
					else
					{
						n_0 = n + (model[zoom_distance][model_inc*5+2] & 0x7f);
					}

					if (abs(m_0 - m_1) < model[zoom_distance][model_inc*5+3] && abs(n_0 - n_1) < model[zoom_distance][model_inc*5+4])
					{
						temp = model[zoom_distance][model_inc*5] & 0xf0;
						temp = temp>>4;
						model_v_pnt[model_inc] = temp;
					}
				}

				if ((w == 0x11||w == 0x21) && C_x == 0x03)								//45
				{
					temp = model[zoom_distance][model_inc*5+1] & 0x80;
					if (temp == 0x80)
					{
						m_0 = m - (model[zoom_distance][model_inc*5+1] & 0x7f);
					}
					else
					{
						m_0 = m + (model[zoom_distance][model_inc*5+1] & 0x7f);
					}

					temp = model[zoom_distance][model_inc*5+2] & 0x80;
					if (temp == 0x80) 
					{
						n_0 = n - (model[zoom_distance][model_inc*5+2] & 0x7f);
					}
					else
					{
						n_0 = n + (model[zoom_distance][model_inc*5+2] & 0x7f);
					}

					if (abs(m_0 - m_1) < model[zoom_distance][model_inc*5+3] && abs(n_0 - n_1) < model[zoom_distance][model_inc*5+4])
					{
						temp = model[zoom_distance][model_inc*5] & 0xf0;
						temp = temp>>4;
						model_v_pnt[model_inc] = temp;
					}
				}														 
				if (w == 0x12 && C_x == 0x04)					   //135
				{
					temp = model[zoom_distance][model_inc*5+1] & 0x80;
					if (temp == 0x80)
					{
						m_0 = m - (model[zoom_distance][model_inc*5+1] & 0x7f);

					}
					else
					{
						m_0 = m + (model[zoom_distance][model_inc*5+1] & 0x7f);
					}

					temp = model[zoom_distance][model_inc*5+2] & 0x80;
					if (temp == 0x80) 
					{
						n_0 = n - (model[zoom_distance][model_inc*5+2] & 0x7f);
					}
					else
					{
						n_0 = n + (model[zoom_distance][model_inc*5+2] & 0x7f);
					}

					if (abs(m_0 - m_1) < model[zoom_distance][model_inc*5+3] && abs(n_0 - n_1) < model[zoom_distance][model_inc*5+4])
					{
						temp = model[zoom_distance][model_inc*5] & 0xf0;
						temp = temp>>4;
						model_v_pnt[model_inc] = temp;
					}
				}

				if (w == 0x22 && C_x == 0x05)					   //135
				{
					temp = model[zoom_distance][model_inc*5+1] & 0x80;
					if (temp == 0x80)
					{
						m_0 = m - (model[zoom_distance][model_inc*5+1] & 0x7f);
					}
					else
					{
						m_0 = m + (model[zoom_distance][model_inc*5+1] & 0x7f);
					}

					temp = model[zoom_distance][model_inc*5+2] & 0x80;
					if (temp == 0x80) 
					{
						n_0 = n - (model[zoom_distance][model_inc*5+2] & 0x7f);
					}
					else
					{
						n_0 = n + (model[zoom_distance][model_inc*5+2] & 0x7f);
					}

					if (abs(m_0 - m_1) < model[zoom_distance][model_inc*5+3] && abs(n_0 - n_1) < model[zoom_distance][model_inc*5+4])
					{
						temp = model[zoom_distance][model_inc*5] & 0xf0;
						temp = temp>>4;
						model_v_pnt[model_inc] = temp;
					}
				}
				
				if ((w == 0x12||w == 0x22) && C_x == 0x06)						//135
				{
					temp = model[zoom_distance][model_inc*5+1] & 0x80;
					if (temp == 0x80)
					{
						m_0 = m - (model[zoom_distance][model_inc*5+1] & 0x7f);
					}
					else
					{
						m_0 = m + (model[zoom_distance][model_inc*5+1] & 0x7f);
					}

					temp = model[zoom_distance][model_inc*5+2] & 0x80;
					if (temp == 0x80) 
					{
						n_0 = n - (model[zoom_distance][model_inc*5+2] & 0x7f);
					}
					else
					{
						n_0 = n + (model[zoom_distance][model_inc*5+2] & 0x7f);
					}

					if (abs(m_0 - m_1) < model[zoom_distance][model_inc*5+3] && abs(n_0 - n_1) < model[zoom_distance][model_inc*5+4])
					{
						temp = model[zoom_distance][model_inc*5] & 0xf0;
						temp = temp>>4;
						model_v_pnt[model_inc] = temp;
					}
				}															  


				if (w == 0x14 && C_x == 0x07)					  //90
				{
					temp = model[zoom_distance][model_inc*5+1] & 0x80;
					if (temp == 0x80)
					{
						m_0 = m - (model[zoom_distance][model_inc*5+1] & 0x7f);
					}
					else
					{
						m_0 = m + (model[zoom_distance][model_inc*5+1] & 0x7f);
					}

					temp = model[zoom_distance][model_inc*5+2] & 0x80;
					if (temp == 0x80) 
					{
						n_0 = n - (model[zoom_distance][model_inc*5+2] & 0x7f);
					}
					else
					{
						n_0 = n + (model[zoom_distance][model_inc*5+2] & 0x7f);
					}

					if (abs(m_0 - m_1) < model[zoom_distance][model_inc*5+3] && abs(n_0 - n_1) < model[zoom_distance][model_inc*5+4])
					{
						temp = model[zoom_distance][model_inc*5] & 0xf0;
						temp = temp>>4;
						model_v_pnt[model_inc] = temp;
					}
				}
				if (w == 0x24 && C_x == 0x08)					  //90
				{
					temp = model[zoom_distance][model_inc*5+1] & 0x80;
					if (temp == 0x80)
					{
						m_0 = m - (model[zoom_distance][model_inc*5+1] & 0x7f);
					}
					else
					{
						m_0 = m + (model[zoom_distance][model_inc*5+1] & 0x7f);
					}

					temp = model[zoom_distance][model_inc*5+2] & 0x80;
					if (temp == 0x80) 
					{
						n_0 = n - (model[zoom_distance][model_inc*5+2] & 0x7f);
					}
					else
					{
						n_0 = n + (model[zoom_distance][model_inc*5+2] & 0x7f);
					}

					if (abs(m_0 - m_1) < model[zoom_distance][model_inc*5+3] && abs(n_0 - n_1) < model[zoom_distance][model_inc*5+4])
					{
						temp = model[zoom_distance][model_inc*5] & 0xf0;
						temp = temp>>4;
						model_v_pnt[model_inc] = temp;
					}
				}
				if ((w == 0x14||w == 0x24) && C_x == 0x09)					   //90
				{
					temp = model[zoom_distance][model_inc*5+1] & 0x80;
					if (temp == 0x80)
					{
						m_0 = m - (model[zoom_distance][model_inc*5+1] & 0x7f);
					}
					else
					{
						m_0 = m + (model[zoom_distance][model_inc*5+1] & 0x7f);
					}

					temp = model[zoom_distance][model_inc*5+2] & 0x80;
					if (temp == 0x80) 
					{
						n_0 = n - (model[zoom_distance][model_inc*5+2] & 0x7f);
					}
					else
					{
						n_0 = n + (model[zoom_distance][model_inc*5+2] & 0x7f);
					}

					if (abs(m_0 - m_1) < model[zoom_distance][model_inc*5+3] && abs(n_0 - n_1) < model[zoom_distance][model_inc*5+4])
					{
						temp = model[zoom_distance][model_inc*5] & 0xf0;
						temp = temp>>4;
						model_v_pnt[model_inc] = temp;
					}
				}

				if (C_x == 0x0f)									//all
				{
					temp = model[zoom_distance][model_inc*5+1] & 0x80;
					if (temp == 0x80)
					{
						m_0 = m - (model[zoom_distance][model_inc*5+1] & 0x7f);
					}
					else
					{
						m_0 = m + (model[zoom_distance][model_inc*5+1] & 0x7f);
					}

					temp = model[zoom_distance][model_inc*5+2] & 0x80;
					if (temp == 0x80) 
					{
						n_0 = n - (model[zoom_distance][model_inc*5+2] & 0x7f);
					}
					else
					{
						n_0 = n + (model[zoom_distance][model_inc*5+2] & 0x7f);
					}

					if (abs(m_0 - m_1) < model[zoom_distance][model_inc*5+3] && abs(n_0 - n_1) < model[zoom_distance][model_inc*5+4])
					{
						if (m_1 < m_0 && n_1 < n_0)
						{
							if (w == 0x11||w == 0x21)
							{
								A_inc_45_L++; 
							}
							if (w == 0x12||w == 0x22)
							{
								A_inc_135_L++;
							}
						}
						if (m_1 > m_0 && n_1 < n_0)
						{
							if (w == 0x11||w == 0x21)
							{
								A_inc_45_R++; 
							}
							if (w == 0x12||w == 0x22)
							{
								A_inc_135_R++;
							}
						}
						A_inc++;
						if (n_1 > n_0)
						{
							if (A_inc > input_param->dynamic_param.model_multiple)
							{
								if (A_inc_45_L < A_inc_135_L)
								{
									continue;
								}
								if (A_inc_45_R > A_inc_135_R)
								{
									continue;
								}
								if (A_inc_45_R > A_inc_45_L)
								{
									continue;
								}
								if (A_inc_135_L > A_inc_135_R)
								{
									continue; 
								}
								temp = model[zoom_distance][model_inc*5] & 0xf0;
								temp = temp>>4;
								model_v_pnt[model_inc] = temp;
							}
						}
					}
				}															  //
			}
			
		}

		for (model_inc= 0;model_inc< model_sum;model_inc++)
		{
			model_v_cnt[k] = model_v_cnt[k] + model_v_pnt[model_inc]; 
			if (model_v_pnt[model_inc] > 0)
			{
				temp = 0x8000;
				temp = temp>>model_inc;
				model_v_mnt[k] = model_v_mnt[k]|temp;
			}
		} 
		k++;
	}
 
	Sum = k;
	all_target_cnt = 0;
	temp_0 = 0;
	temp_1 = 800;

	//È·¶¨¸ú×ÙÄ¿±ê
	for(k= 0;k< Sum;k++)
	{
		if (model_v_cnt[k] > input_param->dynamic_param.model_level)
		{
			j = 0;
			for (i = 0;i < 4;i++)
			{
				temp = 0x8000;
				temp = temp>>i;
				temp = model_v_mnt[k] & temp;
				if (temp != 0)
				{
					j++;
				}
			}
			if (j < 3)
			{
				continue; 
			}
			m = *((Uint16 *)T_U_Buffer + 6*k);
			n = *((Uint16 *)T_U_Buffer + 6*k+1);


			all_target[all_target_cnt] = m;
			all_target_cnt++; 
			all_target[all_target_cnt] = n;
			all_target_cnt++; 
			if (model_v_cnt[k] > temp_0)
			{
				temp_0 = model_v_cnt[k];
			}
			if (n < temp_1)
			{
				temp_1 = n;
			}
		}  
	}

 	if (all_target_cnt > 2)
	{
		for(i= 0;i< all_target_cnt/2;i++) 
		{
			m = all_target[2*i];
			n = all_target[2*i+1];
			if (m <= 0 && n <= 0)
			{
				continue;
			}
			for(j= 0;j< all_target_cnt/2;j++) 
			{
				m_0 = all_target[2*j];
				n_0 = all_target[2*j+1];
				if (m_0 <= 0 && n_0 <= 0)
				{
					continue;
				}
				if (j!=i)
				{
					if (n_0 >= n)
					{
						//if (abs(m - m_0) < (int32_t)model[zoom_distance][38] && (n_0 - n) < (int32_t)model[zoom_distance][39])
						if (abs(m - m_0) < (int32_t)model[zoom_distance][38])
						{
							all_target[2*j] = -1;
							all_target[2*j+1] = -1;
						}
					}
				}
			}
		}
	}	   
	ucCommand = 1;
	//memset(pst_det_obj, 0, sizeof(STR_OBJECTS_DETECTED));
	pst_det_obj->stTrigger.sX = input_param->dynamic_param.TrackOtherParam.dst_trigger_x/x_offset-4;
	pst_det_obj->stTrigger.sY = input_param->dynamic_param.TrackOtherParam.dst_trigger_y/y_offset-4;
	pst_det_obj->stTrigger.usWidth  = 8;
	pst_det_obj->stTrigger.usHeight = 8;

	dst_target_cnt=0;
	for(index = 0; index < all_target_cnt/2; index++)
	{
		m = all_target[2*index];
		n = all_target[2*index+1];
		if((m<=0)||(n<=0))
		{
			continue;
		}
		pst_det_obj->astInfo[dst_target_cnt].sX = all_target[2*index];
		pst_det_obj->astInfo[dst_target_cnt].sY = all_target[2*index + 1];
		dst_target_cnt++;
	}
	pst_det_obj->ucNumber = dst_target_cnt;

////////
	//¸ú×ÙÄ£Ê½
	if(inst->t_global_obj.nTrackType==2)
	{
		n=0;
		for(index = 0; index < dst_target_cnt; index++)
		{
			m = pst_det_obj->astInfo[index].sX;
			if(abs(m*x_offset-inst->t_global_obj.tAutoTrackObj.X)<nFaceWidth*2)
			{
				n=1;
			}

		}
		
		if(n==0)
		{
			inst->t_global_obj.nMeanShiftFaceLost++;
			if(inst->t_global_obj.nMeanShiftFaceLost>=5)
			{
				inst->t_global_obj.nMeanShiftFaceLost=0;
				inst->t_global_obj.m_offset = mid_x;
				inst->t_global_obj.n_offset = mid_y;
				inst->t_global_obj.turn_flag = 2;
				output_param->cmd_type = CAM_CTRL_POSITION;
				output_param->cam_position	= TRIGGER_POSITION1;
				inst->t_global_obj.nTrackType=0;
			}
			else
			{
				inst->t_global_obj.nTrackType=1;

			}
			
			return;
		}
		else
		{
			inst->t_global_obj.nMeanShiftFaceLost=0;
			if(dst_target_cnt==1)
			{
				m = pst_det_obj->astInfo[0].sX;
				if(abs(m*x_offset-inst->t_global_obj.tAutoTrackObj.X)<nFaceWidth)
				{
					inst->t_global_obj.nTrackType=0;
				}
				else
				{
					inst->t_global_obj.nTrackType=1;
				}
			}
			else
			{
				inst->t_global_obj.nTrackType=1;
				return;
				
			}
		}
	}

///////
	//¸ú×Ù´¦Àí
	//Èç¹ûÉãÏñÍ·ÔÚ¶¯£¬ÄÇ¼ì²âµ½60Ö¡¶ª¾ÍÈÏÎª¶ª
	if((inst->t_global_obj.last_move_flag==CAMERA_MOVE_LEFT)
		||(inst->t_global_obj.last_move_flag==CAMERA_MOVE_RIGHT))
	{
		pst_det_obj->nCamMoveFlag=1;
	}
	else
	{
		pst_det_obj->nCamMoveFlag=0;
	}
	ret = TrakingObjectProcess(pic_width, pic_height, pst_det_obj, pst_trak_obj,ucCommand);
	
	inst->t_global_obj.last_track_status = ret;
	inst->t_global_obj.skin_colour_num++;
	if(0 == ret)		//´¦ÀíÕý³£,Î´·¢ÏÖ¸ú×ÙÄ¿±ê
	{
		inst->t_global_obj.m_offset = mid_x;
		inst->t_global_obj.n_offset = mid_y;
		inst->t_global_obj.turn_flag = 2;
		output_param->cmd_type = CAM_CTRL_POSITION;
		output_param->cam_position	= TRIGGER_POSITION1;
		inst->t_global_obj.nTargetFlag=0;
		return;
	}
	else if(2 == ret)	//´¦ÀíÕý³£,·¢ÏÖ¸ú×ÙÄ¿±ê
	{
		inst->t_global_obj.nTargetFlag=1;
		index = (pst_trak_obj->uiPointNum - 1) & 0x7f;
		inst->t_global_obj.turn_flag = 0;
		inst->t_global_obj.m_offset = pst_trak_obj->astPoint[index].sX;
		inst->t_global_obj.n_offset = pst_trak_obj->astPoint[index].sY;

		inst->t_global_obj.nLastTrackState=inst->t_global_obj.track_start;
//ÎªÁË±£Ö¤ÀÏÊ¦×ßµ½±ßÔµ£¬ÉãÏñÍ·»á¼ÌÐø×ª¶¯£¬±£Ö¤ÄÜ¹»¼ÌÐøÕÒµ½Ä¿±ê
		#if 0
		if(pst_trak_obj->uiLostFrame > 3)
		{
			if(CAMERA_MOVE_STOP != inst->t_global_obj.last_move_flag)
			{
				output_param->move_distance 	= inst->t_global_obj.m_offset - mid_x;
				output_param->move_direction  	= CAMERA_MOVE_STOP;
				output_param->cmd_type			= CAM_CTRL_ROTATION;
				inst->t_global_obj.last_move_flag	= CAMERA_MOVE_STOP;
			}
			return;
		}
		#endif
		
	}
	else				//´«Èë²ÎÊýÓÐÎó
	{
		inst->t_global_obj.nTargetFlag=0;
		return;
	}
	CtrlCam(inst,output_param);
	
	//if((pst_det_obj->ucNumber>1)&&(inst->t_global_obj.track_start==1))
	if(pst_det_obj->ucNumber>1)
	{
		inst->t_global_obj.nTrackType=GetTrackType(inst);
		if(input_param->dynamic_param.TrackOtherParam.meanshift_flag==0)
		{
			inst->t_global_obj.nTrackType=0;
		}
		if(inst->t_global_obj.nTrackType==1)
		{
			inst->t_global_obj.tAutoTrackObj.X=inst->t_global_obj.m_offset*x_offset;
			inst->t_global_obj.tAutoTrackObj.nLastX=inst->t_global_obj.m_offset*x_offset;
			//index = (pst_trak_obj->uiPointNum - 1) & 0x7f;
			//inst->t_global_obj.m_offset = pst_trak_obj->astPoint[index].sX;
			//inst->t_global_obj.n_offset = pst_trak_obj->astPoint[index].sY;
			return;
		}
		
	}

	return;
}

#if 0
static Int32 Draw(TrackHand *inst,char *addr)
{
	
	char *p = addr;
	char *y	= inst->t_global_obj.src_y_buf;
	char *u	= inst->t_global_obj.src_cb_buf;
	char *v	= inst->t_global_obj.src_cr_buf;
	int height= inst->input_param.static_param.pic_height;
	int width= inst->input_param.static_param.pic_width;

	memset(inst->t_global_obj.src_y_buf,0x0,width*height);
	memset(inst->t_global_obj.src_cb_buf,0x80,width*height/4);
	memset(inst->t_global_obj.src_cr_buf,0x80,width*height/4);

	
	int i = 0;
	for(i = 0; i<height; i++)
	{
		memcpy(p,y,width);
		p = p + inst->t_global_obj.nSrcBufWidth;
		y = y + width;
	}

	p = p + inst->t_global_obj.nSrcBufWidth*inst->t_global_obj.nSrcBufHeight;
	int j = 0;
	for(i = 0; i<inst->t_global_obj.nSrcBufHeight/2; i++)
	{
		for(j = 0; j <inst->t_global_obj.nSrcBufWidth/2;j++)
		{
			if((i< height) && (j < width))
			{
				*p = *u;
				p++;
				*p = *v;
				 u++;
				 v++;
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

#endif

int32_t	process(TrackHand *inst,int8_t *output_buf, 
	ITRACK_OutArgs *output_param)
{
	int32_t					img_flag	= 0;
	//struct 	timeval        		tv;
	ITRACK_Params *input_param;
	uint8_t *src_y_buf; //´æ·ÅÒª½øÐÐÔËËãµÄÔ´ybufÊý¾Ý
	uint8_t *src_cr_buf; //´æ·ÅÒª½øÐÐÔËËãµÄÔ´crbufÊý¾Ý
	unsigned char *pOutYUV[2];
	int32_t	x_offset;
	int32_t	y_offset;
	
	yuv_value_info_t		yuv_value 	= {0};
	line_info_t 			line_info 	= {0};
	short nx,ny,nw,nh;
	int16_t trigger_x0;
	int16_t trigger_y0;
	int16_t trigger_x1;
	int16_t trigger_y1;
	int32_t	index			= 0;
	int8_t	zoom_distance;
	int32_t nFaceWidth;
	int32_t x0,x1,y0,y1;
	
	uint8_t	(*model)[MODEL_NUM_MAX] = inst->t_global_obj.model;
	int32_t	nFaceTrackFlag			= 0;
	int32_t nRet=0;
	if((NULL == inst)||(NULL == output_buf)||(NULL == output_param))
	{
		return -1;
	}

	inst->t_global_obj.debugValue0 = 1;
	
	int bufferlen = inst->t_global_obj.nSrcBufWidth*inst->t_global_obj.nSrcBufHeight*2;

	memset(output_param, 0, sizeof(ITRACK_OutArgs));
	
	//	Draw(inst, output_buf);

	pOutYUV[0]=(unsigned char *)output_buf;
	pOutYUV[1]=pOutYUV[0]+inst->t_global_obj.nSrcBufWidth*inst->t_global_obj.nSrcBufHeight;
	input_param=&(inst->input_param);
	src_y_buf=inst->t_global_obj.src_y_buf; //´æ·ÅÒª½øÐÐÔËËãµÄÔ´ybufÊý¾Ý
	src_cr_buf=inst->t_global_obj.src_cr_buf; //´æ·ÅÒª½øÐÐÔËËãµÄÔ´crbufÊý¾Ý
	x_offset		= input_param->dynamic_param.x_offset;
	y_offset		= input_param->dynamic_param.y_offset;
	trigger_x0 		= input_param->dynamic_param.TrackOtherParam.meanshift_trigger.trigger_x0;
	trigger_y0 		= input_param->dynamic_param.TrackOtherParam.meanshift_trigger.trigger_y0;
	trigger_x1 		= input_param->dynamic_param.TrackOtherParam.meanshift_trigger.trigger_x1;
	trigger_y1 		= input_param->dynamic_param.TrackOtherParam.meanshift_trigger.trigger_y1;
	zoom_distance		= input_param->dynamic_param.zoom_distance;
	nFaceWidth=model[zoom_distance][38]*2*x_offset;
	if(inst->t_global_obj.track_start==0)
	{
		inst->t_global_obj.nTrackType=0;
		inst->t_global_obj.nTargetFlag=0;
	}
	if(inst->t_global_obj.track_start==1)
	{
		inst->t_global_obj.nPos1MvFlag=0;
	}
	if(input_param->dynamic_param.TrackOtherParam.meanshift_flag==0)
	{
		inst->t_global_obj.nTrackType=0;
	}
	//if((0 ==input_param->dynamic_param.process_type)||(inst->t_global_obj.track_start==0)||(nFaceTrackFlag!=0))
	if(inst->t_global_obj.nTrackType!=1)
	{
		inst->t_global_obj.debugValue0 = 2;
		inst->t_global_obj.nMeanShiftFrame=0;
		
		data_copy(inst,output_buf);
		image_y_value_compare(inst,output_buf,output_param);
		
		if(inst->t_global_obj.track_start)
		{
		    img_flag = 0;
			inst->t_global_obj.debugValue0 = 3;
			check_image_key_point(inst,(const unsigned char *)src_y_buf, img_flag);	 

			inst->t_global_obj.debugValue0 = 4;
			img_flag = 1;
		    check_image_key_point(inst,(const unsigned char *)src_cr_buf, img_flag);

			inst->t_global_obj.debugValue0 = 5;
		    image_arc_position(inst); 

			inst->t_global_obj.debugValue0 = 6;
		    image_arc_normalized(inst,output_buf,output_param);
		}
		
		if(1 == inst->t_global_obj.turn_flag)
		{
			if(inst->t_global_obj.turn_time > input_param->dynamic_param.start_track_time)
			{
				inst->t_global_obj.track_start 	= 1;
				inst->t_global_obj.turn_flag	= 0;
				inst->t_global_obj.turn_time	= 0;
				inst->t_global_obj.last_track_status = 0;
				inst->t_global_obj.skin_colour_num = 0;
			}
			inst->t_global_obj.turn_time++;
		}
		
		if(2 == inst->t_global_obj.turn_flag)
		{
			inst->t_global_obj.track_start 	= 0;
			inst->t_global_obj.pst_trak_obj.uiPointNum=0;
			if(inst->t_global_obj.turn_time > input_param->dynamic_param.reset_time)
			{
				inst->t_global_obj.turn_flag	= 0;
				inst->t_global_obj.turn_time	= 0;
			}
			inst->t_global_obj.turn_time++;
		}
		
	}
	
	if((inst->t_global_obj.track_start == 1)&&(inst->t_global_obj.last_track_status == 0))
	{
		inst->t_global_obj.nTrackStartFlag=1;

	}
	inst->t_global_obj.debugValue0 = 7;
	if((inst->t_global_obj.nTargetFlag == 1)&&(inst->t_global_obj.nTrackStartFlag == 1))
	{
		inst->t_global_obj.nTrackStartFlag=0;
		inst->t_global_obj.tAutoTrackObj.Status=0;
		nx=inst->t_global_obj.m_offset*x_offset;
		ny=(trigger_y0+trigger_y1)/2;
		nw=(trigger_x1-trigger_x0)/6;
		nh=trigger_y1-trigger_y0;
		ObjectTrackerInit(&(inst->t_global_obj.tAutoTrackObj),pOutYUV,nx,ny,nw,nh,inst->t_global_obj.nSrcBufWidth,inst->t_global_obj.nSrcBufHeight,trigger_x0,trigger_x1,trigger_y0,trigger_y1);
		inst->t_global_obj.last_track_status=1;
		line_info.start_x  = inst->t_global_obj.tAutoTrackObj.X-16;
		line_info.start_y= inst->t_global_obj.tAutoTrackObj.Y;
		line_info.end_x = line_info.start_x+32;
		line_info.end_y = line_info.start_y+32;
		yuv_value.y_value = 255;
		yuv_value.cb_value	= 255;
		yuv_value.cr_value	= 255;
		draw_rectangle(inst,output_buf, &yuv_value, &line_info);
	}
	
	if(inst->t_global_obj.nTrackType!=0)
	{
		inst->t_global_obj.debugValue0 = 8;
		inst->t_global_obj.nMeanShiftFrame++;
		if(inst->t_global_obj.nMeanShiftFrame==10)
		{
			inst->t_global_obj.nTrackType=2;
			inst->t_global_obj.nMeanShiftFrame=0;
		}
		if(DRAW_TRIGGER_TRACK == input_param->dynamic_param.message)
		{
			line_info.start_x  = inst->t_global_obj.tAutoTrackObj.X-16;
			line_info.start_y= inst->t_global_obj.tAutoTrackObj.Y;
			line_info.end_x = line_info.start_x+32;
			line_info.end_y = line_info.start_y+32;
			yuv_value.y_value = 255;
			yuv_value.cb_value	= 0;
			yuv_value.cr_value	= 255;
			draw_rectangle(inst,output_buf, &yuv_value, &line_info);
		}
		nRet=ObjeckTracker(&(inst->t_global_obj.tAutoTrackObj),pOutYUV);
		inst->t_global_obj.debugValue0 = 9;
		if(DRAW_TRIGGER_TRACK == input_param->dynamic_param.message)
		{
			if(inst->t_global_obj.tAutoTrackObj.X<nFaceWidth)
			{
				x0=0;
			}
			else
			{
				x0=inst->t_global_obj.tAutoTrackObj.X-nFaceWidth;
			}
			x1=inst->t_global_obj.tAutoTrackObj.X+nFaceWidth;
			y0=input_param->dynamic_param.mid_y*y_offset;
			y1=input_param->dynamic_param.mid_y*y_offset+nFaceWidth;
			line_info.start_x  = x0;//inst->t_global_obj.tAutoTrackObj.X;
			line_info.start_y= y0;//inst->t_global_obj.tAutoTrackObj.Y;
			line_info.end_x = x1;//line_info.start_x+(nFaceWidth>>1);
			line_info.end_y = y1;//line_info.start_y+32;
			yuv_value.y_value = 255;
			yuv_value.cb_value	= 0;
			yuv_value.cr_value	= 255;
			//draw_rectangle(inst,output_buf, &yuv_value, &line_info);
			if(inst->t_global_obj.tAutoTrackObj.X<16)
			{
				line_info.start_x  = 0;
			}
			else
			{
				line_info.start_x  = inst->t_global_obj.tAutoTrackObj.X-16;
			}
			line_info.start_y= inst->t_global_obj.tAutoTrackObj.Y;
			line_info.end_x = line_info.start_x+32;
			line_info.end_y = line_info.start_y+32;
			yuv_value.y_value = 255;
			yuv_value.cb_value	= 255;
			yuv_value.cr_value	= 255;
			draw_rectangle(inst,output_buf, &yuv_value, &line_info);

		}
		
		inst->t_global_obj.m_offset=inst->t_global_obj.tAutoTrackObj.X/x_offset;
		//inst->t_global_obj.n_offset=input_param->dynamic_param.mid_y;
		index = (inst->t_global_obj.pst_trak_obj.uiPointNum - 1) & 0x7f;
		inst->t_global_obj.pst_trak_obj.astPoint[index].sX = inst->t_global_obj.m_offset;
		//inst->t_global_obj.pst_trak_obj.astPoint[index].sY = inst->t_global_obj.n_offset;
		CtrlCam(inst,output_param);
	}

	inst->t_global_obj.debugValue0 = 10;
	if((inst->t_global_obj.track_start == 0)&&(inst->t_global_obj.nPos1MvFlag == 1))
	{
		output_param->position1_mv_status=1;
	}

	output_param->track_status = inst->t_global_obj.track_start;
	//output_param->reserve1 = inst->t_global_obj.nTrackType;
	//output_param->reserve2 = input_param->dynamic_param.TrackOtherParam.pos1_trigger_num;
	//output_param->reserve3 = inst->t_global_obj.tAutoTrackObj.ntestx;
	//output_param->reserve4 = inst->t_global_obj.nTrackType;
	if(DRAW_TRIGGER_TRACK == input_param->dynamic_param.message)
	{
		draw_track_rect(inst,output_buf);
	}
	//draw_target(inst,output_buf);
	//Cache_inv(output_buf,bufferlen,Cache_Type_ALL,TRUE);
	Cache_wbInv(output_buf,bufferlen,Cache_Type_ALL,TRUE);
	inst->t_global_obj.debugValue0 = 11;
	return 0;
}


int32_t	process_only_switch(TrackHand *inst,int8_t *input_buf,int8_t *output_buf, 
	ITRACK_OutArgs *output_param)
{
	ITRACK_Params *input_param;
	if((NULL==inst)||(NULL==output_buf)||(NULL==output_param))
	{
		return -1;
	}
	input_param=&(inst->input_param);

	if(0 ==input_param->dynamic_param.process_type)
	{
		memset(output_param, 0, sizeof(ITRACK_OutArgs));
		data_copy(inst,output_buf);
		image_y_value_compare_only_switch(inst,input_buf,output_buf,output_param);
	}
	if(DRAW_TRIGGER_TRACK == input_param->dynamic_param.message)
	{
		draw_track_rect(inst,output_buf);
	}
	return 0;
}

int32_t	track_process(TrackHand *inst,int8_t *input_buf,int8_t *output_buf, 
	ITRACK_OutArgs *output_param)
{
	if((NULL==inst)||(NULL==output_buf)||(NULL==output_param))
	{
		return -1;
	}
	if(inst->input_param.dynamic_param.TrackOtherParam.nTrackSwitchType==0)
	{
		process(inst,output_buf,output_param);
	}
	else
	{
		process_only_switch(inst,input_buf,output_buf,output_param);
	}
}
