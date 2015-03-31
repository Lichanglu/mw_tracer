/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include "stutrackLink_priv.h"
#include <mcfw/interfaces/common_def/ti_vsys_common_def.h>
#include "track_students.h"

#if 0
UInt32 AlgLink_StuTrackalgAreaCheck(AlgLink_StuTrackChObj * pChObj)
{
	
	int32_t	 	k				= 0;	
	int32_t		m				= 0;
	int32_t		n				= 0;
	vertex_t	point_y_value	= {0};

	
	StuITRACK_Params *pTrackParams = &pChObj->StuTrackParams;
	int16_t		video_width		= pTrackParams->static_param.video_width;
	int16_t		video_height	= pTrackParams->static_param.video_height;

	
	
	#if 1
	for (k = 0; k < video_width * video_height; k++)  
	{
		m = k%video_width;     //列
		n = k/video_width;    //行

		point_y_value.x = m;
		point_y_value.y = n;

		if (pt_in_poly ((const vertex_t *)pTrackParams->dynamic_param.track_point, pTrackParams->dynamic_param.track_point_num,  
			&point_y_value)==1)
		{
		#if 0
			if (n%2 == 0)
			{
				*((Uint8 *)Area_vertex + n/2 * video_width + m) = 255;
			}
			else
			{
				*((Uint8 *)Area_vertex + (n/2 + video_height/2)  * video_width + m) = 255;
			}
		#endif	
			*((Uint8 *)pChObj->Students_Area_vertex + n * video_width + m) = 255;
		}
		else
		{
		#if 0
			if (n%2 == 0)
			{
				*((Uint8 *)Area_vertex + n/2 * video_width + m) = 0;
			}
			else
			{
				*((Uint8 *)Area_vertex + (n/2 + video_height/2) * video_width + m) = 0;
			}
		#endif	
			*((Uint8 *)pChObj->Students_Area_vertex + n * video_width + m) = 0;
		}
	}
	#endif
	return FVID2_SOK;
}
#endif

Int32 AlgLink_StuTrackalgSetChTracePrm(AlgLink_StuTrackChObj *pChObj,
                            AlgLink_StuTrackCreateParams * params)
{
	Int32 status = 0;
	StuITRACK_Params *pChParams =	&pChObj->stutrackhandle.input_param;

	memset(pChParams, 0x0, sizeof(StuITRACK_Params));
	memcpy(pChParams, &params->StuTrackParms, sizeof(StuITRACK_Params));

	Vps_printf("Pic width x height[ %dx%d ] VD width x height[ %dx%d ] Size[%d] \n",pChParams->static_param.pic_width, pChParams->static_param.pic_height,\
	pChParams->static_param.video_width,pChParams->static_param.video_height,pChParams->static_param.size);


	return (status);
}





