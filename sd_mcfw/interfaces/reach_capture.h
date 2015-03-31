#ifndef __REACH_CAPTURE_H__
#define __REACH_CAPTURE_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "reach_system.h"
#include "link_api/algLink.h"
#include "common_def/ti_vcap_common_def.h"
#include <osa_que.h>
#include <osa_mutex.h>
#include <osa_thr.h>
#include <osa_sem.h>


#define CAP_FILL_YUV_PATTERN(y,u,v)             ((((y) & 0xFF) << 0)  | \
                                                          	(((u) & 0xFF) << 8)  | \
                                                            (((y) & 0xFF) << 16) | \
                                                            (((v) & 0xFF) << 24))


typedef struct _cap_struct_ {
	Uint32						link_id;
	CaptureLink_CreateParams	create_params;
	Void						*adv7441_phandle;
	Void						*adv7442_phandle;
	Void						*gs2971_phandle;
	Void						*gs2972_phandle;
	Void						*tvp5158_phandle;
	OSA_ThrHndl   				taskHand;
	Bool                        isDetect;
}cap_struct;

typedef struct _video_status_ {
	UInt32	isVideoDetect;
	UInt32	frameWidth;
	UInt32	frameHeight;
	UInt32	isInterlaced;
}video_status;



void cap_init_create_param(CaptureLink_CreateParams *prm);

Int32 cap_get_video_status(Uint32 captureId, VCAP_VIDEO_SOURCE_STATUS_S *pStatus);

Int32 cap_detect_video(Uint32 captureId);

Int32 cap_config_videodecoder(Uint32 captureId);

Int32 cap_set_extra_frameschId(Uint32 captureId, UInt32 chId);

Int32 cap_skip_alternate_frames(Uint32 captureId, UInt32 queId, UInt32 chId, UInt32 skipMask);

// Int32 cap_set_minni_framerate(Uint32 captureId, UInt32 queId, UInt32 chId, UInt32 framerate);

Int32 cap_print_advstatistics(Uint32 captureId);

Int32 cap_print_bufferstatistics(Uint32 captureId);

Int32 cap_config_adv7441(Void **phandle, UInt8 instId, VCAP_VIDEO_SOURCE_CH_STATUS_S *pVideoStatus);

Int32 cap_config_adv7442(Void **phandle, UInt8 instId, VCAP_VIDEO_SOURCE_CH_STATUS_S *pVideoStatus);

Int32 cap_config_adv7619(Void **phandle, UInt8 instId, VCAP_VIDEO_SOURCE_CH_STATUS_S *pVideoStatus);

Int32 cap_config_gs2971(Void **phandle, UInt8 instId, VCAP_VIDEO_SOURCE_CH_STATUS_S *pVideoStatus);

Int32 cap_config_gs2972(Void **phandle, UInt8 instId, VCAP_VIDEO_SOURCE_CH_STATUS_S *pVideoStatus);

Int32 cap_config_tvp5158(Void **phandle, UInt8 instId, VCAP_VIDEO_SOURCE_CH_STATUS_S *pVideoStatus);


Int32 cap_get_7441_resolution(Void **phandle, VCAP_VIDEO_SOURCE_CH_STATUS_S *pvstatus);

Int32 cap_get_7442_resolution(Void **phandle, VCAP_VIDEO_SOURCE_CH_STATUS_S *pvstatus);

Int32 cap_get_2971_resolution(Void **phandle, VCAP_VIDEO_SOURCE_CH_STATUS_S *pvstatus);

Int32 cap_get_2972_resolution(Void **phandle, VCAP_VIDEO_SOURCE_CH_STATUS_S *pvstatus);

Int32 cap_get_5158_resolution(Void **phandle, VCAP_VIDEO_SOURCE_CH_STATUS_S *pvstatus);



Int32 cap_get_adv7441_HV(Void **phandle, int* hv);
Int32 cap_get_adv7442_HV(Void **phandle, int* hv);
Int32 cap_set_adv7441_HV(Void **phandle, int hv);
Int32 cap_set_adv7442_HV(Void **phandle, int hv);

Int32 Cap_SetSource7441Chan(Void **phandle, int chan);
Int32 Cap_SetSource7442Chan(Void **phandle, int chan);
Int32 cap_invert_cbcr_adv7441_HV(Void **phandle, int hv);
Int32 cap_invert_cbcr_adv7442_HV(Void **phandle, int hv);
Int32 cap_check_status(Uint32 captureId);
Int32 cap_set_blind_area(Uint32 captureId, CaptureLink_BlindInfo *params);



#endif
