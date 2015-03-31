#include "reach_system_priv.h"
#include "reach_video_priv.h"


Int32 video_set_ch2ivahd_map_tbl(SystemVideo_Ivahd2ChMap_Tbl  *pmaptbl)
{
	Int32 status = -1;
	status = System_linkControl(SYSTEM_LINK_ID_M3VIDEO,
					SYSTEM_COMMON_CMD_SET_CH2IVAHD_MAP_TBL,
					pmaptbl,
					sizeof(SystemVideo_Ivahd2ChMap_Tbl),
					TRUE
	);

	return status;
}

Int32 video_calc_cpuload_start()
{
	Int32 status = -1;
	
	status = System_linkControl(SYSTEM_LINK_ID_M3VIDEO,
							   SYSTEM_COMMON_CMD_CPU_LOAD_CALC_START,
							   NULL,
							   0,
							   TRUE
							   );

	return status;
}


Int32 video_calc_cpuload_stop()
{
	Int32 status = -1;
	
	status = System_linkControl(SYSTEM_LINK_ID_M3VIDEO,
							   SYSTEM_COMMON_CMD_CPU_LOAD_CALC_STOP,
							   NULL,
							   0,
							   TRUE
							   );

	return status;
}



Int32 video_calc_cpuload_reset()
{
	Int32 status = -1;
	
	status = System_linkControl(SYSTEM_LINK_ID_M3VIDEO,
							   SYSTEM_COMMON_CMD_CPU_LOAD_CALC_RESET,
							   NULL,
							   0,
							   TRUE
							   );

	return status;
}



Int32 video_print_cpuload()
{
	Int32 status = -1;

	SystemCommon_PrintStatus printStatus;

	memset(&printStatus, 0, sizeof(printStatus));

	printStatus.printCpuLoad = TRUE;
	printStatus.printTskLoad = TRUE;
	
	status = System_linkControl(SYSTEM_LINK_ID_M3VIDEO,
							   SYSTEM_COMMON_CMD_PRINT_STATUS,
							   &printStatus,
							   sizeof(printStatus),
							   TRUE
							   );
		
	return status;
}


