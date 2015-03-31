#ifndef __REACH_SELECT_H__
#define __REACH_SELECT_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "reach_system.h"
#include "common_def/ti_vcap_common_def.h"


typedef struct _select_struct_ {
	Uint32					link_id;
	SelectLink_CreateParams	create_params;
}select_struct;


Void select_init_create_param(SelectLink_CreateParams *prm);

Int32 select_set_outque_chinfo(Uint32 selectId, SelectLink_OutQueChInfo *prm);

Int32 select_get_outque_chinfo(Uint32 selectId, SelectLink_OutQueChInfo *prm);

Int32 select_set_outque_chinfo2(Uint32 selectId, SelectLink_OutQueChInfo *prm);



#endif

