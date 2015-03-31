/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include "studoubletrackLink_priv.h"
#include <mcfw/interfaces/common_def/ti_vsys_common_def.h>
#include "track_students_double_side.h"


Int32 AlgLink_StuDoubleSideTrackalgSetChTracePrm(AlgLink_StuDoubleSideTrackChObj *pChObj,
                            AlgLink_StuDoubleSideTrackCreateParams * params)
{
	Int32 status = 0;
	StuDoubleSideITRACK_Params *pChParams =	&pChObj->studoublesidetrackhandle.input_param;
	T_STU_DOUBLE_SIDE_GLOBAL_OBJ *global =	&pChObj->studoublesidetrackhandle.t_global_obj;
	pChObj->StuDoubleSideAddr[0][0] = params->StuDoubleSideAddr[0][0];
	pChObj->StuDoubleSideAddr[0][1] = params->StuDoubleSideAddr[0][1];
	pChObj->StuDoubleSideAddr[1][0] = params->StuDoubleSideAddr[1][0];
	pChObj->StuDoubleSideAddr[1][1] = params->StuDoubleSideAddr[1][1];

	memset(pChParams, 0x0, sizeof(StuDoubleSideITRACK_Params));
	memcpy(pChParams, &params->StuDoubleSideTrackParms, sizeof(StuDoubleSideITRACK_Params));

	memset(global->gray_buf, 0x0, 704*576);

	Vps_printf("Pic width x height[ %dx%d ] VD width x height[ %dx%d ] Size[%d] %d %d\n",pChParams->static_param.pic_width, pChParams->static_param.pic_height,\
	pChParams->static_param.video_width,pChParams->static_param.video_height,pChParams->static_param.size,\
	params->StuDoubleSideTrackParms.static_param.pic_width,params->StuDoubleSideTrackParms.static_param.pic_height);

	return (status);
}


 void *AlgLink_StuDoubleSideMalloc(int size)
 {
 	Bitstream_Buf buf;
 	Utils_memBitBufAlloc(&buf,size,1);
	UTILS_assert(buf.addr != NULL);

	return buf.addr;
 }


