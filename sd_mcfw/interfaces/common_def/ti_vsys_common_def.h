/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
    \ingroup MCFW_API
    \defgroup MCFW_SYS_API McFW System Level (VSYS) API

    @{
*/

/**
    \file ti_vsys_common_def.h
    \brief McFW System Level (VSYS) API - Common definitions and data structures
*/

#ifndef __TI_VSYS_COMMON_DEF_H__
#define __TI_VSYS_COMMON_DEF_H__

#include "../ti_media_std.h"



/* =============================================================================
 * Enums
 * =============================================================================
 */

typedef enum
{
    VSYS_STD_NTSC = 0u,
    /**< 720x480 30FPS interlaced NTSC standard. */
    VSYS_STD_PAL,
    /**< 720x576 30FPS interlaced PAL standard. */

    VSYS_STD_480I,
    /**< 720x480 30FPS interlaced SD standard. */
    VSYS_STD_576I,
    /**< 720x576 30FPS interlaced SD standard. */

    VSYS_STD_CIF,
    /**< Interlaced, 360x120 per field NTSC, 360x144 per field PAL. */
    VSYS_STD_HALF_D1,
    /**< Interlaced, 360x240 per field NTSC, 360x288 per field PAL. */
    VSYS_STD_D1,
    /**< Interlaced, 720x240 per field NTSC, 720x288 per field PAL. */

    VSYS_STD_480P,
    /**< 720x480 60FPS progressive ED standard. */
    VSYS_STD_576P,
    /**< 720x576 60FPS progressive ED standard. */

    VSYS_STD_720P_60,
    /**< 1280x720 60FPS progressive HD standard. */
    VSYS_STD_720P_50,			// ------10
    /**< 1280x720 50FPS progressive HD standard. */

    VSYS_STD_1080I_60,
    /**< 1920x1080 30FPS interlaced HD standard. */
    VSYS_STD_1080I_50,
    /**< 1920x1080 50FPS interlaced HD standard. */

    VSYS_STD_1080P_60,
    /**< 1920x1080 60FPS progressive HD standard. */
    VSYS_STD_1080P_50,
    /**< 1920x1080 50FPS progressive HD standard. */

    VSYS_STD_1080P_24,
    /**< 1920x1080 24FPS progressive HD standard. */
    VSYS_STD_1080P_30,
    /**< 1920x1080 30FPS progressive HD standard. */
	
#ifdef HDVPSS_VER_01_00_01_36
    VSYS_STD_VGA_60,
    /**< 640x480 60FPS VESA standard. */
#else
	VSYS_STD_VGA_60 = 0x100,
#endif
    VSYS_STD_VGA_72,
    /**< 640x480 72FPS VESA standard. */
    VSYS_STD_VGA_75,
    /**< 640x480 75FPS VESA standard. */
    VSYS_STD_VGA_85,			
    /**< 640x480 85FPS VESA standard. */

#ifndef HDVPSS_VER_01_00_01_36
	VSYS_STD_WVGA_60,
	/**< 800x480 60PFS WVGA standard. */
#endif

    VSYS_STD_SVGA_60,
    /**< 800x600 60FPS VESA standard. */
    VSYS_STD_SVGA_72,
    /**< 800x600 72FPS VESA standard. */
    VSYS_STD_SVGA_75,
    /**< 800x600 75FPS VESA standard. */
    VSYS_STD_SVGA_85,
    /**< 800x600 85FPS VESA standard. */

#ifndef HDVPSS_VER_01_00_01_36
	VSYS_STD_WSVGA_70,
	/**< 1024x600 70FPS standard. */
#endif

    VSYS_STD_XGA_60,
    /**< 1024x768 60FPS VESA standard. */
    VSYS_STD_XGA_70,
    /**< 1024x768 72FPS VESA standard. */
    VSYS_STD_XGA_75,
    /**< 1024x768 75FPS VESA standard. */
    VSYS_STD_XGA_85,
    /**< 1024x768 85FPS VESA standard. */

    VSYS_STD_WXGA_60,
    /**< 1280x768 60FPS VESA standard. */
    VSYS_STD_WXGA_75,			
    /**< 1280x768 75FPS VESA standard. */
    VSYS_STD_WXGA_85,
    /**< 1280x768 85FPS VESA standard. */

    VSYS_STD_SXGA_60,
    /**< 1280x1024 60FPS VESA standard. */
    VSYS_STD_SXGA_75,
    /**< 1280x1024 75FPS VESA standard. */
    VSYS_STD_SXGA_85,
    /**< 1280x1024 85FPS VESA standard. */

#ifndef HDVPSS_VER_01_00_01_36
	VSYS_STD_WSXGAP_60,
	/**< 1680x1050 60 PFS VESA>*/
#endif

    VSYS_STD_SXGAP_60,
    /**< 1400x1050 60FPS VESA standard. */
    VSYS_STD_SXGAP_75,
    /**< 1400x1050 75FPS VESA standard. */

    VSYS_STD_UXGA_60,
    /**< 1600x1200 60FPS VESA standard. */

    VSYS_STD_1280x800_60,
    /**< 1280x800 60 PFS VESA>*/

#ifndef HDVPSS_VER_01_00_01_36
	VSYS_STD_1368_768_60,
	/**< 1368x768 60 PFS VESA standard. */
	VSYS_STD_1366_768_60,
	/**< 1366x768 60 PFS VESA standard. */
	VSYS_STD_1360_768_60,
	/**< 1360x768 60 PFS VESA standard. */
#endif

 //#ifndef HDVPSS_VER_01_00_01_36
	VSYS_STD_1440_900_60,
	/**< 1440x900 60 PFS VESA>*/
//#endif

   VSYS_STD_1920x1200_60,
    /**< 1900x1200 60FPS VESA standard. */
	
    VSYS_STD_2560x1440_60,
    /**< 2560x1440 60FPS VESA standard. */

	VSYS_STD_3840x1200_60,
    /**< 3840x1200 60FPS VESA standard. */

	VSYS_STD_3840x2400_60,
    /**< 3840x2400 60FPS VESA standard. */

#ifdef HDVPSS_VER_01_00_01_36
	VSYS_STD_MUX_2CH_D1,
#else
	VSYS_STD_MUX_2CH_D1 = 0x200,
#endif
	
    /**< Interlaced, 2Ch D1, NTSC or PAL. */
    VSYS_STD_MUX_2CH_HALF_D1,
    /**< Interlaced, 2ch half D1, NTSC or PAL. */
    VSYS_STD_MUX_2CH_CIF,
    /**< Interlaced, 2ch CIF, NTSC or PAL. */
    VSYS_STD_MUX_4CH_D1,
    /**< Interlaced, 4Ch D1, NTSC or PAL. */
    VSYS_STD_MUX_4CH_CIF,
    /**< Interlaced, 4Ch CIF, NTSC or PAL. */
    VSYS_STD_MUX_4CH_HALF_D1,
    /**< Interlaced, 4Ch Half-D1, NTSC or PAL. */
    VSYS_STD_MUX_8CH_CIF,
    /**< Interlaced, 8Ch CIF, NTSC or PAL. */
    VSYS_STD_MUX_8CH_HALF_D1,
    /**< Interlaced, 8Ch Half-D1, NTSC or PAL. */

#ifdef HDVPSS_VER_01_00_01_36
	VSYS_STD_AUTO_DETECT,
#else
	VSYS_STD_AUTO_DETECT = 0x300,
#endif
    /**< Auto-detect standard. Used in capture mode. */

    VSYS_STD_REACH_LAST,

	VSYS_STD_CUSTOM,
    /**< Custom standard used when connecting to external LCD etc...
         The video timing is provided by the application.
         Used in display mode. */
 
    VSYS_STD_MAX
    /**< Should be the last value of this enumeration.
         Will be used by driver for validating the input parameters. */
} VSYS_VIDEO_STANDARD_E;


/* =============================================================================
 * Defines
 * =============================================================================
 */

/* =============================================================================
 *  Structures
 * =============================================================================
 */

/**
    \brief Window Data structure

    Used to specific resolution is VCAP, VDIS and other sub-system's
*/
typedef struct WINDOW_S
{
    UInt32 start_X;
    /**< Horizontal offset in pixels, from which picture needs to be positioned. */

    UInt32 start_Y;
    /**< Vertical offset from which picture needs to be positioned. */

    UInt32 width;
    /**< Width of frame in pixels. */

    UInt32 height;
    /**< Height of frame in lines. */

} WINDOW_S;


/**
    This event is received by McFW when there is a change in the video source.
    A change means either a video source is
    - connected
    - or disconnected
    - or changed from one standard to another standard

    On receiving this event user call Vcap APIs to get details about the
    channel on which this event has happended
*/
#define VSYS_EVENT_VIDEO_DETECT     (0x0000)


/**
    This event is received by McFW when camera tamper is detected by SCD link

    On receiving this event user call Scd APIs to get details about the
    channel on which this event has happended
*/
#define VSYS_EVENT_TAMPER_DETECT     (0x0001)

/**
    This event is received by McFW when motion is detected by SCD link

    On receiving this event user call Scd APIs to get details about the
    channel on which this event has happended
*/
#define VSYS_EVENT_MOTION_DETECT     (0x0002)

/**
    This event is received by McFW when decoder sees an error in decoding

    On receiving this event user can take appropriate action in the decoder
    channel on which this event has happended. The message packet received with
    this event is decoder error message and can be extracted by VDEC_CH_ERROR_MSG 
    structre defined in <mcfw/interfaces/common_def/ti_vdec_common_def.h>.
*/
#define VSYS_EVENT_DECODER_ERROR     (0x0003)



#define VSYS_EVENT_TRACK_STATUS        (0x0004)

//#define VSYS_EVENT_STUTRACK_STATUS     (0x0005)  5一直发不过去，why?

#define VSYS_EVENT_STUTRACK_STATUS     (0x0006)

#define VSYS_EVENT_STUSIDETRACK_STATUS     (0x0007)

#define VSYS_EVENT_STUSIDETRACK_SAVE_VIEW     (0x0008)

#define VSYS_EVENT_TRACK_SAVE_VIEW     (0x0009)

#define VSYS_EVENT_STUDOUBLESIDETRACK_STATUS     (0x000a)

#define VSYS_EVENT_STUDOUBLESIDETRACK_SAVE_VIEW     (0x000b)

#define VSYS_EVENT_CEILTRACK_STATUS     (0x000c)

#define VSYS_EVENT_CEILTRACK_SAVE_VIEW     (0x000d)

#define VSYS_EVENT_BLACKBOARD_TRACK_STATUS     (0x000e)
#endif


/* @} */
