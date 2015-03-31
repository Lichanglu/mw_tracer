#include <string.h>

#include "reach_system_priv.h"
#include "reach_audio.h"
#include <osa_thr.h>
#include <sys/time.h>

static UInt32 getostime()
{
    struct timeval tv;
    struct timezone tz;
    UInt32 ultime;

    gettimeofday (&tv , &tz);
    ultime= tv.tv_sec*1000+tv.tv_usec/1000;
    return (ultime);
}

extern Void* Audio_allocMem (Int32 _size);

extern Void Audio_freeMem (Void *buf);

extern Void Audio_systemProcInit (Void);

extern Void Audio_systemProcDeInit (Void);

extern Void *Audio_allocateSharedRegionBuf (Int32 bufSize);

extern Void Audio_freeSharedRegionBuf (Void *buf, Int32 bufSize);

extern Int32 Audio_getEncoderContextSize (Int32 codecType);

extern Int32 Audio_getDecoderContextSize (Int32 codecType);

extern Int32 Audio_encode(Void* ctxMem, AENC_PROCESS_PARAMS_S *pPrm);

extern Int32 Audio_decode(Void* ctxMem, ADEC_PROCESS_PARAMS_S *pPrm);

extern Void* Audio_createAacEncAlgorithm(Void *ctxMem, AENC_CREATE_PARAMS_S *pPrm);

extern Void *Audio_createAacDecAlgorithm(Void *ctxMem, ADEC_CREATE_PARAMS_S *pPrm);

extern Int32 Audio_deleteEncAlgorithm(Void *ctxMem);

extern Int32 Audio_deleteDecAlgorithm(Void *ctxMem);

extern Void* Audio_setAacEncParam(Void *ctxMem, AENC_CREATE_PARAMS_S *pPrm,int sampleRate);

Int32 setUpdateflag(Void **pahandle)
{

	audio_capture_handle *ppahandle;
	ppahandle=*pahandle;
	ppahandle->updateparam=1;
	return ppahandle->updateparam;
}
Int32 getUpdateflag(Void **pahandle)
{

	audio_capture_handle *ppahandle;
	ppahandle=*pahandle;
	return ppahandle->updateparam;
}
Int32 clearUpdateflag(Void **pahandle)
{

	audio_capture_handle *ppahandle;
	ppahandle=*pahandle;
	ppahandle->updateparam=0;
	return ppahandle->updateparam;
}

static Int32 hwparam_init(audio_capture_params *paparam, snd_pcm_t **pphandle, snd_pcm_hw_params_t **pparams)
{
	UInt32 ret = 0;
	UInt32 samplerate = 0;
	UInt32 channel = 0;
	snd_pcm_uframes_t period_size;
	if(paparam == NULL)
	{
		OSA_printf("audio_capture_init: handle is NULL: %s\n", snd_strerror(ret));
		return -1;
	}
	if(*pparams != NULL)
	{
		snd_pcm_hw_params_free(*pparams);
		*pparams = NULL;
	}
//	printf("1 *pparams = %p\n",*pparams);
	ret = snd_pcm_hw_params_malloc(pparams);
	printf("2 *pparams = %p\n",*pparams);
	if(ret < 0)
	{
		OSA_printf("audio_capture_init: snd_pcm_hw_params_malloc error: %s\n", snd_strerror(ret));
		goto cleanup;
	}
//	printf("3 *pparams = %p=*pphandle=%p\n",*pparams,*pphandle);
	ret = snd_pcm_hw_params_any(*pphandle, *pparams);
	if(ret < 0)
	{
		OSA_printf("audio_capture_init: snd_pcm_hw_params_any error: %s\n", snd_strerror(ret));
		goto cleanup;
	}

	ret = snd_pcm_hw_params_set_access(*pphandle, *pparams, SND_PCM_ACCESS_RW_INTERLEAVED);
	if(ret < 0)
	{
		OSA_printf("audio_capture_init: snd_pcm_hw_params_set_access error: %s\n", snd_strerror(ret));
		goto cleanup;
	}

	ret = snd_pcm_hw_params_set_format(*pphandle, *pparams, SND_PCM_FORMAT_S16_LE);
	if(ret < 0)
	{
		OSA_printf("audio_capture_init: snd_pcm_hw_params_set_format error: %s\n", snd_strerror(ret));
		goto cleanup;
	}

	samplerate = paparam->samplerate;
//	printf("==================samplerate=%d\n",samplerate);
	ret = snd_pcm_hw_params_set_rate_near(*pphandle, *pparams, &samplerate, 0);
	if(ret < 0)
	{
		OSA_printf("audio_capture_init: snd_pcm_hw_params_set_rate_near error: %s\n", snd_strerror(ret));
		goto cleanup;
	}

	channel = paparam->channel;
	ret = snd_pcm_hw_params_set_channels(*pphandle, *pparams, channel);
	if(ret < 0)
	{
		OSA_printf("audio_capture_init: snd_pcm_hw_params_set_channels error: %s\n", snd_strerror(ret));
		goto cleanup;
	}

	UInt32 drive_buf_size = AUDIO_DRIVER_BUFFER_SIZE;
	ret = snd_pcm_hw_params_set_buffer_size(*pphandle, *pparams, drive_buf_size);
	if(ret < 0)
	{
		OSA_printf("audio_capture_init: snd_pcm_hw_params_set_buffer_size error: %s\n", snd_strerror(ret));
		goto cleanup;
	}

	period_size = paparam->bufsize / (2 * paparam->channel);
	ret = snd_pcm_hw_params_set_period_size_near(*pphandle, *pparams, &period_size, 0);
	if(ret < 0)
	{
		OSA_printf("audio_capture_init: snd_pcm_hw_params_set_period_size_near error: %s\n", snd_strerror(ret));
		goto cleanup;
	}

	ret = snd_pcm_hw_params(*pphandle, *pparams);
	if(ret < 0)
	{
		OSA_printf("audio_capture_init: snd_pcm_hw_params error: %s\n", snd_strerror(ret));
		goto cleanup;
	}

	ret = snd_pcm_prepare(*pphandle);
	if(ret < 0)
	{
		OSA_printf("audio_capture_init: snd_pcm_prepare error: %s\n", snd_strerror(ret));
		goto cleanup;
	}

		return 0;

cleanup:

	if(*pparams != NULL)
	{
		snd_pcm_hw_params_free(*pparams);
		*pparams = NULL;
	}

	return -1;

}


