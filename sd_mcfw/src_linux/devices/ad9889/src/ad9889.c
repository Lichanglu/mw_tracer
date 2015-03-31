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
#include <device_ad9889.h>
#include <ad9889_priv.h>
#include <osa_i2c.h>



/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
#define TPI_INPUTBUS_PIXEL_REP_BIT4_MASK                (0x10u)

#define AD9889_IIC_SLAVE_ADDR						(0x39)
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
} Device_AD9889ModeInfo;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Int32 Device_ad9889GetHdmiChipId(Device_Ad9889Obj* psiiObj,
                                   Device_HdmiChipId *hdmichipId);

static Int32 Device_ad9889GetDetailedChipId(Device_Ad9889Handle handle,
                                          Ptr cmdArgs,
                                          Ptr cmdStatusArgs);

static Int32 Device_ad9889SetMode(Device_Ad9889Handle handle,
                                Ptr cmdArgs,
                                Ptr cmdStatusArgs);

static Int32 Device_ad9889Start(Device_Ad9889Handle handle,
                              Ptr cmdArgs,
                              Ptr cmdStatusArgs);
static Int32 Device_ad9889Stop(Device_Ad9889Handle handle,
                             Ptr cmdArgs,
                             Ptr cmdStatusArgs);

static Int32 Device_ad9889GetHpd(Device_Ad9889Handle handle,
                               Ptr cmdArgs,
                               Ptr cmdStatusArgs);

static Int32 Device_ad9889SetPrms(Device_Ad9889Handle handle,
                               Ptr cmdArgs,
                               Ptr cmdStatusArgs);
static Int32 Device_ad9889GetPrms(Device_Ad9889Handle handle,
                               Ptr cmdArgs,
                               Ptr cmdStatusArgs);
static Int32 Device_ad9889DeviceInit(Device_Ad9889Obj* pObj);

static Int32 Device_ad9889Reset(Device_Ad9889Obj* pObj);
static Int32 Device_ad9889Enable(Device_Ad9889Obj* pObj);
static Int32 Device_ad9889PowerUpTxm(Device_Ad9889Obj* pObj);
static Int32 Device_ad9889CfgInBus(Device_Ad9889Obj* pObj);
static Int32 Device_ad9889CfgYcMode(Device_Ad9889Obj* pObj);
static Int32 Device_ad9889CfgSyncMode(Device_Ad9889Obj* pObj);
static Int32 Device_ad9889PrgmEmbSyncTimingInfo(
                Device_Ad9889Obj *siiObj,
                Device_AD9889ModeInfo *siModeInfo);
static Int32 Device_ad9889PrgmExtSyncTimingInfo(
                Device_Ad9889Obj *siiObj,
                Device_AD9889ModeInfo *siModeInfo);
static Int32 Device_ad9889PrgmAvInfoFrame(
                Device_Ad9889Obj *siiObj,
                Device_AD9889ModeInfo *modeInfo);
static Int32 Device_ad9889PrgmMdResetRegs(
                Device_Ad9889Obj *siiObj,
                Device_AD9889ModeInfo *siModeInfo);
static Int32 Device_ad9889CalcCRC(UInt8 *regAddr, UInt8 *regValue, UInt32 *regCnt);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

Device_AD9889ModeInfo AD9889ModeInfo[] =
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

unsigned char AD9889_ycbcr[24] = {0x0C,0x52,0x08,0x00,0x00,0x00,0x19,0xD7,0x1C,0x54,0x08,0x00,0x1E,0x89,0x02,0x91,0x00,0x00,0x08,0x00,0x0E,0x87,0x18,0xBC};

#define DEVICE_AD9889_MAX_MODES  (sizeof(AD9889ModeInfo) /                     \
                                 sizeof(Device_AD9889ModeInfo))


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
UInt8 Device_ad9889_Read8(Device_Ad9889Obj * pObj, UInt8 RegAddr)
{
	Int32 retVal = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	retVal = OSA_i2cRead8(&gDevice_ad9889CommonObj.i2cHandle,AD9889_IIC_SLAVE_ADDR,regAddr,regValue,1);

	return regValue[0];
}

Int32 Device_ad9889_Write8(Device_Ad9889Obj * pObj, UInt8 RegAddr, UInt8 RegVal)
{
	Int32 retVal = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};
	
	regAddr[0] = RegAddr;
	regValue[0] = RegVal;
	
	retVal = OSA_i2cWrite8 (&gDevice_ad9889CommonObj.i2cHandle, AD9889_IIC_SLAVE_ADDR, regAddr, regValue,1);
}

Int32 Device_ad9889Init(void)
{
	/* Set to 0's for global object, descriptor memory  */
	memset(&gDevice_ad9889CommonObj, 0, sizeof (Device_Ad9889CommonObj));
	return 0;
}

