/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include "algLink_priv.h"

#pragma DATA_ALIGN(gAlgLink_tskStack, 32)
#pragma DATA_SECTION(gAlgLink_tskStack, ".bss:taskStackSection")
UInt8 gAlgLink_tskStack[ALG_LINK_OBJ_MAX][ALG_LINK_TSK_STACK_SIZE];
UInt8 gAlgTrackLink_tskStack[ALG_LINK_TSK_STACK_SIZE];
UInt8 gAlgstuTrackLink_tskStack[ALG_LINK_TSK_STACK_SIZE];
UInt8 gAlgstusideTrackLink_tskStack[ALG_LINK_TSK_STACK_SIZE];
UInt8 gAlgstudoublesideTrackLink_tskStack[ALG_LINK_TSK_STACK_SIZE];
UInt8 gAlgCeilTrackLink_tskStack[ALG_LINK_TSK_STACK_SIZE];
UInt8 gAlgBlackboardTrackLink_tskStack[ALG_LINK_TSK_STACK_SIZE];
UInt8 gAlgTrackDebugLink_tskStack[ALG_LINK_TSK_STACK_SIZE];

AlgLink_Obj gAlgLink_obj[ALG_LINK_OBJ_MAX];
UInt32 gAlgLinkRunCount[ALG_LINK_OBJ_MAX];

