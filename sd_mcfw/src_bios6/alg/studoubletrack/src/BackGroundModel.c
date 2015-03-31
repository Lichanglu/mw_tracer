
#include "BackGroundModel.h"
#include <stdio.h>

typedef char byte;


typedef struct ParameterClassification
{
	int m_nModelSize;  //采样点的数量，即P点的数量
	int m_nRadius; //检测半径
	int m_nThreshold; //阈值
	int m_nImageSize; 
	int m_nWidth;  //图像的宽
	int m_nHeight; //图像的高
	int m_nRowIndex; //各个块的起始标志位
	int m_nRows;  //没有被MAX_THREAD整除的行，多余的行数
	int m_bModelInitialized;
	int left;
	int	right;
	int top;
	int bottom;
	unsigned char * m_pModel;  //图像数据指针
	unsigned char * m_pIntensity; //分块后图像数据的指针，可以移动
	unsigned short * m_pForeground;
	//char * m_pDifferenceModel;
	void *(*Allocate)(int size);
	unsigned int (*SetSeed)();
}ParameterClassification;

#define false 0
#define true 1

#define SUBSAMPLE_FACTOR 16 //子采样概率  
#define SUBSAMPLE_FACTOR_1 16 //子采样概率  
int c_xoff[9] = {-1,  0,  1, -1, 1, -1, 0, 1, 0};  //x的邻居点  
int c_yoff[9] = {-1,  0,  1, -1, 1, -1, 0, 1, 0};  //y的邻居点  

#define srand(a)  

#if 0
//针对八连通域
int getOffset(int nIndex, int nWidth, int nHeight, int nRandom)
{
	int s_size = nWidth * nHeight;
	int nOffset = -1;
	switch(nRandom)
	{
	case 0:
		if(nIndex - nWidth > 0) 
		{
			nOffset =  nIndex - nWidth;
		}
		break;
	case 1:
		if(nIndex - nWidth + 1 > 0) 
		{
			nOffset = nIndex - nWidth + 1;
		}
		break;
	case 2:
		if(nIndex + 1 <  s_size) 
		{
			nOffset = nIndex + 1;
		}
		break; 
	case 3:
		if(nIndex + nWidth + 1 < s_size) 
		{
			nOffset = nIndex + nWidth + 1;
		}
		break; 
	case 4:
		if(nIndex + nWidth <  s_size)
		{
			nOffset = nIndex + nWidth;
		}
		break; 
	case 5:
		if(nIndex + nWidth -1 < s_size)
		{
			nOffset = nIndex + nWidth - 1;
		}
		break; 
	case 6:
		if(nIndex -1 > 0)
		{
			nOffset = nIndex - 1;
		}
		break; 
	case 7:
		if(nIndex - nWidth - 1 > 0)
		{
			nOffset = nIndex - nWidth - 1;
		}
		break; 
	}
	return nOffset;
}


