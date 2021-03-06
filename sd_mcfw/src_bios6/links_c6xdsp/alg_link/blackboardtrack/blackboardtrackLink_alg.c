/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include "blackboardtrackLink_priv.h"
#include <mcfw/interfaces/common_def/ti_vsys_common_def.h>
#include "blackboardtrack.h"


Int32 AlgLink_BlackboardTrackalgSetChTracePrm(AlgLink_BlackboardTrackChObj *pChObj,
                            AlgLink_BlackboardTrackCreateParams * params)
{
	Int32 status = 0;
	BlackboardTrack_Params *pChParams =	&pChObj->blackboardtrackhandle.input_param;
	T_BLACKBOARD_TRACK_GLOBAL_OBJ *global =	&pChObj->blackboardtrackhandle.t_global_obj;

	
	pChObj->BlackboardTrackAddr[0][0] = params->BlackboardTrackAddr[0][0];
	pChObj->BlackboardTrackAddr[0][1] = params->BlackboardTrackAddr[0][1];
	pChObj->BlackboardTrackAddr[1][0] = params->BlackboardTrackAddr[1][0];
	pChObj->BlackboardTrackAddr[1][1] = params->BlackboardTrackAddr[1][1];

	memset(pChParams, 0x0, sizeof(BlackboardTrack_Params));
	memcpy(pChParams, &params->BlackboardTrackParms, sizeof(BlackboardTrack_Params));

	memset(global->gray_buf, 0x0, 704*576);


	Vps_printf("AlgLink_BlackboardTrackalgSetChTracePrm Pic width x height[ %dx%d ] VD width x height[ %dx%d ] Size[%d] %d %d\n",pChParams->static_param.pic_width, pChParams->static_param.pic_height,\
	pChParams->static_param.video_width,pChParams->static_param.video_height,pChParams->static_param.size,\
	params->BlackboardTrackParms.static_param.pic_width,params->BlackboardTrackParms.static_param.pic_height);


	return (status);
}





UInt32 AlgLink_BlackboardTrackalgMalloc(AlgLink_BlackboardTrackChObj * pChObj)
{

	BlackboardTrack_Params  Params;
	BlackboardTrack_Params *TrackParams  = &Params;
	Bitstream_Buf buf;
	UInt32 status;

	T_BLACKBOARD_TRACK_GLOBAL_OBJ *pObj =  &pChObj->blackboardtrackhandle.t_global_obj;
	
	TrackParams->static_param.video_width  = 1280;
	TrackParams->static_param.video_height = 720;
	TrackParams->static_param.pic_width    = 480;
	TrackParams->static_param.pic_height   = 270;

	UInt32 Size0 = TrackParams->static_param.video_width * TrackParams->static_param.video_height;
	//UInt32 Size1 = TrackParams->static_param.pic_width   * TrackParams->static_param.pic_height;

	//status = Utils_memBitBufAlloc(&buf,1920*1080*2,1);
	//pObj->InBuffer = buf.addr;
	//UTILS_assert(pObj->InBuffer != NULL);


	status = Utils_memBitBufAlloc(&buf,Size0,1);
	pObj->Area_vertex = buf.addr;
	UTILS_assert(pObj->Area_vertex != NULL);

	
	status = Utils_memBitBufAlloc(&buf,Size0,1);
	pObj->src_y_buf = buf.addr;
	UTILS_assert(pObj->src_y_buf != NULL);

	status = Utils_memBitBufAlloc(&buf,Size0,1);
	pObj->src_cbcr_buf = buf.addr;
	UTILS_assert(pObj->src_cbcr_buf != NULL);

	status = Utils_memBitBufAlloc(&buf,Size0,1);
	pObj->tmp_src_y_buf   = buf.addr;
	UTILS_assert(pObj->tmp_src_y_buf != NULL);

	status = Utils_memBitBufAlloc(&buf,Size0,1);
	pObj->tmp_src_cbcr_buf   = buf.addr;
	UTILS_assert(pObj->tmp_src_cbcr_buf != NULL);

	
	status = Utils_memBitBufAlloc(&buf,Size0,1);
	pObj->gray_buf = buf.addr;
	UTILS_assert(pObj->gray_buf != NULL);

	
	status = Utils_memBitBufAlloc(&buf,Size0,1);
	pObj->mid_buf   = buf.addr;
	UTILS_assert(pObj->mid_buf != NULL);
	
	status = Utils_memBitBufAlloc(&buf,Size0,1);
	pObj->marked_buf  = buf.addr;
	UTILS_assert(pObj->marked_buf != NULL);


	status = Utils_memBitBufAlloc(&buf,Size0,1);
	pObj->pre_frame  = buf.addr;
	UTILS_assert(pObj->pre_frame != NULL);
		
	
	return FVID2_SOK;
}



