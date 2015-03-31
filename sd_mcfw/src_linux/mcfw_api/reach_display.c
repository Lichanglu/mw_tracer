#include "reach_system_priv.h"
#include "reach_display_priv.h"

#include <mcfw/interfaces/link_api/displayLink.h>

#include <device_ad9889.h>
#include <mcfw/src_linux/devices/ad9889/src/ad9889_priv.h>
#include <ad9889.h>
#include <device_ths8200.h>
#include <mcfw/src_linux/devices/ths8200/src/ths8200_priv.h>
#include <ths8200.h>
#include "ti_vdis_timings.h"

Void dis_init_create_param(DisplayLink_CreateParams *prm)
{
	return DisplayLink_CreateParams_Init(prm);
}


Int32 dis_switch_input_mode(Uint32 displayId, Bool inputMode)
{
	Int status = -1;
	
    DisplayLink_SwitchInputMode prm;
    prm.enableFieldSeparatedInputMode = inputMode;
	
	if(displayId != SYSTEM_LINK_ID_INVALID){
    	status = System_linkControl(displayId,
									DISPLAY_LINK_CMD_SWITCH_INPUT_MODE,
									&prm,
									sizeof(prm),
									TRUE
									);
	}

	return status;
}

Int32 dis_print_statistics(Uint32 displayId)
{
	Int status = -1;
	
    if(displayId != SYSTEM_LINK_ID_INVALID){
		status = System_linkControl(displayId,
									DISPLAY_LINK_CMD_PRINT_STATISTICS,
									NULL,
									0,
									TRUE
									);
    }

	return status;
}

Int32 dis_print_bufferstatistics(Uint32 displayId)
{
	Int status = -1;
	
	if(displayId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(displayId,
	                				DISPLAY_LINK_CMD_PRINT_BUFFER_STATISTICS,
	                				NULL,
	                				0,
	                				TRUE
	                				);
	}

	return status;
}

Int32 dis_switch_activechannel(Uint32 displayId, UInt32 channelId)
{
    Int32 status = -1;
    DisplayLink_SwitchChannelParams params = {0};

	params.activeChId = channelId;

    if(displayId != SYSTEM_LINK_ID_INVALID) {
        status = System_linkControl(displayId,
                           			DISPLAY_LINK_CMD_SWITCH_CH,
                           			&params,
                           			sizeof(params),
                           			TRUE
                           			);
    }

    return status;
}


Int32 dis_start_drv(Uint32 displayId)
{
    Int32 status = -1;

	if(displayId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(displayId,
                            		DISPLAY_LINK_CMD_START_DRV,
                            		NULL,
                            		0,
                            		TRUE
                            		);
	}
	
    return status;
}

Int32 dis_stop_drv(Uint32 displayId)
{
    Int32 status = -1;

	if(displayId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(displayId,
                            		DISPLAY_LINK_CMD_STOP_DRV,
                            		NULL,
                            		0,
                            		TRUE
                            		);
	}
	
    return status;
}

