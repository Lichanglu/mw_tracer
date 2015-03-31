/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _DEVICEDRV_ADV7442_H_
#define _DEVICEDRV_ADV7442_H_
#include "../src/adv7442_priv.h"


#include "../src/adv7442_priv.h"


Int32 Device_adv7442Init (  );
Int32 Device_adv7442DeInit (  );

Device_Adv7442Handle Device_adv7442Create ( UInt32 drvId,
                                UInt32 instanceId,
                                Ptr createArgs,
                                Ptr createStatusArgs);

Int32 Device_adv7442Delete ( Device_Adv7442Handle handle, Ptr deleteArgs );

Int32 Device_adv7442Control ( Device_Adv7442Handle handle,
                           UInt32 cmd, Ptr cmdArgs, Ptr cmdStatusArgs );

#endif