Int32 AlgLink_BlackboardTrackalgChInit(AlgLink_BlackboardTrackChObj *pChObj)
{

	
	T_BLACKBOARD_TRACK_GLOBAL_OBJ *pobj = &pChObj->blackboardtrackhandle.t_global_obj;
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

	blackboard_track_area_check(&pChObj->blackboardtrackhandle);
	blackboard_track_set_check_max_rect(&pChObj->blackboardtrackhandle);
	return FVID2_SOK;
}





Int32 AlgLink_BlackboardTrackalgChCreate(AlgLink_BlackboardTrackChObj *pChObj, AlgLink_BlackboardTrackCreateParams *pChPrm)
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
    status = AlgLink_BlackboardTrackalgSetChTracePrm(pChObj, pChPrm);
    UTILS_assert(status == FVID2_SOK);
	Vps_printf("AlgLink_BlackboardTrackalgChCreate: AlgLink_BlackboardTrackalgSetChTracePrm Ing\n");

	Vps_printf("AlgLink_BlackboardTrackalgChCreate: AlgLink_BlackboardTrackalgMalloc Ing\n");
	status = AlgLink_BlackboardTrackalgMalloc(pChObj);
	UTILS_assert(status == FVID2_SOK);
	
	
	AlgLink_BlackboardTrackalgChInit(pChObj);

	
	return FVID2_SOK;
}

Int32 AlgLink_BlackboardTrackalgPrintPrm(AlgLink_BlackboardTrackCreateParams *pChPrm)
{

	Int32 i = 0;
	BlackboardTrack_Params *pTrackParms = &pChPrm->BlackboardTrackParms;
	Vps_printf("BlackboardTrackAddr[%p] size[%d] \npic_width[%d] pic_height[%d] video_width[%d] video_height[%d]\n"\
			   "reset_time[%d] message[%d]   reset_level[%d]\n"\
			    "sens[%d] track_point_num[%d] control_mode[%d]"\
			    "trigger [%d:%d] [%d:%d]"\
			    "track_mode[%d]"\
				,pChPrm->BlackboardTrackAddr[0][0],
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
				pTrackParms->dynamic_param.control_mode,
				pTrackParms->dynamic_param.trigger[0].trigger_x0,
				pTrackParms->dynamic_param.trigger[0].trigger_y0,
				pTrackParms->dynamic_param.trigger[0].trigger_x1,
				pTrackParms->dynamic_param.trigger[0].trigger_y1,
				pTrackParms->dynamic_param.track_mode
				);

	Vps_printf("Point [n]  [ x:y ]\n");
	for(i = 0; i < pTrackParms->dynamic_param.track_point_num; i++)
	{
		Vps_printf("     [%2d] [%3d:%3d]\n",i,pTrackParms->dynamic_param.track_point[i].x,pTrackParms->dynamic_param.track_point[i].y);
	}

	return FVID2_SOK;
}



