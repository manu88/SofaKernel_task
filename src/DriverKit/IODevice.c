//
//  IODevice.c
//  kernel_taskSofaV2
//
//  Created by Manuel Deneu on 08/05/2019.
//  Copyright © 2019 Manuel Deneu. All rights reserved.
//

#include <string.h> // memset
#include "IODevice.h"


static void IONodegetInfos( const struct kobject *obj , char outDesc[MAX_DESC_SIZE] );


static const KClass ioNodeClass = KClassMake("IONode", IONodegetInfos);


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
    
    snprintf(outDesc, MAX_DESC_SIZE, "INODE Attached Driver %p" , (const void*) self->_attachedDriver );
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
