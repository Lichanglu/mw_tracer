#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "reach_system_priv.h"
#include "reach_capture_priv.h"


#include <device.h>
#include <device_videoDecoder.h>

#include <device_adv7441.h>
#include <device_adv7442.h>
#include <device_adv7619.h>
#include <device_gs2971.h>
#include <device_gs2972.h>
#include <device_tvp5158.h>
#include <mcfw/src_linux/devices/adv7441/src/adv7441_priv.h>
#include <mcfw/src_linux/devices/adv7442/src/adv7442_priv.h>
#include <mcfw/src_linux/devices/adv7619/src/adv7619_priv.h>
#include <mcfw/src_linux/devices/gs2971/src/gs2971_priv.h>
#include <mcfw/src_linux/devices/gs2972/src/gs2972_priv.h>
#include <mcfw/src_linux/devices/tvp5158/src/tvp5158_priv.h>
#include <adv7441.h>
#include <adv7442.h>
#include <adv7619.h>
#include <gs2971.h>
#include <gs2972.h>
#include <tvp5158.h>
#include <assert.h>

struct R_GPIO_data{
	unsigned int gpio_num;
	unsigned int gpio_value;
};

int AD7441Init(void)
{
	Int32 								status = 0;
	int 									gpio_fd;
	int 									counts;
	struct R_GPIO_data 					data;
	Device_VideoDecoderCreateParams		createArgs;
	Device_VideoDecoderCreateStatus		createStatusArgs;
	VCAP_VIDEO_SOURCE_STATUS_PARAMS_S	videoStatusArgs;
	VCAP_VIDEO_SOURCE_CH_STATUS_S		videoStatus;
	Device_Adv7441Handle handle;
#if 0
	gpio_fd = open("/dev/Rgpio",O_RDWR | O_SYNC);
	data.gpio_num = 39;data.gpio_value = 0;
	ioctl(gpio_fd, 0x55555555, &data);
	data.gpio_num = 40;data.gpio_value = 0;
	ioctl(gpio_fd, 0x55555555, &data);
	printf("Set GPIO to Low!\n");
	sleep(1);
//	close(gpio_fd);
#endif

	Device_adv7441Init();
	memset(&createArgs, 0, sizeof(Device_VideoDecoderCreateParams));

	createArgs.deviceI2cInstId	= 1;
	createArgs.numDevicesAtPort	= 1;
	createArgs.deviceI2cAddr[0] = 0;
	createArgs.deviceResetGpio[0] = DEVICE_VIDEO_DECODER_GPIO_NONE;
	handle = (Void *)Device_adv7441Create(0, 0, &(createArgs), &(createStatusArgs));
	videoStatusArgs.channelNum = 0;
	videoStatus.isVideoDetect = FALSE;
	counts = 0;
	while(1) {
		counts++;
		status = Device_adv7441Control(handle,
										IOCTL_DEVICE_VIDEO_DECODER_GET_VIDEO_STATUS,
										&videoStatusArgs, &videoStatus);
		if(videoStatus.isVideoDetect == TRUE){
			printf("Get Source For Input! videoStatus.frameWidth = %d, videoStatus.frameHeight=%d\n",videoStatus.frameWidth,videoStatus.frameHeight);
			break;
		}

		sleep(1);
		if(counts > 10){
			printf("Get Source For Input Failed!\n");
			status = -1;
			break;
		}
	}

	return status;
}

int AD7442Init(void)
{
	Int32 								status = 0;
	int 									gpio_fd;
	int 									counts;
	struct R_GPIO_data 					data;
	Device_VideoDecoderCreateParams		createArgs;
	Device_VideoDecoderCreateStatus		createStatusArgs;
	VCAP_VIDEO_SOURCE_STATUS_PARAMS_S	videoStatusArgs;
	VCAP_VIDEO_SOURCE_CH_STATUS_S		videoStatus;
	Device_Adv7442Handle handle;
#if 0
	gpio_fd = open("/dev/Rgpio",O_RDWR | O_SYNC);
	data.gpio_num = 39;data.gpio_value = 0;
	ioctl(gpio_fd, 0x55555555, &data);
	data.gpio_num = 40;data.gpio_value = 0;
	ioctl(gpio_fd, 0x55555555, &data);
//	close(gpio_fd);
#endif
	Device_adv7442Init();
	memset(&createArgs, 0, sizeof(Device_VideoDecoderCreateParams));

	createArgs.deviceI2cInstId	= 1;
	createArgs.numDevicesAtPort	= 1;
	createArgs.deviceI2cAddr[0] = 0;
	createArgs.deviceResetGpio[0] = DEVICE_VIDEO_DECODER_GPIO_NONE;
	handle = (Void *)Device_adv7442Create(0, 0, &(createArgs), &(createStatusArgs));
	videoStatusArgs.channelNum = 0;
	videoStatus.isVideoDetect = FALSE;
	counts = 0;
	while(1) {
		counts++;
		status = Device_adv7442Control(handle,
										IOCTL_DEVICE_VIDEO_DECODER_GET_VIDEO_STATUS,
										&videoStatusArgs, &videoStatus);
		if(videoStatus.isVideoDetect == TRUE){
			printf("Get Source For Input!\n");
			break;
		}

		sleep(1);
		if(counts > 10){
			printf("Get Source For Input Failed!\n");
			status = -1;
			break;
		}
	}

	return status;
}