UInt32 AlgLink_StuDoubleSideTrackalgMalloc(AlgLink_StuDoubleSideTrackChObj * pChObj)
{

	StuDoubleSideITRACK_Params  Params;
	StuDoubleSideITRACK_Params *TrackParams  = &Params;
	Bitstream_Buf buf;

	T_STU_DOUBLE_SIDE_GLOBAL_OBJ *pObj =  &pChObj->studoublesidetrackhandle.t_global_obj;
	
	TrackParams->static_param.video_width  = 1280;
	TrackParams->static_param.video_height = 720;

	UInt32 Size0 = TrackParams->static_param.video_width * TrackParams->static_param.video_height;
	//UInt32 Size1 = TrackParams->static_param.pic_width   * TrackParams->static_param.pic_height;

	//status = Utils_memBitBufAlloc(&buf,1920*1080*2,1);
	//pObj->InBuffer = buf.addr;
	//UTILS_assert(pObj->InBuffer != NULL);


	Utils_memBitBufAlloc(&buf,Size0,1);
	pObj->Area_vertex = buf.addr;
	UTILS_assert(pObj->Area_vertex != NULL);

	
	Utils_memBitBufAlloc(&buf,Size0,1);
	pObj->src_y_buf = buf.addr;
	UTILS_assert(pObj->src_y_buf != NULL);

	Utils_memBitBufAlloc(&buf,Size0,1);
	pObj->src_cbcr_buf = buf.addr;
	UTILS_assert(pObj->src_cbcr_buf != NULL);

	Utils_memBitBufAlloc(&buf,Size0,1);
	pObj->tmp_src_y_buf   = buf.addr;
	UTILS_assert(pObj->tmp_src_y_buf != NULL);

	Utils_memBitBufAlloc(&buf,Size0,1);
	pObj->tmp_src_cbcr_buf   = buf.addr;
	UTILS_assert(pObj->tmp_src_cbcr_buf != NULL);

	
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



Int32 AlgLink_StuDoubleSideTrackalgChInit(AlgLink_StuDoubleSideTrackChObj *pChObj)
{

	
	T_STU_DOUBLE_SIDE_GLOBAL_OBJ *pobj = &pChObj->studoublesidetrackhandle.t_global_obj;
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

	track_students_double_side_area_check(&pChObj->studoublesidetrackhandle);
	studoubleside_set_check_max_rect(&pChObj->studoublesidetrackhandle);
	return FVID2_SOK;
}





Int32 AlgLink_StuDoubleSideTrackalgChCreate(AlgLink_StuDoubleSideTrackChObj *pChObj, AlgLink_StuDoubleSideTrackCreateParams *pChPrm)
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
    status = AlgLink_StuDoubleSideTrackalgSetChTracePrm(pChObj, pChPrm);
    UTILS_assert(status == FVID2_SOK);
	Vps_printf("AlgLink_StuDoubleSideTrackalgCreate: AlgLink_StuDoubleSideTrackalgSetChTracePrm Ing\n");

	Vps_printf("AlgLink_StuDoubleSideTrackalgCreate: AlgLink_StuDoubleSideTrackalgMalloc Ing\n");
	status = AlgLink_StuDoubleSideTrackalgMalloc(pChObj);
	UTILS_assert(status == FVID2_SOK);
	
	
	AlgLink_StuDoubleSideTrackalgChInit(pChObj);

	#if 1
	pChObj->studoublesidetrackhandle.pBackGroundModel = BackGroundModel(704,576,5,AlgLink_StuDoubleSideMalloc,NULL,Utils_getCurTimeInMsec);
	UTILS_assert(pChObj->studoublesidetrackhandle.pBackGroundModel != NULL);


	T_Rect rect = {0};
	studoubleside_get_max_rect(&pChObj->studoublesidetrackhandle.input_param, &rect);
	Uint32 		left			= rect.nLeft;
	Uint32 		top				= rect.nTop;
	Uint32 		right			= rect.nRight;
	Uint32 		bottom			= rect.nBottom;
	Uint32      len = (right - left + 1);
	Vps_printf("FirstAnalysisSampleFrame left %d top %d right %d bottom%d\n",left,top,right,bottom);
	FirstAnalysisSampleFrame(pChObj->studoublesidetrackhandle.pBackGroundModel,(Int8 *)pChObj->StuDoubleSideAddr[0][0],(Int8 *)pChObj->StuDoubleSideAddr[0][0]+1280*720,len,left,right,top,bottom);
	#endif
	
	return FVID2_SOK;
}

Int32 AlgLink_StuDoubleSideTrackalgPrintPrm(AlgLink_StuDoubleSideTrackCreateParams *pChPrm)
{

	Int32 i = 0;
	StuDoubleSideITRACK_Params *pTrackParms = &pChPrm->StuDoubleSideTrackParms;
	Vps_printf("StuDoubleSideAddr[%p] size[%d] \npic_width[%d] pic_height[%d] video_width[%d] video_height[%d]\n"\
			   "reset_time[%d] message[%d]   reset_level[%d]\n"\
			    "sens[%d] track_point_num[%d] control_mode[%d]"\
				,pChPrm->StuDoubleSideAddr[0][0],
				pTrackParms->static_param.size,
				pTrackParms->static_param.pic_width,
				pTrackParms->static_param.pic_height ,
				pTrackParms->static_param.video_width,
				pTrackParms->static_param.video_height,
				pTrackParms->dynamic_param.reset_time,
				pTrackParms->dynamic_param.message,
				pTrackParms->dynamic_param.reset_level,
				pTrackParms->dynamic_param.sens,
				pTrackParms->dynamic_param.track_point_num,
				pTrackParms->dynamic_param.control_mode
				);

	Vps_printf("Point [n]  [ x:y ]\n");
	for(i = 0; i < pTrackParms->dynamic_param.track_point_num; i++)
	{
		Vps_printf("     [%2d] [%3d:%3d]\n",i,pTrackParms->dynamic_param.track_point[i].x,pTrackParms->dynamic_param.track_point[i].y);
	}

	return FVID2_SOK;
}



