#ifndef __REACH_DEI_H__
#define __REACH_DEI_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "reach_system.h"
#include "common_def/ti_vcap_common_def.h"


typedef struct _dei_struct_ {
	Uint32					link_id;
	DeiLink_CreateParams	create_params;
}dei_struct;


Void dei_init_create_param(DeiLink_CreateParams *prm);

Int32 dei_disable_channel(Uint32 deiId, Uint32 chId, Uint32 streamId);

Int32 dei_enable_channel(Uint32 deiId, Uint32 chId, Uint32 streamId);

Int32 dei_get_output_resolution(Uint32 deiId, DeiLink_chDynamicSetOutRes *params);

Int32 dei_set_output_resolution(Uint32 deiId, DeiLink_chDynamicSetOutRes *params);

Int32 dei_change_fid_flag(Uint32 deiId, Uint32 *chid);

Int32 dei_print_statics(Uint32 deiId);

Int32 dei_print_buffer_statics(Uint32 deiId);



#endif