int GS2971Init(void)
{
	Int32 								status = 0;
	int 									gpio_fd;
	int 									counts;
	struct R_GPIO_data 					data;
	Device_VideoDecoderCreateParams		createArgs;
	Device_VideoDecoderCreateStatus		createStatusArgs;
	VCAP_VIDEO_SOURCE_STATUS_PARAMS_S	videoStatusArgs;
	VCAP_VIDEO_SOURCE_CH_STATUS_S		videoStatus;
	Device_Gs2971Handle handle;

	gpio_fd = open("/dev/Rgpio",O_RDWR | O_SYNC);
	data.gpio_num = 39;data.gpio_value = 1;
	ioctl(gpio_fd, 0x55555555, &data);
	data.gpio_num = 40;data.gpio_value = 1;
	ioctl(gpio_fd, 0x55555555, &data);
	close(gpio_fd);

	Device_gs2971Init();
	memset(&createArgs, 0, sizeof(Device_VideoDecoderCreateParams));

	createArgs.deviceI2cInstId	= 1;
	createArgs.numDevicesAtPort	= 1;
	createArgs.deviceI2cAddr[0] = 0;
	createArgs.deviceResetGpio[0] = DEVICE_VIDEO_DECODER_GPIO_NONE;
	handle = (Void *)Device_gs2971Create(0, 0, &(createArgs), &(createStatusArgs));
	videoStatusArgs.channelNum = 0;
	videoStatus.isVideoDetect = FALSE;
	counts = 0;
	while(1) {
		counts++;
		status = Device_gs2971Control(handle,
										IOCTL_DEVICE_VIDEO_DECODER_GET_VIDEO_STATUS,
										&videoStatusArgs, &videoStatus);
		if(videoStatus.isVideoDetect == TRUE){
			printf("Get Source For Input!\n");
			break;
		}

		sleep(1);
		if(counts > 10){
			printf("Get Source For Input Failed!\n");
			status = -1;
			break;
		}
	}

	return status;
}

int GS2972Init(void)
{
	Int32 								status = 0;
	int 									gpio_fd;
	int 									counts;
	struct R_GPIO_data 					data;
	Device_VideoDecoderCreateParams		createArgs;
	Device_VideoDecoderCreateStatus		createStatusArgs;
	VCAP_VIDEO_SOURCE_STATUS_PARAMS_S	videoStatusArgs;
	VCAP_VIDEO_SOURCE_CH_STATUS_S		videoStatus;
	Device_Gs2971Handle handle;

	gpio_fd = open("/dev/Rgpio",O_RDWR | O_SYNC);
	data.gpio_num = 39;data.gpio_value = 1;
	ioctl(gpio_fd, 0x55555555, &data);
	data.gpio_num = 40;data.gpio_value = 1;
	ioctl(gpio_fd, 0x55555555, &data);
	close(gpio_fd);

	Device_gs2972Init();
	memset(&createArgs, 0, sizeof(Device_VideoDecoderCreateParams));

	createArgs.deviceI2cInstId	= 1;
	createArgs.numDevicesAtPort	= 1;
	createArgs.deviceI2cAddr[0] = 0;
	createArgs.deviceResetGpio[0] = DEVICE_VIDEO_DECODER_GPIO_NONE;
	handle = (Void *)Device_gs2972Create(0, 0, &(createArgs), &(createStatusArgs));
	videoStatusArgs.channelNum = 0;
	videoStatus.isVideoDetect = FALSE;
	counts = 0;
	while(1) {
		counts++;
		status = Device_gs2972Control(handle,
										IOCTL_DEVICE_VIDEO_DECODER_GET_VIDEO_STATUS,
										&videoStatusArgs, &videoStatus);
		if(videoStatus.isVideoDetect == TRUE){
			printf("Get Source For Input!\n");
			break;
		}

		sleep(1);
		if(counts > 10){
			printf("Get Source For Input Failed!\n");
			status = -1;
			break;
		}
	}

	return status;
}

