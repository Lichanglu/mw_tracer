#include "reach_system_priv.h"
#include "reach_vpss_priv.h"


static char *gMultiCh_cpuName[SYSTEM_PLATFORM_CPU_REV_MAX] = {
    "ES1.0",
    "ES1.1",
    "ES2.0",
    "ES2.1",
    "UNKNOWN",
};

static char *gMultiCh_boardName[SYSTEM_PLATFORM_BOARD_MAX] = {
    "UNKNOWN",
    "4x TVP5158 VS",
    "2x SII9135, 1x TVP7002 VC",
    "2x SIL1161A, 2x TVP7002 Catalog"
    "2x SIL1161A, 2x TVP7002 DVR"
};

static char *gMultiCh_boardRev[SYSTEM_PLATFORM_BOARD_REV_MAX] = {
    "UNKNOWN",
    "REV A",
    "REV B",
    "REV C",
    "DVR"
};





Int32 vpss_reset_video_devices ()
{
	Int status = -1;
	
	status = System_linkControl(SYSTEM_LINK_ID_M3VPSS,
								SYSTEM_M3VPSS_CMD_RESET_VIDEO_DEVICES,
								NULL,
								0,
								TRUE
								);

	return status;
}


Int32 vpss_detect_board()
{
	Int32 status;
	UInt32 boardRev;
	UInt32 cpuRev;
	UInt32 boardId;


	SystemVpss_PlatformInfo  platformInfo;

	status = System_linkControl(
				SYSTEM_LINK_ID_M3VPSS,
				SYSTEM_M3VPSS_CMD_GET_PLATFORM_INFO,
				&platformInfo,
				sizeof(platformInfo),
				TRUE
				);

	UTILS_assert(status==OSA_SOK);

	/* Get CPU version */
	cpuRev = platformInfo.cpuRev;
	if(cpuRev >= SYSTEM_PLATFORM_CPU_REV_MAX) {
		cpuRev = SYSTEM_PLATFORM_CPU_REV_UNKNOWN;
	}
	printf(" %u: MCFW  : CPU Revision [%s] !!! \r\n",
		OSA_getCurTimeInMsec(), gMultiCh_cpuName[cpuRev]);

	/* Detect board */
	boardId = platformInfo.boardId;
	if(boardId >= SYSTEM_PLATFORM_BOARD_MAX) {
		boardId = SYSTEM_PLATFORM_BOARD_UNKNOWN;
	}
	printf(" %u: MCFW  : Detected [%s] Board !!! \r\n",
		OSA_getCurTimeInMsec(), gMultiCh_boardName[boardId]);

	/* Get base board revision */
	boardRev = platformInfo.baseBoardRev;
	if(boardRev >= SYSTEM_PLATFORM_BOARD_REV_MAX) {
		boardRev = SYSTEM_PLATFORM_BOARD_REV_UNKNOWN;
	}
	printf(" %u: MCFW  : Base Board Revision [%s] !!! \r\n",
		OSA_getCurTimeInMsec(), gMultiCh_boardRev[boardRev]);

	if(boardId != SYSTEM_PLATFORM_BOARD_UNKNOWN) {
		/* Get daughter card revision */
		boardRev = platformInfo.dcBoardRev;
		if(boardRev >= SYSTEM_PLATFORM_BOARD_REV_MAX) {
			boardRev = SYSTEM_PLATFORM_BOARD_REV_UNKNOWN;
		}
		printf(" %u: MCFW  : Daughter Card Revision [%s] !!! \r\n",
			OSA_getCurTimeInMsec(), gMultiCh_boardRev[boardRev]);
	}

	return 0;
}



static void printf_vdis_param(VDIS_PARAMS_S *info)
{
	printf("numUserChannels=%d\n",info->numUserChannels);
	printf("tiedDevicesMask=%d\n",info->tiedDevicesMask);
	printf("enableConfigExtThsFilter=%d\n",info->enableConfigExtThsFilter);
	printf("enableConfigExtVideoEncoder=%d\n",info->enableConfigExtVideoEncoder);
	printf("enableEdgeEnhancement=%d\n",info->enableEdgeEnhancement);

	printf("enableLayoutGridDraw=%d\n",info->enableLayoutGridDraw);
	printf("numChannels=%d\n",info->numChannels);
	
	printf("#info->deviceParams[0].resolution=%d#\n",info->deviceParams[0].resolution);

}

Int32 vpss_displayctrl_init(VDIS_PARAMS_S *prm)
{
    Int32 status = -1;

//	prm->deviceParams[0].resolution =13;
	printf_vdis_param(prm);
	if(prm != NULL){
	    status = System_linkControl(
	        SYSTEM_LINK_ID_M3VPSS,
	        SYSTEM_M3VPSS_CMD_GET_DISPLAYCTRL_INIT,
	        &(*prm),
	        sizeof(*prm),
	        TRUE
	        );
	}
	
    return status;
}

Int32 vpss_displayctrl_deinit(VDIS_PARAMS_S *prm)
{
	Int32 status = -1;

	if(prm != NULL){
		status = System_linkControl(
			SYSTEM_LINK_ID_M3VPSS,
			SYSTEM_M3VPSS_CMD_GET_DISPLAYCTRL_DEINIT,
			&(*prm),
			sizeof(*prm),
			TRUE
			);
	}
	
	return status;
}

Int32 vpss_calc_cpuload_start()
{
	Int32 status = -1;
	
	status = System_linkControl(SYSTEM_LINK_ID_M3VPSS,
							   SYSTEM_COMMON_CMD_CPU_LOAD_CALC_START,
							   NULL,
							   0,
							   TRUE
							   );

	return status;
}


Int32 vpss_calc_cpuload_stop()
{
	Int32 status = -1;
	
	status = System_linkControl(SYSTEM_LINK_ID_M3VPSS,
							   SYSTEM_COMMON_CMD_CPU_LOAD_CALC_STOP,
							   NULL,
							   0,
							   TRUE
							   );

	return status;
}



Int32 vpss_calc_cpuload_reset()
{
	Int32 status = -1;
	
	status = System_linkControl(SYSTEM_LINK_ID_M3VPSS,
							   SYSTEM_COMMON_CMD_CPU_LOAD_CALC_RESET,
							   NULL,
							   0,
							   TRUE
							   );

	return status;
}



Int32 vpss_print_cpuload()
{
	Int32 status = -1;

	SystemCommon_PrintStatus printStatus;

	memset(&printStatus, 0, sizeof(printStatus));

	printStatus.printCpuLoad = TRUE;
	printStatus.printTskLoad = TRUE;
	
	status = System_linkControl(SYSTEM_LINK_ID_M3VPSS,
							   SYSTEM_COMMON_CMD_PRINT_STATUS,
							   &printStatus,
							   sizeof(printStatus),
							   TRUE
							   );
		
	return status;
}