Int32 checkAudioCapParam(audio_struct *pahandle)
{
	UInt32 ret = 0;
	Int8 device_name[64] = {0};
	int i;
	Int32 num_inst = 1;

	for(i=0;i<num_inst;i++){
		if(pahandle[i].pacaphandle == NULL)
		{
			OSA_printf("warnning,the %d audio cap is null\n",i);
			continue;
		}
		if(pahandle[i].pacaphandle->updateparam==1){
			strcpy(device_name, pahandle[i].pacaphandle->create_params.device_name);
			snd_pcm_close(pahandle[i].pacaphandle->phandle_in);
			pahandle[i].pacaphandle->phandle_in = NULL;
			usleep(100000);
			ret = snd_pcm_open(&(pahandle[i].pacaphandle->phandle_in), device_name, SND_PCM_STREAM_CAPTURE, 0);
			if(ret < 0){
				OSA_printf("audio_capture_init: snd_pcm_open error: %s\n", snd_strerror(ret));
			}
			OSA_printf("snd pcm reopen,pahandle[i].pacaphandle->phandle_i =%p=%d==%s\n",pahandle[i].pacaphandle->phandle_in,i,device_name);
			ret = hwparam_init(&(pahandle[i].pacaphandle->create_params), &(pahandle[i].pacaphandle->phandle_in), &(pahandle[i].pacaphandle->params_in));
			pahandle[i].pacaphandle->updateparam=0;
		}
	}
	return -1;
}

Int32 audio_device_create_inst(Void **pahandle, audio_capture_params *paparam)
{
	Int32 ret = 0;
	Int8 device_name[256] = {0};

	if((pahandle == NULL) || (*pahandle != NULL) || (paparam == NULL)){
		OSA_printf("audio_capture_init error, handle or params is NULL!\n");
		return -1;
	}

	if(paparam->bufsize <= 0){
		OSA_printf("audio_capture_init error, bufsize error!\n");
		return -1;
	}

	if(paparam->mode != AUDIO_INPUT_MODE && paparam->mode != AUDIO_OUTPUT_MODE
	        && paparam->mode != AUDIO_FULLDUPLEX_MODE)
	{
		OSA_printf("audio_capture_init: audio mode error!\n");
		return -1;
	}

	audio_capture_handle *ppahandle = (audio_capture_handle *)malloc(sizeof(audio_capture_handle));
	if(ppahandle == NULL){
		OSA_printf("audio_capture_init: malloc handle faile!\n");
		return -1;
	}
	memset(ppahandle,0,sizeof(audio_capture_handle));

	strcpy(device_name, paparam->device_name);

	if(paparam->mode == AUDIO_INPUT_MODE || paparam->mode == AUDIO_FULLDUPLEX_MODE){
		printf("pahandle[i].pacaphandle->phandle_i =%p\n",ppahandle->phandle_in);

		ret = snd_pcm_open(&(ppahandle->phandle_in), device_name, SND_PCM_STREAM_CAPTURE, 0);
		if(ret < 0){
			OSA_printf("audio_capture_init: snd_pcm_open error: %s\n", snd_strerror(ret));
			goto cleanup;
		}
		printf("=%p,ppahandle->params_i=%p\n",ppahandle->phandle_in,ppahandle->params_in);
		ret = hwparam_init(paparam, &(ppahandle->phandle_in), &(ppahandle->params_in));
		if(ret < 0){
			OSA_printf("audio_capture_init: hwparam_init error: %s\n", snd_strerror(ret));
			goto cleanup;
		}
	}

	if(paparam->mode == AUDIO_OUTPUT_MODE || paparam->mode == AUDIO_FULLDUPLEX_MODE){
		ret = snd_pcm_open(&(ppahandle->phandle_out), device_name, SND_PCM_STREAM_PLAYBACK, 0);
		if(ret < 0){
			OSA_printf("audio_capture_init: snd_pcm_open error: %s\n", snd_strerror(ret));
			goto cleanup;
		}

		ret = hwparam_init(paparam, &(ppahandle->phandle_out), &(ppahandle->params_out));
		if(ret < 0){
			OSA_printf("audio_capture_init: hwparam_init error: %s\n", snd_strerror(ret));
			goto cleanup;
		}
	}

	memcpy(&(ppahandle->create_params), paparam, sizeof(audio_capture_params));

	*pahandle = ppahandle;

	return 0;

cleanup:

	if(ppahandle->phandle_in){
		snd_pcm_drain(ppahandle->phandle_in);
		snd_pcm_close(ppahandle->phandle_in);
	}

	if(ppahandle->phandle_out){
		snd_pcm_drain(ppahandle->phandle_out);
		snd_pcm_close(ppahandle->phandle_out);
	}

	if(ppahandle){
		free(ppahandle);
	}

	return -1;
}

static Int arecovery(snd_pcm_t *handle, Int err)
{
	if(err == -EPIPE)
	{
		if(snd_pcm_prepare(handle) < 0)
		{
			OSA_printf("arecovery error !\n");
			return -1;
		}
	}
	else
		if(err == -ESTRPIPE)
		{
			while((err = snd_pcm_resume(handle)) == -EAGAIN)
			{
				sleep (1);
			}

			if(snd_pcm_prepare(handle) < 0)
			{
				OSA_printf("arecovery error !\n");
				return -1;
			}
		}
		else
		{
			return -1;
		}

	return 0;
}


Int32 audio_read_frame(Void *pahandle, Int8 *abuf)
{
	Int32 num_samples = 0, read_samples = 0;
	Int8 *bufptr = NULL;

	audio_capture_handle *ahandle = (audio_capture_handle *)pahandle;

	if(ahandle == NULL)
	{
		OSA_printf("audio_read_frame error, handle is NULL!\n");
		return -1;
	}

	if(abuf == NULL)
	{
		OSA_printf("audio_read_frame error, buf is NULL!\n");
		return -1;
	}

	audio_capture_params	*paparam = &(ahandle->create_params);
	snd_pcm_t				*phandle = ahandle->phandle_in;

	read_samples = paparam->bufsize / (2 * paparam->channel);
	bufptr = abuf;

	while(read_samples > 0)
	{
		num_samples = snd_pcm_readi(phandle, bufptr, read_samples);
		if(num_samples == -EAGAIN)
			continue;

		if(num_samples < 0)
		{
			if(arecovery(phandle, num_samples) < 0)
			{
				OSA_printf("audio_read_frame error, (%s)\n", strerror(num_samples));
				return -1;
			}
		}
		else
		{
			bufptr += num_samples * 2 * paparam->channel;
			read_samples -= num_samples;
		}
	}

	return num_samples;
}

