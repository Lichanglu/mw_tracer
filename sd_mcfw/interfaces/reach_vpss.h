#ifndef __REACH_VPSS_H__
#define __REACH_VPSS_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "reach_system.h"
#include "common_def/ti_vcap_common_def.h"
#include "common_def/ti_vdis_common_def.h"






Int32 vpss_reset_video_devices ();

Int32 vpss_detect_board();

Int32 vpss_displayctrl_init(VDIS_PARAMS_S *prm);

Int32 vpss_displayctrl_deinit(VDIS_PARAMS_S *prm);

Int32 vpss_calc_cpuload_start();

Int32 vpss_print_cpuload();

Int32 vpss_calc_cpuload_reset();











#endif

