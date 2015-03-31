#ifndef __REACH_SWMS_H__
#define __REACH_SWMS_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "reach_system.h"
#include "common_def/ti_vcap_common_def.h"
#include "common_def/ti_vdis_common_def.h"

typedef struct _swms_struct_ {
	Uint32					link_id;
	SwMsLink_CreateParams	create_params;
}swms_struct;



Void swms_init_create_param(SwMsLink_CreateParams *prm);

Int32 swms_set_layout(Uint32 swmsId, SwMsLink_LayoutPrm *playout);

Int32 swms_get_layout(Uint32 swmsId, SwMsLink_LayoutPrm *playout);

Int32 swms_get_input_channel_info(Uint32 swmsId, SwMsLink_WinInfo pinfo);

Int32 swms_print_statics(Uint32 sclrId);

Int32 swms_print_bufferstatics(Uint32 swMsId);

void swms_set_default_mosaic_param(VDIS_MOSAIC_S *vdMosaicParam);

Void swms_set_swms_layout(UInt32 devId, SwMsLink_CreateParams *swMsCreateArgs);

Int32 getoutsize(UInt32 outRes, UInt32 * width, UInt32 * height);

Int32 swMsSwitchLayout(UInt32 swMsLinkId, SwMsLink_CreateParams *swMsPrm, UInt32 curLayoutId,UInt32 Mode);

UInt32 GetSwmsTypeWH(UInt32 OutType,UInt32 SwmsType, UInt32 ch, UInt32 *width, UInt32 *hight,UInt32 *x, UInt32 *y);

UInt32 CalculationSclrMode(SclrLink_CalSclrMode CalSclrMode, UInt32 *Width, UInt32 *Height);

#endif

