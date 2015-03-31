#ifndef __REACH_HOST_H__
#define __REACH_HOST_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "reach_system.h"
#include "common_def/ti_vcap_common_def.h"
#include "common_def/ti_vdis_common_def.h"



#define REACH_INIT_STRUCT(structName,structObj)  structName##_Init(&structObj)




Int32 host_system_init();

Int32 host_system_deinit();




#endif