static gBuff[1024] = {0};
/*主要调用系统命令，设置输出模式*/
static Int32 dis_set_system_resolution(UInt32 displayId, UInt32 resolution)
{
    printf("-----------dis_set_system_resolution-------------------\n");
#if 0
	//if(displayId == VDIS_DEV_HDMI)
	{
			//8168x use
			printf("-----------1873-------------------\n");
            VDIS_CMD_ARG2(gBuff, VDIS_TIMINGS_SETVENC, 0, 0)
                printf("-----------1111111111111-------------------\n");
            switch(resolution) {
                case VSYS_STD_1080P_60:
                    VDIS_CMD_ARG1(gBuff, VDIS_TIMINGS_1080P_60, 0)
                break;
                case VSYS_STD_1080P_50:
                    VDIS_CMD_ARG1(gBuff, VDIS_TIMINGS_1080P_50, 0)
                break;
                case VSYS_STD_720P_60:
                    VDIS_CMD_ARG1(gBuff, VDIS_TIMINGS_720P_60, 0)
                break;
                case VSYS_STD_XGA_60:
                    VDIS_CMD_ARG1(gBuff, VDIS_TIMINGS_XGA_60, 0)
                break;
                case VSYS_STD_SXGA_60:
                    VDIS_CMD_ARG1(gBuff, VDIS_TIMINGS_SXGA_60, 0)
                break;
                default:
                    printf("\n Resolution not supported for HDMI!! \n");
                break;
            }
            printf("-----------222222222222-------------------\n");
            VDIS_CMD_ARG2(gBuff, VDIS_TIMINGS_SETVENC, 1, 0)
                printf("-----------333333333333333333333-------------------\n");
        }

#else
    
    VDIS_CMD_ARG2(gBuff, VDIS_TIMINGS_SETVENC, 0, 3)                
    VDIS_CMD_ARG2(gBuff, VDIS_TIMINGS_SETVENC, 0, 0)                
    VDIS_CMD_ARG2(gBuff, VDIS_CLKSRC_SETVENC,"dclk",3)      //Arun                
    switch(resolution) 
    {                    
        case VSYS_STD_1080P_60:                        
            VDIS_CMD_ARG1(gBuff, VDIS_TIMINGS_1080P_60, 3)                        
            VDIS_CMD_ARG1(gBuff, VDIS_TIMINGS_1080P_60, 0)                    
            break;                   
        case VSYS_STD_1080P_50:                        
            VDIS_CMD_ARG1(gBuff, VDIS_TIMINGS_1080P_50, 3)     
            VDIS_CMD_ARG1(gBuff, VDIS_TIMINGS_1080P_50, 0)                 
            break;   
      case VSYS_STD_1080P_30:                        
          VDIS_CMD_ARG1(gBuff, VDIS_TIMINGS_1080P_30, 3)     
          VDIS_CMD_ARG1(gBuff, VDIS_TIMINGS_1080P_30, 0)                 
          break;
        case VSYS_STD_720P_60:                        
            VDIS_CMD_ARG1(gBuff, VDIS_TIMINGS_720P_60, 3)                        
            VDIS_CMD_ARG1(gBuff, VDIS_TIMINGS_720P_60, 0)                    
            break;    
       case VSYS_STD_720P_50:                        
           VDIS_CMD_ARG1(gBuff, VDIS_TIMINGS_720P_50, 3)                        
           VDIS_CMD_ARG1(gBuff, VDIS_TIMINGS_720P_50, 0)                    
           break;  
        case VSYS_STD_XGA_60:                        
            VDIS_CMD_ARG1(gBuff, VDIS_TIMINGS_XGA_60, 3)                        
            VDIS_CMD_ARG1(gBuff, VDIS_TIMINGS_XGA_60, 0)                    
            break;                    
        case VSYS_STD_SXGA_60:                        
            VDIS_CMD_ARG1(gBuff, VDIS_TIMINGS_SXGA_60, 3)                        
            VDIS_CMD_ARG1(gBuff, VDIS_TIMINGS_SXGA_60, 0)                    
            break;                    
        default:                        
            printf("\n Resolution not supported for this HDMI|HDCOMP!! \n");                    
            break;                
    }                
    /* Tie HDMI and HDCOMP from A8 side */               
    VDIS_CMD_ARG1(gBuff, VDIS_TIMINGS_TIEDVENCS, VDIS_VENC_HDCOMP | VDIS_VENC_HDMI)                
    VDIS_CMD_ARG2(gBuff, VDIS_TIMINGS_SETVENC, 1, 3)                
    VDIS_CMD_ARG2(gBuff, VDIS_TIMINGS_SETVENC, 1, 0)
#endif
    printf("-----------dis_set_system_resolution end-------------------\n");
	return 0;
}

Int32 dis_set_resolution(UInt32 displayId, UInt32 resolution)
{
	Int32 status = -1;
	DisplayLink_RtParams params;

	dis_set_system_resolution(displayId,resolution);
	//return 0;
	params.resolution = resolution;

	
	if(displayId != SYSTEM_LINK_ID_INVALID) {
	    status = System_linkControl(displayId,
	                                DISPLAY_LINK_CMD_CHANGE_RESOLUTION,
	                                &params,
	                                sizeof(DisplayLink_RtParams),
	                                TRUE
	                                );
	}

	return 0;
}

