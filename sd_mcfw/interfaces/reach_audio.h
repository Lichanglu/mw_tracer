#ifndef __REACH_AUDIO_H__
#define __REACH_AUDIO_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "reach_system.h"
#include "common_def/ti_vcap_common_def.h"
#include "common_def/ti_vdis_common_def.h"
#include "ti_audio.h"

#include <osa_que.h>
#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>


#define	AUDIO_DEVICE_NAME_LENGTH		(64)

#define	AUDIO_INPUT_MODE				(1)
#define	AUDIO_OUTPUT_MODE				(2)
#define	AUDIO_FULLDUPLEX_MODE			(3)


#define MAX_INPUT_STR_SIZE				(128)

#define MAX_INPUT_BUFFER				(4*1024)
#define MAX_OUTPUT_BUFFER				(4*1024)

#define MAX_IN_SAMPLES					(1024)
#define SAMPLE_LEN						2

#define AAC_ENC							(0)
#define AAC_DEC							(1)
#define AUDIO_ENCDEC_INBUF_QUE_MAX			(4)
#define AUDIO_ENCDEC_OUTBUF_QUE_MAX			(1)


#define AUDIO_INDATA_GET_FXN_TSK_PRI		(99)
#define AUDIO_INDATA_GET_FXN_TSK_STACK_SIZE	(0)

#define	AUDIO_DRIVER_BUFFER_SIZE		((8192*2)>>2)

typedef struct _audio_capture_params_ {
	Char		device_name[AUDIO_DEVICE_NAME_LENGTH];
	UInt32		samplerate;
	UInt32		samplebit;
	UInt32		channel;
	UInt32		bufsize;
	UInt32		mode;
	Uint32		vol[2];
	UInt32		inputmode;
	UInt32		mictype;
}audio_capture_params;

typedef struct _audio_capture_handle_ {
	snd_pcm_t				*phandle_in;
	snd_pcm_t				*phandle_out;
	snd_pcm_hw_params_t		*params_in;
	snd_pcm_hw_params_t		*params_out;
	audio_capture_params	create_params;
	UInt32					updateparam;
}audio_capture_handle;

typedef struct _audio_buf_ {
	void *addr;
	UInt32 buf_size;
	UInt32 fill_length;
	UInt32 channel_num;
	UInt32 coding_type;

	UInt32 time_stamp;

	UInt32 sample_rate;
	UInt32 bit_rate;
	UInt32 channel;
	UInt32 phy_addr;
	UInt32 seq_id;

	UInt32 LVolume;
	UInt32 RVolume;

	UInt32 reserved[2];
} audio_buf;

typedef struct _audio_encdec_params_ {
	Int32					inst_type;
	AENC_CREATE_PARAMS_S	aenc_create_param;
	ADEC_CREATE_PARAMS_S	adec_create_param;
}audio_encdec_params;

typedef struct _data_user_param_ {
	OSA_QueHndl full_que;
	OSA_QueHndl empty_que;

	UInt32		run_status;

}data_user_param;

typedef Void (*audio_process_fxn)(data_user_param *);

typedef struct _audio_encdec_handle_
{
	UInt32		run_status;
	UInt32		inst_type;

	Void		*aenchandle;
	Void		*adechandle;

	AENC_CREATE_PARAMS_S	aenc_create_param;
	ADEC_CREATE_PARAMS_S	adec_create_param;

	Int32		inbuf_size;
	Int32		outbuf_size;
	audio_buf	inbuf_node[AUDIO_ENCDEC_INBUF_QUE_MAX];
	audio_buf	outbuf_node[AUDIO_ENCDEC_OUTBUF_QUE_MAX];

	OSA_ThrHndl indata_thr;
	OSA_ThrHndl user_indata_thr;
	OSA_ThrHndl user_outdata_thr;

	audio_process_fxn	user_indata_get_fxn;
	audio_process_fxn	user_outdata_put_fxn;

	data_user_param	indata_user_handle;
	data_user_param	outdata_user_handle;
	int updateparm;
} audio_encdec_handle;


typedef struct _audio_struct_ {
	audio_capture_handle *pacaphandle;
	audio_encdec_handle *paenchandle;
	audio_encdec_params paparam;
}audio_struct;
//¡Á?¦Ì¡Á2?¨ª¡§¨®?¦Ì?audio¨¦¨¨??
typedef struct __AudioCommonInfo{
	unsigned int samplerate;   // 48,44.1KHZ
	unsigned int bitrate;   // bitrate 96--128 kbps
	unsigned int channel;    // 2
	unsigned int samplebit;  //16
	unsigned int lvolume ; //left volume
	unsigned int rvolume;
	unsigned int inputmode; // micin  linein
	unsigned int mictype; //??oa ¡ê?¡¤???oa
	unsigned int is_mute; // 1 mute  0 nomute;
	unsigned int input_num ; // input num. 1 :input 1, 2:input 2, 3 :input 1+2
}AudioCommonInfo;

Void audio_dsp_system_init();

Void audio_dsp_system_deinit();


Int32 audio_device_create_inst(Void **pahandle, audio_capture_params *paparam);

Int32 audio_read_frame(Void *pahandle, Int8 *abuf);

Int32 audio_write_frame(Void *pahandle, Int8 *abuf);

Int32 audio_device_delete(Void **ppahandle);

Int32 checkAudioCapParam(audio_struct *pahandle);



Int32 audio_encdec_create_inst(audio_struct *pahandle,
                               Void *user_indata_get_fxn(audio_struct *hndle_param),
                               Void *user_outdata_put_fxn(audio_struct *hndle_param),
                               audio_encdec_params* encparam,
                               Void *put_fxn_app_param);

Int32 audio_dec_create_inst(audio_struct *pahandle, audio_encdec_params *param,
                               Void *user_indata_get_fxn, Void *user_outdata_put_fxn);


Int32 audio_encdec_delete_inst(Void **ppahandle);

Int32 audio_indata_get_empty_buf(data_user_param *param, audio_buf **ppabuf);
Int32 audio_indata_put_full_buf(data_user_param *param, audio_buf *pabuf);
Int32 audio_outdata_get_full_buf(data_user_param *param, audio_buf **ppabuf);
Int32 audio_outdata_put_empty_buf(data_user_param *param, audio_buf *pabuf);

int audio_setCapParamSampleRate(Void *pahandle,int samprate);
int audio_setCapParamFlag(Void *pahandle);
int audio_setCapParamSampleRate(Void *pahandle,int samprate);

int audio_setEncParamFlag(Void * prm);
int audio_setEncParam(Void * prm,int samprate,int bitRate);

extern int audio_setParam(audio_struct *paudosinst, AudioCommonInfo *af);
extern int audio_getParam(audio_struct *paudosinst, AudioCommonInfo *af);
#endif

