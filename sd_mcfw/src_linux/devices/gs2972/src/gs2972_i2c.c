/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include "ti_media_std.h"
#include "ti_vsys_common_def.h"
#include <device.h>
#include <device_videoDecoder.h>
#include <device_gs2972.h>
#include <gs2972_priv.h>
#include <osa_i2c.h>

static int gGS2972_Lps = -1;
static int gGS2972_Mode=-1;
static int gGS2972_TMDS = -1;
static int gGS2972_Hpv = 0;
static int gGS2972_Vps = 0;
static int gGS2972_CurStatus=-1;

static Vps_Gs2972_hpv_vps gs2972_gLineNum[DEVICE_STD_REACH_LAST] = {
	{"480I",		240,60,1587,720,240}, // 0-480ix60             
	{"576I",		288,50,1600,720,288},  // 1-576ix50
	{"480I",		240,60,1587,720,240}, // 2-480ix60             
	{"576I",		288,50,1600,720,288},  // 3-576ix50
	{"Revs",		0xFFFF,0xFFFF,0xFFFF,0,0}, // FVID2_STD_CIF, /**< Interlaced, 360x120 per field NTSC, 360x144 per field PAL. */
	{"Revs",		0xFFFF,0xFFFF,0xFFFF,0,0}, // FVID2_STD_HALF_D1, /**< Interlaced, 360x240 per field NTSC, 360x288 per field PAL. */
	{"Revs",		0xFFFF,0xFFFF,0xFFFF,0,0}, // FVID2_STD_D1, /**< Interlaced, 720x240 per field NTSC, 720x288 per field PAL. */
	{"480P",		0xFFFF,0xFFFF,0xFFFF,0,0}, // 7-480px60             
	{"576P",		0xFFFF,0xFFFF,0xFFFF,0,0},  // 8-576px50
	{"720P60",	720,60,1111,1280,720},  // 9-1280x720x60 
	{"720P50",	720,50,1333,1280,720},  // 10-1280x720x50 
	{"1080I60",	540,60,1481,1920,540},  // 11-1920x1080x60i                 
	{"1080I50",	540,50,1777,1920,540},  // 12-1920x1080x50i
	{"1080P60",	1080,60,740,1920,1080}, // 13-1920x1080x60p             
	{"1080P50",	1080,50,888,1920,1080},  // 14-1920x1080x50p
	{"1080P25",	1080,25,1777,1920,1080}, // 15-1920x1080x25p             
	{"1080P30",	1080,30,1481,1920,1080},  // 16-1920x1080x30p

	{"640x480@60",	525,60,1588,640,480},  // 17-640x480x60   
	{"640x480@72",	520,72,1320,640,480},  // 18-640x480x72   
	{"640x480@75",	500,75,1333,640,480},  // 19-640x480x75   
	{"640x480@85",	509,85,1155,640,480},  // 20-640x480x85   
	{"800x600@60",	628,60,1320,800,600},  // 21-800x600x60   
	{"800x600@72",	666,72,1040,800,600},  // 22-800x600x72
	{"800x600@75",	625,75,1066,800,600},  // 23-800x600x75   
	{"800x600@85",	631,85,931,800,600},  // 24-800x600x85   
	{"1024x768@60",	806,60,1033,1024,768},  // 25-1024x768x60                   
	{"1024x768@70",	806,70,885,1024,768},  // 26-1024x768x70
	{"1024x768@75",	800,75,833,1024,768},  // 27-1024x768x75                  
	{"1024x768@85",	808,85,728,1024,768},  // 28-1024x768x85 
	{"1280x768@60",	798,60,1054,1280,768},  // 29-1280x768x60 
	{"1280x768@75",	0xFFFF,0xFFFF,0xFFFF,0,0},  // 30-1280x768x75 
	{"1280x768@85",	0xFFFF,0xFFFF,0xFFFF,0,0},  // 31-1280x768x85 
	{"1280x800@60",	828,60,1006,1280,800},  // 32-1280x800x60 
	{"1280x960@60",	1000,60,833,1280,960},  // 33-1280x960x60 
	{"1280x1024@60",	1066,60,781,1280,1024}, // 34-1280x1024x60
	{"1280x1024@75",	1066,75,625,1280,1024}, // 35-1280x1024x75
	{"1280x1024@85",	0xFFFF,0xFFFF,0xFFFF,0,0}, // 36-1280x1024x85
	{"1440x900@60",	934,60,901,1440,900}, // 37-1440x900x60
	{"1400x1050@60",	1089,60,765,1400,1050}, // 38-1400x1050x60
	{"1400x1050@75",	0xFFFF,0xFFFF,0xFFFF,0,0}, // 39-1400x1050x75
	{"1600x1200@60",	1250,60,666,1600,1200},// 40-1600x1200x60
	{"1920x1200@60",	1244,60,675,1920,1200},// 41-1920x1200x60
	{"2560x1440@60",	1481,60,0xFFFF,2560,1440},// 42-2560x1440x60
	
    	{"Revs",0xFFFF,0xFFFF,0xFFFF,0,0}, // FVID2_STD_MUX_2CH_D1,/**< Interlaced, 2Ch D1, NTSC or PAL. */
    	{"Revs",0xFFFF,0xFFFF,0xFFFF,0,0}, // FVID2_STD_MUX_2CH_HALF_D1, /**< Interlaced, 2ch half D1, NTSC or PAL. */
    	{"Revs",0xFFFF,0xFFFF,0xFFFF,0,0},// FVID2_STD_MUX_2CH_CIF, /**< Interlaced, 2ch CIF, NTSC or PAL. */
    	{"Revs",0xFFFF,0xFFFF,0xFFFF,0,0}, // FVID2_STD_MUX_4CH_D1, /**< Interlaced, 4Ch D1, NTSC or PAL. */
    	{"Revs",0xFFFF,0xFFFF,0xFFFF,0,0}, // FVID2_STD_MUX_4CH_CIF, /**< Interlaced, 4Ch CIF, NTSC or PAL. */
    	{"Revs",0xFFFF,0xFFFF,0xFFFF,0,0}, // FVID2_STD_MUX_4CH_HALF_D1, /**< Interlaced, 4Ch Half-D1, NTSC or PAL. */
    	{"Revs",0xFFFF,0xFFFF,0xFFFF,0,0},// FVID2_STD_MUX_8CH_CIF, /**< Interlaced, 8Ch CIF, NTSC or PAL. */
    	{"Revs",0xFFFF,0xFFFF,0xFFFF,0,0},// FVID2_STD_MUX_8CH_HALF_D1, /**< Interlaced, 8Ch Half-D1, NTSC or PAL. */

	{"Revs",0xFFFF,0xFFFF,0xFFFF,0,0}, // FVID2_STD_AUTO_DETECT, /**< Auto-detect standard. Used in capture mode. */
	{"Revs",0xFFFF,0xFFFF,0xFFFF,0,0}, // FVID2_STD_CUSTOM, /**< Custom standard used when connecting to external LCD etc...
			//The video timing is provided by the application.
			//Used in display mode. */

	{"Max",0xFFFF,0xFFFF,0xFFFF,0,0} // FVID2_STD_MAX
};