Void dis_params_init(VDIS_PARAMS_S * pContext)
{
	UInt16 i, j, width, height;

	memset(pContext, 0, sizeof(VDIS_PARAMS_S));
	
	for(i = 0; i < VDIS_DEV_MAX; i++) {
		pContext->deviceParams[i].backGroundColor = 0;
		pContext->deviceParams[i].resolution  = VSYS_STD_1080P_60;

		width  = 1920;
		height = 1080;

		if(i == VDIS_DEV_SD) {
			pContext->deviceParams[i].resolution  = VSYS_STD_NTSC;

			width  = 720;
			height = 480;
		}

		pContext->mosaicParams[i].displayWindow.start_X = 0;
		pContext->mosaicParams[i].displayWindow.start_Y = 0;
		pContext->mosaicParams[i].displayWindow.width	= width;
		pContext->mosaicParams[i].displayWindow.height	= height;

		pContext->mosaicParams[i].numberOfWindows		= 1;
		pContext->mosaicParams[i].outputFPS 			= 30;

		for(j=0; j<pContext->mosaicParams[i].numberOfWindows; j++) {
			pContext->mosaicParams[i].winList[j].start_X	= 0;
			pContext->mosaicParams[i].winList[j].start_Y	= 0;
			pContext->mosaicParams[i].winList[j].width		= width;
			pContext->mosaicParams[i].winList[j].height 	= height;

			pContext->mosaicParams[i].chnMap[j] 			= j;
		}
		pContext->mosaicParams[i].userSetDefaultSWMLayout = TRUE;
	}

	pContext->tiedDevicesMask = VDIS_VENC_HDMI  ;
	pContext->enableConfigExtVideoEncoder = FALSE;

	pContext->deviceParams[VDIS_DEV_DVO2].enable = FALSE;
	pContext->deviceParams[VDIS_DEV_DVO2].outputInfo.vencNodeNum = VDIS_VENC_DVO2;
	pContext->deviceParams[VDIS_DEV_DVO2].outputInfo.aFmt = VDIS_A_OUTPUT_COMPOSITE;
	pContext->deviceParams[VDIS_DEV_DVO2].outputInfo.dvoFidPolarity = VDIS_POLARITY_ACT_HIGH;
	pContext->deviceParams[VDIS_DEV_DVO2].outputInfo.dvoVsPolarity = VDIS_POLARITY_ACT_HIGH;
	pContext->deviceParams[VDIS_DEV_DVO2].outputInfo.dvoHsPolarity = VDIS_POLARITY_ACT_HIGH;
	pContext->deviceParams[VDIS_DEV_DVO2].outputInfo.dvoActVidPolarity = VDIS_POLARITY_ACT_HIGH;
	pContext->deviceParams[VDIS_DEV_DVO2].outputInfo.dvoFmt = VDIS_DVOFMT_TRIPLECHAN_DISCSYNC;//VDIS_DVOFMT_DOUBLECHAN;
	pContext->deviceParams[VDIS_DEV_DVO2].outputInfo.dataFormat = SYSTEM_DF_RGB24_888;//SYSTEM_DF_RGB24_888;//SYSTEM_DF_YUV422SP_UV;

	pContext->deviceParams[VDIS_DEV_HDMI].enable = TRUE;
	pContext->deviceParams[VDIS_DEV_HDMI].outputInfo.vencNodeNum = VDIS_VENC_HDMI;
	pContext->deviceParams[VDIS_DEV_HDMI].outputInfo.aFmt = VDIS_A_OUTPUT_COMPOSITE;
	pContext->deviceParams[VDIS_DEV_HDMI].outputInfo.dvoFmt = VDIS_DVOFMT_TRIPLECHAN_EMBSYNC;//VDIS_DVOFMT_TRIPLECHAN_DISCSYNC;
	pContext->deviceParams[VDIS_DEV_HDMI].outputInfo.dataFormat = SYSTEM_DF_RGB24_888;

	pContext->deviceParams[VDIS_DEV_SD].enable = FALSE;
	pContext->deviceParams[VDIS_DEV_SD].outputInfo.vencNodeNum = VDIS_VENC_SD;
	pContext->deviceParams[VDIS_DEV_SD].outputInfo.aFmt = VDIS_A_OUTPUT_COMPOSITE;
	pContext->deviceParams[VDIS_DEV_SD].outputInfo.dvoFmt = VDIS_DVOFMT_TRIPLECHAN_DISCSYNC;
	pContext->deviceParams[VDIS_DEV_SD].outputInfo.dataFormat = SYSTEM_DF_RGB24_888;

	pContext->deviceParams[VDIS_DEV_HDCOMP].enable = FALSE;
	pContext->deviceParams[VDIS_DEV_HDCOMP].outputInfo.vencNodeNum = VDIS_VENC_HDCOMP;
	pContext->deviceParams[VDIS_DEV_HDCOMP].outputInfo.aFmt = VDIS_A_OUTPUT_COMPONENT;
	pContext->deviceParams[VDIS_DEV_HDCOMP].outputInfo.dvoFmt = VDIS_DVOFMT_TRIPLECHAN_DISCSYNC;//VDIS_DVOFMT_TRIPLECHAN_EMBSYNC;
	pContext->deviceParams[VDIS_DEV_HDCOMP].outputInfo.dataFormat = SYSTEM_DF_RGB24_888;//SYSTEM_DF_YUV444P;
	pContext->tiedDevicesMask = VDIS_VENC_HDCOMP | VDIS_VENC_HDMI;//VDIS_VENC_DVO2;
	pContext->enableEdgeEnhancement = TRUE;

	pContext->deviceParams[SYSTEM_DC_VENC_HDMI].colorSpaceMode = VDIS_CSC_MODE_SDTV_GRAPHICS_Y2R;
	pContext->enableConfigExtThsFilter = TRUE;
	pContext->enableLayoutGridDraw = FALSE;
}