Int audio_write_frame(Void *pahandle, Int8 *abuf)
{
	Int32 num_samples, write_samples;
	Int8 *bufPtr;

	audio_capture_handle *ahandle = (audio_capture_handle *)pahandle;

	if(ahandle == NULL)
	{
		OSA_printf("audio_read_frame error, handle is NULL!\n");
		return -1;
	}

	if(abuf == NULL)
	{
		OSA_printf("audio_read_frame error, buf is NULL!\n");
		return -1;
	}

	audio_capture_params	*paparam = &(ahandle->create_params);
	snd_pcm_t				*phandle = ahandle->phandle_out;

	write_samples = paparam->bufsize / (2 * paparam->channel);
	bufPtr = abuf;

	while(write_samples > 0)
	{
		num_samples = snd_pcm_writei(phandle, bufPtr, write_samples);
		//OSA_printf("write: num_samples = %d\n", num_samples);
		if (num_samples == -EAGAIN)
			continue;

		if(num_samples < 0)
		{
			if (arecovery(phandle,num_samples) < 0)
			{
				OSA_printf("audio_write_frame error,  (%s)\n", strerror(num_samples));
				return -1;
			}
		}
		else
		{
			bufPtr += num_samples * 2 * paparam->channel;
			write_samples -= num_samples;
		}
	}

	return 0;
}

Int32 audio_device_delete(Void **ppahandle)
{
	if((ppahandle == NULL) || (*ppahandle == NULL)){
		OSA_printf("audio_device_delete error, handle is NULL!\n");
		return -1;
	}

	audio_capture_handle *pahandle = (audio_capture_handle *)(*ppahandle);

	if(pahandle == NULL){
		OSA_printf("audio_device_delete: handle is NULL!\n");
		return -1;
	}

	if(((pahandle->create_params.mode == AUDIO_INPUT_MODE)
		|| (pahandle->create_params.mode == AUDIO_FULLDUPLEX_MODE))
		&& pahandle->phandle_in != NULL)
	{
		snd_pcm_drain(pahandle->phandle_in);
		snd_pcm_close(pahandle->phandle_in);
	}

	if(((pahandle->create_params.mode == AUDIO_OUTPUT_MODE)
		|| (pahandle->create_params.mode == AUDIO_FULLDUPLEX_MODE))
		&& pahandle->phandle_out != NULL)
	{
		snd_pcm_drain(pahandle->phandle_out);
		snd_pcm_close(pahandle->phandle_out);
	}

	free(pahandle);

	*ppahandle = NULL;

	return 0;
}


Void audio_dsp_system_init()
{
	return Audio_systemProcInit();
}

Void audio_dsp_system_deinit()
{
	return Audio_systemProcDeInit();
}


static Void *audio_allocate_sharedregionbuf(Int32 bufSize)
{
	return Audio_allocateSharedRegionBuf(bufSize);
}

static Void audio_free_sharedregionbuf(Void *buf, Int32 bufSize)
{
	return Audio_freeSharedRegionBuf(buf, bufSize);
}



static Int32 audio_aacenc_create(Void **handle, AENC_CREATE_PARAMS_S *pPrm)
{
	*handle = Audio_allocMem(Audio_getEncoderContextSize(AUDIO_CODEC_TYPE_AAC_LC));

	if(Audio_createAacEncAlgorithm(*handle, pPrm) != NULL)
		return 0;

	return -1;
}

static Int32 audio_aacenc_process(Void *handle, AENC_PROCESS_PARAMS_S *pPrm)
{
	return Audio_encode(handle, pPrm);
}

static Int32 audio_aacenc_delete(Void *handle)
{
	Int32 status = ERROR_FAIL;

	if(handle)
	{
		status = Audio_deleteEncAlgorithm(handle);
		Audio_freeMem(handle);
	}

	return status;
}


static Int32 audio_aacdec_create(Void **handle, ADEC_CREATE_PARAMS_S *pPrm)
{
	*handle = Audio_allocMem(Audio_getDecoderContextSize(AUDIO_CODEC_TYPE_AAC_LC));

	if(Audio_createAacDecAlgorithm(*handle, pPrm) != NULL)
		return 0;

	return -1;
}

static Int32 audio_aacdec_process(Void *handle, ADEC_PROCESS_PARAMS_S *pPrm)
{
	return Audio_decode(handle, pPrm);
}

static Int32 audio_aacdec_delete(Void *handle)
{
	Int32 status = ERROR_FAIL;

	if(handle)
	{
		status = Audio_deleteDecAlgorithm(handle);
		Audio_freeMem(handle);
	}

	return status;
}
int audio_setEncParamFlag(Void * prm)
{
	audio_encdec_handle *phandle = (audio_encdec_handle *)prm;
	phandle->updateparm=1;

	return 0;
}
int audio_setEncParam(Void * prm,int samprate,int bitRate)
{
	audio_encdec_handle *phandle = (audio_encdec_handle *)prm;
	phandle->aenc_create_param.sampleRate=samprate;
	phandle->aenc_create_param.bitRate=bitRate;

	return 0;
}
int audio_setCapParamFlag(Void *pahandle)
{

	audio_capture_handle *ahandle = (audio_capture_handle *)pahandle;
	ahandle->updateparam=1;

	return 0;
}
int audio_setCapParamSampleRate(Void *pahandle,int samprate)
{

	audio_capture_handle *ahandle = (audio_capture_handle *)pahandle;
	ahandle->create_params.samplerate=samprate;

	return 0;
}