Int32 AlgLink_StuDoubleSideTrackalgCreate(AlgLink_StuDoubleSideTrackObj * pObj)
{
	Int32 status, chId ,numchanel,i;
    AlgLink_StuDoubleSideTrackChObj *pChObj;
    AlgLink_StuDoubleSideTrackCreateParams *pChPrm;

	Vps_printf("AlgLink_StuDoubleSideTrackalgCreate Ing [%d]...\n",pObj->inQueInfo->numCh);



	if(pObj->inQueInfo->numCh > ALG_LINK_STUDOUBLESIDETRACK_MAX_CH)
	{
		numchanel = ALG_LINK_STUDOUBLESIDETRACK_MAX_CH;
	}
	else
	{
		numchanel = pObj->inQueInfo->numCh;
	}


	//各通道对应处理流ID
	for(i = 0; i < ALG_LINK_STUDOUBLESIDETRACK_MAX_CH; i++)
	{
		pObj->StreamId[i] = pObj->studoublesidetrackChCreateParams[i].StreamId;
	}
	

    for(chId=0; chId < numchanel; chId++)
    {

		Vps_printf("Create StuDoubleSide CH[%d] Ing ...\n",chId);
        pChObj = &pObj->chObj[chId];

        pChPrm = &pObj->studoublesidetrackChCreateParams[chId];

        pChObj->chId = chId;
	
		//创建参数初始化,暂时放这里
		AlgLink_StuDoubleSideTrackalgPrintPrm(pChPrm);
		status	= AlgLink_StuDoubleSideTrackalgChCreate(pChObj,pChPrm);
		UTILS_assert(status == FVID2_SOK);
    }

	return FVID2_SOK;
}


Int32 AlgLink_StuDoubleSideTracealgDelete(AlgLink_StuDoubleSideTrackObj * pObj)
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


