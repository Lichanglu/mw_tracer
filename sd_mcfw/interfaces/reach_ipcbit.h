#ifndef __REACH_IPCBIT_H__
#define __REACH_IPCBIT_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "reach_system.h"
#include "common_def/ti_vcap_common_def.h"
#include "common_def/ti_vdis_common_def.h"

#include <osa_que.h>


typedef struct _bitsin_user_param_ {
	OSA_QueHndl bits_full_que;
	OSA_QueHndl bits_empty_que;
	Uint32		run_status;

	Void		*appdata;
}bits_user_param;

typedef Int32 (*bits_callback_fxn)(bits_user_param *hndle_param);

typedef struct _ipcbit_outhost_struct_ {
	Uint32							link_id;
	IpcBitsOutLinkHLOS_CreateParams	create_params;
}ipcbit_outhost_struct;

typedef struct _ipcbit_inhost_struct_ {
	Uint32							link_id;
	IpcBitsInLinkHLOS_CreateParams	create_params;
}ipcbit_inhost_struct;

typedef struct _ipcbit_outvideo_struct_ {
	Uint32							link_id;
	IpcBitsOutLinkRTOS_CreateParams	create_params;
}ipcbit_outvideo_struct;

typedef struct _ipcbit_invideo_struct_ {
	Uint32							link_id;
	IpcBitsInLinkRTOS_CreateParams	create_params;
}ipcbit_invideo_struct;




Int32 ipcbit_create_bitsprocess_inst(Void **ppipcbit_handle,
											  Uint32 ipcBitInLinkId,
											  Uint32 ipcBitOutLinkId,
											  bits_callback_fxn pbitsin_cbfxn,
											  Void *bitsin_param,
											  bits_callback_fxn pbitsout_cbfxn,
											  Void *bitsout_param);




Int32 ipcbit_delete_bitsprocess_inst(Void *handle);




#endif

