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
#include "IONode.h"
#include "../Bootstrap.h"

static void IONodegetInfos( const struct kobject *obj , char outDesc[MAX_DESC_SIZE] );


static const KClass ioNodeClass = KClassMake("IONode", IONodegetInfos,NULL /*Release*/);
const KClass *IONodeClass = &ioNodeClass;

OSError IONodeInit(IONode* device, const char* name)
{
    memset(device, 0, sizeof(IONode));
    kset_init(&device->base);
    device->_attachedDriver = NULL;
    device->base.obj.k_name =  strdup(name);
    
    device->base.obj._class = &ioNodeClass;
    device->attributes = NULL;
    
    //strncpy(device->name, name, 32);
    
    return OSError_None;
}

static void IONodegetInfos( const struct kobject *obj , char outDesc[MAX_DESC_SIZE] )
{
    const IONode* self = (const IONode*) obj;
    
    snprintf(outDesc, MAX_DESC_SIZE, "HID 0X%llX Attached Driver %p" , self->hid, (const void*) self->_attachedDriver );
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


IONode* IONodeGetChildByName( const IONode* node, const char* name)
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

OSError IONodeAddAttr( IONode* node ,const char*name , int type , void*data )
{
    if (name == NULL || strlen(name) == 0)
    {
        return OSError_ArgError;
    }
    
    IOAttribute* attr = kmalloc(sizeof(IOAttribute));
    ALWAYS_ASSERT(attr);
    
    strcpy(attr->id, name);
    attr->type = type;
    attr->data.ptr = data;
    
    HASH_ADD_STR(node->attributes, id, attr);
    
    return OSError_None;
}

/*
size_t  IONodeGetAttrCount( const IONode* node )
{
    return HASH_COUNT(node->attributes);
}
*/
IOAttribute* IONodeGetAttr( const IONode* node, const char*name)
{
    
    IOAttribute* attr = NULL;
    HASH_FIND_STR(node->attributes, name, attr);
    return attr;
}

OSError IONodeGetAttribute(const IONode* node, const char*name , IOData *data)
{
    IOAttribute* attr = NULL;
    HASH_FIND_STR(node->attributes, name, attr);
    if (attr)
    {
        data->type = attr->type;
        
        switch (data->type)
        {
            case IODataType_Invalid:
                break;
                
            case IODataType_Numeric:
                data->data.val = attr->data.v;
                
            case IODataType_Pointer :
                data->data.ptr = attr->data.ptr;
                
            case IODataType_String :
                data->data.str = attr->data.ptr;
                
            return OSError_None;
        }
    }
    if( !node->GetAttr)
        return OSError_Unimplemented;
    
    return node->GetAttr(node , name , data);
}

const char* IONodeGetName( const IONode*node)
{
    return node->base.obj.k_name;
}