void cap_init_create_param(CaptureLink_CreateParams *prm)
{
	return CaptureLink_CreateParams_Init(prm);
}

Int32 cap_get_video_status(Uint32 captureId, VCAP_VIDEO_SOURCE_STATUS_S *pStatus)
{
	Int status = -1;

//#ifdef SYSTEM_USE_VIDEO_DECODER
	if(captureId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(captureId,
								CAPTURE_LINK_CMD_GET_VIDEO_STATUS,
								pStatus,
								sizeof(*pStatus),
								TRUE
								);
	}
//#endif

	return status;
}

Int32 cap_detect_video(Uint32 captureId)
{
    Int status = -1;

    if(captureId != SYSTEM_LINK_ID_INVALID) {
        status = System_linkControl(captureId,
                                	CAPTURE_LINK_CMD_DETECT_VIDEO,
                                	NULL,
                                	0,
                                	TRUE
                                	);
    }

    return status;
}

Int32 cap_config_videodecoder(Uint32 captureId)
{
	Int status = -1;


	if(captureId != SYSTEM_LINK_ID_INVALID) {
//#ifdef SYSTEM_USE_VIDEO_DECODER
		status = System_linkControl(captureId,
									CAPTURE_LINK_CMD_CONFIGURE_VIP_DECODERS,
									NULL,
									0,
									TRUE
									);
//#endif
	}

	return status;
}

Int32 cap_set_extra_frameschId(Uint32 captureId, UInt32 chId)
{
    CaptureLink_ExtraFramesChId  prm;
    Int status = -1;

    prm.queId = 0;
    prm.chId  = chId;

	if(captureId != SYSTEM_LINK_ID_INVALID) {
	    status = System_linkControl(captureId,
						            CAPTURE_LINK_CMD_SET_EXTRA_FRAMES_CH_ID,
						            &prm,
						            sizeof(prm),
						            TRUE
						            );
	}

    return status;
}

Int32 cap_skip_alternate_frames(Uint32 captureId, UInt32 queId, UInt32 chId, UInt32 skipMask)
{
    CaptureLink_SkipAlternateFrames  prm;
    Int status = -1;

    prm.queId = queId;
    prm.skipAlternateFramesChBitMask = skipMask;

	if(captureId != SYSTEM_LINK_ID_INVALID) {
	    status = System_linkControl(captureId,
						            CAPTURE_LINK_CMD_SKIP_ALTERNATE_FRAMES,
						            &prm,
						            sizeof(prm),
						            FALSE
						            );
	}

    return status;
}

#if 0
Int32 cap_set_minni_framerate(Uint32 captureId, UInt32 queId, UInt32 chId, UInt32 framerate)
{
    CaptureLink_SetChannelFramerate  prm;
    Int status = -1;

    prm.queId = queId;
    prm.chId = chId;
	prm.framerate = framerate;

	if(captureId != SYSTEM_LINK_ID_INVALID) {
	    status = System_linkControl(captureId,
						            CAPTURE_LINK_CMD_SET_OUTPUT_FRAMERATE,
						            &prm,
						            sizeof(prm),
						            FALSE
						            );
	}

    return status;
}
#endif



Int32 cap_print_advstatistics(Uint32 captureId)
{
    Int status = -1;

    if(captureId != SYSTEM_LINK_ID_INVALID) {
        status = System_linkControl(captureId,
                            		CAPTURE_LINK_CMD_PRINT_ADV_STATISTICS,
                            		NULL,
                            		0,
                            		TRUE
                            		);
    }

	return status;
}

Int32 cap_print_bufferstatistics(Uint32 captureId)
{
	Int status = -1;

    if(captureId != SYSTEM_LINK_ID_INVALID) {
        status = System_linkControl(captureId,
                            		CAPTURE_LINK_CMD_PRINT_BUFFER_STATISTICS,
                            		NULL,
                            		0,
                            		TRUE
                            		);
    }

	return status;
}