UInt32 AlgLink_StuTrackalgMalloc(AlgLink_StuTrackChObj * pChObj)
{

	StuITRACK_Params  Params;
	StuITRACK_Params *TrackParams  = &Params;
	Bitstream_Buf buf;
	UInt32 status;

	T_STU_GLOBAL_OBJ *pObj =  &pChObj->stutrackhandle.t_stu_global_obj;
	
	TrackParams->static_param.video_width  = 1920;
	TrackParams->static_param.video_height = 1080;
	TrackParams->static_param.pic_width    = 480;
	TrackParams->static_param.pic_height   = 270;

	UInt32 Size0 = TrackParams->static_param.video_width * TrackParams->static_param.video_height;
	UInt32 Size1 = TrackParams->static_param.pic_width   * TrackParams->static_param.pic_height;

	#if 1
	status = Utils_memBitBufAlloc(&buf,Size1,1);
	pObj->students_src_y_buf[0] = buf.addr;
	UTILS_assert(pObj->students_src_y_buf[0] != NULL);
	Vps_printf("---------1------------\n");

	status = Utils_memBitBufAlloc(&buf,Size1,1);
	pObj->students_src_y_buf[1] = buf.addr;
	UTILS_assert(pObj->students_src_y_buf[1] != NULL);
	Vps_printf("---------1------------\n");
	

	status = Utils_memBitBufAlloc(&buf,Size1,1);
	pObj->students_dst_y_buf[0]   = buf.addr;
	UTILS_assert(pObj->students_dst_y_buf[0] != NULL);
	Vps_printf("---------2------------\n");
	status = Utils_memBitBufAlloc(&buf,Size1,1);
	pObj->students_dst_y_buf[1]  = buf.addr;
	UTILS_assert(pObj->students_dst_y_buf[1] != NULL);

	status = Utils_memBitBufAlloc(&buf,Size1,1);
	pObj->students_dst_y_buf[2]  = buf.addr;
	UTILS_assert(pObj->students_dst_y_buf[2] != NULL);

	
	status = Utils_memBitBufAlloc(&buf,Size1,1);
	pObj->students_time_count[0]   = buf.addr;
	UTILS_assert(pObj->students_time_count[0] != NULL);
	
	status = Utils_memBitBufAlloc(&buf,Size1,1);
	pObj->students_time_count[1]  = buf.addr;
	UTILS_assert(pObj->students_time_count[1] != NULL);
	Vps_printf("---------3------------\n");
	
	status = Utils_memBitBufAlloc(&buf,Size1,1);
	pObj->students_time_count[2]  = buf.addr;
	UTILS_assert(pObj->students_time_count[2] != NULL);
	
	status = Utils_memBitBufAlloc(&buf,Size1,1);
	pObj->students_time_count[3]   = buf.addr;
	UTILS_assert(pObj->students_time_count[3] != NULL);
	
	status = Utils_memBitBufAlloc(&buf,Size1,1);
	pObj->students_time_count[4] = buf.addr;
	UTILS_assert(pObj->students_time_count[4] != NULL);
	
		Vps_printf("--------4------------\n");
	status = Utils_memBitBufAlloc(&buf,Size1,1);
	pObj->students_time_count[5]  = buf.addr;
	UTILS_assert(pObj->students_time_count[5] != NULL);
	
	status = Utils_memBitBufAlloc(&buf,Size1,1);
	pObj->students_time_count[6]    = buf.addr;
	UTILS_assert(pObj->students_time_count[6] != NULL);
	
	status = Utils_memBitBufAlloc(&buf,Size1,1);
	pObj->students_time_count[7]    = buf.addr;
	UTILS_assert(pObj->students_time_count[7] != NULL);
	
	Vps_printf("---------5------------\n");
	status = Utils_memBitBufAlloc(&buf,Size1,1);
	pObj->students_time_count[8]    = buf.addr;
	UTILS_assert(pObj->students_time_count[8] != NULL);
	
	Vps_printf("---------16-----------\n");
	status = Utils_memBitBufAlloc(&buf,Size0,1);
	pObj->Students_Area_vertex    = buf.addr;
	UTILS_assert(pObj->Students_Area_vertex != NULL);


	

	//status = Utils_memBitBufAlloc(&buf,sizeof(student_trigger_info_t) * STUDENTS_TRIGGER_NUM,1);
	//pChObj->StuTrackParams.dynamic_param.trigger_info    = buf.addr;
	//UTILS_assert(pChObj->StuTrackParams.dynamic_param.trigger_info != NULL);
	//Vps_printf("--------17------------\n");

	//status = Utils_memBitBufAlloc(&buf,sizeof(student_trigger_info_t) * STUDENTS_SHIELD_NUM,1);
	//pChObj->StuTrackParams.dynamic_param.shield_info    = buf.addr;
	//UTILS_assert(pChObj->StuTrackParams.dynamic_param.shield_info != NULL);
	Vps_printf("--------18------------\n");


	#else
	pChObj->Area_vertex = Utils_memAlloc(Size0,4);
	UTILS_assert(pChObj->Area_vertex != NULL);

	Vps_printf("---------1------------\n");
	
	pChObj->src_y_buf   = Utils_memAlloc(Size1,4);
	UTILS_assert(pChObj->src_y_buf != NULL);
	Vps_printf("---------2------------\n");
	pChObj->src_cb_buf  = Utils_memAlloc(Size1,4);
	UTILS_assert(pChObj->src_cb_buf != NULL);
	
	pChObj->src_cr_buf  = Utils_memAlloc(Size1,4);
	UTILS_assert(pChObj->src_cr_buf != NULL);
	
	pChObj->dst_y_buf   = Utils_memAlloc(Size1,4);
	UTILS_assert(pChObj->dst_y_buf != NULL);
	
	pChObj->dst_cb_buf  = Utils_memAlloc(Size1,4);
	UTILS_assert(pChObj->dst_cb_buf != NULL);
	Vps_printf("---------3------------\n");
	pChObj->dst_cr_buf  = Utils_memAlloc(Size1,4);
	UTILS_assert(pChObj->dst_cr_buf != NULL);

	pChObj->img_coordinate   = Utils_memAlloc(Size1,4);
	UTILS_assert(pChObj->img_coordinate != NULL);

	pChObj->img_coordinate_g = Utils_memAlloc(Size1,4);
	UTILS_assert(pChObj->img_coordinate_g != NULL);
		Vps_printf("--------4------------\n");
	pChObj->Point_Buffer  = Utils_memAlloc(Size1*6,4);
	UTILS_assert(pChObj->Point_Buffer != NULL);
	
	pChObj->T_L_Buffer    = Utils_memAlloc(Size1*6,4);
	UTILS_assert(pChObj->T_L_Buffer != NULL);
	
	pChObj->T_R_Buffer    = Utils_memAlloc(Size1*6,4);
	UTILS_assert(pChObj->T_R_Buffer != NULL);
	Vps_printf("---------5------------\n");
	pChObj->T_U_Buffer    = Utils_memAlloc(Size1*6,4);
	UTILS_assert(pChObj->T_U_Buffer != NULL);
		
	pChObj->T_P_Buffer    = Utils_memAlloc(Size1*6,4);
	UTILS_assert(pChObj->T_P_Buffer != NULL);
Vps_printf("---------6-----------\n");
	pChObj->output_cb_buf    = Utils_memAlloc(Size0,4);
	UTILS_assert(pChObj->output_cb_buf != NULL);
	
	pChObj->output_cr_buf    = Utils_memAlloc(Size0,4);
	UTILS_assert(pChObj->output_cr_buf != NULL);
Vps_printf("--------7------------\n");


	//TrackParams->static_param.video_width  = 1280;
	//TrackParams->static_param.video_height = 720;
	//TrackParams->static_param.pic_width    = 160;
	//TrackParams->static_param.pic_height   = 90;
	#endif
	return FVID2_SOK;
}







