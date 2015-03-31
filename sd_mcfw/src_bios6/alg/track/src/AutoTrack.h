#ifndef _AUTOTRACK_H
#define _AUTOTRACK_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//定义数据类型
#define HISTOGRAM_LEN 8192
typedef struct OBJECT
{
	int	Status;
	int	assignedAnObject;
	unsigned int	 nImageWidth;  //图象的宽度
	unsigned int	 nImageHeight; //图象的高度
	unsigned int	X;	 
	unsigned int	Y;	 
	unsigned int	W;	 
	unsigned int	H;
	unsigned int LimitX0;
	unsigned int LimitX1;
	unsigned int LimitY0;
	unsigned int LimitY1;
	unsigned int  *initHistogram; 
	unsigned int  *currentHistogram; 
	unsigned int	*weights;
	unsigned int nInitPixelNum;
	unsigned int nCurPixelNum;
	unsigned int	nLastX;	 
	unsigned int	nLastY;	 
	unsigned int ntestx;
}t_TrackingObject; 
extern void ObjectTrackerInit(t_TrackingObject *tpTrackingObj,
							unsigned char *pInYUV[2],
							int x,int y,
							int TrackWidth,int TrackHeight,
							int ImgWidth,int ImgHeight,
							int LimitX0,int LimitX1,int LimitY0,int LimitY1);
extern int ObjeckTracker(t_TrackingObject *tpTrackingObj,unsigned char *pInYUV[2]);

#endif


