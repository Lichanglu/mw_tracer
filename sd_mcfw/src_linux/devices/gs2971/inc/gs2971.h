/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _DEVICEDRV_GS2971_H_
#define _DEVICEDRV_GS2971_H_

Int32 Device_gs2971Init (  );
Int32 Device_gs2971DeInit (  );

Device_Gs2971Handle Device_gs2971Create ( UInt32 drvId,
                                UInt32 instanceId,
                                Ptr createArgs,
                                Ptr createStatusArgs);

Int32 Device_gs2971Delete ( Device_Gs2971Handle handle, Ptr deleteArgs );

Int32 Device_gs2971Control ( Device_Gs2971Handle handle,
                           UInt32 cmd, Ptr cmdArgs, Ptr cmdStatusArgs );

#endif