UInt32 StuDoubleSideTrackBackdrop(AlgLink_StuDoubleSideTrackObj * pObj, int chid, FVID2_Frame *pFrame)
{

	AlgLink_StuDoubleSideTrackCreateParams	*pChParams = &pObj->studoublesidetrackChCreateParams[chid];
	AlgLink_StuDoubleSideTrackChObj *pChobj = &pObj->chObj[chid];

	
	if(pChParams->SaveView.SaveViewFlag == TRUE)
	{
		Vps_printf("enableSTUDOUBLESIDETRACKAlg 11111111\n");
		if(pChParams->StuDoubleSideAddr[0][0] != NULL)
		{
			System_FrameInfo *pFrameInfo = NULL;
			pFrameInfo = (System_FrameInfo*)pFrame->appData;
			UTILS_assert(pFrameInfo != NULL);
			Uint32 nSrcBufWidth  = pFrameInfo->rtChInfo.pitch[0];
			Uint32 nSrcBufHeight = ((Int8*)pFrame->addr[0][1] - (Int8*)pFrame->addr[0][0])/pFrameInfo->rtChInfo.pitch[0];
			Uint32 nDstBufWidth  = pFrameInfo->rtChInfo.width;
			Uint32 nDstBufHeight = pFrameInfo->rtChInfo.height;
			
			Int8 *pSrc = (Int8 *)pFrame->addr[0][0];		
			Uint32 i = 0;
			T_Rect rect = {0};
			studoubleside_get_max_rect(&pChParams->StuDoubleSideTrackParms, &rect);

			Uint32 		left			= rect.nLeft;
			Uint32 		top				= rect.nTop;
			Uint32 		right			= rect.nRight;
			Uint32 		bottom			= rect.nBottom;
			Uint32      index = 0;
			Uint32      len = (right - left + 1);

			if(nDstBufWidth*nDstBufHeight <= 1920*1080)
			{
				if(pChParams->SaveView.SaveType == 1)
				{
					Int8 *pDst = (Int8 *)pChParams->StuDoubleSideAddr[0][0];
					for(i = top; i < bottom; i++)
					{
						memcpy(pDst, pSrc + i*nSrcBufWidth + left, len);
						pDst = pDst + len;
					}

					pDst = (Int8 *)pChParams->StuDoubleSideAddr[0][0] + 1280*720;
					for(i = top; i < bottom; i = i+2)
					{
						memcpy(pDst, pSrc + nSrcBufWidth*nSrcBufHeight + i/2*nSrcBufWidth + left/2*2, len);
						pDst = pDst + len;
					}

					AnalysisSampleFrame(pChobj->studoublesidetrackhandle.pBackGroundModel,(Int8 *)pFrame->addr[0][0],(Int8*)pFrame->addr[0][1],nSrcBufWidth,left,right,top,bottom);					
					
					Cache_wbInv(pChParams->StuDoubleSideAddr[0][0],1280*720*2,Cache_Type_ALL,TRUE);
					AlgLink_StuDoubleSideTrackSaveView State;
					State.chId = chid;
					System_linkControl(SYSTEM_LINK_ID_HOST, VSYS_EVENT_STUDOUBLESIDETRACK_SAVE_VIEW, &State, sizeof(AlgLink_StuDoubleSideTrackSaveView),TRUE);
				}
				else if(pChParams->SaveView.SaveType == 2)
				{
					for(index = 0; index < TRIGGER_POINT_NUM; index++)
					{

						Int8 *pDst = (Int8 *)pChParams->StuDoubleSideAddr[0][0];
						Uint32 triggerlen = pChParams->SaveView.g_trigger_info[index].x1 - pChParams->SaveView.g_trigger_info[index].x0 + 1;

						if(1 == pChParams->SaveView.g_trigger_info[index].trigger_flag)
						{
							
							if(pChParams->SaveView.g_trigger_info[index].x0 < left)
							{
								pChParams->SaveView.g_trigger_info[index].x0  = left;
							}

							if(pChParams->SaveView.g_trigger_info[index].x1 > right)
							{
								pChParams->SaveView.g_trigger_info[index].x1  = right;
							}

							if(pChParams->SaveView.g_trigger_info[index].y0 < top)
							{
								pChParams->SaveView.g_trigger_info[index].y0 = top;
							}

							if(pChParams->SaveView.g_trigger_info[index].y1 > bottom)
							{
								pChParams->SaveView.g_trigger_info[index].y1 = bottom;
							}
							
							pDst = pDst + (pChParams->SaveView.g_trigger_info[index].y0 - top)*len;
							for(i = pChParams->SaveView.g_trigger_info[index].y0; i <= pChParams->SaveView.g_trigger_info[index].y1; i++)
							{
								memcpy(pDst + pChParams->SaveView.g_trigger_info[index].x0 - left, pSrc + i*nSrcBufWidth + pChParams->SaveView.g_trigger_info[index].x0, triggerlen);
								pDst = pDst + len;
							}
						

							pDst = (Int8 *)pChParams->StuDoubleSideAddr[0][0] + 1280*720 + (pChParams->SaveView.g_trigger_info[index].y0 - top)/2*len;

							for(i = pChParams->SaveView.g_trigger_info[index].y0; i <= pChParams->SaveView.g_trigger_info[index].y1; i = i+2)
							{
								memcpy(pDst + (pChParams->SaveView.g_trigger_info[index].x0 - left)/2*2, pSrc + nSrcBufWidth*nSrcBufHeight + i/2*nSrcBufWidth + pChParams->SaveView.g_trigger_info[index].x0/2*2, triggerlen);
								pDst = pDst + len;
							}
							
						}
					}

					Cache_wbInv(pChParams->StuDoubleSideAddr[0][0],1280*720*2,Cache_Type_ALL,TRUE);
				}
				else if(pChParams->SaveView.SaveType == 3)
				{
					Int8 *pDst = (Int8 *)pChParams->StuDoubleSideAddr[0][0];
					for(i = top; i < bottom; i++)
					{
						memcpy(pDst, pSrc + i*nSrcBufWidth + left, len);
						pDst = pDst + len;
					}

	
					pDst = (Int8 *)pChParams->StuDoubleSideAddr[0][0] + 1280*720;
					for(i = top; i < bottom; i = i+2)
					{
						memcpy(pDst, pSrc + nSrcBufWidth*nSrcBufHeight + i/2*nSrcBufWidth + left/2*2, len);
						pDst = pDst + len;
					}
					Cache_wbInv(pChParams->StuDoubleSideAddr[0][0],1280*720*2,Cache_Type_ALL,TRUE);
				}

			}
		
			
			Vps_printf("Save StuDoubleSide%dView : wxh[%dx%d] %p [%d][%d][%d][%d] SaveType[%d]\n",chid,nDstBufWidth,nDstBufHeight,pChParams->StuDoubleSideAddr[0][0],\
			left,right,top,bottom,pChParams->SaveView.SaveType);
		}
		
		pChParams->SaveView.SaveViewFlag = FALSE;
		pChParams->SaveView.SaveType = 0;

		Vps_printf("enableSTUDOUBLESIDETRACKAlg 2222222\n");
	}
	
	return 0;
}