Void dis_params_init_720P(VDIS_PARAMS_S * pContext)
{
	UInt16 i, j, width, height;

	memset(pContext, 0, sizeof(VDIS_PARAMS_S));
	
	for(i = 0; i < VDIS_DEV_MAX; i++) {
		pContext->deviceParams[i].backGroundColor = 0;
		pContext->deviceParams[i].resolution  = VSYS_STD_1080P_60;

		width  = 1280;
		height = 720;

		if(i == VDIS_DEV_SD) {
			pContext->deviceParams[i].resolution  = VSYS_STD_NTSC;

			width  = 720;
			height = 480;
		}

		pContext->mosaicParams[i].displayWindow.start_X = 0;
		pContext->mosaicParams[i].displayWindow.start_Y = 0;
		pContext->mosaicParams[i].displayWindow.width	= width;
		pContext->mosaicParams[i].displayWindow.height	= height;

		pContext->mosaicParams[i].numberOfWindows		= 1;
		pContext->mosaicParams[i].outputFPS 			= 30;

		for(j=0; j<pContext->mosaicParams[i].numberOfWindows; j++) {
			pContext->mosaicParams[i].winList[j].start_X	= 0;
			pContext->mosaicParams[i].winList[j].start_Y	= 0;
			pContext->mosaicParams[i].winList[j].width		= width;
			pContext->mosaicParams[i].winList[j].height 	= height;

			pContext->mosaicParams[i].chnMap[j] 			= j;
		}
		pContext->mosaicParams[i].userSetDefaultSWMLayout = TRUE;
	}

	pContext->tiedDevicesMask = VDIS_VENC_HDMI | VDIS_VENC_DVO2;
	pContext->enableConfigExtVideoEncoder = TRUE;

	pContext->deviceParams[VDIS_DEV_DVO2].enable = TRUE;
	pContext->deviceParams[VDIS_DEV_DVO2].outputInfo.vencNodeNum = VDIS_VENC_DVO2;
	pContext->deviceParams[VDIS_DEV_DVO2].outputInfo.aFmt = VDIS_A_OUTPUT_COMPOSITE;
	pContext->deviceParams[VDIS_DEV_DVO2].outputInfo.dvoFidPolarity = VDIS_POLARITY_ACT_HIGH;
	pContext->deviceParams[VDIS_DEV_DVO2].outputInfo.dvoVsPolarity = VDIS_POLARITY_ACT_HIGH;
	pContext->deviceParams[VDIS_DEV_DVO2].outputInfo.dvoHsPolarity = VDIS_POLARITY_ACT_HIGH;
	pContext->deviceParams[VDIS_DEV_DVO2].outputInfo.dvoActVidPolarity = VDIS_POLARITY_ACT_HIGH;
	pContext->deviceParams[VDIS_DEV_DVO2].outputInfo.dvoFmt = VDIS_DVOFMT_TRIPLECHAN_DISCSYNC;//VDIS_DVOFMT_DOUBLECHAN;
	pContext->deviceParams[VDIS_DEV_DVO2].outputInfo.dataFormat = SYSTEM_DF_RGB24_888;//SYSTEM_DF_YUV422SP_UV;

	pContext->deviceParams[VDIS_DEV_HDMI].enable = TRUE;
	pContext->deviceParams[VDIS_DEV_HDMI].outputInfo.vencNodeNum = VDIS_VENC_HDMI;
	pContext->deviceParams[VDIS_DEV_HDMI].outputInfo.aFmt = VDIS_A_OUTPUT_COMPOSITE;
	pContext->deviceParams[VDIS_DEV_HDMI].outputInfo.dvoFmt = VDIS_DVOFMT_TRIPLECHAN_EMBSYNC;//VDIS_DVOFMT_TRIPLECHAN_DISCSYNC;
	pContext->deviceParams[VDIS_DEV_HDMI].outputInfo.dataFormat = SYSTEM_DF_RGB24_888;

	pContext->deviceParams[VDIS_DEV_SD].enable = TRUE;
	pContext->deviceParams[VDIS_DEV_SD].outputInfo.vencNodeNum = VDIS_VENC_SD;
	pContext->deviceParams[VDIS_DEV_SD].outputInfo.aFmt = VDIS_A_OUTPUT_COMPOSITE;
	pContext->deviceParams[VDIS_DEV_SD].outputInfo.dvoFmt = VDIS_DVOFMT_TRIPLECHAN_DISCSYNC;
	pContext->deviceParams[VDIS_DEV_SD].outputInfo.dataFormat = SYSTEM_DF_RGB24_888;

	pContext->deviceParams[VDIS_DEV_HDCOMP].enable = TRUE;
	pContext->deviceParams[VDIS_DEV_HDCOMP].outputInfo.vencNodeNum = VDIS_VENC_HDCOMP;
	pContext->deviceParams[VDIS_DEV_HDCOMP].outputInfo.aFmt = VDIS_A_OUTPUT_COMPONENT;
	pContext->deviceParams[VDIS_DEV_HDCOMP].outputInfo.dvoFmt = VDIS_DVOFMT_TRIPLECHAN_DISCSYNC;//VDIS_DVOFMT_TRIPLECHAN_EMBSYNC;
	pContext->deviceParams[VDIS_DEV_HDCOMP].outputInfo.dataFormat = SYSTEM_DF_RGB24_888;//SYSTEM_DF_YUV444P;
	pContext->tiedDevicesMask = VDIS_VENC_HDCOMP | VDIS_VENC_DVO2;
	pContext->enableEdgeEnhancement = TRUE;

	pContext->deviceParams[SYSTEM_DC_VENC_HDMI].colorSpaceMode = VDIS_CSC_MODE_SDTV_GRAPHICS_Y2R;
	pContext->enableConfigExtThsFilter = TRUE;
	pContext->enableLayoutGridDraw = FALSE;
}


