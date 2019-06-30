/*
 * This file is part of the Sofa project
 * Copyright (c) 2018 Manuel Deneu.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "../Sofa.h"
#include "../KObject/KObject.h"
#include "IONode.h"

typedef struct _IODriverBase IODriverBase;
typedef struct _KernelTaskContext KernelTaskContext;

typedef struct
{
    OSError (*init)(IODriverBase *driver  ) NO_NULL_POINTERS;
    OSError (*release)(IODriverBase *driver  ) NO_NULL_POINTERS;
    
    OSError (*probeDevice)(IODriverBase* driver , IONode* node , KernelTaskContext* context) NO_NULL_POINTERS;
    
    OSError (*onInterupt)(IODriverBase* driver , uint32_t intNum) NO_NULL_POINTERS;
    
} IODriverCallbacks;

typedef struct _IODriverBase
{
    struct kobject base;
    IODriverCallbacks *driverMethods; // default methods will do nothing and return OSError_None
    
    uint8_t isInit:1;
    
} IODriverBase;

OSError IODriverBaseInit(IODriverBase* base, const char* name) NO_NULL_POINTERS;


