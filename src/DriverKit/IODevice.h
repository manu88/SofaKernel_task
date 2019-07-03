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

SOFA_BEGIN_DCL


typedef enum
{
        IOCTL_Reset = 1,
} IOCTLRequests;
extern const KClass *IODeviceClass;
typedef struct _IODevice IODevice;
typedef struct _KernelTaskContext KernelTaskContext;

typedef struct
{
    ssize_t (*read)(IODevice* dev, uint64_t offset,uint8_t* toBuf,  size_t maxBufSize  ) NO_NULL_POINTERS;
    ssize_t (*write)(IODevice* dev,uint64_t offset, const uint8_t* buf , size_t bufSize  ) NO_NULL_POINTERS;
    OSError (*ioctl)(IODevice* dev, int request, void *argp);
    
} IODeviceCallbacks;

//!  IODevice
/*!
 Represents an exported device that may or may not be attached to a real system Node (nodeRef).
 */
typedef struct _IODevice
{
    struct kobject base;
    IONode* nodeRef; // attached system node. Can be NULL if the Device is 'virtual'
    
    IODeviceCallbacks* methods;
    
    KernelTaskContext * ctx; // will be set when attaching Device to DriverKit

} IODevice;


OSError IODeviceInit(IODevice* dev, IONode* fromNode, const char* name) NO_NULL_POINTERS;


static inline NO_NULL_POINTERS ssize_t IODeviceRead( IODevice* dev,uint64_t offset, void* toBuf , size_t maxBufSize)
{
    return dev->methods->read(dev,offset, toBuf , maxBufSize);
}

static inline NO_NULL_POINTERS ssize_t IODeviceWrite( IODevice* dev,uint64_t offset, const void* buf , size_t bufSize)
{
    return dev->methods->write(dev,offset, buf , bufSize);
}

static inline NO_NULL_ARGS(1, 1) OSError IODeviceCtl(IODevice* dev, int request, void *argp)
{
    return dev->methods->ioctl(dev , request , argp);
}

SOFA_END_DCL
