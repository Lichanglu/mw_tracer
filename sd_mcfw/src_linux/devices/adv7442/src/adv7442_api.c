/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


#include "ti_media_std.h"
#include <device.h>
#include <device_videoDecoder.h>
#include <device_adv7442.h>
#include <adv7442_priv.h>

/* Global object storing all information related to all
  ADV7442 driver handles */

/* Control API that gets called when Device_control is called

  This API does handle level semaphore locking

  handle - ADV7442 driver handle
  cmd - command
  cmdArgs - command arguments
  cmdStatusArgs - command status

  returns error in case of
  - illegal parameters
  - I2C command RX/TX error
*/
Int32 Device_adv7442Control ( Device_Adv7442Handle handle,
                           UInt32 cmd, Ptr cmdArgs, Ptr cmdStatusArgs )
{
    Device_Adv7442Obj *pObj = ( Device_Adv7442Obj * ) handle;
    Int32 status;

    if ( pObj == NULL )
        return -1;

    switch ( cmd )
    {
        case DEVICE_CMD_START:
            status = Device_adv7442Start ( pObj );
            break;

        case DEVICE_CMD_STOP:
            status = Device_adv7442Stop ( pObj );
            break;

        case IOCTL_DEVICE_VIDEO_DECODER_GET_CHIP_ID:
            status = Device_adv7442GetChipId ( pObj, cmdArgs, cmdStatusArgs );
            break;

        case IOCTL_DEVICE_VIDEO_DECODER_RESET:
            status = Device_adv7442Reset ( pObj );
            break;

        case IOCTL_DEVICE_VIDEO_DECODER_SET_VIDEO_MODE:
            status = Device_adv7442SetVideoMode ( pObj, cmdArgs );
            break;

        case IOCTL_DEVICE_VIDEO_DECODER_GET_VIDEO_STATUS:
            status = Device_adv7442GetVideoStatus ( pObj, cmdArgs, cmdStatusArgs );
            break;
			
	case IOCTL_DEVICE_VIDEO_GET_FPGA_VERSION:		
		status = Device_adv7442GetFPGAVersion(pObj);
	break;

        case IOCTL_DEVICE_VIDEO_DECODER_SET_VIDEO_COLOR:
            status = Device_adv7442SetVideoColor ( pObj, cmdArgs );
            break;

        case IOCTL_DEVICE_ADV7442_SET_VIDEO_NF:
            status = Device_adv7442SetVideoNf ( pObj, cmdArgs );
            break;

        case IOCTL_DEVICE_VIDEO_DECODER_REG_WRITE:
            status = Device_adv7442RegWrite ( pObj, cmdArgs );
            break;

        case IOCTL_DEVICE_VIDEO_DECODER_REG_READ:
            status = Device_adv7442RegRead ( pObj, cmdArgs );
            break;
			
        case IOCTL_DEVICE_VIDEO_DECODER_SET_DIRECTION:
            status = Device_adv7442SetDirection(pObj, cmdArgs);
            break;
			
        case IOCTL_DEVICE_VIDEO_DECODER_GET_DIRECTION:
            status = Device_adv7442GetDirection(pObj, cmdArgs);
            break;
			
         case IOCTL_DEVICE_VIDEO_DECODER_SET_SOURCE_CHAN:
            status = Device_adv7442SetSourceChan(pObj, cmdArgs);
            break;
			
        case IOCTL_DEVICE_VIDEO_DECODER_GET_SOURCE_CHAN:
            status = Device_adv7442GetSourceChan(pObj, cmdArgs);
            break;
			
       case IOCTL_DEVICE_VIDEO_DECODER_INVERT_CBCR:
            status = Device_adv7442InvertCbCr(pObj);
            break;
			
	case IOCTL_DEVICE_VIDEO_DECODER_CSC:			
            status = Device_adv7442_CSC(pObj,cmdArgs);
            break;		
			
         case IOCTL_DEVICE_VIDEO_DECODER_GET_HDCP:
            status = Device_adv7442GetHDCP(pObj, cmdArgs);
            break;
			
        case IOCTL_DEVICE_VIDEO_DECODER_EXTERN_INFORM:
            status = Device_adv7442GetExternInformation(pObj, cmdArgs);
            break;
			
       default:
            status = -1;
            break;
    }

    return status;
}

