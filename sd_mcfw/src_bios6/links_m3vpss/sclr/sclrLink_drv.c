/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include "sclrLink_priv.h"

// #define SCLR_LINK_QUEUE_REQ
static
Int32 SclrLink_drvDmaCreate(SclrLink_Obj * pObj)
{
    Int32 status;

    status = Utils_dmaCreateCh(&pObj->dmaObj,
                               UTILS_DMA_DEFAULT_EVENT_Q,
                               1, TRUE);

    UTILS_assert(status==FVID2_SOK);

    return status;
}
static
Int32 SclrLink_drvDmaDelete(SclrLink_Obj * pObj)
{
    UInt32 status;

    status = Utils_dmaDeleteCh(&pObj->dmaObj);
    UTILS_assert(status==FVID2_SOK);

    return status;
}
static
Int32 SclrLink_drvCheckBlankOutputBuffer(SclrLink_Obj * pObj, UInt32 chId, UInt32 *Width, UInt32 *Height)
{
    Int32 status = 0;
    Utils_DmaFill2D blankFrameInfo;
    UInt32 pitch;
	UInt32 outNumFrames = 0;
	UInt32 i = 0;
	UInt32 StartNum = 0;
	FVID2_Frame *pFrame = NULL;
	UInt32 numTx = 0;

	memset(&blankFrameInfo,0,sizeof(Utils_DmaFill2D));

	outNumFrames = pObj->outObj.outNumFrames/pObj->inQueInfo.numCh;
	StartNum	 = chId*outNumFrames;
	pitch        = 	(*Width)*2;
//	Vps_printf("1Width=%d,heigh=%d,pitch=%d,outNumFrames=%d\n",*Width,*Height,pitch,outNumFrames);

//	Vps_printf("3Width=%d,heigh=%d,pitch=%d\n",blankFrameInfo.width,blankFrameInfo.height,blankFrameInfo.destPitch[0]);

	for(i = 0; i < outNumFrames; i++)
	{
		numTx = 0;
		pFrame = &pObj->outObj.outFrames[StartNum++];
		memset(&(blankFrameInfo),0,sizeof(Utils_DmaFill2D));

		blankFrameInfo.destAddr[0]  = pFrame->addr[0][0];
        blankFrameInfo.destPitch[0] = pitch;
        blankFrameInfo.dataFormat   = FVID2_DF_YUV422I_YUYV;
        blankFrameInfo.width  = *Width ;
        blankFrameInfo.height = *Height;
        blankFrameInfo.fillColorYUYV =
                UTILS_DMA_GENERATE_FILL_PATTERN(0x00,
                                                0x80,
                                                0x80);
        blankFrameInfo.startX = 0;
        blankFrameInfo.startY = 0;
		numTx++;
#if 1
//	Vps_printf("2Width=%d,heigh=%d,pitch=%d,numTx=%d,pFrame=%p\n",*Width,*Height,pitch,numTx,pFrame);

    Vps_printf("SCLR: Start Blanking of output buffer: "
               "BufAddr: Width:[%d] Height:[%d] Pitch:[%d] Pixel:[0x%X]",
            //   blankFrameInfo[numTx].destAddr[0],
               blankFrameInfo.width,
               blankFrameInfo.height,
               blankFrameInfo.destPitch[0],
               blankFrameInfo.fillColorYUYV);
//	Vps_printf("3Width=%d,heigh=%d,pitch=%d\n",blankFrameInfo.width,blankFrameInfo.height,blankFrameInfo.destPitch[0]);

#endif
		status = Utils_dmaFill2D(&pObj->dmaObj, &(blankFrameInfo), numTx);

	    if (UTILS_ISERROR(status))
	    {
	        Vps_printf("SWMS: Utils_dmaFill2D for output buffer failed!!");
	    }
	    else
	    {
	        #if 1
	            Vps_printf("%d:SCLR: End Blanking of output buffer: "
	                   "BufAddr:[%p] ",
	                   Utils_getCurTimeInMsec(),
	                   blankFrameInfo.destAddr[0]);
	        #endif
	    }

	}

    return status;
}


UInt32 SclrLink_CalculationSclrMode(SclrLink_CalSclrMode CalSclrMode, UInt32 *Width, UInt32 *Height)
{
	Float  SrcRatio  = 0;
	Float  DstRatio  = 0;
	UInt32 SclrMode  = SCLR_SCALE_MODE_FULLSCREEN;
	UInt32 outOffset = 0;
	const double EPSILON = 1.00e-07;
	Float tmpWidth,tmpHeight,tmpdifference;

	if((CalSclrMode.SrcWidth <= 0) || (CalSclrMode.SrcHeight <= 0)\
		|| (CalSclrMode.DstWidth <= 0) || (CalSclrMode.DstHeight <= 0))
	{
		UTILS_assert(0);
	}

	tmpWidth  = (Float)CalSclrMode.SrcWidth;
	tmpHeight = (Float)CalSclrMode.SrcHeight;
	SrcRatio = tmpWidth/tmpHeight;
	tmpWidth  = (Float)CalSclrMode.DstWidth;
	tmpHeight  = (Float)CalSclrMode.DstHeight;
	DstRatio = tmpWidth/tmpHeight;

	tmpdifference = SrcRatio - DstRatio;
	Vps_printf("%f %f %f\n",SrcRatio, DstRatio,tmpdifference);

	if((tmpdifference <= EPSILON) && (tmpdifference >= -EPSILON))
	{
		SclrMode = SCLR_SCALE_MODE_FULLSCREEN;
		*Width  =  CalSclrMode.DstWidth;
		*Height = CalSclrMode.DstHeight;
	}
	else if(tmpdifference > EPSILON)
	{
		SclrMode = SCLR_SCALE_MODE_WIDTH;
		*Height = CalSclrMode.DstWidth*CalSclrMode.SrcHeight/CalSclrMode.SrcWidth;
		*Height = VpsUtils_floor(*Height, 2);
		*Width  = VpsUtils_floor(CalSclrMode.DstWidth, 2);
		outOffset = (CalSclrMode.DstHeight - *Height)/2*CalSclrMode.Dstpitch[0];

	}
	else if(tmpdifference < -EPSILON)
	{
		SclrMode = SCLR_SCALE_MODE_HEIGHT;
		*Width  = CalSclrMode.DstHeight*CalSclrMode.SrcWidth/CalSclrMode.SrcHeight;
		*Width  = VpsUtils_floor(*Width, 2);
		*Height = VpsUtils_floor(CalSclrMode.DstHeight, 2);

		outOffset = (CalSclrMode.DstWidth- *Width)/2*2;
	}
	else
	{
		SclrMode = SCLR_SCALE_MODE_MAXMODE;
		//....
	}

	Vps_printf("SclrLink_CalculationSclrMode: Mode[%d] Width = %d Height = %d dstWidth = %d dstHeight = %d srcWidth = %d srcHeight = %d %d\n",\
		SclrMode,*Width,*Height,CalSclrMode.DstWidth,CalSclrMode.DstHeight,CalSclrMode.SrcWidth,CalSclrMode.SrcHeight,outOffset);

	return outOffset;
}


Int32 SclrLink_resetStatistics(SclrLink_Obj * pObj)
{
	UInt32 chId;
	SclrLink_ChObj *pChObj;

	for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
	{
		pChObj = &pObj->chObj[chId];

		pChObj->inFrameRecvCount = 0;
		pChObj->inFrameRejectCount = 0;
		pChObj->inFrameProcessCount = 0;

		pChObj->outFrameUserSkipCount = 0;
		pChObj->outFrameSkipCount = 0;
		pChObj->outFrameCount = 0;

		pChObj->minLatency = 0xFF;
		pChObj->maxLatency = 0;
	}

	pObj->statsStartTime = Utils_getCurTimeInMsec();

	return 0;
}


/* Add ScaleMode Full screen and  proportion sclr */
Int32 SclrLink_drvSetScaleMode(SclrLink_Obj * pObj, SclrLink_SclrMode *pSclrMode)
{

	SclrLink_chDynamicSetOutRes params[SCLR_LINK_MAX_CH];

	SclrLink_ChObj *pChObj;
	UInt32 chId;
	UInt key;
	Bool SclrMode = pSclrMode->SclrMode;
	chId = pSclrMode->chId;

	if(chId < 0 || chId >= SCLR_LINK_MAX_CH)
	{
		return FVID2_EFAIL;
	}

	key = Hwi_disable();

	pChObj = &pObj->chObj[chId];

	/* 设置等比缩放模式 */
	if((FALSE == pChObj->StretchMod) && (TRUE == SclrMode))
	{
		pChObj->StretchMod = TRUE;
	//	for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
		{
			params[chId].chId   = chId;
			params[chId].width  = pChObj->sclrRtOutFrmPrm.width;
			params[chId].height = pChObj->sclrRtOutFrmPrm.height;
			params[chId].pitch[0] = pChObj->sclrRtOutFrmPrm.pitch[0];
			params[chId].pitch[1] = 0;

			Vps_printf("SclrMode = TRUE CH[%d] outwidth[%d] outheight[%d] pitch[%d]",\
				params[chId].chId,params[chId].width,params[chId].height,params[chId].pitch[0]);

			SclrLink_drvSetChDynamicOutputRes(pObj, &params[chId]);
		}
	}
	else if((TRUE == pChObj->StretchMod) && (FALSE == SclrMode))
	{
		pChObj->StretchMod = FALSE;
		//for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
		{
			params[chId].chId   = chId;
			params[chId].width  = pChObj->outFormat.width;
			params[chId].height = pChObj->outFormat.height;
			params[chId].pitch[0] = pChObj->outFormat.pitch[0];
			params[chId].pitch[1] = 0;

			Vps_printf("SclrMode = TRUE CH[%d] outwidth[%d] outheight[%d] pitch[%d]",\
				params[chId].chId,params[chId].width,params[chId].height,params[chId].pitch[0]);

			SclrLink_drvSetChDynamicOutputRes(pObj, &params[chId]);
		}
	}
	else
	{

	}

	Hwi_restore(key);
	return FVID2_SOK;
}

