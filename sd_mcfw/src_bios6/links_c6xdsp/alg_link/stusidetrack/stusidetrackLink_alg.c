/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include "stusidetrackLink_priv.h"
#include <mcfw/interfaces/common_def/ti_vsys_common_def.h>
#include "track_students_right_side.h"


Int32 AlgLink_StuSideTrackalgSetChTracePrm(AlgLink_StuSideTrackChObj *pChObj,
                            AlgLink_StuSideTrackCreateParams * params)
{
	Int32 status = 0;
	StuSideITRACK_Params *pChParams =	&pChObj->stusidetrackhandle.input_param;
	pChObj->StuSideAddr[0][0] = params->StuSideAddr[0][0];
	pChObj->StuSideAddr[0][1] = params->StuSideAddr[0][1];
	pChObj->StuSideAddr[1][0] = params->StuSideAddr[1][0];
	pChObj->StuSideAddr[1][1] = params->StuSideAddr[1][1];

	memset(pChParams, 0x0, sizeof(StuSideITRACK_Params));
	memcpy(pChParams, &params->StuSideTrackParms, sizeof(StuSideITRACK_Params));

	Vps_printf("Pic width x height[ %dx%d ] VD width x height[ %dx%d ] Size[%d] \n",pChParams->static_param.pic_width, pChParams->static_param.pic_height,\
	pChParams->static_param.video_width,pChParams->static_param.video_height,pChParams->static_param.size);


	return (status);
}





UInt32 AlgLink_StuSideTrackalgMalloc(AlgLink_StuSideTrackChObj * pChObj)
{

	StuSideITRACK_Params  Params;
	StuSideITRACK_Params *TrackParams  = &Params;
	Bitstream_Buf buf;

	T_STU_SIDE_GLOBAL_OBJ *pObj =  &pChObj->stusidetrackhandle.t_global_obj;
	
	TrackParams->static_param.video_width  = 1920;
	TrackParams->static_param.video_height = 1080;
	TrackParams->static_param.pic_width    = 480;
	TrackParams->static_param.pic_height   = 270;

	UInt32 Size0 = TrackParams->static_param.video_width * TrackParams->static_param.video_height;
//	UInt32 Size1 = TrackParams->static_param.pic_width   * TrackParams->static_param.pic_height;

    Utils_memBitBufAlloc(&buf,1920*1080*2,1);
	pObj->InBuffer = buf.addr;
	UTILS_assert(pObj->InBuffer != NULL);


	Utils_memBitBufAlloc(&buf,Size0,1);
	pObj->Area_vertex = buf.addr;
	UTILS_assert(pObj->Area_vertex != NULL);

	
	Utils_memBitBufAlloc(&buf,Size0,1);
	pObj->src_y_buf = buf.addr;
	UTILS_assert(pObj->src_y_buf != NULL);

	Utils_memBitBufAlloc(&buf,Size0,1);
	pObj->tmp_src_y_buf   = buf.addr;
	UTILS_assert(pObj->tmp_src_y_buf != NULL);

	
	Utils_memBitBufAlloc(&buf,Size0,1);
	pObj->gray_buf = buf.addr;
	UTILS_assert(pObj->gray_buf != NULL);

	
	Utils_memBitBufAlloc(&buf,Size0,1);
	pObj->mid_buf   = buf.addr;
	UTILS_assert(pObj->mid_buf != NULL);
	
	Utils_memBitBufAlloc(&buf,Size0,1);
	pObj->marked_buf  = buf.addr;
	UTILS_assert(pObj->marked_buf != NULL);

	return FVID2_SOK;
}







Int32 AlgLink_StuSideTrackalgChInit(AlgLink_StuSideTrackChObj *pChObj)
{

	
	T_STU_SIDE_GLOBAL_OBJ *pobj = &pChObj->stusidetrackhandle.t_global_obj;
	if(pobj == NULL)
	{
		return FVID2_EFAIL;
	}
	//初始化依赖初始参数

	//初始化全局变量数据结构
	pobj->nSrcBufWidth  = 1920;
	pobj->nSrcBufHeight = 1080;

	pobj->turn_flag 	= 0;
	pobj->track_start 	= 0;
	pobj->turn_time 	= 0;
	pobj->first_time 	= 0;
	pobj->last_move_flag  = 0;
	pobj->skin_colour_num = 0;
	pobj->frame_num       = 0;

	track_students_right_side_area_check(&pChObj->stusidetrackhandle);
	return FVID2_SOK;
}





