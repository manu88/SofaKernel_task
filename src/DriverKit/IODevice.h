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

#include <stdint.h>
#include "../Sofa.h"
#include "../KObject/KObject.h"

typedef enum
{
    IONodeType_Device,
    IONodeType_Node,
    
    
} IONodeType;


typedef enum
{
  IOCategory_PCIBus,
    
} IOCategory;

typedef struct _IONode
{
    
}IONode;

typedef struct
{
    // needs to remain first!
    struct kset base;
    
    IONodeType type;
    uint64_t hid;
     
}IODevice;

OSError IODeviceInit(IODevice* device,IONodeType type, const char* name) NO_NULL_POINTERS;

OSError IODeviceAddChild( IODevice* baseDev, IODevice* child) NO_NULL_POINTERS;
