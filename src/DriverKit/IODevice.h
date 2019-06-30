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
#include "IONode.h"


extern const KClass *IODeviceClass;
typedef struct _IODevice IODevice;

typedef struct
{
    ssize_t (*read)(IODevice* dev, uint8_t* toBuf,  size_t maxBufSize  ) NO_NULL_POINTERS;
    ssize_t (*write)(IODevice* dev, const uint8_t* buf , size_t bufSize  ) NO_NULL_POINTERS;
    
} IODeviceCallbacks;

typedef struct _IODevice
{
    struct kobject base;
    IONode* nodeRef;
    
    IODeviceCallbacks* methods;
    
    
    
} IODevice;


OSError IODeviceInit(IODevice* dev, IONode* fromNode, const char* name) NO_NULL_POINTERS;


static inline NO_NULL_POINTERS ssize_t IODeviceRead( IODevice* dev, uint8_t* toBuf , size_t maxBufSize)
{
    return dev->methods->read(dev, toBuf , maxBufSize);
}

static inline NO_NULL_POINTERS ssize_t IODeviceWrite( IODevice* dev, const uint8_t* buf , size_t bufSize)
{
    return dev->methods->write(dev, buf , bufSize);
}