Int32 SclrLink_drvSetInChInfo(SclrLink_Obj * pObj, System_LinkChInfo2 *pchinfo)
{
	System_LinkChInfo *rtChInfo;
	UInt key;

	key = Hwi_disable();

	if(pchinfo->chid >= 0 && pchinfo->chid < pObj->inQueInfo.numCh){
		pObj->chObj[pchinfo->chid].chRtOutInfoUpdate = TRUE;
		pObj->chObj[pchinfo->chid].chRtInInfoUpdate = TRUE;
		rtChInfo = &pObj->chObj[pchinfo->chid].rtChannelInfo;
		rtChInfo->startX = pchinfo->chinfo.startX;
		rtChInfo->startY = pchinfo->chinfo.startY;
		rtChInfo->width = pchinfo->chinfo.width;
		rtChInfo->height = pchinfo->chinfo.height;
		Vps_printf("[SclrLink_drvSetInChInfo] chid:[%d] chRtInInfoUpdate",
					pchinfo->chid,
					pObj->chObj[pchinfo->chid].chRtInInfoUpdate);
	}

	Hwi_restore(key);
	return FVID2_SOK;
}



Int32 SclrLink_drvPrintStatistics(SclrLink_Obj * pObj, Bool resetAfterPrint)
{
	UInt32 chId;
	SclrLink_ChObj *pChObj;
	UInt32 elaspedTime;

	elaspedTime = Utils_getCurTimeInMsec() - pObj->statsStartTime; // in msecs
	elaspedTime /= 1000; // convert to secs

	Vps_printf( " \n"
	            " *** [%s] SCLR Statistics *** \n"
	            " \n"
	            " Elasped Time           : %d secs\n"
	            " Total Fields Processed : %d \n"
	            " Total Fields FPS       : %d FPS\n"
	            " \n"
	            " \n"
	            " CH  | In Recv In Reject In Process Out Skip Out User Out Latency   \n"
	            " Num | FPS     FPS       FPS        FPS    FPS   Skip FPS Min / Max \n"
	            " ---------------------------------------------------------------------\n",
	            pObj->name,
	            elaspedTime,
	            pObj->getProcessFrameCount,
	            pObj->getProcessFrameCount * 100 / (pObj->totalTime / 10)
	          );

	for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
	{
		pChObj = &pObj->chObj[chId];

		Vps_printf( " %3d | %7d %9d %10d %3d %8d %8d %3d %3d\n",
		            chId,
		            pChObj->inFrameRecvCount/elaspedTime,
		            pChObj->inFrameRejectCount/elaspedTime,
		            pChObj->inFrameProcessCount/elaspedTime,
		            pChObj->outFrameCount/elaspedTime,
		            pChObj->outFrameSkipCount/elaspedTime,
		            pChObj->outFrameUserSkipCount/elaspedTime,
		            pChObj->minLatency,
		            pChObj->maxLatency
		          );

		Utils_bufPrintStatus("sclr in", &pChObj->inQue);
		Utils_bufPrintStatus("sclr out",&pObj->outObj.bufOutQue[chId]);
	}

	Vps_printf( " \n");

	if(resetAfterPrint)
	{
		SclrLink_resetStatistics(pObj);
	}
	return FVID2_SOK;
}

Int32 SclrLink_drvFvidCb(FVID2_Handle handle, Ptr appData, Ptr reserved)
{
	SclrLink_Obj *pObj = (SclrLink_Obj *) appData;

#ifdef SCLR_LINK_QUEUE_REQ
	Utils_tskSendCmd(&pObj->tsk, DEI_LINK_CMD_GET_PROCESSED_DATA);
#else
	Semaphore_post(pObj->complete);
#endif
	return FVID2_SOK;
}

Int32 SclrLink_drvFvidErrCb(FVID2_Handle handle,
                            Ptr appData, Ptr errList, Ptr reserved)
{
	return FVID2_SOK;
}

Int32 SclrLink_drvUpdateRtParams(SclrLink_Obj * pObj, UInt32 chId)
{
	SclrLink_ChObj *pChObj;
	System_LinkChInfo *rtChInfo;
	Vps_FrameParams *rtParam;
	Vps_CropConfig  *cropCfg;

	pChObj = &pObj->chObj[chId];
	rtChInfo = &pChObj->rtChannelInfo;
	rtParam =  &pChObj->sclrRtInFrmPrm;
	cropCfg = &pChObj->scCropConfig;

	cropCfg->cropStartX = VpsUtils_align(rtChInfo->startX, 2);
	cropCfg->cropStartY = VpsUtils_align(rtChInfo->startY, 2);
	rtParam->width = VpsUtils_align(rtChInfo->width, 2);
	rtParam->height = VpsUtils_align(rtChInfo->height, 2);
	rtParam->pitch[0] = rtChInfo->pitch[0];
	rtParam->pitch[1] = rtChInfo->pitch[1];
	rtParam->pitch[2] = rtChInfo->pitch[2];

#ifdef SCLR_LINK_ENABLE_MODIFY_FRAME_POINTER_ALWAYS
	cropCfg->cropStartX = 0;
	cropCfg->cropStartY = 0;
#endif

	cropCfg->cropWidth = rtParam->width;
	cropCfg->cropHeight = rtParam->height;
	rtParam->width += cropCfg->cropStartX;
	rtParam->height += cropCfg->cropStartY;

	if (rtParam->width > SCLR_LINK_MAX_WIDTH_SUPPORTED)
	{
		rtParam->width = cropCfg->cropWidth;
		cropCfg->cropStartX = 0;
	}

	if(pObj->createArgs.enableLineSkipSc && rtParam->memType == VPS_VPDMA_MT_NONTILEDMEM)
	{
		/* half the height and double the pitch possible
		   only when input is non-tiled */
		rtParam->pitch[0] *= 2;
		rtParam->pitch[1] *= 2;
		rtParam->height /= 2;

		/* half the height and crop start Y
		   possible only when input is non-tiled
		 */
		cropCfg->cropStartY /= 2;
		cropCfg->cropHeight /= 2;
	}

	return 0;
}

Int32 SclrLink_drvModifyFramePointer(SclrLink_Obj * pObj,
                                     FVID2_FrameList * inFrameList,
                                     Bool addOffset)
{
	Int32 offset[2],i, bytesPerPixel;
	Vps_FrameParams *rtParam;
	System_LinkChInfo *rtChInfo;
	UInt32 chId;

	for(i = 0; i< inFrameList->numFrames; i++)
	{
		chId = inFrameList->frames[i]->channelNum;

		if (chId < pObj->inQueInfo.numCh)
		{
			rtParam = &pObj->chObj[chId].sclrRtInFrmPrm;
			rtChInfo = &pObj->chObj[chId].rtChannelInfo;

			if(rtParam->dataFormat == SYSTEM_DF_YUV422I_YUYV)
				bytesPerPixel = 2;
			else
				bytesPerPixel = 1;

			offset[0] = rtChInfo->pitch[0]*rtChInfo->startY    +
			            rtChInfo->startX*bytesPerPixel;
			offset[1] = rtChInfo->pitch[1]*rtChInfo->startY/2  +
			            rtChInfo->startX*bytesPerPixel;

			if(addOffset == FALSE)
			{
				offset[0] = -offset[0];
				offset[1] = -offset[1];
			}

#ifndef SCLR_LINK_ENABLE_MODIFY_FRAME_POINTER_ALWAYS
			if ((rtChInfo->width + rtChInfo->startX) > SCLR_LINK_MAX_WIDTH_SUPPORTED)
#endif
			{
				inFrameList->frames[i]->addr[0][0] =
				    (Ptr) ((Int32)inFrameList->frames[i]->addr[0][0] + offset[0]);
				inFrameList->frames[i]->addr[0][1] =
				    (Ptr) ((Int32)inFrameList->frames[i]->addr[0][1] + offset[1]);
			}
		}
	}
	return 0;
}

Int32  SclrLink_drvCreateReqObj(SclrLink_Obj * pObj)
{
	Int32 status;
	UInt32 reqId;

	memset(pObj->reqObj, 0, sizeof(pObj->reqObj));

	status = Utils_queCreate(&pObj->reqQue,
	                         SCLR_LINK_MAX_REQ,
	                         pObj->reqQueMem, UTILS_QUE_FLAG_NO_BLOCK_QUE);
	UTILS_assert(status == FVID2_SOK);

	pObj->reqQueCount = 0;
	pObj->isReqPend = FALSE;

	for (reqId = 0; reqId < SCLR_LINK_MAX_REQ; reqId++)
	{
		status =
		    Utils_quePut(&pObj->reqQue, &pObj->reqObj[reqId], BIOS_NO_WAIT);
		UTILS_assert(status == FVID2_SOK);
	}

	pObj->reqNumOutLists = 1;

	return FVID2_SOK;
}

