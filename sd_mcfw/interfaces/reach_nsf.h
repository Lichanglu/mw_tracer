#ifndef __REACH_NSF_H__
#define __REACH_NSF_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "reach_system.h"
#include "common_def/ti_vcap_common_def.h"

typedef struct _nsf_struct_ {
	Uint32					link_id;
	NsfLink_CreateParams	create_params;
}nsf_struct;


Void nsf_init_create_param(NsfLink_CreateParams *prm);

Int32 nsf_set_framerate(Uint32 nsfId, Uint32 chId, Uint32 inputFrameRate, Uint32 outputFrameRate);

Int32 nsf_print_static(Uint32 nsfId);

Int32 nsf_print_bufferstatic(Uint32 nsfId);




#endif

