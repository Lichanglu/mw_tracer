/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include "trackLink_priv.h"
#include <mcfw/interfaces/common_def/ti_vsys_common_def.h>
#include "track.h"

Int32 AlgLink_TrackalgSetChTracePrm(AlgLink_TrackChObj *pChObj,
                            AlgLink_TrackCreateParams * params)
{
	Int32 status = 0;
	ITRACK_Params *pParams = &pChObj->trackhandle.input_param;
	T_GLOBAL_OBJ *pObj = &pChObj->trackhandle.t_global_obj;
	memcpy(pParams, &params->TrackParms, sizeof(ITRACK_Params));


	pChObj->InBufferAddr[0][0] = params->InBufferAddr[0][0];
	pChObj->InBufferAddr[0][1] = params->InBufferAddr[0][1];
	pChObj->InBufferAddr[1][0] = params->InBufferAddr[1][0];
	pChObj->InBufferAddr[1][1] = params->InBufferAddr[1][1];


	pObj->m_offset = params->TrackParms.dynamic_param.mid_x;
	pObj->n_offset = params->TrackParms.dynamic_param.mid_y;
	Vps_printf("Pic width x height[ %dx%d ] VD width x height[ %dx%d ] Size[%d] m[%d] n[%d]\n",pParams->static_param.pic_width, pParams->static_param.pic_height,\
	pParams->static_param.video_width,pParams->static_param.video_height,pParams->static_param.size,pObj->m_offset,pObj->n_offset);

	return (status);
}





