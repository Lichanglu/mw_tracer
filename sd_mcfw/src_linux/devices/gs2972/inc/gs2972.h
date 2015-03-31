/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _DEVICEDRV_GS2972_H_
#define _DEVICEDRV_GS2972_H_

Int32 Device_gs2972Init (  );
Int32 Device_gs2972DeInit (  );

Device_Gs2972Handle Device_gs2972Create ( UInt32 drvId,
                                UInt32 instanceId,
                                Ptr createArgs,
                                Ptr createStatusArgs);

Int32 Device_gs2972Delete ( Device_Gs2972Handle handle, Ptr deleteArgs );

Int32 Device_gs2972Control ( Device_Gs2972Handle handle,
                           UInt32 cmd, Ptr cmdArgs, Ptr cmdStatusArgs );

#endif