Int32 AlgLink_StuTrackalgChInit(AlgLink_StuTrackChObj *pChObj)
{
	
	//初始化依赖初始参数
	track_students_init(&pChObj->stutrackhandle);
	
	return FVID2_SOK;
}





Int32 AlgLink_StuTrackalgChCreate(AlgLink_StuTrackChObj *pChObj, AlgLink_StuTrackCreateParams *pChPrm)
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
    status = AlgLink_StuTrackalgSetChTracePrm(pChObj, pChPrm);
    UTILS_assert(status == FVID2_SOK);
	Vps_printf("AlgLink_StuTrackalgCreate: AlgLink_StuTrackalgSetChTracePrm Ing\n");

	Vps_printf("AlgLink_StuTrackalgCreate: AlgLink_StuTrackalgMalloc Ing\n");
	status = AlgLink_StuTrackalgMalloc(pChObj);
	UTILS_assert(status == FVID2_SOK);
	
	
	AlgLink_StuTrackalgChInit(pChObj);

	
	return FVID2_SOK;
}

Int32 AlgLink_StuTrackalgPrintPrm(AlgLink_StuTrackCreateParams *pChPrm)
{

	Int32 i = 0;
	StuITRACK_Params *pTrackParms = &pChPrm->StuTrackParms;
	Vps_printf("size[%d] \npic_width[%d] pic_height[%d] video_width[%d] video_height[%d]\n"\
			   "reset_time[%d] message[%d]  process_type[%d] reset_level[%d]\n"\
			    "sens[%d] track_point_num[%d]  trigger_num[%d]\n x_offset[%d] y_offset[%d]\n"\
				,pTrackParms->static_param.size,
				pTrackParms->static_param.pic_width,
				pTrackParms->static_param.pic_height ,
				pTrackParms->static_param.video_width,
				pTrackParms->static_param.video_height,
				pTrackParms->dynamic_param.reset_time,
				pTrackParms->dynamic_param.message,
				pTrackParms->dynamic_param.process_type,
				pTrackParms->dynamic_param.reset_level,
				pTrackParms->dynamic_param.sens,
				pTrackParms->dynamic_param.track_point_num,
				pTrackParms->dynamic_param.trigger_num,
				pTrackParms->dynamic_param.x_offset,
				pTrackParms->dynamic_param.y_offset
				);

	Vps_printf("Point [n]  [ x:y ]\n");
	for(i = 0; i < pTrackParms->dynamic_param.track_point_num; i++)
	{
		Vps_printf("     [%2d] [%3d:%3d]\n",i,pTrackParms->dynamic_param.track_point[i].x,pTrackParms->dynamic_param.track_point[i].y);
	}

	Vps_printf("trigger_info [n]  [ x:y ] [width*height]\n");
	for(i = 0; i < STUDENTS_TRIGGER_NUM; i++)
	{
		Vps_printf("            [%2d] [%3d:%3d] [%3d*%3d]\n",i,pTrackParms->dynamic_param.trigger_info[i].x,pTrackParms->dynamic_param.trigger_info[i].y,\
						pTrackParms->dynamic_param.trigger_info[i].width,pTrackParms->dynamic_param.trigger_info[i].height);
	}

	Vps_printf("shield_info  [n]  [ x:y ] [width*height]\n");
	for(i = 0; i < STUDENTS_SHIELD_NUM; i++)
	{
		Vps_printf("            [%2d] [%3d:%3d] [%3d*%3d]\n",i,pTrackParms->dynamic_param.shield_info[i].x,pTrackParms->dynamic_param.shield_info[i].y,\
						pTrackParms->dynamic_param.shield_info[i].width,pTrackParms->dynamic_param.shield_info[i].height);
	}
		
	return FVID2_SOK;
}



