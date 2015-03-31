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
#include <device_adv7619.h>
#include <adv7619_priv.h>
#include <osa_i2c.h>

#define FXTAL						28636360

#define ADV7619_I2C_INST_ID					(0x01)
#define ADV7619_IO_MAP_ADDR					(0x98>>1)
#define ADV7619_CEC_MAP_ADDR					(0x80>>1)
#define ADV7619_INFOFRAME_MAP_ADDR			(0x7c>>1)
#define ADV7619_DPLL_MAP_ADDR					(0x4c>>1)
#define ADV7619_KSV_MAP_ADDR					(0x64>>1)
#define ADV7619_EDID_MAP_ADDR				(0x6c>>1)
#define ADV7619_HDMI_MAP_ADDR				(0x68>>1)
#define ADV7619_CP_MAP_ADDR					(0x44>>1)

static int gAD7619_Mode=-1;
static int gAD7619_TMDS = -1;
static int gAD7619_Hpv = 0;
static int gAD7619_Vps = 0;
static int gAD7619_CurStatus=-1;
static AD7619_INPUT gAD7619_InterFace = AD7619_INPUT_UNKNOW;

static Vps_Adv7619_hpv_vps adv7619_gLineNum[DEVICE_STD_REACH_LAST] = {
	{"480I",		263,60,720,240}, // 0-480ix60             
	{"576I",		313,50,720,288},  // 1-576ix50
	{"480I",		263,60,720,240}, // 2-480ix60             
	{"576I",		313,50,720,288},  // 3-576ix50
	{"Revs",		0xFFFF,0xFFFF,0,0}, // FVID2_STD_CIF, /**< Interlaced, 360x120 per field NTSC, 360x144 per field PAL. */
	{"Revs",		0xFFFF,0xFFFF,0,0}, // FVID2_STD_HALF_D1, /**< Interlaced, 360x240 per field NTSC, 360x288 per field PAL. */
	{"Revs",		0xFFFF,0xFFFF,0,0}, // FVID2_STD_D1, /**< Interlaced, 720x240 per field NTSC, 720x288 per field PAL. */
	{"480P",		0xFFFF,0xFFFF,0,0}, // 7-480px60             
	{"576P",		0xFFFF,0xFFFF,0,0},  // 8-576px50
	{"720P60",	750,60,1280,720},  // 9-1280x720x60 
	{"720P50",	750,50,1280,720},  // 10-1280x720x50 
	{"1080I60",	563,60,1920,540},  // 11-1920x1080x60i                 
	{"1080I50",	562,50,1920,540},  // 12-1920x1080x50i
	{"1080P60",	1125,60,1920,1080}, // 13-1920x1080x60p             
	{"1080P50",	1125,50,1920,1080},  // 14-1920x1080x50p
	{"1080P25",	1125,25,1920,1080}, // 15-1920x1080x25p             
	{"1080P30",	1125,30,1920,1080},  // 16-1920x1080x30p

	{"640x480@60",	525,60,640,480},  // 17-640x480x60   
	{"640x480@72",	520,72,640,480},  // 18-640x480x72   
	{"640x480@75",	500,75,640,480},  // 19-640x480x75   
	{"640x480@85",	509,85,640,480},  // 20-640x480x85   
	{"800x600@60",	628,60,800,600},  // 21-800x600x60   
	{"800x600@72",	666,72,800,600},  // 22-800x600x72
	{"800x600@75",	625,75,800,600},  // 23-800x600x75   
	{"800x600@85",	631,85,800,600},  // 24-800x600x85   
	{"1024x768@60",	806,60,1024,768},  // 25-1024x768x60                   
	{"1024x768@70",	806,70,1024,768},  // 26-1024x768x70
	{"1024x768@75",	800,75,1024,768},  // 27-1024x768x75                  
	{"1024x768@85",	808,85,1024,768},  // 28-1024x768x85 
	{"1280x768@60",	798,60,1280,768},  // 29-1280x768x60 
	{"1280x768@75",	0xFFFF,0xFFFF,0,0},  // 30-1280x768x75 
	{"1280x768@85",	0xFFFF,0xFFFF,0,0},  // 31-1280x768x85 
	{"1280x1024@60",	1066,60,1280,1024}, // 32-1280x1024x60
	{"1280x1024@75",	1066,75,1280,1024}, // 33-1280x1024x75
	{"1280x1024@85",	0xFFFF,0xFFFF,0,0}, // 34-1280x1024x85
	{"1400x1050@60",	1089,60,1400,1050}, // 35-1400x1050x60
	{"1400x1050@75",	0xFFFF,0xFFFF,0,0}, // 36-1400x1050x75
	{"1600x1200@60",	1250,60,1600,1200},// 37-1600x1200x60
	{"1920x1200@60",	1244,60,1920,1200},// 38-1920x1200x60
	{"2560x1440@60",	1481,60,2560,1440},// 39-2560x1440x60
	
    	{"Revs",0xFFFF,0xFFFF,0,0}, // FVID2_STD_MUX_2CH_D1,/**< Interlaced, 2Ch D1, NTSC or PAL. */
    	{"Revs",0xFFFF,0xFFFF,0,0}, // FVID2_STD_MUX_2CH_HALF_D1, /**< Interlaced, 2ch half D1, NTSC or PAL. */
    	{"Revs",0xFFFF,0xFFFF,0,0},// FVID2_STD_MUX_2CH_CIF, /**< Interlaced, 2ch CIF, NTSC or PAL. */
    	{"Revs",0xFFFF,0xFFFF,0,0}, // FVID2_STD_MUX_4CH_D1, /**< Interlaced, 4Ch D1, NTSC or PAL. */
    	{"Revs",0xFFFF,0xFFFF,0,0}, // FVID2_STD_MUX_4CH_CIF, /**< Interlaced, 4Ch CIF, NTSC or PAL. */
    	{"Revs",0xFFFF,0xFFFF,0,0}, // FVID2_STD_MUX_4CH_HALF_D1, /**< Interlaced, 4Ch Half-D1, NTSC or PAL. */
    	{"Revs",0xFFFF,0xFFFF,0,0},// FVID2_STD_MUX_8CH_CIF, /**< Interlaced, 8Ch CIF, NTSC or PAL. */
    	{"Revs",0xFFFF,0xFFFF,0,0},// FVID2_STD_MUX_8CH_HALF_D1, /**< Interlaced, 8Ch Half-D1, NTSC or PAL. */

	{"Revs",0xFFFF,0xFFFF,0,0}, // FVID2_STD_AUTO_DETECT, /**< Auto-detect standard. Used in capture mode. */
	{"Revs",0xFFFF,0xFFFF,0,0}, // FVID2_STD_CUSTOM, /**< Custom standard used when connecting to external LCD etc...
			//The video timing is provided by the application.
			//Used in display mode. */

	{"Max",0xFFFF,0xFFFF,0,0} // FVID2_STD_MAX
};