int  Classification(void *pParam,  char * pIntensity,int nWidth, int nHeight,int nBufWidth)
{
	ParameterClassification *param = (ParameterClassification *)pParam;
	
	//int nWidth = param->m_nWidth; 
	//int nHeight = param->m_nHeight;
	int nRadius = param->m_nRadius;
	int nThreshold = param->m_nThreshold;
	int	nModelSize = param->m_nModelSize;
	char * pModel		= param->m_pModel; 
	int nImageSize = nWidth * nHeight;  
	int nDifference = 0; 
	int nOffset = 0;
	int nIntensityModel, nIntensityFrame; 
	int n;
	int i,j;

	int left   = param->left;
	int right  = param->right;
	int top    = param->top;
	int bottom = param->bottom;
	static int jjj = 0;

	if(nWidth != param->m_nWidth || nHeight != param->m_nHeight || nBufWidth > 1920)
	{
		return 0;
	}

	if(true!= param->m_bModelInitialized)
	{
		return 0;	
	}

	
	#if 0
	
	for(i = top; i < bottom; i++)
	{
		for(j = left; j < right; j++)
		{
			nOffset =  jjj * nImageSize +  i*nWidth + j;
			pIntensity[i*nBufWidth + j] = pModel[nOffset];
			
		}
	}
	jjj++;
	if(3 == jjj)
	{
		jjj = 0;
	}
	
	return 0;
#endif

	Vps_printf("------------>%d %d %d %d %d\n",left,right,top,bottom,nModelSize);

	for(i = top; i < bottom; i++)
	{
		for(j = left; j < right; j++)
		{
			int nCount = 0; 
			int bUpdate = false;  
			// set to be foreground
			//pForeground[n] = pIntensity[n];
			for(n = 0; n < nModelSize; n++)
			{
				// offset for the same pixel of the model 
				nOffset =  n * nImageSize +  i*nWidth + j;
				nIntensityModel = pModel[nOffset];
				nIntensityFrame = pIntensity[i*nBufWidth + j];
				nDifference =  abs(nIntensityModel - nIntensityFrame);
				//if(nDifference <= nRadius)
				if(nDifference < 30)
				{
					nCount++; 
				}
				// classfiy as background
				if(nCount >= nThreshold)
				{
					//pForeground[n] = 0;
					bUpdate = true;
					break; 
				}	
			}

			// 分别在时间域和空间域 update a backround pixel; 
			if(bUpdate)
			{	
				#if 1
				//initialize random seed: 
				srand (Utils_getCurTimeInMsec());
				 //generate randaom number: 
				int nIndex  = rand() % nModelSize;

				// update randomly in time sequence
				nOffset =  nIndex * nImageSize + i*nWidth + j;;
				pModel[nOffset] = nIntensityFrame;
				srand (Utils_getCurTimeInMsec());
				nIndex  = rand() % 8;

				// update randomly in spatially 
				nOffset = getOffset(nOffset, nWidth, nHeight, nIndex); 
				if(nOffset > 0)
				{
					pModel[nOffset] = nIntensityFrame;
				}
				#endif

				//pIntensity[i*nBufWidth + j] = 0;
				//pIntensity[i*nBufWidth + j] = 0;
			}
			else
			{
				pIntensity[i*nBufWidth + j] = 0;
			}
		}
	}

	return 0;
}

//采集样本
int initialModelData(void *Param, char * pIntensity, int nIndex, int nBufWidth,int left,int right,int top,int bottom)
{
    ParameterClassification *param = (ParameterClassification *)Param;
	int i = 0;
    if(NULL == param)
    {
        return 0;
    }

    // input validation 
    if(!pIntensity)
    {
        return 0;
    }

    if(nBufWidth > 1920)
    {
        return 0;
    }

        
	if(nIndex >= param->m_nModelSize || nIndex <0) 
	{
		return 0;
	}

	#if 0
    if(param->m_bModelInitialized)
    {
        return 0;
    }
	#endif

	param->left = left;
	param->top = top;
	param->bottom = bottom;
	param->right = right;


	Vps_printf("initialModelData index %d nBufWidth %d\n",nIndex,nBufWidth);
		
	//nOffset是一个偏移量，将连续几张图片的数据读入m_pModel
	int nOffset = param->m_nImageSize * nIndex; 

	//由src指向地址为起始地址的连续n个字节的数据复制到以dest指向地址为起始地址的空间内
	//void *memcpy(void *dest, const void *src, size_t n);

	for(i = 0; i< param->m_nHeight; i++)
	{
		memcpy(param->m_pModel + nOffset + i*param->m_nWidth, pIntensity + i*nBufWidth, param->m_nWidth);
	}

	
	//memcpy(param->m_pModel + nOffset, pIntensity, param->m_nImageSize);

	// check if the model intializaed or not 
	if(nIndex == param->m_nModelSize - 1)
	{
		param->m_bModelInitialized = true;
	}

    return 1;
}
#endif

