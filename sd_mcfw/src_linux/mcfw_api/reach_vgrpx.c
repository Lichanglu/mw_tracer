#include "reach_system_priv.h"
#include "reach_vgrpx_priv.h"


Void vgrpx_init_create_param(VGRPX_CREATE_PARAM_S *prm)
{
	return GrpxLink_CreateParams_Init(prm);
}

Int32 vgrpx_get_info(UInt32 grpxId, VGRPX_BUFFER_INFO_S *pPrm)
{
    Int32 status = -1;

    if(grpxId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(
						grpxId,
						GRPX_LINK_CMD_GET_BUFFER_INFO,
						pPrm,
						sizeof(*pPrm),
						TRUE
						);
    }

    return status;
}

Int32 vgrpx_set_dynamicparam(UInt32 grpxId, VGRPX_DYNAMIC_PARAM_S *pPrm)
{
    Int32 status = -1;

    if(grpxId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(
						grpxId,
						GRPX_LINK_CMD_SET_DYNAMIC_PARAMS,
						pPrm,
						sizeof(*pPrm),
						TRUE
						);
    }

    return status;
}