static Vps_Adv7619_sav_eav_vbi  adv7619_SavEavVbi[DEVICE_STD_REACH_LAST] = {
	{"480I",		0x00,0x00,0x00,0x00},  			// 0-480ix60                                                
	{"576I",		0x00,0x00,0x00,0x00},  			// 1-576ix50     
	{"480I",		0x00,0x00,0x00,0x00},  			// 2-480ix60                                                
	{"576I",		0x00,0x00,0x00,0x00},  			// 3-576ix50     
	{"Revs",		0x00,0x00,0x00,0x00},				// FVID2_STD_CIF, /**< Interlaced, 360x120 per field NTSC, 360x144 per field PAL. */                                       
	{"Revs",		0x00,0x00,0x00,0x00},  			// FVID2_STD_HALF_D1, /**< Interlaced, 360x240 per field NTSC, 360x288 per field PAL. */                                               
	{"Revs",		0x00,0x00,0x00,0x00},  			// FVID2_STD_D1, /**< Interlaced, 720x240 per field NTSC, 720x288 per field PAL. */                                               
	{"480P",		0x00,0x00,0x00,0x00},  			// 7-480px60     
	{"576P",		0x00,0x00,0x00,0x00},				// 8-576px50   
	{"720P60",	0x00,0x00,0x00,0x00},				// 9-1280x720x60
	{"720P50",	0x00,0x00,0x00,0x00},  			// 10-1280x720x50           
	{"1080I60",	0x00,0x00,0x00,0x00},				// 11-1920x1080x60i              	
	{"1080I50",	0x00,0x00,0x00,0x00},				// 12-1920x1080x50i  
	{"1080P60",	0x00,0x00,0x00,0x00},				// 13-1920x1080x60p      
	{"1080P50",	0x00,0x00,0x00,0x00},				// 14-1920x1080x50p      
	{"1080P25",	0x00,0x00,0x00,0x00},				// 15-1920x1080x25p              
	{"1080P30",	0x00,0x00,0x00,0x00},				// 16-1920x1080x30p  
	
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
	{"1024x768@75",	100,1130,800,32},		 		// 27-1024x768x75              
	{"1024x768@85",	100,1130,808,40},				 // 28-1024x768x85              
	{"1280x768@60",	100,1386,798,30},				 // 29-1280x768x60
	{"1280x768@75",	0x00,0x00,0x00,0x00},			 // 30-1280x768x75 
	{"1280x768@85",	0x00,0x00,0x00,0x00},	 		// 31-1280x768x85 
	{"1280x1024@60",	360,1646,1066,42},	 		// 32-1280x1024x60
	{"1280x1024@75",	100,1386,1066,42},	 		// 33-1280x1024x75	
	{"1280x1024@85",	100,1386,1072,48},	 		// 34-1280x1024x85	
	{"1400x1050@60",	100,1406,1087,37},	 		// 35-1400x1050x60
	{"1400x1050@75",	100,1406,1096,46},	 		// 36-1400x1050x75
	{"1600x1200@60",	480,2086,1250,50},  			 // 37-1600x1200x60
	{"1920x1200@60",	536,2462,1242,38},  			 // 38-1920x1200x60
	{"2560x1440@60",	0x00,0x00,0x00,0x00},  		 // 39-2560x1440x60

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

static Vps_Adv7619_InMode ArgMode_7619[DEVICE_STD_REACH_LAST] = {
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
	{"1280x1024@60",0x05,0x21,0x5d,0x10,0x63,0x5d,0x10,0x05,0x21,0x5d,0x10},	 // 32-1280x1024x60
	{"1280x1024@75",0x06,0x21,0x5d,0x10,0x63,0x5e,0x10,0x05,0x21,0x5e,0x10},	 // 33-1280x1024x75	
	{"1280x1024@85",0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	 // 34-1280x1024x85	
	{"1400x1050@60",0x00,0x21,0x5d,0x10,0xE7,0x48,0x10,0x01,0xBA,0x44,0x1E},	 // 35-1400x1050x60
	{"1400x1050@75",0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	 // 36-1400x1050x75
	{"1600x1200@60",0x00,0x21,0x5d,0x10,0xe8,0x70,0xd0,0x01,0x7e,0x4e,0x20},  	 // 37-1600x1200x60
	{"1920x1200@60",0x00,0x21,0x5d,0x10,0xea,0x20,0xd0,0x01,0x7c,0x4d,0xa0},	 //38-1920x1200x60   
	{"2560x1440@60",0x00,0x21,0x5d,0x10,0xea,0x20,0xd0,0x01,0x7c,0x4d,0xa0},	 //39-2560x1440x60   

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

UInt8 Device_adv7619_Read8_Iomap(Device_Adv7619Obj * pObj, UInt8 RegAddr)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	status = OSA_i2cRead8 (&gDevice_adv7619CommonObj.i2cHandle, ADV7619_IO_MAP_ADDR, regAddr, regValue, 1);

	return regValue[0];
}

UInt8 Device_adv7619_Read8_Cecmap(Device_Adv7619Obj * pObj, UInt8 RegAddr)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	status = OSA_i2cRead8 (&gDevice_adv7619CommonObj.i2cHandle, ADV7619_CEC_MAP_ADDR, regAddr, regValue, 1);

	return regValue[0];
}

UInt8 Device_adv7619_Read8_Infoframemap(Device_Adv7619Obj * pObj, UInt8 RegAddr)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	status = OSA_i2cRead8 (&gDevice_adv7619CommonObj.i2cHandle, ADV7619_INFOFRAME_MAP_ADDR, regAddr, regValue, 1);

	return regValue[0];
}

UInt8 Device_adv7619_Read8_Dpllmap(Device_Adv7619Obj * pObj, UInt8 RegAddr)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	status = OSA_i2cRead8 (&gDevice_adv7619CommonObj.i2cHandle, ADV7619_DPLL_MAP_ADDR, regAddr, regValue, 1);

	return regValue[0];
}