UInt32 AlgLink_TrackalgMalloc(AlgLink_TrackChObj * pChObj)
{

	ITRACK_Params  Params;
	ITRACK_Params *TrackParams  = &Params;
	Bitstream_Buf buf;
	UInt32 status;
	T_GLOBAL_OBJ *pObj = &pChObj->trackhandle.t_global_obj;

	TrackParams->static_param.video_width  = 1920;
	TrackParams->static_param.video_height = 1080;
	TrackParams->static_param.pic_width    = 480;
	TrackParams->static_param.pic_height   = 270;

	UInt32 Size0 = TrackParams->static_param.video_width * TrackParams->static_param.video_height;
	UInt32 Size1 = TrackParams->static_param.pic_width   * TrackParams->static_param.pic_height;

	#if 1
	status = Utils_memBitBufAlloc(&buf,Size0,1);
	pObj->Area_vertex = buf.addr;
	UTILS_assert(pObj->Area_vertex != NULL);
	Vps_printf("---------1------------\n");

	status = Utils_memBitBufAlloc(&buf,Size1,1);
	pObj->src_y_buf   = buf.addr;
	UTILS_assert(pObj->src_y_buf != NULL);
	Vps_printf("---------2------------\n");
	status = Utils_memBitBufAlloc(&buf,Size1,1);
	pObj->src_cb_buf  = buf.addr;
	UTILS_assert(pObj->src_cb_buf != NULL);

	status = Utils_memBitBufAlloc(&buf,Size1,1);
	pObj->src_cr_buf  = buf.addr;
	UTILS_assert(pObj->src_cr_buf != NULL);
	status = Utils_memBitBufAlloc(&buf,Size1,1);
	pObj->dst_y_buf   = buf.addr;
	UTILS_assert(pObj->dst_y_buf != NULL);
	status = Utils_memBitBufAlloc(&buf,Size1,1);
	pObj->dst_cb_buf  = buf.addr;
	UTILS_assert(pObj->dst_cb_buf != NULL);
	Vps_printf("---------3------------\n");
	status = Utils_memBitBufAlloc(&buf,Size1,1);
	pObj->dst_cr_buf  = buf.addr;
	UTILS_assert(pObj->dst_cr_buf != NULL);
	status = Utils_memBitBufAlloc(&buf,Size1,1);
	pObj->img_coordinate   = buf.addr;
	UTILS_assert(pObj->img_coordinate != NULL);
	status = Utils_memBitBufAlloc(&buf,Size1,1);
	pObj->img_coordinate_g = buf.addr;
	UTILS_assert(pObj->img_coordinate_g != NULL);
		Vps_printf("--------4------------\n");
	status = Utils_memBitBufAlloc(&buf,Size1*6,1);
	pObj->Point_Buffer  = buf.addr;
	UTILS_assert(pObj->Point_Buffer != NULL);
	status = Utils_memBitBufAlloc(&buf,Size1*6,1);
	pObj->T_L_Buffer    = buf.addr;
	UTILS_assert(pObj->T_L_Buffer != NULL);
	status = Utils_memBitBufAlloc(&buf,Size1*6,1);
	pObj->T_R_Buffer    = buf.addr;
	UTILS_assert(pObj->T_R_Buffer != NULL);
	Vps_printf("---------5------------\n");
	status = Utils_memBitBufAlloc(&buf,Size1*6,1);
	pObj->T_U_Buffer    = buf.addr;
	UTILS_assert(pObj->T_U_Buffer != NULL);
	status = Utils_memBitBufAlloc(&buf,Size1*6,1);
	pObj->T_P_Buffer    = buf.addr;
	UTILS_assert(pObj->T_P_Buffer != NULL);
	Vps_printf("---------6-----------\n");
	status = Utils_memBitBufAlloc(&buf,Size0,1);
	pObj->output_cb_buf    = buf.addr;
	UTILS_assert(pObj->output_cb_buf != NULL);

	status = Utils_memBitBufAlloc(&buf,Size0,1);
	pObj->output_cr_buf    = buf.addr;
	UTILS_assert(pObj->output_cr_buf != NULL);
	Vps_printf("--------7------------\n");

	status = Utils_memBitBufAlloc(&buf,8192*sizeof(int),1);
	pObj->tAutoTrackObj.initHistogram    = buf.addr;
	UTILS_assert(pObj->tAutoTrackObj.initHistogram != NULL);
	Vps_printf("--------8------------\n");

		status = Utils_memBitBufAlloc(&buf,8192*sizeof(int),1);
	pObj->tAutoTrackObj.currentHistogram    = buf.addr;
	UTILS_assert(pObj->tAutoTrackObj.currentHistogram != NULL);
	Vps_printf("--------9------------\n");

		status = Utils_memBitBufAlloc(&buf,8192*sizeof(int),1);
	pObj->tAutoTrackObj.weights    = buf.addr;
	UTILS_assert(pObj->tAutoTrackObj.weights != NULL);
	Vps_printf("--------10------------\n");




	#else
	pObj->Area_vertex = Utils_memAlloc(Size0,32);
	UTILS_assert(pObj->Area_vertex != NULL);
	Vps_printf("---------1------------\n");


	pObj->src_y_buf   = Utils_memAlloc(Size1,32);
	UTILS_assert(pObj->src_y_buf != NULL);
	Vps_printf("---------2------------\n");

	pObj->src_cb_buf  = Utils_memAlloc(Size1,32);
	UTILS_assert(pObj->src_cb_buf != NULL);


	pObj->src_cr_buf  = Utils_memAlloc(Size1,32);
	UTILS_assert(pObj->src_cr_buf != NULL);

	pObj->dst_y_buf   = Utils_memAlloc(Size1,32);
	UTILS_assert(pObj->dst_y_buf != NULL);

	pObj->dst_cb_buf  = Utils_memAlloc(Size1,32);
	UTILS_assert(pObj->dst_cb_buf != NULL);
	Vps_printf("---------3------------\n");

	pObj->dst_cr_buf  =  Utils_memAlloc(Size1,32);
	UTILS_assert(pObj->dst_cr_buf != NULL);

	pObj->img_coordinate   = Utils_memAlloc(Size1,32);
	UTILS_assert(pObj->img_coordinate != NULL);

	pObj->img_coordinate_g = Utils_memAlloc(Size1,32);
	UTILS_assert(pObj->img_coordinate_g != NULL);
		Vps_printf("--------4------------\n");

	pObj->Point_Buffer  = Utils_memAlloc(Size1*6,32);
	UTILS_assert(pObj->Point_Buffer != NULL);

	pObj->T_L_Buffer    =  Utils_memAlloc(Size1*6,32);
	UTILS_assert(pObj->T_L_Buffer != NULL);

	pObj->T_R_Buffer    = Utils_memAlloc(Size1*6,32);
	UTILS_assert(pObj->T_R_Buffer != NULL);
	Vps_printf("---------5------------\n");

	pObj->T_U_Buffer    = Utils_memAlloc(Size1*6,32);
	UTILS_assert(pObj->T_U_Buffer != NULL);

	pObj->T_P_Buffer    = Utils_memAlloc(Size1*6,32);
	UTILS_assert(pObj->T_P_Buffer != NULL);
	Vps_printf("---------6-----------\n");

	pObj->output_cb_buf    =Utils_memAlloc(Size0,32);
	UTILS_assert(pObj->output_cb_buf != NULL);

	pObj->output_cr_buf    = Utils_memAlloc(Size0,32);
	UTILS_assert(pObj->output_cr_buf != NULL);
	Vps_printf("--------7------------\n");


	pObj->tAutoTrackObj.initHistogram    =Utils_memAlloc(8192*sizeof(int),32);
	UTILS_assert(pObj->tAutoTrackObj.initHistogram != NULL);
	Vps_printf("--------8------------\n");


	pObj->tAutoTrackObj.currentHistogram    =Utils_memAlloc(8192*sizeof(int),32);
	UTILS_assert(pObj->tAutoTrackObj.currentHistogram != NULL);
	Vps_printf("--------9------------\n");

	pObj->tAutoTrackObj.weights    = Utils_memAlloc(8192*sizeof(int),32);
	UTILS_assert(pObj->tAutoTrackObj.weights != NULL);
	Vps_printf("--------10------------\n");
	#endif
	return FVID2_SOK;
}

