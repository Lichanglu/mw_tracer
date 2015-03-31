#ifndef __REACH_SYSTEM_PRIV_H__
#define __REACH_SYSTEM_PRIV_H__

#include <mcfw/interfaces/link_api/system_const.h>

#include "reach_system.h"

#include <osa.h>
#include <mcfw/interfaces/link_api/system.h>
#include <mcfw/interfaces/link_api/captureLink.h>
#include <mcfw/interfaces/link_api/deiLink.h>
#include <mcfw/interfaces/link_api/nsfLink.h>
#include <mcfw/interfaces/link_api/displayLink.h>
#include <mcfw/interfaces/link_api/nullLink.h>
#include <mcfw/interfaces/link_api/grpxLink.h>
#include <mcfw/interfaces/link_api/dupLink.h>
#include <mcfw/interfaces/link_api/swMsLink.h>
#include <mcfw/interfaces/link_api/mergeLink.h>
#include <mcfw/interfaces/link_api/nullSrcLink.h>
#include <mcfw/interfaces/link_api/ipcLink.h>
#include <mcfw/interfaces/link_api/systemLink_m3vpss.h>
#include <mcfw/interfaces/link_api/systemLink_m3video.h>
#include <mcfw/interfaces/link_api/encLink.h>
#include <mcfw/interfaces/link_api/selectLink.h>
#include <mcfw/interfaces/link_api/decLink.h>
#include <mcfw/interfaces/link_api/nullSrcLink.h>
#include <mcfw/interfaces/link_api/grpxLink.h>



#include <mcfw/interfaces/link_api/sclrLink.h>
#include <mcfw/interfaces/link_api/avsync.h>

#include <ti/syslink/utils/IHeap.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/ipc/SharedRegion.h>
#include <string.h>

#include <ti/xdais/xdas.h>
#include <ti/xdais/dm/xdm.h>
#include <ti/xdais/dm/ivideo.h>
#include <ih264enc.h>
#include <ih264vdec.h>

#include <mcfw/interfaces/reach_capture.h>
#include <mcfw/interfaces/reach_display.h>
#include <mcfw/interfaces/reach_enc.h>
#include <mcfw/interfaces/reach_dec.h>
#include <mcfw/interfaces/reach_tiler.h>
#include <mcfw/interfaces/reach_dup.h>
#include <mcfw/interfaces/reach_swms.h>
#include <mcfw/interfaces/reach_nsf.h>
#include <mcfw/interfaces/reach_vpss.h>
#include <mcfw/interfaces/reach_video.h>
#include <mcfw/interfaces/reach_sclr.h>
#include <mcfw/interfaces/reach_dei.h>
#include <mcfw/interfaces/reach_host.h>
#include <mcfw/interfaces/reach_dsp.h>
#include <mcfw/interfaces/reach_ipcbit.h>
#include <mcfw/interfaces/reach_audio.h>
#include <mcfw/interfaces/reach_merge.h>
#include <mcfw/interfaces/reach_ipcoutm3.h>
#include <mcfw/interfaces/reach_ipcinm3.h>
#include <mcfw/interfaces/reach_audio.h>
#include <mcfw/interfaces/reach_select.h>
#include <mcfw/interfaces/reach_nullsrc.h>
#include <mcfw/interfaces/reach_vgrpx.h>
//#include <mcfw/interfaces/reach_null.h>
























#endif
