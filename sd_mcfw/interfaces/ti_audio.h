#ifndef _TI_AUDIO_H_
#define _TI_AUDIO_H_


#define     AUDIO_ENABLE_INFO_PRINT
#define     AUDIO_ENABLE_ERROR_PRINT




#ifdef      AUDIO_ENABLE_INFO_PRINT
#define     AUDIO_INFO_PRINT(X)    printf X
#else
#define     AUDIO_INFO_PRINT(X)    
#endif

#ifdef      AUDIO_ENABLE_ERROR_PRINT
#define     AUDIO_ERROR_PRINT(X)    printf X
#else
#define     AUDIO_ERROR_PRINT(X)    
#endif


typedef enum
{
    AUDIO_CODEC_TYPE_G711,
    AUDIO_CODEC_TYPE_AAC_LC,
    AUDIO_CODEC_TYPE_MAX

} AUDIO_CODEC_TYPE;




typedef struct {

        Void  *dataBuf;  
        /**< Data buffer. Should be >= alg's expected min input buffer size, else codec API will fail.
             Can be set just once; data would be copied to this same data pointer every time in this case.
             If different buffers are used, AXXX_setBufInfo() should be invoked before every AXXX_process() API.
             Should be in shared region if DSP encode / decode is used.
         */
        UInt32 dataBufSize;  /** Size of dataBuf */
} AUDIO_BUFFER_PARAMS_S;

typedef struct {

    Int32        encoderType;        /**< CodecType - AUDIO_ENC_TYPE_AAC_LC / G711 */
    Int32        bitRate;            /**< Bitrate of encoded stream */
    Int32        sampleRate;         /**< Sample rate of encoded stream */
    Int32        numberOfChannels;   /**< Number of audio channels.  mono=1, stereo=2 */

    Int32        minInBufSize;      /**< Min input buffer size requirement - returned by encoder */ 
    Int32        minOutBufSize;     /**< Min output buffer size requirement - returned by encoder */ 
} AENC_CREATE_PARAMS_S;


typedef struct {

    AUDIO_BUFFER_PARAMS_S   inBuf;        /**< Input Buffer details. dataBufSize tells the input number of bytes */
    AUDIO_BUFFER_PARAMS_S   outBuf;       /**< Output Buffer details. dataBufSize tells the max output buffer size */

} AENC_PROCESS_PARAMS_S;

typedef struct {

    Int32        decoderType;         /**< CodecType - AUDIO_ENC_TYPE_AAC_LC / G711 */
    Int32        desiredChannelMode;  /**< Channel mode - mono, stereo */
    Int32        minInBufSize;        /**< Min input buffer size requirement - returned by encoder */ 
    Int32        minOutBufSize;       /**< Min output buffer size requirement - returned by encoder */ 

} ADEC_CREATE_PARAMS_S;


typedef struct {

    AUDIO_BUFFER_PARAMS_S   inBuf;        /**< Input Buffer details. dataBufSize tells the input number of bytes */
    AUDIO_BUFFER_PARAMS_S   outBuf;       /**< Output Buffer details. dataBufSize tells the max output buffer size */
    Int32                   numSamples;    /**< Samples generated */
    Int32                   channelMode;  /**< Output channel mode - same as IAUDIO_ChannelMode */
    Int32                   pcmFormat;     /**< Output PCM Format - Block/Interleaved, same as IAUDIO_PcmFormat*/
    Int32                   bytesPerSample; /**< Bytes per sample */
} ADEC_PROCESS_PARAMS_S;




#endif