#if 1
UInt32 AlgLink_TrackalgModel(AlgLink_TrackChObj * pChObj)
{
	init_model(&pChObj->trackhandle);

	return FVID2_SOK;
}

UInt32 AlgLink_TrackalgAreaCheck(AlgLink_TrackChObj * pChObj)
{

	Vps_printf("AlgLink_TrackalgAreaCheck\n");
	area_check(&pChObj->trackhandle);

	return FVID2_SOK;
}


#endif

Int32 AlgLink_TrackalgChInit(AlgLink_TrackChObj *pChObj)
{
	ITRACK_Params *pParams = &pChObj->trackhandle.input_param;
	T_GLOBAL_OBJ  *PObj    = &pChObj->trackhandle.t_global_obj;

	memset(pParams, 0x0, sizeof(ITRACK_Params));
	memset(&PObj->pst_det_obj, 0x0, sizeof(STR_OBJECTS_DETECTED));
	memset(&PObj->pst_trak_obj, 0x0, sizeof(STR_OBJECT_TRAKING));
	PObj->m_offset = 0;
	PObj->n_offset  = 0;
	PObj->turn_flag = 0;			//为0时表示不需要重新跟踪，为1时表示捕获到目标，设置跟踪标志，为2时表示跟踪目标丢失
	PObj->turn_time = 0;			//定位到目标后,需要等待摄像头调用预置位的时间
	PObj->track_start = 0;		//表示是否跟踪上目标，0表示未跟踪上，1表示跟踪上了目标
	PObj->first_time  = 0;			//开始进入算法的帧数,目前是前十帧不做处理
	PObj->last_move_flag = 0;		//方向移动标志,为0表示未移动方向,为1表示移动了方向
	PObj->skin_colour_num = 0;
	PObj->nTrackType= 0;
	PObj->last_track_status = 0;


	/*表示每种角度的点的个数*/
	PObj->Point_180_Inc	= 0;
	PObj->Point_90_Inc	= 0;
	PObj->Point_45_Inc	= 0;
	PObj->Point_135_Inc	= 0;
	PObj->Point_Inc		= 0;
//	PObj->reset_inc 		= 0;

	PObj->nSrcBufHeight = 1080;
	PObj->nSrcBufWidth = 1920;

    unsigned int *p1 = NULL;
    unsigned int *p2 = NULL;
	unsigned int *p3 = NULL;
    p1 = PObj->tAutoTrackObj.currentHistogram;
	p2 = PObj->tAutoTrackObj.initHistogram;
	p3 = PObj->tAutoTrackObj.weights;
	memset(&PObj->tAutoTrackObj, 0x0, sizeof(t_TrackingObject));
	PObj->tAutoTrackObj.currentHistogram = p1;
	PObj->tAutoTrackObj.initHistogram = p2;
	PObj->tAutoTrackObj.weights = p3;

	PObj->nLastTrackState = 0;;
	PObj->nTrackType      = 0;//跟踪类型,0为轮廓跟踪,1为meanshift跟踪,2为轮廓和meanshift同时跟踪
	PObj->nTargetFlag     = 0;//表示轮廓找到目标
	PObj->nTrackStartFlag = 0;//表示预跟踪延时结束,开始跟踪标志,但可能没有找到目标
	PObj->nMeanShiftFrame = 0;//meanshift跟踪时的帧统计，每10帧检测一次轮廓
	PObj->nMeanShiftFaceLost = 0;//meanshift跟踪同时检测轮廓，轮廓没找到的次数
	PObj->nPos1MvFlag = 0;//预置位1时全景检测到运动标志
	PObj->nPos1MvLost = 0;//预置位1时全景未检测到运动时间

	memset(PObj->model_v_cnt, 0x0, sizeof(PObj->model_v_cnt));
	memset(PObj->model_v_mnt, 0x0, sizeof(PObj->model_v_mnt));
	memset(PObj->model_v_pnt, 0x0, sizeof(PObj->model_v_pnt));

	return FVID2_SOK;
}





