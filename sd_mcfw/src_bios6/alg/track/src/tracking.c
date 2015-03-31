/********************************************************
name      : traking.c                                         ver:      0.2
function  : analyse detected objects and choose one of them to track
interface :TrakingObjectProcess(unsigned short usPicWidth, 
                                            unsigned short usPicHeight,
	                                         STR_OBJECTS_DETECTED* pstDetObj, 
	                                         STR_OBJECT_TRAKING* pstTrakObj, 
	                                         unsigned char  ucCommand)
               usPicWidth:the analysed video's width
               usPicHeight:the analysed video's height
               pstDetObj: the point to the struct for detected objects info
               pstTrakObj: the point to the struct for traking objects info
*********************************************************/


#include <stdio.h>
#include "tracking.h"
typedef float		float_t;

int TrakingObjectInit(STR_OBJECTS_DETECTED* pstDetObj, STR_OBJECT_TRAKING* pstTrakObj,
	                                                        unsigned short usPicWidth, unsigned short usPicHeight)
{
	STR_OBJECT_TRAKING *pstTrak = NULL;
	STR_OBJECTS_DETECTED* pstDet = NULL;
	unsigned int uiDistance = 0, uiMinDistance = 0, uiNearest = 0, i = 0, j = 0, uiNum = 0;	
	int iRet = 0;
	pstTrak = pstTrakObj;
	pstDet = pstDetObj ;

	uiNum = pstDet->ucNumber;

	if(pstDet->ucNumber == 0 && pstTrak->uiPointNum == 0)                       //no traking and detected objects
	{
	
	       if(pstTrak->ucInitRdy <= 50)
	       {
	           	pstTrak->ucInitRdy++;
			 	return 1;  
	       }
		  pstTrak->ucInitRdy = 0;
		  
		  return 0;
	}


	//检测参数
	iRet = TrakingObjectInputCheck(pstDet, usPicWidth, usPicHeight);
	if(iRet != 0)
	{
	     return iRet;
	}

	memset(pstTrakObj, 0, sizeof(STR_OBJECT_TRAKING));
	for(i = 0; i < uiNum; i++)
	{
		uiDistance = (pstDet->stTrigger.sX - pstDet->astInfo[i].sX) * (pstDet->stTrigger.sX - pstDet->astInfo[i].sX)
		  + (pstDet->stTrigger.sY- pstDet->astInfo[i].sY) * (pstDet->stTrigger.sY- pstDet->astInfo[i].sY);
		if(i == 0 || uiMinDistance > uiDistance)
		{
		  uiMinDistance = uiDistance;
		  uiNearest = i;
		}
	}
	pstTrak->astPoint[0].sX = pstDet->astInfo[uiNearest].sX;
	pstTrak->astPoint[0].sY = pstDet->astInfo[uiNearest].sY;
	pstTrak->astPoint[0].usWidth = pstDet->astInfo[uiNearest].usWidth;
	pstTrak->astPoint[0].usHeight = pstDet->astInfo[uiNearest].usHeight;
	pstTrak->uiPointNum = 1;
	pstTrak->uiCaptureFrame = 1;
	pstTrak->ucState =1;
	pstTrak->aucLost[0] = 0;

	return 1;
}

