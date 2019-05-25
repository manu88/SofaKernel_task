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




typedef struct _IODriverBase IODriverBase;

typedef struct
{
    struct kset base;
    
    uint64_t hid;
    IODriverBase* _attachedDriver;
     
}IONode;

OSError IONodeInit(IONode* node, const char* name) NO_NULL_POINTERS;

OSError IONodeAddChild( IONode* node, IONode* child) NO_NULL_POINTERS;
OSError IONodeRemoveChild( IONode* node, IONode* child) NO_NULL_POINTERS;

IONode* IONodeGetChildName( const IONode* node, const char* name) NO_NULL_POINTERS;

#define IONodeForEach( node, el) kset_foreach( (&node->base) , el)


typedef struct _IODevice
{
    struct kobject base;
    IONode* nodeRef;
    
} IODevice;


OSError IODeviceInit(IODevice* dev, IONode* fromNode, const char* name) NO_NULL_POINTERS;