static Void *audio_encThread(audio_struct *paudosinst)
{
	audio_encdec_handle *phandle = paudosinst[0].paenchandle;
	//audio_encdec_handle *phandle2 = paudosinst[1].paenchandle;
	Int32 status = -1;
	audio_buf	*painbuf;
	audio_buf	*paoutbuf;
	audio_buf	*painbuf2;
	audio_buf	*paoutbuf2;
	AENC_PROCESS_PARAMS_S	enc_param;
	ADEC_PROCESS_PARAMS_S	dec_param;
	AENC_CREATE_PARAMS_S *paacenc_params = &(phandle->aenc_create_param);
	//FILE *fp = fopen("audio.pcm", "w+");

	//printf("phandle= %p,phandle2 =%p\n",phandle,phandle2);
 	while(TRUE == phandle->run_status)
	{
		if(phandle->updateparm)
			{
				Audio_setAacEncParam(phandle->aenchandle, paacenc_params,phandle->aenc_create_param.sampleRate);
				printf("phandle->aenc_create_param.sampleRate=%d\n",phandle->aenc_create_param.sampleRate);
				phandle->updateparm=0;
			}
		if(phandle->indata_user_handle.full_que.count >= 5)
		{
	//		printf("phandle->outdata_user_handle.empty_que =%d %d\n",phandle->indata_user_handle.full_que.count,phandle->outdata_user_handle.empty_que.count);
		}
		status = OSA_queGet(&(phandle->indata_user_handle.full_que), (Int32 *)(&painbuf), OSA_TIMEOUT_FOREVER);
		status = OSA_queGet(&(phandle->outdata_user_handle.empty_que), (Int32 *)(&paoutbuf), OSA_TIMEOUT_FOREVER);

		#if 0
		struct timeval tv1;
		struct timeval tv2;
		gettimeofday(&tv1, NULL);
		#endif
		//fwrite(painbuf->addr, painbuf->buf_size, 1, fp);
		if(phandle->inst_type == AAC_ENC)
		{
	#if 1
			enc_param.inBuf.dataBufSize		= phandle->inbuf_size;
			enc_param.inBuf.dataBuf			= painbuf->addr;
			enc_param.outBuf.dataBufSize	= phandle->outbuf_size;
			enc_param.outBuf.dataBuf		= paoutbuf->addr;

			status = audio_aacenc_process(phandle->aenchandle, &enc_param);
			if(enc_param.inBuf.dataBufSize <= 0)
			{
				OSA_printf("audio enc frame error!\n");
				break;
			}

			paoutbuf->buf_size = enc_param.outBuf.dataBufSize;
			paoutbuf->fill_length = enc_param.outBuf.dataBufSize;
			paoutbuf->time_stamp  = getostime();
			paoutbuf->LVolume = painbuf->LVolume;
			paoutbuf->RVolume = painbuf->RVolume;
		#endif
		}
		else if(phandle->inst_type == AAC_DEC)
		{
			dec_param.inBuf.dataBufSize		= phandle->inbuf_size;
			dec_param.inBuf.dataBuf			= painbuf->addr;
			dec_param.outBuf.dataBufSize	= phandle->outbuf_size;
			dec_param.outBuf.dataBuf		= paoutbuf->addr;

			//fwrite(painbuf->addr, painbuf->buf_size, 1, fp);
			status = audio_aacdec_process(phandle->adechandle, &dec_param);
			if(enc_param.inBuf.dataBufSize == phandle->inbuf_size)
			{
				OSA_printf("audio dec frame error!\n");
				break;
			}

		}

		#if 0
		gettimeofday(&tv2, NULL);
	    if(tv2.tv_usec < tv1.tv_usec)
	    {
	        printf("time  %u %u\n", tv2.tv_sec - 1 - tv1.tv_sec, tv2.tv_usec + 1000000 - tv1.tv_usec);
	    }
	    else
	    {
	        printf("time  %u %u\n",tv2.tv_sec - tv1.tv_sec, tv2.tv_usec - tv1.tv_usec);
	    }
		#endif

		status = OSA_quePut(&(phandle->outdata_user_handle.full_que), (Int32)paoutbuf,OSA_TIMEOUT_NONE);
		status = OSA_quePut(&(phandle->indata_user_handle.empty_que), (Int32)painbuf,OSA_TIMEOUT_NONE);

#if 0
		if(phandle2->updateparm)
		{
			Audio_setAacEncParam(phandle2->aenchandle, paacenc_params,phandle2->aenc_create_param.sampleRate);
			printf("phandle->aenc_create_param.sampleRate=%d\n",phandle->aenc_create_param.sampleRate);
			phandle2->updateparm=0;
		}

		status = OSA_queGet(&(phandle2->indata_user_handle.full_que), (Int32 *)(&painbuf2), OSA_TIMEOUT_FOREVER);
		status = OSA_queGet(&(phandle2->outdata_user_handle.empty_que), (Int32 *)(&paoutbuf2), OSA_TIMEOUT_FOREVER);
//此处编码第二路

#if 0
			enc_param.inBuf.dataBufSize		= phandle2->inbuf_size;
			enc_param.inBuf.dataBuf			= painbuf2->addr;
			enc_param.outBuf.dataBufSize	= phandle2->outbuf_size;
			enc_param.outBuf.dataBuf		= paoutbuf2->addr;

			status = audio_aacenc_process(phandle2->aenchandle, &enc_param);
			if(enc_param.inBuf.dataBufSize <= 0)
			{
				OSA_printf("audio enc frame error!\n");
				break;
			}

			paoutbuf2->buf_size = enc_param.outBuf.dataBufSize;
			paoutbuf2->fill_length = enc_param.outBuf.dataBufSize;
			paoutbuf2->time_stamp  = getostime();
			paoutbuf2->LVolume = painbuf2->LVolume;
			paoutbuf2->RVolume = painbuf2->RVolume;
#endif
		status = OSA_quePut(&(phandle2->outdata_user_handle.full_que), (Int32)paoutbuf2,OSA_TIMEOUT_NONE);
		status = OSA_quePut(&(phandle2->indata_user_handle.empty_que), (Int32)painbuf2,OSA_TIMEOUT_NONE);

#endif

	}

	return NULL;
}

static Void *audio_decThread(audio_struct *paudosinst)
{
	audio_encdec_handle *phandle = paudosinst->paenchandle;
	Int32 status = -1;

	audio_buf	*painbuf;
	audio_buf	*paoutbuf;
	ADEC_PROCESS_PARAMS_S	dec_param;

	while(TRUE == phandle->run_status){
		status = OSA_queGet(&(phandle->indata_user_handle.full_que), (Int32 *)(&painbuf), OSA_TIMEOUT_FOREVER);
		status = OSA_queGet(&(phandle->outdata_user_handle.empty_que), (Int32 *)(&paoutbuf), OSA_TIMEOUT_FOREVER);

		dec_param.inBuf.dataBufSize 	= phandle->inbuf_size;
		dec_param.inBuf.dataBuf 		= painbuf->addr;
		dec_param.outBuf.dataBufSize	= phandle->outbuf_size;
		dec_param.outBuf.dataBuf		= paoutbuf->addr;

		status = audio_aacdec_process(phandle->adechandle, &dec_param);
		if(dec_param.inBuf.dataBufSize == phandle->inbuf_size){
			OSA_printf("audio dec frame error!\n");
			break;
		}

		status = OSA_quePut(&(phandle->outdata_user_handle.full_que), (Int32)paoutbuf,OSA_TIMEOUT_NONE);
		status = OSA_quePut(&(phandle->indata_user_handle.empty_que), (Int32)painbuf,OSA_TIMEOUT_NONE);
	}

	return NULL;
}


