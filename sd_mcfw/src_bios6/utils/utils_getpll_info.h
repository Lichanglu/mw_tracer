/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
    \file utils_getpll_info.h
    \brief Get PLL Clocks API
*/

#ifndef _UTILS_GETPLL_INFO_H_
#define _UTILS_GETPLL_INFO_H_

#include <mcfw/src_bios6/utils/utils.h>


#ifdef WR_MEM_32
#undef WR_MEM_32
#endif
#define WR_MEM_32(addr, data) *(volatile unsigned int*)(addr) =(unsigned int)(data)

#ifdef RD_MEM_32
#undef RD_MEM_32
#endif
#define RD_MEM_32(addr) *(volatile unsigned int*)(addr)

/****************** PLL Configuration ******************************************/
#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
    #define SYSTEM_OSC_FREQ          20
    /***    PLL,Control Base Adress  Base Address   ***********/
    #define PLL_BASE_ADDRESS         0x481C5000
    #define CONTROL_BASE_ADDRESS     0x48140000
    #define OSC_SRC_CTRL            (PLL_BASE_ADDRESS+0x02c0)
    #define ARM_SRC_CLK             (PLL_BASE_ADDRESS+0x02c4)

    /***    Top Level ADPLLJ  Base Address   ***********/
    #define MODENA_PLL_BASE         (PLL_BASE_ADDRESS+0x048)
    #define L3_PLL_BASE             (PLL_BASE_ADDRESS+0x110)
    #define IVA_PLL_BASE            (PLL_BASE_ADDRESS+0x0E0)
    #define DSS_PLL_BASE            (PLL_BASE_ADDRESS+0x170)
    #define SGX_PLL_BASE            (PLL_BASE_ADDRESS+0x0B0)
    #define DSP_PLL_BASE            (PLL_BASE_ADDRESS+0x080)
    #define ISS_PLL_BASE            (PLL_BASE_ADDRESS+0x140)
    #define USB_PLL_BASE            (PLL_BASE_ADDRESS+0x260)
    #define AUDIO_PLL_BASE          (PLL_BASE_ADDRESS+0x230)
    #define VIDEO_0_PLL_BASE        (PLL_BASE_ADDRESS+0x1A0)
    #define VIDEO_1_PLL_BASE        (PLL_BASE_ADDRESS+0x1D0)
    #define HDMI_PLL_BASE           (PLL_BASE_ADDRESS+0x200)
    #define DDR_PLL_BASE            (PLL_BASE_ADDRESS+0x290)
    #define TIMER_CLKSRC				(PLL_BASE_ADDRESS + 0x02E0)

	/********** ADPLL intrnal Offset Registers  ***********/
    #define CLKCTRL 				0x4
    #define TENABLE 				0x8
    #define TENABLEDIV 				0xC
    #define M2NDIV  				0x10
    #define MN2DIV    			    0x14
    #define STATUS 				    0x24

	/*********** MODENA ADPLLS REGISTERS  *********/
    #define MODENAPLL_CLKCTRL		(PLL_BASE_ADDRESS+0x04c)
    #define MODENAPLL_TENABLE		(PLL_BASE_ADDRESS+0x050)
    #define MODENAPLL_TENABLEDIV   (PLL_BASE_ADDRESS+0x054)
    #define MODENAPLL_M2NDIV    	(PLL_BASE_ADDRESS+0x058)
    #define MODENAPLL_MN2DIV    	(PLL_BASE_ADDRESS+0x05c)
    #define MODENAPLL_STATUS		(PLL_BASE_ADDRESS+0x06c)
