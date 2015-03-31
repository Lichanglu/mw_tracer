/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


#include "ti_media_std.h"
#include <device.h>
#include <device_videoDecoder.h>
#include <device_gs2971.h>
#include <gs2971_priv.h>

/* Global object storing all information related to all
  GS2971 driver handles */

/* Control API that gets called when Device_control is called

  This API does handle level semaphore locking

  handle - GS2971 driver handle
  cmd - command
  cmdArgs - command arguments
  cmdStatusArgs - command status

  returns error in case of
  - illegal parameters
  - I2C command RX/TX error
*/
Int32 Device_gs2971Control ( Device_Gs2971Handle handle,
                           UInt32 cmd, Ptr cmdArgs, Ptr cmdStatusArgs )
{
    Device_Gs2971Obj *pObj = ( Device_Gs2971Obj * ) handle;
    Int32 status;

    if ( pObj == NULL )
        return -1;

    switch ( cmd )
    {
        case DEVICE_CMD_START:
            status = Device_gs2971Start ( pObj );
            break;

        case DEVICE_CMD_STOP:
            status = Device_gs2971Stop ( pObj );
            break;

        case IOCTL_DEVICE_VIDEO_DECODER_GET_CHIP_ID:
            status = Device_gs2971GetChipId ( pObj, cmdArgs, cmdStatusArgs );
            break;

        case IOCTL_DEVICE_VIDEO_DECODER_RESET:
            status = Device_gs2971Reset ( pObj );
            break;

        case IOCTL_DEVICE_VIDEO_DECODER_SET_VIDEO_MODE:
            status = Device_gs2971SetVideoMode ( pObj, cmdArgs );
            break;

        case IOCTL_DEVICE_VIDEO_DECODER_GET_VIDEO_STATUS:
            status = Device_gs2971GetVideoStatus ( pObj, cmdArgs, cmdStatusArgs );
            break;

        case IOCTL_DEVICE_VIDEO_DECODER_SET_VIDEO_COLOR:
            status = Device_gs2971SetVideoColor ( pObj, cmdArgs );
            break;

        case IOCTL_DEVICE_GS2971_SET_VIDEO_NF:
            status = Device_gs2971SetVideoNf ( pObj, cmdArgs );
            break;

        case IOCTL_DEVICE_VIDEO_DECODER_REG_WRITE:
            status = Device_gs2971RegWrite ( pObj, cmdArgs );
            break;

        case IOCTL_DEVICE_VIDEO_DECODER_REG_READ:
            status = Device_gs2971RegRead ( pObj, cmdArgs );
            break;
			
	case IOCTL_DEVICE_VIDEO_DECODER_EXTERN_INFORM:
            status = Device_gs2971GetExternInformation(pObj, cmdArgs);
            break;
			
       default:
            status = -1;
            break;
    }

    return status;
}

/*
  Create API that gets called when Device_create is called

  This API does not configure the GS2971 is any way.

  This API
  - validates parameters
  - allocates driver handle
  - stores create arguments in its internal data structure.

  Later the create arguments will be used when doing I2C communcation with
  GS2971

  drvId - driver ID, must be DEVICE_VID_DEC_GS2971_DRV
  instanceId - must be 0
  createArgs - create arguments
  createStatusArgs - create status
  fdmCbParams  - NOT USED, set to NULL

  returns NULL in case of any error
*/
Device_Gs2971Handle Device_gs2971Create ( UInt8 drvId,
                                            UInt8 instId,
                                            Ptr createArgs,
                                            Ptr createStatusArgs)
{
    Int32 status = 0;
    Device_Gs2971Obj *pObj;
    Device_VideoDecoderCreateParams *vidDecCreateArgs
        = ( Device_VideoDecoderCreateParams * ) createArgs;

    Device_VideoDecoderCreateStatus *vidDecCreateStatus
        = ( Device_VideoDecoderCreateStatus * ) createStatusArgs;

    /*
     * check parameters
     */
    if ( vidDecCreateStatus == NULL )
        return NULL;

    vidDecCreateStatus->retVal = 0;

    if ( vidDecCreateArgs->deviceI2cInstId >= DEVICE_I2C_INST_ID_MAX
         || vidDecCreateArgs->numDevicesAtPort > DEVICE_GS2971_DEV_MAX )
    {
        vidDecCreateStatus->retVal = -1;
        return NULL;
    }

    pObj = (Device_Gs2971Obj *)malloc(sizeof(Device_Gs2971Obj));

    if ( pObj == NULL )
    {
        vidDecCreateStatus->retVal = -1;
        return NULL;
    }
    else
    {
        memset(pObj, 0, sizeof(Device_Gs2971Obj));

        gDevice_gs2971CommonObj.gs2971handle[instId] = pObj;

        /*
         * copy parameters to allocate driver handle
         */
        memcpy ( &pObj->createArgs, vidDecCreateArgs,
                 sizeof ( *vidDecCreateArgs ) );

        status = OSA_i2cOpen(&(gDevice_gs2971CommonObj.i2cHandle), I2C_DEFAULT_INST_ID);

        if ( status < 0)
        {
            vidDecCreateStatus->retVal = -1;
            return NULL;
        }
    }
    /*
     * return driver handle object pointer
     */
    return pObj;
}

/*
  Delete function that is called when Device_delete is called

  This API
  - free's driver handle object

  handle - driver handle
  deleteArgs - NOT USED, set to NULL

*/
Int32 Device_gs2971Delete ( Device_Gs2971Handle handle, Ptr deleteArgs )
{
    Device_Gs2971Obj *pObj = ( Device_Gs2971Obj * ) handle;

    if ( pObj == NULL )
        return -1;


    OSA_i2cClose(&(gDevice_gs2971CommonObj.i2cHandle));


    /*
     * free driver handle object
     */
    free(pObj);

    return 0;
}

Int32 Device_gs2971_SetFPGAMode(int fpgaMode)
{
    UInt8 regAddr[1]={0};
    UInt8 regValue[2]={0};
    unsigned short data;
	
    regAddr[0] = 14*2;
    OSA_CPLD_i2cRead16(&gDevice_gs2971CommonObj.i2cHandle,0, regAddr, regValue, 2);
    regValue[0] |= (1<<2);
    OSA_CPLD_i2cWrite16(&gDevice_gs2971CommonObj.i2cHandle,0, regAddr, regValue, 2);
    sleep(1)	;
    OSA_CPLD_i2cRead16(&gDevice_gs2971CommonObj.i2cHandle,0, regAddr, regValue, 2);
    data = regValue[1]<<8|regValue[0];
    printf("8-Device_gs2971Init() FPGA %d = 0x%x 0x%x\n", regAddr[0], regValue[1], regValue[0]);

    return 0;	
}
/*
  System init for GS2971 driver

  This API
  - create semaphore locks needed
  - registers driver to FVID2 sub-system
  - gets called as part of Device_deviceInit()

*/
Int32 Device_gs2971Init (  )
{
    /*
     * Set to 0's for global object, descriptor memory
     */
    memset ( &gDevice_gs2971CommonObj, 0, sizeof ( gDevice_gs2971CommonObj ) );

    return 0;
}

Int32 Device_gs2971DeInit (  )
{
    /*
     * Set to 0's for global object, descriptor memory
     */
    return 0;
}

