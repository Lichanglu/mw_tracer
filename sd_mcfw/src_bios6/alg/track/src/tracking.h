#ifndef _TRACKING_H
#define _TRACKING_H

#define TK_MAX_OBJECT 64
#define TK_MAX_POINT   128
#define TK_MAX_AFRAME_DISTANCE_SCALE    15
#define TK_MAX_DISTANCE_SCALE_LEVEL1      8 
#define TK_MAX_DISTANCE_SCALE_LEVEL2      4 
#define TK_LOST_FRAME_LEVEL1                           10
#define TK_LOST_FRAME_LEVEL2                           60    //25
#define TK_LOST_FRAME_LEVEL3                           128   //50
#define TK_MIN_ONTRAKING_FRAMES              4

#define TK_FUNC_MAX_COUNT   8
#define TK_FUNC_ALL_STOP     0
#define TK_FUNC_ALL_START   0xff
#define TK_FUNC_DISTANCE     1
#define TK_FUNC_SIZE                 2
#define TK_FUNC_SPEED            4
#define TK_FUNC_RELATIVE_POS   8



typedef struct strDetectedInfo
{
 short sX;
 short sY;
unsigned short usWidth;
unsigned short usHeight;
}STR_POS_INFO;

typedef struct strObjectsDetected
{
unsigned char ucNumber;    //目标个数
unsigned char aucResv[3];
STR_POS_INFO stTrigger;		//触发框信息
STR_POS_INFO astInfo[100];  //目标信息
unsigned int nCamMoveFlag;
}STR_OBJECTS_DETECTED;

typedef struct strPos
{
 short sX;
 short sY;
}STR_POS;

typedef struct strObjectTraking
{
	unsigned short usAreaWidth;
	unsigned short usAreaHeight;
	//unsigned short usObjWidth;
	//unsigned short usObjHeight;
	float                  fSpeed;
	float                  fDirection;
	unsigned int    uiLostFrame;   //连着丢失的帧数
	unsigned int    uiCaptureFrame;
	unsigned int    uiPointNum;                          //from start to miss,  pos = uiPointNum&0x7f
	unsigned char ucState;                                    //0: miss  1:start  2:on traking 
	unsigned char	ucInitRdy;
	unsigned char aucLost[TK_LOST_FRAME_LEVEL3];
	STR_POS_INFO  astPoint[128];  //记录每帧目标信息
}STR_OBJECT_TRAKING;

int TrakingObjectInit(STR_OBJECTS_DETECTED* pstDetObj, STR_OBJECT_TRAKING* pstTrakObj,
	                                                        unsigned short usPicWidth, unsigned short usPicHeight);
int TrakingObjectContinue(STR_OBJECTS_DETECTED* pstDetObj, STR_OBJECT_TRAKING* pstTrakObj,
	                                                        unsigned short usPicWidth, unsigned short usPicHeight, unsigned char ucCommand);
int TrakingObjectProcess(unsigned short usPicWidth, unsigned short usPicHeight,
	                                            STR_OBJECTS_DETECTED* pstDetObj, STR_OBJECT_TRAKING* pstTrakObj, unsigned char  ucCommand);
int TrakingObjectDistanceMod(STR_OBJECTS_DETECTED* pstDetObj, STR_OBJECT_TRAKING* pstTrakObj,
	                                                        unsigned char ucCommand, unsigned short usMinW, unsigned short usMinH);
int TrakingObjectSizeMod(STR_OBJECTS_DETECTED* pstDetObj, STR_OBJECT_TRAKING* pstTrakObj,
	                                     unsigned char ucCommand);
int TrakingObjectJudgeMod(STR_OBJECTS_DETECTED* pstDetObj,STR_OBJECT_TRAKING* pstTrakObj);
int TrakingObjectInputCheck(STR_OBJECTS_DETECTED* pstDetObj, unsigned short usWidth,unsigned short usHeight);


#endif
