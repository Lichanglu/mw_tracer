#ifndef __REACH_NULLSRC_H__
#define __REACH_NULLSRC_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "reach_system.h"
#include "common_def/ti_vcap_common_def.h"

typedef struct _nullsrc_struct_ {
	Uint32						link_id;
	NullSrcLink_CreateParams	create_params;
}nullsrc_struct;




Void nullsrc_init_create_param(NullSrcLink_CreateParams *prm);


#endif


