#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AutoTrack.h"
//#define YUV 1
#define min(a, b)  (((a) < (b)) ? (a) : (b))
#define max(a, b)  (((a) > (b)) ? (a) : (b)) 
//#define _spacku4(a,b) (((b) > (a))?((((b) > (255))?(255):(b))):(a))
#define  MEANSHIFT_ITARATION_NO 8
#define  DISTANCE_ITARATION_NO 1
#define  ALPHA 1
#define  EDGE_DETECT_TRESHOLD  320
#define  EDGE_DETECT_TRESHOLD_YUV  32
#define  EDGE_WEIGHT 4096

#define RGB(r, g ,b)  ((unsigned int) (((unsigned char) (r) | ((unsigned char) (g) << 8)) | (((unsigned int) (unsigned char) (b)) << 16))) 

short r_table_v[256]={
-176,-175,-173,-172,-171,-169,-168,-166,-165,-164,-162,-161,-160,-158,-157,-155,
-154,-153,-151,-150,-149,-147,-146,-144,-143,-142,-140,-139,-138,-136,-135,-133,
-132,-131,-129,-128,-127,-125,-124,-122,-121,-120,-118,-117,-116,-114,-113,-111,
-110,-109,-107,-106,-105,-103,-102,-100,-99,-98,-96,-95,-94,-92,-91,-89,
-88,-87,-85,-84,-83,-81,-80,-78,-77,-76,-74,-73,-72,-70,-69,-67,
-66,-65,-63,-62,-61,-59,-58,-56,-55,-54,-52,-51,-50,-48,-47,-45,
-44,-43,-41,-40,-39,-37,-36,-34,-33,-32,-30,-29,-28,-26,-25,-23,
-22,-21,-19,-18,-17,-15,-14,-12,-11,-10,-8,-7,-6,-4,-3,-1,
0,1,3,4,6,7,8,10,11,12,14,15,17,18,19,21,
22,23,25,26,28,29,30,32,33,34,36,37,39,40,41,43,
44,45,47,48,50,51,52,54,55,56,58,59,61,62,63,65,
66,67,69,70,72,73,74,76,77,78,80,81,83,84,85,87,
88,89,91,92,94,95,96,98,99,100,102,103,105,106,107,109,
110,111,113,114,116,117,118,120,121,122,124,125,127,128,129,131,
132,133,135,136,138,139,140,142,143,144,146,147,149,150,151,153,
154,155,157,158,160,161,162,164,165,166,168,169,171,172,173,175,
};
char g_table_u[256]={
44,44,43,43,43,42,42,42,41,41,41,40,40,40,39,39,
39,38,38,37,37,37,36,36,36,35,35,35,34,34,34,33,
33,33,32,32,32,31,31,31,30,30,30,29,29,29,28,28,
28,27,27,26,26,26,25,25,25,24,24,24,23,23,23,22,
22,22,21,21,21,20,20,20,19,19,19,18,18,18,17,17,
17,16,16,15,15,15,14,14,14,13,13,13,12,12,12,11,
11,11,10,10,10,9,9,9,8,8,8,7,7,7,6,6,
6,5,5,4,4,4,3,3,3,2,2,2,1,1,1,0,
0,0,-1,-1,-1,-2,-2,-2,-3,-3,-3,-4,-4,-4,-5,-5,
-6,-6,-6,-7,-7,-7,-8,-8,-8,-9,-9,-9,-10,-10,-10,-11,
-11,-11,-12,-12,-12,-13,-13,-13,-14,-14,-14,-15,-15,-15,-16,-16,
-17,-17,-17,-18,-18,-18,-19,-19,-19,-20,-20,-20,-21,-21,-21,-22,
-22,-22,-23,-23,-23,-24,-24,-24,-25,-25,-25,-26,-26,-26,-27,-27,
-28,-28,-28,-29,-29,-29,-30,-30,-30,-31,-31,-31,-32,-32,-32,-33,
-33,-33,-34,-34,-34,-35,-35,-35,-36,-36,-36,-37,-37,-37,-38,-38,
-39,-39,-39,-40,-40,-40,-41,-41,-41,-42,-42,-42,-43,-43,-43,-44,
};
char g_table_v[256]={
90,89,89,88,87,86,86,85,84,84,83,82,82,81,80,79,
79,78,77,77,76,75,75,74,73,72,72,71,70,70,69,68,
68,67,66,65,65,64,63,63,62,61,60,60,59,58,58,57,
56,56,55,54,53,53,52,51,51,50,49,49,48,47,46,46,
45,44,44,43,42,41,41,40,39,39,38,37,37,36,35,34,
34,33,32,32,31,30,30,29,28,27,27,26,25,25,24,23,
23,22,21,20,20,19,18,18,17,16,15,15,14,13,13,12,
11,11,10,9,8,8,7,6,6,5,4,4,3,2,1,1,
0,-1,-1,-2,-3,-4,-4,-5,-6,-6,-7,-8,-8,-9,-10,-11,
-11,-12,-13,-13,-14,-15,-15,-16,-17,-18,-18,-19,-20,-20,-21,-22,
-23,-23,-24,-25,-25,-26,-27,-27,-28,-29,-30,-30,-31,-32,-32,-33,
-34,-34,-35,-36,-37,-37,-38,-39,-39,-40,-41,-41,-42,-43,-44,-44,
-45,-46,-46,-47,-48,-49,-49,-50,-51,-51,-52,-53,-53,-54,-55,-56,
-56,-57,-58,-58,-59,-60,-60,-61,-62,-63,-63,-64,-65,-65,-66,-67,
-68,-68,-69,-70,-70,-71,-72,-72,-73,-74,-75,-75,-76,-77,-77,-78,
-79,-79,-80,-81,-82,-82,-83,-84,-84,-85,-86,-86,-87,-88,-89,-89,
};
short b_table_u[256]={
-222,-220,-219,-217,-215,-213,-212,-210,-208,-206,-205,-203,-201,-199,-198,-196,
-194,-193,-191,-189,-187,-186,-184,-182,-180,-179,-177,-175,-173,-172,-170,-168,
-167,-165,-163,-161,-160,-158,-156,-154,-153,-151,-149,-147,-146,-144,-142,-140,
-139,-137,-135,-134,-132,-130,-128,-127,-125,-123,-121,-120,-118,-116,-114,-113,
-111,-109,-108,-106,-104,-102,-101,-99,-97,-95,-94,-92,-90,-88,-87,-85,
-83,-82,-80,-78,-76,-75,-73,-71,-69,-68,-66,-64,-62,-61,-59,-57,
-56,-54,-52,-50,-49,-47,-45,-43,-42,-40,-38,-36,-35,-33,-31,-29,
-28,-26,-24,-23,-21,-19,-17,-16,-14,-12,-10,-9,-7,-5,-3,-2,
0,2,3,5,7,9,10,12,14,16,17,19,21,23,24,26,
28,29,31,33,35,36,38,40,42,43,45,47,49,50,52,54,
56,57,59,61,62,64,66,68,69,71,73,75,76,78,80,82,
83,85,87,88,90,92,94,95,97,99,101,102,104,106,108,109,
111,113,114,116,118,120,121,123,125,127,128,130,132,134,135,137,
139,140,142,144,146,147,149,151,153,154,156,158,160,161,163,165,
167,168,170,172,173,175,177,179,180,182,184,186,187,189,191,193,
194,196,198,199,201,203,205,206,208,210,212,213,215,217,219,220,
};
// Returns the edge insformation of a pixel at (x,y)
unsigned char CheckEdgeExistance(t_TrackingObject *tpTrackingObj,unsigned char *pInYUV[2],unsigned int _x,unsigned int _y)
{
	unsigned char  E = 0;
	int  GrayCenter = 0;
	int  GrayLeft = 0;
	int  GrayRight = 0;
	int  GrayUp = 0;
	int  GrayDown = 0;
	unsigned int pixelValues = 0;
	int R,G,B;
	unsigned char Y,U,V;
	unsigned int nWidth=tpTrackingObj->nImageWidth;
	unsigned int nHeight=tpTrackingObj->nImageHeight;
	unsigned int x_2=(_x>>1)<<1;
	unsigned int xsub1_2=((_x-1)>>1)<<1;
	unsigned int xadd1_2=((_x+1)>>1)<<1;
	unsigned int y_2=_y>>1;
	unsigned int ysub1_2=(_y-1)>>1;
	unsigned int yadd1_2=(_y+1)>>1;
	
	Y=*(pInYUV[0]+nWidth*_y+_x);
	U=*(pInYUV[1]+nWidth*y_2+x_2);
	V=*(pInYUV[1]+nWidth*y_2+x_2+1);
	R=(int)Y+(int)r_table_v[V];
	G=(int)Y+(int)g_table_u[U]+(int)g_table_v[V];
	//B=(int)Y+(int)b_table_u[U];
	
	R=_spacku4(0,R);
	G=_spacku4(0,G);
	//R=_min(_max(0,R),255);
	//G=_min(_max(0,G),255);
	//B=_min(_max(0,B),255);
	GrayCenter = (int)(3*R+7*G);

	Y=*(pInYUV[0]+nWidth*_y+(_x-1));
	U=*(pInYUV[1]+nWidth*y_2+xsub1_2);
	V=*(pInYUV[1]+nWidth*y_2+xsub1_2+1);
	R=(int)Y+(int)r_table_v[V];
	G=(int)Y+(int)g_table_u[U]+(int)g_table_v[V];
	R=_spacku4(0,R);
	G=_spacku4(0,G);
	//R=_min(_max(0,R),255);
	//G=_min(_max(0,G),255);
	GrayLeft = (int)(3*R+7*G);

	Y=*(pInYUV[0]+nWidth*_y+(_x+1));
	U=*(pInYUV[1]+nWidth*y_2+xadd1_2);
	V=*(pInYUV[1]+nWidth*y_2+xadd1_2+1);
	R=(int)Y+(int)r_table_v[V];
	G=(int)Y+(int)g_table_u[U]+(int)g_table_v[V];
	R=_spacku4(0,R);
	G=_spacku4(0,G);
	//R=_min(_max(0,R),255);
	//G=_min(_max(0,G),255);
	GrayRight = (int)(3*R+7*G);

	Y=*(pInYUV[0]+nWidth*(_y-1)+_x);
	U=*(pInYUV[1]+nWidth*ysub1_2+x_2);
	V=*(pInYUV[1]+nWidth*ysub1_2+x_2+1);
	R=(int)Y+(int)r_table_v[V];
	G=(int)Y+(int)g_table_u[U]+(int)g_table_v[V];
	R=_spacku4(0,R);
	G=_spacku4(0,G);
	//R=_min(_max(0,R),255);
	//G=_min(_max(0,G),255);
	GrayUp = (int)(3*R+7*G);

	Y=*(pInYUV[0]+nWidth*(_y+1)+_x);
	U=*(pInYUV[1]+nWidth*yadd1_2+x_2);
	V=*(pInYUV[1]+nWidth*yadd1_2+x_2+1);
	R=(int)Y+(int)r_table_v[V];
	G=(int)Y+(int)g_table_u[U]+(int)g_table_v[V];
	R=_spacku4(0,R);
	G=_spacku4(0,G);
	//R=_min(_max(0,R),255);
	//G=_min(_max(0,G),255);
	GrayDown = (int)(3*R+7*G);
	if ((abs((GrayCenter-GrayLeft))>EDGE_DETECT_TRESHOLD)
		||(abs((GrayCenter-GrayRight))>EDGE_DETECT_TRESHOLD)
		||(abs((GrayCenter-GrayUp))>EDGE_DETECT_TRESHOLD)
		||(abs((GrayCenter-GrayDown))>EDGE_DETECT_TRESHOLD))
	{
		E = 1;
	}
	return(E);
}
// Computes weights and drives the new location of object in the next frame
void FindWightsAndCOM(t_TrackingObject *tpTrackingObj,unsigned char *pInYUV[2], unsigned int *histogram)
{
	unsigned int  i = 0;
	unsigned int  x = 0;
	unsigned int  y = 0;
	unsigned int minx,maxx,miny,maxy;
	unsigned char  E = 0;
	unsigned long long  sumOfWeights = 0;
	unsigned int  ptr  = 0;
	unsigned char  qR = 0,qG = 0,qB = 0;
	unsigned long long   newX = 0;
	unsigned long long   newY = 0;
	unsigned int pixelValues = 0;
	unsigned char Y,U,V;
	unsigned int nWidth=tpTrackingObj->nImageWidth;

	unsigned int *weights = tpTrackingObj->weights;
	int R,G,B;
	int  GrayCenter = 0;
	int  GrayLeft = 0;
	int  GrayRight = 0;
	int  GrayUp = 0;
	int  GrayDown = 0;
	unsigned int x_2;
	unsigned int xsub1_2;
	unsigned int xadd1_2;
	unsigned int y_2;
	unsigned int ysub1_2;
	unsigned int yadd1_2;

	for (i=0;i<HISTOGRAM_LEN;i++)
	{
		if (histogram[i] >0 )
		{
			weights[i] = (tpTrackingObj->initHistogram[i]*tpTrackingObj->nCurPixelNum<<7)/(histogram[i]*tpTrackingObj->nInitPixelNum);
		}
		else
		{
			weights[i] = 0;
		}
	}

	if(tpTrackingObj->X>tpTrackingObj->W+tpTrackingObj->LimitX0)
	{
		minx=tpTrackingObj->X-tpTrackingObj->W;
	}
	else
	{
		minx=tpTrackingObj->LimitX0;
	}
	maxx=min(tpTrackingObj->X+tpTrackingObj->W,tpTrackingObj->LimitX1);
	if(tpTrackingObj->Y>tpTrackingObj->H+tpTrackingObj->LimitY0)
	{
		miny=tpTrackingObj->Y-tpTrackingObj->H;
	}
	else
	{
		miny=tpTrackingObj->LimitY0;
	}
	maxy=min(tpTrackingObj->Y+tpTrackingObj->H,tpTrackingObj->LimitY1);
	for (y=miny;y<=maxy;y++)
	{
		for (x=minx;x<=maxx;x+=2)
		{
			x_2=(x>>1)<<1;
			xsub1_2=((x-1)>>1)<<1;
			xadd1_2=((x+1)>>1)<<1;
			y_2=y>>1;
			ysub1_2=(y-1)>>1;
			yadd1_2=(y+1)>>1;
			Y=*(pInYUV[0]+nWidth*y+x);
			U=*(pInYUV[1]+nWidth*y_2+x_2);
			V=*(pInYUV[1]+nWidth*y_2+x_2+1);
			
			R=(int)Y+(int)r_table_v[V];
			G=(int)Y+(int)g_table_u[U]+(int)g_table_v[V];
			B=(int)Y+(int)b_table_u[U];
			R=_spacku4(0,R);
			G=_spacku4(0,G);
			B=_spacku4(0,B);
			GrayCenter = (int)(3*R+7*G);
			qR=R>>4;
			qG=G>>4;
			qB=B>>4;
			
			Y=*(pInYUV[0]+nWidth*y+(x-1));
			U=*(pInYUV[1]+nWidth*y_2+xsub1_2);
			V=*(pInYUV[1]+nWidth*y_2+xsub1_2+1);
			R=(int)Y+(int)r_table_v[V];
			G=(int)Y+(int)g_table_u[U]+(int)g_table_v[V];
			R=_spacku4(0,R);
			G=_spacku4(0,G);
			GrayLeft = (int)(3*R+7*G);
			
			Y=*(pInYUV[0]+nWidth*y+(x+1));
			U=*(pInYUV[1]+nWidth*y_2+xadd1_2);
			V=*(pInYUV[1]+nWidth*y_2+xadd1_2+1);
			R=(int)Y+(int)r_table_v[V];
			G=(int)Y+(int)g_table_u[U]+(int)g_table_v[V];
			R=_spacku4(0,R);
			G=_spacku4(0,G);
			GrayRight = (int)(3*R+7*G);
			
			Y=*(pInYUV[0]+nWidth*(y-1)+x);
			U=*(pInYUV[1]+nWidth*ysub1_2+x_2);
			V=*(pInYUV[1]+nWidth*ysub1_2+x_2+1);
			R=(int)Y+(int)r_table_v[V];
			G=(int)Y+(int)g_table_u[U]+(int)g_table_v[V];
			R=_spacku4(0,R);
			G=_spacku4(0,G);
			GrayUp = (int)(3*R+7*G);
			
			Y=*(pInYUV[0]+nWidth*(y+1)+x);
			U=*(pInYUV[1]+nWidth*yadd1_2+x_2);
			V=*(pInYUV[1]+nWidth*yadd1_2+x_2+1);
			R=(int)Y+(int)r_table_v[V];
			G=(int)Y+(int)g_table_u[U]+(int)g_table_v[V];
			R=_spacku4(0,R);
			G=_spacku4(0,G);
			GrayDown = (int)(3*R+7*G);
			if ((abs((GrayCenter-GrayLeft))>EDGE_DETECT_TRESHOLD)
				||(abs((GrayCenter-GrayRight))>EDGE_DETECT_TRESHOLD)
				||(abs((GrayCenter-GrayUp))>EDGE_DETECT_TRESHOLD)
				||(abs((GrayCenter-GrayDown))>EDGE_DETECT_TRESHOLD))
			{
				E = 1;
			}
			else
			{
				E = 0;
			}
			ptr = EDGE_WEIGHT*E+(qR<<8)+(qG<<4)+qB;
			newX += (weights[ptr]*x);
			newY += (weights[ptr]*y);
			sumOfWeights += weights[ptr];
		}
	}
	if (sumOfWeights>0)
	{
		tpTrackingObj->X = (unsigned int)(newX/sumOfWeights);
		tpTrackingObj->Y = (unsigned int)(newY/sumOfWeights);
	}
	//printf("tpTrackingObj->X=%d,Y=%d\n",tpTrackingObj->X,tpTrackingObj->Y);
}
//Extracts the histogram of box
unsigned int FindHistogram(t_TrackingObject *tpTrackingObj,unsigned char *pInYUV[2], unsigned int *histogram)
{
	unsigned int  i = 0;
	unsigned int  x = 0;
	unsigned int  y = 0;
	unsigned int minx,maxx,miny,maxy;
	unsigned char  E = 0;
	unsigned char  qR = 0,qG = 0,qB = 0;
	int pixelValues = 0;
	unsigned int numberOfPixel = 0;
	unsigned char Y,U,V;
	unsigned int nWidth=tpTrackingObj->nImageWidth;
	unsigned int nHeight=tpTrackingObj->nImageHeight;
	int R,G,B;
	int  GrayCenter = 0;
	int  GrayLeft = 0;
	int  GrayRight = 0;
	int  GrayUp = 0;
	int  GrayDown = 0;
	unsigned int x_2;
	unsigned int xsub1_2;
	unsigned int xadd1_2;
	unsigned int y_2;
	unsigned int ysub1_2;
	unsigned int yadd1_2;

	for (i=0;i<HISTOGRAM_LEN;i++)
	{
		histogram[i] = 0;
	}
	if(tpTrackingObj->X>tpTrackingObj->W+tpTrackingObj->LimitX0)
	{
		minx=tpTrackingObj->X-tpTrackingObj->W;
	}
	else
	{
		minx=tpTrackingObj->LimitX0;
	}
	maxx=min(tpTrackingObj->X+tpTrackingObj->W,tpTrackingObj->LimitX1);
	if(tpTrackingObj->Y>tpTrackingObj->H+tpTrackingObj->LimitY0)
	{
		miny=tpTrackingObj->Y-tpTrackingObj->H;
	}
	else
	{
		miny=tpTrackingObj->LimitY0;
	}
	maxy=min(tpTrackingObj->Y+tpTrackingObj->H,tpTrackingObj->LimitY1);
	for (y=miny;y<=maxy;y++)
	{
		for (x=minx;x<=maxx;x+=2)
		{
			x_2=(x>>1)<<1;
			xsub1_2=((x-1)>>1)<<1;
			xadd1_2=((x+1)>>1)<<1;
			y_2=y>>1;
			ysub1_2=(y-1)>>1;
			yadd1_2=(y+1)>>1;
			Y=*(pInYUV[0]+nWidth*y+x);
			U=*(pInYUV[1]+nWidth*y_2+x_2);
			V=*(pInYUV[1]+nWidth*y_2+x_2+1);
			R=(int)Y+(int)r_table_v[V];
			G=(int)Y+(int)g_table_u[U]+(int)g_table_v[V];
			B=(int)Y+(int)b_table_u[U];
			R=_spacku4(0,R);
			G=_spacku4(0,G);
			B=_spacku4(0,B);
			GrayCenter = (int)(3*R+7*G);
			qR=R>>4;
			qG=G>>4;
			qB=B>>4;

			Y=*(pInYUV[0]+nWidth*y+(x-1));
			U=*(pInYUV[1]+nWidth*y_2+xsub1_2);
			V=*(pInYUV[1]+nWidth*y_2+xsub1_2+1);
			R=(int)Y+(int)r_table_v[V];
			G=(int)Y+(int)g_table_u[U]+(int)g_table_v[V];
			R=_spacku4(0,R);
			G=_spacku4(0,G);
			GrayLeft = (int)(3*R+7*G);
			
			Y=*(pInYUV[0]+nWidth*y+(x+1));
			U=*(pInYUV[1]+nWidth*y_2+xadd1_2);
			V=*(pInYUV[1]+nWidth*y_2+xadd1_2+1);
			R=(int)Y+(int)r_table_v[V];
			G=(int)Y+(int)g_table_u[U]+(int)g_table_v[V];
			R=_spacku4(0,R);
			G=_spacku4(0,G);
			GrayRight = (int)(3*R+7*G);
			
			Y=*(pInYUV[0]+nWidth*(y-1)+x);
			U=*(pInYUV[1]+nWidth*ysub1_2+x_2);
			V=*(pInYUV[1]+nWidth*ysub1_2+x_2+1);
			R=(int)Y+(int)r_table_v[V];
			G=(int)Y+(int)g_table_u[U]+(int)g_table_v[V];
			R=_spacku4(0,R);
			G=_spacku4(0,G);
			GrayUp = (int)(3*R+7*G);
			
			Y=*(pInYUV[0]+nWidth*(y+1)+x);
			U=*(pInYUV[1]+nWidth*yadd1_2+x_2);
			V=*(pInYUV[1]+nWidth*yadd1_2+x_2+1);
			R=(int)Y+(int)r_table_v[V];
			G=(int)Y+(int)g_table_u[U]+(int)g_table_v[V];
			R=_spacku4(0,R);
			G=_spacku4(0,G);
			GrayDown = (int)(3*R+7*G);
			if ((abs((GrayCenter-GrayLeft))>EDGE_DETECT_TRESHOLD)
				||(abs((GrayCenter-GrayRight))>EDGE_DETECT_TRESHOLD)
				||(abs((GrayCenter-GrayUp))>EDGE_DETECT_TRESHOLD)
				||(abs((GrayCenter-GrayDown))>EDGE_DETECT_TRESHOLD))
			{
				E = 1;
			}
			else
			{
				E = 0;
			}

			histogram[EDGE_WEIGHT*E+(qR<<8)+(qG<<4)+qB] += 1;
			numberOfPixel++;
		}
	}
	return numberOfPixel;

}
// Computes weights and drives the new location of object in the next frame
void FindWightsAndCOM_yuv(t_TrackingObject *tpTrackingObj,unsigned char *pInYUV[2], unsigned int *histogram)
{
	unsigned int  i = 0;
	unsigned int  x = 0;
	unsigned int  y = 0;
	unsigned int minx,maxx,miny,maxy;
	unsigned char  E = 0;
	unsigned long long  sumOfWeights = 0;
	unsigned int  ptr  = 0;
	unsigned char  qY = 0,qU = 0,qV = 0;
	unsigned long long   newX = 0;
	unsigned long long   newY = 0;
	unsigned int pixelValues = 0;
	unsigned char Y,U,V;
	unsigned int nWidth=tpTrackingObj->nImageWidth;
	unsigned int *weights = tpTrackingObj->weights;
	int  GrayCenter = 0;
	int  GrayLeft = 0;
	int  GrayRight = 0;
	int  GrayUp = 0;
	int  GrayDown = 0;
	unsigned int x_2;
	unsigned int xsub1_2;
	unsigned int xadd1_2;
	unsigned int y_2;
	unsigned int ysub1_2;
	unsigned int yadd1_2;

	for (i=0;i<HISTOGRAM_LEN;i++)
	{
		if (histogram[i] >0 )
		{
			weights[i] = (tpTrackingObj->initHistogram[i]*tpTrackingObj->nCurPixelNum<<7)/(histogram[i]*tpTrackingObj->nInitPixelNum);
		}
		else
		{
			weights[i] = 0;
		}
	}


	if(tpTrackingObj->X>tpTrackingObj->W+tpTrackingObj->LimitX0)
	{
		minx=tpTrackingObj->X-tpTrackingObj->W;
	}
	else
	{
		minx=tpTrackingObj->LimitX0;
	}
	maxx=min(tpTrackingObj->X+tpTrackingObj->W,tpTrackingObj->LimitX1);
	if(tpTrackingObj->Y>tpTrackingObj->H+tpTrackingObj->LimitY0)
	{
		miny=tpTrackingObj->Y-tpTrackingObj->H;
	}
	else
	{
		miny=tpTrackingObj->LimitY0;
	}
	maxy=min(tpTrackingObj->Y+tpTrackingObj->H,tpTrackingObj->LimitY1);
	for (y=miny;y<=maxy;y++)
	{
		for (x=minx;x<=maxx;x+=2)
		{
			x_2=(x>>1)<<1;
			xsub1_2=((x-1)>>1)<<1;
			xadd1_2=((x+1)>>1)<<1;
			y_2=y>>1;
			ysub1_2=(y-1)>>1;
			yadd1_2=(y+1)>>1;
			Y=*(pInYUV[0]+nWidth*y+x);
			U=*(pInYUV[1]+nWidth*y_2+x_2);
			V=*(pInYUV[1]+nWidth*y_2+x_2+1);
			
			GrayCenter = Y;
			qY=Y>>4;
			qU=U>>4;
			qV=V>>4;
			
			Y=*(pInYUV[0]+nWidth*y+(x-1));
			U=*(pInYUV[1]+nWidth*y_2+xsub1_2);
			V=*(pInYUV[1]+nWidth*y_2+xsub1_2+1);
			GrayLeft = Y;
			
			Y=*(pInYUV[0]+nWidth*y+(x+1));
			U=*(pInYUV[1]+nWidth*y_2+xadd1_2);
			V=*(pInYUV[1]+nWidth*y_2+xadd1_2+1);
			GrayRight = Y;
			
			Y=*(pInYUV[0]+nWidth*(y-1)+x);
			U=*(pInYUV[1]+nWidth*ysub1_2+x_2);
			V=*(pInYUV[1]+nWidth*ysub1_2+x_2+1);
			GrayUp = Y;
			
			Y=*(pInYUV[0]+nWidth*(y+1)+x);
			U=*(pInYUV[1]+nWidth*yadd1_2+x_2);
			V=*(pInYUV[1]+nWidth*yadd1_2+x_2+1);
			GrayDown = Y;
			if ((abs((GrayCenter-GrayLeft))>EDGE_DETECT_TRESHOLD_YUV)
				||(abs((GrayCenter-GrayRight))>EDGE_DETECT_TRESHOLD_YUV)
				||(abs((GrayCenter-GrayUp))>EDGE_DETECT_TRESHOLD_YUV)
				||(abs((GrayCenter-GrayDown))>EDGE_DETECT_TRESHOLD_YUV))
			{
				E = 1;
			}
			else
			{
				E = 0;
			}
			ptr = EDGE_WEIGHT*E+(qV<<8)+(qY<<4)+qU;
			newX += (weights[ptr]*x);
			newY += (weights[ptr]*y);
			sumOfWeights += weights[ptr];
		}
	}
	if (sumOfWeights>0)
	{
		tpTrackingObj->X = newX/sumOfWeights;
		tpTrackingObj->Y = newY/sumOfWeights;
	}
	//printf("tpTrackingObj->X=%d,Y=%d\n",tpTrackingObj->X,tpTrackingObj->Y);
}
//Extracts the histogram of box
unsigned int FindHistogram_yuv(t_TrackingObject *tpTrackingObj,unsigned char *pInYUV[2], unsigned int *histogram)
{
	unsigned int  i = 0;
	unsigned int  x = 0;
	unsigned int  y = 0;
	unsigned int minx,maxx,miny,maxy;
	unsigned char  E = 0;
	unsigned char  qY = 0,qU = 0,qV = 0;
	int pixelValues = 0;
	unsigned int numberOfPixel = 0;
	unsigned char Y,U,V;
	unsigned int nWidth=tpTrackingObj->nImageWidth;
	unsigned int nHeight=tpTrackingObj->nImageHeight;
	int  GrayCenter = 0;
	int  GrayLeft = 0;
	int  GrayRight = 0;
	int  GrayUp = 0;
	int  GrayDown = 0;
	unsigned int x_2;
	unsigned int xsub1_2;
	unsigned int xadd1_2;
	unsigned int y_2;
	unsigned int ysub1_2;
	unsigned int yadd1_2;

	for (i=0;i<HISTOGRAM_LEN;i++)
	{
		histogram[i] = 0;
	}
	if(tpTrackingObj->X>tpTrackingObj->W+tpTrackingObj->LimitX0)
	{
		minx=tpTrackingObj->X-tpTrackingObj->W;
	}
	else
	{
		minx=tpTrackingObj->LimitX0;
	}
	maxx=min(tpTrackingObj->X+tpTrackingObj->W,tpTrackingObj->LimitX1);
	if(tpTrackingObj->Y>tpTrackingObj->H+tpTrackingObj->LimitY0)
	{
		miny=tpTrackingObj->Y-tpTrackingObj->H;
	}
	else
	{
		miny=tpTrackingObj->LimitY0;
	}
	maxy=min(tpTrackingObj->Y+tpTrackingObj->H,tpTrackingObj->LimitY1);
	for (y=miny;y<=maxy;y++)
	{
		for (x=minx;x<=maxx;x+=2)
		{
			x_2=(x>>1)<<1;
			xsub1_2=((x-1)>>1)<<1;
			xadd1_2=((x+1)>>1)<<1;
			y_2=y>>1;
			ysub1_2=(y-1)>>1;
			yadd1_2=(y+1)>>1;
			Y=*(pInYUV[0]+nWidth*y+x);
			U=*(pInYUV[1]+nWidth*y_2+x_2);
			V=*(pInYUV[1]+nWidth*y_2+x_2+1);
			GrayCenter = Y;
			qY=Y>>4;
			qU=U>>4;
			qV=V>>4;

			Y=*(pInYUV[0]+nWidth*y+(x-1));
			U=*(pInYUV[1]+nWidth*y_2+xsub1_2);
			V=*(pInYUV[1]+nWidth*y_2+xsub1_2+1);
			GrayLeft = Y;
			
			Y=*(pInYUV[0]+nWidth*y+(x+1));
			U=*(pInYUV[1]+nWidth*y_2+xadd1_2);
			V=*(pInYUV[1]+nWidth*y_2+xadd1_2+1);
			GrayRight = Y;
			
			Y=*(pInYUV[0]+nWidth*(y-1)+x);
			U=*(pInYUV[1]+nWidth*ysub1_2+x_2);
			V=*(pInYUV[1]+nWidth*ysub1_2+x_2+1);
			GrayUp = Y;
			
			Y=*(pInYUV[0]+nWidth*(y+1)+x);
			U=*(pInYUV[1]+nWidth*yadd1_2+x_2);
			V=*(pInYUV[1]+nWidth*yadd1_2+x_2+1);
			GrayDown = Y;
			if ((abs((GrayCenter-GrayLeft))>EDGE_DETECT_TRESHOLD_YUV)
				||(abs((GrayCenter-GrayRight))>EDGE_DETECT_TRESHOLD_YUV)
				||(abs((GrayCenter-GrayUp))>EDGE_DETECT_TRESHOLD_YUV)
				||(abs((GrayCenter-GrayDown))>EDGE_DETECT_TRESHOLD_YUV))
			{
				E = 1;
			}
			else
			{
				E = 0;
			}
			histogram[EDGE_WEIGHT*E+(qV<<8)+(qY<<4)+qU] += 1;
			numberOfPixel++;
		}
	}
	return numberOfPixel;

}

