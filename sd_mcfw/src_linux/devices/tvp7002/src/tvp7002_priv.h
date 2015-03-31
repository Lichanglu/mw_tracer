/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _DEVICE_TVP7002_PRIV_H_
#define _DEVICE_TVP7002_PRIV_H_

#include <osa_sem.h>
#include <osa_i2c.h>

#include "ti_vcap_common_def.h"


#define DEVICE_TVP7002_DEBUG
//#define DEVICE_TVP7002_VERBOSE_DEBUG

//#define DEVICE_TVP7002_ENABLE_NF
#define DEVICE_TVP7002_ENABLE_FIRMWARE_PATCHES

#define DEVICE_TVP7002_ENABLE_COMB_FILTER_SETUP

/* Uncomment this to NOT download TVP7002 patch */
#ifdef TI_814X_BUILD
#define DEVICE_TVP7002_NO_PATCH_DOWNLOAD
#endif

/* Uncomment this to FORCE TVP7002 patch download each time
  IOCTL_DEVICE_VIDEO_DECODER_SET_VIDEO_MODE is called
*/
/*#define DEVICE_TVP7002_FORCE_PATCH_DOWNLOAD*/

/* Max possible channel per TVP7002 device  */
#define DEVICE_TVP7002_CH_PER_DEVICE_MAX    (4)

/* Max possible devices cascade together to feed video data to one video port */
#define DEVICE_TVP7002_DEV_MAX              (4)

/* TVP7002 handle state - not in use  */
#define DEVICE_TVP7002_OBJ_STATE_UNUSED   (0)

/* TVP7002 handle state - in use and in idle state  */
#define DEVICE_TVP7002_OBJ_STATE_IDLE     (1)

/* Apply settings to all core's */
#define DEVICE_TVP7002_CORE_ALL    (0xF)

/* TVP7002 registers  */
#define DEVICE_TVP7002_REG_STATUS_1        (0x00)
#define DEVICE_TVP7002_REG_STATUS_2        (0x01)
#define DEVICE_TVP7002_REG_ROM_VERSION     (0x04)
#define DEVICE_TVP7002_REG_RAM_VERSION_0   (0x05)
#define DEVICE_TVP7002_REG_RAM_VERSION_1   (0x06)
#define DEVICE_TVP7002_REG_CHIP_ID_MSB     (0x08)
#define DEVICE_TVP7002_REG_CHIP_ID_LSB     (0x09)
#define DEVICE_TVP7002_REG_VID_STD_STATUS  (0x0C)
#define DEVICE_TVP7002_REG_VID_STD_SELECT  (0x0D)
#define DEVICE_TVP7002_REG_AUTO_SW_MASK    (0x0E)
#define DEVICE_TVP7002_REG_Y_BRIGHTNESS    (0x10)
#define DEVICE_TVP7002_REG_Y_CONTRAST      (0x11)
#define DEVICE_TVP7002_REG_C_SATURATION    (0x13)
#define DEVICE_TVP7002_REG_C_HUE           (0x14)
#define DEVICE_TVP7002_REG_Y_CTRL_1        (0x18)
#define DEVICE_TVP7002_REG_Y_CTRL_2        (0x19)
#define DEVICE_TVP7002_REG_C_CTRL_1        (0x1B)
#define DEVICE_TVP7002_REG_C_CTRL_2        (0x1C)
#define DEVICE_TVP7002_REG_NR_MAX_NOISE    (0x5C)
#define DEVICE_TVP7002_REG_NR_CTRL         (0x5D)
#define DEVICE_TVP7002_REG_OP_MODE_CTRL    (0x60)
#define DEVICE_TVP7002_REG_FV_DEC_CTRL     (0x88)
#define DEVICE_TVP7002_REG_FV_CTRL         (0x89)
#define DEVICE_TVP7002_REG_FBIT_DURATION   (0x8B)
#define DEVICE_TVP7002_REG_ESYNC_OFFSET_1  (0xAE)
#define DEVICE_TVP7002_REG_ESYNC_OFFSET_2  (0xAF)
#define DEVICE_TVP7002_REG_AVD_OUT_CTRL_1  (0xB0)
#define DEVICE_TVP7002_REG_AVD_OUT_CTRL_2  (0xB1)
#define DEVICE_TVP7002_REG_OFM_MODE_CTRL   (0xB2)
#define DEVICE_TVP7002_REG_OFM_CH_SEL_1    (0xB3)
#define DEVICE_TVP7002_REG_OFM_CH_SEL_2    (0xB4)
#define DEVICE_TVP7002_REG_OFM_CH_SEL_3    (0xB5)
#define DEVICE_TVP7002_REG_OFM_SFRAME_LSB  (0xB6)
#define DEVICE_TVP7002_REG_OFM_SFRAME_MSB  (0xB7)
#define DEVICE_TVP7002_REG_OFM_HBLANK_LSB  (0xB8)
#define DEVICE_TVP7002_REG_OFM_HBLANK_MSB  (0xB9)
#define DEVICE_TVP7002_REG_MISC_OFM_CTRL   (0xBA)
#define DEVICE_TVP7002_REG_AUDIO_SAMPLE_HZ (0xC0)
#define DEVICE_TVP7002_REG_AUDIO_GAIN_1    (0xC1)
#define DEVICE_TVP7002_REG_AUDIO_GAIN_2    (0xC2)
#define DEVICE_TVP7002_REG_AUDIO_CTRL      (0xC3)
#define DEVICE_TVP7002_REG_AUDIO_MIXER     (0xC4)
#define DEVICE_TVP7002_REG_AUDIO_MUTE     (0xC5)
#define DEVICE_TVP7002_REG_AUDIO_CASCADE   (0xC8)
#define DEVICE_TVP7002_REG_MIN_F1_ACT      (0xD5)
#define DEVICE_TVP7002_REG_DEC_WR_EN       (0xFE)
#define DEVICE_TVP7002_REG_DEC_RD_EN       (0xFF)