Int32 cap_config_adv7441(Void **phandle, UInt8 instId,
									VCAP_VIDEO_SOURCE_CH_STATUS_S *pVideoStatus)
{
	Int32 status = 0;
	Device_VideoDecoderCreateParams		createArgs;
	Device_VideoDecoderCreateStatus		createStatusArgs;
	VCAP_VIDEO_SOURCE_STATUS_PARAMS_S	videoStatusArgs;
	VCAP_VIDEO_SOURCE_CH_STATUS_S		videoStatus;

	Device_Adv7441Handle *handle = (Device_Adv7441Handle *)phandle;

	Device_adv7441Init();

	memset(&createArgs, 0, sizeof(Device_VideoDecoderCreateParams));

	createArgs.deviceI2cInstId	= 1;
	createArgs.numDevicesAtPort	= 1;
	createArgs.deviceI2cAddr[instId] = 0;
	createArgs.deviceResetGpio[0] = DEVICE_VIDEO_DECODER_GPIO_NONE;

	*handle = (Void *)Device_adv7441Create(0, instId, &(createArgs), &(createStatusArgs));
	videoStatusArgs.channelNum = 0;

	status = Device_adv7441Control(*handle,
									IOCTL_DEVICE_VIDEO_DECODER_GET_VIDEO_STATUS,
									&videoStatusArgs, &videoStatus);

	if(pVideoStatus != NULL)
		memcpy(pVideoStatus, &videoStatus, sizeof(VCAP_VIDEO_SOURCE_CH_STATUS_S));
	if(videoStatus.isVideoDetect) {
#if 0
        printf("\nCAPTURE: Detected video (%dx%d@%dHz, %d)!!!\n",
                   videoStatus.frameWidth,
                   videoStatus.frameHeight,
                   1000000 / videoStatus.frameInterval,
                   videoStatus.isInterlaced);

        if(videoStatus.frameHeight == 288)
            (*handle)->isPalMode = TRUE;
#endif
    }
    else {
        OSA_printf("\nCAPTURE ERROR: Could not detect video at adv7441 !!!\n");
		return -1;
    }

	return 0;
}

Int32 cap_config_adv7442(Void **phandle, UInt8 instId,
									VCAP_VIDEO_SOURCE_CH_STATUS_S *pVideoStatus)
{
	Int32 status = 0;
	Device_VideoDecoderCreateParams		createArgs;
	Device_VideoDecoderCreateStatus		createStatusArgs;
	VCAP_VIDEO_SOURCE_STATUS_PARAMS_S	videoStatusArgs;
	VCAP_VIDEO_SOURCE_CH_STATUS_S		videoStatus;
	Device_Adv7442Handle *handle = (Device_Adv7442Handle *)phandle;

	Device_adv7442Init();
	memset(&createArgs, 0, sizeof(Device_VideoDecoderCreateParams));

	createArgs.deviceI2cInstId	= 1;
	createArgs.numDevicesAtPort	= 1;
	createArgs.deviceI2cAddr[instId] = 0;
	createArgs.deviceResetGpio[0] = DEVICE_VIDEO_DECODER_GPIO_NONE;

	*handle = (Void *)Device_adv7442Create(0, instId, &createArgs, &createStatusArgs);
	videoStatusArgs.channelNum = 0;
	status = Device_adv7442Control(*handle,
									IOCTL_DEVICE_VIDEO_DECODER_GET_VIDEO_STATUS,
									&videoStatusArgs, &videoStatus);
	if(pVideoStatus != NULL)
		memcpy(pVideoStatus, &videoStatus, sizeof(VCAP_VIDEO_SOURCE_CH_STATUS_S));
    if(videoStatus.isVideoDetect) {
#if 0
        printf("\nCAPTURE: Detected video (%dx%d@%dHz, %d)!!!\n",
                   videoStatus.frameWidth,
                   videoStatus.frameHeight,
                   1000000 / videoStatus.frameInterval,
                   videoStatus.isInterlaced);

        if(videoStatus.frameHeight == 288)
            (*handle)->isPalMode = TRUE;
#endif
    }
    else {
        OSA_printf("\nCAPTURE ERROR: Could not detect video at adv7442 !!!\n");
		return -1;
    }

	return 0;
}

Int32 cap_config_gs2971(Void **phandle, UInt8 instId,
									VCAP_VIDEO_SOURCE_CH_STATUS_S *pVideoStatus)
{
	Int32 status = 0;
	Device_VideoDecoderCreateParams		createArgs;
	Device_VideoDecoderCreateStatus		createStatusArgs;
	VCAP_VIDEO_SOURCE_STATUS_PARAMS_S	videoStatusArgs;
	VCAP_VIDEO_SOURCE_CH_STATUS_S		videoStatus;
	Device_Gs2971Handle *handle = (Device_Gs2971Handle *)phandle;

	Device_gs2971Init();

	memset(&createArgs, 0, sizeof(Device_VideoDecoderCreateParams));

	createArgs.deviceI2cInstId	= 1;
	createArgs.numDevicesAtPort	= 1;
	createArgs.deviceI2cAddr[instId] = 0;
	createArgs.deviceResetGpio[0] = DEVICE_VIDEO_DECODER_GPIO_NONE;

	*handle = Device_gs2971Create(0, instId, &(createArgs), &(createStatusArgs));

	videoStatusArgs.channelNum = 0;
	status = Device_gs2971Control(*handle,
									IOCTL_DEVICE_VIDEO_DECODER_GET_VIDEO_STATUS,
									&videoStatusArgs, &videoStatus);

	if(pVideoStatus != NULL)
		memcpy(pVideoStatus, &videoStatus, sizeof(VCAP_VIDEO_SOURCE_CH_STATUS_S));
    if(videoStatus.isVideoDetect) {
#if 0
        printf("\nCAPTURE: Detected video (%dx%d@%dHz, %d)!!!\n",
                   videoStatus.frameWidth,
                   videoStatus.frameHeight,
                   1000000 / videoStatus.frameInterval,
                   videoStatus.isInterlaced);

        if(videoStatus.frameHeight == 288)
            (*handle)->isPalMode = TRUE;
#endif
    }
    else {
        OSA_printf("\nCAPTURE ERROR: detect video at gs2971 mode = -1 !\n");
		return -1;
    }

	return 0;
}

