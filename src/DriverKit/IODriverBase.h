//
//  IODriverBase.h
//  kernel_taskSofaV2
//
//  Created by Manuel Deneu on 22/05/2019.
//  Copyright © 2019 Manuel Deneu. All rights reserved.
//

#pragma once

#include "../Sofa.h"
#include "../KObject/KObject.h"
#include "IODevice.h"

typedef struct _IODriverBase IODriverBase;

typedef struct
{
    OSError (*init)(IODriverBase *driver  ) NO_NULL_POINTERS;
    OSError (*release)(IODriverBase *driver  ) NO_NULL_POINTERS;
    
    OSError (*probeDevice)(IODriverBase* driver , IONode* node) NO_NULL_POINTERS;
    
    OSError (*onInterupt)(IODriverBase* driver , uint32_t intNum) NO_NULL_POINTERS;
    
} IODriverCallbacks;


typedef struct _IODriverBase
{
    struct kobject base;
    IODriverCallbacks *driverMethods; // default methods will do nothing and return OSError_None
    
    uint8_t isInit:1;
    
} IODriverBase;

OSError IODriverBaseInit(IODriverBase* base, const char* name) NO_NULL_POINTERS;