/* TVP7002 register field values  */

/* Mode of operation  */
#define DEVICE_TVP7002_NON_INTERLEAVED_MODE          (0)
#define DEVICE_TVP7002_PIXEL_INTERLEAVED_MODE        (1)
#define DEVICE_TVP7002_LINE_INTERLEAVED_MODE         (2)
#define DEVICE_TVP7002_LINE_INTERLEAVED_HYBRID_MODE  (3)

/* Number of channels */
#define DEVICE_TVP7002_1CH_MUX                       (0)
#define DEVICE_TVP7002_2CH_MUX                       (1)
#define DEVICE_TVP7002_4CH_MUX                       (2)
#define DEVICE_TVP7002_8CH_MUX                       (3)

/* Resolution */
#define DEVICE_TVP7002_RES_D1                        (0)
#define DEVICE_TVP7002_RES_HALF_D1                   (2)
#define DEVICE_TVP7002_RES_CIF                       (3)

/* 8-bit or 16-bit data width */
#define DEVICE_TVP7002_OUT_TYPE_8BIT                 (0)
#define DEVICE_TVP7002_OUT_TYPE_16BIT                (1)

/* Super-frame size type  */
#define DEVICE_TVP7002_SUPER_FRAME_SIZE_525_STD      (0)
#define DEVICE_TVP7002_SUPER_FRAME_SIZE_625_STD      (1)
#define DEVICE_TVP7002_SUPER_FRAME_SIZE_MANUAL       (3)

/* Bit pattern to enable TVP7002 output */
#define DEVICE_TVP7002_OUT_ENABLE      (0x05)

/* TVP7002 status */
#define DEVICE_TVP7002_HSYNC_LOCKED    (0x02)
#define DEVICE_TVP7002_VSYNC_LOCKED    (0x04)
#define DEVICE_TVP7002_SIGNAL_DETECT   (0x80)

#define DEVICE_TVP7002_SIGNAL_60HZ     (0x20)

#define DEVICE_TVP7002_VID_STD_MASK    (0x07)

/* TVP7002 video standard's */
#define DEVICE_TVP7002_VID_STD_PAL_BDGHIN  (0x02)
#define DEVICE_TVP7002_VID_STD_PAL_M       (0x03)
#define DEVICE_TVP7002_VID_STD_PAL_COMB_N  (0x04)
#define DEVICE_TVP7002_VID_STD_PAL_60      (0x07)
#define DEVICE_TVP7002_VID_STD_NTSC_MJ     (0x01)
#define DEVICE_TVP7002_VID_STD_NTSC_4_43   (0x05)

#define DEVICE_TVP7002_NTSC_PAL_WIDTH      (720)
#define DEVICE_TVP7002_NTSC_HEIGHT         (240)
#define DEVICE_TVP7002_PAL_HEIGHT          (288)

/* Audio sampling rate  */
#define DEVICE_TVP7002_AUDIO_16KHZ                   (0)
#define DEVICE_TVP7002_AUDIO_8KHZ                    (2)