int audio_setParam(audio_struct *paudosinst, AudioCommonInfo *af)
{
	unsigned int temp;
	int devneedupdate=0;
	int ecneedupdate=0;

	if(paudosinst==NULL)return -1;
	int ch = af->input_num;


	if(ch != 0 || ch != 1) {
		ch = 0;
	};

	temp = af->samplerate;
	printf(" audio_setParam: audio_setParam=%d\n",temp);
	if((temp != 16000) && (temp != 44100) && (temp != 48000)) {
		return -1;
	}

	temp = af->bitrate;
	printf("==temp==%dh\n",temp);
	if(temp != 32000 && temp != 64000 && temp != 96000 && temp != 128000) {
		return -1;
	}
	temp = af->inputmode;
	printf("==temp==%dh\n",temp);
	if(temp != 1 && temp != 0) {
		return -1;
	}

	temp = af->mictype;
printf("==temp==%dh\n",temp);
	if(temp != 1 && temp != 0) {
		return -1;
	}

	temp = af->is_mute;

	if(temp != 1 || temp != 0) {
		//return -1;
	}
	temp = af->input_num;
	if(temp != 1 && temp != 2 && temp != 3) {
		//return -1;
	}
	temp = paudosinst[ch].pacaphandle->create_params.samplerate; //采样率
	printf("old=%d,new=%d\n",temp,af->samplerate);
	printf("=%d=phandle= %p,phandle2 =%p\n",ch,paudosinst[0].paenchandle,paudosinst[1].paenchandle);
	if(temp != af->samplerate) {
		paudosinst[ch].pacaphandle->create_params.samplerate = af->samplerate;
		paudosinst[ch].paenchandle->aenc_create_param.sampleRate = af->samplerate;
		devneedupdate=1;
		ecneedupdate=1;
		};
	temp = paudosinst[ch].paenchandle->aenc_create_param.bitRate;
	if(temp != af->bitrate) {
		paudosinst[ch].paenchandle->aenc_create_param.bitRate = af->bitrate;
		ecneedupdate=1;
		};
	temp = paudosinst[ch].pacaphandle->create_params.vol[0];
	if(temp != af->lvolume) {
		paudosinst[ch].pacaphandle->create_params.vol[0] = af->lvolume;
		devneedupdate=1;
		};
	temp = paudosinst[ch].pacaphandle->create_params.vol[1];
	if(temp != af->rvolume) {
		paudosinst[ch].pacaphandle->create_params.vol[1] = af->rvolume;
		devneedupdate=1;
		};
	temp = paudosinst[ch].pacaphandle->create_params.inputmode;
	if(temp != af->inputmode) {
		paudosinst[ch].pacaphandle->create_params.inputmode = af->inputmode;
		devneedupdate=1;
		};
	temp = paudosinst[ch].pacaphandle->create_params.mictype;
	if(temp != af->mictype) {
		paudosinst[ch].pacaphandle->create_params.mictype = af->mictype;
		devneedupdate=1;
		};
	//temp = paudosinst[ch].paenchandle->aenc_create_param.is_mute;
	//if(temp != af.is_mute) {devneedupdate=1;ecneedupdate=1;};
printf("========ch=%d=====%d,%d\n",ch,devneedupdate,ecneedupdate);

		paudosinst[ch].pacaphandle->updateparam = devneedupdate;
		paudosinst[ch].paenchandle->updateparm = ecneedupdate;
	return 0;
}
int audio_getParam(audio_struct *paudosinst, AudioCommonInfo *af)
{
	if(paudosinst==NULL || af==NULL)return -1;
	int ch = af->input_num;

	if(ch != 0 || ch != 1) {
		ch = af->input_num = 0;
	};

	af->samplerate = paudosinst[ch].pacaphandle->create_params.samplerate;

	af->bitrate = paudosinst[ch].paenchandle->aenc_create_param.bitRate;
	af->channel = paudosinst[ch].pacaphandle->create_params.channel;

	af->samplebit = paudosinst[ch].pacaphandle->create_params.samplebit;

	af->lvolume = paudosinst[ch].pacaphandle->create_params.vol[0];

	af->rvolume = paudosinst[ch].pacaphandle->create_params.vol[1];

	af->inputmode = paudosinst[ch].pacaphandle->create_params.inputmode;

	af->mictype = paudosinst[ch].pacaphandle->create_params.mictype;

	return 0;
}

