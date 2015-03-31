#ifndef __REACH_ENC_H__
#define __REACH_ENC_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "reach_system.h"
#include "common_def/ti_vcap_common_def.h"

typedef struct _enc_struct_ {
	Uint32					link_id;
	EncLink_CreateParams	create_params;
}enc_struct;


Void enc_init_create_param(EncLink_CreateParams *prm);

Int32 enc_set_bitrate(Uint32 encId, Uint32 chId, Uint32 bitrate);

Int32 enc_set_interval(Uint32 encId, Uint32 chId, Uint32 interval);

Int32 enc_set_fps(Uint32 encId, Uint32 chId, Uint32 fps, Uint32 bitate);

Int32 enc_force_iframe(Uint32 encId, Uint32 chId);

Int32 enc_get_dynparams(Uint32 encId, EncLink_GetDynParams *params);

Int32 enc_print_ivahd_statistics(Uint32 encId);

Int32 enc_print_statistics(Uint32 encId);

Int32 enc_print_buffer_statistics(Uint32 encId);

Int32 enc_enable_channel(Uint32 encId, Uint32 chId);

Int32 enc_disable_channel(Uint32 encId, Uint32 chId);

Int32 enc_set_rcalgo(Uint32 encId, Uint32 chId, Uint32 rcalg);

Int32 enc_set_iframe_qp(Uint32 encId, EncLink_ChQPParams *params);

Int32 enc_set_pframe_qp(Uint32 encId, EncLink_ChQPParams *params);

Int32 enc_set_codec_vbrd(Uint32 encId, Uint32 chId, Uint32 vbrduration);

Int32 enc_set_codec_vbrs(Uint32 encId, Uint32 chId, Uint32 vbrsensitivity);

Int32 enc_set_inputfps(Uint32 encId, Uint32 chId, Uint32 inputfps);

Int32 enc_set_snapshot(Uint32 encId, Uint32 chId);


#endif