Int32 AlgLink_StuDoubleSideTrackalgProcessFrame(AlgLink_StuDoubleSideTrackObj * pObj, FVID2_Frame *pFrame)
{

    AlgLink_StuDoubleSideTrackChObj *pChObj = NULL;
    System_FrameInfo *pFrameInfo = NULL;
	static UInt32 studoublesidetrackStartTime[6] = {0};
	static UInt32 studoublesidetrackprocessframes[6] =  {0};
	Int32 chid = -1;
	Int32 i = 0;
	for(i = 0; i < ALG_LINK_STUDOUBLESIDETRACK_MAX_CH; i++)
  	{
		if(pObj->StreamId[i] == pFrame->channelNum)
		{
			chid = i;
			break;
		}
	}

	if(chid < 0 || chid >= ALG_LINK_STUDOUBLESIDETRACK_MAX_CH)
	{
		return 0;
	}

	pFrameInfo = (System_FrameInfo*)pFrame->appData;
    UTILS_assert(pFrameInfo != NULL);
	
	pChObj = &pObj->chObj[chid];
	StuDoubleSideITRACK_Params *pParams        = &pChObj->studoublesidetrackhandle.input_param;
	T_STU_DOUBLE_SIDE_GLOBAL_OBJ *ptrackObj    = &pChObj->studoublesidetrackhandle.t_global_obj;


	
	StuDoubleSideTrackBackdrop(pObj, chid, pFrame);

	if(pObj->studoublesidetrackChCreateParams[chid].StuDoubleSideTrackParms.dynamic_param.control_mode != 0 || 
	   pObj->studoublesidetrackChCreateParams[chid].StuDoubleSideTrackParms.dynamic_param.size == 1)
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
		#if 0
	    if (pFrameInfo->rtChInfo.pitch[0] != rtChannelInfo->pitch[0])
	    {
	        rtChannelInfo->pitch[0] = pFrameInfo->rtChInfo.pitch[0];
	        rtParamUpdatePerFrame = TRUE;
	    }
		#endif
	}

	if(pChObj->rtParamUpdatePerFrame)
	{
		 AlgLink_StuDoubleSideTrackCreateParams *pChPrm;
		 UInt32 status;
		 if(pFrameInfo->rtChInfo.width != 704 || pFrameInfo->rtChInfo.height != 576)
		 {
		 	return 0;
		 }

		 pChObj->rtParamUpdatePerFrame = FALSE;
		 
		 pChPrm = &pObj->studoublesidetrackChCreateParams[chid];
		//pChPrm->StuDoubleSideTrackParms.static_param.video_width  = pFrameInfo->rtChInfo.width;
		 //pChPrm->StuDoubleSideTrackParms.static_param.video_height = pFrameInfo->rtChInfo.height;
		 //pChPrm->StuDoubleSideTrackParms.static_param.pic_width    = pChPrm->StuDoubleSideTrackParms.static_param.pic_width;
		 //pChPrm->StuDoubleSideTrackParms.static_param.pic_height   = pChPrm->StuDoubleSideTrackParms.static_param.pic_height;

	     status = AlgLink_StuDoubleSideTrackalgSetChTracePrm(pChObj, pChPrm);
   		 UTILS_assert(status == FVID2_SOK); 

		 AlgLink_StuDoubleSideTrackalgChInit(pChObj);
	
		if(pFrame->addr[0][1] > 0)
		{
			ptrackObj->nSrcBufWidth  = pFrameInfo->rtChInfo.pitch[0];
			ptrackObj->nSrcBufHeight = ((Int8*)pFrame->addr[0][1] - (Int8*)pFrame->addr[0][0])/pFrameInfo->rtChInfo.pitch[0];
			Vps_printf("AlgLink_StuDoubleSideTrackalgProcessFrame[%d]: update nSrcBufWidth nSrcBufHeight [%d] [%d]\n",chid,ptrackObj->nSrcBufWidth,ptrackObj->nSrcBufHeight);
		}

		 studoubleside_set_check_max_rect(&pChObj->studoublesidetrackhandle);
		
		 Vps_printf("AlgLink_StuDoubleSideTrackalgProcessFrame[%d]: rtParamUpdatePerFrame Done!!!\n",chid);
		 AlgLink_StuDoubleSideTrackalgPrintPrm(pChPrm);
	}
	

	#if 1
	if( (pFrameInfo->rtChInfo.width == pParams->static_param.video_width) 
		&& (pFrameInfo->rtChInfo.height ==  pParams->static_param.video_height) 
	)
	{
			//Vps_printf("AlgLink_StuTrackalgProcessFrame: ch[ %d ] [%p] Src[%dx%d] [%d][%d] [%p] [%p] [%d]\n",pFrame->channelNum,(UInt8 *)pFrame->addr[0][0],pFrameInfo->rtChInfo.width,pFrameInfo->rtChInfo.height,\
				//pFrameInfo->rtChInfo.pitch[0],pFrameInfo->rtChInfo.pitch[1],pFrame->addr[0][0],pFrame->addr[0][1],pFrameInfo->rtChInfo.dataFormat);

		AlgLink_StuDoubleSideTrackChStatus chStatus = {0};
		chStatus.chId = pChObj->chId;

		if((ptrackObj->nSrcBufWidth != pFrameInfo->rtChInfo.pitch[0]) && (pFrame->addr[0][1] > 0))
		{
			
			ptrackObj->nSrcBufWidth  = pFrameInfo->rtChInfo.pitch[0];
			ptrackObj->nSrcBufHeight = ((Int8*)pFrame->addr[0][1] - (Int8*)pFrame->addr[0][0])/pFrameInfo->rtChInfo.pitch[0];
			Vps_printf("AlgLink_StuDoubleSideTrackalgProcessFrame[%d]: update nSrcBufWidth nSrcBufHeight [%d] [%d]\n",chid,ptrackObj->nSrcBufWidth,ptrackObj->nSrcBufHeight);
		}

		if(pChObj->StuDoubleSideAddr[0][0] != NULL)
		{	

			//UpdateBackground(pChObj->pBackGroundModel, (Int8 *)pFrame->addr[0][0], ptrackObj->nSrcBufWidth,NULL,0,30,2,50);
			//Classification(pChObj->pBackGroundModel, (Int8 *)pFrame->addr[0][0], pFrameInfo->rtChInfo.width, pFrameInfo->rtChInfo.height, ptrackObj->nSrcBufWidth);

		//	memset(pFrame->addr[0][1],0x80,1920*540);
			//Cache_wbInv((Int8 *)pFrame->addr[0][1],1920*540,Cache_Type_ALL,TRUE);

			track_students_double_side_process(&pChObj->studoublesidetrackhandle,(Int8 *)pChObj->StuDoubleSideAddr[0][0],(Int8 *)pFrame->addr[0][0], &chStatus.args);
		}
		//Vps_printf("SrcWidth[%d] SrcHeight[%d]\n",ptrackObj->nSrcBufWidth,ptrackObj->nSrcBufHeight);

	
  		System_linkControl(SYSTEM_LINK_ID_HOST, VSYS_EVENT_STUDOUBLESIDETRACK_STATUS, &chStatus, sizeof(AlgLink_StuDoubleSideTrackChStatus), TRUE);
	
		studoublesidetrackprocessframes[chid]++;
		if ((Utils_getCurTimeInMsec() - studoublesidetrackStartTime[chid]) > 1 * 1000)
	    {
			studoublesidetrackStartTime[chid] = Utils_getCurTimeInMsec();
			pObj->algstate.frames = studoublesidetrackprocessframes[chid];
			Vps_printf("StuDoubleSideTrack_Process[%d] Done %dfps...... \n",chid,studoublesidetrackprocessframes[chid]);
			studoublesidetrackprocessframes[chid] = 0;
		}
	
	
	}
	#endif
    return 0;
}