Int32 SclrLink_drvCreateOutObj(SclrLink_Obj * pObj)
{
	Int32 status;
	SclrLink_OutObj *pOutObj;
	System_LinkChInfo *pChInfo;
	UInt32 frameId,chId;
	System_LinkChInfo *pInChInfo;
	System_LinkChInfo *pOutChInfo;
	FVID2_Format *pFormat;
	UInt32 maxOutWidth = 0u, maxOutHeight = 0u;
	UInt32 numFrmsPerCh;
	UInt32 i = 0;
	memset(&pObj->outFrameDrop, 0, sizeof(pObj->outFrameDrop));

	maxOutWidth = pObj->inQueInfo.chInfo[0].width;
	maxOutHeight = pObj->inQueInfo.chInfo[0].height;
	if (pObj->createArgs.scaleMode == SCLR_SCALE_MODE_RATIO)
	{
		maxOutHeight = (maxOutHeight * pObj->createArgs.outScaleFactor.ratio.heightRatio.numerator) / pObj->createArgs.outScaleFactor.ratio.heightRatio.denominator;
		maxOutWidth = (maxOutWidth * pObj->createArgs.outScaleFactor.ratio.widthRatio.numerator) / pObj->createArgs.outScaleFactor.ratio.widthRatio.denominator;
	}
	else
	{
		maxOutHeight = pObj->createArgs.outScaleFactor.absoluteResolution.outHeight;
		maxOutWidth = pObj->createArgs.outScaleFactor.absoluteResolution.outWidth;
	}
	maxOutWidth = VpsUtils_floor(maxOutWidth, VPS_BUFFER_ALIGNMENT);


	pOutObj = &pObj->outObj;

	if (SCLR_LINK_NUM_BUFS_PER_CH_DEFAULT == pObj->createArgs.numBufsPerCh)
	{
		numFrmsPerCh = SCLR_LINK_MAX_OUT_FRAMES_PER_CH;
	}
	else
	{
		numFrmsPerCh = pObj->createArgs.numBufsPerCh ;
	}

	pOutObj->outNumFrames = pObj->inQueInfo.numCh * numFrmsPerCh;

	UTILS_assert(pObj->inQueInfo.numCh <= SCLR_LINK_MAX_CH);
	UTILS_assert(pOutObj->outNumFrames <= SCLR_LINK_MAX_OUT_FRAMES);

	for(i = 0; i < pObj->inQueInfo.numCh; i++)
	{
		status = Utils_bufCreate(&pOutObj->bufOutQue[i], TRUE, FALSE);
		UTILS_assert(status == FVID2_SOK);
	}

	pFormat = &pOutObj->outFormat;
	pChInfo = &pObj->info.queInfo[0].chInfo[0];

	pFormat->dataFormat     = pObj->createArgs.outDataFormat; // FVID2_DF_YUV422I_YUYV;//FVID2_DF_YUV422SP_UV; FVID2_DF_YUV420SP_UV


	if( pFormat->dataFormat == FVID2_DF_YUV420SP_UV)
	{
		if (pObj->createArgs.tilerEnable)
		{
			pChInfo->pitch[0] = VPSUTILS_TILER_CNT_8BIT_PITCH;
			pChInfo->pitch[1] = VPSUTILS_TILER_CNT_16BIT_PITCH;
		}
		else
		{
			pChInfo->pitch[0] =
			    VpsUtils_align(maxOutWidth, VPS_BUFFER_ALIGNMENT * 2);
			pChInfo->pitch[1] =
			    VpsUtils_align(maxOutWidth, VPS_BUFFER_ALIGNMENT * 2);
		}
		pChInfo->pitch[2] = 0;
	}
	else
	{
		pChInfo->pitch[0] = VpsUtils_align(maxOutWidth, VPS_BUFFER_ALIGNMENT)*2;
		pChInfo->pitch[1] = pChInfo->pitch[2] = 0;
	}

	pFormat->channelNum     = 0;
	pFormat->width          = maxOutWidth;
	pFormat->height         = maxOutHeight;
	pFormat->fieldMerged[0] = FALSE;
	pFormat->fieldMerged[1] = FALSE;
	pFormat->fieldMerged[2] = FALSE;

	pFormat->pitch[0]       = pChInfo->pitch[0];
	pFormat->pitch[1]       = pChInfo->pitch[1];
	pFormat->pitch[2]       = pChInfo->pitch[2];
	pFormat->scanFormat     = FVID2_SF_PROGRESSIVE;
	pFormat->bpp            = FVID2_BPP_BITS16;
	pFormat->reserved       = NULL;


	/* alloc buffer of max possible size but use only what is needed for a
	 * given resolution */
	if (pObj->createArgs.tilerEnable)
	{
		status = Utils_tilerFrameAlloc(&pOutObj->outFormat,
		                               pOutObj->outFrames,
		                               pOutObj->outNumFrames);
	}
	else
	{
		status = Utils_memFrameAlloc(&pOutObj->outFormat,
		                             pOutObj->outFrames,
		                             pOutObj->outNumFrames);
	}
	UTILS_assert(status == FVID2_SOK);

	i = 0;
	for (frameId = 0; frameId < pObj->outObj.outNumFrames; frameId++)
	{
		pObj->outObj.frameInfo[frameId].rtChInfoUpdate = FALSE;
		pObj->outObj.outFrames[frameId].appData = &pObj->outObj.frameInfo[frameId];

		status = Utils_bufPutEmptyFrame(&pObj->outObj.bufOutQue[frameId/numFrmsPerCh],
	                               	 &pObj->outObj.outFrames[frameId]);
		UTILS_assert(status == FVID2_SOK);

#ifdef SYSTEM_VERBOSE_PRINTS
		{
			Vps_rprintf(" %d: SCLR: %2d: 0x%08x, %d x %d, %08x B\n",
			            Utils_getCurTimeInMsec(),
			            frameId, pOutObj->outFrames[frameId].addr[0][0],
			            pOutObj->outFormat.pitch[0] / 2,
			            pOutObj->outFormat.height,
			            pOutObj->outFormat.height *
			            pOutObj->outFormat.pitch[0]);
		}
#endif
	}

	pObj->info.numQue = SCLR_LINK_MAX_OUT_QUE;
	pObj->info.queInfo[0].numCh = pObj->inQueInfo.numCh;

	for (chId = 0u; chId < pObj->inQueInfo.numCh; chId++)
	{
		pInChInfo = &pObj->inQueInfo.chInfo[chId];

		pOutChInfo = &pObj->info.queInfo[0].chInfo[chId];

		pFormat = &pObj->outObj.outFormat;
		pOutChInfo->memType = VPS_VPDMA_MT_NONTILEDMEM;

		pOutChInfo->dataFormat = (FVID2_DataFormat) pFormat->dataFormat;

		/* Out Width / Height can be derived from pFormat calculated in previous for loop;
		 *  still this is per channel, hence use pInChInfo and recalculate
		 */
		pOutChInfo->width = pInChInfo->width;
		pOutChInfo->height = pInChInfo->height;

		if (pObj->createArgs.scaleMode == SCLR_SCALE_MODE_RATIO)
		{
			pOutChInfo->height = (pOutChInfo->height * pObj->createArgs.outScaleFactor.ratio.heightRatio.numerator) / pObj->createArgs.outScaleFactor.ratio.heightRatio.denominator;
			pOutChInfo->width = (pOutChInfo->width * pObj->createArgs.outScaleFactor.ratio.widthRatio.numerator) / pObj->createArgs.outScaleFactor.ratio.widthRatio.denominator;
		}
		else
		{
			pOutChInfo->height = pObj->createArgs.outScaleFactor.absoluteResolution.outHeight;
			pOutChInfo->width = pObj->createArgs.outScaleFactor.absoluteResolution.outWidth;
		}
		pOutChInfo->width = VpsUtils_floor(pOutChInfo->width, VPS_BUFFER_ALIGNMENT);

		pOutChInfo->startX = 0;
		pOutChInfo->startY = 0;
		pOutChInfo->pitch[0] = pFormat->pitch[0];
		pOutChInfo->pitch[1] = pFormat->pitch[1];
		pOutChInfo->pitch[2] = pFormat->pitch[2];
		pOutChInfo->scanFormat = (FVID2_ScanFormat) pFormat->scanFormat;
	}

	return (status);
}

