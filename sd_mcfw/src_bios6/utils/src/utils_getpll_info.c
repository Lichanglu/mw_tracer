/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2012 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <mcfw/src_bios6/utils/utils_getpll_info.h>
#include <mcfw/src_bios6/links_common/system/system_priv_common.h>

#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
Int32 Utils_getPLLStatus(Int8 *module, UInt32  pll_base)
{
    UInt32 temp, MN2, M2N, M2, N, M;
    Int32 freq = 0;
    
    temp = RD_MEM_32(pll_base+STATUS);
    
    if(((temp >> 4) & 0x00000001) == 0x1)
    {
        MN2=RD_MEM_32(pll_base+MN2DIV);
        M = MN2 & 0xFFF;
        M2N=RD_MEM_32(pll_base+M2NDIV);
        if(pll_base == MODENA_PLL_BASE) 
        {
            N = M2N & 0x3F;
            M2 = (M2N>>16) & 0x1F;
        }
        else 
        {
            N = M2N & 0xFF;
            M2 = (M2N>>16) & 0x7F;
        }
        freq = (M*(SYSTEM_OSC_FREQ/(N+1)))/M2;

        Vps_printf("\t%s Freq  : %d MHz\n", module, freq);
        //Vps_printf("\t%s_M   : %d\n", module, M);
        //Vps_printf("\t%s_N   : %d\n", module, N);
        //Vps_printf("\t%s_M2  : %d\n", module, M2);
    }
    else 
    {
        Vps_printf("%s: \t is NOT Locked \n", module);
    }
    Vps_printf("\n");
    return freq;
}
float Utils_getM3Frequency(Void)
{
    Int32 issFreq;
    float m3Freq;

    issFreq = Utils_getPLLStatus("ISS", ISS_PLL_BASE);
    if(issFreq == 0)
        m3Freq = 0;
    else
        m3Freq = (issFreq/2);

    return m3Freq;
}
Int32 Utils_getDSPFrequency(Void)
{
    float dspFreq=0;

#if defined(TI_814X_BUILD)
    dspFreq = Utils_getPLLStatus("DSP", DSP_PLL_BASE);
#endif    
    return dspFreq;
}

float Utils_getHDVPSSFrequency(Void)
{
    float dssFreq=0;

    dssFreq = Utils_getPLLStatus("HDVPSS", DSS_PLL_BASE);
    return dssFreq;
}

#else

float readPLL(char *module, unsigned int pll_ctrl, unsigned int pll_freq, unsigned int pll_div, unsigned int prcm_div)
{
	unsigned int MAIN_N, PREDIV, MDIV, INTFREQ, ctrl_reg, freq_reg;
	float FRACFREQ, FREQ, IPFREQ = 0;
	
	ctrl_reg = RD_MEM_32(pll_ctrl);
	if ((ctrl_reg & 0x00000008) == 0x8)
    {
    	freq_reg    = RD_MEM_32(pll_freq);
    	MAIN_N      = (ctrl_reg & 0xFFFF0000) >> 16;
    	PREDIV      = (ctrl_reg & 0xFF00)>>8;
    	FRACFREQ    = ((float)(freq_reg & 0xFFFFFF))/ ((float) (0x1000000-1));
    	INTFREQ     = (freq_reg & 0xF000000) >>24;
    	FREQ        = FRACFREQ + INTFREQ;
    	MDIV        = (RD_MEM_32(pll_div) & 0xFF);
    	IPFREQ      = (((float) (27 * MAIN_N * 8)) / ((float) ((PREDIV * FREQ) * MDIV))/(float)prcm_div);
    }
    else
    {
        Vps_printf("%s: \t is NOT Locked \n", module);
    }
	return IPFREQ;
}

float Utils_getM3Frequency(Void)
{
    Int32 prcmDiv;
    float freq;

    prcmDiv = (RD_MEM_32(CM_SYSCLK5_CLKSEL) & 0x01) + 1;
    freq = readPLL("SYSCLK5 (Ducati M3/ HDVPSS)", MAINPLL_CTRL, MAINPLL_FREQ4, MAINPLL_DIV4, prcmDiv);
    
    return freq;
}

float Utils_getDSPFrequency(Void)
{
    Int32 prcmDiv;
    float freq;

    prcmDiv = (RD_MEM_32(CM_SYSCLK1_CLKSEL) & 0x01) + 1;
    freq = readPLL("SYSCLK5 (Ducati M3/ HDVPSS)", MAINPLL_CTRL, MAINPLL_FREQ1, MAINPLL_DIV1, prcmDiv);
    
    return freq;
}

float Utils_getHDVPSSFrequency(Void)
{
    Int32 prcmDiv;
    float freq;

    prcmDiv = (RD_MEM_32(CM_SYSCLK1_CLKSEL) & 0x01) + 1;
    freq = readPLL("SYSCLK5 (Ducati M3/ HDVPSS)", MAINPLL_CTRL, MAINPLL_FREQ1, MAINPLL_DIV1, prcmDiv);
    
    return freq;
}

#endif

float Utils_getFrequency(Int32 module)
{
    switch (module)
    {
        case PLL_MODULE_DUCATI_M3:
            return Utils_getM3Frequency();

        case PLL_MODULE_DSP:
            return Utils_getDSPFrequency();

        case PLL_MODULE_HDVPSS:
            return Utils_getHDVPSSFrequency();

        default:
            return 0;
    }
}
