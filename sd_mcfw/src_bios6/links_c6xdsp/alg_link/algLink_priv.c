/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include "algLink_priv.h"
#include <mcfw/src_bios6/utils/utils_mem.h>
#include <mcfw/interfaces/common_def/ti_vsys_common_def.h>
//#define UTILS_SCD_GET_OUTBUF_SIZE()   (sizeof(SCD_Output) + (sizeof(SCD_blkChngMeta) * ALG_LINK_SCD_MAX_BLOCKS_IN_FRAME))

#define UTILS_SCD_GET_OUTBUF_SIZE()   (sizeof(AlgLink_ScdResult))


Int32 AlgLink_printStatistics (AlgLink_Obj *pObj)
{
	Vps_printf("AlgFrames[%u]                      GetNum[%u]                 FreeNum[%u]\n"\
			   "Frames[%u] Track BufQue[%u]        ProcessFlag[%u] GetNum[%d] FreeNum[%u]\n"\
		       "Frames[%u] StuTrack BufQue[%u]     ProcessFlag[%u] GetNum[%d] FreeNum[%u]\n"\
		       "Frames[%u] StuSideTrack BufQue[%u] ProcessFlag[%u] GetNum[%d] FreeNum[%u]\n"\
		       "Frames[%u] StuDoubleSideTrack BufQue[%u] ProcessFlag[%u] GetNum[%d] FreeNum[%u]\n"\
		       "Frames[%u] CeilTrack BufQue[%u] ProcessFlag[%u] GetNum[%d] FreeNum[%u]\n"\
		       "Track Debug [%d] [%d] [%d]\n"\
		       "DoubleSide[0] [%d] [%d] [%d]\n"\
		       "DoubleSide[1] [%d] [%d] [%d]\n"\
		       "CeilTrack [%d]\n"\
			   "debug[%d]\n",\
	 	  pObj->algstate.frames,\
	 	  pObj->algstate.GetNum,\
	 	  pObj->algstate.FreeNum,\

		 pObj->trackAlg.algstate.frames,\
		 Utils_queGetQueuedCount(&pObj->trackAlg.trackoutFrameBufQue),\
	     pObj->trackAlg.ProcessFlag,\
	     pObj->trackAlg.algstate.GetNum,\
	     pObj->trackAlg.algstate.FreeNum,\

		 pObj->stutrackAlg.algstate.frames,\
	     Utils_queGetQueuedCount(&pObj->stutrackAlg.stutrackoutFrameBufQue),\
	     pObj->stutrackAlg.ProcessFlag,\
	     pObj->stutrackAlg.algstate.GetNum,\
	     pObj->stutrackAlg.algstate.FreeNum,\

		 pObj->stusidetrackAlg.algstate.frames,\
	     Utils_queGetQueuedCount(&pObj->stusidetrackAlg.stusidetrackoutFrameBufQue),\
	     pObj->stusidetrackAlg.ProcessFlag,\
	     pObj->stusidetrackAlg.algstate.GetNum,\
	     pObj->stusidetrackAlg.algstate.FreeNum,\

		 pObj->studoublesidetrackAlg.algstate.frames,\
	     Utils_queGetQueuedCount(&pObj->studoublesidetrackAlg.studoublesidetrackoutFrameBufQue),\
	     pObj->studoublesidetrackAlg.ProcessFlag,\
	     pObj->studoublesidetrackAlg.algstate.GetNum,\
	     pObj->studoublesidetrackAlg.algstate.FreeNum,\

		 pObj->ceiltrackAlg.algstate.frames,\
	     Utils_queGetQueuedCount(&pObj->ceiltrackAlg.ceiltrackoutFrameBufQue),\
	     pObj->ceiltrackAlg.ProcessFlag,\
	     pObj->ceiltrackAlg.algstate.GetNum,\
	     pObj->ceiltrackAlg.algstate.FreeNum,\

		pObj->trackAlg.chObj[0].trackhandle.t_global_obj.debugValue0,\
	    pObj->trackAlg.chObj[0].trackhandle.t_global_obj.debugValue1,\
	    pObj->trackAlg.chObj[0].trackhandle.t_global_obj.debugValue2,\
	    pObj->studoublesidetrackAlg.chObj[0].studoublesidetrackhandle.t_global_obj.debugValue0,\
	    pObj->studoublesidetrackAlg.chObj[0].studoublesidetrackhandle.t_global_obj.debugValue1,\
	    pObj->studoublesidetrackAlg.chObj[0].studoublesidetrackhandle.t_global_obj.debugValue2,\
	    pObj->studoublesidetrackAlg.chObj[1].studoublesidetrackhandle.t_global_obj.debugValue0,\
	    pObj->studoublesidetrackAlg.chObj[1].studoublesidetrackhandle.t_global_obj.debugValue1,\
	    pObj->studoublesidetrackAlg.chObj[1].studoublesidetrackhandle.t_global_obj.debugValue2,\
	    pObj->ceiltrackAlg.chObj[0].ceiltrackhandle.t_global_obj.debugValue0,\
	    pObj->algstate.Debug);


	return 0;
}


