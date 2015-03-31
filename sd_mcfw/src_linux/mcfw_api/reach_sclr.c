#include "reach_system_priv.h"
#include "reach_sclr_priv.h"


Void sclr_init_create_param(SclrLink_CreateParams *prm)
{
	return SclrLink_CreateParams_Init(prm);
}

Int32 sclr_disable_channel(Uint32 sclrId, Uint32 chId)
{
	Int32 status = -1;
	 SclrLink_ChannelInfo chInfo;

        chInfo.channelId	= chId;
        chInfo.enable	= FALSE;

	if(sclrId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(sclrId,
						SCLR_LINK_CMD_DISABLE_CHANNEL,
						&chInfo,
						sizeof(chInfo),
						TRUE
						);
	}

	return status;
}

Int32 sclr_enable_channel(Uint32 sclrId, Uint32 chId)
{
	Int32 status = -1;
	SclrLink_ChannelInfo chInfo;

	chInfo.channelId = chId;
	chInfo.enable	 = TRUE;

	if(sclrId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(sclrId,
								SCLR_LINK_CMD_ENABLE_CHANNEL,
								&chInfo,
								sizeof(chInfo),
								TRUE
								);
	}

	return status;
}


Int32 sclr_get_output_resolution(Uint32 sclrId, SclrLink_chDynamicSetOutRes *params)
{
	Int32 status = -1;

	if(sclrId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(sclrId,
								DEI_LINK_CMD_GET_OUTPUTRESOLUTION,
								params,
								sizeof(*params),
								TRUE
								);

	}

	return status;
}

Int32 sclr_set_output_resolution(Uint32 sclrId, SclrLink_chDynamicSetOutRes *params)
{
	Int32 status = -1;

	if(sclrId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(sclrId,
								SCLR_LINK_CMD_SET_OUTPUTRESOLUTION,
								params,
								sizeof(*params),
								TRUE
								);

	}

	return status;
}

Int32 sclr_set_SclrMode(Uint32 sclrId, SclrLink_SclrMode *params)
{
	Int32 status = -1;

	if(sclrId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(sclrId,
								SCLR_LINK_CMD_SET_SCLR_MODE,
								params,
								sizeof(*params),
								TRUE
								);

	}

	return status;
}

Int32 sclr_set_inchinfo(Uint32 sclrId, System_LinkChInfo2 *params)
{
	Int32 status = -1;

	if(sclrId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(sclrId,
								SCLR_LINK_CMD_SET_INCHAN_INFO,
								params,
								sizeof(*params),
								TRUE
								);

	}

	return status;
}



Int32 sclr_skip_fidtype(Uint32 sclrId, UInt32 vcChnId, Int32 fidType)
{
	Int32 status = -1;
	
	SclrLink_chDynamicSkipFidType params;

	params.chId = vcChnId;
	params.fidType = fidType;
	if(sclrId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(sclrId,
								SCLR_LINK_CMD_SKIP_FID_TYPE,
								&(params),
								sizeof(params),
								TRUE
								);
	}

	return status;
}

Int32 sclr_set_framerate(Uint32 sclrId, Uint32 chId, Uint32 inputFrameRate, Uint32 outputFrameRate)
{
	Int32 status = -1;

	SclrLink_ChFpsParams params;

	params.chId = chId;
	params.inputFrameRate = inputFrameRate;
	params.outputFrameRate = outputFrameRate;

	if(sclrId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(sclrId, 
								SCLR_LINK_CMD_SET_FRAME_RATE,
								&params, 
								sizeof(params), 
								TRUE
								);
	}

	return -1;
}

Int32 sclr_print_statics(Uint32 sclrId)
{
	Int32 status = -1;

	if(sclrId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(sclrId,
								SCLR_LINK_CMD_PRINT_STATISTICS,
								NULL,
								0,
								TRUE
								);

	}

	return status;
}