// Mean-shift iteration 
unsigned int FindNextLocation(t_TrackingObject *tpTrackingObj,unsigned char *pInYUV[2])
{
	unsigned char  iteration = 0;
	unsigned int *currentHistogram = tpTrackingObj->currentHistogram;
	unsigned int i=0;
	unsigned int nx=0;
	tpTrackingObj->ntestx=tpTrackingObj->nLastX;
	for (iteration=0; iteration<MEANSHIFT_ITARATION_NO; iteration++)
	{
	#ifdef YUV
		tpTrackingObj->nCurPixelNum=FindHistogram_yuv(tpTrackingObj,pInYUV,currentHistogram); //current frame histogram
		FindWightsAndCOM_yuv(tpTrackingObj,pInYUV,currentHistogram);//derive weights and new location
	#else
		tpTrackingObj->nCurPixelNum=FindHistogram(tpTrackingObj,pInYUV,currentHistogram); //current frame histogram
		FindWightsAndCOM(tpTrackingObj,pInYUV,currentHistogram);//derive weights and new location
	#endif
	if(abs(tpTrackingObj->nLastX-tpTrackingObj->X)<10)
		{
			//return iteration;
			//break;
		}
		//越界处理
		if(abs((int)tpTrackingObj->X-(int)tpTrackingObj->nLastX)
			>(tpTrackingObj->LimitX1-tpTrackingObj->LimitX0)/2
			)
		{
			tpTrackingObj->ntestx=0xffffffff;
			tpTrackingObj->X=tpTrackingObj->nLastX;
		}
		else
		{
			tpTrackingObj->nLastX=tpTrackingObj->X;
		}
		if((tpTrackingObj->Y<=tpTrackingObj->LimitY0)
			||(tpTrackingObj->Y>=tpTrackingObj->LimitY1)
			)
		{
			tpTrackingObj->Y=tpTrackingObj->nLastY;
		}
		else
		{
			tpTrackingObj->nLastY=tpTrackingObj->Y;
		}
		//FindHistogram(tpTrackingObj,frame,currentHistogram);   //uptade histogram
		//UpdateInitialHistogram(tpTrackingObj,currentHistogram);//uptade initial histogram
	}
	nx=tpTrackingObj->X;
	return nx;
	//memcpy(tpTrackingObj->initHistogram,tpTrackingObj->currentHistogram,HISTOGRAM_LEN * sizeof(int));
	//return 0;
}
//initializes the object parameters 
void ObjectTrackerInit(t_TrackingObject *tpTrackingObj,
							unsigned char *pInYUV[2],
							int x,int y,
							int TrackWidth,int TrackHeight,
							int ImgWidth,int ImgHeight,
							int LimitX0,int LimitX1,int LimitY0,int LimitY1)
{
	tpTrackingObj->nImageWidth= ImgWidth;
	tpTrackingObj->nImageHeight= ImgHeight;
	tpTrackingObj->X = x;
	tpTrackingObj->Y = y;
	tpTrackingObj->nLastX = x;
	tpTrackingObj->nLastY = y;
	tpTrackingObj->W = TrackWidth;
	tpTrackingObj->H = TrackHeight;
	if(LimitX0<2)
	{
		tpTrackingObj->LimitX0=2;
	}
	else
	{
		tpTrackingObj->LimitX0=LimitX0;
	}
	if(LimitX1>ImgWidth-2)
	{
		tpTrackingObj->LimitX1=ImgWidth-2;
	}
	else
	{
		tpTrackingObj->LimitX1=LimitX1;
	}
	if(LimitY0<2)
	{
		tpTrackingObj->LimitY0=2;
	}
	else
	{
		tpTrackingObj->LimitY0=LimitY0;
	}
	if(LimitY1>ImgHeight-2)
	{
		tpTrackingObj->LimitY1=ImgHeight-2;
	}
	else
	{
		tpTrackingObj->LimitY1=LimitY1;
	}

	tpTrackingObj->Status = 1;
	#ifdef YUV
	tpTrackingObj->nInitPixelNum=FindHistogram_yuv(tpTrackingObj,pInYUV,tpTrackingObj->initHistogram);
	#else
	tpTrackingObj->nInitPixelNum=FindHistogram(tpTrackingObj,pInYUV,tpTrackingObj->initHistogram);
	#endif
}

int ObjeckTracker(t_TrackingObject *tpTrackingObj,unsigned char *pInYUV[2])
{
	int nRet=0;
	if (tpTrackingObj->Status)
	{
		nRet=FindNextLocation(tpTrackingObj,pInYUV);    
		//DrawObjectBox(tpTrackingObj,frame);
	}
	return nRet;
}