static Int32 AlgLink_createOutObj(AlgLink_Obj * pObj)
{
    AlgLink_OutObj *pOutObj;
    System_LinkChInfo *pOutChInfo;
    Int32 status;
    UInt32 bufIdx;
    Int i,j,queueId,chId;
    UInt32 totalBufCnt;

    for(queueId = 0; queueId < ALG_LINK_MAX_OUT_QUE; queueId++)
    {

        pOutObj = &pObj->outObj[queueId];

        pObj->outObj[queueId].numAllocPools = 1;

        pOutObj->buf_size[0] = UTILS_SCD_GET_OUTBUF_SIZE();
        pOutObj->buf_size[0] = 
          VpsUtils_align(pOutObj->buf_size[0], 
                         SharedRegion_getCacheLineSize(SYSTEM_IPC_SR_CACHED));

        status = Utils_bitbufCreate(&pOutObj->bufOutQue, TRUE, FALSE,
                                    pObj->outObj[queueId].numAllocPools);
        UTILS_assert(status == FVID2_SOK);

        totalBufCnt = 0;
        for (i = 0; i < pOutObj->numAllocPools; i++)
        {
            pOutObj->outNumBufs[i] = (pObj->scdAlg.createArgs.numValidChForSCD * pObj->scdAlg.createArgs.numBufPerCh);

            for (j = 0; j < pObj->scdAlg.createArgs.numValidChForSCD; j++)
            {
                pOutObj->ch2poolMap[j] =  i;
            }

            status = Utils_memBitBufAlloc(&(pOutObj->outBufs[totalBufCnt]),
                                          pOutObj->buf_size[i],
                                          pOutObj->outNumBufs[i]);
            UTILS_assert(status == FVID2_SOK);

            for (bufIdx = 0; bufIdx < pOutObj->outNumBufs[i]; bufIdx++)
            {
                UTILS_assert((bufIdx + totalBufCnt) < ALG_LINK_SCD_MAX_OUT_FRAMES);
                pOutObj->outBufs[bufIdx + totalBufCnt].allocPoolID = i;
                pOutObj->outBufs[bufIdx + totalBufCnt].doNotDisplay =
                    FALSE;
                status =
                    Utils_bitbufPutEmptyBuf(&pOutObj->bufOutQue,
                                            &pOutObj->outBufs[bufIdx +
                                                              totalBufCnt]);
                UTILS_assert(status == FVID2_SOK);
            }
            /* align size to minimum required frame buffer alignment */
            totalBufCnt += pOutObj->outNumBufs[i];
        }
    }
    pObj->info.numQue = ALG_LINK_MAX_OUT_QUE;

    for (queueId = 0u; queueId < ALG_LINK_MAX_OUT_QUE; queueId++)
    {
        pObj->info.queInfo[queueId].numCh = pObj->inQueInfo.numCh;
    }

    for (chId = 0u; chId < pObj->inQueInfo.numCh; chId++)
    {
        for (queueId = 0u; queueId < ALG_LINK_MAX_OUT_QUE; queueId++)
        {
            pOutChInfo = &pObj->info.queInfo[queueId].chInfo[chId];
            pOutChInfo->bufType = SYSTEM_BUF_TYPE_VIDBITSTREAM;
            pOutChInfo->codingformat = NULL;
            pOutChInfo->memType = NULL;
            pOutChInfo->scanFormat = pObj->inQueInfo.chInfo[chId].scanFormat;
            pOutChInfo->width = pObj->inQueInfo.chInfo[chId].width;
            pOutChInfo->height = pObj->inQueInfo.chInfo[chId].height;
        }
    }


    return (status);
}