Int32 cap_config_gs2972(Void **phandle, UInt8 instId,
									VCAP_VIDEO_SOURCE_CH_STATUS_S *pVideoStatus)
{
	Int32 status = 0;
	Device_VideoDecoderCreateParams		createArgs;
	Device_VideoDecoderCreateStatus		createStatusArgs;
	VCAP_VIDEO_SOURCE_STATUS_PARAMS_S	videoStatusArgs;
	VCAP_VIDEO_SOURCE_CH_STATUS_S		videoStatus;
	Device_Gs2972Handle *handle = (Device_Gs2972Handle *)phandle;

	Device_gs2972Init();
	memset(&createArgs, 0, sizeof(Device_VideoDecoderCreateParams));

	createArgs.deviceI2cInstId	= 1;
	createArgs.numDevicesAtPort	= 1;
	createArgs.deviceI2cAddr[instId] = 0;
	createArgs.deviceResetGpio[0] = DEVICE_VIDEO_DECODER_GPIO_NONE;

	*handle = Device_gs2972Create(0, instId, &(createArgs), &(createStatusArgs));
	videoStatusArgs.channelNum = 0;
	status = Device_gs2972Control(*handle,
									IOCTL_DEVICE_VIDEO_DECODER_GET_VIDEO_STATUS,
									&videoStatusArgs, &videoStatus);
	if(pVideoStatus != NULL)
		memcpy(pVideoStatus, &videoStatus, sizeof(VCAP_VIDEO_SOURCE_CH_STATUS_S));
    if(videoStatus.isVideoDetect) {
#if 0
        printf("\nCAPTURE: Detected video (%dx%d@%dHz, %d)!!!\n",
                   videoStatus.frameWidth,
                   videoStatus.frameHeight,
                   1000000 / videoStatus.frameInterval,
                   videoStatus.isInterlaced);

        if(videoStatus.frameHeight == 288)
            (*handle)->isPalMode = TRUE;
#endif
    }
    else {
        OSA_printf("\nCAPTURE ERROR: detect video at gs2972 mode = -1 !\n");
		return -1;
    }

	return 0;
}

Int32 cap_config_tvp5158(Void **phandle, UInt8 instId,
									VCAP_VIDEO_SOURCE_CH_STATUS_S *pVideoStatus)
{
	Int32 status = 0;
	Device_VideoDecoderCreateParams		createArgs;
	Device_VideoDecoderCreateStatus		createStatusArgs;
	VCAP_VIDEO_SOURCE_STATUS_PARAMS_S	videoStatusArgs;
	VCAP_VIDEO_SOURCE_CH_STATUS_S		videoStatus;

	Device_Tvp5158Handle *handle = (Device_Tvp5158Handle *)phandle;

	Device_tvp5158Init();

	memset(&createArgs, 0, sizeof(Device_VideoDecoderCreateParams));

	createArgs.deviceI2cInstId	= 1;
	createArgs.numDevicesAtPort	= 1;
	createArgs.deviceI2cAddr[instId] = 0;
	createArgs.deviceResetGpio[0] = DEVICE_VIDEO_DECODER_GPIO_NONE;

	*handle = (Void *)Device_tvp5158Create(0, instId, &(createArgs), &(createStatusArgs));
	videoStatusArgs.channelNum = 0;

	status = Device_tvp5158Control(*handle,
									IOCTL_DEVICE_VIDEO_DECODER_GET_VIDEO_STATUS,
									&videoStatusArgs, &videoStatus);

	if(pVideoStatus != NULL)
		memcpy(pVideoStatus, &videoStatus, sizeof(VCAP_VIDEO_SOURCE_CH_STATUS_S));
	if(videoStatus.isVideoDetect) {
#if 0
        printf("\nCAPTURE: Detected video (%dx%d@%dHz, %d)!!!\n",
                   videoStatus.frameWidth,
                   videoStatus.frameHeight,
                   1000000 / videoStatus.frameInterval,
                   videoStatus.isInterlaced);

        if(videoStatus.frameHeight == 288)
            (*handle)->isPalMode = TRUE;
#endif
    }
    else {
        OSA_printf("\nCAPTURE ERROR: Could not detect video at adv7441 !!!\n");
		return -1;
    }

	return 0;
}