int TrakingObjectDistanceMod(STR_OBJECTS_DETECTED* pstDetObj, STR_OBJECT_TRAKING* pstTrakObj,
	                                                        unsigned char ucCommand, unsigned short usMinW, unsigned short usMinH)
{
	STR_OBJECT_TRAKING *pstTrak = NULL;
	STR_OBJECTS_DETECTED* pstDet = NULL;
	unsigned int i = 0, j = 0, k = 0, uiPos = 0, uiLastPos = 0,  uiNum = 0;
	unsigned short usTmpW = 0, usTmpH = 0;
	STR_POS_INFO stTemp;
	short sX = 0, sY = 0, sFit = 0, m = 0;

	pstTrak = pstTrakObj;
	pstDet = pstDetObj;

	uiPos = pstTrak->uiPointNum & 0x7f;
	uiLastPos = (pstTrak->uiPointNum - 1) & 0x7f; 
	uiNum = pstDet->ucNumber;
	sX = pstTrak->astPoint[uiLastPos].sX;
	sY = pstTrak->astPoint[uiLastPos].sY;

     if(ucCommand & TK_FUNC_DISTANCE != TK_FUNC_DISTANCE)                                     //distance is not the param
     {
          return 0;
     }

	usTmpW = usMinW;
	usTmpH = usMinH;
	j = 0;
	if(ucCommand == TK_FUNC_DISTANCE)                                                                               //distance is the only param
	{
	     if(uiNum != 0)                                                                                                                            //object found
	     {
	          for(i = 0; i < uiNum; i++) 
		     {
		          if(abs(sX - pstDet->astInfo[i].sX) < usMinW
			 	     && abs(sY - pstDet->astInfo[i].sY) < usMinH
			 	     && (sX - pstDet->astInfo[i].sX)*(sX - pstDet->astInfo[i].sX) + (sY - pstDet->astInfo[i].sY)*(sY - pstDet->astInfo[i].sY)
			 	    < usTmpW*usTmpW + usTmpH*usTmpH )                                                           //all aim object found
			     {
					usTmpW = abs(sX - pstDet->astInfo[i].sX);
					usTmpH = abs(sY - pstDet->astInfo[i].sY);
				     j = i;
					sFit = 1;
			     }
		      }
		      if(sFit != 0)                                                                                                                            //choose the best aim object
		      {
				pstTrak->astPoint[uiPos].sX = pstDet->astInfo[j].sX;
				pstTrak->astPoint[uiPos].sY = pstDet->astInfo[j].sY;
			     //pstTrak->usObjWidth = pstDet->astInfo[j].usWidth;
	               //pstTrak->usObjHeight = pstDet->astInfo[j].usHeight;
	               pstTrak->uiPointNum++;
	               pstTrak->uiCaptureFrame++;
				pstTrak->uiLostFrame = 0;
				m = pstTrak->uiPointNum % TK_LOST_FRAME_LEVEL3;
				pstTrak->aucLost[m] = 0;
				//pstTrak->ucResv[0] = 1;
		      }
		      else                                                                                                                                          //no aim object
		      {
		          pstTrak->astPoint[uiPos].sX = pstTrak->astPoint[uiLastPos].sX;
	               pstTrak->astPoint[uiPos].sY = pstTrak->astPoint[uiLastPos].sY;
		          pstTrak->uiPointNum++;
		          pstTrak->uiLostFrame++;
		          pstTrak->uiCaptureFrame = 0;
				m = pstTrak->uiPointNum % TK_LOST_FRAME_LEVEL3;
				pstTrak->aucLost[m] = 1;
				//  pstTrak->ucResv[0] = 2;
		      }
	     }
	     else                                                                                                                                                 //object not found
	     {
		     pstTrak->astPoint[uiPos].sX = pstTrak->astPoint[uiLastPos].sX;
	          pstTrak->astPoint[uiPos].sY = pstTrak->astPoint[uiLastPos].sY;
		     pstTrak->uiPointNum++;
		     pstTrak->uiLostFrame++;
		     pstTrak->uiCaptureFrame = 0;
			m = pstTrak->uiPointNum % TK_LOST_FRAME_LEVEL3;
			pstTrak->aucLost[m] = 1;
			// pstTrak->ucResv[0] = 3;
	     }
	}
	else                                                                                                                                                       //distance is one of the params
	{
		 for(i = 0; i < uiNum; i++)                                                                                                             //delete objects out of range
		 { 
		      if(abs(sX - pstDet->astInfo[i].sX) >= usMinW && abs(sY - pstDet->astInfo[i].sY) >= usMinH)
		      {
		           for(j = i ; j < uiNum - 1 ; j++)
		           {
		                pstDet->astInfo[j].sX = pstDet->astInfo[j + 1].sX;
					 pstDet->astInfo[j].sY = pstDet->astInfo[j + 1].sY;
					 pstDet->astInfo[j].usWidth = pstDet->astInfo[j + 1].usWidth;
					 pstDet->astInfo[j].usHeight = pstDet->astInfo[j + 1].usHeight;
		           }
				  i --;
				  uiNum --;
		      }
		 }

		 for(i = 1; i < uiNum ; i++)                                                                                                             //exchange order, from small to big
		 {
		      for(j = 0; j < i; j++)
		      {
		           if((sX - pstDet->astInfo[i].sX)*(sX - pstDet->astInfo[i].sX)
				   	+ (sY - pstDet->astInfo[i].sY)*(sY - pstDet->astInfo[i].sY)
				   	< (sX - pstDet->astInfo[j].sX)*(sX - pstDet->astInfo[j].sX)
				   	+ (sY - pstDet->astInfo[j].sY)*(sY - pstDet->astInfo[j].sY))
		           {
		                stTemp.sX = pstDet->astInfo[i].sX;
				      stTemp.sY = pstDet->astInfo[i].sX;
		                stTemp.usWidth = pstDet->astInfo[i].usWidth;
				      stTemp.usHeight = pstDet->astInfo[i].usHeight;
				      for(k = i; k > j; k--)
				      {
				           pstDet->astInfo[k].sX = pstDet->astInfo[k - 1].sX;
						 pstDet->astInfo[k].sY = pstDet->astInfo[k - 1].sY;
						 pstDet->astInfo[k].usWidth = pstDet->astInfo[k - 1].usWidth;
						 pstDet->astInfo[k].usHeight = pstDet->astInfo[k - 1].usHeight;
				      }
					 pstDet->astInfo[j].sX = stTemp.sX;
					 pstDet->astInfo[j].sY = stTemp.sY;
					 pstDet->astInfo[j].usWidth = stTemp.usWidth;
					 pstDet->astInfo[j].usHeight = stTemp.usHeight;

					 break;
		           }
		      }
		 }
	}

	return 0;

}

