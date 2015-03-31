#ifndef _BackGroundModel_H
#define _BackGroundModel_H

#include <mcfw/src_bios6/utils/utils.h>
#include <mcfw/src_bios6/links_c6xdsp/system/system_priv_c6xdsp.h>
#include <mcfw/interfaces/link_api/algLink.h>


void * BackGroundModel(
	int nWidth,      //处理图像的宽度
	int nHeight, 	//处理图像的高度
	int nModelSize,  //样本个数
	void* (*Allocate)(int size), //注册内存分配函数
	void *Buffer,  //当Allocate为NULL时，直接内存指针
	unsigned int(*SetSeed)() //注册获取随机seed函数
	);


extern int AnalysisSampleFrame(
		void *Param,    //句柄
		unsigned char *pIntensity, //输入图像
		unsigned char *pChroma,
		int nBufWidth,  //buf宽度
		int left,  
		int right, 
		int top, 
		int bottom);

extern int UpdateBackground(
		void *pParam, //句柄
		unsigned char *validArea,
		unsigned char * pIntensity,  //输入图像
		unsigned char *pChroma,
		int nSrcBufWidth,  //buf宽度
		unsigned char *pOutBuf,   //输出buf
		int nOutBufWidth, //输出宽度
		int nRadius,      //sens值
		int nThreshold,  //阀值
		unsigned int frames       //多少帧更新固定变化
		);

//int initialModelData(void *Param, char * pIntensity, int nIndex, int nBufWidth,int left,int right,int top,int bottom);
//int  Classification(void *pParam,  char * pIntensity,int nWidth, int nHeight,int nBufWidth);



#endif

