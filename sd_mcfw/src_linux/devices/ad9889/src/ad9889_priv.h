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

#ifndef _DEVICE_AD9889_PRIV_H_
#define _DEVICE_AD9889_PRIV_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <device_videoEncoder.h>
#include <device_ad9889.h>

#include <osa_sem.h>
#include <osa_i2c.h>




/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief AD9889 handle state - not used. */
#define VPS_AD9889_OBJ_STATE_UNUSED   (0)

/** \brief AD9889 handle state -  used */
#define VPS_AD9889_OBJ_STATE_USED     (1)

/** \brief AD9889 device state -  initilized succefully */
#define VPS_AD9889_NOT_INITED         (0)

/** \brief AD9889 device state -  initilized succefully */
#define VPS_AD9889_INITED             (1)

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
    Device_HdmiChipId  hdmiChipid;
    /* HDMI Chip id */
    UInt32  hpd;
    /* HDMI Hot plug detect */
    UInt32 syncCfgReg;
    /**< Sync Configuration Register */
    UInt32 syncPolarityReg;
    UInt32 isRgbOutput;
    UInt32 inBusCfg;
} Device_Ad9889Obj;

typedef Device_Ad9889Obj * Device_Ad9889Handle;

/*
  Overall HDMI driver object
*/
typedef struct
{
    OSA_I2cHndl i2cHandle;
    /** < i2c handle for i2c read/write*/
    Device_Ad9889Handle ad9889Handle[DEVICE_MAX_HANDLES];
    /** < HDMI handle objects */
    Device_Ad9889Prms prms;
    /**< AD9889 params */
} Device_Ad9889CommonObj;


Device_Ad9889CommonObj gDevice_ad9889CommonObj;

#endif

