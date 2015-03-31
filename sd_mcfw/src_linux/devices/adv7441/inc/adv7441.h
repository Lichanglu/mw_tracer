/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _DEVICEDRV_ADV7441_H_
#define _DEVICEDRV_ADV7441_H_

#include "../src/adv7441_priv.h"

Int32 Device_adv7441Init (  );
Int32 Device_adv7441DeInit (  );

Device_Adv7441Handle Device_adv7441Create ( UInt32 drvId,
                                UInt32 instanceId,
                                Ptr createArgs,
                                Ptr createStatusArgs);

Int32 Device_adv7441Delete ( Device_Adv7441Handle handle, Ptr deleteArgs );

Int32 Device_adv7441Control ( Device_Adv7441Handle handle,
                           UInt32 cmd, Ptr cmdArgs, Ptr cmdStatusArgs );

#endif
