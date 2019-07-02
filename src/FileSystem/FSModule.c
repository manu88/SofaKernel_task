//
//  FSModule.c
//  kernel_taskSofaV2
//
//  Created by Manuel Deneu on 02/07/2019.
//  Copyright © 2019 Manuel Deneu. All rights reserved.
//

#include <string.h>
#include "FSModule.h"


OSError FSModuleInit(FSModule* module, const char* name)
{
    kobject_init(&module->obj);
    module->obj.k_name = name;
    return OSError_None;
}