UInt8 Device_adv7619_Read8_Ksvmap(Device_Adv7619Obj * pObj, UInt8 RegAddr)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	status = OSA_i2cRead8 (&gDevice_adv7619CommonObj.i2cHandle, ADV7619_KSV_MAP_ADDR, regAddr, regValue, 1);

	return regValue[0];
}

UInt8 Device_adv7619_Read8_Edidmap(Device_Adv7619Obj * pObj, UInt8 RegAddr)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	status = OSA_i2cRead8 (&gDevice_adv7619CommonObj.i2cHandle, ADV7619_EDID_MAP_ADDR, regAddr, regValue, 1);

	return regValue[0];
}

UInt8 Device_adv7619_Read8_Hdmimap(Device_Adv7619Obj * pObj, UInt8 RegAddr)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	status = OSA_i2cRead8 (&gDevice_adv7619CommonObj.i2cHandle, ADV7619_HDMI_MAP_ADDR, regAddr, regValue, 1);

	return regValue[0];
}

UInt8 Device_adv7619_Read8_Cpmap(Device_Adv7619Obj * pObj, UInt8 RegAddr)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	status = OSA_i2cRead8 (&gDevice_adv7619CommonObj.i2cHandle, ADV7619_CP_MAP_ADDR, regAddr, regValue, 1);

	return regValue[0];
}

Int32 Device_adv7619_Write8_Iomap(Device_Adv7619Obj * pObj, UInt8 RegAddr, UInt8 RegVal)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	regValue[0] = RegVal;
	status = OSA_i2cWrite8 (&gDevice_adv7619CommonObj.i2cHandle, ADV7619_IO_MAP_ADDR, regAddr, regValue, 1 );

	return status;
}

Int32 Device_adv7619_Write8_Cecmap(Device_Adv7619Obj * pObj, UInt8 RegAddr, UInt8 RegVal)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	regValue[0] = RegVal;
	status = OSA_i2cWrite8 (&gDevice_adv7619CommonObj.i2cHandle, ADV7619_CEC_MAP_ADDR, regAddr, regValue, 1 );

	return status;
}

Int32 Device_adv7619_Write8_Infoframemap(Device_Adv7619Obj * pObj, UInt8 RegAddr, UInt8 RegVal)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	regValue[0] = RegVal;
	status = OSA_i2cWrite8 (&gDevice_adv7619CommonObj.i2cHandle, ADV7619_INFOFRAME_MAP_ADDR, regAddr, regValue, 1 );

	return status;
}

Int32 Device_adv7619_Write8_Dpllmap(Device_Adv7619Obj * pObj, UInt8 RegAddr, UInt8 RegVal)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	regValue[0] = RegVal;
	status = OSA_i2cWrite8 (&gDevice_adv7619CommonObj.i2cHandle, ADV7619_DPLL_MAP_ADDR, regAddr, regValue, 1 );

	return status;
}

Int32 Device_adv7619_Write8_Ksvmap(Device_Adv7619Obj * pObj, UInt8 RegAddr, UInt8 RegVal)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	regValue[0] = RegVal;
	status = OSA_i2cWrite8 (&gDevice_adv7619CommonObj.i2cHandle, ADV7619_KSV_MAP_ADDR, regAddr, regValue, 1 );

	return status;
}

Int32 Device_adv7619_Write8_Edidmap(Device_Adv7619Obj * pObj, UInt8 RegAddr, UInt8 RegVal)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	regValue[0] = RegVal;
	status = OSA_i2cWrite8 (&gDevice_adv7619CommonObj.i2cHandle, ADV7619_EDID_MAP_ADDR, regAddr, regValue, 1 );

	return status;
}