Int32 AlgLink_algCreate(AlgLink_Obj * pObj, AlgLink_CreateParams * pPrm)
{
    Int32 status;
	UInt32 i = 0;
	Semaphore_Params semParams;
	Semaphore_Params_init(&semParams);
	semParams.mode = Semaphore_Mode_BINARY;
    Vps_printf(" %d: ALG : Create in progress !!!\n", Utils_getCurTimeInMsec());


    UTILS_MEMLOG_USED_START();
    memcpy(&pObj->createArgs, pPrm, sizeof(*pPrm));
    memcpy(&pObj->scdAlg.createArgs, &pPrm->scdCreateParams, sizeof(AlgLink_ScdCreateParams));

	for(i = 0; i < ALG_LINK_TRACK_MAX_CH; i++)
	{
		memcpy(&pObj->trackAlg.trackChCreateParams[i], &pPrm->trackCreateParams, sizeof(AlgLink_TrackCreateParams));
	}

	for(i = 0; i < ALG_LINK_CEILTRACK_MAX_CH; i++)
	{
		memcpy(&pObj->ceiltrackAlg.ceiltrackChCreateParams[i], &pPrm->ceiltrackCreateParams[i], sizeof(AlgLink_CeilTrackCreateParams));
	}

	for(i = 0; i < ALG_LINK_STUTRACK_MAX_CH; i++)
	{
		memcpy(&pObj->stutrackAlg.stutrackChCreateParams[i], &pPrm->stutrackCreateParams, sizeof(AlgLink_StuTrackCreateParams));
	}

	for(i = 0; i < ALG_LINK_STUSIDETRACK_MAX_CH; i++)
	{
		memcpy(&pObj->stusidetrackAlg.stusidetrackChCreateParams[i], &pPrm->stusidetrackCreateParams, sizeof(AlgLink_StuSideTrackCreateParams));
	}

	for(i = 0; i < ALG_LINK_STUDOUBLESIDETRACK_MAX_CH; i++)
	{
		memcpy(&pObj->studoublesidetrackAlg.studoublesidetrackChCreateParams[i], &pPrm->studoublesidetrackCreateParams[i], sizeof(AlgLink_StuDoubleSideTrackCreateParams));
	}

	for(i = 0; i < ALG_LINK_BLACKBOARDTRACK_MAX_CH; i++)
	{
		memcpy(&pObj->blackboardtrackAlg.blackboardtrackChCreateParams[i], &pPrm->blackboardtrackCreateParams[i], sizeof(AlgLink_BlackboardTrackCreateParams));
	}
	
	memcpy(&pObj->osdAlg.osdChCreateParams, &pPrm->osdChCreateParams, (sizeof(AlgLink_OsdChCreateParams) * ALG_LINK_OSD_MAX_CH));

	
	status = System_linkGetInfo(pPrm->inQueParams.prevLinkId, &pObj->inTskInfo);

    UTILS_assert(status == FVID2_SOK);

    UTILS_assert(pPrm->inQueParams.prevLinkQueId < pObj->inTskInfo.numQue);

    memcpy(&pObj->inQueInfo,
           &pObj->inTskInfo.queInfo[pPrm->inQueParams.prevLinkQueId],
           sizeof(pObj->inQueInfo));

    UTILS_assert(pObj->inQueInfo.numCh <= ALG_LINK_OSD_MAX_CH);

    pObj->scdAlg.inQueInfo = &pObj->inQueInfo;

    pObj->osdAlg.inQueInfo = &pObj->inQueInfo;

	pObj->trackAlg.inQueInfo = &pObj->inQueInfo;

	pObj->stutrackAlg.inQueInfo = &pObj->inQueInfo;

	pObj->stusidetrackAlg.inQueInfo = &pObj->inQueInfo;

	pObj->studoublesidetrackAlg.inQueInfo = &pObj->inQueInfo;

	pObj->ceiltrackAlg.inQueInfo = &pObj->inQueInfo;

	pObj->blackboardtrackAlg.inQueInfo = &pObj->inQueInfo;


    if (pObj->createArgs.enableOSDAlg)
    {
        status = AlgLink_OsdalgCreate(&pObj->osdAlg);
        UTILS_assert(status == FVID2_SOK);
    }

    if (pObj->createArgs.enableSCDAlg)
    {
        status = AlgLink_ScdalgCreate(&pObj->scdAlg);
        UTILS_assert(status == FVID2_SOK);
        AlgLink_createOutObj(pObj);
    }

	if (pObj->createArgs.enableTRACKAlg)
    {
		
	 //	Vps_printf("create Done %p %d %d\n",pObj,pObj->trackoutFrameBufQue.count,pObj->trackoutFrameBufQue.maxElements);

        status = AlgLink_TrackalgCreate(&pObj->trackAlg);
        UTILS_assert(status == FVID2_SOK);

		memset(&pObj->trackAlg.trackoutFrameBufQue, 0, sizeof(pObj->trackAlg.trackoutFrameBufQue));
	 	status = Utils_queCreate(&pObj->trackAlg.trackoutFrameBufQue,
                             100,
                            pObj->trackAlg.queueMem,
                             UTILS_QUE_FLAG_NO_BLOCK_QUE);

		UTILS_assert(status == FVID2_SOK);

	    pObj->tracklock = Semaphore_create(1u, &semParams, NULL);
	    UTILS_assert(pObj->tracklock != NULL);

    }

	if(pObj->createArgs.enableSTUTRACKAlg)
	{
		status = AlgLink_StuTrackalgCreate(&pObj->stutrackAlg);
        UTILS_assert(status == FVID2_SOK);

		memset(&pObj->stutrackAlg.stutrackoutFrameBufQue, 0, sizeof(pObj->stutrackAlg.stutrackoutFrameBufQue));
	 	status = Utils_queCreate(&pObj->stutrackAlg.stutrackoutFrameBufQue,
                             100,
                            pObj->stutrackAlg.queueMem,
                             UTILS_QUE_FLAG_NO_BLOCK_QUE);

		UTILS_assert(status == FVID2_SOK);

		pObj->stutracklock = Semaphore_create(1u, &semParams, NULL);
	    UTILS_assert(pObj->stutracklock != NULL);

	}

	if(pObj->createArgs.enableSTUSIDETRACKAlg)
	{
		status = AlgLink_StuSideTrackalgCreate(&pObj->stusidetrackAlg);
        UTILS_assert(status == FVID2_SOK);

		memset(&pObj->stusidetrackAlg.stusidetrackoutFrameBufQue, 0, sizeof(pObj->stusidetrackAlg.stusidetrackoutFrameBufQue));
	 	status = Utils_queCreate(&pObj->stusidetrackAlg.stusidetrackoutFrameBufQue,
                             100,
                            pObj->stusidetrackAlg.queueMem,
                             UTILS_QUE_FLAG_NO_BLOCK_QUE);

		UTILS_assert(status == FVID2_SOK);

		pObj->stusidetracklock = Semaphore_create(1u, &semParams, NULL);
	    UTILS_assert(pObj->stusidetracklock != NULL);
	}


	if(pObj->createArgs.enableSTUDOUBLESIDETRACKAlg)
	{
		
		status = AlgLink_StuDoubleSideTrackalgCreate(&pObj->studoublesidetrackAlg);
        UTILS_assert(status == FVID2_SOK);

		memset(&pObj->studoublesidetrackAlg.studoublesidetrackoutFrameBufQue, 0, sizeof(pObj->studoublesidetrackAlg.studoublesidetrackoutFrameBufQue));
	 	status = Utils_queCreate(&pObj->studoublesidetrackAlg.studoublesidetrackoutFrameBufQue,
                             100,
                            pObj->studoublesidetrackAlg.queueMem,
                             UTILS_QUE_FLAG_NO_BLOCK_QUE);

		UTILS_assert(status == FVID2_SOK);

		pObj->studoublesidetracklock = Semaphore_create(1u, &semParams, NULL);
	    UTILS_assert(pObj->studoublesidetracklock != NULL);
	}

	if(pObj->createArgs.enableCEILTRACKAlg)
	{
		
		status = AlgLink_CeilTrackalgCreate(&pObj->ceiltrackAlg);
        UTILS_assert(status == FVID2_SOK);

		memset(&pObj->ceiltrackAlg.ceiltrackoutFrameBufQue, 0, sizeof(pObj->ceiltrackAlg.ceiltrackoutFrameBufQue));
	 	status = Utils_queCreate(&pObj->ceiltrackAlg.ceiltrackoutFrameBufQue,
                             100,
                            pObj->ceiltrackAlg.queueMem,
                             UTILS_QUE_FLAG_NO_BLOCK_QUE);

		UTILS_assert(status == FVID2_SOK);

		pObj->ceiltracklock = Semaphore_create(1u, &semParams, NULL);
	    UTILS_assert(pObj->ceiltracklock != NULL);
	}

	if(pObj->createArgs.enableBLACKBOARDTRACKAlg)
	{
		
		status = AlgLink_BlackboardTrackalgCreate(&pObj->blackboardtrackAlg);
        UTILS_assert(status == FVID2_SOK);

		memset(&pObj->blackboardtrackAlg.blackboardtrackoutFrameBufQue, 0, sizeof(pObj->blackboardtrackAlg.blackboardtrackoutFrameBufQue));
	 	status = Utils_queCreate(&pObj->blackboardtrackAlg.blackboardtrackoutFrameBufQue,
                             100,
                            pObj->blackboardtrackAlg.queueMem,
                             UTILS_QUE_FLAG_NO_BLOCK_QUE);

		UTILS_assert(status == FVID2_SOK);

		//pObj->blackboardtracklock = Semaphore_create(1u, &semParams, NULL);
	    //UTILS_assert(pObj->ceiltracklock != NULL);
	}
	

    pObj->isCreated = ALG_LINK_STATE_ACTIVE;

    UTILS_MEMLOG_USED_END(pObj->memUsed);
    UTILS_MEMLOG_PRINT("ALGLINK",
                       pObj->memUsed,
                       UTILS_ARRAYSIZE(pObj->memUsed));
    Vps_printf(" %d: ALG : Create Done !!!\n", Utils_getCurTimeInMsec());
    return FVID2_SOK;
}

