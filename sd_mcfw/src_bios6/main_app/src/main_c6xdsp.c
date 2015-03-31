/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <mcfw/src_bios6/utils/utils.h>
#include <mcfw/interfaces/link_api/system.h>
#include <mcfw/src_bios6/links_common/system/system_priv_common.h>


Int32 System_start(Task_FuncPtr chainsMainFunc);

Void C6XDSP_main(UArg arg0, UArg arg1)
{
    char ch;

    while (1)
    {
        Task_sleep(100);
        Utils_getChar(&ch, BIOS_NO_WAIT);
        if (ch == 'x')
            break;
    }
}
Int32 main(void)
{
    float dspFreq = Utils_getFrequency(PLL_MODULE_DSP);
    if (dspFreq <= 0)  
        dspFreq = SYSTEM_DSP_FREQ;
    Utils_setCpuFrequency(dspFreq*1000*1000);
	
    System_start(C6XDSP_main);
    BIOS_start();

    return (0);
}
