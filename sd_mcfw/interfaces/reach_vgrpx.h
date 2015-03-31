#ifndef __REACH_VGRPX_H__
#define __REACH_VGRPX_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "reach_system.h"
#include "common_def/ti_vcap_common_def.h"




Void vgrpx_init_create_param(VGRPX_CREATE_PARAM_S *prm);

Int32 vgrpx_get_info(UInt32 grpxId, VGRPX_BUFFER_INFO_S *pPrm);

Int32 vgrpx_set_dynamicparam(UInt32 grpxId, VGRPX_DYNAMIC_PARAM_S *pPrm);






#endif