/* just used for test */
Int32 cap_config_adv7619(Void **phandle, UInt8 instId,
									VCAP_VIDEO_SOURCE_CH_STATUS_S *pVideoStatus)
{
	Int32 status = 0;

	Device_VideoDecoderCreateParams 	createArgs;
	Device_VideoDecoderCreateStatus 	createStatusArgs;
	VCAP_VIDEO_SOURCE_STATUS_PARAMS_S	videoStatusArgs;
	VCAP_VIDEO_SOURCE_CH_STATUS_S		videoStatus;

	Device_Adv7619Handle *handle = (Device_Adv7619Handle *)phandle;

	Device_adv7619Init();
	memset(&createArgs, 0, sizeof(Device_VideoDecoderCreateParams));

	createArgs.deviceI2cInstId	= 1;
	createArgs.numDevicesAtPort = 1;
	createArgs.deviceI2cAddr[instId] = 0;
	createArgs.deviceResetGpio[0] = DEVICE_VIDEO_DECODER_GPIO_NONE;

	*handle = Device_adv7619Create(0, instId, &(createArgs), &(createStatusArgs));
	videoStatusArgs.channelNum = 0;
	status = Device_adv7619Control(*handle,
									IOCTL_DEVICE_VIDEO_DECODER_RESET,
									&videoStatusArgs, &videoStatus);

	status = Device_adv7619Control((Device_Adv7619Handle)(*handle),
									IOCTL_DEVICE_VIDEO_DECODER_GET_VIDEO_STATUS,
									&videoStatusArgs, &videoStatus);
	if(pVideoStatus != NULL)
		memcpy(pVideoStatus, &videoStatus, sizeof(VCAP_VIDEO_SOURCE_CH_STATUS_S));
#if 0
	while(!videoStatus.isVideoDetect) {
		sleep(1);
		status = Device_adv7619Control(*handle,
									IOCTL_DEVICE_VIDEO_DECODER_GET_VIDEO_STATUS,
									&videoStatusArgs, &videoStatus);
	}
#endif

	if(videoStatus.isVideoDetect) {
#if 0
		printf("\nCAPTURE: Detected video (%dx%d@%dHz)!!!\n",
				   videoStatus.frameWidth,
				   videoStatus.frameHeight,
		//		   1000000 / videoStatus.frameInterval,
				   videoStatus.isInterlaced);

		if(videoStatus.frameHeight == 288)
			(*handle)->isPalMode = TRUE;
#endif
	}
	else {
		OSA_printf("\nCAPTURE ERROR: detect video at adv7619 mode = -1 !\n");
		return -1;
	}

	return 0;

}

Int32 cap_get_7441_resolution(Void **phandle, VCAP_VIDEO_SOURCE_CH_STATUS_S *pvstatus)
{
	Int32 status = 0;

	VCAP_VIDEO_SOURCE_STATUS_PARAMS_S	videoStatusArgs;
	VCAP_VIDEO_SOURCE_CH_STATUS_S		videoStatus;

	Device_Adv7441Handle *handle = (Device_Adv7441Handle *)phandle;

	status = Device_adv7441Control(*handle,
									IOCTL_DEVICE_VIDEO_DECODER_GET_VIDEO_STATUS,
									&videoStatusArgs, &videoStatus);
	pvstatus->isVideoDetect = videoStatus.isVideoDetect;
    if(videoStatus.isVideoDetect) {
#if 0
        printf("\n adv7441 chip: detected video (%dx%d@%dHz, %d)!!!\n",
                   videoStatus.frameWidth,
                   videoStatus.frameHeight,
                   0,
                   videoStatus.isInterlaced);
#endif
		pvstatus->frameWidth = videoStatus.frameWidth;
		pvstatus->frameHeight = videoStatus.frameHeight;
		pvstatus->isInterlaced = videoStatus.isInterlaced;


        if(videoStatus.frameHeight == 288)
            (*handle)->isPalMode = TRUE;

    }
    else {
        OSA_printf("\n adv7441 chip: no video detected !\n");
		return -1;
    }

	return 0;
}

