/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <xdc/std.h>
#include "system_priv_c6xdsp.h"
#include <mcfw/interfaces/link_api/algLink.h>
#include <mcfw/interfaces/link_api/ipcLink.h>
#include <mcfw/src_bios6/links_c6xdsp/utils/utils_dsp.h>
#include "algLink_priv.h"
#ifdef  DSP_RPE_AUDIO_ENABLE
#include "ti/rpe.h"
#endif

System_DspObj gSystem_objDsp;

Int32 System_init()
{
    Int32 status = FVID2_SOK;

#ifdef SYSTEM_DEBUG
    Vps_printf(" %d: SYSTEM  : System DSP Init in progress !!!\n",
               Clock_getTicks());
#endif

#ifdef  DSP_RPE_AUDIO_ENABLE
    extern int32_t RpeServer_init (void *heapHdl);

    status = RpeServer_init(Utils_getAlgMemoryHeapHandle());
    Vps_printf(" %d: SYSTEM  : RpeServer_init() done... Ret Val %d!!!\n",
               Clock_getTicks(), status);
#endif

    IpcFramesInLink_init();
    IpcFramesOutLink_init();
    IpcBitsOutLink_init();

    Utils_dspInit();

    System_initLinks();

#ifdef SYSTEM_DEBUG
    Vps_printf(" %d: SYSTEM  : System DSP Init Done !!!\n", Clock_getTicks());
#endif
    return status;
}

Int32 System_deInit()
{
#ifdef SYSTEM_DEBUG
    Vps_printf(" %d: SYSTEM  : System Dsp De-Init in progress !!!\n",
               Clock_getTicks());
#endif

    IpcFramesInLink_deInit();
    IpcFramesOutLink_deInit();
    IpcBitsOutLink_deInit();

    System_deInitLinks();

#ifdef SYSTEM_DEBUG
    Vps_printf(" %d: SYSTEM  : System Dsp De-Init Done !!!\n",
               Clock_getTicks());
#endif
#ifdef  DSP_RPE_AUDIO_ENABLE
    extern int32_t RpeServer_deInit ();

    RpeServer_deInit();
    Vps_printf(" %d: SYSTEM  : RpeServer_deInit() done... !!!\n",
               Clock_getTicks());
#endif
    return FVID2_SOK;
}

extern UInt32 gAlgLinkRunCount[ALG_LINK_OBJ_MAX];
static void SystemHeartTskMain(UArg arg0, UArg arg1)
{
	UInt8 objId = 0;
	while(1)
	{
		/*10s打印一次各个任务是运行状态*/
		Task_sleep(10000);
		
		Vps_printf("----------tsk-run---------\n");
		for(objId = 0; objId < ALG_LINK_OBJ_MAX; objId++)
			Vps_printf("=========AlgLink-objId:%d:RunCount:%d\n",objId,gAlgLinkRunCount[objId]);
	}
}

Void System_initLinks()
{
    Vps_printf(" %d: SYSTEM  : Initializing Links !!! \r\n", Clock_getTicks());
    System_memPrintHeapStatus();

    AlgLink_init();
    NullLink_init();
    MergeLink_init();
    SelectLink_init();

    Task_Handle tsk;
    tsk = Task_create(SystemHeartTskMain, NULL, NULL);
    UTILS_assert(tsk != NULL);

    Vps_printf(" %d: SYSTEM  : Initializing Links ... DONE !!! \r\n",
               Clock_getTicks());
}

Void System_deInitLinks()
{
    Vps_printf(" %d: SYSTEM  : De-Initializing Links !!! \r\n",
               Clock_getTicks());

    SelectLink_deInit();
    MergeLink_deInit();
    NullLink_deInit();
    AlgLink_deInit();

    Vps_printf(" %d: SYSTEM  : De-Initializing Links ... DONE !!! \r\n",
               Clock_getTicks());
}