Int32 AlgLink_StuSideTrackalgChCreate(AlgLink_StuSideTrackChObj *pChObj, AlgLink_StuSideTrackCreateParams *pChPrm)
{
	Int32 status;
	//先按最大分配
	Int32 CacheStatus = 0;
	CacheStatus = SharedRegion_isCacheEnabled(0);
	Vps_printf("SYSTEM_IPC_SR_M3_LIST_MP CacheStatusc [%d]\n",CacheStatus);
	CacheStatus = SharedRegion_isCacheEnabled(1);
	Vps_printf("SYSTEM_IPC_SR_CACHED CacheStatusc [%d]\n",CacheStatus);
	CacheStatus = SharedRegion_isCacheEnabled(2);
	Vps_printf("SYSTEM_IPC_SR_VIDEO_FRAME CacheStatusc [%d]\n",CacheStatus);


	//设置通道信息澹澹(用户参数)
    status = AlgLink_StuSideTrackalgSetChTracePrm(pChObj, pChPrm);
    UTILS_assert(status == FVID2_SOK);
	Vps_printf("AlgLink_StuSideTrackalgCreate: AlgLink_StuSideTrackalgSetChTracePrm Ing\n");

	Vps_printf("AlgLink_StuSideTrackalgCreate: AlgLink_StuSideTrackalgMalloc Ing\n");
	status = AlgLink_StuSideTrackalgMalloc(pChObj);
	UTILS_assert(status == FVID2_SOK);
	
	
	AlgLink_StuSideTrackalgChInit(pChObj);

	
	return FVID2_SOK;
}

Int32 AlgLink_StuSideTrackalgPrintPrm(AlgLink_StuSideTrackCreateParams *pChPrm)
{

	Int32 i = 0;
	StuSideITRACK_Params *pTrackParms = &pChPrm->StuSideTrackParms;
	Vps_printf("StuSideAddr[%p] size[%d] \npic_width[%d] pic_height[%d] video_width[%d] video_height[%d]\n"\
			   "reset_time[%d] message[%d]   reset_level[%d]\n"\
			    "sens[%d] track_point_num[%d] "\
				,pChPrm->StuSideAddr[0][0],
				pTrackParms->static_param.size,
				pTrackParms->static_param.pic_width,
				pTrackParms->static_param.pic_height ,
				pTrackParms->static_param.video_width,
				pTrackParms->static_param.video_height,
				pTrackParms->dynamic_param.reset_time,
				pTrackParms->dynamic_param.message,
				pTrackParms->dynamic_param.reset_level,
				pTrackParms->dynamic_param.sens,
				pTrackParms->dynamic_param.track_point_num
				);

	Vps_printf("Point [n]  [ x:y ]\n");
	for(i = 0; i < pTrackParms->dynamic_param.track_point_num; i++)
	{
		Vps_printf("     [%2d] [%3d:%3d]\n",i,pTrackParms->dynamic_param.track_point[i].x,pTrackParms->dynamic_param.track_point[i].y);
	}

	return FVID2_SOK;
}



Int32 AlgLink_StuSideTrackalgCreate(AlgLink_StuSideTrackObj * pObj)
{
	Int32 status, chId ,numchanel,i;
    AlgLink_StuSideTrackChObj *pChObj;
    AlgLink_StuSideTrackCreateParams *pChPrm;

	Vps_printf("AlgLink_StuSideTrackalgCreate Ing [%d]...\n",pObj->inQueInfo->numCh);


//	pObj->inQueInfo->numCh = 1;

	if(pObj->inQueInfo->numCh > ALG_LINK_STUSIDETRACK_MAX_CH)
	{
		numchanel = ALG_LINK_STUSIDETRACK_MAX_CH;
	}
	else
	{
		numchanel = pObj->inQueInfo->numCh;
	}

	for(i = 0; i < ALG_LINK_STUSIDETRACK_MAX_TO_CH; i++)
	{
		pObj->ChToStream[i] = pObj->stusidetrackChCreateParams[0].ChToStream[i];
	}
	

    for(chId=0; chId< numchanel; chId++)
    {
        pChObj = &pObj->chObj[chId];

        pChPrm = &pObj->stusidetrackChCreateParams[chId];

        pChObj->chId = chId;
	
		//创建参数初始化,暂时放这里
		AlgLink_StuSideTrackalgPrintPrm(pChPrm);
		status	= AlgLink_StuSideTrackalgChCreate(pChObj,pChPrm);

		UTILS_assert(status == FVID2_SOK);
    }

	return FVID2_SOK;
}


Int32 AlgLink_StuSideTracealgDelete(AlgLink_StuSideTrackObj * pObj)
{
  //  SWOSD_close(&pObj->osdObj);

	return FVID2_SOK;
}


