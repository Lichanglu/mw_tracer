/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
    \ingroup MCFW_API
    \defgroup MCFW_VDIS_API McFW Video Display (VDIS) API

    @{
*/

/**
    \file ti_vdis_common_def.h
    \brief McFW Video Display (VDIS) API
*/

#ifndef __TI_VDIS_COMMON_DEF_H__
#define __TI_VDIS_COMMON_DEF_H__

#include "ti_vsys_common_def.h"


/* =============================================================================
 * Defines
 * =============================================================================
 */
#define VDIS_DEV_MAX         (0x04)

/** Maximum Windows in a display mosaic */

#ifdef TI_816X_BUILD
#define VDIS_MOSAIC_WIN_MAX     (36)
#endif

#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
#define VDIS_MOSAIC_WIN_MAX     (20)
#endif


/** Maximum Video Display channels */
#define VDIS_CHN_MAX            (32)

/** Invalid channel ID for a mosaic. Used to disable a window on the display */
#define VDIS_CHN_INVALID        (0xFF)

/** \brief Bitmask for HDMI VENC */
#define VDIS_VENC_HDMI          (0x1u)
/** \brief Bitmask for HDCOMP VENC */
#define VDIS_VENC_HDCOMP        (0x2u)
/** \brief Bitmask for DVO2 VENC */
#define VDIS_VENC_DVO2          (0x4u)
/** \brief Bitmask for SD VENC. */
#define VDIS_VENC_SD            (0x8u)

/** \brief Default display sequence id */
#define VDIS_DISPLAY_SEQID_DEFAULT  (SYSTEM_DISPLAY_SEQID_DEFAULT)



/* =============================================================================
 *  Structures
 * =============================================================================
 */


/**
    \brief Display Device ID

    Association of display device ID to HDMI or HDDAC is done via VDIS_PARAMS_S
*/
typedef enum
{
    VDIS_DEV_HDMI   = 0,
    /**< Display 0 */

    VDIS_DEV_HDCOMP = 1,
    /**< Display 1 */

    VDIS_DEV_DVO2   = 2,
    /**< Display 2 */

    VDIS_DEV_SD     = 3,
    /**< Display 3 */

} VDIS_DEV;


/**
    \brief Display Type
*/
typedef enum
{
    VDIS_TYPE_SDDAC_COMPOSITE_CVBS  = 0,
    /**< Analog Composite - SDTV */

    VDIS_TYPE_DVO2_BT1120_YUV422 = 1,
    /**< Digital 16-bit embedded sync mode via DVO2 - HDTV */

    VDIS_TYPE_DVO1_BT1120_YUV422 = 2,
    /**< Digital 16-bit embedded sync mode via DVO1 - HDTV */

    VDIS_TYPE_HDDAC_VGA_RGB = 3,
    /**< Analog Component YPbPr - HDTV */

    VDIS_TYPE_HDDAC_COMPONENT_YPBPR = 4,
    /**< Analog Component RGB VESA Standard - HDTV */

    VDIS_TYPE_HDMI_RGB    = 5,
    /**< Digital HDMI Standard (On-Chip HDMI) - HDTV */

} VDIS_TYPE_E;

/**
    \brief TrickPlay Rate
*/
typedef enum
{
    VDIS_AVSYNC_1X = 1000,
    /**< Normal Speed Playback */

    VDIS_AVSYNC_2X = 2000,
    /**< 2X Speed Playback */

    VDIS_AVSYNC_4X = 4000,
    /**< 4X Speed Playback */

    VDIS_AVSYNC_8X = 8000,
    /**< 8X Speed Playback */

    VDIS_AVSYNC_16X = 16000,
    /**< 16X Speed Playback */

    VDIS_AVSYNC_32X = 32000,
    /**< 32X Speed Playback */

    VDIS_AVSYNC_HALFX = 500,
    /**< 1/2X Speed Playback */

    VDIS_AVSYNC_QUARTERX = 250,
    /**< 1/4X Speed Playback */

    VDIS_AVSYNC_IFRAMEONLY = 0,
    /**< 1/4X Speed Playback */


    VDIS_AVSYNC_MAX   = 100000
    /**< Maximum Playback Rate */

}VDIS_AVSYNC;

/**
    \brief Color space conversion mode
*/
typedef enum
{
    VDIS_CSC_MODE_SDTV_VIDEO_R2Y = 0,
    /**< Select coefficient for SDTV Video */

    VDIS_CSC_MODE_SDTV_VIDEO_Y2R,
    /**< Select coefficient for SDTV Video */

    VDIS_CSC_MODE_SDTV_GRAPHICS_R2Y,
    /**< Select coefficient for SDTV Graphics */

    VDIS_CSC_MODE_SDTV_GRAPHICS_Y2R,
    /**< Select coefficient for SDTV Graphics */

    VDIS_CSC_MODE_HDTV_VIDEO_R2Y,
    /**< Select coefficient for HDTV Video */

    VDIS_CSC_MODE_HDTV_VIDEO_Y2R,
    /**< Select coefficient for HDTV Video */

    VDIS_CSC_MODE_HDTV_GRAPHICS_R2Y,
    /**< Select coefficient for HDTV Graphics */

    VDIS_CSC_MODE_HDTV_GRAPHICS_Y2R,
    /**< Select coefficient for HDTV Graphics */

    VDIS_CSC_MODE_MAX,
    /**< Should be the last value of this enumeration.
         Will be used by driver for validating the input parameters. */

    VDIS_CSC_MODE_NONE = 0xFFFFu
    /**< Used when coefficients are provided */

}VDIS_COLOR_SPACE_MODE_E;

/**
 * \brief DVO Format
 */
typedef enum
{
    VDIS_DVOFMT_SINGLECHAN = 0,
    /**< Ouput data format is single channel with embedded sync */
    VDIS_DVOFMT_DOUBLECHAN,
    /**< Output data format is dual channel with embedded sync */
    VDIS_DVOFMT_TRIPLECHAN_EMBSYNC,
    /**< Output data format is tripple channel with embedded sync */
    VDIS_DVOFMT_TRIPLECHAN_DISCSYNC,
    /**< Ouptut data format is triple channel with discrete sync */
    VDIS_DVOFMT_DOUBLECHAN_DISCSYNC,
    /**< Output data format is dual channel with discrete sync */
    VDIS_DVOFMT_MAX
    /**< This should be the last Enum */
} VDIS_DIGITAL_FMT_E;

/**
 * \brief Analog Format
 */
typedef enum
{
    VDIS_A_OUTPUT_COMPOSITE = 0,
    /**< Analog output format composite */
    VDIS_A_OUTPUT_SVIDEO,
    /**< Analog output format svideo */
    VDIS_A_OUTPUT_COMPONENT,
    /**< Analog output format component */
    VDIS_A_OUTPUT_MAX
} VDIS_ANALOG_FMT_E;

/**
 * \brief Signal polarity
 */
typedef enum
{
    VDIS_POLARITY_ACT_HIGH = 0,
    /**< Signal polarity Active high */
    VDIS_POLARITY_ACT_LOW = 1,
    /**< Signal polarity Active low */
    VDIS_POLARITY_MAX = 2,
    /**< Signal polarity Active low */
}VDIS_SIGNAL_POLARITY_E;





/**
    \brief Display controller output info
*/
typedef struct  VDIS_DC_OUTPUT_INFO_S
{
    UInt32 vencNodeNum;
    /**< Node Number of the Venc */

    UInt32 dvoFmt;
    /**< digital output. See #Vps_DcDigitalFmt for the possible Values */

    UInt32 aFmt;
    /**< Analog output. See #Vps_DcAnalogFmt for the possible Values */

    UInt32 dataFormat;
    /**< Output Data format from Venc. Currently, valid values are
         FVID2_DF_RGB24_888, FVID2_DF_YUV444P, FVID2_DF_YUV422SP_UV */

    UInt32 dvoFidPolarity;
    /**< Polarity for the field id signal for the digital output only
         valid values see #vps_DcSignalPolarity */

    UInt32 dvoVsPolarity;
    /**< Polarity for the vertical sync signal for the digital output only
         valid values see #vps_DcSignalPolarity */

    UInt32 dvoHsPolarity;
    /**< Polarity for the horizontal sync signal for the digital output only
         valid values see #vps_DcSignalPolarity */

    UInt32 dvoActVidPolarity;
    /**< Polarity for the active video signal for the digital output only
         valid values see #vps_DcSignalPolarity */

}VDIS_DC_OUTPUT_INFO_S;

/**
    \brief Display Device Parameter
*/
typedef struct  VDIS_DEV_PARAM_S
{
    UInt32                  enable;
    /**< Enable/Disable Venc usage */

    UInt32                  enableGrpx;
    /**< Enable/Disable Grpx for the device */

    UInt32                  backGroundColor;
    /**< RGB 24-bit color to use for filling background color on a display device */

    UInt32                  colorSpaceMode;
    /**< Color space conversion mode */

    UInt32                  maxOutRes;
    /**< Display resolution */

    UInt32                  resolution;
    /**< Display resolution */

    VDIS_DC_OUTPUT_INFO_S   outputInfo;
    /**< Display output info */


} VDIS_DEV_PARAM_S;


/**
    \brief Mosaic Display Information
*/
typedef struct VDIS_MOSAIC_S
{
    WINDOW_S            displayWindow;
    /**< Target area we want to show mosaic at */

    UInt32              numberOfWindows;
    /**< Number of windows in the mosaic */

    WINDOW_S            winList[VDIS_MOSAIC_WIN_MAX];
    /**< Position and size of each window in the mosaic */

    Bool                useLowCostScaling[VDIS_MOSAIC_WIN_MAX];
    /**< Enable/Disable low cost scaling in mosaic scaling for performance */

    UInt32              chnMap[VDIS_MOSAIC_WIN_MAX];
    /**< Window to channel mapping for each window. Set to VDIS_CHN_INVALID is no channel is mapped to a window */

    Bool                onlyCh2WinMapChanged;
    /**< If only Channel to window mapping is changed then set this flag to TRUE for a smoother window mapping transition.
         If window information is changed keep this to FALSE for a smoother window mapping transition.

     */

    UInt32              outputFPS;
    /**< Rate at which output frames should be generated,
         should typically be equal to the display rate

         Example, for 60fps display, outputFPS should be 60
    */

    Bool                userSetDefaultSWMLayout; //To be Removed

} VDIS_MOSAIC_S;

/**
    \brief Display Sub-system initialization parameters
*/
typedef struct
{
    UInt32           numUserChannels;
    /**< Number of displays that are active */

    UInt32           tiedDevicesMask;
    /**< Vencs tied together. This can be a ORed value of
         VDIS_VENC_HDMI,
         VDIS_VENC_HDCOMP,
         VDIS_VENC_DVO2,
         VDIS_VENC_SD */

    UInt32           enableConfigExtThsFilter;
    /**< When FALSE do not configure THS from M3 / McFW side */

    UInt32           enableConfigExtVideoEncoder;
    /**< When FALSE do not configure Sii9022A from M3 / McFW side */

    UInt32           enableEdgeEnhancement;
    /**< Enable/Disable edge enhancement */

    UInt32 enableLayoutGridDraw;
    /**< Enable/Disable Drawing of Layout specific Grids: TRUE/FALSE */

    VDIS_DEV_PARAM_S deviceParams[VDIS_DEV_MAX];
    /**< Device parameters */

    VDIS_MOSAIC_S    mosaicParams[VDIS_DEV_MAX];
    /**< Initial mosaic for each display device */

    UInt32           numChannels;
    /**< Number of channel inputs to display device */

} VDIS_PARAMS_S;



#endif  /* __TI_VDIS_COMMON_DEF_H__ */

/* @} */
