/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpsdrv_hdmi9889.c
 *
 *  \brief HDMI9889 driver
 *  This file implements functionality for the HDMI.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "ti_media_std.h"
#include "ti_vsys_common_def.h"
#include <device.h>
#include <device_videoEncoder.h>
#include <device_ths8200.h>
#include <ths8200_priv.h>
#include <osa_i2c.h>



/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
#define TPI_INPUTBUS_PIXEL_REP_BIT4_MASK                (0x10u)

#define THS8200_IIC_SLAVE_ADDR						(0x21)
#define THS7303_IIC_SLAVE_ADDR						(0x2C)
/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

typedef struct
{
    VSYS_VIDEO_STANDARD_E standard;
    UInt32                modeCode;
    UInt32                pixClk;
    UInt32                vFreq;
    UInt32                pixels;
    UInt32                lines;
    struct {
        UInt32            hBitToHSync;
        UInt32            field2Offset;
        UInt32            hWidth;
        UInt32            vBitToVSync;
        UInt32            vWidth;
    } embSyncPrms;
    struct {
        UInt32            deDelay;
        /**< Width of the area to the left of the active display */
        UInt32            deTop;
        /**< Height of the area to the top of the active display */
        UInt32            deCnt;
        /**< Width of the active display */
        UInt32            deLine;
        /**< Height of the active display */
    } extSyncPrms;
} Device_THS8200ModeInfo;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Int32 Device_ths8200GetHdmiChipId(Device_Ths8200Obj* psiiObj,
                                   Device_Ths8200HdmiChipId *hdmichipId);

static Int32 Device_ths8200GetDetailedChipId(Device_Ths8200Handle handle,
                                          Ptr cmdArgs,
                                          Ptr cmdStatusArgs);

static Int32 Device_ths8200SetMode(Device_Ths8200Handle handle,
                                Ptr cmdArgs,
                                Ptr cmdStatusArgs);

static Int32 Device_ths8200Start(Device_Ths8200Handle handle,
                              Ptr cmdArgs,
                              Ptr cmdStatusArgs);
static Int32 Device_ths8200Stop(Device_Ths8200Handle handle,
                             Ptr cmdArgs,
                             Ptr cmdStatusArgs);

static Int32 Device_ths8200GetHpd(Device_Ths8200Handle handle,
                               Ptr cmdArgs,
                               Ptr cmdStatusArgs);

static Int32 Device_ths8200SetPrms(Device_Ths8200Handle handle,
                               Ptr cmdArgs,
                               Ptr cmdStatusArgs);
static Int32 Device_ths8200GetPrms(Device_Ths8200Handle handle,
                               Ptr cmdArgs,
                               Ptr cmdStatusArgs);
static Int32 Device_ths8200DeviceInit(Device_Ths8200Obj* pObj);

static Int32 Device_ths8200Reset(Device_Ths8200Obj* pObj);
static Int32 Device_ths8200Enable(Device_Ths8200Obj* pObj);
static Int32 Device_ths8200PowerUpTxm(Device_Ths8200Obj* pObj);
static Int32 Device_ths8200CfgInBus(Device_Ths8200Obj* pObj);
static Int32 Device_ths8200CfgYcMode(Device_Ths8200Obj* pObj);
static Int32 Device_ths8200CfgSyncMode(Device_Ths8200Obj* pObj);
static Int32 Device_ths8200PrgmEmbSyncTimingInfo(
                Device_Ths8200Obj *siiObj,
                Device_THS8200ModeInfo *siModeInfo);
static Int32 Device_ths8200PrgmExtSyncTimingInfo(
                Device_Ths8200Obj *siiObj,
                Device_THS8200ModeInfo *siModeInfo);
static Int32 Device_ths8200PrgmAvInfoFrame(
                Device_Ths8200Obj *siiObj,
                Device_THS8200ModeInfo *modeInfo);
static Int32 Device_ths8200PrgmMdResetRegs(
                Device_Ths8200Obj *siiObj,
                Device_THS8200ModeInfo *siModeInfo);
static Int32 Device_ths8200CalcCRC(UInt8 *regAddr, UInt8 *regValue, UInt32 *regCnt);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

Device_THS8200ModeInfo THS8200ModeInfo[] =
{
    {VSYS_STD_480P, 3, 2700, 60, 858, 525,
        {16, 0, 62, 9, 6}, {122, 36, 720, 480}},
    {VSYS_STD_576P, 18, 2700, 50, 864, 625,
        {12, 0, 64, 5, 5}, {132, 44, 720, 576}},
    {VSYS_STD_720P_60, 4, 7425, 60, 1650, 750,
        {110, 0, 40, 5, 5}, {260, 25, 1280, 720}},
    {VSYS_STD_720P_50, 19, 7425, 50, 1980, 750,
        {440, 0, 40, 5, 5}, {260, 25, 1280, 720}},
    {VSYS_STD_1080P_30, 34, 7425, 30, 2200, 1125,
        {88, 0, 44, 4, 5}, {192, 41, 1920, 1080}},
    {VSYS_STD_1080P_50, 16, 7425, 50, 2640, 1125,
        {528, 0, 44, 4, 5}, {192, 41, 1920, 1080}},
    {VSYS_STD_1080P_60, 16, 14850, 60, 2200, 1125,
        {88, 0, 44, 4, 5}, {192, 41, 1920, 1080}},
    {VSYS_STD_1080I_60, 5, 7425, 60, 2200, 1125,
        {88, 564, 44, 2, 5}, {192, 20, 1920, 540}},
    {VSYS_STD_1080I_50, 20, 7425, 50, 2640, 1125,
        {528, 564, 44, 2, 5}, {192, 20, 1920, 540}},
};