Int32 Device_adv7619_Write8_Hdmimap(Device_Adv7619Obj * pObj, UInt8 RegAddr, UInt8 RegVal)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	regValue[0] = RegVal;
	status = OSA_i2cWrite8 (&gDevice_adv7619CommonObj.i2cHandle, ADV7619_HDMI_MAP_ADDR, regAddr, regValue, 1 );

	return status;
}

Int32 Device_adv7619_Write8_Cpmap(Device_Adv7619Obj * pObj, UInt8 RegAddr, UInt8 RegVal)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	regValue[0] = RegVal;
	status = OSA_i2cWrite8 (&gDevice_adv7619CommonObj.i2cHandle, ADV7619_CP_MAP_ADDR, regAddr, regValue, 1 );

	return status;
}

static Int32 Device_adv7619GetInfo_D(Device_Adv7619Obj *pObj, ADV7619_SyncInfo *SyncInfo)
{
	Int32 	retVal = 0;
	unsigned short	wTotPixLine;
	unsigned short    	wcurrTotField1_Hght;
	unsigned short    	wcurrTotField0_Hght;
	unsigned int    	dwTMDS_Freq;
	unsigned short    	wtemp1; 
	unsigned int    	dwtemp2;
	unsigned short    	wtemp3;
	unsigned short    	wtemp4;
	unsigned int    	dwtemp5;
	unsigned short    	pix_rep;
	unsigned short   	w_deep_color_mode;
	unsigned short    	wHsync_FrontPorch;
	unsigned short    	wHsync_BackPorch;
	unsigned short    	wHsync_PulseWidth;
	unsigned short    	wFiled0_Vsync_FrontPorch;
	unsigned short    	wFiled0_Vsync_BackPorch;
	unsigned short    	wFiled0_Vsync_PulseWidth;
	unsigned short    	wFiled1_Vsync_FrontPorch;
	unsigned short    	wFiled1_Vsync_BackPorch;
	unsigned short    	wFiled1_Vsync_PulseWidth;
	unsigned char  	temp;
	unsigned short 	width;
	unsigned short   	length;

	temp = Device_adv7619_Read8_Hdmimap(pObj, 0x04);	
	SyncInfo->TMDS_A=(temp&(0x01<<1))>>1;
	SyncInfo->TMDS_A=SyncInfo->TMDS_B;
	
	temp = Device_adv7619_Read8_Hdmimap(pObj, 0x0B);
	SyncInfo->Interlaced = (temp&0x20) > 0 ? 1 : 0;

	//pix per line
	temp = Device_adv7619_Read8_Hdmimap(pObj, 0x1E);
	wTotPixLine = (temp&0x3F)<<8;
	temp = Device_adv7619_Read8_Hdmimap(pObj, 0x1F);
	wTotPixLine += temp;

	//filed0 height
	temp = Device_adv7619_Read8_Hdmimap(pObj, 0x09);
	length = (temp&0x1F)<<8;
	temp = Device_adv7619_Read8_Hdmimap(pObj, 0x0A);
	length += temp;
	
	if((1150<length)&&(length<1250))
		SyncInfo->Interlaced =0;
	
	SyncInfo->HTotPix = wTotPixLine;                               

	//filed0 total height
	temp = Device_adv7619_Read8_Hdmimap(pObj, 0x26);
	wcurrTotField0_Hght = (temp&0x3F)<<8;
	temp = Device_adv7619_Read8_Hdmimap(pObj, 0x27);
	wcurrTotField0_Hght += temp;
	//filed1 total height
	temp = Device_adv7619_Read8_Hdmimap(pObj, 0x28);
	wcurrTotField1_Hght = (temp&0x3F)<<8;
	temp = Device_adv7619_Read8_Hdmimap(pObj, 0x29);
	wcurrTotField1_Hght += temp;
	
	if(SyncInfo->Interlaced) {
		SyncInfo->VTotPix =( (wcurrTotField1_Hght >> 1) 
							+ (wcurrTotField0_Hght >> 1))>>1;
	}
	else {
		SyncInfo->VTotPix = (wcurrTotField0_Hght) >> 1;
	}
	
	temp = Device_adv7619_Read8_Hdmimap(pObj, 0x0B);
	w_deep_color_mode = ((temp & 0xC0) >> 6);
	temp = Device_adv7619_Read8_Hdmimap(pObj, 0x05);
	pix_rep = (temp & 0x0F);
	pix_rep++;										/*pixel repetition factor*/			

	temp = Device_adv7619_Read8_Hdmimap(pObj, 0x51);
	wtemp1 = temp<<1;
	temp = Device_adv7619_Read8_Hdmimap(pObj, 0x52);
//	Vps_printf("wtemp1 = %d\n",wtemp1);
	wtemp1 = wtemp1+(temp&1);
//	Vps_printf("wtemp1 = %d\n",wtemp1);
//	wtemp1 = ((FXTAL/10)*wtemp1)/(27000000/10);
	dwTMDS_Freq = (unsigned int )wtemp1;

	wtemp1 = SyncInfo->HTotPix;	
	/*HSync Frequency in 100Hz*/
	if(wtemp1 != 0) 
	{
		dwtemp2 = (unsigned int ) (((dwTMDS_Freq * 1000000)/ (wtemp1))/10);
		SyncInfo->HFreq = (unsigned short ) dwtemp2;
		if(pix_rep != 0)
		{
			SyncInfo->HFreq = SyncInfo->HFreq/pix_rep ;
		}
	}
	wtemp3 = SyncInfo->HFreq;										/*VSync Frequency*/
	wtemp4 = SyncInfo->VTotPix;
	if(wtemp4 !=0) {
		dwtemp5 = (unsigned int ) (((wtemp3 * 10)/wtemp4)); //dwtemp5 = (unsigned int ) (((wtemp3 * 10)/

		SyncInfo->VFreq = (unsigned short ) dwtemp5;

		if(SyncInfo->Interlaced) {
			SyncInfo->VFreq = SyncInfo->VFreq ; //SyncInfo->VFreq = SyncInfo->VFreq << 1;
		}
	}

	OSA_printf("wTotPixLine = %d\n",wTotPixLine);
	OSA_printf("length = %d\n",length);
	OSA_printf("wcurrTotField0_Hght = %d\n",wcurrTotField0_Hght);
	OSA_printf("wcurrTotField1_Hght = %d\n",wcurrTotField1_Hght);
	OSA_printf("HFreq = %d\n",SyncInfo->HFreq);
	OSA_printf("VTotPix = %d\n",SyncInfo->VTotPix);
	OSA_printf("VFreq = %d\n",SyncInfo->VFreq);
	
	return retVal;
}

