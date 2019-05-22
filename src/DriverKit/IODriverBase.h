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
typedef struct _IODriverBase
{
    struct kobject base;
    const char* name; // this is just a ref
    
} IODriverBase;

OSError IODriverBaseInit(IODriverBase* base, const char* name) NO_NULL_POINTERS;