Int32 AlgLink_TrackalgChCreate(AlgLink_TrackChObj *pChObj, AlgLink_TrackCreateParams *pChPrm)
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

	AlgLink_TrackalgChInit(pChObj);

	Vps_printf("AlgLink_TrackalgCreate: AlgLink_TrackalgMalloc Ing\n");
	status = AlgLink_TrackalgMalloc(pChObj);
	UTILS_assert(status == FVID2_SOK);

	Vps_printf("AlgLink_TrackalgCreate: AlgLink_TrackalgSetChTracePrm Ing\n");
	//设置通道信息澹澹(用户参数)
    status = AlgLink_TrackalgSetChTracePrm(pChObj, pChPrm);
    UTILS_assert(status == FVID2_SOK);

	Vps_printf("AlgLink_TrackalgCreate: AlgLink_TrackalgModel Ing\n");
	status = AlgLink_TrackalgModel(pChObj);
	UTILS_assert(status == FVID2_SOK);

	Vps_printf("AlgLink_TrackalgCreate: AlgLink_TrackalgAreaCheck Ing\n");
	status = AlgLink_TrackalgAreaCheck(pChObj);
	UTILS_assert(status == FVID2_SOK);

	return status;
}

Int32 AlgLink_TrackalgPrintPrm(AlgLink_TrackCreateParams *pChPrm)
{
	ITRACK_Params *pTrackParms = &pChPrm->TrackParms;
	Vps_printf("				AlgLink_TrackalgPrintPrm\n");
	Vps_printf("size[%d] \npic_width[%d] pic_height[%d] video_width[%d] video_height[%d]\n"\
			   "zoom_distance[%d] \nstart_track_time[%d] \nreset_time[%d] \nlimit_height[%d]\n"\
			   "x_offset[%d] y_offset[%d] \ntrigger_sum[%d] trigger_x0[%d] trigger_y0[%d]"\
			   "trigger_x1[%d] trigger_y1[%d] \n trigger1_x0[%d] trigger_y1[%d] trigger1_x0[%d] trigger1_y0[%d]\n"\
			   "control_mode[%d] \nsens[%d] \nmessage[%d]\n"\
			   "model_sum[%d] \ntrack_sel[%d] \nmid_x[%d] \nmid_y[%d] \nmodel_multiple[%d] \nmodel_level[%d]\n"\
			   "track_point_num[%d]\n meanshift_flag[%d] x0[%d] y0[%d] x1[%d] y1[%d]\n pos1_trigger_num[%d] x0[%d] y0[%d] x1[%d] y1[%d]"
			   "nTrackSwitchType[%d]"\
			   "reset_level[%d]",\
				pTrackParms->static_param.size,
				pTrackParms->static_param.pic_width,
				pTrackParms->static_param.pic_height ,
				pTrackParms->static_param.video_width,
				pTrackParms->static_param.video_height,
				pTrackParms->dynamic_param.zoom_distance,
				pTrackParms->dynamic_param.start_track_time,
				pTrackParms->dynamic_param.reset_time ,
				pTrackParms->dynamic_param.limit_height,
				pTrackParms->dynamic_param.x_offset,
				pTrackParms->dynamic_param.y_offset,
				pTrackParms->dynamic_param.trigger_sum,
				pTrackParms->dynamic_param.trigger[0].trigger_x0,
				pTrackParms->dynamic_param.trigger[0].trigger_y0,
				pTrackParms->dynamic_param.trigger[0].trigger_x1,
				pTrackParms->dynamic_param.trigger[0].trigger_y1,
				pTrackParms->dynamic_param.trigger[1].trigger_x0,
				pTrackParms->dynamic_param.trigger[1].trigger_y0,
				pTrackParms->dynamic_param.trigger[1].trigger_x1,
				pTrackParms->dynamic_param.trigger[1].trigger_y1,
				pTrackParms->dynamic_param.control_mode,
				pTrackParms->dynamic_param.sens ,
				pTrackParms->dynamic_param.message ,
				pTrackParms->dynamic_param.model_sum,
				pTrackParms->dynamic_param.track_sel,
				pTrackParms->dynamic_param.mid_x,
				pTrackParms->dynamic_param.mid_y ,
				pTrackParms->dynamic_param.model_multiple,
				pTrackParms->dynamic_param.model_level ,
				pTrackParms->dynamic_param.track_point_num,
				pTrackParms->dynamic_param.TrackOtherParam.meanshift_flag,
				pTrackParms->dynamic_param.TrackOtherParam.meanshift_trigger.trigger_x0,
				pTrackParms->dynamic_param.TrackOtherParam.meanshift_trigger.trigger_y0,
				pTrackParms->dynamic_param.TrackOtherParam.meanshift_trigger.trigger_x1,
				pTrackParms->dynamic_param.TrackOtherParam.meanshift_trigger.trigger_y1,
				pTrackParms->dynamic_param.TrackOtherParam.pos1_trigger_num,
				pTrackParms->dynamic_param.TrackOtherParam.pos1_trigger.trigger_x0,
				pTrackParms->dynamic_param.TrackOtherParam.pos1_trigger.trigger_y0,
				pTrackParms->dynamic_param.TrackOtherParam.pos1_trigger.trigger_x1,
				pTrackParms->dynamic_param.TrackOtherParam.pos1_trigger.trigger_y1,
				pTrackParms->dynamic_param.TrackOtherParam.nTrackSwitchType,
				pTrackParms->dynamic_param.reset_level);

	return FVID2_SOK;
}



