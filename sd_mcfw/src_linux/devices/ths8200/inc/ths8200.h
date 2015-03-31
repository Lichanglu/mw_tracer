/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
*  \file vpsdrv_hdmi9889.h
*
*  \brief .
*
*/

#ifndef _DEVICE_THS8200_H_
#define _DEVICE_THS8200_H_


/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

// #include <ti/psp/devices/vps_videoEncoder.h>
// #include <ti/psp/vps/drivers/fvid2_drvMgr.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */



/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

Int32 Device_ths8200Init(void);
Int32 Device_ths8200DeInit(void);

Device_Ad9889Handle Device_ths8200Create (UInt32 drvId,
                                               UInt32 instanceId,
                                               Ptr createArgs,
                                               Ptr createStatusArgs);
                                               //,
                                               //const FVID2_DrvCbParams * fdmCbParams);

Int32 Device_ths8200Delete(Device_Ad9889Handle handle, Ptr deleteArgs);

Int32 Device_ths8200Control (Device_Ad9889Handle handle,
                              UInt32 cmd,
                              Ptr cmdArgs,
                              Ptr cmdStatusArgs);

#endif

