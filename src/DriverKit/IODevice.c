//
//  IODevice.c
//  kernel_taskSofaV2
//
//  Created by Manuel Deneu on 08/05/2019.
//  Copyright © 2019 Manuel Deneu. All rights reserved.
//

#include <string.h> // memset
#include "IODevice.h"



OSError IODeviceInit(IODevice* device,IONodeType type, const char* name)
{
    memset(device, 0, sizeof(IODevice));
    kset_init(&device->base);
    
    device->base.obj.k_name =  strdup(name);
    //strncpy(device->name, name, 32);
    device->type = type;
    return OSError_None;
}

OSError IODeviceAddChild( IODevice* baseDev, IODevice* child)
{
    if (baseDev == child)
    {
        return OSError_ArgError;
    }
    
    return kset_append(&baseDev->base, &child->base.obj);
}