Int32 AlgLink_algDelete(AlgLink_Obj * pObj)
{
    Int32 status;
    Int32 i,outId,bitbuf_index;
    AlgLink_OutObj *pOutObj;

    Vps_printf(" %d: ALG : Delete in progress !!!\n", Utils_getCurTimeInMsec());


    if (pObj->createArgs.enableOSDAlg)
    {
        status = AlgLink_OsdalgDelete(&pObj->osdAlg);
        UTILS_assert(status == FVID2_SOK);
    }

    if (pObj->createArgs.enableSCDAlg)
    {
        status = AlgLink_ScdalgDelete(&pObj->scdAlg);
        UTILS_assert(status == FVID2_SOK);
        for (outId = 0; outId < ALG_LINK_MAX_OUT_QUE; outId++)
        {
            {
                pOutObj = &pObj->outObj[outId];

                status = Utils_bitbufDelete(&pOutObj->bufOutQue);
                UTILS_assert(status == FVID2_SOK);
                bitbuf_index = 0;
                for (i = 0; i < pOutObj->numAllocPools; i++)
                {
                    UTILS_assert((pOutObj->outBufs[bitbuf_index].bufSize ==
                                  pOutObj->buf_size[i]));
                    status = Utils_memBitBufFree(&pOutObj->outBufs[bitbuf_index],
                                        pOutObj->outNumBufs[i]);
                    UTILS_assert(status == FVID2_SOK);
                    bitbuf_index += pOutObj->outNumBufs[i];
                }
            }
        }
    }
    pObj->isCreated = ALG_LINK_STATE_INACTIVE;
    Vps_printf(" %d: ALG : Delete Done !!!\n", Utils_getCurTimeInMsec());

	return FVID2_SOK;
}


