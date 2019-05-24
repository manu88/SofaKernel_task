//
//  PCIDriver.h
//  kernel_taskSofaV2
//
//  Created by Manuel Deneu on 22/05/2019.
//  Copyright © 2019 Manuel Deneu. All rights reserved.
//

#pragma once

#include "../DriverKit/IODriverBase.h"



typedef struct
{
    IODriverBase base;
    
    
} PCIDriver;

OSError PCIDriverInit( PCIDriver* driver) NO_NULL_POINTERS;
