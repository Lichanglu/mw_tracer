#ifndef __REACH_SCLR_H__
#define __REACH_SCLR_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "reach_system.h"
#include "common_def/ti_vcap_common_def.h"

typedef struct _sclr_struct_ {
	Uint32					link_id;
	SclrLink_CreateParams	create_params;
}sclr_struct;


Void sclr_init_create_param(SclrLink_CreateParams *prm);

Int32 sclr_disable_channel(Uint32 sclrId, Uint32 chId);

Int32 sclr_enable_channel(Uint32 sclrId, Uint32 chId);

Int32 sclr_get_output_resolution(Uint32 sclrId, SclrLink_chDynamicSetOutRes *params);

Int32 sclr_set_output_resolution(Uint32 sclrId, SclrLink_chDynamicSetOutRes *params);

Int32 sclr_skip_fidtype(Uint32 sclrId, Uint32 vcChnId, Int32 fidType);

Int32 sclr_set_framerate(Uint32 sclrId, Uint32 chId, Uint32 inputFrameRate, Uint32 outputFrameRate);

Int32 sclr_print_statics(Uint32 sclrId);

Int32 sclr_set_SclrMode(Uint32 sclrId, SclrLink_SclrMode *params);

Int32 sclr_set_inchinfo(Uint32 sclrId, System_LinkChInfo2 *params);



#endif