typedef enum __THS8200_SUPPORT_FORMAT{
	THS8200_VGA_1024X768X60,//----------0
	THS8200_VGA_1280X720X60,//----------1
	THS8200_VGA_1280X1024X60,//----------2
	THS8200_VGA_1366X768X60,//----------3
	THS8200_VGA_1280X800X60,//----------4
	THS8200_VGA_1440X900X60,//----------5
	THS8200_VGA_1400X1050X60,//----------6
	THS8200_VIDEO_720P60,//----------7
	THS8200_VIDEO_1080P60,//----------8
	THS8200_SUPPORT_MAX,
}THS8200_SUPPORT_FORMAT;

static unsigned char THS8200_mode[THS8200_SUPPORT_MAX][16] = {
//	{0x03,0x20,0x01,0x27,0x0d,0x60,0x02,0xc0,0x02,0x02,0x0c,0x00,0xa8,0x00,0x01,0x40},//+64?+16 //640x480x60  0
//	{0x03,0x40,0x01,0x27,0x08,0x28,0x03,0x18,0x03,0x02,0x07,0x00,0x70,0x00,0x01,0x40},//+64?    //640x480x72  1
//	{0x03,0x48,0x01,0x17,0xf4,0x40,0x03,0x08,0x03,0x01,0xf3,0x00,0x80,0x00,0x01,0x40},//+64?    //640x480x75  2
//	{0x03,0x40,0x01,0x17,0xfd,0x38,0x03,0x08,0x03,0x01,0xfc,0x00,0x78,0x00,0x01,0x40},//+64?    //640x480x85  3
//	{0x04,0x20,0x01,0x27,0x74,0x80,0x03,0xa0,0x04,0x02,0x73,0x00,0xb8,0x00,0x01,0x58},//+48     //800x600x60  4
//	{0x04,0x10,0x01,0x27,0x9a,0x78,0x03,0x98,0x06,0x02,0x99,0x00,0xb0,0x00,0x01,0x58},//+56     //800x600x72  5
//	{0x04,0x20,0x01,0x27,0x71,0x50,0x03,0xd0,0x03,0x02,0x70,0x00,0x90,0x00,0x01,0x58},          //800x600x75  6
//	{0x04,0x18,0x01,0x27,0x77,0x40,0x03,0xd8,0x03,0x02,0x76,0x00,0x80,0x00,0x01,0x58},          //800x600x85  7	
//	{0x05,0x40,0x01,0x37,0x26,0x88,0x04,0xc3,0x06,0x03,0x25,0x00,0xc8,0x00,0x09,0x40},            //1024x768x60  8
//	{0x05,0x30,0x01,0x37,0x26,0x88,0x04,0xa8,0x06,0x03,0x25,0x00,0xc8,0x00,0x01,0x40},          //1024x768x70  9
//	{0x05,0x20,0x01,0x37,0x20,0x60,0x04,0xc0,0x03,0x03,0x1f,0x00,0x98,0x00,0x01,0x58},//+56     //1024x768x75  10
//	{0x05,0x60,0x01,0x37,0x28,0x60,0x05,0x00,0x03,0x03,0x27,0x00,0x90,0x00,0x01,0x58},//+48     //1024x768x85  11
//	{0x06,0x98,0x01,0x47,0x2a,0x70,0x06,0x28,0x03,0x04,0x29,0x00,0xb0,0x00,0x01,0x58},          //1280x1024x60 12
//	{0x06,0x90,0x01,0x27,0xea,0x88,0x06,0x08,0x03,0x02,0xe9,0x00,0xe0,0x00,0x01,0x50},//13
//	{0x06,0x80,0x01,0x37,0x1e,0x80,0x06,0x00,0x07,0x03,0x1d,0x00,0x8c,0x00,0x01,0x50},//14
//	{0x06,0x80,0x01,0x37,0x3b,0x80,0x06,0x00,0x03,0x03,0x3a,0x00,0xa0,0x00,0x01,0x50},//15
//	{0x06,0x80,0x01,0x37,0x1e,0x88,0x04,0x00,0x03,0x03,0x25,0x00,0xa0,0x00,0x01,0x40},//1280x768x60 16
//	  34h    35h  37h   39h    3ah  70h  71h   72h   73h   74h   75h	 79    7a     7b    7c   
//	{0x05,0x40,0x01,0x37,0x26,0x88,0x04,0xc3,0x06,0x03,0x25,0x00,0xc8,0x00,0x09,0x40},           		 //1024x768x60  8
	{0x05,0x40,0x01,0x37,0x26,0x88,0x00,0x32,0x06,0x00,0x01,0x00,0x00,0x00,0x00,0x43},				//1024x768x60  
	{0x06,0x72,0x01,0x22,0xee,0x28,0x00,0x01,0x05,0x00,0x01,0x00,0x00,0x00,0x00,0x58},				//1280x720x60  
	{0x06,0x98,0x01,0x47,0x2a,0x70,0x00,0x30,0x03,0x00,0x01,0x00,0x00,0x00,0x00,0x5b},				//1280x1024x60
	{0x07,0x06,0x01,0x37,0x1b,0x90,0x00,0x28,0x03,0x00,0x01,0x00,0x00,0x00,0x00,0x53},				//1366x768x60
	{0x06,0x90,0x01,0x37,0x3c,0x88,0x00,0x30,0x03,0x00,0x01,0x00,0x00,0x00,0x00,0x53},				//1280x800x60
	{0x07,0x70,0x01,0x37,0xa4,0x98,0x00,0x38,0x03,0x00,0x01,0x00,0x00,0x00,0x00,0x53},				//1440x900x60
	{0x07,0x58,0x01,0x47,0x3f,0x98,0x00,0x38,0x03,0x00,0x01,0x00,0x00,0x00,0x00,0x53},				//1400x1050x60
};

