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

#include <string.h> // memset
#include "IODevice.h"


static void IONodegetInfos( const struct kobject *obj , char outDesc[MAX_DESC_SIZE] );


static const KClass ioNodeClass = KClassMake("IONode", IONodegetInfos,NULL /*Release*/);


OSError IONodeInit(IONode* device, const char* name)
{
    memset(device, 0, sizeof(IONode));
    kset_init(&device->base);
    device->_attachedDriver = NULL;
    device->base.obj.k_name =  strdup(name);
    
    device->base.obj.class = &ioNodeClass;
    //strncpy(device->name, name, 32);

    return OSError_None;
}

static void IONodegetInfos( const struct kobject *obj , char outDesc[MAX_DESC_SIZE] )
{
    const IONode* self = (const IONode*) obj;
    
    snprintf(outDesc, MAX_DESC_SIZE, "Attached Driver %p" , (const void*) self->_attachedDriver );
}

OSError IONodeAddChild( IONode* baseDev, IONode* child)
{
    if (baseDev == child)
    {
        return OSError_ArgError;
    }
    
    return kset_append(&baseDev->base, &child->base.obj);
}

OSError IONodeRemoveChild( IONode* node, IONode* child)
{
    return kset_remove(&node->base, &child->base.obj);
}


IONode* IONodeGetChildName( const IONode* node, const char* name)
{
    struct kobject* obj = NULL;
    
    IONodeForEach(node , obj)
    {
        if (strcmp(obj->k_name, name) == 0)
        {
            return (IONode*) obj;
        }
    }
    
    return NULL;
}


static void IODevgetInfos( const struct kobject *obj , char outDesc[MAX_DESC_SIZE] );


static const KClass ioDeviceClass = KClassMake("IODevice", IODevgetInfos,NULL /*Release*/);


OSError IODeviceInit(IODevice* dev, IONode* fromNode, const char* name)
{
    kobject_initWithName(&dev->base, name);
    dev->base.class = &ioDeviceClass;
    dev->nodeRef = fromNode;
    
    return OSError_None;
}

static void IODevgetInfos( const struct kobject *obj , char outDesc[MAX_DESC_SIZE] )
{
    const IODevice* self = (const IODevice*) obj;
    
    snprintf(outDesc, MAX_DESC_SIZE, "Attached Node %p" , (const void*) self->nodeRef );
}
