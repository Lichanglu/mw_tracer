#include "reach_system_priv.h"
#include "reach_dsp_priv.h"



Int32 dsp_calc_cpuload_start()
{
	Int32 status = -1;
	
	status = System_linkControl(SYSTEM_LINK_ID_DSP,
							   SYSTEM_COMMON_CMD_CPU_LOAD_CALC_START,
							   NULL,
							   0,
							   TRUE
							   );

	return status;
}


Int32 dsp_calc_cpuload_stop()
{
	Int32 status = -1;
	
	status = System_linkControl(SYSTEM_LINK_ID_DSP,
							   SYSTEM_COMMON_CMD_CPU_LOAD_CALC_STOP,
							   NULL,
							   0,
							   TRUE
							   );

	return status;
}



Int32 dsp_calc_cpuload_reset()
{
	Int32 status = -1;
	
	status = System_linkControl(SYSTEM_LINK_ID_DSP,
							   SYSTEM_COMMON_CMD_CPU_LOAD_CALC_RESET,
							   NULL,
							   0,
							   TRUE
							   );

	return status;
}



Int32 dsp_print_cpuload()
{
	Int32 status = -1;

	SystemCommon_PrintStatus printStatus;

	memset(&printStatus, 0, sizeof(printStatus));

	printStatus.printCpuLoad = TRUE;
	printStatus.printTskLoad = TRUE;
	
	status = System_linkControl(SYSTEM_LINK_ID_DSP,
							   SYSTEM_COMMON_CMD_PRINT_STATUS,
							   &printStatus,
							   sizeof(printStatus),
							   TRUE
							   );
		
	return status;
}


