#ifndef __REACH_VIDEO_H__
#define __REACH_VIDEO_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "reach_system.h"
#include "common_def/ti_vcap_common_def.h"

Int32 video_set_ch2ivahd_map_tbl(SystemVideo_Ivahd2ChMap_Tbl  *pmaptbl);

Int32 video_calc_cpuload();

Int32 video_print_cpuload();

Int32 video_calc_cpuload_start();

Int32 video_calc_cpuload_stop();

Int32 video_calc_cpuload_reset();

Int32 video_print_cpuload();



#endif