/* Audio TDM mode */
#define DEVICE_TVP7002_AUDIO_TDM_2CH       (0x0)
#define DEVICE_TVP7002_AUDIO_TDM_4CH       (0x1)
#define DEVICE_TVP7002_AUDIO_TDM_8CH       (0x2)
#define DEVICE_TVP7002_AUDIO_TDM_12CH      (0x3)
#define DEVICE_TVP7002_AUDIO_TDM_16CH      (0x4)

/*
  TVP7002 Object
*/
typedef struct
{
    Bool   isPalMode;
    /*palMode to be set based on detect status */
    Device_VideoDecoderVideoModeParams videoModeParams;
    /* video mode params */
    Device_VideoDecoderCreateParams createArgs;
    /* create time arguments */
} Device_Tvp7002Obj;

typedef Device_Tvp7002Obj * Device_Tvp7002Handle;
/*
  Overall TVP7002 driver object
*/
typedef struct
{

    OSA_I2cHndl i2cHandle;
    /* i2cHandle for i2c read write operations */
    Device_Tvp7002Handle tvp7002handle[DEVICE_MAX_HANDLES];
    /*
     * TVP7002 handle objects
     */

} Device_Tvp7002CommonObj;

Device_Tvp7002CommonObj gDevice_tvp7002CommonObj;

Int32 Device_tvp7002GetChipId ( Device_Tvp7002Obj * pObj,
                             Device_VideoDecoderChipIdParams * pPrm,
                             Device_VideoDecoderChipIdStatus * pStatus );

Int32 Device_tvp7002GetVideoStatus ( Device_Tvp7002Obj * pObj,
                                     VCAP_VIDEO_SOURCE_STATUS_PARAMS_S * pPrm,
                                     VCAP_VIDEO_SOURCE_CH_STATUS_S     * pStatus );

Int32 Device_tvp7002Reset ( Device_Tvp7002Obj * pObj );

Int32 Device_tvp7002SetVideoMode ( Device_Tvp7002Obj * pObj,
                                Device_VideoDecoderVideoModeParams * pPrm );

Int32 Device_tvp7002Start ( Device_Tvp7002Obj * pObj );

Int32 Device_tvp7002Stop ( Device_Tvp7002Obj * pObj );

Int32 Device_tvp7002SetVideoColor ( Device_Tvp7002Obj * pObj,
                                 Device_VideoDecoderColorParams * pPrm );

Int32 Device_tvp7002SetVideoNf ( Device_Tvp7002Obj * pObj,
                                 Device_Tvp7002VideoNfParams * pPrm );

Int32 Device_tvp7002RegWrite ( Device_Tvp7002Obj * pObj,
                            Device_VideoDecoderRegRdWrParams * pPrm );

Int32 Device_tvp7002RegRead ( Device_Tvp7002Obj * pObj,
                           Device_VideoDecoderRegRdWrParams * pPrm );

Int32 Device_tvp7002OfmReset ( Device_Tvp7002Obj * pObj);

Int32 Device_tvp7002SelectWrite ( Device_Tvp7002Obj * pObj,
                               UInt32 devId, UInt32 value );

Int32 Device_tvp7002VbusWrite ( Device_Tvp7002Obj * pObj,
                             UInt32 devId, UInt32 vbusAddr, UInt8 val,
                             UInt32 len );

Int32 Device_tvp7002VbusRead ( Device_Tvp7002Obj * pObj,
                            UInt32 devId, UInt32 vbusAddr, UInt8 * val );

Int32 Device_tvp7002PatchDownload ( Device_Tvp7002Obj * pObj, UInt32 devId );

Int32 Device_tvp7002OutputEnable ( Device_Tvp7002Obj * pObj, UInt32 enable );

Int32 Device_tvp7002CheckVideoSettings ( UInt32 interleaveMode,
                                      UInt32 chMuxNum, UInt32 vidResSel,
                                      UInt32 outputType );

Int32 Device_tvp7002SetAudioVolume ( Device_Tvp7002Obj * pObj,
                                  UInt32 channelNum, UInt32 audioVolume );

Int32 Device_tvp7002NfEnableAll(Device_Tvp7002Obj * pObj, Bool enable);

Int32 Device_tvp7002PrintChipId ( Device_Tvp7002Obj * pObj);

Int32 Device_tvp7002SetExtendedSettings( Device_Tvp7002Obj * pObj);

Int32 Device_tvp7002SetIndirectRegisters(Device_Tvp7002Obj * pObj);

#endif /*  _DEVICE_TVP7002_PRIV_H_  */