Void AlgLink_tracktskMain(struct Utils_TskHndl *pTsk, Utils_MsgHndl * pMsg)
{
	 UInt32 cmd = Utils_msgGetCmd(pMsg);
	 Bool ackMsg, done;
	 Int32 status;
	 done = FALSE;
     ackMsg = FALSE;
	 AlgLink_Obj *pObj;
  	
     pObj = (AlgLink_Obj *) pTsk->appData;

	 Vps_printf("AlgLink_tracktskMain Done!\n");

	 while (!done)
   	 {
        status = Utils_tskRecvMsg(pTsk, &pMsg, BIOS_WAIT_FOREVER);
        if (status != FVID2_SOK)
            break;

        cmd = Utils_msgGetCmd(pMsg);

        switch (cmd)
        {
            case SYSTEM_CMD_NEW_DATA:
			{
				
              	  Utils_tskAckOrFreeMsg(pMsg, status);

				  FVID2_Frame        *frames;
				  Int32 status;		
				  pObj->trackAlg.ProcessFlag = 1;
				  status = Utils_queGet(&pObj->trackAlg.trackoutFrameBufQue, (Ptr *) & frames, 1, BIOS_NO_WAIT);
	         	  if (status == FVID2_SOK)
	         	  {
					 	 System_LinkInQueParams *pInQueParams;
						 pInQueParams = &pObj->createArgs.inQueParams;
						 FVID2_FrameList frameList;
						 frameList.numFrames = 1;
						 frameList.frames[0] = frames;

						 AlgLink_TrackalgProcessFrame(&pObj->trackAlg, frames);
 
						 System_putLinksEmptyFrames(pInQueParams->prevLinkId,
	                                   pInQueParams->prevLinkQueId, &frameList);
						 pObj->trackAlg.algstate.FreeNum = frameList.numFrames + pObj->trackAlg.algstate.FreeNum;
	                  
	         	  }
				 
				  pObj->trackAlg.ProcessFlag = 0;
				  
            }
			break;
			case ALG_LINK_TRACK_CMD_SET_CHANNEL_WIN_PRM:
			{
				AlgLink_TrackCreateParams *params;
				AlgLink_TrackCreateParams *pChPrm;
				AlgLink_TrackChObj *pChObj;

				//Semaphore_pend(pObj->tracklock, BIOS_WAIT_FOREVER);
				params = (AlgLink_TrackCreateParams *) Utils_msgGetPrm(pMsg);
				if(params->chId < ALG_LINK_TRACK_MAX_CH)
				{
					pChPrm = &pObj->trackAlg.trackChCreateParams[params->chId];
					pChObj = &pObj->trackAlg.chObj[params->chId];
					pChPrm->chId = params->chId;
					memcpy(&pChPrm->TrackParms, &params->TrackParms, sizeof(ITRACK_Params));
					pChObj->rtParamUpdatePerFrame = TRUE;
				}
				//Semaphore_post(pObj->tracklock);

				Utils_tskAckOrFreeMsg(pMsg, status);
			}
            break;
			case ALG_LINK_TRACK_CMD_SAVE_VIEW:
			{
				AlgLink_TrackSaveView *params;
				params = (AlgLink_TrackSaveView *) Utils_msgGetPrm(pMsg);

				//Semaphore_pend(pObj->tracklock, BIOS_WAIT_FOREVER);
				if(params->chId < ALG_LINK_TRACK_MAX_CH)
				{
					pObj->trackAlg.trackChCreateParams[params->chId].SaveViewFlag = TRUE;
				}
				//Semaphore_post(pObj->tracklock);

				Utils_tskAckOrFreeMsg(pMsg, status);
			}
			break;
			default:
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }

    if (ackMsg && pMsg != NULL)
        Utils_tskAckOrFreeMsg(pMsg, status);

    return;
}

Void AlgLink_stutracktskMain(struct Utils_TskHndl *pTsk, Utils_MsgHndl * pMsg)
{
	 UInt32 cmd = Utils_msgGetCmd(pMsg);
	 Bool ackMsg, done;
	 Int32 status;
	 done = FALSE;
     ackMsg = FALSE;
	 AlgLink_Obj *pObj;
  	
     pObj = (AlgLink_Obj *) pTsk->appData;
 	 
	Vps_printf("AlgLink_stutracktskMain Done!\n");
	 while (!done)
   	 {
        status = Utils_tskRecvMsg(pTsk, &pMsg, BIOS_WAIT_FOREVER);
        if (status != FVID2_SOK)
            break;

        cmd = Utils_msgGetCmd(pMsg);

        switch (cmd)
        {
            case SYSTEM_CMD_NEW_DATA:
			{
			      Utils_tskAckOrFreeMsg(pMsg, status);
				  FVID2_Frame        *frames;
				  Int32 status;
				  pObj->stutrackAlg.ProcessFlag = 1;
				  status = Utils_queGet(&pObj->stutrackAlg.stutrackoutFrameBufQue, (Ptr *) & frames, 1, BIOS_NO_WAIT);
	         	  if (status == FVID2_SOK)
	         	  {
					 	 System_LinkInQueParams *pInQueParams;
						 pInQueParams = &pObj->createArgs.inQueParams;
						 FVID2_FrameList frameList;
						 frameList.numFrames = 1;
						 frameList.frames[0] = frames;

						 AlgLink_StuTrackalgProcessFrame(&pObj->stutrackAlg, frames);
 
						 System_putLinksEmptyFrames(pInQueParams->prevLinkId,
	                                   pInQueParams->prevLinkQueId, &frameList);
						  pObj->stutrackAlg.algstate.FreeNum = frameList.numFrames + pObj->stutrackAlg.algstate.FreeNum;
	                  
	         	  }
				  pObj->stutrackAlg.ProcessFlag = 0;
            }
			break;
			case ALG_LINK_STUTRACK_CMD_SET_CHANNEL_WIN_PRM:
			{
				AlgLink_StuTrackCreateParams *params;
				AlgLink_StuTrackCreateParams *pChPrm;
				AlgLink_StuTrackChObj *pChObj;
				  
				params = (AlgLink_StuTrackCreateParams *) Utils_msgGetPrm(pMsg);
				if(params->chId < ALG_LINK_STUTRACK_MAX_CH)
				{
					pChPrm = &pObj->stutrackAlg.stutrackChCreateParams[params->chId];
					pChObj = &pObj->stutrackAlg.chObj[params->chId];
					pChPrm->chId = params->chId;
					memcpy(&pChPrm->StuTrackParms, &params->StuTrackParms, sizeof(StuITRACK_Params));
					pChObj->rtParamUpdatePerFrame = TRUE;
				}

				Utils_tskAckOrFreeMsg(pMsg, status);
			}
            break;
			default:
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }

    if (ackMsg && pMsg != NULL)
        Utils_tskAckOrFreeMsg(pMsg, status);

    return;
}

Void AlgLink_stusidetracktskMain(struct Utils_TskHndl *pTsk, Utils_MsgHndl * pMsg)
{
	 UInt32 cmd = Utils_msgGetCmd(pMsg);
	 Bool ackMsg, done;
	 Int32 status;
	 done = FALSE;
     ackMsg = FALSE;
	 AlgLink_Obj *pObj;
  	
     pObj = (AlgLink_Obj *) pTsk->appData;
 	 
	Vps_printf("AlgLink_stusidetracktskMain Done!\n");
	 while (!done)
   	 {
        status = Utils_tskRecvMsg(pTsk, &pMsg, BIOS_WAIT_FOREVER);
        if (status != FVID2_SOK)
            break;

        cmd = Utils_msgGetCmd(pMsg);

        switch (cmd)
        {
            case SYSTEM_CMD_NEW_DATA:
			{
			      Utils_tskAckOrFreeMsg(pMsg, status);
				  FVID2_Frame        *frames;
				  Int32 status;
				  pObj->stusidetrackAlg.ProcessFlag = 1;
				  status = Utils_queGet(&pObj->stusidetrackAlg.stusidetrackoutFrameBufQue, (Ptr *) & frames, 1, BIOS_NO_WAIT);
	         	  if (status == FVID2_SOK)
	         	  {
					 	 System_LinkInQueParams *pInQueParams;
						 pInQueParams = &pObj->createArgs.inQueParams;
						 FVID2_FrameList frameList;
						 frameList.numFrames = 1;
						 frameList.frames[0] = frames;

						AlgLink_StuSideTrackalgProcessFrame(&pObj->stusidetrackAlg, frames);
 
						 System_putLinksEmptyFrames(pInQueParams->prevLinkId,
	                                   pInQueParams->prevLinkQueId, &frameList);
						  pObj->stusidetrackAlg.algstate.FreeNum = frameList.numFrames + pObj->stusidetrackAlg.algstate.FreeNum;
	                  
	         	  }
				  pObj->stusidetrackAlg.ProcessFlag = 0;
            }
            break;
			case ALG_LINK_STUSIDETRACK_CMD_SET_CHANNEL_WIN_PRM:
			{
				AlgLink_StuSideTrackCreateParams *params;
				AlgLink_StuSideTrackCreateParams *pChPrm;
				AlgLink_StuSideTrackChObj *pChObj;

				//Semaphore_pend(pObj->stusidetracklock, BIOS_WAIT_FOREVER);
				params = (AlgLink_StuSideTrackCreateParams *) Utils_msgGetPrm(pMsg);
				if(params->chId < ALG_LINK_STUSIDETRACK_MAX_CH)
				{
					pChPrm = &pObj->stusidetrackAlg.stusidetrackChCreateParams[params->chId];
					pChObj = &pObj->stusidetrackAlg.chObj[params->chId];
					pChPrm->chId = params->chId;
					memcpy(&pChPrm->StuSideTrackParms, &params->StuSideTrackParms, sizeof(StuSideITRACK_Params));
					pChObj->rtParamUpdatePerFrame = TRUE;
				}
				//Semaphore_post(pObj->stusidetracklock);

				Utils_tskAckOrFreeMsg(pMsg, status);
			}
			break;
			case ALG_LINK_STUSIDETRACK_CMD_SAVE_VIEW:
			{
				AlgLink_StuSideTrackSaveView *params;
				params = (AlgLink_StuSideTrackSaveView *) Utils_msgGetPrm(pMsg);

				//Semaphore_pend(pObj->stusidetracklock, BIOS_WAIT_FOREVER);
				if(params->chId < ALG_LINK_STUSIDETRACK_MAX_CH)
				{
					pObj->stusidetrackAlg.stusidetrackChCreateParams[params->chId].SaveViewFlag = TRUE;
				}
				//Semaphore_post(pObj->stusidetracklock);
				
				Utils_tskAckOrFreeMsg(pMsg, status);
			}
			break;
			default:
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }

    if (ackMsg && pMsg != NULL)
        Utils_tskAckOrFreeMsg(pMsg, status);

    return;
}

Void AlgLink_studoublesidetracktskMain(struct Utils_TskHndl *pTsk, Utils_MsgHndl * pMsg)
{
	 UInt32 cmd = Utils_msgGetCmd(pMsg);
	 Bool ackMsg, done;
	 Int32 status;
	 done = FALSE;
     ackMsg = FALSE;
	 AlgLink_Obj *pObj;
  	
     pObj = (AlgLink_Obj *) pTsk->appData;
 	 
	 Vps_printf("AlgLink_studoublesidetracktskMain Done!\n");
	 while (!done)
   	 {
        status = Utils_tskRecvMsg(pTsk, &pMsg, BIOS_WAIT_FOREVER);
        if (status != FVID2_SOK)
            break;

        cmd = Utils_msgGetCmd(pMsg);

        switch (cmd)
        {
            case SYSTEM_CMD_NEW_DATA:
			{
			      Utils_tskAckOrFreeMsg(pMsg, status);
				  FVID2_Frame        *frames;
				  Int32 status;
				  pObj->studoublesidetrackAlg.ProcessFlag = 1;
				  status = Utils_queGet(&pObj->studoublesidetrackAlg.studoublesidetrackoutFrameBufQue, (Ptr *) & frames, 1, BIOS_NO_WAIT);
	         	  if (status == FVID2_SOK)
	         	  {
					 	 System_LinkInQueParams *pInQueParams;
						 pInQueParams = &pObj->createArgs.inQueParams;
						 FVID2_FrameList frameList;
						 frameList.numFrames = 1;
						 frameList.frames[0] = frames;

						 AlgLink_StuDoubleSideTrackalgProcessFrame(&pObj->studoublesidetrackAlg, frames);
 
						 System_putLinksEmptyFrames(pInQueParams->prevLinkId,
	                                   pInQueParams->prevLinkQueId, &frameList);
						  pObj->studoublesidetrackAlg.algstate.FreeNum = frameList.numFrames + pObj->studoublesidetrackAlg.algstate.FreeNum;
	                  
	         	  }
				  pObj->studoublesidetrackAlg.ProcessFlag = 0;
            }
            break;
			case ALG_LINK_STUDOUBLESIDETRACK_CMD_SET_CHANNEL_WIN_PRM:
			{
	
				AlgLink_StuDoubleSideTrackCreateParams *params;
				AlgLink_StuDoubleSideTrackCreateParams *pChPrm;
				AlgLink_StuDoubleSideTrackChObj *pChObj;

				//Semaphore_pend(pObj->studoublesidetracklock, BIOS_WAIT_FOREVER);
				params = (AlgLink_StuDoubleSideTrackCreateParams *) Utils_msgGetPrm(pMsg);
				if(params->chId < ALG_LINK_STUDOUBLESIDETRACK_MAX_CH)
				{
					pChPrm = &pObj->studoublesidetrackAlg.studoublesidetrackChCreateParams[params->chId];
					pChObj = &pObj->studoublesidetrackAlg.chObj[params->chId];
					pChPrm->chId = params->chId;
					memcpy(&pChPrm->StuDoubleSideTrackParms, &params->StuDoubleSideTrackParms, sizeof(StuDoubleSideITRACK_Params));
					pChObj->rtParamUpdatePerFrame = TRUE;
				}
				//Semaphore_post(pObj->studoublesidetracklock);

				Utils_tskAckOrFreeMsg(pMsg, status);
			}
            break;
			case ALG_LINK_STUDOUBLESIDETRACK_CMD_SAVE_VIEW:
			{
				AlgLink_StuDoubleSideTrackSaveView *params;
				params = (AlgLink_StuDoubleSideTrackSaveView *) Utils_msgGetPrm(pMsg);

				//Semaphore_pend(pObj->studoublesidetracklock, BIOS_WAIT_FOREVER);
				if(params->chId < ALG_LINK_STUDOUBLESIDETRACK_MAX_CH)
				{
					memcpy(&pObj->studoublesidetrackAlg.studoublesidetrackChCreateParams[params->chId].SaveView, params, sizeof(AlgLink_StuDoubleSideTrackSaveView));
				}
				//Semaphore_post(pObj->studoublesidetracklock);
				Utils_tskAckOrFreeMsg(pMsg, status);
			}
			break;
			default:
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }

    if (ackMsg && pMsg != NULL)
        Utils_tskAckOrFreeMsg(pMsg, status);

    return;
}

Void AlgLink_ceiltracktskMain(struct Utils_TskHndl *pTsk, Utils_MsgHndl * pMsg)
{
	 UInt32 cmd = Utils_msgGetCmd(pMsg);
	 Bool ackMsg, done;
	 Int32 status;
	 done = FALSE;
     ackMsg = FALSE;
	 AlgLink_Obj *pObj;
  	
     pObj = (AlgLink_Obj *) pTsk->appData;
 	 
	 Vps_printf("AlgLink_ceiltracktskMain Done!\n");
	 while (!done)
   	 {
        status = Utils_tskRecvMsg(pTsk, &pMsg, BIOS_WAIT_FOREVER);
        if (status != FVID2_SOK)
            break;

        cmd = Utils_msgGetCmd(pMsg);

        switch (cmd)
        {
            case SYSTEM_CMD_NEW_DATA:
			{
			      Utils_tskAckOrFreeMsg(pMsg, status);
				  FVID2_Frame        *frames;
				  Int32 status;
				  pObj->ceiltrackAlg.ProcessFlag = 1;
				  status = Utils_queGet(&pObj->ceiltrackAlg.ceiltrackoutFrameBufQue, (Ptr *) & frames, 1, BIOS_NO_WAIT);
	         	  if (status == FVID2_SOK)
	         	  {
					 	 System_LinkInQueParams *pInQueParams;
						 pInQueParams = &pObj->createArgs.inQueParams;
						 FVID2_FrameList frameList;
						 frameList.numFrames = 1;
						 frameList.frames[0] = frames;

						 AlgLink_CeilTrackalgProcessFrame(&pObj->ceiltrackAlg, frames);
 
						 System_putLinksEmptyFrames(pInQueParams->prevLinkId,
	                                   pInQueParams->prevLinkQueId, &frameList);
						  pObj->ceiltrackAlg.algstate.FreeNum = frameList.numFrames + pObj->ceiltrackAlg.algstate.FreeNum;
	                  
	         	  }
				  pObj->ceiltrackAlg.ProcessFlag = 0;
            }
            break;
			case ALG_LINK_CEIL_TRACK_CMD_SET_CHANNEL_WIN_PRM:
			{
				
				AlgLink_CeilTrackCreateParams *params;
				AlgLink_CeilTrackCreateParams *pChPrm;
				AlgLink_CeilTrackChObj *pChObj;
		
				//Semaphore_pend(pObj->ceiltracklock, BIOS_WAIT_FOREVER);
				params = (AlgLink_CeilTrackCreateParams *) Utils_msgGetPrm(pMsg);
				Vps_printf("ALG_LINK_CEIL_TRACK_CMD_SET_CHANNEL_WIN_PRM 11111 %d",params->chId);
				if(params->chId < ALG_LINK_CEILTRACK_MAX_CH)
				{
					pChPrm = &pObj->ceiltrackAlg.ceiltrackChCreateParams[params->chId];
					pChObj = &pObj->ceiltrackAlg.chObj[params->chId];
					pChPrm->chId = params->chId;
					memcpy(&pChPrm->CeilTrackParms, &params->CeilTrackParms, sizeof(CeilITRACK_Params));
					pChObj->rtParamUpdatePerFrame = TRUE;

					CeilITRACK_Params *pTrackParms = &params->CeilTrackParms;
					Vps_printf("CeilTrackAddr[%p] size[%d] \npic_width[%d] pic_height[%d] video_width[%d] video_height[%d]\n"\
							   "reset_time[%d] message[%d]   reset_level[%d]\n"\
							    "sens[%d] track_point_num[%d] control_mode[%d]"\
							    "trigger [%d:%d] [%d:%d]"\
								,pChPrm->CeilTrackAddr[0][0],
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
								pTrackParms->dynamic_param.trigger[0].trigger_y1
								);
					//Vps_printf("ALG_LINK_CEIL_TRACK_CMD_SET_CHANNEL_WIN_PRM 22222 %d %d\n",params->CeilTrackParms.static_param.video_width,params->CeilTrackParms.static_param.video_height);
				}
				//Semaphore_post(pObj->ceiltracklock);

				Utils_tskAckOrFreeMsg(pMsg, status);
				Vps_printf("ALG_LINK_CEIL_TRACK_CMD_SET_CHANNEL_WIN_PRM 22222");
			}
			break;
			case ALG_LINK_CEIL_TRACK_CMD_SAVE_VIEW:
			{
			
				AlgLink_CeilTrackSaveView *params;
				params = (AlgLink_CeilTrackSaveView *) Utils_msgGetPrm(pMsg);

				Vps_printf("ALG_LINK_CEIL_TRACK_CMD_SAVE_VIEW 1111111 %d\n",params->chId);
				//Semaphore_pend(pObj->ceiltracklock, BIOS_WAIT_FOREVER);
				if(params->chId < ALG_LINK_CEILTRACK_MAX_CH)
				{
					memcpy(&pObj->ceiltrackAlg.ceiltrackChCreateParams[params->chId].SaveView, params, sizeof(AlgLink_CeilTrackSaveView));
				}
				//Semaphore_post(pObj->ceiltracklock);

				Utils_tskAckOrFreeMsg(pMsg, status);
				Vps_printf("ALG_LINK_CEIL_TRACK_CMD_SAVE_VIEW 222222\n");
			}
			break;
			default:
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }

    if (ackMsg && pMsg != NULL)
        Utils_tskAckOrFreeMsg(pMsg, status);

    return;
}

Void AlgLink_blackboardtracktskMain(struct Utils_TskHndl *pTsk, Utils_MsgHndl * pMsg)
{
	 UInt32 cmd = Utils_msgGetCmd(pMsg);
	 Bool ackMsg, done;
	 Int32 status;
	 done = FALSE;
     ackMsg = FALSE;
	 AlgLink_Obj *pObj;
  	
     pObj = (AlgLink_Obj *) pTsk->appData;
 	 
	 Vps_printf("AlgLink_blackboardtracktskMain Done!\n");
	 while (!done)
   	 {
        status = Utils_tskRecvMsg(pTsk, &pMsg, BIOS_WAIT_FOREVER);
        if (status != FVID2_SOK)
            break;

        cmd = Utils_msgGetCmd(pMsg);

        switch (cmd)
        {
            case SYSTEM_CMD_NEW_DATA:
			{
			      Utils_tskAckOrFreeMsg(pMsg, status);
				  FVID2_Frame        *frames;
				  Int32 status;
				  pObj->blackboardtrackAlg.ProcessFlag = 1;
				  status = Utils_queGet(&pObj->blackboardtrackAlg.blackboardtrackoutFrameBufQue, (Ptr *) & frames, 1, BIOS_NO_WAIT);
	         	  if (status == FVID2_SOK)
	         	  {
					 	 System_LinkInQueParams *pInQueParams;
						 pInQueParams = &pObj->createArgs.inQueParams;
						 FVID2_FrameList frameList;
						 frameList.numFrames = 1;
						 frameList.frames[0] = frames;

						 AlgLink_BlackboardTrackalgProcessFrame(&pObj->blackboardtrackAlg, frames);
 
						 System_putLinksEmptyFrames(pInQueParams->prevLinkId,
	                                   pInQueParams->prevLinkQueId, &frameList);
						  pObj->blackboardtrackAlg.algstate.FreeNum = frameList.numFrames + pObj->blackboardtrackAlg.algstate.FreeNum;
	                  
	         	  }
				  pObj->blackboardtrackAlg.ProcessFlag = 0;
            }
            break;
			case ALG_LINK_BLACKBOARD_CMD_SET_CHANNEL_WIN_PRM:
			{
				AlgLink_BlackboardTrackCreateParams *params;
				AlgLink_BlackboardTrackCreateParams *pChPrm;
				AlgLink_BlackboardTrackChObj *pChObj;
		
				params = (AlgLink_BlackboardTrackCreateParams *) Utils_msgGetPrm(pMsg);
				Vps_printf("ALG_LINK_BLACKBOARD_CMD_SET_CHANNEL_WIN_PRM 11111 %d",params->chId);
				if(params->chId < ALG_LINK_BLACKBOARDTRACK_MAX_CH)
				{
					pChPrm = &pObj->blackboardtrackAlg.blackboardtrackChCreateParams[params->chId];
					pChObj = &pObj->blackboardtrackAlg.chObj[params->chId];
					pChPrm->chId = params->chId;
					memcpy(&pChPrm->BlackboardTrackParms, &params->BlackboardTrackParms, sizeof(BlackboardTrack_Params));
					pChObj->rtParamUpdatePerFrame = TRUE;
				}

				Utils_tskAckOrFreeMsg(pMsg, status);
				Vps_printf("ALG_LINK_BLACKBOARD_CMD_SET_CHANNEL_WIN_PRM 22222");
			}
			break;
			default:
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }

    if (ackMsg && pMsg != NULL)
        Utils_tskAckOrFreeMsg(pMsg, status);

    return;
}


Void AlgLink_trackdebugtskMain(struct Utils_TskHndl *pTsk, Utils_MsgHndl * pMsg)
{
	 AlgLink_Obj *pObj;
  	
     pObj = (AlgLink_Obj *) pTsk->appData;

	 while(1)
	 {
	 	Task_sleep(2000);
		continue;
		Vps_printf("Que Info \n"
				   "Track[%d] StuTrack[%d] CeilTrack[%d] StuSideTrack[%d] StuDoubleTrack[%d]\n",
		Utils_queGetQueuedCount(&pObj->trackAlg.trackoutFrameBufQue),
		Utils_queGetQueuedCount(&pObj->stutrackAlg.stutrackoutFrameBufQue),
		Utils_queGetQueuedCount(&pObj->ceiltrackAlg.ceiltrackoutFrameBufQue),
		Utils_queGetQueuedCount(&pObj->stusidetrackAlg.stusidetrackoutFrameBufQue),
		Utils_queGetQueuedCount(&pObj->studoublesidetrackAlg.studoublesidetrackoutFrameBufQue));
		AlgLink_printStatistics(pObj);
	 }
 	 
}

Void AlgLink_tskMain(struct Utils_TskHndl *pTsk, Utils_MsgHndl * pMsg)
{
    UInt32 cmd = Utils_msgGetCmd(pMsg);
    Bool ackMsg, done;
    Int32 status;
    AlgLink_Obj *pObj;
    UInt32 flushCmds[4];
    UInt8 objId = 0;

    pObj = (AlgLink_Obj *) pTsk->appData;

    if (cmd != SYSTEM_CMD_CREATE)
    {
        Utils_tskAckOrFreeMsg(pMsg, FVID2_EFAIL);
        return;
    }

    status = AlgLink_algCreate(pObj, Utils_msgGetPrm(pMsg));


//Vps_printf("pObj->createArgs.inQueParams.prevLinkId = %d %p\n",pObj->createArgs.inQueParams.prevLinkId,pObj);
    Utils_tskAckOrFreeMsg(pMsg, status);

    if (status != FVID2_SOK)
        return;

    if((pObj->createArgs.enableOSDAlg == 0) && (pObj->createArgs.enableSCDAlg == 1))
    {
       /* If algLink has only SCD algorithm, modify priority of link containing SCD to lower priority .
       * This ensures SCD operation happens in background and
       * is preempted by other algLinks whose algorithms have stringent realtime deadlines
       * link SWOSD
       */

       Vps_printf(" Warning: Changing Tsk priority of %s ALG Link \n",pObj->name);
       status = Utils_tskSetPri(pTsk, ALG_LINK_TSK_PRI_SCD);
       if (status != FVID2_SOK)
          Vps_printf(" Could not change Tsk priority of %s ALG Link \n",pObj->name);
    }

    done = FALSE;
    ackMsg = FALSE;

    objId = pObj->linkId - SYSTEM_LINK_ID_ALG_0;
    if(objId >= ALG_LINK_OBJ_MAX)
   {
   	objId = ALG_LINK_OBJ_MAX - 1;
   }
   gAlgLinkRunCount[objId] = 0;
    while (!done)
    {	
        status = Utils_tskRecvMsg(pTsk, &pMsg, BIOS_WAIT_FOREVER);

		
        if (status != FVID2_SOK)
        {
            break;
        }

        cmd = Utils_msgGetCmd(pMsg);
        switch (cmd)
        {
            case SYSTEM_CMD_NEW_DATA:
                  Utils_tskAckOrFreeMsg(pMsg, status);
  		          flushCmds[0] = SYSTEM_CMD_NEW_DATA;
                  Utils_tskFlushMsg(pTsk, flushCmds, 1);

                  AlgLink_algProcessData(pObj);

		    gAlgLinkRunCount[objId]++;
                  break;

            case ALG_LINK_OSD_CMD_SET_CHANNEL_WIN_PRM:
                 {
                   AlgLink_OsdChWinParams *params;

                   params = (AlgLink_OsdChWinParams *) Utils_msgGetPrm(pMsg);
                   AlgLink_OsdalgSetChOsdWinPrm(&pObj->osdAlg, params);
                   Utils_tskAckOrFreeMsg(pMsg, status);
                 }
                break;		
            case ALG_LINK_SCD_CMD_SET_CHANNEL_BLOCKCONFIG:
                 {
                   AlgLink_ScdChblkUpdate *params;

                   params = (AlgLink_ScdChblkUpdate *) Utils_msgGetPrm(pMsg);
                   AlgLink_ScdalgSetChblkUpdate(&pObj->scdAlg, params);
                   Utils_tskAckOrFreeMsg(pMsg, status);
                 }
                break;

            case ALG_LINK_SCD_CMD_PRINT_STATISTICS:
                AlgLink_ScdprintStatistics(&pObj->scdAlg, TRUE);
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;
				
		    case ALG_LINK_TRACK_CMD_PRINT_STATE:
				{
               		 //AlgLink_printStatistics(pObj);
              	 	 Utils_tskAckOrFreeMsg(pMsg, status);
		   		 }
                break;

            case ALG_LINK_SCD_CMD_SET_CHANNEL_IGNORELIGHTSON:
                 {
                   AlgLink_ScdChParams *params;

                   params = (AlgLink_ScdChParams *) Utils_msgGetPrm(pMsg);
                   AlgLink_ScdalgSetChScdIgnoreLightsOn(&pObj->scdAlg, params);
                   Utils_tskAckOrFreeMsg(pMsg, status);
                 }
                break;

            case ALG_LINK_SCD_CMD_SET_CHANNEL_IGNORELIGHTSOFF:
                 {
                   AlgLink_ScdChParams *params;

                   params = (AlgLink_ScdChParams *) Utils_msgGetPrm(pMsg);
                   AlgLink_ScdalgSetChScdIgnoreLightsOff(&pObj->scdAlg, params);
                   Utils_tskAckOrFreeMsg(pMsg, status);
                 }
                break;

            case ALG_LINK_SCD_CMD_SET_CHANNEL_MODE:
                 {
                   AlgLink_ScdChParams *params;

                   params = (AlgLink_ScdChParams *) Utils_msgGetPrm(pMsg);
                   AlgLink_ScdalgSetChScdMode(&pObj->scdAlg, params);
                   Utils_tskAckOrFreeMsg(pMsg, status);
                 }
                break;

            case ALG_LINK_SCD_CMD_SET_CHANNEL_SENSITIVITY:
                 {
                   AlgLink_ScdChParams *params;

                   params = (AlgLink_ScdChParams *) Utils_msgGetPrm(pMsg);
                   AlgLink_ScdalgSetChScdSensitivity(&pObj->scdAlg, params);
                   Utils_tskAckOrFreeMsg(pMsg, status);
                 }
                  break;
            case ALG_LINK_SCD_CMD_GET_ALL_CHANNEL_FRAME_STATUS:
                 {
                   AlgLink_ScdAllChFrameStatus *params;

                   params = (AlgLink_ScdAllChFrameStatus *) Utils_msgGetPrm(pMsg);
                   AlgLink_scdAlgGetAllChFrameStatus(&pObj->scdAlg, params);
                   Utils_tskAckOrFreeMsg(pMsg, status);
                 }
                break;
            case ALG_LINK_SCD_CMD_CHANNEL_RESET:
                 {
                   AlgLink_ScdChCtrl *params;

                   params = (AlgLink_ScdChCtrl *) Utils_msgGetPrm(pMsg);
                   AlgLink_ScdalgGetChResetChannel(&pObj->scdAlg, params);
                   Utils_tskAckOrFreeMsg(pMsg, status);
                 }
                  break;
            case SYSTEM_CMD_DELETE:
                flushCmds[0] = SYSTEM_CMD_NEW_DATA;
                Utils_tskFlushMsg(pTsk, flushCmds, 1);
                done = TRUE;
                ackMsg = TRUE;
                break;

            default:
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }

    AlgLink_algDelete(pObj);

    if (ackMsg && pMsg != NULL)
        Utils_tskAckOrFreeMsg(pMsg, status);

    return;
}

Int32 AlgLink_init()
{
    Int32 status;
    System_LinkObj linkObj;
    AlgLink_Obj *pObj;
    UInt32 objId;
	
//ALG_LINK_OBJ_MAX

//ps: 为2，则需改track流程
    for (objId = 0; objId < 1; objId++)
    {
        pObj = &gAlgLink_obj[objId];

        memset(pObj, 0, sizeof(*pObj));

        pObj->linkId = SYSTEM_LINK_ID_ALG_0 + objId;

        linkObj.pTsk = &pObj->tsk;
        linkObj.linkGetFullFrames   = NULL;
        linkObj.linkPutEmptyFrames  = NULL;
        linkObj.linkGetFullBitBufs  = AlgLink_getFullBufs;
        linkObj.linkPutEmptyBitBufs = AlgLink_putEmptyBufs;
        linkObj.getLinkInfo         = AlgLink_getInfo;

        sprintf(pObj->name, "ALG%d   ", objId);

        pObj->isCreated = ALG_LINK_STATE_INACTIVE;

        System_registerLink(pObj->linkId, &linkObj);

        status = Utils_tskCreate(&pObj->tsk,
                                 AlgLink_tskMain,
                                 ALG_LINK_TSK_PRI,
                                 gAlgLink_tskStack[objId],
                                 ALG_LINK_TSK_STACK_SIZE, pObj, pObj->name);
        UTILS_assert(status == FVID2_SOK);
    }

	Vps_printf("start create ING TRACK and STUTRACK %d\n",pObj->linkId);
#if 1
	pObj = &gAlgLink_obj[0];
	linkObj.pTsk = &pObj->tracktsk;
    linkObj.linkGetFullFrames   = NULL;
    linkObj.linkPutEmptyFrames  = NULL;
    linkObj.linkGetFullBitBufs  = NULL;
    linkObj.linkPutEmptyBitBufs = NULL;
    linkObj.getLinkInfo         = NULL;

	//sprintf(pObj->name, "TRACK ");
	System_registerLink(SYSTEM_LINK_ID_TRACK, &linkObj);
	status = Reach_tskCreate(&pObj->tracktsk,
                                 AlgLink_tracktskMain,
                                 8,
                                 gAlgTrackLink_tskStack,
                                 ALG_LINK_TSK_STACK_SIZE, pObj, "TRACK");
    UTILS_assert(status == FVID2_SOK);

	pObj = &gAlgLink_obj[0];
	linkObj.pTsk = &pObj->stutracktsk;
    linkObj.linkGetFullFrames   = NULL;
    linkObj.linkPutEmptyFrames  = NULL;
    linkObj.linkGetFullBitBufs  = NULL;
    linkObj.linkPutEmptyBitBufs = NULL;
    linkObj.getLinkInfo         = NULL;

	//sprintf(pObj->name, "TRACK ");
	System_registerLink(SYSTEM_LINK_ID_STUTRACK, &linkObj);
	status = Reach_tskCreate(&pObj->stutracktsk,
                                 AlgLink_stutracktskMain,
                                 8,
                                 gAlgstuTrackLink_tskStack,
                                 ALG_LINK_TSK_STACK_SIZE, pObj, "STUTRACK");
    UTILS_assert(status == FVID2_SOK);

	
	pObj = &gAlgLink_obj[0];
	linkObj.pTsk = &pObj->stusidetracktsk;
    linkObj.linkGetFullFrames   = NULL;
    linkObj.linkPutEmptyFrames  = NULL;
    linkObj.linkGetFullBitBufs  = NULL;
    linkObj.linkPutEmptyBitBufs = NULL;
    linkObj.getLinkInfo         = NULL;

	//sprintf(pObj->name, "TRACK ");
	System_registerLink(SYSTEM_LINK_ID_STUSIDETRACK, &linkObj);
	status = Reach_tskCreate(&pObj->stusidetracktsk,
                                 AlgLink_stusidetracktskMain,
                                 1,
                                 gAlgstusideTrackLink_tskStack,
                                 ALG_LINK_TSK_STACK_SIZE, pObj, "STUSIDETRACK");
    UTILS_assert(status == FVID2_SOK);


	pObj = &gAlgLink_obj[0];
	linkObj.pTsk = &pObj->studoublesidetracktsk;
    linkObj.linkGetFullFrames   = NULL;
    linkObj.linkPutEmptyFrames  = NULL;
    linkObj.linkGetFullBitBufs  = NULL;
    linkObj.linkPutEmptyBitBufs = NULL;
    linkObj.getLinkInfo         = NULL;
	System_registerLink(SYSTEM_LINK_ID_STUDOUBLESIDETRACK, &linkObj);
	status = Reach_tskCreate(&pObj->studoublesidetracktsk,
                                 AlgLink_studoublesidetracktskMain,
                                 1,
                                 gAlgstudoublesideTrackLink_tskStack,
                                 ALG_LINK_TSK_STACK_SIZE, pObj, "STUDOUBLESIDETRACK");
    UTILS_assert(status == FVID2_SOK);


	pObj = &gAlgLink_obj[0];
	linkObj.pTsk = &pObj->ceiltracktsk;
    linkObj.linkGetFullFrames   = NULL;
    linkObj.linkPutEmptyFrames  = NULL;
    linkObj.linkGetFullBitBufs  = NULL;
    linkObj.linkPutEmptyBitBufs = NULL;
    linkObj.getLinkInfo         = NULL;
	System_registerLink(SYSTEM_LINK_ID_CEILTRACK, &linkObj);
	status = Reach_tskCreate(&pObj->ceiltracktsk,
                                 AlgLink_ceiltracktskMain,
                                 1,
                                 gAlgCeilTrackLink_tskStack,
                                 ALG_LINK_TSK_STACK_SIZE, pObj, "CEILTRACK");
    UTILS_assert(status == FVID2_SOK);
	

	pObj = &gAlgLink_obj[0];
	linkObj.pTsk = &pObj->blackboardtracktsk;
    linkObj.linkGetFullFrames   = NULL;
    linkObj.linkPutEmptyFrames  = NULL;
    linkObj.linkGetFullBitBufs  = NULL;
    linkObj.linkPutEmptyBitBufs = NULL;
    linkObj.getLinkInfo         = NULL;
	System_registerLink(SYSTEM_LINK_ID_BLACKBOARDTRACK, &linkObj);
	status = Reach_tskCreate(&pObj->blackboardtracktsk,
                                 AlgLink_blackboardtracktskMain,
                                 1,
                                 gAlgBlackboardTrackLink_tskStack,
                                 ALG_LINK_TSK_STACK_SIZE, pObj, "BLACKBOARDTRACK");
    UTILS_assert(status == FVID2_SOK);


	status = Reach_tskCreate(&pObj->trackdebugtsk,
                                 AlgLink_trackdebugtskMain,
                                 1,
                                 gAlgTrackDebugLink_tskStack,
                                 ALG_LINK_TSK_STACK_SIZE, pObj, "TrackDebug");
    UTILS_assert(status == FVID2_SOK);
	
#endif

    return status;
}


Int32 AlgLink_deInit()
{
    UInt32 objId;
    AlgLink_Obj *pObj;

    for (objId = 0; objId < ALG_LINK_OBJ_MAX; objId++)
    {
        pObj = &gAlgLink_obj[objId];

        Utils_tskDelete(&pObj->tsk);
    }

    return FVID2_SOK;
}

Int32 AlgLink_getInfo(Utils_TskHndl * pTsk, System_LinkInfo * info)
{
    AlgLink_Obj *pObj = (AlgLink_Obj *) pTsk->appData;

    memcpy(info, &pObj->info, sizeof(*info));

    return FVID2_SOK;
}
Int32 AlgLink_getFullBufs(Utils_TskHndl * pTsk, UInt16 queId,
                           Bitstream_BufList * pBufList)
{
    AlgLink_Obj *pObj = (AlgLink_Obj *) pTsk->appData;

    UTILS_assert(queId < ALG_LINK_MAX_OUT_QUE);

    if(pObj->isCreated == ALG_LINK_STATE_ACTIVE)
    {
       /* If Alg link is active then pass buffer list to next link */
       return Utils_bitbufGetFull(&pObj->outObj[queId].bufOutQue, pBufList, BIOS_NO_WAIT);
    }
    else
    {
       /* If Alg link is in-active don't do anything */
       pBufList->numBufs = 0;
       return FVID2_SOK;
    }
}

Int32 AlgLink_putEmptyBufs(Utils_TskHndl * pTsk, UInt16 queId,
                           Bitstream_BufList * pBufList)
{
    AlgLink_Obj *pObj = (AlgLink_Obj *) pTsk->appData;

    UTILS_assert(queId < ALG_LINK_MAX_OUT_QUE);

    if(pObj->isCreated == ALG_LINK_STATE_ACTIVE)
       return Utils_bitbufPutEmpty(&pObj->outObj[queId].bufOutQue, pBufList);
    else
       return FVID2_SOK;
}