Int32 SclrLink_drvCreateChObj(SclrLink_Obj * pObj, UInt32 chId)
{
	SclrLink_ChObj *pChObj;
	System_LinkChInfo *pInChInfo;
	System_LinkChInfo *pOutChInfo;
	Vps_M2mScChParams *pDrvChParams;
	FVID2_Format *pFormat;
	Int32 status;
	pChObj = &pObj->chObj[chId];

	status = Utils_bufCreate(&pChObj->inQue, FALSE, FALSE);
	UTILS_assert(status == FVID2_SOK);

	pChObj->nextFid = 0;

	pInChInfo = &pObj->inQueInfo.chInfo[chId];

	if(pInChInfo->width > SCLR_LINK_MAX_WIDTH_SUPPORTED)
	{
		pInChInfo->width = SCLR_LINK_MAX_WIDTH_SUPPORTED;
	}

	if(pInChInfo->height > SCLR_LINK_MAX_HEIGHT_SUPPORTED)
	{
		pInChInfo->height = SCLR_LINK_MAX_HEIGHT_SUPPORTED;
	}

	pDrvChParams = &pObj->drvChArgs[chId];

	pFormat = &pDrvChParams->inFmt;

	pFormat->channelNum = chId;
	pFormat->width = pInChInfo->width;
	pFormat->height = pInChInfo->height;

	pFormat->fieldMerged[0] = FALSE;
	pFormat->fieldMerged[1] = FALSE;
	pFormat->fieldMerged[2] = FALSE;
	pFormat->pitch[0] = pInChInfo->pitch[0];
	pFormat->pitch[1] = pInChInfo->pitch[1];
	pFormat->pitch[2] = pInChInfo->pitch[2];
	pFormat->dataFormat = pInChInfo->dataFormat;
	pFormat->scanFormat = pInChInfo->scanFormat;//FVID2_SF_PROGRESSIVE;
	pFormat->bpp = FVID2_BPP_BITS16;
	pFormat->reserved = NULL;

	pChObj->scCropConfig.cropStartX = 0;
	pChObj->scCropConfig.cropWidth = pInChInfo->width;
	pChObj->scCropConfig.cropStartY = 0;
	pChObj->scCropConfig.cropHeight = pInChInfo->height;

	if(pObj->createArgs.enableLineSkipSc && pInChInfo->memType == VPS_VPDMA_MT_NONTILEDMEM)
	{
		/* half the height and double the pitch
		    possible only when input is non-tiled
		 */
		pFormat->pitch[0] *= 2;
		pFormat->pitch[1] *= 2;
		pFormat->height /= 2;
		/* half the height and crop start Y
		   possible only when input is non-tiled
		 */
		pChObj->scCropConfig.cropStartY /= 2;
		pChObj->scCropConfig.cropHeight /= 2;
	}

	pDrvChParams->inMemType = pInChInfo->memType;

	pChObj->curFrameNum = 0;
	pChObj->frameSkipCount = 0;
	pChObj->frameSkipCtx.firstTime = TRUE;

	pChObj->frameSkipCtx.firstTime = TRUE;
	pChObj->frameSkipCtx.inputFrameRate = pObj->createArgs.inputFrameRate;
	pChObj->frameSkipCtx.outputFrameRate = pObj->createArgs.outputFrameRate;

	pChObj->doFrameDrop = FALSE;

	pChObj->enableOut = TRUE;

	pOutChInfo = &pObj->info.queInfo[0].chInfo[chId];

	pFormat = &pChObj->outFormat;

	if (pInChInfo->width < pOutChInfo->width
	        || pInChInfo->height < pOutChInfo->height)
	{
		pObj->loadUpsampleCoeffs = TRUE;
	}

	pFormat->channelNum = chId;
	pFormat->width = pOutChInfo->width;
	pFormat->height = pOutChInfo->height;
	pFormat->fieldMerged[0] = FALSE;
	pFormat->fieldMerged[1] = FALSE;
	pFormat->fieldMerged[2] = FALSE;
	pFormat->pitch[0] = pOutChInfo->pitch[0];
	pFormat->pitch[1] = pOutChInfo->pitch[1];
	pFormat->pitch[2] = pOutChInfo->pitch[2];
	pFormat->dataFormat = pOutChInfo->dataFormat;
	pFormat->scanFormat = pOutChInfo->scanFormat;
	pFormat->bpp = FVID2_BPP_BITS16;
	pFormat->reserved = NULL;

	/* Initilaize the rtparm ouput resolution from outObj */
	pChObj->chRtOutInfoUpdate = FALSE;
	pChObj->chRtInInfoUpdate = FALSE;

	pChObj->isSkipOneFidType = FALSE;
	pChObj->skipFidType = FVID2_FID_BOTTOM;

	memset(&pChObj->sclrRtPrm, 0, sizeof(pChObj->sclrRtPrm));

	pChObj->sclrRtOutFrmPrm.width =
	    pObj->outObj.outFormat.width;
	pChObj->sclrRtOutFrmPrm.height =
	    pObj->outObj.outFormat.height;
	pChObj->sclrRtOutFrmPrm.pitch[0] =
	    pObj->outObj.outFormat.pitch[0];
	pChObj->sclrRtOutFrmPrm.pitch[1] =
	    pObj->outObj.outFormat.pitch[1];
	pChObj->sclrRtOutFrmPrm.pitch[2] =
	    pObj->outObj.outFormat.pitch[2];
	pChObj->sclrRtOutFrmPrm.memType = pOutChInfo->memType;

	pChObj->sclrRtOutFrmPrm.dataFormat = pOutChInfo->dataFormat;

	pChObj->scCfg.bypass = FALSE;
	pChObj->scCfg.nonLinear = FALSE;
	pChObj->scCfg.stripSize = 0;
	pChObj->scCfg.vsType = VPS_SC_VST_POLYPHASE;

	/* 等比缩放使用 */
	pChObj->outOffset  = 0;
	pChObj->SclrWidth  = 0;
	pChObj->SclrHeight = 0;

	return FVID2_SOK;
}


Int32 SclrLink_drvSetScCoeffs(FVID2_Handle fvidHandle, Bool loadUpsampleCoeffs)
{
	Int32 retVal = FVID2_SOK;
	Vps_ScCoeffParams coeffPrms;

	if (loadUpsampleCoeffs)
	{
		Vps_rprintf(" %d: SCLR    : Loading Up-scaling Co-effs ... \n",
		            Utils_getCurTimeInMsec());

		coeffPrms.hScalingSet = VPS_SC_US_SET;
		coeffPrms.vScalingSet = VPS_SC_US_SET;
	}
	else
	{
		Vps_rprintf(" %d: SCLR    : Loading Down-scaling Co-effs ... \n",
		            Utils_getCurTimeInMsec());

		coeffPrms.hScalingSet = VPS_SC_DS_SET_0;
		coeffPrms.vScalingSet = VPS_SC_DS_SET_0;
	}
	coeffPrms.coeffPtr = NULL;
	coeffPrms.scalarId = VPS_M2M_SC_SCALAR_ID_DEFAULT;

	/* Program scalar coefficient - Always used */
	retVal = FVID2_control(fvidHandle, IOCTL_VPS_SET_COEFFS, &coeffPrms, NULL);
	UTILS_assert(FVID2_SOK == retVal);

	Vps_rprintf(" %d: SCLR    : Co-effs Loading ... DONE !!!\n",
	            Utils_getCurTimeInMsec());

	return (retVal);
}


Int32 SclrLink_drvCreateFvidObj(SclrLink_Obj * pObj)
{
	Vps_M2mScChParams *pDrvChPrm;
	UInt32 chId;
	System_LinkChInfo *pChInfo;
	FVID2_CbParams cbParams;
	UInt32 pathId = VPS_M2M_INST_SEC0_SC5_WB2;

	pObj->scCreateParams.mode = VPS_M2M_CONFIG_PER_CHANNEL;
	pObj->scCreateParams.numChannels = pObj->inQueInfo.numCh;;
	pObj->scCreateParams.chParams = (Vps_M2mScChParams *) pObj->drvChArgs;

	for (chId = 0; chId < pObj->scCreateParams.numChannels; chId++)
	{
		pDrvChPrm = &pObj->drvChArgs[chId];

		/* assume all CHs are of same input size, format, pitch */
		pChInfo = &pObj->inQueInfo.chInfo[chId];

		Vps_printf("chid[%d] inFmt.width[%d] inFmt.height[%d] inFmt.pitch0[%d] inFmt.pitch2[%d]",\
		chId,pChInfo->width,pChInfo->height,pChInfo->pitch[0],pChInfo->pitch[1]);

		pDrvChPrm->inFmt.channelNum = chId;
		pDrvChPrm->inFmt.width = pChInfo->width;
		pDrvChPrm->inFmt.height = pChInfo->height;
		pDrvChPrm->inFmt.pitch[0] = pChInfo->pitch[0];
		pDrvChPrm->inFmt.pitch[1] = pChInfo->pitch[1];
		pDrvChPrm->inFmt.pitch[2] = pChInfo->pitch[2];
		pDrvChPrm->inFmt.fieldMerged[0] = FALSE;
		pDrvChPrm->inFmt.fieldMerged[1] = FALSE;
		pDrvChPrm->inFmt.fieldMerged[0] = FALSE;
		pDrvChPrm->inFmt.dataFormat = pChInfo->dataFormat;
		pDrvChPrm->inFmt.scanFormat = FVID2_SF_PROGRESSIVE;
		pDrvChPrm->inFmt.bpp = FVID2_BPP_BITS16;

		memcpy(&pDrvChPrm->outFmt, &pObj->outObj.outFormat, sizeof(FVID2_Format));
		Vps_printf("chid[%d] outFmt.width[%d] outFmt.height[%d] outFmt.pitch0[%d] outFmt.pitch2[%d]",\
			chId,pDrvChPrm->outFmt.width,pDrvChPrm->outFmt.height,pDrvChPrm->outFmt.pitch[0],pDrvChPrm->outFmt.pitch[1]);

		pDrvChPrm->inMemType = pChInfo->memType;

		if (pObj->createArgs.tilerEnable)
			pDrvChPrm->outMemType = VPS_VPDMA_MT_TILEDMEM;
		else
			pDrvChPrm->outMemType = VPS_VPDMA_MT_NONTILEDMEM;

		pDrvChPrm->scCfg = &pObj->chObj[chId].scCfg;
		pDrvChPrm->srcCropCfg = &pObj->chObj[chId].scCropConfig;

		pDrvChPrm->scCfg->bypass = FALSE;
		pDrvChPrm->scCfg->nonLinear = FALSE;
		pDrvChPrm->scCfg->stripSize = 0;
		pDrvChPrm->scCfg->vsType = VPS_SC_VST_POLYPHASE;

		pDrvChPrm->srcCropCfg->cropStartX = 0;
		pDrvChPrm->srcCropCfg->cropStartY = 0;
		pDrvChPrm->srcCropCfg->cropWidth = pChInfo->width;
		pDrvChPrm->srcCropCfg->cropHeight = pChInfo->height;

		if(pObj->createArgs.enableLineSkipSc && pChInfo->memType == VPS_VPDMA_MT_NONTILEDMEM)
		{
			/* half the height and double the pitch possible
			   only when input is non-tiled */
			pDrvChPrm->inFmt.pitch[0] *= 2;
			pDrvChPrm->inFmt.pitch[1] *= 2;
			pDrvChPrm->inFmt.height /= 2;

			/* half the height and crop start Y
			   possible only when input is non-tiled
			 */
			pDrvChPrm->srcCropCfg->cropStartY /= 2;
			pDrvChPrm->srcCropCfg->cropHeight /= 2;
		}

		pObj->chObj[chId].sclrRtInFrmPrm.width = pDrvChPrm->inFmt.width;
		pObj->chObj[chId].sclrRtInFrmPrm.height = pDrvChPrm->inFmt.height;
		pObj->chObj[chId].sclrRtInFrmPrm.pitch[0] = pDrvChPrm->inFmt.pitch[0];
		pObj->chObj[chId].sclrRtInFrmPrm.pitch[1] = pDrvChPrm->inFmt.pitch[1];
		pObj->chObj[chId].sclrRtInFrmPrm.pitch[2] = pDrvChPrm->inFmt.pitch[2];
		pObj->chObj[chId].sclrRtInFrmPrm.memType = pDrvChPrm->inMemType;
		pObj->chObj[chId].sclrRtInFrmPrm.dataFormat= pDrvChPrm->inFmt.dataFormat;

		pObj->chObj[chId].scCropConfig.cropStartX = pDrvChPrm->srcCropCfg->cropStartX;
		pObj->chObj[chId].scCropConfig.cropStartY = pDrvChPrm->srcCropCfg->cropStartY;
		pObj->chObj[chId].scCropConfig.cropWidth = pDrvChPrm->srcCropCfg->cropWidth;
		pObj->chObj[chId].scCropConfig.cropHeight = pDrvChPrm->srcCropCfg->cropHeight;

		/* Initilaize the rtparm input resolution */
		pObj->chObj[chId].rtChannelInfo.startX = VpsUtils_align(pChInfo->startX, 2);
		pObj->chObj[chId].rtChannelInfo.startY = VpsUtils_align(pChInfo->startY, 2);
		pObj->chObj[chId].rtChannelInfo.width = VpsUtils_align(pChInfo->width, 2);
		pObj->chObj[chId].rtChannelInfo.height = VpsUtils_align(pChInfo->height, 2);
		pObj->chObj[chId].rtChannelInfo.pitch[0] = pChInfo->pitch[0];
		pObj->chObj[chId].rtChannelInfo.pitch[1] = pChInfo->pitch[1];
		pObj->chObj[chId].rtChannelInfo.pitch[2] = pChInfo->pitch[2];
		pObj->chObj[chId].rtChannelInfo.memType = pChInfo->memType;
		pObj->chObj[chId].rtChannelInfo.dataFormat = pChInfo->dataFormat;
	}

	memset(&cbParams, 0, sizeof(cbParams));
	cbParams.cbFxn = SclrLink_drvFvidCb;
	cbParams.errCbFxn = SclrLink_drvFvidErrCb;
	cbParams.errList = &pObj->errProcessList;
	cbParams.appData = pObj;

	switch(pObj->createArgs.pathId)
	{
		case SCLR_LINK_SC5:
			pathId = VPS_M2M_INST_SEC0_SC5_WB2;
			break;
		case SCLR_LINK_SEC0_SC3:
			pathId = VPS_M2M_INST_SEC1_SC4_VIP1;//pathId = VPS_M2M_INST_SEC0_SC3_VIP0;
			break;

		default
				:
			UTILS_assert(FALSE);
	}

	pObj->fvidHandle = FVID2_create(FVID2_VPS_M2M_SC_DRV,
	                                pathId,
	                                &pObj->scCreateParams,
	                                &pObj->scCreateStatus, &cbParams);
	UTILS_assert(pObj->fvidHandle != NULL);

	SclrLink_drvSetScCoeffs(pObj->fvidHandle, TRUE);   /* TODO:
                                                                 * change
                                                                 * based on
                                                                 * scaling
                                                                 * ratio */
	return FVID2_SOK;
}