Int32 AlgLink_BlackboardTrackalgCreate(AlgLink_BlackboardTrackObj * pObj)
{
	Int32 status, chId ,numchanel,i;
    AlgLink_BlackboardTrackChObj *pChObj;
    AlgLink_BlackboardTrackCreateParams *pChPrm;

	Vps_printf("AlgLink_BlackboardTrackalgCreate Ing [%d]...\n",pObj->inQueInfo->numCh);



	if(pObj->inQueInfo->numCh > ALG_LINK_BLACKBOARDTRACK_MAX_CH)
	{
		numchanel = ALG_LINK_BLACKBOARDTRACK_MAX_CH;
	}
	else
	{
		numchanel = pObj->inQueInfo->numCh;
	}


	//各通道对应处理流ID
	for(i = 0; i < ALG_LINK_BLACKBOARDTRACK_MAX_CH; i++)
	{
		pObj->StreamId[i] = pObj->blackboardtrackChCreateParams[i].StreamId;
	}
	

    for(chId=0; chId < numchanel; chId++)
    {

		Vps_printf("AlgLink_BlackboardTrackalgCreate CH[%d] Ing ...\n",chId);
        pChObj = &pObj->chObj[chId];

        pChPrm = &pObj->blackboardtrackChCreateParams[chId];

        pChObj->chId = chId;
	
		//创建参数初始化,暂时放这里
		AlgLink_BlackboardTrackalgPrintPrm(pChPrm);
		status	= AlgLink_BlackboardTrackalgChCreate(pChObj,pChPrm);
		UTILS_assert(status == FVID2_SOK);
    }

	return FVID2_SOK;
}