/*
  Create API that gets called when Device_create is called

  This API does not configure the ADV7442 is any way.

  This API
  - validates parameters
  - allocates driver handle
  - stores create arguments in its internal data structure.

  Later the create arguments will be used when doing I2C communcation with
  ADV7442

  drvId - driver ID, must be DEVICE_VID_DEC_ADV7442_DRV
  instanceId - must be 0
  createArgs - create arguments
  createStatusArgs - create status
  fdmCbParams  - NOT USED, set to NULL

  returns NULL in case of any error
*/
Device_Adv7442Handle Device_adv7442Create ( UInt8 drvId,
                                            UInt8 instId,
                                            Ptr createArgs,
                                            Ptr createStatusArgs)
{
    Int32 status = 0;
    Device_Adv7442Obj *pObj;
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
         || vidDecCreateArgs->numDevicesAtPort > DEVICE_ADV7442_DEV_MAX )
    {
        vidDecCreateStatus->retVal = -1;
        return NULL;
    }

    pObj = (Device_Adv7442Obj *)malloc(sizeof(Device_Adv7442Obj));

    if ( pObj == NULL )
    {
        vidDecCreateStatus->retVal = -1;
        return NULL;
    }
    else
    {
        memset(pObj, 0, sizeof(Device_Adv7442Obj));

        gDevice_adv7442CommonObj.adv7442handle[instId] = pObj;

        /*
         * copy parameters to allocate driver handle
         */
        memcpy ( &pObj->createArgs, vidDecCreateArgs,
                 sizeof ( *vidDecCreateArgs ) );

        status = OSA_i2cOpen(&(gDevice_adv7442CommonObj.i2cHandle), I2C_DEFAULT_INST_ID);

        if ( status < 0)
        {
            vidDecCreateStatus->retVal = -1;
            return NULL;
        }
    }
    /*
     * return driver handle object pointer
     */
    Device_adv7442_SetFPGAMode(0);
	
    return pObj;
}

/*
  Delete function that is called when Device_delete is called

  This API
  - free's driver handle object

  handle - driver handle
  deleteArgs - NOT USED, set to NULL

*/
Int32 Device_adv7442Delete ( Device_Adv7442Handle handle, Ptr deleteArgs )
{
    Device_Adv7442Obj *pObj = ( Device_Adv7442Obj * ) handle;

    if ( pObj == NULL )
        return -1;


    OSA_i2cClose(&(gDevice_adv7442CommonObj.i2cHandle));


    /*
     * free driver handle object
     */
    free(pObj);

    return 0;
}

Int32 Device_adv7442_SetFPGAMode(int fpgaMode)
{
    UInt8 regAddr[1]={0};
    UInt8 regValue[2]={0};
    unsigned short data;
	
    regAddr[0] = 14*2;
    OSA_CPLD_i2cRead16(&gDevice_adv7442CommonObj.i2cHandle,0, regAddr, regValue, 2);
    regValue[0] |= (1<<1);
    OSA_CPLD_i2cWrite16(&gDevice_adv7442CommonObj.i2cHandle,0, regAddr, regValue, 2);
	
    OSA_CPLD_i2cRead16(&gDevice_adv7442CommonObj.i2cHandle,0, regAddr, regValue, 2);
    data = regValue[1]<<8|regValue[0];
    printf("Device_adv7442Init() FPGA %d = 0x%x\n", regAddr[0], data);

    return 0;
}
/*
  System init for ADV7442 driver

  This API
  - create semaphore locks needed
  - registers driver to FVID2 sub-system
  - gets called as part of Device_deviceInit()

*/
Int32 Device_adv7442Init (  )
{
    /*
     * Set to 0's for global object, descriptor memory
     */
    memset ( &gDevice_adv7442CommonObj, 0, sizeof ( gDevice_adv7442CommonObj ) );

    return 0;
}

Int32 Device_adv7442DeInit (  )
{
    /*
     * Set to 0's for global object, descriptor memory
     */
    return 0;
}