Int32 SclrLink_drvQueueFramesToChQue(SclrLink_Obj * pObj)
{
	UInt32 frameId, freeFrameNum;
	FVID2_Frame *pFrame;
	System_LinkInQueParams *pInQueParams;
	FVID2_FrameList frameList;
	SclrLink_ChObj *pChObj;
	Int32 status;
    System_FrameInfo *pFrameInfo;
	pInQueParams = &pObj->createArgs.inQueParams;

	System_getLinksFullFrames(pInQueParams->prevLinkId,
	                          pInQueParams->prevLinkQueId, &frameList);
	if (frameList.numFrames)
	{
#ifdef SYSTEM_DEBUG_SCLR_RT
		Vps_printf(" %d: SCLR    : Received %d IN frames !!!\n",
		           Utils_getCurTimeInMsec(), frameList.numFrames);
#endif

        pObj->inFrameGetCount += frameList.numFrames;

        freeFrameNum = 0;

        // in interlace mode, we need top and bottom field to do filed merge mode,
        // so we should treate one top and one bottom as a frame and then do frame drop.
        // so we'll only do frame drop logic when nextFid = 0 for interlace stuff.
        for (frameId = 0; frameId < frameList.numFrames; frameId++)
        {
            pFrame = frameList.frames[frameId];
                /* input channel mapped to a output que */
                pFrameInfo = (System_FrameInfo *) pFrame->appData;
#if 0
		Vps_printf("[SclrLink_drvQueueFramesToChQue]pFrameInfo [%d] width:[%d] height:[%d] startX:[%d] startY:[%d]\n",
				   pFrame->channelNum,
				   pFrameInfo->rtChInfo.width,
				   pFrameInfo->rtChInfo.height,
				   pFrameInfo->rtChInfo.startX,
				   pFrameInfo->rtChInfo.startY);
#endif
		pChObj = &pObj->chObj[pFrame->channelNum];

            pChObj->inFrameRecvCount++;

            pObj->ulSclrFrameRate[pFrame->channelNum]++;
            if ((pChObj->isSkipOneFidType) && (pFrame->fid == pChObj->skipFidType))
            {
                pChObj->inFrameRejectCount++;
                // frame is not of expected FID, so release frame
                frameList.frames[freeFrameNum] = pFrame;
                freeFrameNum++;
                continue;
            }

            if ((pObj->inQueInfo.chInfo[pFrame->channelNum].scanFormat ==
                                               FVID2_SF_PROGRESSIVE) ||
                                               (pChObj->nextFid == 0) ||
                                               (pChObj->isSkipOneFidType))
                pChObj->doFrameDrop = Utils_doSkipFrame(&(pChObj->frameSkipCtx));

            /* frame skipped due to user setting */
            if(pChObj->doFrameDrop)
            {
                pChObj->frameSkipCount++;
                pChObj->outFrameUserSkipCount++;
            }
            //pChObj->nextFid = pFrame->fid;
            if ((pObj->inQueInfo.chInfo[pFrame->channelNum].scanFormat !=
                                                FVID2_SF_INTERLACED) ||
                (pChObj->isSkipOneFidType))
            {
                pChObj->nextFid = pFrame->fid;
            }

            if (((pChObj->nextFid == pFrame->fid) ||
                 (pFrame->fid == FVID2_FID_FRAME)) &&
                 (pChObj->doFrameDrop == FALSE))
            {
                // frame is of the expected FID use it, else drop it
                status = Utils_bufPutFullFrame(&pChObj->inQue, pFrame);
                UTILS_assert(status == FVID2_SOK);
                pChObj->nextFid ^= 1;  // toggle to next
            }
            else
            {
                pChObj->inFrameRejectCount++;

                // frame is not of expected FID, so release frame
                frameList.frames[freeFrameNum] = pFrame;
                freeFrameNum++;
                if (pChObj->nextFid == pFrame->fid)
                {
                    pChObj->nextFid ^= 1;  // toggle to next
                }
            }
        }

        if (freeFrameNum)
        {
            frameList.numFrames = freeFrameNum;
#ifdef SYSTEM_DEBUG_SCLR_RT
			Vps_printf(" %d: SCLR    : Skipped %d IN frames !!!\n",
			           Utils_getCurTimeInMsec(), frameList.numFrames);
#endif

			pObj->inFramePutCount += freeFrameNum;

			System_putLinksEmptyFrames(pInQueParams->prevLinkId,
			                           pInQueParams->prevLinkQueId, &frameList);
		}
	}

	return FVID2_SOK;
}