Int32 Device_ad9889DeInit( void )
{
	return 0;
}

Device_Ad9889Handle Device_ad9889Create (UInt32 drvId,
                                 UInt32 instId,
                                 Ptr createArgs,
                                 Ptr createStatusArgs)
{

	Device_Ad9889Obj*  pObj;
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
		pObj = (Device_Ad9889Obj *)malloc(sizeof(Device_Ad9889Obj));

		if ( pObj == NULL )
		{
			return NULL;
		}
		else
		{
			memset(pObj, 0, sizeof(Device_Ad9889Obj));

			gDevice_ad9889CommonObj.ad9889Handle[instId] = pObj;

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
		gDevice_ad9889CommonObj.prms.outputFormat = DEVICE_AD9889_HDMI_RGB;
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

		retVal = OSA_i2cOpen(&(gDevice_ad9889CommonObj.i2cHandle), I2C_DEFAULT_INST_ID);

		/* Enable DE in syncpolarity register at 0x63 */
		pObj->syncPolarityReg = 0x40;
		Device_ad9889DeviceInit(pObj);
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


Int32 Device_ad9889Delete(Device_Ad9889Handle handle, Ptr deleteArgs)
{
    Device_Ad9889Obj* pObj= (Device_Ad9889Obj*)handle;

    if ( pObj == NULL )
        return -1;

    OSA_i2cClose(&(gDevice_ad9889CommonObj.i2cHandle));

    /*
     * free driver handle object
     */
    free(pObj);

    return 0;
}

Int32 Device_ad9889Control (Device_Ad9889Handle handle,
                              UInt32 cmd,
                              Ptr cmdArgs,
                              Ptr cmdStatusArgs)
{
    Device_Ad9889Obj *pObj = (Device_Ad9889Obj *)handle;
    Int32 status;

    if ( handle == NULL )
        return -1;

    /*lock handle    */

    switch (cmd)
    {
        case DEVICE_CMD_START:
            status = Device_ad9889Start(pObj, cmdArgs, cmdStatusArgs);
            break;

        case DEVICE_CMD_STOP:
            status = Device_ad9889Stop(pObj, cmdArgs, cmdStatusArgs);
            break;

        case IOCTL_DEVICE_VIDEO_ENCODER_GET_CHIP_ID:
            status = -1;
            break;

        case IOCTL_DEVICE_VIDEO_ENCODER_SET_MODE:
            status = Device_ad9889SetMode(pObj, cmdArgs, cmdStatusArgs);
            break;

        case IOCTL_DEVICE_AD9889_GET_DETAILED_CHIP_ID:
            status = Device_ad9889GetDetailedChipId(handle,
                                                  cmdArgs,
                                                  cmdStatusArgs);
            break;

        case IOCTL_DEVICE_AD9889_QUERY_HPD:
            status = Device_ad9889GetHpd(pObj, cmdArgs, cmdStatusArgs);
            break;

        case IOCTL_DEVICE_AD9889_SET_PARAMS:
            status = Device_ad9889SetPrms(pObj, cmdArgs, cmdStatusArgs);
            break;

        case IOCTL_DEVICE_AD9889_GET_PARAMS:
            status = Device_ad9889GetPrms(pObj, cmdArgs, cmdStatusArgs);
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
static Int32 Device_ad9889GetDetailedChipId(Device_Ad9889Handle handle, Ptr cmdArgs,
                                  Ptr cmdStatusArgs)
{
	Int32 status = 0;
	return (status);
}



/* Enable TMDS output */
static Int32 Device_ad9889Start(Device_Ad9889Handle handle,
                              Ptr cmdArgs,
                              Ptr cmdStatusArgs)
{
	Int32 retVal = 0;
	return (retVal);
}



/* Disable TMDS output */
static Int32 Device_ad9889Stop(Device_Ad9889Handle handle,
                             Ptr cmdArgs,
                             Ptr cmdStatusArgs)
{
	Int32 retVal = 0;
	return (retVal);
}



/*
  Set Mode
*/
static Int32 Device_ad9889SetMode(Device_Ad9889Handle handle,
                                Ptr cmdArgs,
                                Ptr cmdStatusArgs)
{
	Int32 retVal = 0;
	return (retVal);
}



static Int32 Device_ad9889GetHpd(Device_Ad9889Handle handle,
                               Ptr cmdArgs,
                               Ptr cmdStatusArgs)
{
	Int32 retVal = 0;
	return (retVal);
}



/*
  Device Init
*/
static Int32 Device_ad9889DeviceInit(Device_Ad9889Obj* pObj)
{
	Int32 status = 0;
	Int32 i;
	unsigned char regVal=0, regAddr=0;

	regVal = Device_ad9889_Read8(pObj, 0x00);
	regVal = Device_ad9889_Read8(pObj, 0x00);
	OSA_printf(" AD9889 CHIP ID = 0x%x\n",regVal);
	regVal = Device_ad9889_Read8(pObj, 0x42);
	OSA_printf(" AD9889 CHIP ID = 0x%x\n",regVal);
	regVal = Device_ad9889_Read8(pObj, 0x42);
	if(regVal & 0x40)
	{
	//power down
		Device_ad9889_Write8(pObj, 0xA1, 0x38);
	//power up
		Device_ad9889_Write8(pObj, 0x41, 0x10);
		Device_ad9889_Write8(pObj, 0x94, 0x84);
		Device_ad9889_Write8(pObj, 0x95, 0xC3);
	}

	Device_ad9889_Write8(pObj,  0xA3, 0x84);
	Device_ad9889_Write8(pObj,  0x0B, 0x07);
	Device_ad9889_Write8(pObj,  0x15, 0x04);
	Device_ad9889_Write8(pObj,  0x16, 0x30);
	Device_ad9889_Write8(pObj,  0x17, 0x0B);
	Device_ad9889_Write8(pObj,  0x3B, 0x01);

	regAddr = 0x18;
	for(i=0;i<24;i++)
	{
		Device_ad9889_Write8(pObj,  regAddr, AD9889_ycbcr[i]);
		regAddr++;
	}

	Device_ad9889_Write8(pObj,  0x30, 0x1B);
	Device_ad9889_Write8(pObj,  0x31, 0x82);
	Device_ad9889_Write8(pObj,  0x32, 0x80);
	Device_ad9889_Write8(pObj,  0x33, 0x14);
	Device_ad9889_Write8(pObj,  0x34, 0x05);
///////////////////////////////////////////////////////////////////////
//720P-60
	Device_ad9889_Write8(pObj,  0x35, 0x40);
	Device_ad9889_Write8(pObj,  0x36, 0xD9);
///////////////////////////////////////////////////////////////////////
//720P-50
///////////////////////////////////////////////////////////////////////
	Device_ad9889_Write8(pObj,  0x37, 0x0A);//width
	Device_ad9889_Write8(pObj,  0x38, 0x00);
	Device_ad9889_Write8(pObj,  0x39, 0x2D);//height
	Device_ad9889_Write8(pObj,  0x3A, 0x00);
	Device_ad9889_Write8(pObj,  0x3C, 0x04);
	//Device_ad9889_Write8(pObj,  0x44, 0xF8);
	Device_ad9889_Write8(pObj,  0x44, 0xFF);
	Device_ad9889_Write8(pObj,  0x45, 0x08);
	Device_ad9889_Write8(pObj,  0x46, 0x78);
	Device_ad9889_Write8(pObj,  0x47, 0x80);
	Device_ad9889_Write8(pObj,  0xAF, 0x00);
	/*HDMI 0x06 DVI 0x04*/
	//Device_ad9889_Write8(pObj,  0xAF, 0x06);
	//Device_ad9889_Write8(pObj,  0x0A, 0x91);
	Device_ad9889_Write8(pObj,  0x0A, 0x10);
	//Device_ad9889_Write8(pObj,  0x3B, 0x81);
	//Device_ad9889_Write8(pObj,  0x3B, 0x81);
	Device_ad9889_Write8(pObj,  0x3B, 0x01);
	//Device_ad9889_Write8(pObj,  0x3B, 0x81);
	/*sample freqs*/
	//Device_ad9889_Write8(pObj,  0x15, 0x00);
	//Device_ad9889_Write8(pObj,  0x15, 0x20);
	Device_ad9889_Write8(pObj,  0x41, 0x10);
	Device_ad9889_Write8(pObj,  0xBA, 0x60);
	Device_ad9889_Write8(pObj,  0x0B, 0x7);
	
       /*for error codes start*/
	regVal = Device_ad9889_Read8(pObj, 0x0A);
	regVal = regVal&0x9F;
	Device_ad9889_Write8(pObj,  0x0A, regVal);
	Device_ad9889_Write8(pObj,  0x98, 0x0f);//03
	Device_ad9889_Write8(pObj,  0x9C, 0x38);
	regVal = Device_ad9889_Read8(pObj, 0x9D);
	regVal = regVal&0xFC;
	regVal = regVal|0x01;
	Device_ad9889_Write8(pObj,  0x9D, regVal);
	Device_ad9889_Write8(pObj,  0xA2, 0x87);//can be 0x87 for high speed operation(pixel clock>80Mhz)
	Device_ad9889_Write8(pObj,  0xA3, 0x87);//up to 80Mhz 
	Device_ad9889_Write8(pObj,  0xBB, 0xFF);

	Device_ad9889_Write8(pObj,  0x98, 0x03);
	Device_ad9889_Write8(pObj,  0x9c, 0x38);
	Device_ad9889_Write8(pObj,  0x9d, 0x61);
	Device_ad9889_Write8(pObj,  0x9e, 0x12);
	Device_ad9889_Write8(pObj,  0xA2, 0x87);
	Device_ad9889_Write8(pObj,  0xA3, 0x87);
	Device_ad9889_Write8(pObj,  0xbb, 0xff);
	/*for error codes end*/
	Device_ad9889_Write8(pObj,  0xA1, 0x00);//power up
#if 0
//RGB input to AD9889
//	Device_ad9889_Write8(pObj,  0x15, 0x00);
//	Device_ad9889_Write8(pObj,  0x16, 0x00);
#else
//YUV422 input to AD9889
//	Device_ad9889_Write8(pObj,  0x15, 0x02);
//	Device_ad9889_Write8(pObj,  0x16, 0x30);
//	Device_ad9889_Write8(pObj,  0x17, 0x0B);
	
	Device_ad9889_Write8(pObj,  0x15, 0x00);
	Device_ad9889_Write8(pObj,  0x16, 0x00);
	Device_ad9889_Write8(pObj,  0x17, 0x0A);			
#endif
	Device_ad9889_Write8(pObj,  0x3B, 0x80);
	Device_ad9889_Write8(pObj,  0x45, 0x00);
	
	return status;
}

/*
  Device Res-set
*/
static Int32 Device_ad9889Reset(Device_Ad9889Obj* pObj)
{
	Int32 status = 0;
	return status;
}

/*
  Device Enable
*/
static Int32 Device_ad9889Enable(Device_Ad9889Obj* pObj)
{
	Int32 status = 0;
	return status;
}


/*
  Device Power up Transmitter
*/
static Int32 Device_ad9889PowerUpTxm(Device_Ad9889Obj* pObj)
{
	Int32 status = 0;
	return status;
}


/*
  Configure Input Bus and pixel repetation
*/
static Int32 Device_ad9889CfgInBus(Device_Ad9889Obj* pObj)
{
	Int32 status = 0;
	return status;
}


/*
  Configure YC Mode
*/
static Int32 Device_ad9889CfgYcMode(Device_Ad9889Obj* pObj)
{
	Int32 status = 0;
	return status;
}


/*
  Configure Sync Mode
*/
static Int32 Device_ad9889CfgSyncMode(Device_Ad9889Obj* pObj)
{
	Int32 status = 0;
	return status;
}


/*
  Get TP5158 chip ID, revision ID and firmware patch ID
*/
static Int32 Device_ad9889GetHdmiChipId(Device_Ad9889Obj* pObj,
                                   Device_HdmiChipId *hdmichipId)
{
	Int32 status = 0;
	return (status);
}


/*
 * Program Timing information for the separate sync input
 */
static Int32 Device_ad9889PrgmExtSyncTimingInfo(
            Device_Ad9889Obj *pObj,
            Device_AD9889ModeInfo *siModeInfo)
{
	Int32 retVal;
	return (retVal);
}


static Int32 Device_ad9889PrgmEmbSyncTimingInfo(
            Device_Ad9889Obj *pObj,
            Device_AD9889ModeInfo *siModeInfo)
{
	Int32 retVal;
	return (retVal);
}

/* Program AVInfo Frame */
static Int32 Device_ad9889PrgmAvInfoFrame(Device_Ad9889Obj *pObj,
                                        Device_AD9889ModeInfo *modeInfo)
{
	Int32 retVal = 0;
	return (retVal);
}

static Int32 Device_ad9889PrgmMdResetRegs(Device_Ad9889Obj *pObj,
                                        Device_AD9889ModeInfo *siModeInfo)
{
	Int32 retVal = 0;
	return (retVal);
}

static Int32 Device_ad9889CalcCRC(UInt8 *regAddr, UInt8 *regValue, UInt32 *regCnt)
{
	Int32 retVal = 0;
	return (retVal);
}

static Int32 Device_ad9889SetPrms(Device_Ad9889Handle handle,
                               Ptr cmdArgs,
                               Ptr cmdStatusArgs)
{
	Int32 retVal = 0;
	return (retVal);
}

static Int32 Device_ad9889GetPrms(Device_Ad9889Handle handle,
                               Ptr cmdArgs,
                               Ptr cmdStatusArgs)
{
	Int32 retVal = 0;
	return (retVal);
}

