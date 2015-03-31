#ifndef __REACH_DUP_H__
#define __REACH_DUP_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "reach_system.h"
#include "common_def/ti_vcap_common_def.h"

typedef struct _dup_struct_ {
	Uint32					link_id;
	DupLink_CreateParams	create_params;
}dup_struct;



Void dup_init_create_param(DupLink_CreateParams *prm);



#endif