#if 0
void AlgLink_TracealgPrintInfo(SWOSD_Obj *pSwOsdObj, FVID2_Frame *pFrame)
{
    Vps_printf(" SWOSD: CH%d: VID: addr=0x%X start=%d,%d %dx%d, pitch=%d ; GRPX: start=%d,%d %dx%d, pitch=%d\n",
        pFrame->channelNum,
        pSwOsdObj->videoWindowAddr,
        pSwOsdObj->videoWindowPrm.startX,
        pSwOsdObj->videoWindowPrm.startY,
        pSwOsdObj->videoWindowPrm.width,
        pSwOsdObj->videoWindowPrm.height,
        pSwOsdObj->videoWindowPrm.lineOffset,
        pSwOsdObj->graphicsWindowPrm.startX,
        pSwOsdObj->graphicsWindowPrm.startY,
        pSwOsdObj->graphicsWindowPrm.width,
        pSwOsdObj->graphicsWindowPrm.height,
        pSwOsdObj->graphicsWindowPrm.lineOffset
    );
}

#endif

UInt32 StuSideTrackBackdrop(AlgLink_StuSideTrackObj * pObj, int chid, FVID2_Frame *pFrame)
{

	AlgLink_StuSideTrackCreateParams	*pChParams = &pObj->stusidetrackChCreateParams[chid];

	
	if(pChParams->SaveViewFlag == TRUE)
	{
		if(pChParams->StuSideAddr[0][0] != NULL)
		{
			System_FrameInfo *pFrameInfo = NULL;
			pFrameInfo = (System_FrameInfo*)pFrame->appData;
			UTILS_assert(pFrameInfo != NULL);
			Uint32 nSrcBufWidth  = pFrameInfo->rtChInfo.pitch[0];
			Uint32 nDstBufWidth  = pFrameInfo->rtChInfo.width;
			Uint32 nDstBufHeight = pFrameInfo->rtChInfo.height;
			
			Int8 *pSrc = (Int8 *)pFrame->addr[0][0];
			Int8 *pDst = (Int8 *)pChParams->StuSideAddr[0][0];
			Uint32 i = 0;
			T_Rect rect = {0};
			get_max_rect(&pChParams->StuSideTrackParms, &rect);
			Uint32 		left			= rect.nLeft;
			Uint32 		top				= rect.nTop;
			Uint32 		right			= rect.nRight;
			Uint32 		bottom			= rect.nBottom;
			Uint32      len = (right - left + 1);
			
			if(nDstBufWidth*nDstBufHeight <= 1920*1080)
			{
				for(i = top; i < bottom; i++)
				{
					memcpy(pDst, pSrc + i*nSrcBufWidth + left, len);
					pDst = pDst + len;
				}

				System_linkControl(SYSTEM_LINK_ID_HOST, VSYS_EVENT_STUSIDETRACK_SAVE_VIEW, NULL, 0,TRUE);
			}
		
			
			Vps_printf("Save View : wxh[%dx%d] %p [%d][%d][%d][%d]\n",nDstBufWidth,nDstBufHeight,pChParams->StuSideAddr[0][0],\
			left,right,top,bottom);
		}
		
		pChParams->SaveViewFlag = FALSE;
	}
	
	return 0;
}