Int32 AlgLink_StuTrackalgCreate(AlgLink_StuTrackObj * pObj)
{
	Int32 status, chId ,numchanel,i;
    AlgLink_StuTrackChObj *pChObj;
    AlgLink_StuTrackCreateParams *pChPrm;

	Vps_printf("AlgLink_StuTrackalgCreate Ing [%d]...\n",pObj->inQueInfo->numCh);


//	pObj->inQueInfo->numCh = 1;

	if(pObj->inQueInfo->numCh > ALG_LINK_STUTRACK_MAX_CH)
	{
		numchanel = ALG_LINK_STUTRACK_MAX_CH;
	}
	else
	{
		numchanel = pObj->inQueInfo->numCh;
	}

	for(i = 0; i < ALG_LINK_STUTRACK_MAX_TO_CH; i++)
	{
		pObj->ChToStream[i] = pObj->stutrackChCreateParams[0].ChToStream[i];
	}
	

    for(chId=0; chId< numchanel; chId++)
    {
        pChObj = &pObj->chObj[chId];

        pChPrm = &pObj->stutrackChCreateParams[chId];

        pChObj->chId = chId;
	
		//创建参数初始化,暂时放这里
		AlgLink_StuTrackalgPrintPrm(pChPrm);
		status	= AlgLink_StuTrackalgChCreate(pChObj,pChPrm);

		UTILS_assert(status == FVID2_SOK);
    }

	return FVID2_SOK;
}


Int32 AlgLink_StuTracealgDelete(AlgLink_StuTrackObj * pObj)
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

typedef struct CvScalar
{
    double val[4];
}
CvScalar;
typedef struct CvPoint
{
    int x;
    int y;
}
CvPoint;