Int32 cap_get_7442_resolution(Void **phandle, VCAP_VIDEO_SOURCE_CH_STATUS_S *pvstatus)
{
	Int32 status = 0;

	VCAP_VIDEO_SOURCE_STATUS_PARAMS_S	videoStatusArgs;
	VCAP_VIDEO_SOURCE_CH_STATUS_S		videoStatus;

	Device_Adv7442Handle *handle = (Device_Adv7442Handle *)phandle;

	status = Device_adv7442Control(*handle,
									IOCTL_DEVICE_VIDEO_DECODER_GET_VIDEO_STATUS,
									&videoStatusArgs, &videoStatus);
	pvstatus->isVideoDetect = videoStatus.isVideoDetect;
    if(videoStatus.isVideoDetect) {
#if 1
        OSA_printf("\n adv7442 chip: detected video (%dx%d@%dHz, %d)!!!\n",
                   videoStatus.frameWidth,
                   videoStatus.frameHeight,
                   0,
                   videoStatus.isInterlaced);
#endif
		pvstatus->frameWidth = videoStatus.frameWidth;
		pvstatus->frameHeight = videoStatus.frameHeight;
		pvstatus->isInterlaced = videoStatus.isInterlaced;

        if(videoStatus.frameHeight == 288) {
            (*handle)->isPalMode = TRUE;
        }
        if(videoStatus.frameHeight > 1080) {
		pvstatus->isVideoDetect = 0;
		return -1;
	}
    }
    else {
        OSA_printf("\n adv7442 chip: no video detected !\n");
		return -1;
    }

	return 0;
}

Int32 cap_get_2971_resolution(Void **phandle, VCAP_VIDEO_SOURCE_CH_STATUS_S *pvstatus)
{
	Int32 status = 0;

	VCAP_VIDEO_SOURCE_STATUS_PARAMS_S	videoStatusArgs;
	VCAP_VIDEO_SOURCE_CH_STATUS_S		videoStatus;

	Device_Gs2971Handle *handle = (Device_Gs2971Handle *)phandle;

	status = Device_gs2971Control(handle,
									IOCTL_DEVICE_VIDEO_DECODER_GET_VIDEO_STATUS,
									&videoStatusArgs, &videoStatus);
	pvstatus->isVideoDetect = videoStatus.isVideoDetect;
    if(videoStatus.isVideoDetect) {
#if 0
        OSA_printf("\n gs2971 chip: detected video (%dx%d@%dHz, %d)!!!\n",
                   videoStatus.frameWidth,
                   videoStatus.frameHeight,
                   0,
                   videoStatus.isInterlaced);
#endif
		pvstatus->frameWidth = videoStatus.frameWidth;
		pvstatus->frameHeight = videoStatus.frameHeight;
		pvstatus->isInterlaced = videoStatus.isInterlaced;


        if(videoStatus.frameHeight == 288)
            (*handle)->isPalMode = TRUE;

    }
    else {
        OSA_printf("\n gs2971 chip: no video detected !\n");
		return -1;
    }

	return 0;
}

Int32 cap_get_2972_resolution(Void **phandle, VCAP_VIDEO_SOURCE_CH_STATUS_S *pvstatus)
{
	Int32 status = 0;

	VCAP_VIDEO_SOURCE_STATUS_PARAMS_S	videoStatusArgs;
	VCAP_VIDEO_SOURCE_CH_STATUS_S		videoStatus;

	Device_Gs2972Handle *handle = (Device_Gs2972Handle *)phandle;

	status = Device_gs2972Control(handle,
									IOCTL_DEVICE_VIDEO_DECODER_GET_VIDEO_STATUS,
									&videoStatusArgs, &videoStatus);
	pvstatus->isVideoDetect = videoStatus.isVideoDetect;
    if(videoStatus.isVideoDetect) {
#if 0
        OSA_printf("\n gs2972 chip: detected video (%dx%d@%dHz, %d)!!!\n",
                   videoStatus.frameWidth,
                   videoStatus.frameHeight,
                   0,
                   videoStatus.isInterlaced);
#endif
		pvstatus->frameWidth = videoStatus.frameWidth;
		pvstatus->frameHeight = videoStatus.frameHeight;
		pvstatus->isInterlaced = videoStatus.isInterlaced;


        if(videoStatus.frameHeight == 288)
            (*handle)->isPalMode = TRUE;

    }
    else {
        OSA_printf("\n gs2972 chip: no video detected !\n");
		return -1;
    }

	return 0;
}