Int32 AlgLink_BlackboardTracealgDelete(AlgLink_BlackboardTrackObj * pObj)
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
#if 0
UInt32 CeilTrackBackdrop(AlgLink_BlackboardTrackObj *pObj, Int chid, FVID2_Frame *pFrame)
{

	AlgLink_BlackboardTrackCreateParams	*pChParams = &pObj->blackboardtrackChCreateParams[chid];

	if(pChParams->SaveView.SaveViewFlag == TRUE)
	{
		Vps_printf("enableCEILTRACKAlg 11111111111111111111111\n");
		if(pChParams->BlackboardTrackAddr[0][0] != NULL)
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
			ceil_track_get_max_rect(&pChParams->BlackboardTrackParms, &rect);

			Uint32 		left			= rect.nLeft;
			Uint32 		top				= rect.nTop;
			Uint32 		right			= rect.nRight;
			Uint32 		bottom			= rect.nBottom;
			Uint32      index = 0;
			Uint32      len = (right - left + 1);


			Vps_printf("%d %d %d %d\n",left,top,right,bottom);
			if(nDstBufWidth*nDstBufHeight <= 1920*1080)
			{
				if(pChParams->SaveView.SaveType == 1)
				{
					Int8 *pDst = (Int8 *)pChParams->BlackboardTrackAddr[0][0];
					for(i = top; i < bottom; i++)
					{
						memcpy(pDst, pSrc + i*nSrcBufWidth + left, len);
						pDst = pDst + len;
					}

					pDst = (Int8 *)pChParams->BlackboardTrackAddr[0][0] + 1280*720;
					for(i = top; i < bottom; i = i+2)
					{
						memcpy(pDst, pSrc + nSrcBufWidth*nSrcBufHeight + i/2*nSrcBufWidth + left/2*2, len);
						pDst = pDst + len;
					}

					Cache_wbInv(pChParams->BlackboardTrackAddr[0][0],1280*720*2,Cache_Type_ALL,TRUE);
					
					AlgLink_CeilTrackSaveView State;
					State.chId = chid;
					System_linkControl(SYSTEM_LINK_ID_HOST, VSYS_EVENT_CEILTRACK_SAVE_VIEW, &State, sizeof(AlgLink_CeilTrackSaveView),TRUE);
				}
				
				else if(pChParams->SaveView.SaveType == 2)
				{
					for(index = 0; index < TRIGGER_POINT_NUM; index++)
					{

						Int8 *pDst = (Int8 *)pChParams->BlackboardTrackAddr[0][0];
						Uint32 triggerlen = pChParams->SaveView.g_trigger_info[index].x1 - pChParams->SaveView.g_trigger_info[index].x0 + 1;

						if(1 == pChParams->SaveView.g_trigger_info[index].trigger_flag)
						{

							Vps_printf("left %d right %d top %d bottom %d x0 %d x1 %d y0 %d y1 %d\n",left,right,top,bottom,
								pChParams->SaveView.g_trigger_info[index].x0,pChParams->SaveView.g_trigger_info[index].x1,
								pChParams->SaveView.g_trigger_info[index].y0,pChParams->SaveView.g_trigger_info[index].y1);
							
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
						

							pDst = (Int8 *)pChParams->BlackboardTrackAddr[0][0] + 1280*720 + (pChParams->SaveView.g_trigger_info[index].y0 - top)/2*len;

							for(i = pChParams->SaveView.g_trigger_info[index].y0; i <= pChParams->SaveView.g_trigger_info[index].y1; i = i+2)
							{
								memcpy(pDst + (pChParams->SaveView.g_trigger_info[index].x0 - left)/2*2, pSrc + nSrcBufWidth*nSrcBufHeight + i/2*nSrcBufWidth + pChParams->SaveView.g_trigger_info[index].x0/2*2, triggerlen);
								pDst = pDst + len;
							}
							
						}
					}

					Cache_wbInv(pChParams->BlackboardTrackAddr[0][0],1280*720*2,Cache_Type_ALL,TRUE);
				}
				else if(pChParams->SaveView.SaveType == 3)
				{
					Int8 *pDst = (Int8 *)pChParams->BlackboardTrackAddr[0][0];
					for(i = top; i < bottom; i++)
					{
						memcpy(pDst, pSrc + i*nSrcBufWidth + left, len);
						pDst = pDst + len;
					}

	
					pDst = (Int8 *)pChParams->BlackboardTrackAddr[0][0] + 1280*720;
					for(i = top; i < bottom; i = i+2)
					{
						memcpy(pDst, pSrc + nSrcBufWidth*nSrcBufHeight + i/2*nSrcBufWidth + left/2*2, len);
						pDst = pDst + len;
					}
					Cache_wbInv(pChParams->BlackboardTrackAddr[0][0],1280*720*2,Cache_Type_ALL,TRUE);
				}

			}
		
			
			Vps_printf("Save CeilTrack%dView : wxh[%dx%d] %p [%d][%d][%d][%d] SaveType[%d]\n",pChParams->chId,nDstBufWidth,nDstBufHeight,pChParams->BlackboardTrackAddr[0][0],\
			left,right,top,bottom,pChParams->SaveView.SaveType);
		}
		Vps_printf("enableCEILTRACKAlg 22222222\n");
		pChParams->SaveView.SaveViewFlag = FALSE;
		pChParams->SaveView.SaveType = 0;
	}


	return 0;
}
#endif

