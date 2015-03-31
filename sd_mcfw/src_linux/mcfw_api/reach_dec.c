#include "reach_system_priv.h"
#include "reach_dec_priv.h"


Void dec_init_create_param(DecLink_CreateParams *prm)
{
	return DecLink_CreateParams_Init(prm);
}


Int32 dec_enable_channel(Uint32 decId, Uint32 chId)
{
	Int32 status = -1;

	DecLink_ChannelInfo params = { 0 };
	params.chId = chId;
	
	if(decId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(decId, 
								DEC_LINK_CMD_ENABLE_CHANNEL,
                       					&params,
                       					sizeof(params),
                       					TRUE);
	}

	return status;
}

Int32 dec_disable_channel(Uint32 decId, Uint32 chId)
{
	Int32 status = -1;

	DecLink_ChannelInfo params = { 0 };
	params.chId = chId;
	
	if(decId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(decId, 
								DEC_LINK_CMD_DISABLE_CHANNEL,
                       					&params,
                       					sizeof(params),
                       					TRUE);
	}

	return status;
}


Int32 dec_set_trickplayconfig(Uint32 decId, Uint32 chId, Uint32 inputfps, Uint32 targetfps)
{
	Int32 status = -1;

	DecLink_TPlayConfig params = { 0 };
	params.chId = chId;
	params.inputFps = inputfps;
	params.targetFps = targetfps;
	
	if(decId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(decId, 
								DEC_LINK_CMD_SET_TRICKPLAYCONFIG,
                       					&params,
                       					sizeof(params),
                       					TRUE);
	}

	return status;
}

Int32 dec_print_ivahd_statistics(Uint32 decId)
{
	Int32 status = -1;
	
	if(decId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(decId, 
								DEC_LINK_CMD_PRINT_IVAHD_STATISTICS,
                       					NULL,
                       					0,
                       					TRUE);
	}

	return status;
}

Int32 dec_print_statistics(Uint32 decId)
{
	Int32 status = -1;
	
	if(decId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(decId, 
								DEC_LINK_CMD_PRINT_STATISTICS,
                       					NULL,
                       					0,
                       					TRUE);
	}

	return status;
}

Int32 dec_print_buffer_statistics(Uint32 decId)
{
	Int32 status = -1;
	
	if(decId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(decId, 
								DEC_LINK_CMD_PRINT_BUFFER_STATISTICS,
                       					NULL,
                       					0,
                       					TRUE);
	}

	return status;
}