Int32 cap_get_5158_resolution(Void **phandle, VCAP_VIDEO_SOURCE_CH_STATUS_S *pvstatus)
{
	Int32 status = 0;

	VCAP_VIDEO_SOURCE_STATUS_PARAMS_S	videoStatusArgs;
	VCAP_VIDEO_SOURCE_CH_STATUS_S		videoStatus;

	Device_Tvp5158Handle *handle = (Device_Tvp5158Handle *)phandle;

	status = Device_tvp5158Control(*handle,
									IOCTL_DEVICE_VIDEO_DECODER_GET_VIDEO_STATUS,
									&videoStatusArgs, &videoStatus);
	pvstatus->isVideoDetect = videoStatus.isVideoDetect;
    if(videoStatus.isVideoDetect) {
#if 1
        printf("\n tvp5158 chip: detected video (%dx%d@%dHz, %d)!!!\n",
                   videoStatus.frameWidth,
                   videoStatus.frameHeight,
                   0,
                   videoStatus.isInterlaced);
#endif
		pvstatus->frameWidth = videoStatus.frameWidth;
		pvstatus->frameHeight = videoStatus.frameHeight;
		pvstatus->isInterlaced = videoStatus.isInterlaced;


        if(videoStatus.frameHeight == 288)
            (*handle)->isPalMode = TRUE;

    }
    else {
        OSA_printf("\n tvp5158 chip: no video detected !\n");
		return -1;
    }

	return 0;
}


Int32 Cap_SetSource7441Chan(Void **phandle, int chan)
{
	Device_Adv7441Handle *handle = (Device_Adv7441Handle *)phandle;
	return Device_adv7441Control(*handle,
		IOCTL_DEVICE_VIDEO_DECODER_SET_SOURCE_CHAN,
		&chan, NULL);
}

Int32 Cap_SetSource7442Chan(Void **phandle, int chan)
{
	Device_Adv7442Handle *handle = (Device_Adv7442Handle *)phandle;
	return Device_adv7442Control(*handle,
		IOCTL_DEVICE_VIDEO_DECODER_SET_SOURCE_CHAN,
		&chan, NULL);
}


Int32 cap_get_adv7441_HV(Void **phandle, int* hv)
{
	Device_Adv7441Handle *handle = (Device_Adv7441Handle *)phandle;
	return Device_adv7441Control(*handle,
		IOCTL_DEVICE_VIDEO_DECODER_GET_DIRECTION,
		hv, NULL);

}


Int32 cap_set_adv7441_HV(Void **phandle, int hv)
{
	Device_Adv7441Handle *handle = (Device_Adv7441Handle *)phandle;
	return Device_adv7441Control(*handle,
		IOCTL_DEVICE_VIDEO_DECODER_SET_DIRECTION,
		&hv, &hv);

}
Int32 cap_get_adv7442_HV(Void **phandle, int* hv)
{
	Device_Adv7442Handle *handle = (Device_Adv7442Handle *)phandle;
	return Device_adv7442Control(*handle,
		IOCTL_DEVICE_VIDEO_DECODER_GET_DIRECTION,
		hv,NULL);

}

Int32 cap_set_adv7442_HV(Void **phandle, int hv)
{
	Device_Adv7442Handle *handle = (Device_Adv7442Handle *)phandle;
	return Device_adv7442Control(*handle,
		IOCTL_DEVICE_VIDEO_DECODER_SET_DIRECTION,
		&hv, &hv);

}
Int32 cap_invert_cbcr_adv7441_HV(Void **phandle, int hv)
{
	Device_Adv7441Handle *handle = (Device_Adv7441Handle *)phandle;
	return Device_adv7441Control(*handle,
		IOCTL_DEVICE_VIDEO_DECODER_INVERT_CBCR,
		&hv, &hv);

}
Int32 cap_invert_cbcr_adv7442_HV(Void **phandle, int hv)
{
	Device_Adv7442Handle *handle = (Device_Adv7442Handle *)phandle;
	return Device_adv7442Control(*handle,
		IOCTL_DEVICE_VIDEO_DECODER_INVERT_CBCR,
		&hv, &hv);

}

Int32 cap_check_status(Uint32 captureId)
{
	Int status = -1;

	if(captureId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(captureId,
									CAPTURE_LINK_CMD_CHECK_STATUS,
									NULL,
									0,
									TRUE
									);
	}

	return status;
}

Int32 cap_set_blind_area(Uint32 captureId, CaptureLink_BlindInfo *params)
{
	Int32 status = -1;

	if(captureId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(captureId,
									CAPTURE_LINK_CMD_CONFIGURE_BLIND_AREA,
									params,
									sizeof(*params),
									TRUE
									);
	}

	return status;
}