int TrakingObjectSizeMod(STR_OBJECTS_DETECTED* pstDetObj, STR_OBJECT_TRAKING* pstTrakObj,
	                                     unsigned char ucCommand)
{
     unsigned short  usWidthMin = 0, usWidthMax = 0, usWidth = 0;
	unsigned int i = 0, j = 0, uiLastPos = 0, uiCurPos = 0, m = 0;

	if(ucCommand & TK_FUNC_SIZE != TK_FUNC_SIZE)	                                                              //size is not the param
	{
		 return 0;
	}

	uiLastPos = (pstTrakObj->uiPointNum - 1)& 0x7f;

	usWidth = (pstTrakObj->astPoint[uiLastPos].usWidth >> 2);
	usWidth = usWidth >=1 ? 1: usWidth;
	usWidthMin =  pstTrakObj->astPoint[uiLastPos].usWidth - usWidth;
	usWidthMax =  pstTrakObj->astPoint[uiLastPos].usWidth + usWidth;

     for( i = 0; i < pstDetObj->ucNumber; i++)
     {
          if(pstDetObj->astInfo[i].usWidth > usWidthMax || pstDetObj->astInfo[i].usWidth < usWidthMin)
          {
               for(j = i; j < pstDetObj->ucNumber - 1; j++)
               {
                    pstDetObj->astInfo[j].usWidth = pstDetObj->astInfo[j + 1].usWidth;
				pstDetObj->astInfo[j].usHeight = pstDetObj->astInfo[j + 1].usHeight;
				pstDetObj->astInfo[j].sX = pstDetObj->astInfo[j + 1].sX;
				pstDetObj->astInfo[j].sY = pstDetObj->astInfo[j + 1].sY;
               }
			pstDetObj->ucNumber--;
          }
     }
	if((ucCommand >> 2) == 0)                                                                                                                //size is the last param
	{
	     if(pstDetObj->ucNumber > 0)
	     {
	          pstTrakObj->astPoint[uiCurPos].sX = pstDetObj->astInfo[0].sX;
			pstTrakObj->astPoint[uiCurPos].sY = pstDetObj->astInfo[0].sY;
			pstTrakObj->astPoint[uiCurPos].usWidth = pstDetObj->astInfo[0].usWidth;
			pstTrakObj->astPoint[uiCurPos].usHeight = pstDetObj->astInfo[0].usHeight;
			pstTrakObj->uiPointNum++;
			pstTrakObj->uiLostFrame = 0;	
			pstTrakObj->uiCaptureFrame++;
			m = pstTrakObj->uiPointNum % TK_LOST_FRAME_LEVEL3;
			pstTrakObj->aucLost[m] = 0;
	     }
		else
		 {
	          pstTrakObj->astPoint[uiCurPos].sX = pstTrakObj->astPoint[uiLastPos].sX;
			pstTrakObj->astPoint[uiCurPos].sY = pstTrakObj->astPoint[uiLastPos].sY;
			pstTrakObj->astPoint[uiCurPos].usWidth = pstTrakObj->astPoint[uiLastPos].usWidth;
			pstTrakObj->astPoint[uiCurPos].usHeight = pstTrakObj->astPoint[uiLastPos].usHeight;
			pstTrakObj->uiPointNum++;
			pstTrakObj->uiLostFrame++;	
			pstTrakObj->uiCaptureFrame = 0;
			m = pstTrakObj->uiPointNum % TK_LOST_FRAME_LEVEL3;
			pstTrakObj->aucLost[m] = 1;
		 }
	}

	return 0;

}