static Int32 Device_adv7619GetInterface(Device_Adv7619Obj *pObj, int *tmds, AD7619_INPUT *InterFace)
{
	Int32 	retVal = 0;
	return retVal;
}

static Int32 Device_adv7619GetModeIndex(int hpv, int vps, int *index)
{
	Int32 	retVal = 0;
	int i=-1;
	
	for(i = 0;i<DEVICE_STD_REACH_LAST;i++){
		if(((vps>adv7619_gLineNum[i].vps-3)&&(vps<adv7619_gLineNum[i].vps+3))&&((hpv>adv7619_gLineNum[i].hpv-4)&&(hpv<adv7619_gLineNum[i].hpv+4))){
			break;
		}

	}

	if(i == DEVICE_STD_REACH_LAST){
		for(i = 0;i<DEVICE_STD_REACH_LAST;i++){
			if(((vps>adv7619_gLineNum[i].vps-3)&&(vps<adv7619_gLineNum[i].vps+3))&&((hpv>adv7619_gLineNum[i].hpv-10)&&(hpv<adv7619_gLineNum[i].hpv+10))){
				break;
			}
	    	}
	}

	if(i==DEVICE_STD_REACH_LAST)
		*index = -1;
	else
		*index = i;

	return retVal;
}

static Int32 Device_adv7619InitComm(Device_Adv7619Obj *pObj)
{
	Int32 	retVal = 0;
	
	Device_adv7619_Write8_Cpmap(pObj, 0x6C, 0x00);

	Device_adv7619_Write8_Hdmimap(pObj, 0xC0, 0x03);
	Device_adv7619_Write8_Hdmimap(pObj, 0x03, 0x98);
	Device_adv7619_Write8_Hdmimap(pObj, 0x10, 0xA5);
	Device_adv7619_Write8_Hdmimap(pObj, 0x45, 0x04);

	Device_adv7619_Write8_Hdmimap(pObj, 0x3E, 0x69);
	Device_adv7619_Write8_Hdmimap(pObj, 0x3F, 0x46);
	Device_adv7619_Write8_Hdmimap(pObj, 0x4E, 0xFE);
	Device_adv7619_Write8_Hdmimap(pObj, 0x4F, 0x08);
	Device_adv7619_Write8_Hdmimap(pObj, 0x50, 0xC0);
	Device_adv7619_Write8_Hdmimap(pObj, 0x57, 0xA3);
	Device_adv7619_Write8_Hdmimap(pObj, 0x58, 0x07);
	Device_adv7619_Write8_Hdmimap(pObj, 0x93, 0x03);
	Device_adv7619_Write8_Hdmimap(pObj, 0x5A, 0x80);

	Device_adv7619_Write8_Dpllmap(pObj, 0xc3, 0x00);
	Device_adv7619_Write8_Dpllmap(pObj, 0xcf, 0x00);
	Device_adv7619_Write8_Iomap(pObj, 0xDD, 0x00);
	
	return retVal;
}

static Int32 Device_adv7619DigitalInterFaceInit(Device_Adv7619Obj *pObj)
{
	Int32 	retVal = 0;
	return retVal;
}

static Int32 Device_adv7619DigitalInterFaceSetRes(Device_Adv7619Obj *pObj, int inMode)
{
	Int32 	retVal = 0;
	return retVal;
}

static Int32 Device_adv7619AnalogInterFaceInit(Device_Adv7619Obj *pObj)
{
	Int32 	retVal = 0;

	return retVal;
}

static Int32 Device_adv7619AnalogInterFaceSetRes(Device_Adv7619Obj *pObj, int inMode)
{
	Int32 	retVal = 0;
	return retVal;
}

static Int32 Device_adv7619YPbPrInterFaceSetRes(Device_Adv7619Obj *pObj, int inMode)
{
	Int32 	retVal = 0;
	return retVal;
}