unsigned char THS8200_ycbcr[24] = {0x0C,0x52,0x08,0x00,0x00,0x00,0x19,0xD7,0x1C,0x54,0x08,0x00,0x1E,0x89,0x02,0x91,0x00,0x00,0x08,0x00,0x0E,0x87,0x18,0xBC};

#define DEVICE_THS8200_MAX_MODES  (sizeof(THS8200ModeInfo) /                     \
                                 sizeof(Device_THS8200ModeInfo))


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
Int32 Device_ths7303_Write8(Device_Ths8200Obj * pObj, UInt8 RegAddr, UInt8 RegVal)
{
	Int32 retVal = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};
	
	regAddr[0] = RegAddr;
	regValue[0] = RegVal;
	
	retVal = OSA_i2cWrite8 (&gDevice_ths8200CommonObj.i2cHandle, THS7303_IIC_SLAVE_ADDR, regAddr, regValue,1);
}

UInt8 Device_ths8200_Read8(Device_Ths8200Obj * pObj, UInt8 RegAddr)
{
	Int32 retVal = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	retVal = OSA_i2cRead8(&gDevice_ths8200CommonObj.i2cHandle,THS8200_IIC_SLAVE_ADDR,regAddr,regValue,1);

	return regValue[0];
}

Int32 Device_ths8200_Write8(Device_Ths8200Obj * pObj, UInt8 RegAddr, UInt8 RegVal)
{
	Int32 retVal = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};
	
	regAddr[0] = RegAddr;
	regValue[0] = RegVal;
	
	retVal = OSA_i2cWrite8 (&gDevice_ths8200CommonObj.i2cHandle, THS8200_IIC_SLAVE_ADDR, regAddr, regValue,1);
}

Int32 Device_ths8200Init(void)
{
	/* Set to 0's for global object, descriptor memory  */
	memset(&gDevice_ths8200CommonObj, 0, sizeof (Device_Ths8200CommonObj));
	return 0;
}

Int32 Device_ths8200DeInit( void )
{
	return 0;
}

Device_Ths8200Handle Device_ths8200Create (UInt32 drvId,
                                 UInt32 instId,
                                 Ptr createArgs,
                                 Ptr createStatusArgs)
{

	Device_Ths8200Obj*  pObj;
	Int32            retVal = 0;
	Device_VideoEncoderCreateParams* vidEncCreateArgs
		= (Device_VideoEncoderCreateParams*) createArgs;
	Device_VideoEncoderCreateStatus* vidEecCreateStatus
		= (Device_VideoEncoderCreateStatus*) createStatusArgs;

	if(NULL == vidEecCreateStatus)
	{
		retVal= -1;
	}

	if ((retVal >= 0) &&
	(vidEncCreateArgs->deviceI2cInstId > DEVICE_I2C_INST_ID_MAX))
	{
		retVal = -1;
	}


	if (retVal >= 0)
	{
		pObj = (Device_Ths8200Obj *)malloc(sizeof(Device_Ths8200Obj));

		if ( pObj == NULL )
		{
			return NULL;
		}
		else
		{
			memset(pObj, 0, sizeof(Device_Ths8200Obj));

			gDevice_ths8200CommonObj.ths8200Handle[instId] = pObj;

			pObj->instId = instId;

			if (pObj == NULL)
			{
				retVal = -1;
			}
			else
			{
				memcpy(&pObj->createArgs,
					vidEncCreateArgs,
					sizeof(*vidEncCreateArgs));
			}
		}
	}

	if((retVal >= 0) && (NULL != pObj))
	{
		gDevice_ths8200CommonObj.prms.outputFormat = DEVICE_THS8200_HDMI_RGB;
		if (DEVICE_VIDEO_ENCODER_EMBEDDED_SYNC == pObj->createArgs.syncMode)
		{
			pObj->syncCfgReg = 0x84;
			pObj->inBusCfg = 0x60;
		}
		else /* Separate Sync Mode */
		{
			pObj->syncCfgReg = 0x04;
			pObj->inBusCfg = 0x70;
		}

		/* FALSE to latch data on falling clock edge. Rising edge otherwise */
		/* Bit 4 of of TPI Input bus and pixel repetation */
		if (pObj->createArgs.clkEdge == FALSE)
		{
			pObj->inBusCfg &= (~(TPI_INPUTBUS_PIXEL_REP_BIT4_MASK));
		}
		else
		{
			pObj->inBusCfg |= TPI_INPUTBUS_PIXEL_REP_BIT4_MASK;
		}

//		retVal = OSA_i2cOpen(&(gDevice_ths8200CommonObj.i2cHandle), I2C_DEFAULT_INST_ID);
		retVal = OSA_i2cOpen(&(gDevice_ths8200CommonObj.i2cHandle), 1);

		/* Enable DE in syncpolarity register at 0x63 */
		pObj->syncPolarityReg = 0x40;
//		Device_ths8200DeviceInit(pObj);
		vidEecCreateStatus->retVal = 0;

		if ( retVal < 0)
		{
			return NULL;
		}

		return (pObj);
	}
	else
	{
		if (NULL != vidEecCreateStatus)
		{
			vidEecCreateStatus->retVal = retVal;
		}
		return (NULL);
	}
}


Int32 Device_ths8200Delete(Device_Ths8200Handle handle, Ptr deleteArgs)
{
    Device_Ths8200Obj* pObj= (Device_Ths8200Obj*)handle;

    if ( pObj == NULL )
        return -1;

    OSA_i2cClose(&(gDevice_ths8200CommonObj.i2cHandle));

    /*
     * free driver handle object
     */
    free(pObj);

    return 0;
}