int TrakingObjectJudgeMod(STR_OBJECTS_DETECTED* pstDetObj, STR_OBJECT_TRAKING* pstTrakObj)
{
     unsigned int i = 0, j = 0;
	 unsigned int nLostThred=0;
	 if(pstDetObj->nCamMoveFlag == 1)
	 {
		 nLostThred=TK_LOST_FRAME_LEVEL2;

	 }
	 else
	 {
		 nLostThred=TK_LOST_FRAME_LEVEL3;
	 
	 }
	if(pstTrakObj->uiLostFrame > nLostThred)                                                                  //object lost
	{
		   memset(pstTrakObj, 0, sizeof(STR_OBJECT_TRAKING));
		   return 0;
	}
//连着丢了就丢了，不做间隔
#if 0
	if(pstTrakObj->uiPointNum >= 128)
	{
	    for(i = 0; i < 128; i++)
	    	{
	    	    if(pstTrakObj->aucLost[i] == 1)
	    	    	{
	    	    	    j++;
	    	    	}
	    	}
		if(j > 96)
		{
		      memset(pstTrakObj, 0, sizeof(STR_OBJECT_TRAKING));
		      return 0;
		}
	}
#endif
	
	if(pstTrakObj->uiPointNum == TK_MIN_ONTRAKING_FRAMES )                                                     //found object in first 3 frames, start to traking
	{
	     //if(pstTrakObj->uiCaptureFrame == pstTrakObj->uiPointNum)
		//{
		    pstTrakObj->ucState = 2;
	    // }
		// else
		// {
		// 	memset(pstTrakObj, 0, sizeof(STR_OBJECT_TRAKING));
		//     return 0;
		// }
	}

	 return pstTrakObj->ucState;

}