Int32 audio_encdec_create_inst(audio_struct *pahandle,
                               Void *user_indata_get_fxn(audio_struct *hndle_param),
                               Void *user_outdata_put_fxn(audio_struct *hndle_param),
                               audio_encdec_params* encparam,
                               Void *put_fxn_app_param)
{
	Int32 status = -1;
	Int32 index = 0;
	int i;
	audio_struct tmp;
	tmp.pacaphandle=0;

	Int32 num_inst = 1;

	if((pahandle == NULL)|| (user_indata_get_fxn == NULL) || (user_outdata_put_fxn == NULL))
	{
		OSA_printf("audio_capture_init error, handle or params is NULL!\n");
		return -1;
	}
	for(i=0;i<num_inst;i++)
	{
		if(pahandle[i].paparam.inst_type != AAC_ENC && pahandle[i].paparam.inst_type != AAC_DEC){
			OSA_printf("audio_encdec_create_inst: audio inst type error!\n");
			return -1;
		}
	}
	for(i=0;i<num_inst;i++)
	{
		pahandle[i].paenchandle=(audio_encdec_handle *)malloc(sizeof(audio_encdec_handle));
		if(pahandle[i].paenchandle == NULL){
			OSA_printf("audio_capture_init: malloc handle faile!\n");
			return -1;//存在泄漏的可能
		}
	}

	for(i=0;i<num_inst;i++)
	{
		pahandle[i].paenchandle->inst_type = pahandle[i].paparam.inst_type;
		pahandle[i].paenchandle->user_indata_get_fxn = user_indata_get_fxn;
		pahandle[i].paenchandle->user_outdata_put_fxn = user_outdata_put_fxn;
	}

	for(i=0;i<num_inst;i++)
	{
		/* 创建第二路编码库实例 */
		if(pahandle[i].paenchandle->inst_type == AAC_ENC){
			memcpy(&(pahandle[i].paenchandle->aenc_create_param), &(encparam[i].aenc_create_param), sizeof(AENC_CREATE_PARAMS_S));
			status = audio_aacenc_create(&(pahandle[i].paenchandle->aenchandle), &(pahandle[i].paenchandle->aenc_create_param));
			OSA_assert(status == OSA_SOK);
			pahandle[i].paenchandle->inbuf_size = (MAX_IN_SAMPLES * SAMPLE_LEN * pahandle[i].paenchandle->aenc_create_param.numberOfChannels);
			printf("pahandle[%d].paenchandle->inbuf_node[index].addr=%d,%d,%d\n", i, pahandle[i].paenchandle->inbuf_size,pahandle[i].paenchandle->aenc_create_param.minInBufSize,pahandle[i].paenchandle->aenc_create_param.minOutBufSize);
			if(pahandle[i].paenchandle->inbuf_size < pahandle[i].paenchandle->aenc_create_param.minInBufSize){
				pahandle[i].paenchandle->inbuf_size = pahandle[i].paenchandle->aenc_create_param.minInBufSize;
			}

			pahandle[i].paenchandle->outbuf_size = MAX_OUTPUT_BUFFER;
			if(pahandle[i].paenchandle->outbuf_size < pahandle[i].paenchandle->aenc_create_param.minOutBufSize){
				pahandle[i].paenchandle->outbuf_size = pahandle[i].paenchandle->aenc_create_param.minOutBufSize;
			}
		}
		else{
			OSA_printf("ppahandle2 not enc and not dec, what you want to do?\n");
			goto cleanup;
		}
	}
	/*

	if(ppahandle->inst_type == AAC_ENC){
		memcpy(&(ppahandle->aenc_create_param), &(param->aenc_create_param), sizeof(AENC_CREATE_PARAMS_S));
		AENC_CREATE_PARAMS_S *paacenc_params = &(ppahandle->aenc_create_param);

		status = audio_aacenc_create(&ppahandle->aenchandle, paacenc_params);
		OSA_assert(status == OSA_SOK);

		ppahandle->inbuf_size = (MAX_IN_SAMPLES * SAMPLE_LEN * paacenc_params->numberOfChannels);
		if(ppahandle->inbuf_size < paacenc_params->minInBufSize){
			ppahandle->inbuf_size = paacenc_params->minInBufSize;
		}

		ppahandle->outbuf_size = MAX_OUTPUT_BUFFER;
		if(ppahandle->outbuf_size < paacenc_params->minOutBufSize){
			ppahandle->outbuf_size = paacenc_params->minOutBufSize;
		}
	}
	else if(ppahandle->inst_type == AAC_DEC){
		memcpy(&(ppahandle->adec_create_param), &(param->adec_create_param), sizeof(ADEC_CREATE_PARAMS_S));
		ADEC_CREATE_PARAMS_S *paacdec_params = &(ppahandle->adec_create_param);

		status = audio_aacdec_create(&ppahandle->adechandle, paacdec_params);
		OSA_assert(status == OSA_SOK);

		ppahandle->inbuf_size = MAX_INPUT_BUFFER;
		if(ppahandle->inbuf_size < paacdec_params->minInBufSize){
			ppahandle->inbuf_size = paacdec_params->minInBufSize;
		}

		ppahandle->outbuf_size = MAX_OUTPUT_BUFFER;
		if(ppahandle->outbuf_size < paacdec_params->minOutBufSize){
			ppahandle->outbuf_size = paacdec_params->minOutBufSize;
		}
	}
	else{
		OSA_printf("not enc and not dec, what you want to do?\n");
		goto cleanup;
	}
	*/
	///audio_allocate_sharedregionbuf(4096*12);
	for(i=0;i<num_inst;i++)
	{
			/* 创建输入队列 */
		status = OSA_queCreate(&(pahandle[i].paenchandle->indata_user_handle.empty_que), AUDIO_ENCDEC_INBUF_QUE_MAX);
		OSA_assert(status == OSA_SOK);
		status = OSA_queCreate(&(pahandle[i].paenchandle->indata_user_handle.full_que), AUDIO_ENCDEC_INBUF_QUE_MAX);
		OSA_assert(status == OSA_SOK);

		for(index = 0; index < AUDIO_ENCDEC_INBUF_QUE_MAX; index++){
			pahandle[i].paenchandle->inbuf_node[index].addr = audio_allocate_sharedregionbuf(pahandle[i].paenchandle->inbuf_size);
			pahandle[i].paenchandle->inbuf_node[index].buf_size = pahandle[i].paenchandle->inbuf_size;

			status = OSA_quePut(&(pahandle[i].paenchandle->indata_user_handle.empty_que),
			                    (Int32) &(pahandle[i].paenchandle->inbuf_node[index]),
			                    OSA_TIMEOUT_NONE);
			OSA_assert(status == OSA_SOK);
		}


		/* 创建输出队列 */
		status = OSA_queCreate(&(pahandle[i].paenchandle->outdata_user_handle.full_que), AUDIO_ENCDEC_OUTBUF_QUE_MAX);
		OSA_assert(status == OSA_SOK);
		status = OSA_queCreate(&(pahandle[i].paenchandle->outdata_user_handle.empty_que), AUDIO_ENCDEC_OUTBUF_QUE_MAX);
		OSA_assert(status == OSA_SOK);
		for(index = 0; index < AUDIO_ENCDEC_OUTBUF_QUE_MAX; index++){
			pahandle[i].paenchandle->outbuf_node[index].addr = audio_allocate_sharedregionbuf(pahandle[i].paenchandle->outbuf_size);
			pahandle[i].paenchandle->outbuf_node[index].buf_size = pahandle[i].paenchandle->outbuf_size;

			status = OSA_quePut(&(pahandle[i].paenchandle->outdata_user_handle.full_que),
			                    (Int32) &(pahandle[i].paenchandle->outbuf_node[index]),
			                    OSA_TIMEOUT_NONE);
			OSA_assert(status == OSA_SOK);
		}
	}
		/*
	status = OSA_queCreate(&(ppahandle2->indata_user_handle.empty_que), AUDIO_ENCDEC_INBUF_QUE_MAX);
	OSA_assert(status == OSA_SOK);
	status = OSA_queCreate(&(ppahandle2->indata_user_handle.full_que), AUDIO_ENCDEC_INBUF_QUE_MAX);
	OSA_assert(status == OSA_SOK);
	for(index = 0; index < AUDIO_ENCDEC_INBUF_QUE_MAX; index++){
		ppahandle2->inbuf_node[index].addr = audio_allocate_sharedregionbuf(ppahandle2->inbuf_size);
		ppahandle2->inbuf_node[index].buf_size = ppahandle2->inbuf_size;

		status = OSA_quePut(&(ppahandle2->indata_user_handle.empty_que),
		                    (Int32) &(ppahandle2->inbuf_node[index]),
		                    OSA_TIMEOUT_NONE);
		OSA_assert(status == OSA_SOK);
	}


	 创建第二路输出队列
	status = OSA_queCreate(&(ppahandle2->outdata_user_handle.full_que), AUDIO_ENCDEC_OUTBUF_QUE_MAX);
	OSA_assert(status == OSA_SOK);
	status = OSA_queCreate(&(ppahandle2->outdata_user_handle.empty_que), AUDIO_ENCDEC_INBUF_QUE_MAX);
	OSA_assert(status == OSA_SOK);
	for(index = 0; index < AUDIO_ENCDEC_OUTBUF_QUE_MAX; index++){
		ppahandle2->outbuf_node[index].addr = audio_allocate_sharedregionbuf(ppahandle2->outbuf_size);
		ppahandle2->outbuf_node[index].buf_size = ppahandle2->outbuf_size;

		status = OSA_quePut(&(ppahandle2->outdata_user_handle.full_que),
		                    (Int32) &(ppahandle2->outbuf_node[index]),
		                    OSA_TIMEOUT_NONE);
		OSA_assert(status == OSA_SOK);
	}

	*/
	/* 创建音频编码线程*/

	for(i=0;i<num_inst;i++)
	{
		pahandle[i].paenchandle->run_status = TRUE;
	}
//	printf("audio_encThread = %p\n",audio_encThread);
	printf_pthread_create(__FILE__, ("audio_encThread"));
	status = OSA_thrCreate(&(pahandle[0].paenchandle->indata_thr),
	                       (OSA_ThrEntryFunc)audio_encThread,
	                       AUDIO_INDATA_GET_FXN_TSK_PRI,
	                       AUDIO_INDATA_GET_FXN_TSK_STACK_SIZE,
	                       pahandle);
	OSA_assert(status == 0);


	/* 创建采集音频线程 */
	if(user_indata_get_fxn != NULL ){
		pahandle[0].paenchandle->indata_user_handle.run_status = TRUE;
		printf_pthread_create(__FILE__, ("pahandle[0].paenchandle->user_indata_get_fxn"));
		status = OSA_thrCreate(&(pahandle[0].paenchandle->user_indata_thr),
		                       (OSA_ThrEntryFunc)pahandle[0].paenchandle->user_indata_get_fxn,
		                       AUDIO_INDATA_GET_FXN_TSK_PRI,
		                       AUDIO_INDATA_GET_FXN_TSK_STACK_SIZE,
		                       (pahandle));
		OSA_assert(status == 0);

	}

	/* 创建发送给encodermanager线程 */
	if(user_outdata_put_fxn != NULL){
		pahandle[0].paenchandle->outdata_user_handle.run_status = TRUE;
		printf_pthread_create(__FILE__, ("pahandle[0].paenchandle->user_outdata_put_fxn"));
		status = OSA_thrCreate(&(pahandle[0].paenchandle->user_outdata_thr),
		                       (OSA_ThrEntryFunc)pahandle[0].paenchandle->user_outdata_put_fxn,
		                       AUDIO_INDATA_GET_FXN_TSK_PRI,
		                       AUDIO_INDATA_GET_FXN_TSK_STACK_SIZE,
		                       pahandle);
		OSA_assert(status == 0);

	}

	return 0;

cleanup:

	pahandle[0].paenchandle->run_status = FALSE;

	if(pahandle[0].paenchandle != NULL)
		free(pahandle[0].paenchandle);

	return -1;
}