Int32 SclrLink_drvCreate(SclrLink_Obj * pObj, SclrLink_CreateParams * pPrm)
{
	Semaphore_Params semParams;
	Int32 status;
	UInt32 chId;
	Vps_PlatformCpuRev cpuRev;


#ifdef SYSTEM_DEBUG_SCLR
	Vps_printf(" %d: SCLR: Create in progress !!!\n", Utils_getCurTimeInMsec());
#endif

	UTILS_MEMLOG_USED_START();
	memcpy(&pObj->createArgs, pPrm, sizeof(*pPrm));

	status = System_linkGetInfo(pPrm->inQueParams.prevLinkId, &pObj->inTskInfo);
	UTILS_assert(status == FVID2_SOK);
	Vps_printf("pPrm->inQueParams.prevLinkQueId = %d, pObj->inTskInfo.numQue = %d\n",
				pPrm->inQueParams.prevLinkQueId,
				pObj->inTskInfo.numQue);
	UTILS_assert(pPrm->inQueParams.prevLinkQueId < pObj->inTskInfo.numQue);

	memcpy(&pObj->inQueInfo,
	       &pObj->inTskInfo.queInfo[pPrm->inQueParams.prevLinkQueId],
	       sizeof(pObj->inQueInfo));

#ifndef SYSTEM_USE_TILER
	if (pObj->createArgs.tilerEnable)
	{
		Vps_printf("SCLR:!!WARNING.FORCIBLY DISABLING TILER since tiler is disabled at build time");
		pObj->createArgs.tilerEnable = FALSE;
	}
#endif

	cpuRev = Vps_platformGetCpuRev();

	for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
	{
		if (cpuRev < VPS_PLATFORM_CPU_REV_2_0)
		{
			if((pObj->inQueInfo.chInfo[chId].width >
			        DEI_SC_DRV_422FMT_MAX_WIDTH_LIMIT_BEFORE_CPU_REV_2_0) &&
			        ((pObj->inQueInfo.chInfo[chId].dataFormat != FVID2_DF_YUV420SP_UV) &&
			         (pObj->inQueInfo.chInfo[chId].dataFormat != FVID2_DF_YUV420SP_VU) &&
			         (pObj->inQueInfo.chInfo[chId].dataFormat != FVID2_DF_YUV420P)))
			{

				Vps_printf(" %u: Warning: This CPU Revision [%s] does not"
				           "support current set width %d\n",\
				           Utils_getCurTimeInMsec(), gVpss_cpuVer[cpuRev],\
				           pObj->inQueInfo.chInfo[chId].width);
				Vps_printf(" %u: Warning: Limiting Input width to 960\n",
				           Utils_getCurTimeInMsec());
				{
					pObj->inQueInfo.chInfo[chId].width =
					    DEI_SC_DRV_422FMT_MAX_WIDTH_LIMIT_BEFORE_CPU_REV_2_0;
				}
			}
		}
	}

	pObj->inFrameGetCount = 0;
	pObj->inFramePutCount = 0;
	pObj->outFrameGetCount = 0;
	pObj->outFramePutCount = 0;
	pObj->processFrameReqPendCount = 0;
	pObj->processFrameReqPendSubmitCount = 0;
	pObj->processFrameCount = 0;
	pObj->getProcessFrameCount = 0;
	pObj->processFrameReqCount = 0;
	pObj->getProcessFrameReqCount = 0;
	pObj->totalTime = 0;
	pObj->curTime = 0;
	pObj->givenInFrames = 0x0;
	pObj->returnedInFrames = 0x0;
	pObj->loadUpsampleCoeffs = FALSE;
	//pObj->StretchMod = FALSE;
	SclrLink_resetStatistics(pObj);


	Semaphore_Params_init(&semParams);
	semParams.mode = Semaphore_Mode_BINARY;
	pObj->complete = Semaphore_create(0u, &semParams, NULL);
	UTILS_assert(pObj->complete != NULL);

	SclrLink_drvCreateOutObj(pObj);

	SclrLink_drvDmaCreate(pObj);

	Vps_printf("pObj->inQueInfo.numCh = %d\n",pObj->inQueInfo.numCh);
	for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
	{
		SclrLink_drvCreateChObj(pObj, chId);
	}

	SclrLink_drvCreateFvidObj(pObj);
//    SclrLink_drvCreateScDrv(pObj);
	SclrLink_drvCreateReqObj(pObj);
	UTILS_MEMLOG_USED_END(pObj->memUsed);
	UTILS_MEMLOG_PRINT("SCLR",
	                   pObj->memUsed,
	                   UTILS_ARRAYSIZE(pObj->memUsed));

#ifdef SYSTEM_DEBUG_SCLR
	Vps_printf(" %d: SCLR: Create Done !!!\n", Utils_getCurTimeInMsec());
#endif

	return FVID2_SOK;
}

Int32 SclrLink_drvMakeFrameLists(SclrLink_Obj * pObj,
                                 FVID2_FrameList * inFrameList,
                                 FVID2_FrameList * outFrameList)
{
	SclrLink_ChObj *pChObj;
	UInt32 chId, frameId;
	FVID2_Frame *pInFrame, *pOutFrame;
	Int32 status;
	Bool rtParamUpdatePerFrame;
	System_LinkChInfo *rtChInfo;
	System_FrameInfo *pInFrameInfo;
	System_FrameInfo *pOutFrameInfo;
	FVID2_FrameList frameList;
	UInt32 freeFrameNum;

	frameId = 0;
	freeFrameNum = 0;

	for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
	{
		pChObj = &pObj->chObj[chId];

		Utils_bufGetFullFrame(&pChObj->inQue, &pInFrame, BIOS_NO_WAIT);

		if (pInFrame)
		{
			pOutFrame = NULL;
			if((pChObj->enableOut) &&
			        Utils_queGetQueuedCount (&pObj->outObj.bufOutQue[chId].emptyQue))
			{

				status = Utils_bufGetEmptyFrame(&pObj->outObj.bufOutQue[chId],
				                                &pOutFrame,
				                                BIOS_WAIT_FOREVER);
				UTILS_assert(status == FVID2_SOK);
				UTILS_assert(pOutFrame != NULL);

				inFrameList->frames[frameId] = pInFrame;
				pInFrameInfo = (System_FrameInfo *) pInFrame->appData;
				pChObj->inFrameProcessCount++;

				pOutFrameInfo = (System_FrameInfo *) pOutFrame->appData;
				UTILS_assert(pOutFrameInfo != NULL);
				pOutFrameInfo->sclrOrgChannelNum = chId;
				//Vps_printf("pFrameInfo->sclrOrgChannelNum = %d\n",pOutFrameInfo->sclrOrgChannelNum);
				pObj->outFrameGetCount++;
				pChObj->outFrameCount++;
				outFrameList->frames[frameId] = pOutFrame;
				frameId++;
				pChObj->curFrameNum++;

				pInFrame->perFrameCfg = NULL;
				rtParamUpdatePerFrame = FALSE;
				UTILS_assert (pInFrame->channelNum == chId);
				rtChInfo = &pChObj->rtChannelInfo;

				if(TRUE == pChObj->chRtInInfoUpdate) {

				}

				if (
					(pInFrameInfo != NULL)
					&& (pInFrameInfo->rtChInfoUpdate == TRUE)
				    && (FALSE == pChObj->chRtInInfoUpdate)
				   )
				{
#if 0
					Vps_printf("sclr: old: %d x %d [%d,%d],   new: %d x %d [%d,%d]\n",
					           rtChInfo->width,
					           rtChInfo->height,
					           rtChInfo->pitch[0],
					           rtChInfo->pitch[1],
					           pInFrameInfo->rtChInfo.width,
					           pInFrameInfo->rtChInfo.height,
					           pInFrameInfo->rtChInfo.pitch[0],
					           pInFrameInfo->rtChInfo.pitch[1]);
					Vps_printf("startX = %d, startY = %d\n",
								pInFrameInfo->rtChInfo.startX,
								pInFrameInfo->rtChInfo.startY);

					Vps_printf("pInFrameInfo->rtChInfoUpdate = %d, pChObj->chRtInInfoUpdate = %d\n",
								pInFrameInfo->rtChInfoUpdate,
								pChObj->chRtInInfoUpdate);
#endif
					if(pInFrameInfo->rtChInfo.width > SCLR_LINK_MAX_WIDTH_SUPPORTED)
					{
						pInFrameInfo->rtChInfo.width = SCLR_LINK_MAX_WIDTH_SUPPORTED;
					}

					if(pInFrameInfo->rtChInfo.height > SCLR_LINK_MAX_HEIGHT_SUPPORTED)
					{
						pInFrameInfo->rtChInfo.height = SCLR_LINK_MAX_HEIGHT_SUPPORTED;
					}

					if (pInFrameInfo->rtChInfo.startX != rtChInfo->startX)
					{
						rtChInfo->startX = pInFrameInfo->rtChInfo.startX;
						rtParamUpdatePerFrame = TRUE;
					}
					if (pInFrameInfo->rtChInfo.startY != rtChInfo->startY)
					{
						rtChInfo->startY = pInFrameInfo->rtChInfo.startY;
						rtParamUpdatePerFrame = TRUE;
					}
					if (pInFrameInfo->rtChInfo.height != rtChInfo->height)
					{
						rtChInfo->height = pInFrameInfo->rtChInfo.height;
						rtParamUpdatePerFrame = TRUE;
					}
					if (pInFrameInfo->rtChInfo.width != rtChInfo->width)
					{
						rtChInfo->width = pInFrameInfo->rtChInfo.width;
						rtParamUpdatePerFrame = TRUE;
					}
					if (pInFrameInfo->rtChInfo.pitch[0] != rtChInfo->pitch[0])
					{
						rtChInfo->pitch[0] = pInFrameInfo->rtChInfo.pitch[0];
						rtParamUpdatePerFrame = TRUE;
					}
					if (pInFrameInfo->rtChInfo.pitch[1] != rtChInfo->pitch[1])
					{
						rtChInfo->pitch[1] = pInFrameInfo->rtChInfo.pitch[1];
						rtParamUpdatePerFrame = TRUE;
					}
					///pChObj->chRtInInfoUpdate = FALSE;
				}

				if ((pChObj->chRtOutInfoUpdate == TRUE) ||
				        (rtParamUpdatePerFrame == TRUE))
				{
					SclrLink_drvUpdateRtParams(pObj, chId);

					Vps_printf("SclrLink_drvUpdateRtParams!!!! outwidth[%d] outheight[%d] pitch0[%d] inwidth[%d] inheight[%d] pitch0[%d] %d\n",\
						pChObj->sclrRtOutFrmPrm.width,pChObj->sclrRtOutFrmPrm.height,pChObj->sclrRtOutFrmPrm.pitch[0],\
						pChObj->sclrRtInFrmPrm.width,pChObj->sclrRtInFrmPrm.height,pChObj->sclrRtInFrmPrm.pitch[0],chId);

					pInFrame->perFrameCfg = &pChObj->sclrRtPrm;
					pChObj->sclrRtPrm.inFrmPrms  = &pChObj->sclrRtInFrmPrm;

					if(TRUE == pChObj->StretchMod)
					{
						SclrLink_CalSclrMode CalSclrMode;
						UInt32 outOffset = 0;
						UInt32 width;
						UInt32 height;
						CalSclrMode.SrcWidth  = pChObj->sclrRtInFrmPrm.width ;
						CalSclrMode.SrcHeight = pChObj->sclrRtInFrmPrm.height;
						CalSclrMode.DstWidth  = pChObj->outFormat.width;
						CalSclrMode.DstHeight = pChObj->outFormat.height;
						CalSclrMode.Dstpitch[0] = pChObj->outFormat.pitch[0];

						Vps_printf("CalSclrMode.SrcWidth=%d,SrcHeight=%d,DstWidth=%d,DstHeight=%d,Dstpitch[0]=%d\n",
							CalSclrMode.SrcWidth,CalSclrMode.SrcHeight,CalSclrMode.DstWidth,CalSclrMode.DstHeight,CalSclrMode.Dstpitch[0]);

						/* 计算等比缩放后的宽高 */
						outOffset = SclrLink_CalculationSclrMode(CalSclrMode, &width, &height);

						if( (outOffset != 0) )
						//&&  ((width < pChObj->SclrWidth)||(height < pChObj->SclrHeight)) )
						{
							UInt key;
							key = Hwi_disable();
							SclrLink_drvCheckBlankOutputBuffer(pObj, chId, &CalSclrMode.DstWidth, &CalSclrMode.DstHeight);
							Hwi_restore(key);
						}

						pChObj->outOffset = outOffset;

						/* 主要修改的内容 */
						pChObj->sclrRtOutFrmPrm.width    = width;
						pChObj->sclrRtOutFrmPrm.height   = height;
						pChObj->sclrRtOutFrmPrm.pitch[0] = pChObj->outFormat.pitch[0];
						pChObj->sclrRtOutFrmPrm.pitch[1] = 0;
						pChObj->SclrWidth  = width;
						pChObj->SclrHeight = height;

					}

					pChObj->sclrRtPrm.outFrmPrms = &pChObj->sclrRtOutFrmPrm;
					pChObj->sclrRtPrm.srcCropCfg = &pChObj->scCropConfig;
					pChObj->chRtOutInfoUpdate = FALSE;
				}

				pOutFrameInfo->rtChInfo.width    = pChObj->outFormat.width;
				pOutFrameInfo->rtChInfo.height   = pChObj->outFormat.height;
				pOutFrameInfo->rtChInfo.pitch[0] = pChObj->outFormat.pitch[0];
				pOutFrameInfo->rtChInfo.pitch[1] = pChObj->outFormat.pitch[1];
				pOutFrameInfo->rtChInfoUpdate = TRUE;
				if (pInFrameInfo != NULL)
					pOutFrameInfo->ts64  = pInFrameInfo->ts64;

				pOutFrame->outOffset  = pChObj->outOffset;

				pOutFrame->addr[0][0] = (Ptr) ((Int32)pOutFrame->addr[0][0] + pChObj->outOffset);
				pOutFrame->addr[0][1] = 0;

				pOutFrame->channelNum = pInFrame->channelNum;
				pOutFrame->timeStamp  = pInFrame->timeStamp;
				pOutFrame->fid = pInFrame->fid;
			}
			else
			{
				pChObj->outFrameSkipCount++;
				frameList.frames[freeFrameNum] = pInFrame;
				freeFrameNum++;
			}
		}
	}
	inFrameList->numFrames  = frameId;
	outFrameList->numFrames = frameId;

	if (freeFrameNum)
	{
		frameList.numFrames = freeFrameNum;
		System_putLinksEmptyFrames(pObj->createArgs.inQueParams.prevLinkId,
		                           pObj->createArgs.inQueParams.prevLinkQueId,
		                           &frameList);
		pObj->inFramePutCount += freeFrameNum;
	}

	return FVID2_SOK;
}

