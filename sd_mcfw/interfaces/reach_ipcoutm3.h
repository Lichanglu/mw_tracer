#ifndef __REACH_IPCOUTM3_H__
#define __REACH_IPCOUTM3_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "reach_system.h"
#include "common_def/ti_vcap_common_def.h"


typedef struct _ipcoutm3_struct_ {
	Uint32					link_id;
	IpcLink_CreateParams	create_params;
}ipcoutm3_struct;



Void ipcoutm3_init_create_param(IpcLink_CreateParams *pPrm);



#endif

