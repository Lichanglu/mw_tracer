/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 * \brief Arguments for IOCTL_DEVICE_TVP7002_SET_VIDEO_NF
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

} Device_Tvp7002VideoNfParams;

/**
 * \brief Arguments for IOCTL_DEVICE_TVP7002_SET_AUDIO_MODE
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
  /**< Audio volume, Valid values: 0..8. Refer to TVP7002 datasheet for details
   */

    UInt32 tdmChannelNum;
    /**< Number of TDM channels: 0: 2CH, 1: 4CH, 2: 8CH, 3: 12CH 4: 16CH */

} Device_Tvp7002AudioModeParams;

#endif /*  _DEVICE_VIDEO_DECODER_H_  */

/*@}*/