#if 0

Int32 AD9889Init(Int32 outMode)
{	
	Int32 status = 0;
	Device_VideoEncoderCreateParams 	createArgs;
	Device_VideoEncoderCreateStatus 	createStatusArgs;
	Device_Ad9889Handle handle;
	
	Device_ad9889Init();

	memset(&createArgs, 0, sizeof(Device_VideoEncoderCreateParams));

	createArgs.deviceI2cInstId	= 1;
	createArgs.deviceI2cAddr	= 0;
	createArgs.inpClk			= 0;
	createArgs.hdmiHotPlugGpioIntrLine = 0;
	createArgs.syncMode			= DEVICE_VIDEO_ENCODER_EMBEDDED_SYNC;
	createArgs.clkEdge			= FALSE;

	handle = Device_ad9889Create(0, 0, &(createArgs), &(createStatusArgs));
	if(handle == NULL) {
		return -1;
	}

	status = Device_ad9889Control(handle,
									IOCTL_DEVICE_VIDEO_ENCODER_SET_MODE,
									NULL, NULL);
	OSA_assert(status >= 0);

	return 0;
}

Int32 Ths8200Init(Int32 outMode)
{	
	Int32 status = 0;
//	Int32 outMode=DEVICE_STD_VGA_1024X768X60;
	Device_VideoEncoderCreateParams 	createArgs;
	Device_VideoEncoderCreateStatus 	createStatusArgs;
	Device_Ths8200Handle handle;
	
	Device_ths8200Init();

	memset(&createArgs, 0, sizeof(Device_VideoEncoderCreateParams));

	createArgs.deviceI2cInstId	= 0;
	createArgs.deviceI2cAddr	= 0;
	createArgs.inpClk			= 0;
	createArgs.hdmiHotPlugGpioIntrLine = 0;
	createArgs.syncMode			= DEVICE_VIDEO_ENCODER_EMBEDDED_SYNC;
	createArgs.clkEdge			= FALSE;

	handle = Device_ths8200Create(0, 0, &(createArgs), &(createStatusArgs));
	if(handle == NULL) {
		return -1;
	}

	status = Device_ths8200Control(handle,
									IOCTL_DEVICE_VIDEO_ENCODER_SET_MODE,
									&outMode, NULL);
	OSA_assert(status >= 0);

	return 0;
}