int UpdateBackground(void *pParam, unsigned char *validArea,unsigned char * pIntensity, unsigned char *pChroma, int nSrcBufWidth, unsigned char *pOutBuf, int nOutBufWidth, int nRadius, int nThreshold, unsigned int frames)  
{  
	ParameterClassification *param = (ParameterClassification *)pParam;	
	if(NULL == pParam || NULL == pIntensity || nSrcBufWidth > 1920)
	{
		return 0;
	}
	
	if(true!= param->m_bModelInitialized)
	{
		return 0;	
	}

	int nDifferenceY,nDifferenceU,nDifferenceV; 
	int nOffset = 0;
	int nIntensityModel, nIntensityFrame,nChromaUModel,nChromaVModel,nChromaUFrame,nChromaVFrame; 
	int i,j,n;
	int random = 0;
	int nModelBufYOffset,nSrcBufYOffset,nOutBufOffset,nModelChromaOffset,nSrcBufChromaOffset;

	srand (param->SetSeed());

//	Vps_printf("%d %d %d %d\n",param->top,param->bottom,param->left,param->right);
	for(i = param->top; i < param->bottom; i++)
	{
		for(j = param->left; j < param->right; j++)
		{
			if (*((uint8_t *)validArea + i*nOutBufWidth+ j) == 0)
			{
				continue;
			}
			int nCount = 0; 
			int nSensCount = 0; 
			int bUpdate = false;  
			int bSensUpdate = false;  

			//样本图像Y分量偏移
			nModelBufYOffset   = i*param->m_nWidth + j;
			//样本图像UV分量偏移
			nModelChromaOffset = param->m_nWidth*param->m_nHeight +  i/2*param->m_nWidth + j/2*2;

			//源图像Y分量偏移
			nSrcBufYOffset = i*nSrcBufWidth + j;
			//源图像UV分量偏移
			nSrcBufChromaOffset = i/2*nSrcBufWidth + j/2*2;

			//输出图像偏移
			nOutBufOffset   = i*nOutBufWidth + j;

			//实时Y分量值
			nIntensityFrame = pIntensity[nSrcBufYOffset];
			//实时U分量值
			nChromaUFrame	= pChroma[nSrcBufChromaOffset];
			//实时V分量值
			nChromaVFrame	= pChroma[nSrcBufChromaOffset+1];
			
			for(n = 0; n < param->m_nModelSize; n++)
			{
				nIntensityModel = param->m_pModel[n * param->m_nImageSize +  nModelBufYOffset];
				nChromaUModel   = param->m_pModel[n * param->m_nImageSize +  nModelChromaOffset];
				nChromaVModel	= param->m_pModel[n * param->m_nImageSize +  nModelChromaOffset + 1];
				
				nDifferenceY =  abs(nIntensityModel - nIntensityFrame);
				nDifferenceU =  abs(nChromaUModel - nChromaUFrame);
				nDifferenceV =  abs(nChromaVModel - nChromaVFrame);

				//yuv变化都不大.则可能是背景点
				//if((nDifferenceY <= nRadius) && (nDifferenceU <= nRadius) && (nDifferenceV <= nRadius))
				if((nDifferenceY <= 20) && (nDifferenceU <= 20) && (nDifferenceV <= 20))
				//if(nDifferenceY <= 20)
				
				{
					nCount++; 
				}

				if((nDifferenceY >= nRadius) || (nDifferenceU >= nRadius) || (nDifferenceV >= nRadius))
				//if(nDifferenceY >= nRadius)
				{
					nSensCount++; 
				}

				//满足nThreshold组，则认为是背景点
				if(nCount >= nThreshold)
				{
					bUpdate = true;
					//break; 
				}	

				if(nSensCount >= 5)
				{
					bSensUpdate = true;
					//break; 
				}	
			}


			if(bUpdate)
			{	
				param->m_pForeground[nModelBufYOffset] = 0;

				random  = rand() % SUBSAMPLE_FACTOR;
				if(0 == random )
				{
					random  = rand() % param->m_nModelSize;
					param->m_pModel[random*param->m_nImageSize + nModelBufYOffset] = nIntensityFrame;	
					param->m_pModel[random*param->m_nImageSize + nModelChromaOffset] = nChromaUFrame;
					param->m_pModel[random*param->m_nImageSize + nModelChromaOffset+1] = nChromaVFrame;
				}
				
				random  = rand() % SUBSAMPLE_FACTOR_1;
				if(0 == random)
				{
					int row, col;
					random  = rand() % 9;

					 row = i + c_yoff[random];  
					 if (row < 0)   
				   		row = 0;  

					if (row >= param->bottom)  
				    	row = param->bottom - 1;  

					random  = rand() % 9;
					col = j + c_xoff[random];  
					if (col < 0)   
				   		col = 0;  

					if (col >= param->right)  
				    	col = param->right - 1;  

					random  = rand() % param->m_nModelSize;
					//param->m_pModel[random*param->m_nImageSize + row*param->m_nWidth + col] = nIntensityFrame;
					//param->m_pModel[random*param->m_nImageSize + param->m_nWidth*param->m_nHeight +  row/2*param->m_nWidth + col/2*2]        = nChromaUFrame;
					//param->m_pModel[random*param->m_nImageSize + param->m_nWidth*param->m_nHeight +  row/2*param->m_nWidth + col/2*2 + 1] 	 = nChromaVFrame;

					param->m_pModel[random*param->m_nImageSize + row*param->m_nWidth + col] = pIntensity[row*nSrcBufWidth + col];
					param->m_pModel[random*param->m_nImageSize + param->m_nWidth*param->m_nHeight +  row/2*param->m_nWidth + col/2*2]        = pChroma[row/2*nSrcBufWidth + col/2*2];
					param->m_pModel[random*param->m_nImageSize + param->m_nWidth*param->m_nHeight +  row/2*param->m_nWidth + col/2*2 + 1] 	 = pChroma[row/2*nSrcBufWidth + col/2*2 + 1];
				}

			
				if(NULL != pOutBuf)
				{
					pOutBuf[nOutBufOffset] = 0;
				}

			}
			else
			{
				param->m_pForeground[nModelBufYOffset]++;
				if (param->m_pForeground[nModelBufYOffset] > frames && frames) 
				{
					//random  = rand() % SUBSAMPLE_FACTOR;
					//random  = rand() % 4;
					//if(0 == random)
					{
						random  = rand() % param->m_nModelSize;
						param->m_pModel[random*param->m_nImageSize + nModelBufYOffset]     = nIntensityFrame;
						param->m_pModel[random*param->m_nImageSize + nModelChromaOffset]   = nChromaUFrame;
						param->m_pModel[random*param->m_nImageSize + nModelChromaOffset+1] = nChromaVFrame;
					}
				}

				if(bSensUpdate)
				{
		
					//pOutBuf[nOutBufOffset] = 1;

					if(NULL != pOutBuf)
					{
						pOutBuf[nOutBufOffset] = 1;
						pIntensity[nSrcBufYOffset] = 255;
						pChroma[nSrcBufChromaOffset] = 0;
						pChroma[nSrcBufChromaOffset + 1] = 255;

					}
					
				}
				else
				{
					pOutBuf[nOutBufOffset] = 0;
				}
			}

		
		}
	}

	return 0;
} 