static Int32 Device_adv7619Config(Device_Adv7619Obj *pObj, int inMode, int tmds, AD7619_INPUT InterFace)
{
	Int32 	retVal = 0;

//	Device_adv7619Reset(pObj);
	switch(inMode){
		case VSYS_STD_720P_60:
			OSA_printf("Config adv7619 to 720P!\n");
			Device_adv7619_Write8_Hdmimap(pObj, 0xc0, 0x03);
			Device_adv7619_Write8_Iomap(pObj, 0x01, 0x06);
			Device_adv7619_Write8_Iomap(pObj, 0x02, 0xf5);
			Device_adv7619_Write8_Iomap(pObj, 0x03, 0x80);
			Device_adv7619_Write8_Iomap(pObj, 0x05, 0x2C);
			Device_adv7619_Write8_Iomap(pObj, 0x06, 0xa6);
			Device_adv7619_Write8_Iomap(pObj, 0x0c, 0x42);
			Device_adv7619_Write8_Iomap(pObj, 0x15, 0x80);
			Device_adv7619_Write8_Iomap(pObj, 0x19, 0x83);
			Device_adv7619_Write8_Iomap(pObj, 0x33, 0x40);
			Device_adv7619_Write8_Iomap(pObj, 0xDD, 0x00);
			
			Device_adv7619_Write8_Cpmap(pObj, 0xba, 0x01);
			Device_adv7619_Write8_Ksvmap(pObj, 0x40, 0x81);
			Device_adv7619_Write8_Dpllmap(pObj, 0xb5, 0x01);
			Device_adv7619_Write8_Dpllmap(pObj, 0xc3, 0x00);
			Device_adv7619_Write8_Dpllmap(pObj, 0xcf, 0x00);
			
			Device_adv7619_Write8_Hdmimap(pObj, 0x00, 0x08);
			Device_adv7619_Write8_Hdmimap(pObj, 0x02, 0x03);
			Device_adv7619_Write8_Hdmimap(pObj, 0x3e, 0x69);
			Device_adv7619_Write8_Hdmimap(pObj, 0x3f, 0x46);
			Device_adv7619_Write8_Hdmimap(pObj, 0x4e, 0x7e);
			Device_adv7619_Write8_Hdmimap(pObj, 0x4f, 0x42);
			Device_adv7619_Write8_Hdmimap(pObj, 0x57, 0xa3);
			Device_adv7619_Write8_Hdmimap(pObj, 0x58, 0x07);
			Device_adv7619_Write8_Hdmimap(pObj, 0x83, 0xfc);
			Device_adv7619_Write8_Hdmimap(pObj, 0x84, 0x03);
			Device_adv7619_Write8_Hdmimap(pObj, 0x85, 0x10);
			Device_adv7619_Write8_Hdmimap(pObj, 0x86, 0x9b);
			Device_adv7619_Write8_Hdmimap(pObj, 0x89, 0x03);
			Device_adv7619_Write8_Hdmimap(pObj, 0x9B, 0x03);
			break;		
		case VSYS_STD_1080P_60:
			OSA_printf("Config adv7619 to 1080P!\n");
			Device_adv7619_Write8_Hdmimap(pObj, 0xc0, 0x03);
			Device_adv7619_Write8_Iomap(pObj, 0x01, 0x06);
			Device_adv7619_Write8_Iomap(pObj, 0x02, 0xf5);
			Device_adv7619_Write8_Iomap(pObj, 0x03, 0x80);
			Device_adv7619_Write8_Iomap(pObj, 0x05, 0x2C);
			Device_adv7619_Write8_Iomap(pObj, 0x06, 0xa6);
			Device_adv7619_Write8_Iomap(pObj, 0x0c, 0x42);
			Device_adv7619_Write8_Iomap(pObj, 0x15, 0x80);
			Device_adv7619_Write8_Iomap(pObj, 0x19, 0x83);
			Device_adv7619_Write8_Iomap(pObj, 0x33, 0x40);
			Device_adv7619_Write8_Iomap(pObj, 0xDD, 0x00);
			
			Device_adv7619_Write8_Cpmap(pObj, 0xba, 0x01);
			Device_adv7619_Write8_Ksvmap(pObj, 0x40, 0x81);
			Device_adv7619_Write8_Dpllmap(pObj, 0xb5, 0x01);
			Device_adv7619_Write8_Dpllmap(pObj, 0xc3, 0x00);
			Device_adv7619_Write8_Dpllmap(pObj, 0xcf, 0x00);
			
			Device_adv7619_Write8_Hdmimap(pObj, 0x00, 0x08);
			Device_adv7619_Write8_Hdmimap(pObj, 0x02, 0x03);
			Device_adv7619_Write8_Hdmimap(pObj, 0x3e, 0x69);
			Device_adv7619_Write8_Hdmimap(pObj, 0x3f, 0x46);
			Device_adv7619_Write8_Hdmimap(pObj, 0x4e, 0x7e);
			Device_adv7619_Write8_Hdmimap(pObj, 0x4f, 0x42);
			Device_adv7619_Write8_Hdmimap(pObj, 0x57, 0xa3);
			Device_adv7619_Write8_Hdmimap(pObj, 0x58, 0x07);
			Device_adv7619_Write8_Hdmimap(pObj, 0x83, 0xfc);
			Device_adv7619_Write8_Hdmimap(pObj, 0x84, 0x03);
			Device_adv7619_Write8_Hdmimap(pObj, 0x85, 0x10);
			Device_adv7619_Write8_Hdmimap(pObj, 0x86, 0x9b);
			Device_adv7619_Write8_Hdmimap(pObj, 0x89, 0x03);
			Device_adv7619_Write8_Hdmimap(pObj, 0x9B, 0x03);
			break;
		case VSYS_STD_2560x1440_60:
			OSA_printf("Config ADV7619 to VSYS_STD_2560x1440x60!\n");
			Device_adv7619_Write8_Hdmimap(pObj, 0xc0, 0x03);
			Device_adv7619_Write8_Iomap(pObj, 0x01, 0x06);
			Device_adv7619_Write8_Iomap(pObj, 0x02, 0xf5);
			Device_adv7619_Write8_Iomap(pObj, 0x03, 0x94);
			Device_adv7619_Write8_Iomap(pObj, 0x05, 0x2C);
			Device_adv7619_Write8_Iomap(pObj, 0x06, 0xa0);
			Device_adv7619_Write8_Iomap(pObj, 0x0c, 0x42);
			Device_adv7619_Write8_Iomap(pObj, 0x15, 0x80);
			Device_adv7619_Write8_Iomap(pObj, 0x19, 0x83);
			Device_adv7619_Write8_Iomap(pObj, 0x33, 0x40);
			
			Device_adv7619_Write8_Iomap(pObj, 0xDD, 0xA0);
			Device_adv7619_Write8_Dpllmap(pObj, 0xb5, 0x01);
			Device_adv7619_Write8_Dpllmap(pObj, 0xc3, 0x80);
			Device_adv7619_Write8_Dpllmap(pObj, 0xcf, 0x03);
			
			Device_adv7619_Write8_Hdmimap(pObj, 0x3e, 0x69);
			Device_adv7619_Write8_Hdmimap(pObj, 0x3f, 0x46);
			Device_adv7619_Write8_Hdmimap(pObj, 0x4e, 0x7e);
			Device_adv7619_Write8_Hdmimap(pObj, 0x4f, 0x42);
			Device_adv7619_Write8_Hdmimap(pObj, 0x02, 0x03);
			Device_adv7619_Write8_Hdmimap(pObj, 0x57, 0xa3);
			Device_adv7619_Write8_Hdmimap(pObj, 0x58, 0x07);
			Device_adv7619_Write8_Hdmimap(pObj, 0x83, 0xfc);
			Device_adv7619_Write8_Hdmimap(pObj, 0x84, 0x03);
			Device_adv7619_Write8_Hdmimap(pObj, 0x85, 0x10);
			Device_adv7619_Write8_Hdmimap(pObj, 0x86, 0x9b);
			Device_adv7619_Write8_Hdmimap(pObj, 0x89, 0x03);
			Device_adv7619_Write8_Hdmimap(pObj, 0x9B, 0x03);
			break;	
	}
	return retVal;
}

