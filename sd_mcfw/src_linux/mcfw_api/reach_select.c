#include "reach_system_priv.h"
#include "reach_select_priv.h"



Void select_init_create_param(SelectLink_CreateParams *prm)
{
	return SelectLink_CreateParams_Init(prm);
}

Int32 select_set_outque_chinfo(Uint32 selectId, SelectLink_OutQueChInfo *prm)
{
	Int32 status = -1;

	if(selectId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(selectId,
						SELECT_LINK_CMD_SET_OUT_QUE_CH_INFO,
						prm,
						sizeof(*prm),
						TRUE
						);
	}

	return status;
}

Int32 select_set_outque_chinfo2(Uint32 selectId, SelectLink_OutQueChInfo *prm)
{
	Int32 status = -1;

	if(selectId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(selectId,
						SELECT_LINK_CMD_SET_OUT_QUE_CH_INFO2,
						prm,
						sizeof(*prm),
						TRUE
						);
	}

	return status;
}


Int32 select_get_outque_chinfo(Uint32 selectId, SelectLink_OutQueChInfo *prm)
{
	Int32 status = -1;

	if(selectId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(selectId,
						SELECT_LINK_CMD_GET_OUT_QUE_CH_INFO,
						prm,
						sizeof(*prm),
						TRUE
						);
	}

	return status;
}