Int32 Device_ths8200Control (Device_Ths8200Handle handle,
                              UInt32 cmd,
                              Ptr cmdArgs,
                              Ptr cmdStatusArgs)
{
    Device_Ths8200Obj *pObj = (Device_Ths8200Obj *)handle;
    Int32 status;

    if ( handle == NULL )
        return -1;

    /*lock handle    */

    switch (cmd)
    {
        case DEVICE_CMD_START:
            status = Device_ths8200Start(pObj, cmdArgs, cmdStatusArgs);
            break;

        case DEVICE_CMD_STOP:
            status = Device_ths8200Stop(pObj, cmdArgs, cmdStatusArgs);
            break;

        case IOCTL_DEVICE_VIDEO_ENCODER_GET_CHIP_ID:
            status = -1;
            break;

        case IOCTL_DEVICE_VIDEO_ENCODER_SET_MODE:
            status = Device_ths8200SetMode(pObj, cmdArgs, cmdStatusArgs);
            break;

        case IOCTL_DEVICE_THS8200_GET_DETAILED_CHIP_ID:
            status = Device_ths8200GetDetailedChipId(handle,
                                                  cmdArgs,
                                                  cmdStatusArgs);
            break;

        case IOCTL_DEVICE_THS8200_QUERY_HPD:
            status = Device_ths8200GetHpd(pObj, cmdArgs, cmdStatusArgs);
            break;

        case IOCTL_DEVICE_THS8200_SET_PARAMS:
            status = Device_ths8200SetPrms(pObj, cmdArgs, cmdStatusArgs);
            break;

        case IOCTL_DEVICE_THS8200_GET_PARAMS:
            status = Device_ths8200GetPrms(pObj, cmdArgs, cmdStatusArgs);
            break;

        default:
            status = -1;
            break;
    }

    /* unlock handle   */
    return status;
}

/*
  Get TP5158 chip ID, revision ID and firmware patch ID
*/
static Int32 Device_ths8200GetDetailedChipId(Device_Ths8200Handle handle, Ptr cmdArgs,
                                  Ptr cmdStatusArgs)
{
	Int32 status = 0;
	return (status);
}



/* Enable TMDS output */
static Int32 Device_ths8200Start(Device_Ths8200Handle handle,
                              Ptr cmdArgs,
                              Ptr cmdStatusArgs)
{
	Int32 retVal = 0;
	return (retVal);
}



/* Disable TMDS output */
static Int32 Device_ths8200Stop(Device_Ths8200Handle handle,
                             Ptr cmdArgs,
                             Ptr cmdStatusArgs)
{
	Int32 retVal = 0;
	return (retVal);
}

static Int32 Device_ths8200OutPutYPbPr(Device_Ths8200Handle handle,THS8200_SUPPORT_FORMAT outMode)
{
	Int32 retVal = 0;
	Device_Ths8200Obj *pObj = (Device_Ths8200Obj *)handle;
	
	printf("Device_ths8200OutPutYPbPr outMode = %d!\n",outMode);
	switch(outMode){
		case THS8200_VIDEO_720P60:
			printf("THS8200 YPbPr 720P60!\n");
			Device_ths8200DeviceInit(pObj);
			Device_ths8200_Write8(pObj,0x1A,0x00);
			Device_ths8200_Write8(pObj,0x03,0x01);
			Device_ths8200_Write8(pObj,0x1A,0x00); // tst_cntl        
			Device_ths8200_Write8(pObj,0x4A,0x00); // csm_mult_gy_msb     
			Device_ths8200_Write8(pObj,0x83,0x00); // cgms_header         
			Device_ths8200_Write8(pObj,0x84,0x00); // cgms_payload_msb    
			Device_ths8200_Write8(pObj,0x85,0x00); // cgms_payload_lsb    
      
			Device_ths8200_Write8(pObj,0x38,0x87);  // 720P Mode
			Device_ths8200_Write8(pObj,0x82,0x54);  // Ignore FID RGB OR YUV mode_6f
			Device_ths8200_Write8(pObj,0x1c,0x58);//FSADJ2 terminal/**********YUV444 YUV422**********/
			Device_ths8200_Write8(pObj,0x34,0x06);  // Pixels per line
			Device_ths8200_Write8(pObj,0x35,0x72);
			Device_ths8200_Write8(pObj,0x39,0x22);  // Lines in frame_22
			Device_ths8200_Write8(pObj,0x3a,0xee);
			Device_ths8200_Write8(pObj,0x3b,0xff);
			Device_ths8200_Write8(pObj,0x7a,0x4a);  // Horizontal offset/60

			Device_ths8200_Write8(pObj,0x1b,0x00); // tst_cntl        
			Device_ths8200_Write8(pObj,0x36,0x00); // csm_mult_gy_msb     
			Device_ths8200_Write8(pObj,0x37,0x01); // cgms_header         
			Device_ths8200_Write8(pObj,0x41,0x40); // cgms_payload_msb    
			Device_ths8200_Write8(pObj,0x42,0x40); // cgms_payload_lsb    

			Device_ths8200_Write8(pObj,0x43,0x40);
			Device_ths8200_Write8(pObj,0x44,0x53);  // 720P Mode
			Device_ths8200_Write8(pObj,0x45,0x3f);  // Ignore FID RGB mode
			Device_ths8200_Write8(pObj,0x46,0x3f);//FSADJ2 terminal
			Device_ths8200_Write8(pObj,0x4f,0x00);  // Pixels per line/06
			Device_ths8200_Write8(pObj,0x70,0x60);
			Device_ths8200_Write8(pObj,0x71,0x00);  // Lines in frame
			Device_ths8200_Write8(pObj,0x72,0x02);
			Device_ths8200_Write8(pObj,0x73,0x1c);//03
			Device_ths8200_Write8(pObj,0x74,0x00);  
			Device_ths8200_Write8(pObj,0x75,0x03);
			Device_ths8200_Write8(pObj,0x79,0x00);
			Device_ths8200_Write8(pObj,0x7b,0x00);
			Device_ths8200_Write8(pObj,0x7c,0x01);//01

			Device_ths8200_Write8(pObj,0x03,0x00);
			usleep(10*1000);
			Device_ths8200_Write8(pObj,0x03,0x01);

			Device_ths7303_Write8(pObj,0x01, 0xDA);
			Device_ths7303_Write8(pObj,0x02, 0xDA);
			Device_ths7303_Write8(pObj,0x03, 0xDA);			
			break;
		case THS8200_VIDEO_1080P60:
			printf("THS8200 YPbPr 1080P60!\n");
			Device_ths8200DeviceInit(pObj);
			Device_ths7303_Write8(pObj,0x1A,0x00); // tst_cntl        
			Device_ths7303_Write8(pObj,0x4A,0x00); // csm_mult_gy_msb     
			Device_ths7303_Write8(pObj,0x83,0x00); // cgms_header         
			Device_ths7303_Write8(pObj,0x84,0x00); // cgms_payload_msb    
			Device_ths7303_Write8(pObj,0x85,0x00); // cgms_payload_lsb    

			Device_ths7303_Write8(pObj,0x38, 0x80);  // 1080P Mode
			Device_ths7303_Write8(pObj,0x82, 0x54);
			Device_ths7303_Write8(pObj,0x1c, 0x58);  // 24-bit YCrCb
			Device_ths7303_Write8(pObj,0x34, 0x08);  // Pixels per line
			Device_ths7303_Write8(pObj,0x35, 0x98);
			Device_ths7303_Write8(pObj,0x39, 0x44);  // Lines in frame
			Device_ths7303_Write8(pObj,0x3a, 0x65); 
			Device_ths7303_Write8(pObj,0x3b, 0xff);	 
			Device_ths7303_Write8(pObj,0x7a, 0x80);
			Device_ths7303_Write8(pObj,0x7C, 0x01);
			Device_ths7303_Write8(pObj,0x03, 0x01);
			break;
		default:
			printf("THS8200 Out Put Mode not Support!\n");
	}

}