static Vps_Gs2972_sav_eav_vbi  gs2972_SavEavVbi[DEVICE_STD_REACH_LAST] = {
	{"480I",		0x00,0x00,0x00,0x00},  			// 0-480ix60                                                
	{"576I",		0x00,0x00,0x00,0x00},  			// 1-576ix50     
	{"480I",		0x00,0x00,0x00,0x00},  			// 2-480ix60                                                
	{"576I",		0x00,0x00,0x00,0x00},  			// 3-576ix50     
	{"Revs",		0x00,0x00,0x00,0x00},			// FVID2_STD_CIF, /**< Interlaced, 360x120 per field NTSC, 360x144 per field PAL. */                                       
	{"Revs",		0x00,0x00,0x00,0x00},  			// FVID2_STD_HALF_D1, /**< Interlaced, 360x240 per field NTSC, 360x288 per field PAL. */                                               
	{"Revs",		0x00,0x00,0x00,0x00},  			// FVID2_STD_D1, /**< Interlaced, 720x240 per field NTSC, 720x288 per field PAL. */                                               
	{"480P",		0x00,0x00,0x00,0x00},  			// 7-480px60     
	{"576P",		0x00,0x00,0x00,0x00},			// 8-576px50   
	{"720P60",	0x00,0x00,0x00,0x00},		// 9-1280x720x60
	{"720P50",	0x00,0x00,0x00,0x00},  		// 10-1280x720x50           
	{"1080I60",	0x00,0x00,0x00,0x00},		// 11-1920x1080x60i              	
	{"1080I50",	0x00,0x00,0x00,0x00},		// 12-1920x1080x50i  
	{"1080P60",	0x00,0x00,0x00,0x00},		// 13-1920x1080x60p      
	{"1080P50",	0x00,0x00,0x00,0x00},		// 14-1920x1080x50p      
	{"1080P25",	0x00,0x00,0x00,0x00},		// 15-1920x1080x25p              
	{"1080P30",	0x00,0x00,0x00,0x00},		// 16-1920x1080x30p  
	
	{"640x480@60",	100,746,525,45},				// 17 -640x480x60            
	{"640x480@72",	100,746,520,40},				// 18 -640x480x72
	{"640x480@75",	100,746,500,20},				// 19 -640x480x75
	{"640x480@85",	100,746,509,29},				// 20 -640x480x85
	{"800x600@60",	100,906,628,28}, 				// 21 -800x600x60
	{"800x600@72",	100,906,666,66},	 			// 22 -800x600x72               
	{"800x600@75",	100,906,625,25},	 			// 23 -800x600x75               
	{"800x600@85",	100,906,631,31},	 			// 24 -800x600x85
	{"1024x768@60",	100,1130,806,38},				 // 25 -1024x768x60              
	{"1024x768@70",	100,1130,806,38},				 // 26 -1024x768x70              
	{"1024x768@75",	100,1130,800,32},				 // 27-1024x768x75              
	{"1024x768@85",	100,1130,808,40},		 		// 28-1024x768x85              
	{"1280x768@60",	100,1386,798,30},				 // 29-1280x768x60
	{"1280x768@75",	0x00,0x00,0x00,0x00},			 // 30-1280x768x75 
	{"1280x768@85",	0x00,0x00,0x00,0x00},	 		// 31-1280x768x85 
	{"1280x800@60",	200,1486,830,30},	 			// 32-1280x800x60 
	{"1280x960@60",	200,1486,990,30},	 			// 33-1280x960x60 
	{"1280x1024@60",	360,1646,1066,42},	 		// 34-1280x1024x60
	{"1280x1024@75",	100,1386,1066,42},	 		// 35-1280x1024x75	
	{"1280x1024@85",	100,1386,1072,48},	 		// 36-1280x1024x85	
	{"1440x900@60",	100,1506,930,30},	 			// 37-1440x900x60
	{"1400x1050@60",	100,1506,1087,37},	 		// 38-1400x1050x60
	{"1400x1050@75",	100,1506,1096,46},	 		// 39-1400x1050x75
	{"1600x1200@60",	480,2086,1250,50},  			 // 40-1600x1200x60
	{"1920x1200@60",	536,2462,1242,38},  			 // 41-1920x1200x60
	{"2560x1440@60",	0x00,0x00,0x00,0x00},			// 42-2560x1440x60

    	{"Revs",0x00,0x00,0x00,0x00}, // FVID2_STD_MUX_2CH_D1,/**< Interlaced, 2Ch D1, NTSC or PAL. */
    	{"Revs",0x00,0x00,0x00,0x00}, // FVID2_STD_MUX_2CH_HALF_D1, /**< Interlaced, 2ch half D1, NTSC or PAL. */
    	{"Revs",0x00,0x00,0x00,0x00},  // FVID2_STD_MUX_2CH_CIF, /**< Interlaced, 2ch CIF, NTSC or PAL. */
    	{"Revs",0x00,0x00,0x00,0x00},  // FVID2_STD_MUX_4CH_D1, /**< Interlaced, 4Ch D1, NTSC or PAL. */
    	{"Revs",0x00,0x00,0x00,0x00},  // FVID2_STD_MUX_4CH_CIF, /**< Interlaced, 4Ch CIF, NTSC or PAL. */
    	{"Revs",0x00,0x00,0x00,0x00}, // FVID2_STD_MUX_4CH_HALF_D1, /**< Interlaced, 4Ch Half-D1, NTSC or PAL. */
    	{"Revs",0x00,0x00,0x00,0x00},  // FVID2_STD_MUX_8CH_CIF, /**< Interlaced, 8Ch CIF, NTSC or PAL. */
    	{"Revs",0x00,0x00,0x00,0x00},  // FVID2_STD_MUX_8CH_HALF_D1, /**< Interlaced, 8Ch Half-D1, NTSC or PAL. */
	{"Revs",0x00,0x00,0x00,0x00},  // FVID2_STD_AUTO_DETECT, /**< Auto-detect standard. Used in capture mode. */
	{"Revs",0x00,0x00,0x00,0x00}, // FVID2_STD_CUSTOM, /**< Custom standard used when connecting to external LCD etc...
														//The video timing is provided by the application.
														//Used in display mode. */
	{"Max",0x00,0x00,0x00,0x00} // FVID2_STD_MAX
};