Int32 AlgLink_BlackboardTrackalgProcessFrame(AlgLink_BlackboardTrackObj * pObj, FVID2_Frame *pFrame)
{

    AlgLink_BlackboardTrackChObj *pChObj = NULL;
    System_FrameInfo *pFrameInfo = NULL;
	static UInt32 blackboardtrackStartTime = 0;
	static UInt32 blackboardtrackprocessframes[6] =  {0};
	Int32 chid = -1;
	Int32 i = 0;

	for(i = 0; i < ALG_LINK_BLACKBOARDTRACK_MAX_CH; i++)
  	{
		if(pObj->StreamId[i] == pFrame->channelNum)
		{
			chid = i;
			break;
		}
	}

	if(chid < 0 || chid >= ALG_LINK_BLACKBOARDTRACK_MAX_CH)
	{
		return 0;
	}

	pFrameInfo = (System_FrameInfo*)pFrame->appData;
    UTILS_assert(pFrameInfo != NULL);

	
	pChObj = &pObj->chObj[chid];
	BlackboardTrack_Params *pParams			  =	&pChObj->blackboardtrackhandle.input_param;
	T_BLACKBOARD_TRACK_GLOBAL_OBJ *ptrackObj    =	&pChObj->blackboardtrackhandle.t_global_obj;

	//CeilTrackBackdrop(pObj, chid, pFrame);

	if(pObj->blackboardtrackChCreateParams[chid].BlackboardTrackParms.dynamic_param.control_mode != 0)
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
		 AlgLink_BlackboardTrackCreateParams *pChPrm;
		 UInt32 status;
		 if(pFrameInfo->rtChInfo.width  != 704 || pFrameInfo->rtChInfo.height != 576)
		 {
		 	return 0;
		 }
		
		 pChObj->rtParamUpdatePerFrame = FALSE;
		 
		 pChPrm = &pObj->blackboardtrackChCreateParams[chid];
	     status = AlgLink_BlackboardTrackalgSetChTracePrm(pChObj, pChPrm);
   		 UTILS_assert(status == FVID2_SOK); 

		 AlgLink_BlackboardTrackalgChInit(pChObj);
	
		if(pFrame->addr[0][1] > 0)
		{
			ptrackObj->nSrcBufWidth  = pFrameInfo->rtChInfo.pitch[0];
			ptrackObj->nSrcBufHeight = ((Int8*)pFrame->addr[0][1] - (Int8*)pFrame->addr[0][0])/pFrameInfo->rtChInfo.pitch[0];
			Vps_printf("AlgLink_BlackboardTrackalgProcessFrame[%d]: update nSrcBufWidth nSrcBufHeight [%d] [%d]\n",chid,ptrackObj->nSrcBufWidth,ptrackObj->nSrcBufHeight);
		}

		 blackboard_track_set_check_max_rect(&pChObj->blackboardtrackhandle);
		
		 Vps_printf("AlgLink_BlackboardTrackalgProcessFrame[%d]: rtParamUpdatePerFrame Done!!!\n",chid);
		 AlgLink_BlackboardTrackalgPrintPrm(pChPrm);
	}
	
	if( (pFrameInfo->rtChInfo.width == pParams->static_param.video_width) 
		&& (pFrameInfo->rtChInfo.height ==  pParams->static_param.video_height) 
	)
	{
		AlgLink_BlackboardTrackChStatus chStatus = {0};
		chStatus.chId = pChObj->chId;

		if((ptrackObj->nSrcBufWidth != pFrameInfo->rtChInfo.pitch[0]) && (pFrame->addr[0][1] > 0))
		{
			ptrackObj->nSrcBufWidth  = pFrameInfo->rtChInfo.pitch[0];
			ptrackObj->nSrcBufHeight = ((Int8*)pFrame->addr[0][1] - (Int8*)pFrame->addr[0][0])/pFrameInfo->rtChInfo.pitch[0];
			Vps_printf("AlgLink_BlackboardTrackalgProcessFrame[%d]: update nSrcBufWidth nSrcBufHeight [%d] [%d]\n",chid,ptrackObj->nSrcBufWidth,ptrackObj->nSrcBufHeight);
		}

		if(pChObj->BlackboardTrackAddr[0][0] != NULL)
		{
			blackboard_track_process(&pChObj->blackboardtrackhandle,(Int8 *)pChObj->BlackboardTrackAddr[0][0],(Int8 *)pFrame->addr[0][0], &chStatus.args);
		}
	
  		System_linkControl(SYSTEM_LINK_ID_HOST, VSYS_EVENT_BLACKBOARD_TRACK_STATUS, &chStatus, sizeof(AlgLink_BlackboardTrackChStatus), TRUE);
	
		blackboardtrackprocessframes[chid]++;
		if ((Utils_getCurTimeInMsec() - blackboardtrackStartTime) > 1 * 1000)
	    {
			blackboardtrackStartTime = Utils_getCurTimeInMsec();
			pObj->algstate.frames = blackboardtrackprocessframes[chid];
			Vps_printf("AlgLink_BlackboardTrackalgProcessFrame[%d] Done %dfps...... \n",chid,blackboardtrackprocessframes[chid]);
			blackboardtrackprocessframes[chid] = 0;
		}
	
	
	}
    return 0;
}


