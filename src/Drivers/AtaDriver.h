//
//  AtaDriver.h
//  kernel_taskSofaV2
//
//  Created by Manuel Deneu on 30/06/2019.
//  Copyright © 2019 Manuel Deneu. All rights reserved.
//

#pragma once

#include "../DriverKit/IODriverBase.h"



typedef struct
{
    IODriverBase base;

    
} ATADriver;

OSError ATADriverInit( ATADriver* driver) NO_NULL_POINTERS;