Int32 AlgLink_TrackalgCreate(AlgLink_TrackObj * pObj)
{
	Int32 status, chId ,numchanel,i;
    AlgLink_TrackChObj *pChObj;
    AlgLink_TrackCreateParams *pChPrm;

	Vps_printf("AlgLink_TrackalgCreate Ing [%d]...\n",pObj->inQueInfo->numCh);


//	pObj->inQueInfo->numCh = 1;

	if(pObj->inQueInfo->numCh > ALG_LINK_TRACK_MAX_CH)
	{
		numchanel = ALG_LINK_TRACK_MAX_CH;
	}
	else
	{
		numchanel = pObj->inQueInfo->numCh;
	}

	for(i = 0; i < ALG_LINK_TRACK_MAX_TO_CH; i++)
	{
		pObj->ChToStream[i] = pObj->trackChCreateParams[0].ChToStream[i];
	}


    for(chId=0; chId< numchanel; chId++)
    {
        pChObj = &pObj->chObj[chId];

        pChPrm = &pObj->trackChCreateParams[chId];

        pChObj->chId = chId;

		//创建参数初始化,暂时放这里
		//AlgLink_TrackalgInit(pChPrm);
		AlgLink_TrackalgPrintPrm(pChPrm);
		status	= AlgLink_TrackalgChCreate(pChObj,pChPrm);

		UTILS_assert(status == FVID2_SOK);
    }

	return FVID2_SOK;
}


Int32 AlgLink_TracealgDelete(AlgLink_TrackObj * pObj)
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