static Vps_Gs2972_InMode ArgMode_GS2972[DEVICE_STD_REACH_LAST] = {
	{"480I",0x00,0x21,0x5d,0x10,0xe8,0x70,0xd0,0x01,0x7e,0x4e,0x20},  	// 0-480ix60                                                
	{"576I",0x0d,0x21,0x5F,0xd0,0x21,0x5d,0xd0,0x0d,0x21,0x5d,0xd0},  	// 1-576ix50     
	{"480I",0x00,0x21,0x5d,0x10,0xe8,0x70,0xd0,0x01,0x7e,0x4e,0x20},  	// 2-480ix60                                                
	{"576I",0x0d,0x21,0x5F,0xd0,0x21,0x5d,0xd0,0x0d,0x21,0x5d,0xd0},  	// 3-576ix50     
	{"Revs",0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	// FVID2_STD_CIF, /**< Interlaced, 360x120 per field NTSC, 360x144 per field PAL. */                                       
	{"Revs",0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},  	// FVID2_STD_HALF_D1, /**< Interlaced, 360x240 per field NTSC, 360x288 per field PAL. */                                               
	{"Revs",0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},  	// FVID2_STD_D1, /**< Interlaced, 720x240 per field NTSC, 720x288 per field PAL. */                                               
	{"480P60",0x07,0x21,0x5F,0xd0,0x21,0x5F,0xd0,0x07,0x21,0x5F,0xd0},  	// 7-480px60     
	{"576P50",0x00,0x21,0x5d,0xd0,0xE6,0x90,0xd0,0x02,0x58,0x32,0x00},	// 8-576px50   
	{"720P60",0x0a,0x21,0x5F,0xd0,0xe6,0x80,0xd0,0x02,0x7f,0x2e,0xe0},	// 9-1280x720x60
	{"720P50",0x2a,0x21,0x5F,0xd0,0x21,0x5F,0xd0,0x07,0x21,0x5F,0xd0},  	// 10-1280x720x50           
	{"1080I60",0x0c,0x21,0x5d,0x10,0x21,0x5d,0x10,0x05,0x21,0x5d,0x10},	// 11-1920x1080x60i              	
	{"1080I50",0x2c,0x21,0x5d,0x10,0xE6,0x90,0xd0,0x02,0x40,0x33,0xc0},	// 12-1920x1080x50i  
	{"1080P60",0x0b,0x21,0x5d,0x00,0x21,0x5e,0x10,0x05,0x21,0x5e,0x10},	// 13-1920x1080x60p      
	{"1080P50",0x2b,0x21,0x5d,0x00,0x21,0x5d,0x10,0x05,0x21,0x5d,0x10},	// 14-1920x1080x50p      
	{"1080P25",0x05,0x21,0x5d,0x00,0x21,0x5d,0x10,0x05,0x21,0x5d,0x10},	// 15-1920x1080x25p              
	{"1080P30",0x05,0x61,0x5e,0x00,0x61,0x5e,0x10,0x05,0x61,0x5e,0x10},	// 16-1920x1080x30p  
	
	{"640x480@60",0x08,0x11,0x5b,0x10,0x63,0x5b,0x10,0x08,0x11,0x5b,0x10},	 // 17 -640x480x60            
	{"640x480@72",0x09,0x11,0x5b,0x10,0x63,0x5b,0x10,0x09,0x11,0x5b,0x10},  	 // 18 -640x480x72
	{"640x480@75",0x0a,0x11,0x5c,0x10,0x63,0x5c,0x10,0x0a,0x11,0x5c,0x10},  	 // 19 -640x480x75
	{"640x480@85",0x0b,0x11,0x5c,0x10,0x63,0x5c,0x10,0x0b,0x11,0x5c,0x10},	 // 20 -640x480x85
	{"800x600@60",0x01,0x11,0x5c,0x10,0x63,0x5c,0x10,0x01,0x11,0x5c,0x10}, 	 // 21 -800x600x60
	{"800x600@72",0x02,0x11,0x5c,0x10,0x63,0x5c,0x10,0x02,0x11,0x5c,0x10},	 // 22 -800x600x72               
	{"800x600@75",0x03,0x11,0x5c,0x10,0x63,0x5c,0x10,0x03,0x11,0x5c,0x10},	 // 23 -800x600x75               
	{"800x600@85",0x04,0x21,0x5c,0x10,0x63,0x5c,0x10,0x04,0x21,0x5c,0x10},	 // 24 -800x600x85
	{"1024x768@60",0x0c,0x21,0x5d,0x10,0x63,0x5d,0x10,0x0c,0x21,0x5d,0x10},	 // 25 -1024x768x60              
	{"1024x768@70",0x0d,0x21,0x5d,0x10,0x63,0x5d,0x10,0x0d,0x21,0x5d,0x10},	 // 26 -1024x768x70              
	{"1024x768@75",0x0e,0x21,0x5d,0x10,0x63,0x5d,0x10,0x0e,0x21,0x5d,0x10},	 // 27-1024x768x75              
	{"1024x768@85",0x0f,0x21,0x5d,0x10,0x63,0x5d,0x10,0x0f,0x21,0x5d,0x10},	 // 28-1024x768x85              
	{"1280x768@60",0x00,0x21,0x5d,0xd0,0xE6,0x90,0xd0,0x72,0x58,0x31,0xe0},	 // 29-1280x768x60
	{"1280x768@75",0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	 // 30-1280x768x75 
	{"1280x768@85",0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	 // 31-1280x768x85 
	{"1280x800@60",0x00,0x21,0x5d,0x10,0xE6,0x90,0xd0,0x02,0x40,0x33,0xc0},	// 32-1280x800x60 
	{"1280x960@60",0x00,0x21,0x5d,0x10,0xe6,0xb0,0x10,0x01,0xe0,0x3e,0x80},	// 33-1280x960x60 
	{"1280x1024@60",0x05,0x21,0x5d,0x10,0x63,0x5d,0x10,0x05,0x21,0x5d,0x10},	 // 34-1280x1024x60
	{"1280x1024@75",0x06,0x21,0x5d,0x10,0x63,0x5e,0x10,0x05,0x21,0x5e,0x10},	 // 35-1280x1024x75	
	{"1280x1024@85",0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	 // 36-1280x1024x85
	{"1440x900@60",0x00,0x21,0x5d,0x10,0xe7,0x30,0x10,0x02,0x00,0x3a,0x60},	// 37-1440x900x60
	{"1400x1050@60",0x00,0x21,0x5d,0x10,0xE7,0x48,0x10,0x01,0xBA,0x44,0x1E},	 // 38-1400x1050x60
	{"1400x1050@75",0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	 // 39-1400x1050x75
	{"1600x1200@60",0x00,0x21,0x5d,0x10,0xe8,0x70,0xd0,0x01,0x7e,0x4e,0x20},  	 // 40-1600x1200x60
	{"1920x1200@60",0x00,0x21,0x5d,0x10,0xea,0x20,0xd0,0x01,0x7c,0x4d,0xa0},	 //41-1920x1200x60   
	{"2560x1440@60",	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	// 42-2560x1440x60

    	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, // FVID2_STD_MUX_2CH_D1,/**< Interlaced, 2Ch D1, NTSC or PAL. */
    	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, // FVID2_STD_MUX_2CH_HALF_D1, /**< Interlaced, 2ch half D1, NTSC or PAL. */
    	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  // FVID2_STD_MUX_2CH_CIF, /**< Interlaced, 2ch CIF, NTSC or PAL. */
    	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  // FVID2_STD_MUX_4CH_D1, /**< Interlaced, 4Ch D1, NTSC or PAL. */
    	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  // FVID2_STD_MUX_4CH_CIF, /**< Interlaced, 4Ch CIF, NTSC or PAL. */
    	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, // FVID2_STD_MUX_4CH_HALF_D1, /**< Interlaced, 4Ch Half-D1, NTSC or PAL. */
    	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  // FVID2_STD_MUX_8CH_CIF, /**< Interlaced, 8Ch CIF, NTSC or PAL. */
    	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  // FVID2_STD_MUX_8CH_HALF_D1, /**< Interlaced, 8Ch Half-D1, NTSC or PAL. */
	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  // FVID2_STD_AUTO_DETECT, /**< Auto-detect standard. Used in capture mode. */
	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, // FVID2_STD_CUSTOM, /**< Custom standard used when connecting to external LCD etc...
														//The video timing is provided by the application.
														//Used in display mode. */
	{"Max",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF} // FVID2_STD_MAX
};

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
UInt16 Device_gs2972_Read16_CPLD(Device_Gs2972Obj * pObj, UInt8 RegAddr)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[2]={0};
	UInt16 data;
	
	regAddr[0] = RegAddr;
	OSA_CPLD_i2cRead16(&gDevice_gs2972CommonObj.i2cHandle,0, regAddr, regValue, 2);
	data = regValue[1]<<8 | regValue[0];
	
	return data;
}

UInt16 Device_gs2972_Write16_CPLD(Device_Gs2972Obj * pObj, UInt8 RegAddr, UInt16 RegVal)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	regValue[0] = RegVal&0xFF;
	regValue[1] = (RegVal&0xFF00)>>8;
	status = OSA_CPLD_i2cWrite16 (&gDevice_gs2972CommonObj.i2cHandle, 0, regAddr, regValue, 2 );

	return status;
}

UInt8 Device_gs2972_Read8(Device_Gs2972Obj * pObj, UInt8 RegAddr)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	status = OSA_i2cRead8 (&gDevice_gs2972CommonObj.i2cHandle, GS2972_IIC_SLAVE_ADDR, regAddr, regValue, 1);

	return regValue[0];
}

Int32 Device_gs2972_Write8(Device_Gs2972Obj * pObj, UInt8 RegAddr, UInt8 RegVal)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	regValue[0] = RegVal;
	status = OSA_i2cWrite8 (&gDevice_gs2972CommonObj.i2cHandle, GS2972_IIC_SLAVE_ADDR, regAddr, regValue, 1 );

	return status;
}

static Int32 Device_gs2972Config(Device_Gs2972Obj *pObj, int inMode, int tmds)
{
	Int32 	retVal = 0;
	return retVal;
}

static Int32 Device_gs2972GetResolution(Device_Gs2972Obj *pObj)
{
	Int32 	hsfq = 0, linenum=0;
	int i=0, inMode=0;;
	hsfq  = Device_gs2972_Read16_CPLD(pObj, 0x4);
	linenum  = Device_gs2972_Read16_CPLD(pObj, 0x6);

	for(i = 0;i<DEVICE_STD_REACH_LAST;i++){
		if(((hsfq>gs2972_gLineNum[i].lps-10)&&(hsfq<gs2972_gLineNum[i].lps+10))&&((linenum>gs2972_gLineNum[i].hpv-4)&&(linenum<gs2972_gLineNum[i].hpv+4))){
			break;
		}

	}

	if(i == DEVICE_STD_REACH_LAST){
		for(i = 0;i<DEVICE_STD_REACH_LAST;i++){
			if(((hsfq>gs2972_gLineNum[i].lps-10)&&(hsfq<gs2972_gLineNum[i].lps+10))&&((linenum>gs2972_gLineNum[i].hpv-10)&&(linenum<gs2972_gLineNum[i].hpv+10))){
				break;
			}
	    	}
	}

	if(i==DEVICE_STD_REACH_LAST)
		inMode = -1;
	else
		inMode = i;

	gGS2972_Hpv= linenum;
	gGS2972_Lps = hsfq;
	gGS2972_Mode = inMode;

	if(gGS2972_Mode != -1){
		printf("###########################################\n");
		printf("Get GS2972 Resolution!\n");
		printf("gGS2972_std = %s \n",ArgMode_GS2972[gGS2972_Mode].name);
		printf("gGS2972_hpv = %d gGS2972_lps = %d \n",gGS2972_Hpv,gGS2972_Lps);
		printf("gGS2972_Mode = %d \n",gGS2972_Mode);
		printf("###########################################\n");
	}else{
		printf("###########################################\n");
		printf("Get GS2972 Resolution!\n");
		printf("gGS2972_std = unknow! \n");
		printf("gGS2972_hpv = %d gGS2972_lps = %d \n",gGS2972_Hpv,gGS2972_Lps);
		printf("gGS2972_Mode = %d \n",gGS2972_Mode);
		printf("###########################################\n");
	}

	return inMode;
}

Int32 Device_gs2972GetExternInformation(Device_Gs2972Obj * pObj,
							Device_VideoDecoderExternInforms* extenInfor)
{
	Int32 status = 0;
	
	memcpy(extenInfor->DeviceName, "gs2972", sizeof(extenInfor->DeviceName));
	extenInfor->ModeID = gGS2972_Mode;
	extenInfor->SignalTmds = gGS2972_TMDS;
	extenInfor->SignalHpv = gGS2972_Hpv;
	extenInfor->SignalFreq = gGS2972_Vps;
	extenInfor->SignalYPbPr = 0;
	extenInfor->SignalHsfqFpga = gGS2972_Lps;
	extenInfor->SignalLinenumFpga = gGS2972_Hpv;
	
	return status;
}

/* Set ADV7441 mode based on

  - mux mode - line or pixel or no-mux
  - number of channels
  - resolution
  - 8/16-bit data width
  - NTSC/PAL standard
  - cascade mode of operation
*/
Int32 Device_gs2972SetVideoMode ( Device_Gs2972Obj * pObj,
                                Device_VideoDecoderVideoModeParams * pPrm )
{
    Int32 status = 0;

    return status;
}

/* reset ADV7441 OFM logic  */
Int32 Device_gs2972Reset ( Device_Gs2972Obj * pObj )
{
	Int32           retVal = 0;
	printf("GS2972 Reset OK!\n");
	return (retVal);    
}

/*
  Enable ADV7441 output port
*/
Int32 Device_gs2972OutputEnable ( Device_Gs2972Obj * pObj, UInt32 enable )
{
    Int32 status = 0;

    return status;
}

/* start/enable output  */
Int32 Device_gs2972Start ( Device_Gs2972Obj * pObj )
{
    Int32 status = 0;

    status = Device_gs2972OutputEnable ( pObj, TRUE );

    return status;
}

/* stop/disable output  */
Int32 Device_gs2972Stop ( Device_Gs2972Obj * pObj )
{
    Int32 status = 0;

    status = Device_gs2972OutputEnable ( pObj, FALSE );

    return status;
}

/*
  Get ADV7441 chip ID, revision ID and firmware patch ID
*/
Int32 Device_gs2972GetChipId ( Device_Gs2972Obj * pObj,
                             Device_VideoDecoderChipIdParams * pPrm,
                             Device_VideoDecoderChipIdStatus * pStatus )
{
    Int32 status = 0;

    return status;
}

/*
  Get ADV7441 detect vide standard status

  Can be called for each channel
*/
Int32 Device_gs2972GetVideoStatus ( Device_Gs2972Obj * pObj,
                                     VCAP_VIDEO_SOURCE_STATUS_PARAMS_S * pPrm,
                                     VCAP_VIDEO_SOURCE_CH_STATUS_S     * pStatus )
{
	Int32 status = 0;
	
	Device_gs2972GetResolution(pObj);
	if(gGS2972_Mode != -1){
		pStatus->isVideoDetect = TRUE;
		pStatus->frameWidth = gs2972_gLineNum[gGS2972_Mode].width;
		pStatus->frameHeight = gs2972_gLineNum[gGS2972_Mode].hight;
		pStatus->isInterlaced = FALSE;
	}else{
		pStatus->isVideoDetect = FALSE;
	}
	
	return status;
}

/*
  Set video color related parameters
*/
Int32 Device_gs2972SetVideoColor ( Device_Gs2972Obj * pObj,
                                 Device_VideoDecoderColorParams * pPrm )
{
    Int32 status = 0;
    return status;
}

/*
  Set video noise filter related parameters
*/
Int32 Device_gs2972SetVideoNf ( Device_Gs2972Obj * pObj,
                              Device_Gs2972VideoNfParams * pPrm )
{
    Int32 status = 0;
    return status;
}

/* write to I2C registers */
Int32 Device_gs2972RegWrite ( Device_Gs2972Obj * pObj,
                            Device_VideoDecoderRegRdWrParams * pPrm )
{
    Int32 status = 0;
    Device_VideoDecoderCreateParams *pCreateArgs;

    if ( pPrm == NULL )
        return -1;

    pCreateArgs = &pObj->createArgs;

    if ( pPrm->deviceNum > pCreateArgs->numDevicesAtPort )
        return -1;

    status = OSA_i2cWrite8 (&gDevice_gs2972CommonObj.i2cHandle, pCreateArgs->deviceI2cAddr[0], pPrm->regAddr, pPrm->regValue8, pPrm->numRegs );

    return status;
}

/* read from I2C registers */
Int32 Device_gs2972RegRead ( Device_Gs2972Obj * pObj,
                           Device_VideoDecoderRegRdWrParams * pPrm )
{
    Int32 status = 0;
    Device_VideoDecoderCreateParams *pCreateArgs;

    if ( pPrm == NULL )
        return -1;

    pCreateArgs = &pObj->createArgs;

    if ( pPrm->deviceNum > pCreateArgs->numDevicesAtPort )
        return -1;

    status = OSA_i2cRead8 (&gDevice_gs2972CommonObj.i2cHandle, pCreateArgs->deviceI2cAddr[0], pPrm->regAddr, pPrm->regValue8, pPrm->numRegs);

    return status;
}


