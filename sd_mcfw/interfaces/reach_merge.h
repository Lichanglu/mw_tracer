#ifndef __REACH_MERGE_H__
#define __REACH_MERGE_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "reach_system.h"
#include "common_def/ti_vcap_common_def.h"
#include "common_def/ti_vdis_common_def.h"

typedef struct _merge_struct_ {
	Uint32					link_id;
	MergeLink_CreateParams	create_params;
}merge_struct;


Void merge_init_create_param(MergeLink_CreateParams *prm);



#endif