UInt32 TrackBackdrop(AlgLink_TrackObj * pObj, int chid, FVID2_Frame *pFrame)
{

	AlgLink_TrackCreateParams	*pChParams = &pObj->trackChCreateParams[chid];

	if(pChParams->SaveViewFlag == TRUE)
	{
		if(pChParams->InBufferAddr[0][0] != NULL)
		{
			System_FrameInfo *pFrameInfo = NULL;
			pFrameInfo = (System_FrameInfo*)pFrame->appData;
			UTILS_assert(pFrameInfo != NULL);
			Uint32 nSrcBufWidth  = pFrameInfo->rtChInfo.pitch[0];
			Uint32 nDstBufWidth  = pFrameInfo->rtChInfo.width;
			Uint32 nDstBufHeight = pFrameInfo->rtChInfo.height;
			
			Int8 *pSrc = (Int8 *)pFrame->addr[0][0];
			Int8 *pDst = (Int8 *)pChParams->InBufferAddr[0][0];
			Uint32 i = 0;
			
			if(nDstBufWidth*nDstBufHeight <= 1920*1080)
			{
				for(i = 0; i < nDstBufHeight; i++)
				{
					memcpy(pDst, pSrc + i*nSrcBufWidth, nDstBufWidth);
					pDst = pDst + nDstBufWidth;
				}

				System_linkControl(SYSTEM_LINK_ID_HOST, VSYS_EVENT_TRACK_SAVE_VIEW, NULL, 0,TRUE);
			}
			
			Vps_printf("track Save View : wxh[%dx%d] %p \n",nDstBufWidth,nDstBufHeight,pChParams->InBufferAddr[0][0]);
		}
		
		pChParams->SaveViewFlag = FALSE;
	}

	
	return 0;
}