static Int32 Device_ths8200OutPutVGA(Device_Ths8200Handle handle,THS8200_SUPPORT_FORMAT outMode)
{
	Int32 retVal = 0;
	Device_Ths8200Obj *pObj = (Device_Ths8200Obj *)handle;

	printf("Device_ths8200OutPutVGA outMode = %d!\n", outMode);
	Device_ths8200DeviceInit(pObj);
	Device_ths8200_Write8(pObj,0x03,0x01); 
	Device_ths8200_Write8(pObj,0x1A,0x00); // tst_cntl            
	Device_ths8200_Write8(pObj,0x1B,0x00); // tst_ramp_cntl   
	Device_ths8200_Write8(pObj,0x4A,0x00); // csm_mult_gy_msb     

	Device_ths8200_Write8(pObj,0x82,THS8200_mode[outMode][15]); //5F/54/ pol_cntl:FID must high;RGB mode;no embedded timing//
	Device_ths8200_Write8(pObj,0x1C,0x58); // dman_cntl: 0x38 ;24bit 4:4:4 ycbcr data

	Device_ths8200_Write8(pObj,0x34,THS8200_mode[outMode][0]); //*	04h		05h		05h		06h
	Device_ths8200_Write8(pObj,0x35,THS8200_mode[outMode][1]); //*	18h		40h		60h		98h
	Device_ths8200_Write8(pObj,0x36,0x80); 
	Device_ths8200_Write8(pObj,0x37,THS8200_mode[outMode][2]); //*	1bh		1dh		24h		26h

	Device_ths8200_Write8(pObj,0x38,0x97); //vesa mode 97
	Device_ths8200_Write8(pObj,0x39,THS8200_mode[outMode][3]); //*	27h		37h		37h		47h
	Device_ths8200_Write8(pObj,0x3A,THS8200_mode[outMode][4]); //*	77h		26h		28h		2ah
	Device_ths8200_Write8(pObj,0x3B,0xFF); 

	Device_ths8200_Write8(pObj,0x41,0x00); 
	Device_ths8200_Write8(pObj,0x42,0x00); 
	Device_ths8200_Write8(pObj,0x43,0x00); 
	Device_ths8200_Write8(pObj,0x44,0xFF); 
	Device_ths8200_Write8(pObj,0x45,0xFF); 
	Device_ths8200_Write8(pObj,0x46,0xFF); 
	Device_ths8200_Write8(pObj,0x4a,0x00); 
	Device_ths8200_Write8(pObj,0x4f,0x00); 

	Device_ths8200_Write8(pObj,0x70,THS8200_mode[outMode][5]); //*	40h		88h		60h		70h
	Device_ths8200_Write8(pObj,0x71,THS8200_mode[outMode][6]); //*	03h		04h		05h		06h
	Device_ths8200_Write8(pObj,0x72,THS8200_mode[outMode][7]); //*	d8h		b8h		00h		28h
	Device_ths8200_Write8(pObj,0x73,THS8200_mode[outMode][8]); //*	03h		06h		03h		03h
	Device_ths8200_Write8(pObj,0x74,THS8200_mode[outMode][9]); //*	02h		03h		03h		04h
	Device_ths8200_Write8(pObj,0x75,THS8200_mode[outMode][10]); //*	74h		20h		25h		27h
	
	Device_ths8200_Write8(pObj,0x79,THS8200_mode[outMode][11]); // dtg_hs_in_dly_msb adjusts horizontal input delay 
	Device_ths8200_Write8(pObj,0x7A,THS8200_mode[outMode][12]); //d4 dtg_hs_in_dly_lsb 
	Device_ths8200_Write8(pObj,0x7B,THS8200_mode[outMode][13]); // dtg_vs_in_dly_msb adjust vertical input delay 
	Device_ths8200_Write8(pObj,0x7C,THS8200_mode[outMode][14]); //ed dtg_vs_in_dly_lsb 
	Device_ths8200_Write8(pObj,0x03,0x00);     
	usleep(10*1000);
	Device_ths8200_Write8(pObj,0x03,0x01); 	

	Device_ths7303_Write8(pObj,0x01, 0xDA);
	Device_ths7303_Write8(pObj,0x02, 0xDA);
	Device_ths7303_Write8(pObj,0x03, 0xDA);
	Device_ths8200_Write8(pObj,0x1c,0x50);

		
	
	return (retVal);
}
/*
  Set Mode
*/
static Int32 Device_ths8200SetMode(Device_Ths8200Handle handle,
                                Ptr cmdArgs,
                                Ptr cmdStatusArgs)
{
	Int32 retVal = 0;
	Int32 outMode=-1;
	Device_Ths8200Obj *pObj = (Device_Ths8200Obj *)handle;

	outMode = *(Int32 *)cmdArgs;

	switch(outMode){
		case DEVICE_STD_VGA_1024X768X60:
			printf("Device_ths8200SetMode outMode = DEVICE_STD_VGA_1024X768X60!\n");
			outMode = THS8200_VGA_1024X768X60;
			break;
		case DEVICE_STD_VGA_1280X720X60:
			printf("Device_ths8200SetMode outMode = DEVICE_STD_VGA_1280X720X60!\n");
			outMode = THS8200_VGA_1280X720X60;
			break;
		case DEVICE_STD_VGA_1280X1024X60:
			printf("Device_ths8200SetMode outMode = DEVICE_STD_VGA_1280X1024X60!\n");
			outMode = THS8200_VGA_1280X1024X60;
			break;
		case DEVICE_STD_VGA_1366X768X60:
			printf("Device_ths8200SetMode outMode = DEVICE_STD_VGA_1366X768X60!\n");
			outMode = THS8200_VGA_1366X768X60;
			break;
		case DEVICE_STD_VGA_1280X800X60:
			printf("Device_ths8200SetMode outMode = DEVICE_STD_VGA_1280X800X60!\n");
			outMode = THS8200_VGA_1280X800X60;
			break;
		case DEVICE_STD_VGA_1440X900X60:
			printf("Device_ths8200SetMode outMode = DEVICE_STD_VGA_1440X900X60!\n");
			outMode = THS8200_VGA_1440X900X60;
			break;
		case DEVICE_STD_VGA_1400X1050X60:
			printf("Device_ths8200SetMode outMode = DEVICE_STD_VGA_1400X1050X60!\n");
			outMode = THS8200_VGA_1400X1050X60;
			break;
		case DEVICE_STD_720P_60:
			printf("Device_ths8200SetMode outMode = DEVICE_STD_720P_60!\n");
			outMode = THS8200_VIDEO_720P60;
			break;
		case DEVICE_STD_1080P_60:
			printf("Device_ths8200SetMode outMode = DEVICE_STD_1080P_60!\n");
			outMode = THS8200_VIDEO_1080P60;
			break;
		default:
			outMode = -1;
			printf("You Set Out Put Mode not support now!\n");
		}

	if(outMode != -1){
		if(outMode >= THS8200_VIDEO_720P60){
			//VIDEO
			Device_ths8200OutPutYPbPr(handle, outMode);
		}else{
			//VGA
			Device_ths8200OutPutVGA(handle, outMode);
		}
	}

	return (retVal);
}



