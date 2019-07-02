//
//  FSModule.h
//  kernel_taskSofaV2
//
//  Created by Manuel Deneu on 02/07/2019.
//  Copyright © 2019 Manuel Deneu. All rights reserved.
//

#pragma once


#include "../Sofa.h"
#include "../KObject/KObject.h"

typedef struct
{
    struct kobject obj;
} FSModule;


OSError FSModuleInit(FSModule* module , const char* name) NO_NULL_POINTERS;