Int32 SclrLink_drvReleaseFrames(SclrLink_Obj * pObj,
                                FVID2_FrameList * inFrameList,
                                FVID2_FrameList outFrameList)
{
	UInt32 frameId, latency;
	FVID2_Frame *pFrame;
	Int32 status, sendCmd;
	SclrLink_ChObj *pChObj;

	sendCmd = FALSE;
	for (frameId = 0; frameId < inFrameList->numFrames; frameId++)
	{
		pFrame = outFrameList.frames[frameId];
		if (pFrame && pFrame != &pObj->outFrameDrop)
		{
			pChObj = &pObj->chObj[pFrame->channelNum];

			latency = Utils_getCurTimeInMsec() - pFrame->timeStamp;

			if(latency>pChObj->maxLatency)
				pChObj->maxLatency = latency;
			if(latency<pChObj->minLatency)
				pChObj->minLatency = latency;

#if 0
			if(pFrame->channelNum == 0)
			{
				System_FrameInfo *pFrameInfo;
				pFrameInfo = (System_FrameInfo *)pFrame->appData;

				Vps_printf("rtChInfoUpdate[%d] [%d] [%d] [%d]",pFrameInfo->rtChInfoUpdate,pFrameInfo->rtChInfo.width,pFrameInfo->rtChInfo.height,pFrameInfo->rtChInfo.pitch[0]);

			}
#endif
			pFrame->addr[0][0] = (Ptr) ((Int32)pFrame->addr[0][0] - pFrame->outOffset);
			pFrame->addr[0][1] = 0;
			pFrame->outOffset  = 0;
			status =
			    Utils_bufPutFullFrame(&pObj->outObj.bufOutQue[0],
			                          pFrame);
			UTILS_assert(status == FVID2_SOK);

			sendCmd = TRUE;

			pObj->outFramePutCount++;
		}
	}

#ifdef SYSTEM_DEBUG_SCLR_RT
	Vps_printf(" %d: SCLR    : Released %d IN frames !!!\n", Utils_getCurTimeInMsec(),
	           inFrameList->numFrames);
#endif

	pObj->inFramePutCount += inFrameList->numFrames;

	System_putLinksEmptyFrames(pObj->createArgs.inQueParams.prevLinkId,
	                           pObj->createArgs.inQueParams.prevLinkQueId, inFrameList);

	if(sendCmd == TRUE)
	{
		System_sendLinkCmd(pObj->createArgs.outQueParams.nextLink,
		                   SYSTEM_CMD_NEW_DATA);
	}

	return FVID2_SOK;
}

Int32 SclrLink_drvSubmitData(SclrLink_Obj * pObj)
{
	Int32 status;
	SclrLink_ReqObj *pReqObj;

	status = Utils_queGet(&pObj->reqQue, (Ptr *) & pReqObj, 1, BIOS_NO_WAIT);
	if (status != FVID2_SOK)
	{
#ifdef SYSTEM_DEBUG_SCLR_RT
		Vps_printf(" %d: SCLR    : Pending request !!!\n", Utils_getCurTimeInMsec());
#endif

		pObj->processFrameReqPendCount++;

		pObj->isReqPend = TRUE;

		// cannot process more frames to process
		return FVID2_EFAIL;
	}


	if (pObj->processFrameReqPendCount == pObj->processFrameReqPendSubmitCount)
		pObj->isReqPend = FALSE;

	pReqObj->processList.inFrameList[0] = &pReqObj->inFrameList;
	pReqObj->processList.outFrameList[0] = &pReqObj->outFrameList;
	pReqObj->processList.numInLists = 1;
	pReqObj->processList.numOutLists = pObj->reqNumOutLists;

	pReqObj->inFrameList.appData = pReqObj;

	SclrLink_drvMakeFrameLists(pObj, &pReqObj->inFrameList,
	                           &pReqObj->outFrameList);

#ifdef SYSTEM_DEBUG_SCLR_RT
	Vps_printf(" %d: SCLR    : Submitting %d frames !!!\n", Utils_getCurTimeInMsec(),
	           pReqObj->inFrameList.numFrames);
#endif

	if (pReqObj->inFrameList.numFrames)
	{

		pObj->reqQueCount++;

		pObj->processFrameCount += pReqObj->inFrameList.numFrames;
		pObj->processFrameReqCount++;

		pObj->givenInFrames += pReqObj->inFrameList.numFrames;

		pObj->curTime = Utils_getCurTimeInMsec();
		SclrLink_drvModifyFramePointer(pObj, &pReqObj->inFrameList, 1);
		status = FVID2_processFrames(pObj->fvidHandle, &pReqObj->processList);
		UTILS_assert(status == FVID2_SOK);
		SclrLink_drvModifyFramePointer(pObj, &pReqObj->inFrameList, 0);

#ifndef SCLR_LINK_QUEUE_REQ
		Semaphore_pend(pObj->complete, BIOS_WAIT_FOREVER);

		SclrLink_drvGetProcessedData(pObj);
#endif

	}
	else
	{

		status = Utils_quePut(&pObj->reqQue, pReqObj, BIOS_NO_WAIT);
		UTILS_assert(status == FVID2_SOK);

		// no more frames to process
		status = FVID2_EFAIL;
	}

	return status;
}

Int32 SclrLink_drvProcessData(SclrLink_Obj * pObj)
{
	Int32 status;

	SclrLink_drvQueueFramesToChQue(pObj);

	do
	{
		status = SclrLink_drvSubmitData(pObj);
	}
	while (status == FVID2_SOK);

	return FVID2_SOK;
}