Int32 AlgLink_TrackalgProcessFrame(AlgLink_TrackObj * pObj, FVID2_Frame *pFrame)
{

    AlgLink_TrackChObj *pChObj = NULL;
    System_FrameInfo *pFrameInfo = NULL;
	ITRACK_Params * pParams = NULL;
	T_GLOBAL_OBJ *ptrackObj = NULL;
    Bool isInterlaced;
	UInt32 chid = 0;
	static UInt32 trackStartTime = 0;
	static UInt32 trackframes = 0;
	static UInt32 trackprocessframes = 0;

    isInterlaced    = FALSE;

  	if(pFrame->channelNum >= ALG_LINK_TRACK_MAX_TO_CH)
  	{
		return 0;
	}

	chid = pObj->ChToStream[pFrame->channelNum];

	if(chid >= ALG_LINK_TRACK_MAX_CH || chid < 0)
	{
		return 0;
	}



	pChObj = &pObj->chObj[chid];

	pParams   = &pChObj->trackhandle.input_param;
	ptrackObj = &pChObj->trackhandle.t_global_obj;
    pFrameInfo = (System_FrameInfo*)pFrame->appData;
    UTILS_assert(pFrameInfo!=NULL);


	TrackBackdrop(pObj, chid, pFrame);

	if(pObj->trackChCreateParams[chid].TrackParms.dynamic_param.control_mode != 0 ||
	   pObj->trackChCreateParams[chid].TrackParms.dynamic_param.size == 1)
	{
		return 1;
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
		 AlgLink_TrackCreateParams *pChPrm;
		 UInt32 status;

		 if((pFrameInfo->rtChInfo.width  != 704) && (pFrameInfo->rtChInfo.height != 576))
		 {
		 	return 0;	
		 }
		 pChObj->rtParamUpdatePerFrame = FALSE;

		 pChPrm = &pObj->trackChCreateParams[0];
		 pChPrm->TrackParms.static_param.video_width  = pFrameInfo->rtChInfo.width;
		 pChPrm->TrackParms.static_param.video_height = pFrameInfo->rtChInfo.height;
		 pChPrm->TrackParms.static_param.pic_width  = pFrameInfo->rtChInfo.width/pChPrm->TrackParms.dynamic_param.x_offset;
		 pChPrm->TrackParms.static_param.pic_height = pFrameInfo->rtChInfo.height/pChPrm->TrackParms.dynamic_param.y_offset;


		 AlgLink_TrackalgPrintPrm(pChPrm);
		 AlgLink_TrackalgChInit(pChObj);
		 status = AlgLink_TrackalgSetChTracePrm(pChObj, pChPrm);
   		 UTILS_assert(status == FVID2_SOK);

		 //if(pChPrm->TrackParms.dynamic_param.reset_level == RE_START)
		 {
		 	AlgLink_TrackalgAreaCheck(pChObj);
		 }
		// else if(pChPrm->TrackParms.dynamic_param.reset_level == RE_INIT)
		 {
		 	//...
		 }

		 if(pFrame->addr[0][1] > 0)
		 {
			ptrackObj->nSrcBufWidth  = pFrameInfo->rtChInfo.pitch[0];
			ptrackObj->nSrcBufHeight = ((Int8*)pFrame->addr[0][1] - (Int8*)pFrame->addr[0][0])/pFrameInfo->rtChInfo.pitch[0];
			Vps_printf("AlgLink_TrackalgProcessFrame: update nSrcBufWidth nSrcBufHeight [%d] [%d]\n",ptrackObj->nSrcBufWidth,ptrackObj->nSrcBufHeight);
		 }

		 AlgLink_TrackalgPrintPrm(pChPrm);
		 Vps_printf("AlgLink_TrackalgProcessFrame: rtParamUpdatePerFrame Done!!!\n");
	}


	if(    (pFrameInfo->rtChInfo.width == pParams->static_param.video_width)
		&& (pFrameInfo->rtChInfo.height == pParams->static_param.video_height)
		//&& (pFrame->channelNum == 0)
	)
	{
		//	Vps_printf("AlgLink_TrackalgProcessFrame: ch[ %d ] [%p] Src[%dx%d] [%d][%d]",pFrame->channelNum,(UInt8 *)pFrame->addr[0][0],pFrameInfo->rtChInfo.width,pFrameInfo->rtChInfo.height,\
			//	pFrameInfo->rtChInfo.pitch[0],pFrameInfo->rtChInfo.pitch[1]);

		if((ptrackObj->nSrcBufWidth != pFrameInfo->rtChInfo.pitch[0]) && (pFrame->addr[0][1] > 0))
		{

			ptrackObj->nSrcBufWidth  = pFrameInfo->rtChInfo.pitch[0];
			ptrackObj->nSrcBufHeight = ((Int8*)pFrame->addr[0][1] - (Int8*)pFrame->addr[0][0])/pFrameInfo->rtChInfo.pitch[0];
			Vps_printf("AlgLink_TrackalgProcessFrame: update nSrcBufWidth nSrcBufHeight [%d] [%d]\n",ptrackObj->nSrcBufWidth,ptrackObj->nSrcBufHeight);
		}
	//	trackframes++;
		//if(trackframes%2 == 0)
	//	return 0;
		AlgLink_TrackChStatus chStatus = {0};
		chStatus.chId = pChObj->chId;
		//Track_Process(pChObj, (Int8 *)pFrame->addr[0][0], &chStatus.args);


		track_process(&pChObj->trackhandle, (Int8 *)pChObj->InBufferAddr[0][0],(Int8 *)pFrame->addr[0][0],&chStatus.args);

	    System_linkControl(SYSTEM_LINK_ID_HOST, VSYS_EVENT_TRACK_STATUS, &chStatus, sizeof(AlgLink_TrackChStatus), TRUE);

		if(chStatus.args.cam_position > 42)
		{
			UTILS_assert(0);
		}

		trackprocessframes++;
		if ((Utils_getCurTimeInMsec() - trackStartTime) > 1 * 1000)
	    {

			trackStartTime = Utils_getCurTimeInMsec();
			pObj->algstate.frames = trackprocessframes;
			Vps_printf("Track_Process Done %dfps...... \n",trackprocessframes);
			trackprocessframes = 0;
	    }

	#if 0
			int size[2] = {0};
			size[0] = pFrameInfo->rtChInfo.pitch[0] * pFrameInfo->rtChInfo.height;
			size[1] = pFrameInfo->rtChInfo.pitch[1] * pFrameInfo->rtChInfo.height;

			size[0] = pFrameInfo->rtChInfo.pitch[0] * 30;
			size[1] = pFrameInfo->rtChInfo.pitch[1] * 30;

 			//Vps_printf("----->%p %p %d\n",pFrame->addr[0][0],pFrame->addr[0][1],pFrameInfo->rtChInfo.pitch[0] * pFrameInfo->rtChInfo.height);
 			if(((char *)pFrame->addr[0][0] + 1920 * 1080) != (char *)pFrame->addr[0][1])
				Vps_printf("--error>%p %p %d\n",pFrame->addr[0][0],pFrame->addr[0][1],pFrameInfo->rtChInfo.pitch[0] * pFrameInfo->rtChInfo.height);
				//UTILS_assert(0);
			char *pU = pFrame->addr[0][1];
			char *pV = pU + 1;
		    int index = 0;
	        if(pFrame->addr[0][0] && size[0]){
				memset(pFrame->addr[0][0], 0x36, size[0]);
	        }
			for(index = 0; index < size[1] / 4; index++){
				*pU = 0xC0;
				pU++;
				pU++;
			}
			for(index = 0; index < size[1] / 4; index++){
				*pV = 0x70;
				pV++;
				pV++;
			}


	#endif


	}

    return 0;
}






