/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \defgroup DEVICE_DRV_DEVICE_API External Device Interface API
 *
 *  This module defines APIs for external video devices like video
 *  encoders, video decoders, video filters
 *
 *  Typically I2C is used to send control commands to these external devices.
 *  The external device drivers make use of I2C wrapper APIs defined in this module.
 *  The I2C wrapper APIs in turn use the I2C driver to do the actual I2C transfer.
 *
 * @{
*/

/**
 *  \file vps_device.h
 *
 *  \brief External Video Device API
*/

#ifndef _DEVICE_VIDEO_DEVICE_H_
#define _DEVICE_VIDEO_DEVICE_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \name External Video Device Driver ID
 *
 *  Used as drvId when calling FVID2_create()
 *
 */

/* @{ */

#define DEVICE_CMD_BASE   (0x0)
#define DEVICE_CMD_START  (DEVICE_CMD_BASE + 0x1)
#define DEVICE_CMD_STOP   (DEVICE_CMD_BASE + 0x2)
#define DEVICE_CMD_MAX    (DEVICE_CMD_BASE + 0x3)


#define DEVICE_VID_DEC_DRV_BASE            (0x00000400u)

#define DEVICE_VID_ENC_DRV_BASE            (0x00000800u)



/** \brief TVP5158 video decoder driver ID used at the time of FVID2_create() */
#define DEVICE_VID_DEC_TVP5158_DRV    (DEVICE_VID_DEC_DRV_BASE + 0x0000u)

/** \brief TVP7002 video decoder driver ID used at the time of FVID2_create() */
#define DEVICE_VID_DEC_TVP7002_DRV    (DEVICE_VID_DEC_DRV_BASE + 0x0001u)

/** \brief HDMI SII9135 video decoder driver ID used at the time of FVID2_create() */
#define DEVICE_VID_DEC_SII9135_DRV    (DEVICE_VID_DEC_DRV_BASE + 0x0002u)

/** \brief HDMI SII9022a video encoder driver ID used at the time of FVID2_create() */
#define DEVICE_VID_ENC_SII9022A_DRV   (DEVICE_VID_ENC_DRV_BASE + 0x0000u)


#define DEVICE_CA_BOARD_A1_IO_EXP_I2C_ADDR (0x21u)

/* @} */

/**
 *  \name I2C instance ID
 *
 *  Used with I2C APIs
 */

/* @{ */

/** \brief I2C instance 0 */
#define DEVICE_I2C_INST_ID_0   (0)

/** \brief I2C instance 1 */
#define DEVICE_I2C_INST_ID_1   (1)

/** \brief I2C instance 2 */
#define DEVICE_I2C_INST_ID_2   (2)

/** \brief I2C instance 3 */
#define DEVICE_I2C_INST_ID_3   (3)

/** \brief I2C instance not used

    Used as value forDevice_InitParams.i2cClkKHz[n]
    when it is not needed to init i2c instance 'n'
*/
#define DEVICE_I2C_INST_NOT_USED   (0xFFFF)

/* @} */

/**
 *  \name Max limits
 */

/* @{ */

/** \brief Max I2C instance's */
#define DEVICE_I2C_INST_ID_MAX (4)

/** \brief Max I2C instance's */
#define DEVICE_CAPT_INST_MAX  (4)

/** \brief Max handles per external device driver  */
#define DEVICE_MAX_HANDLES  (4)

#define ADV7441_I2C_INST_ID					(0x01)
#define ADV7441_USER_MAP_ADDR				(0x21)
#define ADV7441_USER_MAP1_ADDR				(0x23)
#define ADV7441_USER_MAP2_ADDR				(0x31)
#define ADV7441_USER_MAP3_ADDR				(0x25)
#define ADV7441_HDMI_MAP_ADDR				(0x35)
#define ADV7441_KSV_MAP_ADDR					(0x33)

#define ADV7442_I2C_INST_ID					(0x01)
#define ADV7442_USER_MAP_ADDR				(0x20)
#define ADV7442_USER_MAP1_ADDR				(0x22)
#define ADV7442_USER_MAP2_ADDR				(0x30)
#define ADV7442_USER_MAP3_ADDR				(0x24)
#define ADV7442_HDMI_MAP_ADDR				(0x34)
#define ADV7442_KSV_MAP_ADDR					(0x32)

#define GS2971_IIC_SLAVE_ADDR					(0xFF)

#define GS2972_IIC_SLAVE_ADDR					(0xFF)

#define TVP5158_I2C_ADDR					(0x58)

/* @} */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
  \brief External video device sub-system init parameters
*/
typedef struct
{
    Ptr     i2cRegs[DEVICE_I2C_INST_ID_MAX];
    /**< I2C peripheral base address */

    UInt32  i2cIntNum[DEVICE_I2C_INST_ID_MAX];
    /**< I2C Interrupt number */

    UInt32  i2cClkKHz[DEVICE_I2C_INST_ID_MAX];
    /**< I2C bus clock in KHz

      Set to DEVICE_I2C_INST_NOT_USED in case
      I2C instance init is not needed
    */

    UInt32  isI2cInitReq;
    /**< Indicates whether I2C initialization is required */
}Device_InitParams;

/**
 * \brief Video capture operation mode
*/
typedef enum
{
    DEVICE_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_EMBEDDED_SYNC = 0,
    /**< Single Channel non multiplexed mode */
    DEVICE_CAPT_VIDEO_CAPTURE_MODE_MULTI_CH_LINE_MUX_EMBEDDED_SYNC,
    /**< Multi-channel line-multiplexed mode */
    DEVICE_CAPT_VIDEO_CAPTURE_MODE_MULTI_CH_PIXEL_MUX_EMBEDDED_SYNC,
    /**< Multi-channel pixel muxed */
    DEVICE_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_HSYNC_VBLK,
    /**< Single Channel non multiplexed discrete sync mode with HSYNC and
        VBLK as control signals. */
    DEVICE_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_HSYNC_VSYNC,
    /**< Single Channel non multiplexed discrete sync mode with HSYNC and
        VSYNC as control signals. */
    DEVICE_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VBLK,
    /**< Single Channel non multiplexed discrete sync mode with ACTVID and
        VBLK as control signals. */
    DEVICE_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VSYNC,
    /**< Single Channel non multiplexed discrete sync mode with ACTVID and
        VBLK as control signals. */
    DEVICE_CAPT_VIDEO_CAPTURE_MODE_MULTI_CH_LINE_MUX_SPLIT_LINE_EMBEDDED_SYNC,
    /**< Multi-channel line-multiplexed mode - split line mode */
    DEVICE_CAPT_VIDEO_CAPTURE_MODE_MAX
    /**< Maximum modes */
}Device_CaptVideoCaptureMode;

/**
 * \brief Video interface mode
 */
typedef enum
{
    DEVICE_CAPT_VIDEO_IF_MODE_8BIT = 0,
    /**< Embedded sync mode:  8bit - BT656 standard  */
    DEVICE_CAPT_VIDEO_IF_MODE_16BIT,
    /**< Embedded sync mode:  16bit - BT1120 standard  */
    DEVICE_CAPT_VIDEO_IF_MODE_24BIT,
    /**< Embedded sync mode:  24bit */
    DEVICE_CAPT_VIDEO_IF_MODE_MAX
    /**< Maximum modes */
} Vps_CaptVideoIfMode;

typedef enum
{
    DEVICE_STD_NTSC = 0u,
    DEVICE_STD_PAL = 1,
    DEVICE_STD_480I = 2,
    DEVICE_STD_576I = 3,
    DEVICE_STD_CIF = 4,
    DEVICE_STD_HALF_D1 = 5,
    DEVICE_STD_D1 = 6,
    DEVICE_STD_480P = 7,
    DEVICE_STD_576P = 8,
    DEVICE_STD_720P_60 = 9,
    DEVICE_STD_720P_50 = 10,		
    DEVICE_STD_1080I_60 = 11,
    DEVICE_STD_1080I_50 = 12,
    DEVICE_STD_1080P_60 = 13,
    DEVICE_STD_1080P_50 = 14,
    DEVICE_STD_1080P_24 = 15,
    DEVICE_STD_1080P_30 = 16,   //------16
	
    DEVICE_STD_VGA_640X480X60 = 17,//------17
    DEVICE_STD_VGA_640X480X72 = 18,//------18
    DEVICE_STD_VGA_640X480X75 = 19,//------19
    DEVICE_STD_VGA_640X480X85 = 20,//------20
    DEVICE_STD_VGA_800X600X60 = 21,//------21
    DEVICE_STD_VGA_800X600X72 = 22,//------22
    DEVICE_STD_VGA_800X600X75 = 23,//------23
    DEVICE_STD_VGA_800X600X85 = 24,//------24
    DEVICE_STD_VGA_1024X768X60 = 25,//------25
    DEVICE_STD_VGA_1024X768X70 = 26,//------26
    DEVICE_STD_VGA_1024X768X75 = 27,//------27
    DEVICE_STD_VGA_1024X768X85 = 28,//------28
    DEVICE_STD_VGA_1280X720X60 = 29,//------29
    DEVICE_STD_VGA_1280X768X60 = 30,//------30
    DEVICE_STD_VGA_1280X768X75 = 31,  //------31
    DEVICE_STD_VGA_1280X768X85 = 32,  //------32
    DEVICE_STD_VGA_1280X800X60 = 33,  //------33
    DEVICE_STD_VGA_1280X960X60 = 34,  //------34
    DEVICE_STD_VGA_1280X1024X60 = 35, //------35
    DEVICE_STD_VGA_1280X1024X75 = 36, //------36
    DEVICE_STD_VGA_1280X1024X85 = 37, //------37
    DEVICE_STD_VGA_1366X768X60 = 38, //------38
    DEVICE_STD_VGA_1440X900X60 = 39, //------39
    DEVICE_STD_VGA_1400X1050X60 = 40, //------40
    DEVICE_STD_VGA_1400X1050X75 = 41, //------41
    DEVICE_STD_VGA_1600X1200X60 = 42,//------42
    DEVICE_STD_VGA_1920X1080X60_DMT = 43,//------43
    DEVICE_STD_VGA_1920X1080X60_GTF = 44,//------44
    DEVICE_STD_VGA_1920X1200X60 = 45,//------45
    DEVICE_STD_VGA_2560X1440X60 = 46,//------46

    DEVICE_STD_MUX_2CH_D1 = 47,
    DEVICE_STD_MUX_2CH_HALF_D1 = 48,
    DEVICE_STD_MUX_2CH_CIF = 49,
    DEVICE_STD_MUX_4CH_D1 = 50,
    DEVICE_STD_MUX_4CH_CIF = 51,
    DEVICE_STD_MUX_4CH_HALF_D1 = 52,
    DEVICE_STD_MUX_8CH_CIF = 53,
    DEVICE_STD_MUX_8CH_HALF_D1 = 54,
    DEVICE_STD_AUTO_DETECT = 55,
    DEVICE_STD_CUSTOM = 56,
    
    DEVICE_STD_REACH_LAST,
	
} DEVICE_VIDEO_STANDARD_E;

/* ========================================================================== */
/*                         Functions                                          */
/* ========================================================================== */

/**
  \brief Retrieves i2c address for the decoder

  \param vidDecId   [IN] decoder id
  \param vipInstId  [IN] instance id

  \return FVID2_SOK slave device found, else slave device not found
*/
UInt32 Device_getVidDecI2cAddr(UInt32 vidDecId, UInt32 vipInstId);

/* ------To be implemented ------ */

/**
  \brief Enable/disable debug prints from I2C driver

  Debug prints disabled by default

  \param enable   [IN] TRUE: enable, FALSE: disable

  \return FVID2_SOK on success else failure
*/
Int32 Device_deviceI2cDebugEnable(UInt32 enable);

/**
  \brief Probes an I2C bus for all video devices.

  Probes an I2C bus for all possibly connected slaves to it.
  Prints the detected slave address on the console.

  \param i2cInstId  [IN] \ref DEVICE_I2C_INST_ID_0 or \ref DEVICE_I2C_INST_ID_1

  \return FVID2_SOK on success else failure
*/
Int32 Device_deviceI2cProbeAll(UInt16 i2cInstId);

/**
  \brief Probes an I2C bus for a specific device slave address

  \param i2cInstId  [IN] \ref DEVICE_I2C_INST_ID_0 or \ref DEVICE_I2C_INST_ID_1
  \param slaveAddr  [IN] Slave I2C address

  \return FVID2_SOK slave device found, else slave device not found
*/
Int32 Device_deviceI2cProbeDevice(UInt16 i2cInstId, UInt8 slaveAddr);


#endif /*  _DEVICE_VIDEO_DEVICE_H_  */

/*@}*/
