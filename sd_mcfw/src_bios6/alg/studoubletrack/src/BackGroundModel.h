#ifndef _BackGroundModel_H
#define _BackGroundModel_H

#include <mcfw/src_bios6/utils/utils.h>
#include <mcfw/src_bios6/links_c6xdsp/system/system_priv_c6xdsp.h>
#include <mcfw/interfaces/link_api/algLink.h>


void * BackGroundModel(
	int nWidth,      //����ͼ��Ŀ��
	int nHeight, 	//����ͼ��ĸ߶�
	int nModelSize,  //��������
	void* (*Allocate)(int size), //ע���ڴ���亯��
	void *Buffer,  //��AllocateΪNULLʱ��ֱ���ڴ�ָ��
	unsigned int(*SetSeed)() //ע���ȡ���seed����
	);


extern int AnalysisSampleFrame(
		void *Param,    //���
		unsigned char *pIntensity, //����ͼ��
		unsigned char *pChroma,
		int nBufWidth,  //buf���
		int left,  
		int right, 
		int top, 
		int bottom);

extern int UpdateBackground(
		void *pParam, //���
		unsigned char *validArea,
		unsigned char * pIntensity,  //����ͼ��
		unsigned char *pChroma,
		int nSrcBufWidth,  //buf���
		unsigned char *pOutBuf,   //���buf
		int nOutBufWidth, //������
		int nRadius,      //sensֵ
		int nThreshold,  //��ֵ
		unsigned int frames       //����֡���¹̶��仯
		);

//int initialModelData(void *Param, char * pIntensity, int nIndex, int nBufWidth,int left,int right,int top,int bottom);
//int  Classification(void *pParam,  char * pIntensity,int nWidth, int nHeight,int nBufWidth);



#endif