Int32 audio_dec_create_inst(audio_struct *pahandle, audio_encdec_params *param,
                               Void *user_indata_get_fxn, Void *user_outdata_put_fxn)
{
	Int32 status = -1;
	Int32 index = 0;
	audio_encdec_handle *ppahandle = NULL;


	if((pahandle == NULL) || (param == NULL))
	{
		OSA_printf("audio_dec_create_inst error, handle or params is NULL!\n");
		return -1;
	}

	if(param->inst_type != AAC_DEC)
	{
		OSA_printf("audio_dec_create_inst: this is aac dec inst, not enc inst!\n");
		return -1;
	}

	pahandle->paenchandle = (audio_encdec_handle *)malloc(sizeof(audio_encdec_handle));
	ppahandle = pahandle->paenchandle;
	if(ppahandle == NULL)
	{
		OSA_printf("audio_capture_init: malloc handle faile!\n");
		return -1;
	}

	ppahandle->inst_type = param->inst_type;
	ppahandle->user_indata_get_fxn = user_indata_get_fxn;
	ppahandle->user_outdata_put_fxn = user_outdata_put_fxn;

	/* 创建编码实例 */
	memcpy(&(ppahandle->adec_create_param), &(param->adec_create_param), sizeof(ADEC_CREATE_PARAMS_S));
	ADEC_CREATE_PARAMS_S *paacdec_params = &(ppahandle->adec_create_param);

	status = audio_aacdec_create(&ppahandle->adechandle, paacdec_params);
	OSA_assert(status == OSA_SOK);

	ppahandle->inbuf_size = MAX_INPUT_BUFFER;
	if(ppahandle->inbuf_size < paacdec_params->minInBufSize)
	{
		ppahandle->inbuf_size = paacdec_params->minInBufSize;
	}

	ppahandle->outbuf_size = MAX_OUTPUT_BUFFER;
	if(ppahandle->outbuf_size < paacdec_params->minOutBufSize)
	{
		ppahandle->outbuf_size = paacdec_params->minOutBufSize;
	}

	/* 创建输入队列 */
	status = OSA_queCreate(&(ppahandle->indata_user_handle.empty_que), AUDIO_ENCDEC_INBUF_QUE_MAX);
	OSA_assert(status == OSA_SOK);
	status = OSA_queCreate(&(ppahandle->indata_user_handle.full_que), AUDIO_ENCDEC_INBUF_QUE_MAX);
	OSA_assert(status == OSA_SOK);
	for(index = 0; index < AUDIO_ENCDEC_INBUF_QUE_MAX; index++)
	{
		ppahandle->inbuf_node[index].addr = audio_allocate_sharedregionbuf(ppahandle->inbuf_size);
		ppahandle->inbuf_node[index].buf_size = ppahandle->inbuf_size;

		status = OSA_quePut(&(ppahandle->indata_user_handle.empty_que),
							(Int32) &(ppahandle->inbuf_node[index]),
							OSA_TIMEOUT_NONE);
		OSA_assert(status == OSA_SOK);
	}


	/* 创建输出队列 */
	status = OSA_queCreate(&(ppahandle->outdata_user_handle.full_que), AUDIO_ENCDEC_OUTBUF_QUE_MAX);
	OSA_assert(status == OSA_SOK);

	status = OSA_queCreate(&(ppahandle->outdata_user_handle.empty_que), AUDIO_ENCDEC_INBUF_QUE_MAX);
	OSA_assert(status == OSA_SOK);
	for(index = 0; index < AUDIO_ENCDEC_OUTBUF_QUE_MAX; index++)
	{
		ppahandle->outbuf_node[index].addr = audio_allocate_sharedregionbuf(ppahandle->outbuf_size);
		ppahandle->outbuf_node[index].buf_size = ppahandle->outbuf_size;

		status = OSA_quePut(&(ppahandle->outdata_user_handle.full_que),
							(Int32) &(ppahandle->outbuf_node[index]),
							OSA_TIMEOUT_NONE);
		OSA_assert(status == OSA_SOK);
	}


	/* 创建处理任务 */
	ppahandle->run_status = TRUE;
	printf_pthread_create(__FILE__, ("audio_decThread"));
	status = OSA_thrCreate(&(ppahandle->indata_thr),
						   (OSA_ThrEntryFunc)audio_decThread,
						   AUDIO_INDATA_GET_FXN_TSK_PRI,
						   AUDIO_INDATA_GET_FXN_TSK_STACK_SIZE,
						   pahandle);
	OSA_assert(status == 0);

	/* 创建接收任务 */
	if(user_indata_get_fxn != NULL )
	{
		ppahandle->indata_user_handle.run_status = TRUE;
		printf_pthread_create(__FILE__, ("ppahandle->user_indata_get_fxn"));
		status = OSA_thrCreate(&(ppahandle->user_indata_thr),
							   (OSA_ThrEntryFunc)ppahandle->user_indata_get_fxn,
							   AUDIO_INDATA_GET_FXN_TSK_PRI,
							   AUDIO_INDATA_GET_FXN_TSK_STACK_SIZE,
							   &(ppahandle->indata_user_handle));
		OSA_assert(status == 0);
	}

	/* 创建发送任务 */
	if(user_outdata_put_fxn != NULL)
	{
		ppahandle->outdata_user_handle.run_status = TRUE;
		printf_pthread_create(__FILE__, ("ppahandle->user_outdata_put_fxn"));
		status = OSA_thrCreate(&(ppahandle->user_outdata_thr),
							   (OSA_ThrEntryFunc)ppahandle->user_outdata_put_fxn,
							   AUDIO_INDATA_GET_FXN_TSK_PRI,
							   AUDIO_INDATA_GET_FXN_TSK_STACK_SIZE,
							   pahandle);
		OSA_assert(status == 0);
	}

	return 0;

cleanup:

	ppahandle->run_status = FALSE;

	if(ppahandle != NULL)
		free(ppahandle);

	return -1;
}