static Int32 Device_ths8200GetHpd(Device_Ths8200Handle handle,
                               Ptr cmdArgs,
                               Ptr cmdStatusArgs)
{
	Int32 retVal = 0;
	return (retVal);
}



/*
  Device Init
*/
static Int32 Device_ths8200DeviceInit(Device_Ths8200Obj* pObj)
{
	Int32 status = 0;
	Int32 i;
	unsigned char regVal=0, regAddr=0;


		printf("Call Device_ths8200DeviceInit()!\n");
#if 1
	Device_ths8200_Write8(pObj,0x04,0x81); // csc_ric1            
	Device_ths8200_Write8(pObj,0x05,0xd5); // csc_rfc1            
	Device_ths8200_Write8(pObj,0x06,0x00); // csc_ric2            
	Device_ths8200_Write8(pObj,0x07,0x00); // csc_rfc2            
	Device_ths8200_Write8(pObj,0x08,0x06); // csc_ric3            
	Device_ths8200_Write8(pObj,0x09,0x29); // csc_rfc3            
	Device_ths8200_Write8(pObj,0x0A,0x04); // csc_gic1            
	Device_ths8200_Write8(pObj,0x0B,0x00); // csc_gfc1            
	Device_ths8200_Write8(pObj,0x0C,0x04); // csc_gic2            
	Device_ths8200_Write8(pObj,0x0D,0x80); // csc_gfc2            
	Device_ths8200_Write8(pObj,0x0E,0x04); // csc_gic3            
	Device_ths8200_Write8(pObj,0x0F,0x00); // csc_gfc3            
	Device_ths8200_Write8(pObj,0x10,0x80); // csc_bic1            
	Device_ths8200_Write8(pObj,0x11,0xbb); // csc_bfc1            
	Device_ths8200_Write8(pObj,0x12,0x07); // csc_bic2            
	Device_ths8200_Write8(pObj,0x13,0x42); // csc_bfc2            
	Device_ths8200_Write8(pObj,0x14,0x00); // csc_bic3            
	Device_ths8200_Write8(pObj,0x15,0x00); // csc_bfc3            
	Device_ths8200_Write8(pObj,0x16,0x14); // csc_offset1         
	Device_ths8200_Write8(pObj,0x17,0xae); // csc_offset12        
	Device_ths8200_Write8(pObj,0x18,0x8b); // csc_offset23        
	Device_ths8200_Write8(pObj,0x19,0x14); // csc_offset3  
#endif
	Device_ths8200_Write8(pObj,0x19,0x16); // csc_offset3 
#if 0
	printf("Call Device_ths8200DeviceInit()!\n");
	Device_ths8200_Write8(pObj,0x04,0x82); // csc_ric1            
	Device_ths8200_Write8(pObj,0x05,0xc9); // csc_rfc1            
	Device_ths8200_Write8(pObj,0x06,0x80); // csc_ric2            
	Device_ths8200_Write8(pObj,0x07,0x07); // csc_rfc2            
	Device_ths8200_Write8(pObj,0x08,0x05); // csc_ric3            
	Device_ths8200_Write8(pObj,0x09,0x7d); // csc_rfc3            
	Device_ths8200_Write8(pObj,0x0A,0x04); // csc_gic1            
	Device_ths8200_Write8(pObj,0x0B,0x00); // csc_gfc1            
	Device_ths8200_Write8(pObj,0x0C,0x04); // csc_gic2            
	Device_ths8200_Write8(pObj,0x0D,0x00); // csc_gfc2            
	Device_ths8200_Write8(pObj,0x0E,0x04); // csc_gic3            
	Device_ths8200_Write8(pObj,0x0F,0x00); // csc_gfc3            
	Device_ths8200_Write8(pObj,0x10,0x81); // csc_bic1            
	Device_ths8200_Write8(pObj,0x11,0x4d); // csc_bfc1            
	Device_ths8200_Write8(pObj,0x12,0x06); // csc_bic2            
	Device_ths8200_Write8(pObj,0x13,0xfa); // csc_bfc2            
	Device_ths8200_Write8(pObj,0x14,0x00); // csc_bic3            
	Device_ths8200_Write8(pObj,0x15,0x00); // csc_bfc3            
	Device_ths8200_Write8(pObj,0x16,0x20); // csc_offset1         
	Device_ths8200_Write8(pObj,0x17,0xad); // csc_offset12        
	Device_ths8200_Write8(pObj,0x18,0xea); // csc_offset23        
	Device_ths8200_Write8(pObj,0x19,0xbd); // csc_offset3  

#endif
#if 0

	Device_ths8200_Write8(pObj,0x1A,0x00); // tst_cntl        
	Device_ths8200_Write8(pObj,0x4A,0x00); // csm_mult_gy_msb     
	Device_ths8200_Write8(pObj,0x83,0x00); // cgms_header         
	Device_ths8200_Write8(pObj,0x84,0x00); // cgms_payload_msb    
	Device_ths8200_Write8(pObj,0x85,0x00); // cgms_payload_lsb    
	Device_ths8200_Write8(pObj,0x38, 0x80);  // 1080P Mode
	Device_ths8200_Write8(pObj,0x82, 0x6f);
	Device_ths8200_Write8(pObj,0x1c, 0x5b);  // 20-bit YCrCb
	Device_ths8200_Write8(pObj,0x34, 0x08);  // Pixels per line
	Device_ths8200_Write8(pObj,0x35, 0x98);
	Device_ths8200_Write8(pObj,0x39, 0x44);  // Lines in frame
	Device_ths8200_Write8(pObj,0x3a, 0x65); 
	Device_ths8200_Write8(pObj,0x3b, 0xff);  
	Device_ths8200_Write8(pObj,0x7a, 0x80);
	Device_ths8200_Write8(pObj,0x7C, 0x01);
	Device_ths8200_Write8(pObj,0x03, 0x01);
//#else	 

	Device_ths8200_Write8(pObj,0x03, 0x00);
	Device_ths8200_Write8(pObj,0x38, 0x82);  // 720P Mode
	Device_ths8200_Write8(pObj,0x82, 0xE0);  // Ignore FID
	//Device_ths8200_Write8(pObj,0x1c, 0x03);  // 20-bit YCrCb
	Device_ths8200_Write8(pObj,0x1c, 0x1b);  // 20-bit YCrCb
	Device_ths8200_Write8(pObj,0x19, 0x03);
	Device_ths8200_Write8(pObj,0x34, 0x06);  // Pixels per line
	Device_ths8200_Write8(pObj,0x35, 0x72);
	Device_ths8200_Write8(pObj,0x39, 0x22);  // Lines in frame
	Device_ths8200_Write8(pObj,0x3a, 0xee);
	Device_ths8200_Write8(pObj,0x3b, 0xff);
	Device_ths8200_Write8(pObj,0x7a, 0x80);  // Horizontal offset
	Device_ths8200_Write8(pObj,0x03, 0x00);  // SW reset to reload parameters
	Device_ths8200_Write8(pObj,0x03, 0x01);
#endif	

#if 0
	Device_ths8200_Write8(pObj,0x03,0x01);
	Device_ths8200_Write8(pObj,0x1A,0x00); // tst_cntl        
	Device_ths8200_Write8(pObj,0x4A,0x00); // csm_mult_gy_msb     
	Device_ths8200_Write8(pObj,0x83,0x00); // cgms_header         
	Device_ths8200_Write8(pObj,0x84,0x00); // cgms_payload_msb    
	Device_ths8200_Write8(pObj,0x85,0x00); // cgms_payload_lsb    

	Device_ths8200_Write8(pObj,0x38,0x87);  // 720P Mode
	Device_ths8200_Write8(pObj,0x82,0x57);  // Ignore FID RGB mode_6f
	Device_ths8200_Write8(pObj,0x1c,0x58);//FSADJ2 terminal
	Device_ths8200_Write8(pObj,0x34,0x06);  // Pixels per line
	Device_ths8200_Write8(pObj,0x35,0x72);
	Device_ths8200_Write8(pObj,0x39,0x22);  // Lines in frame_22
	Device_ths8200_Write8(pObj,0x3a,0xee);
	Device_ths8200_Write8(pObj,0x3b,0xff);
	Device_ths8200_Write8(pObj,0x7a,0x4a);  // Horizontal offset/60

	Device_ths8200_Write8(pObj,0x1b,0x00); // tst_cntl        
	Device_ths8200_Write8(pObj,0x36,0x00); // csm_mult_gy_msb     
	Device_ths8200_Write8(pObj,0x37,0x01); // cgms_header         
	Device_ths8200_Write8(pObj,0x41,0x40); // cgms_payload_msb    
	Device_ths8200_Write8(pObj,0x42,0x40); // cgms_payload_lsb    

	Device_ths8200_Write8(pObj,0x43,0x40);
	Device_ths8200_Write8(pObj,0x44,0x53);  // 720P Mode
	Device_ths8200_Write8(pObj,0x45,0x3f);  // Ignore FID RGB mode
	Device_ths8200_Write8(pObj,0x46,0x3f);//FSADJ2 terminal
	Device_ths8200_Write8(pObj,0x4f,0x00);  // Pixels per line/06
	Device_ths8200_Write8(pObj,0x70,0x60);
	Device_ths8200_Write8(pObj,0x71,0x00);  // Lines in frame
	Device_ths8200_Write8(pObj,0x72,0x02);
	Device_ths8200_Write8(pObj,0x73,0x1c);//03
	Device_ths8200_Write8(pObj,0x74,0x00);  
	Device_ths8200_Write8(pObj,0x75,0x03);
	Device_ths8200_Write8(pObj,0x79,0x00);
	Device_ths8200_Write8(pObj,0x7b,0x00);
	Device_ths8200_Write8(pObj,0x7c,0x01);//01

//	Device_ths8200_Write8(pObj,0x03,0x00);
//	sleep(1);
//	Device_ths8200_Write8(pObj,0x03,0x01);
	
	Device_ths7303_Write8(pObj,0x01, 0xDA);
	Device_ths7303_Write8(pObj,0x02, 0xDA);
	Device_ths7303_Write8(pObj,0x03, 0xDA);
#endif	
	return status;
}

