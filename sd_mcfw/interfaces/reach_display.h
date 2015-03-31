#ifndef __REACH_DISPLAY_H__
#define __REACH_DISPLAY_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "reach_system.h"
#include "link_api/algLink.h"
#include "common_def/ti_vcap_common_def.h"
#include "common_def/ti_vdis_common_def.h"

typedef struct _dis_struct_ {
	Uint32						link_id;
	DisplayLink_CreateParams	create_params;
	Void						*handle;
}dis_struct;



Void dis_init_create_param(DisplayLink_CreateParams *prm);

Int32 dis_switch_input_mode(Uint32 displayId, Bool inputMode);

Int32 dis_print_statistics(Uint32 displayId);

Int32 dis_print_bufferstatistics(Uint32 displayId);

Int32 dis_switch_activechannel(Uint32 displayId, UInt32 channelId);

Int32 dis_start_drv(Uint32 displayId);

Int32 dis_stop_drv(Uint32 displayId);

Int32 dis_set_resolution(UInt32 displayId, UInt32 resolution);

Void dis_params_init(VDIS_PARAMS_S * pContext);

Int32 dis_config_adv9889(Void **phandle);

Int32 Ths8200Init(Int32 outMode);




#endif
