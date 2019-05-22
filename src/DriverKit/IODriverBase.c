//
//  IODriverBase.c
//  kernel_taskSofaV2
//
//  Created by Manuel Deneu on 22/05/2019.
//  Copyright © 2019 Manuel Deneu. All rights reserved.
//

#include "IODriverBase.h"


OSError IODriverBaseInit(IODriverBase* base, const char* name)
{
    kobject_init(&base->base);
    base->base.k_name = name;
    
    return OSError_None;
}
