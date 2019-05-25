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

#include "../Sofa.h"
#include "../KObject/KObject.h"
#include "IODevice.h"

static  ssize_t _DefaultRead(IODevice* dev, uint8_t* toBuf,  size_t maxBufSize  )
{
    kprintf("_DefaultRead on device '%s'\n" , dev->base.k_name);
    return OSError_Some;
}

static ssize_t _DefaultWrite(IODevice* dev, const uint8_t* buf , size_t bufSize  )
{
    kprintf("_DefaultWrite on device '%s'\n" , dev->base.k_name);
    return OSError_Some;
}

static IODeviceCallbacks _defaultDevMethods =
{
    _DefaultRead,
    _DefaultWrite
};


static void IODevgetInfos( const struct kobject *obj , char outDesc[MAX_DESC_SIZE] );


static const KClass ioDeviceClass = KClassMake("IODevice", IODevgetInfos,NULL /*Release*/);


OSError IODeviceInit(IODevice* dev, IONode* fromNode, const char* name)
{
    kobject_initWithName(&dev->base, name);
    dev->base.class = &ioDeviceClass;
    dev->nodeRef = fromNode;
    dev->methods = &_defaultDevMethods;
    return OSError_None;
}

static void IODevgetInfos( const struct kobject *obj , char outDesc[MAX_DESC_SIZE] )
{
    const IODevice* self = (const IODevice*) obj;
    
    snprintf(outDesc, MAX_DESC_SIZE, "Attached Node %p" , (const void*) self->nodeRef );
}
