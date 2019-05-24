//
//  PCIDriver.c
//  kernel_taskSofaV2
//
//  Created by Manuel Deneu on 22/05/2019.
//  Copyright © 2019 Manuel Deneu. All rights reserved.
//

#include "PCIDriver.h"
#include "../DriverKit/IODevice.h"

static const char pciName[] = "PCIDriver";


static OSError PCIInit(IODriverBase *driver  ) NO_NULL_POINTERS;
static OSError PCIRelease(IODriverBase *driver  ) NO_NULL_POINTERS;
static OSError PCIProbeDevice(IODriverBase* driver , IONode* node) NO_NULL_POINTERS;

static IODriverCallbacks PCIMethods =
{
    PCIInit,
    PCIRelease,
    PCIProbeDevice
    
    //OSError (*start)(IODriverBase *driver  );
    //OSError (*stop)(IODriverBase *driver  );
    
};

OSError PCIDriverInit( PCIDriver* driver)
{
    OSError ret = IODriverBaseInit(&driver->base, pciName);
    
    if(ret == OSError_None)
    {
        driver->base.driverMethods = &PCIMethods;
        driver->isaNode = NULL;
    }
    
    return ret;
}


static OSError PCIInit(IODriverBase *driver  )
{
    return OSError_None;
}

static OSError PCIRelease(IODriverBase *driver  )
{
    return OSError_Some;
}

static OSError PCIProbeDevice(IODriverBase* driver , IONode* node)
{
    PCIDriver* self = (PCIDriver*) driver;
    if( node->hid == 0x30ad041) // PNP0A03
    {
        IONode* isaNode = IONodeGetChildName(node, "ISA");
        if( isaNode)
        {
            self->isaNode = isaNode;
            isaNode->_attachedDriver = driver;
        }

        return OSError_None;
    }
    return OSError_Some;
}