Int32 AlgLink_StuTrackalgProcessFrame(AlgLink_StuTrackObj * pObj, FVID2_Frame *pFrame)
{

    AlgLink_StuTrackChObj *pChObj = NULL;
    System_FrameInfo *pFrameInfo = NULL;
	static UInt32 stutrackStartTime = 0;
	static UInt32 stutrackprocessframes =  0;
	Int32 chid = -1;

	static UInt32 jj =0;

  	if(pFrame->channelNum >= ALG_LINK_STUTRACK_MAX_TO_CH)
  	{
		return 0;
	}

	chid = pObj->ChToStream[pFrame->channelNum];

	if(chid >= ALG_LINK_STUTRACK_MAX_CH || chid < 0)
	{
		return 0;
	}
	
	pChObj = &pObj->chObj[chid];
	StuITRACK_Params *pParams =	&pChObj->stutrackhandle.input_param;
	T_STU_GLOBAL_OBJ *ptrackObj    =	&pChObj->stutrackhandle.t_stu_global_obj;

    pFrameInfo = (System_FrameInfo*)pFrame->appData;
    UTILS_assert(pFrameInfo != NULL);

	if(pObj->stutrackChCreateParams[chid].StuTrackParms.dynamic_param.control_mode != 0 ||
	  pObj->stutrackChCreateParams[chid].StuTrackParms.dynamic_param.size == 1)
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
		 AlgLink_StuTrackCreateParams *pChPrm;
		 UInt32 status;
		 UTILS_assert(pFrameInfo->rtChInfo.width  <= 1920);
		 UTILS_assert(pFrameInfo->rtChInfo.height <= 1080);
		 pChObj->rtParamUpdatePerFrame = FALSE;
		 
		 pChPrm = &pObj->stutrackChCreateParams[chid];
		 pChPrm->StuTrackParms.static_param.video_width  = pFrameInfo->rtChInfo.width;
		 pChPrm->StuTrackParms.static_param.video_height = pFrameInfo->rtChInfo.height;
		 pChPrm->StuTrackParms.static_param.pic_width    = pFrameInfo->rtChInfo.width/pChPrm->StuTrackParms.dynamic_param.x_offset;
		 pChPrm->StuTrackParms.static_param.pic_height   = pFrameInfo->rtChInfo.height/pChPrm->StuTrackParms.dynamic_param.y_offset;

	     status = AlgLink_StuTrackalgSetChTracePrm(pChObj, pChPrm);
   		 UTILS_assert(status == FVID2_SOK); 
		 AlgLink_StuTrackalgChInit(pChObj);
		
		 
 		if(pFrame->addr[0][1] > 0)
 		{
 			ptrackObj->nSrcBufWidth  = pFrameInfo->rtChInfo.pitch[0];
 			ptrackObj->nSrcBufHeight = ((Int8*)pFrame->addr[0][1] - (Int8*)pFrame->addr[0][0])/pFrameInfo->rtChInfo.pitch[0];
			Vps_printf("AlgLink_StuTrackalgProcessFrame: update nSrcBufWidth nSrcBufHeight [%d] [%d]\n",ptrackObj->nSrcBufWidth,ptrackObj->nSrcBufHeight);
		}
		 Vps_printf("AlgLink_TrackalgProcessFrame: rtParamUpdatePerFrame Done!!!\n");
		 AlgLink_StuTrackalgPrintPrm(pChPrm);
	}
	

	if( (pFrameInfo->rtChInfo.width == pParams->static_param.video_width) 
		&& (pFrameInfo->rtChInfo.height ==  pParams->static_param.video_height) 
	)
	{
			//Vps_printf("AlgLink_StuTrackalgProcessFrame: ch[ %d ] [%p] Src[%dx%d] [%d][%d] [%p] [%p] [%d]\n",pFrame->channelNum,(UInt8 *)pFrame->addr[0][0],pFrameInfo->rtChInfo.width,pFrameInfo->rtChInfo.height,\
				//pFrameInfo->rtChInfo.pitch[0],pFrameInfo->rtChInfo.pitch[1],pFrame->addr[0][0],pFrame->addr[0][1],pFrameInfo->rtChInfo.dataFormat);


		AlgLink_StuTrackChStatus chStatus = {0};
		chStatus.chId = pChObj->chId;

		if((ptrackObj->nSrcBufWidth != pFrameInfo->rtChInfo.pitch[0]) && (pFrame->addr[0][1] > 0))
		{
			
			ptrackObj->nSrcBufWidth  = pFrameInfo->rtChInfo.pitch[0];
			ptrackObj->nSrcBufHeight = ((Int8*)pFrame->addr[0][1] - (Int8*)pFrame->addr[0][0])/pFrameInfo->rtChInfo.pitch[0];
			Vps_printf("AlgLink_StuTrackalgProcessFrame: update nSrcBufWidth nSrcBufHeight [%d] [%d]\n",ptrackObj->nSrcBufWidth,ptrackObj->nSrcBufHeight);
		}

		
		//Vps_printf("SrcWidth[%d] SrcHeight[%d]\n",ptrackObj->nSrcBufWidth,ptrackObj->nSrcBufHeight);
		track_students_process(&pChObj->stutrackhandle, (Int8 *)pFrame->addr[0][0], &chStatus.args);
	
  		System_linkControl(SYSTEM_LINK_ID_HOST, VSYS_EVENT_STUTRACK_STATUS, &chStatus, sizeof(AlgLink_StuTrackChStatus), TRUE);
	
		stutrackprocessframes++;
		if ((Utils_getCurTimeInMsec() - stutrackStartTime) > 1 * 1000)
	    {
			stutrackStartTime = Utils_getCurTimeInMsec();
			pObj->algstate.frames = stutrackprocessframes;
			Vps_printf("StuTrack_Process Done %dfps...... \n",stutrackprocessframes);
			stutrackprocessframes = 0;
		}
	
	}

    return 0;
}