int AnalysisSampleFrame(void *Param, unsigned char *pIntensity, unsigned char *pChroma,int nBufWidth, int left, int right, int top, int bottom)  
{  
    ParameterClassification *param = (ParameterClassification *)Param;
	int i,j,k;
	int row, col,nOffset;  
    if(NULL == pIntensity || NULL == param || nBufWidth > 1920)
    {
        return 0;
    }
 

	param->left   = left;
	param->top    = top;
	param->bottom = bottom;
	param->right  = right; 

	
	for(i = top; i < bottom; i++)
	{
		for(j = left; j < right; j++)
		{
			for( k = 0 ; k < param->m_nModelSize; k++)  
			{
				//srand ((unsigned int) time(NULL) );
				srand (Utils_getCurTimeInMsec());
				int random  = rand() % 9;
				row = i + c_yoff[random];  
				if (row < 0)   
			   		row = 0;  

				if (row >= bottom)  
			    	row = bottom - 1;  

				random  = rand() % 9;
				col = j + c_xoff[random];  
				if (col < 0)   
			   		col = 0;  

				if (col >= right)  
			    	col = right - 1;  

				//载入Y分量
				//param->m_pModel[param->m_nImageSize*k + i*param->m_nWidth + j] = pIntensity[row*nBufWidth + col];
			
				//载入U分量
				//param->m_pModel[param->m_nImageSize*k + param->m_nWidth*param->m_nHeight + i/2*param->m_nWidth + j/2*2] = pChroma[row/2*nBufWidth + col/2*2];

				//载入V分量
				//param->m_pModel[param->m_nImageSize*k + param->m_nWidth*param->m_nHeight + i/2*param->m_nWidth + j/2*2 + 1] = pChroma[row/2*nBufWidth + col/2*2 + 1];

				//载入Y分量
				param->m_pModel[param->m_nImageSize*k + i*param->m_nWidth + j] = pIntensity[i*nBufWidth + j];
			
				//载入U分量
				param->m_pModel[param->m_nImageSize*k + param->m_nWidth*param->m_nHeight + i/2*param->m_nWidth + j/2*2] = pChroma[i/2*nBufWidth + j/2*2];

				//载入V分量
				param->m_pModel[param->m_nImageSize*k + param->m_nWidth*param->m_nHeight + i/2*param->m_nWidth + j/2*2 + 1] = pChroma[i/2*nBufWidth + j/2*2 + 1];
			
			

			}
			
		}
	}

	param->m_bModelInitialized = true;

	Vps_printf("AnalysisSampleFrame ok [%d %d %d %d]\n",left,right,top,bottom);
	return 1;
}  