Int32 audio_indata_get_empty_buf(data_user_param *param, audio_buf **ppabuf)
{
	Int32 status = -1;

	if((param == NULL ) || (ppabuf == NULL)){
		OSA_printf("audio_indata_get_empty_buf error, params is NULL!\n");
		return -1;
	}

	status = OSA_queGet(&(param->empty_que), (Int32 *)(ppabuf), OSA_TIMEOUT_FOREVER);
	OSA_assert(status == 0);

	return 0;
}

Int32 audio_indata_put_full_buf(data_user_param *param, audio_buf *pabuf)
{
	Int32 status = -1;

	if((param == NULL ) || (pabuf == NULL)){
		OSA_printf("audio_indata_put_full_buf error, params is NULL!\n");
		return -1;
	}

	status = OSA_quePut(&(param->full_que), (Int32)pabuf, OSA_TIMEOUT_NONE);
	OSA_assert(status == 0);

	return 0;
}


Int32 audio_outdata_get_full_buf(data_user_param *param, audio_buf **ppabuf)
{
	Int32 status = -1;

	if((param == NULL ) || (ppabuf == NULL)){
		OSA_printf("audio_outdata_get_full_buf error, params is NULL!\n");
		return -1;
	}

	status = OSA_queGet(&(param->full_que), (Int32 *)(ppabuf), OSA_TIMEOUT_FOREVER);
	OSA_assert(status == 0);

	return 0;
}


Int32 audio_outdata_put_empty_buf(data_user_param *param, audio_buf *pabuf)
{
	Int32 status = -1;

	if((param == NULL ) || (pabuf == NULL)){
		OSA_printf("audio_outdata_put_empty_buf error, params is NULL!\n");
		return -1;
	}

	status = OSA_quePut(&(param->empty_que), (Int32)pabuf, OSA_TIMEOUT_NONE);
	OSA_assert(status == 0);

	return 0;
}

Int32 audio_encdec_delete_inst(Void **ppahandle)
{
	Int32 index = 0;

	if((ppahandle == NULL) || (*ppahandle == NULL)){
		OSA_printf("audio_encdec_delete_inst error, handle is NULL!\n");
		return -1;
	}

	audio_encdec_handle *phandle = (audio_encdec_handle *)(*ppahandle);

	phandle->run_status = FALSE;
	phandle->indata_user_handle.run_status = FALSE;
	phandle->outdata_user_handle.run_status = FALSE;

	OSA_queDelete(&(phandle->indata_user_handle.empty_que));
	OSA_queDelete(&(phandle->indata_user_handle.full_que));
	OSA_queDelete(&(phandle->outdata_user_handle.full_que));
	OSA_queDelete(&(phandle->outdata_user_handle.empty_que));

	OSA_thrJoin(&(phandle->user_indata_thr));
	OSA_thrJoin(&(phandle->user_outdata_thr));
	OSA_thrJoin(&(phandle->indata_thr));

	if(phandle->inst_type == AAC_ENC){
		audio_aacenc_delete(phandle->aenchandle);
	}
	else if(phandle->inst_type == AAC_DEC){
		audio_aacdec_delete(phandle->adechandle);
	}

	for(index = 0; index < AUDIO_ENCDEC_INBUF_QUE_MAX; index++){
		audio_free_sharedregionbuf(phandle->inbuf_node[index].addr,
									phandle->inbuf_node[index].buf_size);
	}

	for(index = 0; index < AUDIO_ENCDEC_OUTBUF_QUE_MAX; index++){
		phandle->outbuf_node[index].buf_size = phandle->outbuf_size;
		audio_free_sharedregionbuf(phandle->outbuf_node[index].addr,
									phandle->outbuf_node[index].buf_size);
	}

	free(phandle);

	*ppahandle = NULL;

	return 0;
}