int TrakingObjectInputCheck(STR_OBJECTS_DETECTED* pstDetObj, unsigned short usWidth,unsigned short usHeight)
{
     int i = 0;
	if(pstDetObj->ucNumber > 100)                                                                               //input check
	{
		return 100;                                                                                                               //err return
	}	

	for(i = 0; i < pstDetObj->ucNumber; i++)
	{
	      if(pstDetObj->astInfo[i].sX < 0 || pstDetObj->astInfo[i].sX > usWidth
			 	|| pstDetObj->astInfo[i].sY < 0 || pstDetObj->astInfo[i].sY> usHeight) 
	      {
	      	    return 101;
	      }
	}
	return 0;                                                                                                                           //return ok
}


int TrakingObjectMatching(STR_OBJECTS_DETECTED* pstDetObj, STR_OBJECT_TRAKING* pstTrakObj,
	                                                        unsigned short usPicWidth, unsigned short usPicHeight, unsigned char ucCommand)
{
	STR_OBJECT_TRAKING *pstTrak = NULL;
	STR_OBJECTS_DETECTED* pstDet = NULL;
	unsigned int i = 0, j = 0, uiPos = 0, uiLastPos = 0,  uiNum = 0;
	unsigned short usMinW = 0, usMinH = 0;
	int iRet = 0;
	pstTrak = pstTrakObj;
	pstDet = pstDetObj ;
	uiNum = pstDet->ucNumber;

	iRet = TrakingObjectInputCheck(pstDet, usPicWidth, usPicHeight);
	if(iRet != 0)
	{
	    return iRet;
	}

	if(pstTrak->uiLostFrame < TK_LOST_FRAME_LEVEL1)                                                                   //choose param
	{
			usMinW = usPicWidth ;/// TK_MAX_DISTANCE_SCALE_LEVEL1;//yj mod距离上次目标远近范围变大
	          usMinH = usPicHeight / TK_MAX_DISTANCE_SCALE_LEVEL1;
	}
	else
	{
			usMinW = usPicWidth;// / TK_MAX_DISTANCE_SCALE_LEVEL2;//yj mod距离上次目标远近范围变大
	          usMinH = usPicHeight / TK_MAX_DISTANCE_SCALE_LEVEL2;
	}

	if(TK_FUNC_DISTANCE & ucCommand)
	{
		 TrakingObjectDistanceMod(pstDet, pstTrak, ucCommand, usMinW, usMinH);
	}
	
	if(TK_FUNC_SIZE & ucCommand)
	{
		TrakingObjectSizeMod(pstDet, pstTrak, ucCommand);
	}

	iRet = TrakingObjectJudgeMod(pstDet, pstTrak);

	return iRet;

}


int TrakingObjectProcess(unsigned short usPicWidth, unsigned short usPicHeight,
	                                            STR_OBJECTS_DETECTED* pstDetObj, STR_OBJECT_TRAKING* pstTrakObj, unsigned char  ucCommand)
{ 
    STR_OBJECT_TRAKING *pstTrak = NULL;
    STR_OBJECTS_DETECTED* pstDet = NULL;
    unsigned int uiDistance = 0, uiMinDistance = 0, uiNearest = 0, i = 0, j = 0;
    int iRet = 0;

    pstTrak = pstTrakObj;
    pstDet = pstDetObj ;

	if(ucCommand == TK_FUNC_ALL_STOP)
	{
	     for(i = 0; i < pstDet->ucNumber; i++)
	     {
	           pstTrak->astPoint[i].sX = pstDet->astInfo[i].sX;
		      pstTrak->astPoint[i].sY = pstDet->astInfo[i].sY;
	     }
		 pstTrak->uiPointNum = i;
		 return i;
	}

    if(pstTrak->uiPointNum == 0)                                                                             //no on traking object, init params
    {
         iRet = TrakingObjectInit(pstDetObj, pstTrakObj, usPicWidth, usPicHeight);
    }
    else
    {
	   iRet = TrakingObjectMatching(pstDetObj, pstTrakObj, usPicWidth, usPicHeight, ucCommand);
    }

	return iRet;

}