/*
  Device Res-set
*/
static Int32 Device_ths8200Reset(Device_Ths8200Obj* pObj)
{
	Int32 status = 0;
	return status;
}

/*
  Device Enable
*/
static Int32 Device_ths8200Enable(Device_Ths8200Obj* pObj)
{
	Int32 status = 0;
	return status;
}


/*
  Device Power up Transmitter
*/
static Int32 Device_ths8200PowerUpTxm(Device_Ths8200Obj* pObj)
{
	Int32 status = 0;
	return status;
}


/*
  Configure Input Bus and pixel repetation
*/
static Int32 Device_ths8200CfgInBus(Device_Ths8200Obj* pObj)
{
	Int32 status = 0;
	return status;
}


/*
  Configure YC Mode
*/
static Int32 Device_ths8200CfgYcMode(Device_Ths8200Obj* pObj)
{
	Int32 status = 0;
	return status;
}


/*
  Configure Sync Mode
*/
static Int32 Device_ths8200CfgSyncMode(Device_Ths8200Obj* pObj)
{
	Int32 status = 0;
	return status;
}


/*
  Get TP5158 chip ID, revision ID and firmware patch ID
*/
static Int32 Device_ths8200GetHdmiChipId(Device_Ths8200Obj* pObj,
                                   Device_Ths8200HdmiChipId *hdmichipId)
{
	Int32 status = 0;
	return (status);
}


