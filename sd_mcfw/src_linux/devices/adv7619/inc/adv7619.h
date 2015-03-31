/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _DEVICEDRV_ADV7619_H_
#define _DEVICEDRV_ADV7619_H_

Int32 Device_adv7619Init (  );
Int32 Device_adv7619DeInit (  );

Device_Adv7619Handle Device_adv7619Create ( UInt8 drvId,
                                            UInt8 instId,
                                            Ptr createArgs,
                                            Ptr createStatusArgs);

Int32 Device_adv7619Control ( Device_Adv7619Handle handle,
                           UInt32 cmd, Ptr cmdArgs, Ptr cmdStatusArgs );



#endif