Int32 SclrLink_drvGetProcessedData(SclrLink_Obj * pObj)
{
	SclrLink_ReqObj *pReqObj;
	Int32 status;
	FVID2_ProcessList processList;
	status =
	    FVID2_getProcessedFrames(pObj->fvidHandle, &processList, BIOS_NO_WAIT);
	UTILS_assert(status == FVID2_SOK);

	pObj->curTime = Utils_getCurTimeInMsec() - pObj->curTime;
	pObj->totalTime += pObj->curTime;

#ifdef SYSTEM_DEBUG_SCLR_RT
	Vps_printf(" %d: SCLR    : Completed %d frames !!!\n", Utils_getCurTimeInMsec(),
	           processList.outFrameList[0]->numFrames);
#endif

	pObj->getProcessFrameCount += processList.outFrameList[0]->numFrames;
	pObj->getProcessFrameReqCount++;

	pReqObj = (SclrLink_ReqObj *) processList.inFrameList[0]->appData;

	SclrLink_drvReleaseFrames(pObj, &pReqObj->inFrameList,
	                          pReqObj->outFrameList);

	status = Utils_quePut(&pObj->reqQue, pReqObj, BIOS_NO_WAIT);
	UTILS_assert(status == FVID2_SOK);

	pObj->reqQueCount--;

	if (pObj->isReqPend)
	{
#ifdef SYSTEM_DEBUG_SCLR_RT
		Vps_printf(" %d: SCLR    : Submitting pending request !!!\n",
		           Utils_getCurTimeInMsec());
#endif

		pObj->processFrameReqPendSubmitCount++;

		SclrLink_drvSubmitData(pObj);
	}
	return FVID2_SOK;
}

Int32 SclrLink_drvStop(SclrLink_Obj * pObj)
{
	Int32 rtnValue = FVID2_SOK;

#ifdef SYSTEM_DEBUG_SCLR
	Vps_printf(" %d: SCLR    : Stop in progress, %d requests pending !!!\n",
	           Utils_getCurTimeInMsec(), pObj->reqQueCount);
#endif

	while (pObj->reqQueCount)
	{
		Utils_tskWaitCmd(&pObj->tsk, NULL, SCLR_LINK_CMD_GET_PROCESSED_DATA);
		SclrLink_drvGetProcessedData(pObj);
	}

#ifdef SYSTEM_DEBUG_SCLR
	Vps_printf(" %d: SCLR    : Stop Done !!!\n", Utils_getCurTimeInMsec());
#endif
	return (rtnValue);
}



Int32 SclrLink_drvDelete(SclrLink_Obj * pObj)
{
	UInt32 chId;
	SclrLink_ChObj *pChObj;
	UInt32 i = 0;
#ifdef SYSTEM_DEBUG_SCLR
	Vps_printf(" %d: SCLR    : Fields = %d (fps = %d), FRAMES -> Total %d, skipped %d, fps = %d!!!\n",
	           Utils_getCurTimeInMsec(),
	           pObj->getProcessFrameCount,
	           pObj->getProcessFrameCount * 100 / (pObj->totalTime / 10),
	           pObj->chObj[0].curFrameNum,
	           pObj->chObj[0].frameSkipCount,
	           (pObj->chObj[0].curFrameNum - pObj->chObj[0].frameSkipCount) * 100 / (pObj->totalTime / 10)
	          );
#endif

#ifdef SYSTEM_DEBUG_SCLR
	Vps_printf(" %d: SCLR: Delete in progress !!!\n", Utils_getCurTimeInMsec());
#endif

	FVID2_delete(pObj->fvidHandle, NULL);

	Semaphore_delete(&pObj->complete);

	for (i= 0; i < pObj->inQueInfo.numCh; i++)
	{
		Utils_bufDelete(&pObj->outObj.bufOutQue[i]);
	}

	Utils_memFrameFree(&pObj->outObj.outFormat,
	                   pObj->outObj.outFrames, pObj->outObj.outNumFrames);

	for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
	{
		pChObj = &pObj->chObj[chId];

		Utils_bufDelete(&pChObj->inQue);
	}

	Utils_queDelete(&pObj->reqQue);

	SclrLink_drvDmaDelete(pObj);
#ifdef SYSTEM_DEBUG_SCLR
	Vps_printf(" %d: SCLR: Delete Done !!!\n", Utils_getCurTimeInMsec());
#endif

	return FVID2_SOK;
}

Int32 SclrLink_SetFrameRate(SclrLink_Obj * pObj, SclrLink_ChFpsParams * params)
{

    Int32 status = FVID2_SOK;
    SclrLink_ChObj *pChObj;

    pChObj = &pObj->chObj[params->chId];
	params->inputFrameRate = pObj->ulSCHistoryFrameRate[params->chId];
	if((params->inputFrameRate > 85) || (params->inputFrameRate == 0))
	{
		params->inputFrameRate = 85;
	}

	pChObj->frameSkipCtx.firstTime = TRUE;
	pChObj->frameSkipCtx.inputFrameRate = params->inputFrameRate;
	pChObj->frameSkipCtx.outputFrameRate = params->outputFrameRate;

	return (status);
}

Int32 SclrLink_SetInputFrameRate(SclrLink_Obj * pObj, SclrLink_ChFpsParams * params)
{
    Int32 status = FVID2_SOK;
    SclrLink_ChObj *pChObj;

    pChObj = &pObj->chObj[params->chId];
	params->inputFrameRate = pObj->ulSCHistoryFrameRate[params->chId];
	if((params->inputFrameRate > 85) || (params->inputFrameRate == 0))
	{
		params->inputFrameRate = 85;
	}

//	Vps_printf(" pChObj->frameSkipCtx.inputFrameRate = %d %d %d\n",
//			params->inputFrameRate, params->chId,
//			pChObj->frameSkipCtx.outputFrameRate);
    pChObj->frameSkipCtx.firstTime = TRUE;
    pChObj->frameSkipCtx.inputFrameRate = params->inputFrameRate;

    return (status);

}

Int32 SclrLink_drvSetChannelInfo(SclrLink_Obj * pObj, SclrLink_ChannelInfo *channelInfo)
{
	Int32 status = FVID2_SOK;
	SclrLink_ChObj *pChObj;

	UTILS_assert(channelInfo->channelId < (SCLR_LINK_MAX_CH * 2));


	pChObj = &pObj->chObj[channelInfo->channelId];
	pChObj->enableOut = channelInfo->enable;

	return status;

}

Int32 SclrLink_drvGetChDynamicOutputRes(SclrLink_Obj * pObj,
                                        SclrLink_chDynamicSetOutRes * params)
{
	Int32 status = FVID2_SOK;
	SclrLink_ChObj *pChObj;
	UInt32 chId;

	if (params->chId < SCLR_LINK_MAX_CH)
	{
		chId = params->chId;
		pChObj = &pObj->chObj[chId];

		params->width = pChObj->sclrRtOutFrmPrm.width;
		params->height = pChObj->sclrRtOutFrmPrm.height;
		params->pitch[0] = pChObj->sclrRtOutFrmPrm.pitch[0];
		params->pitch[1] = pChObj->sclrRtOutFrmPrm.pitch[1];
		params->pitch[2] = pChObj->sclrRtOutFrmPrm.pitch[2];
	}
	return (status);
}

Int32 SclrLink_drvSetChDynamicOutputRes(SclrLink_Obj * pObj,
                                        SclrLink_chDynamicSetOutRes * params)
{
	Int32 status = FVID2_SOK;
	SclrLink_ChObj *pChObj;
	UInt32 chId;
	UInt32 width;
    UInt32 height;
	SclrLink_CalSclrMode CalSclrMode;
	FVID2_Format *pFormat;
	UInt32 outOffset = 0;

	if (params->chId < SCLR_LINK_MAX_CH)
	{
		chId = params->chId;
		pChObj = &pObj->chObj[chId];

		/* 设置输出格式 */
		pFormat = &pChObj->outFormat;
		pFormat->width  = params->width;
		pFormat->height = params->height;
		pFormat->pitch[0] = params->pitch[0];
		pFormat->pitch[1] = params->pitch[1];

		if(TRUE == pChObj->StretchMod)
		{
			#if 0
			CalSclrMode.SrcWidth  = pChObj->sclrRtInFrmPrm.width ;
			CalSclrMode.SrcHeight = pChObj->sclrRtInFrmPrm.height;
			CalSclrMode.DstWidth  = params->width;
			CalSclrMode.DstHeight = params->height;
			CalSclrMode.Dstpitch[0] = params->pitch[0];

			/* 计算等比缩放后的宽高 */
			outOffset = SclrLink_CalculationSclrMode(CalSclrMode, &width, &height);

			/* 更改实时参数 */
			pChObj->sclrRtOutFrmPrm.width    = width;
			pChObj->sclrRtOutFrmPrm.height   = height;
			pChObj->sclrRtOutFrmPrm.pitch[0] = params->pitch[0];
			pChObj->sclrRtOutFrmPrm.pitch[1] = 0;
			pChObj->outOffset = outOffset;
			#endif
			pChObj->chRtOutInfoUpdate = TRUE;

		}
		else
		{
			pChObj->sclrRtOutFrmPrm.width    = params->width;
			pChObj->sclrRtOutFrmPrm.height   = params->height;
			pChObj->sclrRtOutFrmPrm.pitch[0] = params->pitch[0];
			pChObj->sclrRtOutFrmPrm.pitch[1] = 0;
			pChObj->outOffset = 0;
			pChObj->chRtOutInfoUpdate = TRUE;
		}
	}
	return (status);
}

Int32 SclrLink_drvDynamicSkipFidType(SclrLink_Obj * pObj,
                                     SclrLink_chDynamicSkipFidType * params)
{
	Int32 status = FVID2_SOK;
	SclrLink_ChObj *pChObj;
	UInt32 chId;

	if (params->chId < SCLR_LINK_MAX_CH)
	{
		chId = params->chId;
		pChObj = &pObj->chObj[chId];

		pChObj->isSkipOneFidType = TRUE;
		pChObj->skipFidType = params->fidType;
	}
	return (status);
}

