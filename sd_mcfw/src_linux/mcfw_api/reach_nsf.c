#include "reach_system_priv.h"
#include "reach_nsf_priv.h"



Void nsf_init_create_param(NsfLink_CreateParams *prm)
{
	return NsfLink_CreateParams_Init(prm);
}


Int32 nsf_set_framerate(Uint32 nsfId, Uint32 chId, Uint32 inputFrameRate, Uint32 outputFrameRate)
{
	Int32 status = -1;

	NsfLink_ChFpsParams params;

	params.chId = chId;
	params.inputFrameRate = inputFrameRate;
	params.outputFrameRate = outputFrameRate;

	if(nsfId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(nsfId, 
								NSF_LINK_CMD_SET_FRAME_RATE,
								&params, 
								sizeof(params), 
								TRUE
								);
	}

	return -1;
}

Int32 nsf_print_static(Uint32 nsfId)
{
	Int32 status = -1;
	
	if(nsfId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(nsfId,
						            NSF_LINK_CMD_PRINT_STATISTICS,
						            NULL, 
						            0, 
						            TRUE);
	}

	return status;
}

Int32 nsf_print_bufferstatic(Uint32 nsfId)
{
	Int32 status = -1;
	
	if(nsfId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(nsfId,
						            NSF_LINK_CMD_PRINT_BUFFER_STATISTICS,
						            NULL, 
						            0, 
						            TRUE);
	}

	return status;
}