Int32 AlgLink_StuSideTrackalgProcessFrame(AlgLink_StuSideTrackObj * pObj, FVID2_Frame *pFrame)
{

    AlgLink_StuSideTrackChObj *pChObj = NULL;
    System_FrameInfo *pFrameInfo = NULL;
	static UInt32 stusidetrackStartTime = 0;
	static UInt32 stusidetrackprocessframes =  0;
	Int32 chid = -1;

  	if(pFrame->channelNum >= ALG_LINK_STUSIDETRACK_MAX_TO_CH)
  	{
		return 0;
	}

	chid = pObj->ChToStream[pFrame->channelNum];

	if(chid >= ALG_LINK_STUSIDETRACK_MAX_CH || chid < 0)
	{
		return 0;
	}

	pFrameInfo = (System_FrameInfo*)pFrame->appData;
    UTILS_assert(pFrameInfo != NULL);
	
	pChObj = &pObj->chObj[chid];
	StuSideITRACK_Params *pParams       = &pChObj->stusidetrackhandle.input_param;
	T_STU_SIDE_GLOBAL_OBJ *ptrackObj    = &pChObj->stusidetrackhandle.t_global_obj;

    
	StuSideTrackBackdrop(pObj, chid, pFrame);

	if(pObj->stusidetrackChCreateParams[chid].StuSideTrackParms.dynamic_param.control_mode != 0)
	{
		return 0;
	}

	if (pFrameInfo->rtChInfoUpdate == TRUE)
	{
		//暂时只支持宽高判断
		if (pFrameInfo->rtChInfo.height != pParams->static_param.video_height)
	    {
	       pChObj->rtParamUpdatePerFrame = TRUE;
	    }
		
	    if (pFrameInfo->rtChInfo.width != pParams->static_param.video_width)
	    {
	        pChObj->rtParamUpdatePerFrame = TRUE;
	    }
	}

	if(pChObj->rtParamUpdatePerFrame)
	{
		 AlgLink_StuSideTrackCreateParams *pChPrm;
		 UInt32 status;
		 if((pFrameInfo->rtChInfo.width  != 704) && (pFrameInfo->rtChInfo.height != 576))
		 {
		 	return 0;	
		 }
		 
		 pChObj->rtParamUpdatePerFrame = FALSE;
		 
		 pChPrm = &pObj->stusidetrackChCreateParams[chid];
		 pChPrm->StuSideTrackParms.static_param.video_width  = pFrameInfo->rtChInfo.width;
		 pChPrm->StuSideTrackParms.static_param.video_height = pFrameInfo->rtChInfo.height;
		 pChPrm->StuSideTrackParms.static_param.pic_width    = pChPrm->StuSideTrackParms.static_param.pic_width;
		 pChPrm->StuSideTrackParms.static_param.pic_height   = pChPrm->StuSideTrackParms.static_param.pic_height;

	     status = AlgLink_StuSideTrackalgSetChTracePrm(pChObj, pChPrm);
   		 UTILS_assert(status == FVID2_SOK); 
		 AlgLink_StuSideTrackalgChInit(pChObj);
		
		 
 		if(pFrame->addr[0][1] > 0)
 		{
 			ptrackObj->nSrcBufWidth  = pFrameInfo->rtChInfo.pitch[0];
 			ptrackObj->nSrcBufHeight = ((Int8*)pFrame->addr[0][1] - (Int8*)pFrame->addr[0][0])/pFrameInfo->rtChInfo.pitch[0];
			Vps_printf("AlgLink_StuSideTrackalgProcessFrame: update nSrcBufWidth nSrcBufHeight [%d] [%d]\n",ptrackObj->nSrcBufWidth,ptrackObj->nSrcBufHeight);
		}

		set_check_max_rect(&pChObj->stusidetrackhandle);
		
		 Vps_printf("AlgLink_StuSideTrackalgProcessFrame: rtParamUpdatePerFrame Done!!!\n");
		 AlgLink_StuSideTrackalgPrintPrm(pChPrm);
	}
	

	if( (pFrameInfo->rtChInfo.width == pParams->static_param.video_width) 
		&& (pFrameInfo->rtChInfo.height ==  pParams->static_param.video_height) 
	)
	{
		AlgLink_StuSideTrackChStatus chStatus = {0};
		chStatus.chId = pChObj->chId;

		if((ptrackObj->nSrcBufWidth != pFrameInfo->rtChInfo.pitch[0]) && (pFrame->addr[0][1] > 0))
		{
			
			ptrackObj->nSrcBufWidth  = pFrameInfo->rtChInfo.pitch[0];
			ptrackObj->nSrcBufHeight = ((Int8*)pFrame->addr[0][1] - (Int8*)pFrame->addr[0][0])/pFrameInfo->rtChInfo.pitch[0];
			Vps_printf("AlgLink_StuSideTrackalgProcessFrame: update nSrcBufWidth nSrcBufHeight [%d] [%d]\n",ptrackObj->nSrcBufWidth,ptrackObj->nSrcBufHeight);
		}

		if(pChObj->StuSideAddr[0][0] != NULL)
		{
			track_students_right_side_process(&pChObj->stusidetrackhandle,(Int8 *)pChObj->StuSideAddr[0][0],(Int8 *)pFrame->addr[0][0], &chStatus.args);
		}
	
  		System_linkControl(SYSTEM_LINK_ID_HOST, VSYS_EVENT_STUSIDETRACK_STATUS, &chStatus, sizeof(AlgLink_StuSideTrackChStatus), TRUE);
	
		stusidetrackprocessframes++;
		if ((Utils_getCurTimeInMsec() - stusidetrackStartTime) > 1 * 1000)
	    {
			stusidetrackStartTime = Utils_getCurTimeInMsec();
			pObj->algstate.frames = stusidetrackprocessframes;
			Vps_printf("StuSideTrack_Process Done %dfps...... \n",stusidetrackprocessframes);
			stusidetrackprocessframes = 0;
		}
	
	
	}

    return 0;
}






