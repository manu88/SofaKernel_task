//
//  AtaDriver.c
//  kernel_taskSofaV2
//
//  Created by Manuel Deneu on 30/06/2019.
//  Copyright © 2019 Manuel Deneu. All rights reserved.
//

#include "AtaDriver.h"

static const char ataName[] = "ATADriver";

static OSError ATAProbeDevice(IODriverBase* driver , IONode* node,KernelTaskContext* ctx) NO_NULL_POINTERS;


static IODriverCallbacks ATAMethods =
{
    NULL,
    NULL,
    ATAProbeDevice
    
    //OSError (*start)(IODriverBase *driver  );
    //OSError (*stop)(IODriverBase *driver  );
    
};

OSError ATADriverInit( ATADriver* driver)
{
    OSError ret = IODriverBaseInit(&driver->base, ataName);
    
    if(ret == OSError_None)
    {
        driver->base.driverMethods = &ATAMethods;
        //driver->base.driverMethods = &PCIMethods;
        //driver->isaNode = NULL;
    }
    
    return ret;
}


static OSError ATAProbeDevice(IODriverBase* driver , IONode* node,KernelTaskContext* ctx)
{
    kprintf("Ata : try to probe node '%s'\n" , IONodeGetName(node));
    return OSError_Some;
}