/*
 * Program Timing information for the separate sync input
 */
static Int32 Device_ths8200PrgmExtSyncTimingInfo(
            Device_Ths8200Obj *pObj,
            Device_THS8200ModeInfo *siModeInfo)
{
	Int32 retVal;
	return (retVal);
}


static Int32 Device_ths8200PrgmEmbSyncTimingInfo(
            Device_Ths8200Obj *pObj,
            Device_THS8200ModeInfo *siModeInfo)
{
	Int32 retVal;
	return (retVal);
}

/* Program AVInfo Frame */
static Int32 Device_ths8200PrgmAvInfoFrame(Device_Ths8200Obj *pObj,
                                        Device_THS8200ModeInfo *modeInfo)
{
	Int32 retVal = 0;
	return (retVal);
}

static Int32 Device_ths8200PrgmMdResetRegs(Device_Ths8200Obj *pObj,
                                        Device_THS8200ModeInfo *siModeInfo)
{
	Int32 retVal = 0;
	return (retVal);
}

static Int32 Device_ths8200CalcCRC(UInt8 *regAddr, UInt8 *regValue, UInt32 *regCnt)
{
	Int32 retVal = 0;
	return (retVal);
}

static Int32 Device_ths8200SetPrms(Device_Ths8200Handle handle,
                               Ptr cmdArgs,
                               Ptr cmdStatusArgs)
{
	Int32 retVal = 0;
	return (retVal);
}

static Int32 Device_ths8200GetPrms(Device_Ths8200Handle handle,
                               Ptr cmdArgs,
                               Ptr cmdStatusArgs)
{
	Int32 retVal = 0;
	return (retVal);
}

