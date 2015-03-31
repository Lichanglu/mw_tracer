/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \ingroup DEVICE_DRV_FVID2_DEVICE_VID_DEC_API
 * \defgroup DEVICE_DRV_FVID2_DEVICE_VID_DEC_GS2971_API GS2971 Video Decoder API
 *
 *  This modules define API specific to GS2971.
 *
 * @{
*/

/**
 *  \file Device_gs2971.h
 *
 *  \brief GS2971 Video Decoder API
*/

#ifndef _DEVICE_GS2971_H_
#define _DEVICE_GS2971_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "device_videoDecoder.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
  * \addtogroup DEVICE_DRV_FVID2_IOCTL_GS2971
  * @{
*/

/**
  * \brief Set GS2971 video noise filter parameters
  *
  * This IOCTL can be called separetly for each channel.
  *
  * \param cmdArgs       [IN]   Device_Gs2971VideoNfParams *
  * \param cmdArgsStatus [OUT]  NULL
  *
  * \return FVID_SOK on success, else failure
  *
*/
#define IOCTL_DEVICE_GS2971_SET_VIDEO_NF      \
            (DEVICE_VID_DEC_IOCTL_BASE + 0x1000)

/**
  * \brief Set GS2971 video noise filter parameters
  *
  * This IOCTL can be called separetly for each channel.
  *
  * \param cmdArgs       [IN]   Device_Gs2971AudioModeParams *
  * \param cmdArgsStatus [OUT]  NULL
  *
  * \return FVID_SOK on success, else failure
  *
*/
#define IOCTL_DEVICE_GS2971_SET_AUDIO_MODE    \
            (DEVICE_VID_DEC_IOCTL_BASE + 0x1001)

/* @} */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 * \brief Arguments for IOCTL_DEVICE_GS2971_SET_VIDEO_NF
*/
typedef struct
{

    UInt32 channelNum;
  /**< channel number for which these parameters are to be applied */

    UInt32 nfEnable;
  /**< TRUE: Noise filter ON, FALSE: Noise filter OFF */

    UInt32 colorKillerEnable;
  /**< TRUE: Reduce color when noise is HIGH, FALSE: Do not reduce color */

    UInt32 maxNoise;
  /**< Max noise level */

} Device_Gs2971VideoNfParams;

/**
 * \brief Arguments for IOCTL_DEVICE_GS2971_SET_AUDIO_MODE
 *
 * The audio mode is applicable for all channels associated with the
 * device 'deviceNum'
 *
*/
typedef struct
{
    UInt32 deviceNum;
  /**< Device number for which to apply the audio parameters */

    UInt32 numAudioChannels;
  /**< Number of audio channels */

    UInt32 samplingHz;
  /**< Audio sampling rate in Hz, Valid values: 8000, 16000 */

    UInt32 masterModeEnable;
  /**< TRUE: Master mode of operation, FALSE: Slave mode of operation */

    UInt32 dspModeEnable;
  /**< TRUE: DSP data format mode, FALSE: I2S data format mode */

    UInt32 ulawEnable;
  /**< TRUE: 8-bit ulaw data format mode, FALSE: 16-bit PCM data format mode */

    UInt32 cascadeStage;
  /**< Cascade stage number, Valid values: 0..3 */

    UInt32 audioVolume;
  /**< Audio volume, Valid values: 0..8. Refer to GS2971 datasheet for details
   */

    UInt32 tdmChannelNum;
    /**< Number of TDM channels: 0: 2CH, 1: 4CH, 2: 8CH, 3: 12CH 4: 16CH */

} Device_Gs2971AudioModeParams;

#endif /*  _DEVICE_VIDEO_DECODER_H_  */

/*@}*/