Int32 dis_config_adv9889(Void **phandle)
{	
	Int32 status = 0;
	
	Device_VideoEncoderCreateParams 	createArgs;
	Device_VideoEncoderCreateStatus 	createStatusArgs;

	Device_Ad9889Handle *handle = (Device_Ad9889Handle *)phandle;
	
	Device_ad9889Init();

	char cmd[512] = {0};

	memset(cmd, 0, 512);
	sprintf(cmd, "echo 40 > /sys/class/gpio/export");
	system(cmd);
	memset(cmd, 0, 512);
	sprintf(cmd, "echo out > /sys/devices/virtual/gpio/gpio40/direction");
	system(cmd);
	memset(cmd, 0, 512);
	sprintf(cmd, "echo 0 > /sys/devices/virtual/gpio/gpio40/value");
	system(cmd);

	memset(&createArgs, 0, sizeof(Device_VideoEncoderCreateParams));

	createArgs.deviceI2cInstId	= 1;
	createArgs.deviceI2cAddr	= 0;
    createArgs.inpClk			= 0;
    createArgs.hdmiHotPlugGpioIntrLine = 0;
    createArgs.syncMode			= DEVICE_VIDEO_ENCODER_EMBEDDED_SYNC;
    createArgs.clkEdge			= FALSE;

	*handle = Device_ad9889Create(0, 0, &(createArgs), &(createStatusArgs));
	if(*handle == NULL) {
		return -1;
	}

	status = Device_ad9889Control(*handle,
									IOCTL_DEVICE_VIDEO_ENCODER_SET_MODE,
									NULL, NULL);
	OSA_assert(status >= 0);

	return 0;
}

Int32 dis_config_ths8200(Void **phandle)
{	
	Int32 status = 0;
	
	Device_VideoEncoderCreateParams 	createArgs;
	Device_VideoEncoderCreateStatus 	createStatusArgs;

	Device_Ths8200Handle *handle = (Device_Ths8200Handle *)phandle;
	
	Device_ths8200Init();

	memset(&createArgs, 0, sizeof(Device_VideoEncoderCreateParams));

	createArgs.deviceI2cInstId	= 1;
	createArgs.deviceI2cAddr	= 0;
    createArgs.inpClk			= 0;
    createArgs.hdmiHotPlugGpioIntrLine = 0;
    createArgs.syncMode			= DEVICE_VIDEO_ENCODER_EMBEDDED_SYNC;
    createArgs.clkEdge			= FALSE;

	*handle = Device_ths8200Create(0, 0, &(createArgs), &(createStatusArgs));
	if(*handle == NULL) {
		return -1;
	}

	status = Device_ths8200Control(*handle,
									IOCTL_DEVICE_VIDEO_ENCODER_SET_MODE,
									NULL, NULL);
	OSA_assert(status >= 0);

	return 0;
}

#endif

