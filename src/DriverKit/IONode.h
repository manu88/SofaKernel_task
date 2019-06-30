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

#include "IONodeAttributes.h"
typedef struct _IODriverBase IODriverBase;

extern const KClass *IONodeClass;

struct _IONode;
typedef struct _IONode
{
    struct kset base;
    
    uint64_t hid;
    IODriverBase* _attachedDriver;
    
    IOAttribute* attributes;
    
    OSError (*GetAttr)(const struct _IONode* , const char*name , IOData *data) NO_NULL_POINTERS;
    
    void* impl;
}IONode;

OSError IONodeInit(IONode* node, const char* name) NO_NULL_POINTERS;

OSError IONodeAddChild( IONode* node, IONode* child) NO_NULL_POINTERS;
OSError IONodeRemoveChild( IONode* node, IONode* child) NO_NULL_POINTERS;

IONode* IONodeGetChildByName( const IONode* node, const char* name) NO_NULL_POINTERS;

#define IONodeForEach( node, el) kset_foreach( (&node->base) , el)

SOFA_DEPRECATED("") OSError IONodeAddAttr( IONode* node ,const char*name , int type , void*data );
//SOFA_DEPRECATED("") size_t  IONodeGetAttrCount( const IONode* node ) NO_NULL_POINTERS;
SOFA_DEPRECATED("") IOAttribute* IONodeGetAttr( const IONode* node, const char*name) NO_NULL_POINTERS;


OSError IONodeGetAttribute(const IONode* node, const char*name , IOData *data) NO_NULL_POINTERS;


#define IOAttributeForEach(node ,tmp ,el) HASH_ITER(hh ,node->attributes ,el ,tmp)


const char* IONodeGetName( const IONode*node) NO_NULL_POINTERS;
