//
//  DriverKit.h
//  kernel_taskSofaV2
//
//  Created by Manuel Deneu on 22/05/2019.
//  Copyright © 2019 Manuel Deneu. All rights reserved.
//

#pragma once
#include "../Sofa.h"
#include "IODevice.h"


typedef struct _IODriverBase IODriverBase;

OSError DriverKitInit(struct kset* node, const uint8_t* fromDatas, size_t bufferSize) NO_NULL_ARGS(1, 1);

void DriverKitDump(void);

OSError DriverKitRegisterDriver( IODriverBase* driver) NO_NULL_POINTERS;
OSError DriverKitRemoveDriver( IODriverBase* driver) NO_NULL_POINTERS;


OSError DriverKitDoMatching(void);



// Per driver operations
OSError DriverKitRegisterInterupt(IODriverBase* base, uint32_t intNum) NO_NULL_ARGS(1, 1);