int FirstAnalysisSampleFrame(void *Param, unsigned char *pIntensity, unsigned char *pChroma,int nBufWidth, int left, int right, int top, int bottom)  
{  
    ParameterClassification *param = (ParameterClassification *)Param;
	int i,j,k;
	int row, col,nOffset;  
    if(NULL == pIntensity || NULL == param || nBufWidth > 1920)
    {
        return 0;
    }

	param->left   = left;
	param->top    = top;
	param->bottom = bottom;
	param->right  = right; 

	for(i = top; i < bottom; i++)
	{
		for(j = left; j < right; j++)
		{
			for( k = 0 ; k < param->m_nModelSize; k++)  
			{
				//srand ((unsigned int) time(NULL) );
				srand (Utils_getCurTimeInMsec());
				int random  = rand() % 9;
				row = i + c_yoff[random];  
				if (row < 0)   
			   		row = 0;  

				if (row >= bottom)  
			    	row = bottom - 1;  

				random  = rand() % 9;
				col = j + c_xoff[random];  
				if (col < 0)   
			   		col = 0;  

				if (col >= right)  
			    	col = right - 1;  

				//载入Y分量
				//param->m_pModel[param->m_nImageSize*k + i*param->m_nWidth + j] = pIntensity[(row - top)*nBufWidth + col-left];
			
				//载入U分量
				//param->m_pModel[param->m_nImageSize*k + param->m_nWidth*param->m_nHeight + i/2*param->m_nWidth + j/2*2] = pChroma[(row-top)*nBufWidth + (col - left)/2*2];

				//载入V分量
				//param->m_pModel[param->m_nImageSize*k + param->m_nWidth*param->m_nHeight + i/2*param->m_nWidth + j/2*2 + 1] = pChroma[(row-top)*nBufWidth  + (col - left)/2*2 + 1];
			
				param->m_pModel[param->m_nImageSize*k + i*param->m_nWidth + j] = pIntensity[(i - top)*nBufWidth + j-left];
			
				//载入U分量
				param->m_pModel[param->m_nImageSize*k + param->m_nWidth*param->m_nHeight + i/2*param->m_nWidth + j/2*2] = pChroma[(i-top)/2*nBufWidth + (j - left)/2*2];

				//载入V分量
				param->m_pModel[param->m_nImageSize*k + param->m_nWidth*param->m_nHeight + i/2*param->m_nWidth + j/2*2 + 1] = pChroma[(i-top)/2*nBufWidth  + (j - left)/2*2 + 1];
			}
			
		}
	}

	param->m_bModelInitialized = true;

	Vps_printf("FirstAnalysisSampleFrame ok [%d %d %d %d]\n",left,right,top,bottom);
	return 1;
}  

//初始化
void * BackGroundModel(
	int nWidth, 
	int nHeight, 
	int nModelSize,
	void* (*Allocate)(int size),void *Buffer,unsigned int(*SetSeed)())
{

	ParameterClassification *param = NULL;
	if((Allocate == NULL) && (Buffer == NULL) && (NULL == SetSeed))
	{
		//Vps_printf("Allocate is Null!!! \n");
		return NULL;
	}
	
	if(nWidth <= 0 || nHeight <= 0 || nModelSize <= 0)
	{
		//Vps_printf("nWidth=%d nHeight=%d Don't 0!!\n",nWidth,nHeight);
		return NULL;
	}

	if(Allocate != NULL)
	{
		param = (ParameterClassification *)Allocate(sizeof(ParameterClassification));
	    if(NULL == param)
	    {
			return NULL;
	    }
	}
	else
	{
		param = (ParameterClassification *)Buffer;
	}
	
	param->m_nWidth      = nWidth; 
	param->m_nHeight     = nHeight;
	param->m_nModelSize  = nModelSize; 
	//param->m_nRadius     = nRadius;
	//param->m_nThreshold  = nThreshold;
	param->m_nImageSize    = nWidth*nHeight + nWidth*nHeight;
	param->m_bModelInitialized = false;


	if(Allocate != NULL)
	{
		param->m_pModel  = (char *)Allocate(param->m_nImageSize*(nModelSize + 2));
	    if(NULL == param->m_pModel)
	    {
	        return NULL;
	    }
		memset(param->m_pModel,0x0,param->m_nImageSize *(nModelSize + 2));
	}
	else
	{
		param = (ParameterClassification *)Buffer;
		param->m_pModel      = (char *)Buffer + sizeof(ParameterClassification);

		memset(param->m_pModel,0x0,param->m_nImageSize *(nModelSize + 2));
	}


	param->m_pForeground = param->m_pModel + param->m_nImageSize * param->m_nModelSize;

	param->SetSeed = SetSeed;

	Vps_printf("create ok\n");
    return (void *)param;

}