Int32 AlgLink_algProcessData(AlgLink_Obj * pObj)
{
    UInt32 frameId, status;
    System_LinkInQueParams *pInQueParams;
    FVID2_Frame *pFrame;

    FVID2_FrameList frameList;
	FVID2_FrameList freeframeList = {0};
	static UInt32 processframes = 0;
	static UInt32 StartTime = 0;
    Int32 chid = 0;
	pObj->algstate.Debug = 1;
    pInQueParams = &pObj->createArgs.inQueParams;
    System_getLinksFullFrames(pInQueParams->prevLinkId,
                              pInQueParams->prevLinkQueId, &frameList);

    if (frameList.numFrames)
    {
		//AlgLink_printStatistics (pObj);
		pObj->algstate.GetNum = frameList.numFrames + pObj->algstate.GetNum;
		processframes = frameList.numFrames + processframes;
		if ((Utils_getCurTimeInMsec() - StartTime) > 1 * 1000)
	    {
		
			pObj->algstate.frames = processframes;
			Vps_printf("Alg Process Done %dfps [%d]......[%d][%d] \n",processframes,(Utils_getCurTimeInMsec() - StartTime),
				Utils_queGetQueuedCount(&pObj->trackAlg.trackoutFrameBufQue),Utils_queGetQueuedCount(&pObj->stutrackAlg.stutrackoutFrameBufQue));
			StartTime = Utils_getCurTimeInMsec();
			processframes = 0;

	    }

		
		//UTILS_assert(frameList.numFrames == 1);
        /* SCD should be done first as it requires to operate on raw YUV */
        if (pObj->createArgs.enableSCDAlg)
        {
			pObj->algstate.Debug = 2;
            status = AlgLink_ScdalgProcessData(&pObj->scdAlg, &frameList, &pObj->outObj[0].bufOutQue);

			#if 0
            if (status == FVID2_SOK)
            {
                /* Send-out the output bitbuffer */
                System_sendLinkCmd(pObj->createArgs.outQueParams[ALG_LINK_SCD_OUT_QUE].nextLink,
                                   SYSTEM_CMD_NEW_DATA);
            }
			#endif
			pObj->algstate.Debug = 3;

        }
		pObj->algstate.Debug = 4;
        for(frameId=0; frameId<frameList.numFrames; frameId++)
        {
			UInt32 IsFree = FALSE;
            pFrame = frameList.frames[frameId];

            if(pFrame->channelNum >= pObj->inQueInfo.numCh)
            {
				UTILS_assert(0);
                continue;
            }

            // do SW OSD
            if (pObj->createArgs.enableOSDAlg)
            {
                AlgLink_OsdalgProcessFrame(&pObj->osdAlg, pFrame);
            }


			//do track
            if (pObj->createArgs.enableTRACKAlg && !IsFree)
            {
					AlgLink_TrackObj	*pChobj = &pObj->trackAlg;
					if((pFrame->channelNum >= ALG_LINK_TRACK_MAX_TO_CH) 
						|| (pChobj->ChToStream[pFrame->channelNum] >= ALG_LINK_TRACK_MAX_CH)
						|| (pChobj->ChToStream[pFrame->channelNum] < 0)
						)
				  	{
						
					}
					else
					{	
					
	
						if(Utils_queGetQueuedCount(&pObj->trackAlg.trackoutFrameBufQue) <= 2)
	
						{							
		                 	status = Utils_quePut(&pObj->trackAlg.trackoutFrameBufQue, pFrame, BIOS_NO_WAIT);
		      				UTILS_assert(status == FVID2_SOK);

							System_sendLinkCmd(SYSTEM_LINK_ID_TRACK,SYSTEM_CMD_NEW_DATA);

							pObj->trackAlg.algstate.GetNum++;
		                 	IsFree = TRUE;
						}
						else
						{
							Vps_printf("trackAlg.trackoutFrameBufQue = %d\n",Utils_queGetQueuedCount(&pObj->trackAlg.trackoutFrameBufQue));
						}
					}
             
            }

			//do stutrack
			if (pObj->createArgs.enableSTUTRACKAlg && !IsFree)
            {				
               AlgLink_StuTrackObj	*pChobj = &pObj->stutrackAlg;
			   if((pFrame->channelNum >= ALG_LINK_STUTRACK_MAX_TO_CH) 
					|| (pChobj->ChToStream[pFrame->channelNum] >= ALG_LINK_STUTRACK_MAX_CH)
					|| (pChobj->ChToStream[pFrame->channelNum] < 0)
					)
			  	{
					
				}
				else
				{	
					if(Utils_queGetQueuedCount(&pObj->stutrackAlg.stutrackoutFrameBufQue) <= 2)
					{
	                 	status = Utils_quePut(&pObj->stutrackAlg.stutrackoutFrameBufQue, pFrame, BIOS_NO_WAIT);
	      				UTILS_assert(status == FVID2_SOK);
						System_sendLinkCmd(SYSTEM_LINK_ID_STUTRACK,SYSTEM_CMD_NEW_DATA);
						pObj->stutrackAlg.algstate.GetNum++;
	                 	IsFree = TRUE;
					}
					else
					{
						 Vps_printf("pObj->stutrackAlg.stutrackoutFrameBufQue = %d\n",Utils_queGetQueuedCount(&pObj->stutrackAlg.stutrackoutFrameBufQue));
					}
				}
   
            }

			

			//do stusidetrack
			if (pObj->createArgs.enableSTUSIDETRACKAlg && !IsFree)
            {				
               AlgLink_StuSideTrackObj	*pChobj = &pObj->stusidetrackAlg;
			   if((pFrame->channelNum >= ALG_LINK_STUSIDETRACK_MAX_TO_CH) 
					|| (pChobj->ChToStream[pFrame->channelNum] >= ALG_LINK_STUSIDETRACK_MAX_CH)
					|| (pChobj->ChToStream[pFrame->channelNum] < 0)
					)
			  	{
					
				}
				else
				{	
					AlgLink_StuSideTrackObj *pStuSideAlg = &pObj->stusidetrackAlg;
			
					if(Utils_queGetQueuedCount(&pStuSideAlg->stusidetrackoutFrameBufQue) <= 2)

					{				
	                 	status = Utils_quePut(&pStuSideAlg->stusidetrackoutFrameBufQue, pFrame, BIOS_NO_WAIT);
						UTILS_assert(status == FVID2_SOK);
						System_sendLinkCmd(SYSTEM_LINK_ID_STUSIDETRACK,SYSTEM_CMD_NEW_DATA);
						pStuSideAlg->algstate.GetNum++;
	                 	IsFree = TRUE;
					}
					else
					{
						Vps_printf("pObj->stusidetrackAlg.stusidetrackoutFrameBufQue = %d\n",Utils_queGetQueuedCount(&pStuSideAlg->stusidetrackoutFrameBufQue));
					}
				}
   
            }

			//do studoublesidetrack
			if (pObj->createArgs.enableSTUDOUBLESIDETRACKAlg && !IsFree)
            {				
               AlgLink_StuDoubleSideTrackObj	*pStuDoubleSideobj = &pObj->studoublesidetrackAlg;
			   for(chid = 0; chid < ALG_LINK_STUDOUBLESIDETRACK_MAX_CH; chid++)
			   {
			   		if(pFrame->channelNum == pStuDoubleSideobj->StreamId[chid] && !IsFree)
			   		{
						if(Utils_queGetQueuedCount(&pStuDoubleSideobj->studoublesidetrackoutFrameBufQue) <= 2)
						{				
		                 	status = Utils_quePut(&pStuDoubleSideobj->studoublesidetrackoutFrameBufQue, pFrame, BIOS_NO_WAIT);
							UTILS_assert(status == FVID2_SOK);
							System_sendLinkCmd(SYSTEM_LINK_ID_STUDOUBLESIDETRACK,SYSTEM_CMD_NEW_DATA);
							pStuDoubleSideobj->algstate.GetNum++;
		                 	IsFree = TRUE;
						}
						else
						{
							 Vps_printf("pObj->studoublesidetrackAlg.stusidetrackoutFrameBufQue = %d\n",Utils_queGetQueuedCount(&pStuDoubleSideobj->studoublesidetrackoutFrameBufQue));
						}
					}
				}
			}
   
      
			if (pObj->createArgs.enableCEILTRACKAlg && !IsFree)
            {				
               AlgLink_CeilTrackObj	*pCeilTrackobj = &pObj->ceiltrackAlg;
			   for(chid = 0; chid < ALG_LINK_CEILTRACK_MAX_CH; chid++)
			   {
			   		if(pFrame->channelNum == pCeilTrackobj->StreamId[chid] && !IsFree)
			   		{
						if(Utils_queGetQueuedCount(&pCeilTrackobj->ceiltrackoutFrameBufQue) <= 2)
						{				
		                 	status = Utils_quePut(&pCeilTrackobj->ceiltrackoutFrameBufQue, pFrame, BIOS_NO_WAIT);
							UTILS_assert(status == FVID2_SOK);
							System_sendLinkCmd(SYSTEM_LINK_ID_CEILTRACK,SYSTEM_CMD_NEW_DATA);
							pCeilTrackobj->algstate.GetNum++;
		                 	IsFree = TRUE;
						}
						else
						{
							 Vps_printf("pObj->ceiltrackAlg.ceiltrackoutFrameBufQue = %d\n",Utils_queGetQueuedCount(&pCeilTrackobj->ceiltrackoutFrameBufQue));
						}
						
					}
				}
				
			}

			if (pObj->createArgs.enableBLACKBOARDTRACKAlg && !IsFree)
            {				
               AlgLink_BlackboardTrackObj	*pBlackboardTrackobj = &pObj->blackboardtrackAlg;
			   for(chid = 0; chid < ALG_LINK_BLACKBOARDTRACK_MAX_CH; chid++)
			   {
			   		if(pFrame->channelNum == pBlackboardTrackobj->StreamId[chid] && !IsFree)
			   		{
						if(Utils_queGetQueuedCount(&pBlackboardTrackobj->blackboardtrackoutFrameBufQue) <= 2)
						{				
		                 	status = Utils_quePut(&pBlackboardTrackobj->blackboardtrackoutFrameBufQue, pFrame, BIOS_NO_WAIT);
							UTILS_assert(status == FVID2_SOK);
							System_sendLinkCmd(SYSTEM_LINK_ID_BLACKBOARDTRACK,SYSTEM_CMD_NEW_DATA);
							pBlackboardTrackobj->algstate.GetNum++;
		                 	IsFree = TRUE;
						}
						else
						{
							 Vps_printf("pObj->blackboardtrackAlg.blackboardtrackoutFrameBufQue = %d\n",Utils_queGetQueuedCount(&pBlackboardTrackobj->blackboardtrackoutFrameBufQue));
						}
						
					}
				}
				
			}
   				
			
			//ÒÑÊÍ·Å
			if(IsFree)
			{
	
			}
			//ÊÍ·Å
			else
			{
				freeframeList.frames[freeframeList.numFrames] = pFrame;
				freeframeList.numFrames++;
			}
			
			
        }

		pObj->algstate.Debug = 5;

		if(freeframeList.numFrames > 0)
		{
			pObj->algstate.FreeNum = freeframeList.numFrames + pObj->algstate.FreeNum;
       		 System_putLinksEmptyFrames(pInQueParams->prevLinkId,
                                   pInQueParams->prevLinkQueId, &freeframeList);
		}
    }
	pObj->algstate.Debug =6;
    return FVID2_SOK;
}