static Int32 Device_adv7619GetResolution(Device_Adv7619Obj *pObj)
{
	Int32           retVal = 0;
	int 		inMode;
	ADV7619_SyncInfo adv7619Infor;

	Device_adv7619GetInfo_D(pObj, &adv7619Infor);
	gAD7619_Hpv = adv7619Infor.VTotPix;
	gAD7619_Vps = adv7619Infor.VFreq;
	gAD7619_TMDS = 1;
	gAD7619_InterFace = 1;
	
	Device_adv7619GetModeIndex(gAD7619_Hpv, gAD7619_Vps, &inMode);
	if((gAD7619_Mode != inMode)&&(inMode != -1)){
		gAD7619_Mode = inMode;
		Device_adv7619Config(pObj, gAD7619_Mode, gAD7619_TMDS, gAD7619_InterFace);
	}

	if((gAD7619_Mode != inMode)&&(inMode == -1)){
		gAD7619_Mode = inMode;
		Device_adv7619Config(pObj, DEVICE_STD_REACH_LAST, 1,1);
	}
	
	if(gAD7619_Mode != -1){
#ifdef DEVICE_ADV7619_DEBUG		
		OSA_printf("###########################################\n");
		OSA_printf("Get AD7619 Resolution!\n");
		OSA_printf("gAD7619_std = %s \n",ArgMode_7619[gAD7619_Mode].name);
		OSA_printf("gAD7619_hpv = %d gAD7619_vps = %d \n",gAD7619_Hpv,gAD7619_Vps);
		OSA_printf("gAD7619_Mode = %d gAD7619_TMDS = %d gAD7619_InterFace = %d\n",gAD7619_Mode,gAD7619_TMDS,gAD7619_InterFace);
		OSA_printf("###########################################\n");
#endif
	}else{
#ifdef 	DEVICE_ADV7619_DEBUG
		OSA_printf("###########################################\n");
		OSA_printf("Get AD7619 Resolution!\n");
		OSA_printf("gAD7619_std = unknow \n");
		OSA_printf("gAD7619_hpv = %d gAD7619_vps = %d \n",gAD7619_Hpv,gAD7619_Vps);
		OSA_printf("gAD7619_Mode = %d gAD7619_TMDS = %d gAD7619_InterFace = %d\n",gAD7619_Mode,gAD7619_TMDS,gAD7619_InterFace);
		OSA_printf("###########################################\n");
#endif		
	}	

	return (retVal);
}
/* Set ADV7619 mode based on

  - mux mode - line or pixel or no-mux
  - number of channels
  - resolution
  - 8/16-bit data width
  - NTSC/PAL standard
  - cascade mode of operation
*/
Int32 Device_adv7619SetVideoMode ( Device_Adv7619Obj * pObj,
                                Device_VideoDecoderVideoModeParams * pPrm )
{
    Int32 status = 0;

    return status;
}

