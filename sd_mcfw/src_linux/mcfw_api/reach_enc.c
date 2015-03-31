#include "reach_system_priv.h"
#include "reach_enc_priv.h"



Void enc_init_create_param(EncLink_CreateParams *prm)
{
	return EncLink_CreateParams_Init(prm);
}


Int32 enc_set_bitrate(Uint32 encId, Uint32 chId, Uint32 bitrate)
{
	Int32 status = -1;
	EncLink_ChBitRateParams params = { 0 };

	params.chId = chId;
	params.targetBitRate = bitrate;

	if(encId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(encId,
								ENC_LINK_CMD_SET_CODEC_BITRATE,
	                                           	&params,
	                                           	sizeof(params),
	                                           	TRUE
	                                           	);
	}

	return status;
}

//set encode gop
Int32 enc_set_interval(Uint32 encId, Uint32 chId, Uint32 interval)
{
	Int32 status = -1;
	EncLink_ChIntraFrIntParams params = { 0 };

	params.chId = chId;
	params.intraFrameInterval = interval;
	//params.intraFrameInterval = 3;
	printf("params.intraFrameInterval=%d\n",params.intraFrameInterval);

	if(encId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(encId,
								ENC_LINK_CMD_SET_CODEC_INTRAI,
	                                           	&params,
	                                           	sizeof(params),
	                                           	TRUE
	                                           	);
	}

	return status;
}

Int32 enc_set_fps(Uint32 encId, Uint32 chId, Uint32 fps, Uint32 bitate)
{
	Int32 status = -1;
	EncLink_ChFpsParams params = { 0 };

	params.chId = chId;
	params.targetFps = fps*1000;
	params.targetBitRate = bitate;

	if(encId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(encId,
								ENC_LINK_CMD_SET_CODEC_FPS,
	                                           	&params,
	                                           	sizeof(params),
	                                           	TRUE
	                                           	);
	}

	return status;
}

Int32 enc_force_iframe(Uint32 encId, Uint32 chId)
{
	Int32 status = -1;
	EncLink_ChForceIFrParams params = { 0 };

	params.chId = chId;

	if(encId != SYSTEM_LINK_ID_INVALID ) {
		status = System_linkControl(encId,
								ENC_LINK_CMD_SET_CODEC_FORCEI,
	                                           	&params,
	                                           	sizeof(params),
	                                           	TRUE
	                                           	);
	}

	return status;
}

Int32 enc_get_dynparams(Uint32 encId, EncLink_GetDynParams *params)
{
	Int32 status = -1;

	if(encId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(encId,
								ENC_LINK_CMD_GET_CODEC_PARAMS,
	                                           	params,
	                                           	sizeof(*params),
	                                           	TRUE
	                                           	);
	}

	return status;
}

Int32 enc_print_ivahd_statistics(Uint32 encId)
{
	Int32 status = -1;

	if(encId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(encId,
								ENC_LINK_CMD_PRINT_IVAHD_STATISTICS,
	                                           	NULL,
	                                           	0,
	                                           	TRUE
	                                           	);
	}

	return status;
}

Int32 enc_print_statistics(Uint32 encId)
{
	Int32 status = -1;

	if(encId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(encId,
								ENC_LINK_CMD_PRINT_STATISTICS,
	                                           	NULL,
	                                           	0,
	                                           	TRUE
	                                           	);
	}

	return status;
}

Int32 enc_print_buffer_statistics(Uint32 encId)
{
	Int32 status = -1;

	if(encId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(encId,
								ENC_LINK_CMD_PRINT_BUFFER_STATISTICS,
	                                           	NULL,
	                                           	0,
	                                           	TRUE
	                                           	);
	}

	return status;
}



Int32 enc_enable_channel(Uint32 encId, Uint32 chId)
{
	Int32 status = -1;

	EncLink_ChannelInfo params = { 0 };
	params.chId = chId;

	if(encId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(encId,
								ENC_LINK_CMD_ENABLE_CHANNEL,
                       					&params,
                       					sizeof(params),
                       					TRUE);
	}

	return status;
}

Int32 enc_disable_channel(Uint32 encId, Uint32 chId)
{
	Int32 status = -1;

	EncLink_ChannelInfo params = { 0 };
	params.chId = chId;

	if(encId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(encId,
								ENC_LINK_CMD_DISABLE_CHANNEL,
                       					&params,
                       					sizeof(params),
                       					TRUE);
	}

	return status;
}

Int32 enc_set_rcalgo(Uint32 encId, Uint32 chId, Uint32 rcalg)
{
	Int32 status = -1;
	EncLink_ChRcAlgParams params = { 0 };

	params.chId = chId;
	params.rcAlg = rcalg;

	if(encId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(encId,
								ENC_LINK_CMD_SET_CODEC_RCALGO,
	                                           	&params,
	                                           	sizeof(params),
	                                           	TRUE
	                                           	);
	}

	return status;
}

Int32 enc_set_iframe_qp(Uint32 encId, EncLink_ChQPParams *params)
{
	Int32 status = -1;

	if(encId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(encId,
								ENC_LINK_CMD_SET_CODEC_QP_I,
	                                           	params,
	                                           	sizeof(*params),
	                                           	TRUE
	                                           	);
	}

	return status;
}

Int32 enc_set_pframe_qp(Uint32 encId, EncLink_ChQPParams *params)
{
	Int32 status = -1;

	if(encId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(encId,
								ENC_LINK_CMD_SET_CODEC_QP_P,
	                                           	params,
	                                           	sizeof(*params),
	                                           	TRUE
	                                           	);
	}

	return status;
}

Int32 enc_set_codec_vbrd(Uint32 encId, Uint32 chId, Uint32 vbrduration)
{
	Int32 status = -1;

	EncLink_ChCVBRDurationParams params = {0};

	params.chId = chId;
	params.vbrDuration = vbrduration;

	if(encId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(encId,
								ENC_LINK_CMD_SET_CODEC_VBRD,
	                                           	&params,
	                                           	sizeof(params),
	                                           	TRUE
	                                           	);
	}

	return status;
}

Int32 enc_set_codec_vbrs(Uint32 encId, Uint32 chId, Uint32 vbrsensitivity)
{
	Int32 status = -1;

	EncLink_ChCVBRSensitivityParams params = {0};

	params.chId = chId;
	params.vbrSensitivity = vbrsensitivity;

	if(encId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(encId,
								ENC_LINK_CMD_SET_CODEC_VBRS,
	                                           	&params,
	                                           	sizeof(params),
	                                           	TRUE
	                                           	);
	}

	return status;
}

Int32 enc_set_inputfps(Uint32 encId, Uint32 chId, Uint32 inputfps)
{
	Int32 status = -1;
	EncLink_ChInputFpsParam params = { 0 };

	params.chId = chId;
	params.inputFps = inputfps;

	if(encId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(encId,
								ENC_LINK_CMD_SET_CODEC_INPUT_FPS,
	                                           	&params,
	                                           	sizeof(params),
	                                           	TRUE
	                                           	);
	}

	return status;
}

Int32 enc_set_snapshot(Uint32 encId, Uint32 chId)
{
	Int32 status = -1;
	EncLink_ChannelInfo params = { 0 };

	params.chId = chId;

	if(encId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(encId,
								ENC_LINK_CMD_SET_CODEC_SNAPSHOT,
	                                           	&params,
	                                           	sizeof(params),
	                                           	TRUE
	                                           	);
	}

	return status;
}






