#include "reach_system_priv.h"
#include "reach_dei_priv.h"



Void dei_init_create_param(DeiLink_CreateParams *prm)
{
	return DeiLink_CreateParams_Init(prm);
}


Int32 dei_disable_channel(Uint32 deiId, Uint32 chId, Uint32 streamId)
{
	Int32 status = -1;
	 DeiLink_ChannelInfo deiChDisable;

        deiChDisable.channelId = chId;
        deiChDisable.streamId	 = streamId;
        deiChDisable.enable	 = FALSE;

	if(deiId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(deiId,
						DEI_LINK_CMD_DISABLE_CHANNEL,
						&deiChDisable,
						sizeof(deiChDisable),
						TRUE
						);
	}

	return status;
}

Int32 dei_enable_channel(Uint32 deiId, Uint32 chId, Uint32 streamId)
{
	Int32 status = -1;
	 DeiLink_ChannelInfo deiChDisable;

        deiChDisable.channelId = chId;
        deiChDisable.streamId	 = streamId;
        deiChDisable.enable	 = TRUE;

	if(deiId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(deiId,
						DEI_LINK_CMD_ENABLE_CHANNEL,
						&deiChDisable,
						sizeof(deiChDisable),
						TRUE
						);
	}

	return status;
}


Int32 dei_get_output_resolution(Uint32 deiId, DeiLink_chDynamicSetOutRes *params)
{
	Int32 status = -1;

	if(deiId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(deiId,
								DEI_LINK_CMD_GET_OUTPUTRESOLUTION,
								params,
								sizeof(*params),
								TRUE
								);

	}

	return status;
}

Int32 dei_set_output_resolution(Uint32 deiId, DeiLink_chDynamicSetOutRes *params)
{
	Int32 status = -1;

	if(deiId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(deiId,
								DEI_LINK_CMD_SET_OUTPUTRESOLUTION,
								params,
								sizeof(*params),
								TRUE
								);

	}

	return status;
}

Int32 dei_change_fid_flag(Uint32 deiId, Uint32 *chid)
{
	Int32 status = -1;

	if(deiId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(deiId,
								DEI_LINK_CMD_CHANGE_CH_FID_FLAG,
								chid,
								sizeof(&chid),
								TRUE
								);

	}

	return status;
}


Int32 dei_print_statics(Uint32 deiId)
{
	Int32 status = -1;

	if(deiId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(deiId,
								DEI_LINK_CMD_PRINT_STATISTICS,
								NULL,
								0,
								TRUE
								);

	}

	return status;
}

Int32 dei_print_buffer_statics(Uint32 deiId)
{
	Int32 status = -1;

	if(deiId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(deiId,
								DEI_LINK_CMD_PRINT_BUFFER_STATISTICS,
								NULL,
								0,
								TRUE
								);

	}

	return status;
}





