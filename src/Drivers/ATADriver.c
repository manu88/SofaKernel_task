//
//  ATADriver.c
//  kernel_taskSofaV2
//
//  Created by Manuel Deneu on 30/06/2019.
//  Copyright © 2019 Manuel Deneu. All rights reserved.
//

#include "ATADriver.h"

static const char ataName[] = "ATADriver";

static OSError ATAProbeDevice(IODriverBase* driver , IONode* node,KernelTaskContext* ctx) NO_NULL_POINTERS;


static IODriverCallbacks ATAMethods =
{
    NULL, // init
    NULL, // release
    ATAProbeDevice
    
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
    IOData class;
    IOData subclass;
    OSError ret = IONodeGetAttribute(node, IONodeAttributePCIClass , &class);
    
    if (ret != OSError_None)
        return ret;
    ret = IONodeGetAttribute(node,  IONodeAttributePCISubClass  ,&subclass);
    
    if (ret != OSError_None)
    {
        return ret;
    }

    if( class.data.val == 0x01 && subclass.data.val == 0x01)
    {
        kprintf("Ata : try to probe node '%s' class %x subclass %x\n" , IONodeGetName(node) , class.data.val , subclass.data.val);
        return OSError_None;
    }
    return OSError_NotSupportedDevice;
}
