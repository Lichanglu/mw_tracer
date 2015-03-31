/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <mcfw/src_bios6/utils/utils.h>
#include <mcfw/interfaces/link_api/system.h>
#include <mcfw/src_bios6/links_common/system/system_priv_common.h>

#include <mcfw/src_bios6/links_m3video/codec_utils/hdvicp2_config.h>

Int32 System_start(Task_FuncPtr chainsMainFunc);

Void M3VIDEO_main(UArg arg0, UArg arg1)
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
    float m3Freq = Utils_getFrequency(PLL_MODULE_DUCATI_M3);
	
    if (m3Freq <= 0)
        m3Freq = SYSTEM_M3VIDEO_FREQ;
    Utils_setCpuFrequency(m3Freq*1000*1000);
	
    HDVICP2_ClearIVAInterrupts();
    System_start(M3VIDEO_main);
    BIOS_start();

    return (0);
}