#else

	#define CLKIN                27
    
    #define MAINPLL_CTRL        0x48140400
    #define MAINPLL_FREQ1       0x48140408
    #define MAINPLL_DIV1        0x4814040C
    #define MAINPLL_FREQ2       0x48140410
    #define MAINPLL_DIV2        0x48140414
	#define MAINPLL_FREQ3       0x48140418
    #define MAINPLL_DIV3        0x4814041C
    #define MAINPLL_FREQ4       0x48140420
    #define MAINPLL_DIV4        0x48140424
    #define MAINPLL_FREQ5       0x48140428
    #define MAINPLL_DIV5        0x4814042C
    #define MAINPLL_DIV6        0x48140434
	#define MAINPLL_DIV7        0x4814043C
    
    #define DDRPLL_CTRL         0x48140440
    #define DDRPLL_DIV1         0x4814044C
    #define DDRPLL_FREQ2        0x48140450
    #define DDRPLL_DIV2         0x48140454
    #define DDRPLL_FREQ3        0x48140458
	#define DDRPLL_DIV3         0x4814045C
    #define DDRPLL_FREQ4        0x48140460
    #define DDRPLL_DIV4         0x48140464
    #define DDRPLL_FREQ5        0x48140468
    #define DDRPLL_DIV5         0x4814046C
    
    #define VIDEOPLL_CTRL       0x48140470
	#define VIDEOPLL_FREQ1      0x48140478
    #define VIDEOPLL_DIV1       0x4814047C
    #define VIDEOPLL_FREQ2      0x48140480
    #define VIDEOPLL_DIV2       0x48140484
    #define VIDEOPLL_FREQ3      0x48140488
    #define VIDEOPLL_DIV3       0x4814048C
    
	#define AUDIOPLL_CTRL       0x481404A0
    #define AUDIOPLL_FREQ2      0x481404B0
    #define AUDIOPLL_DIV2       0x481404B4
    #define AUDIOPLL_FREQ3      0x481404B8
    #define AUDIOPLL_DIV3       0x481404BC
    #define AUDIOPLL_FREQ4      0x481404C0
    #define AUDIOPLL_DIV4       0x481404C4
	#define AUDIOPLL_FREQ5      0x481404C8
    #define AUDIOPLL_DIV5       0x481404CC
    
    #define CM_SYSCLK1_CLKSEL       0x48180300
    #define CM_SYSCLK2_CLKSEL       0x48180304
    #define CM_SYSCLK3_CLKSEL       0x48180308
    #define CM_SYSCLK4_CLKSEL       0x4818030C
	#define CM_SYSCLK5_CLKSEL       0x48180310
    #define CM_SYSCLK6_CLKSEL       0x48180314
    #define CM_SYSCLK7_CLKSEL       0x48180318
    #define CM_SYSCLK10_CLKSEL      0x48180324
    #define CM_SYSCLK11_CLKSEL      0x4818032C
    #define CM_SYSCLK13_CLKSEL      0x48180334
    #define CM_SYSCLK15_CLKSEL      0x48180338
	#define CM_SYSCLK19_CLKSEL      0x4818034C
    #define CM_SYSCLK20_CLKSEL      0x48180350
    #define CM_SYSCLK21_CLKSEL      0x48180354
    #define CM_SYSCLK22_CLKSEL      0x48180358
    #define CM_SYSCLK23_CLKSEL      0x481803B0
    #define CM_SYSCLK24_CLKSEL      0x481803B4
    #define CM_VPB3_CLKSEL           0x48180340
	#define CM_VPC1_CLKSEL           0x48180344
    #define CM_VPD1_CLKSEL           0x48180348
    #define CM_SYSCLK14_CLKSEL      0x48180370
    #define CM_SYSCLK16_CLKSEL      0x48180374
    #define CM_SYSCLK18_CLKSEL      0x48180378
    #define CM_AUDIOCLK_MCASP0_CLKSEL          0x4818037C
    #define CM_AUDIOCLK_MCASP1_CLKSEL          0x48180380
    #define CM_AUDIOCLK_MCASP2_CLKSEL          0x48180384
    #define CM_AUDIOCLK_MCBSP_CLKSEL           0x48180388

#endif

typedef enum
{
    PLL_MODULE_DUCATI_M3,
    PLL_MODULE_HDVPSS,
    PLL_MODULE_DSP /* Add more later */
} PLL_MODULES;

float Utils_getFrequency(Int32 module);

#endif

/* @} */
