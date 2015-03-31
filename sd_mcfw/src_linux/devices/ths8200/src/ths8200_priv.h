/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpsdrv_hdmi9889Priv.h
 *
 *  \brief HDMI9889 driver
 *  This file implements functionality for the HDMI.
 *
 */

#ifndef _DEVICE_THS8200_PRIV_H_
#define _DEVICE_THS8200_PRIV_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <device_videoEncoder.h>
//#include <device_ths8200.h>
#include <device_ad9889.h>

#include <osa_sem.h>
#include <osa_i2c.h>




/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief THS8200 handle state - not used. */
#define VPS_THS8200_OBJ_STATE_UNUSED   (0)

/** \brief THS8200 handle state -  used */
#define VPS_THS8200_OBJ_STATE_USED     (1)

/** \brief THS8200 device state -  initilized succefully */
#define VPS_THS8200_NOT_INITED         (0)

/** \brief THS8200 device state -  initilized succefully */
#define VPS_THS8200_INITED             (1)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 * \brief HDMI Object.
 */
typedef struct
{
    UInt32 instId;
    /* instance id */
    Device_VideoEncoderCreateParams createArgs;
    /* create time arguments */
    Device_Ths8200HdmiChipId  hdmiChipid;
    /* HDMI Chip id */
    UInt32  hpd;
    /* HDMI Hot plug detect */
    UInt32 syncCfgReg;
    /**< Sync Configuration Register */
    UInt32 syncPolarityReg;
    UInt32 isRgbOutput;
    UInt32 inBusCfg;
} Device_Ths8200Obj;

typedef Device_Ths8200Obj * Device_Ths8200Handle;

/*
  Overall HDMI driver object
*/
typedef struct
{
    OSA_I2cHndl i2cHandle;
    /** < i2c handle for i2c read/write*/
//    Device_Ths8200Handle ths8200Handle[DEVICE_MAX_HANDLES];
    Device_Ths8200Handle ths8200Handle[1];
    /** < HDMI handle objects */
    Device_Ths8200Prms prms;
    /**< THS8200 params */
} Device_Ths8200CommonObj;

Device_Ths8200CommonObj gDevice_ths8200CommonObj;

#endif