/* reset ADV7619 OFM logic  */
Int32 Device_adv7619Reset ( Device_Adv7619Obj * pObj )
{
	Int32           retVal = 0;
	UInt8           regVal;

	if(0){
		Vps_printf("Before write ADV7619 IO Map 0xFF!\n");
		retVal = Device_adv7619_Write8_Iomap(pObj,0xFF,0x80);
		Vps_printf("After write ADV7619 IO Map 0xFF!\n");
		while(1){
			Task_sleep(1000);
			Vps_printf("Before Read ADV7619 IO Map 0xFF!\n");
			regVal = Device_adv7619_Read8_Iomap(pObj,0xFF);
			Vps_printf("After Read ADV7619 IO Map 0xFF!\n");
			if(!(regVal & 0x80)){
				Vps_printf("ADV7619 Reset OK!\n");
				break;
			}else{
				Vps_printf("waiting ADV7619 Reset.......Reg 0xFF = 0x%x\n",regVal);
			}
		}
	}
	if (0 == retVal)
	{
		retVal = Device_adv7619_Write8_Iomap(pObj,0xF4,(ADV7619_CEC_MAP_ADDR<<1));
		retVal = Device_adv7619_Write8_Iomap(pObj,0xF5,(ADV7619_INFOFRAME_MAP_ADDR<<1));
		retVal = Device_adv7619_Write8_Iomap(pObj,0xF8,(ADV7619_DPLL_MAP_ADDR<<1));
		retVal = Device_adv7619_Write8_Iomap(pObj,0xF9,(ADV7619_KSV_MAP_ADDR<<1));
		retVal = Device_adv7619_Write8_Iomap(pObj,0xFA,(ADV7619_EDID_MAP_ADDR<<1));
		retVal = Device_adv7619_Write8_Iomap(pObj,0xFB,(ADV7619_HDMI_MAP_ADDR<<1));
		retVal = Device_adv7619_Write8_Iomap(pObj,0xFD,(ADV7619_CP_MAP_ADDR<<1));
		sleep(1);
	}
	Device_adv7619InitComm(pObj);
	Device_adv7619Config(pObj, VSYS_STD_1080P_60, 1,1);
//	sleep(1);
//	Device_adv7619Config(pObj, VSYS_STD_2560x1440x60, 1,1);
//	sleep(1);
	
	return (retVal);
}

/*
  Enable ADV7619 output port
*/
Int32 Device_adv7619OutputEnable ( Device_Adv7619Obj * pObj, UInt32 enable )
{
    Int32 status = 0;

    return status;
}

/* start/enable output  */
Int32 Device_adv7619Start ( Device_Adv7619Obj * pObj )
{
    Int32 status = 0;

    status = Device_adv7619OutputEnable ( pObj, TRUE );

    return status;
}

/* stop/disable output  */
Int32 Device_adv7619Stop ( Device_Adv7619Obj * pObj )
{
    Int32 status = 0;

    status = Device_adv7619OutputEnable ( pObj, FALSE );

    return status;
}

/*
  Get ADV7619 chip ID, revision ID and firmware patch ID
*/
Int32 Device_adv7619GetChipId ( Device_Adv7619Obj * pObj,
                             Device_VideoDecoderChipIdParams * pPrm,
                             Device_VideoDecoderChipIdStatus * pStatus )
{
    Int32 status = 0;

    return status;
}

/*
  Get ADV7619 detect vide standard status

  Can be called for each channel
*/
Int32 Device_adv7619GetVideoStatus ( Device_Adv7619Obj * pObj,
                                     VCAP_VIDEO_SOURCE_STATUS_PARAMS_S * pPrm,
                                     VCAP_VIDEO_SOURCE_CH_STATUS_S     * pStatus )
{
	Int32 status = 0;
	
	Device_adv7619GetResolution(pObj);
	if(gAD7619_Mode != -1){
		pStatus->isVideoDetect = TRUE;
		pStatus->frameWidth = adv7619_gLineNum[gAD7619_Mode].width;
		pStatus->frameHeight = adv7619_gLineNum[gAD7619_Mode].hight;
		pStatus->isInterlaced = FALSE;
	}else{
		pStatus->isVideoDetect = FALSE;
	}
	
	return status;
}

/*
  Set video color related parameters
*/
Int32 Device_adv7619SetVideoColor ( Device_Adv7619Obj * pObj,
                                 Device_VideoDecoderColorParams * pPrm )
{
    Int32 status = 0;
    return status;
}

/*
  Set video noise filter related parameters
*/
Int32 Device_adv7619SetVideoNf ( Device_Adv7619Obj * pObj,
                              Device_Adv7619VideoNfParams * pPrm )
{
    Int32 status = 0;
    return status;
}

/* write to I2C registers */
Int32 Device_adv7619RegWrite ( Device_Adv7619Obj * pObj,
                            Device_VideoDecoderRegRdWrParams * pPrm )
{
    Int32 status = 0;
    Device_VideoDecoderCreateParams *pCreateArgs;

    if ( pPrm == NULL )
        return -1;

    pCreateArgs = &pObj->createArgs;

    if ( pPrm->deviceNum > pCreateArgs->numDevicesAtPort )
        return -1;

    status = OSA_i2cWrite8 (&gDevice_adv7619CommonObj.i2cHandle, pCreateArgs->deviceI2cAddr[0], pPrm->regAddr, pPrm->regValue8, pPrm->numRegs );

    return status;
}

/* read from I2C registers */
Int32 Device_adv7619RegRead ( Device_Adv7619Obj * pObj,
                           Device_VideoDecoderRegRdWrParams * pPrm )
{
    Int32 status = 0;
    Device_VideoDecoderCreateParams *pCreateArgs;

    if ( pPrm == NULL )
        return -1;

    pCreateArgs = &pObj->createArgs;

    if ( pPrm->deviceNum > pCreateArgs->numDevicesAtPort )
        return -1;

    status = OSA_i2cRead8 (&gDevice_adv7619CommonObj.i2cHandle, pCreateArgs->deviceI2cAddr[0], pPrm->regAddr, pPrm->regValue8, pPrm->numRegs);

    return status;
}

