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

#ifndef SOFA_TESTS_ONLY
#include <platsupport/chardev.h>
#endif
#include "PCIDriver.h"
#include "../DriverKit/IODevice.h"
#include "../Bootstrap.h"
#include "../DriverKit/DriverKit.h"

static const char pciName[] = "PCIDriver";


static OSError PCIInit(IODriverBase *driver  ) NO_NULL_POINTERS;
static OSError PCIRelease(IODriverBase *driver  ) NO_NULL_POINTERS;
static OSError PCIProbeDevice(IODriverBase* driver , IONode* node,KernelTaskContext* ctx) NO_NULL_POINTERS;

static IODriverCallbacks PCIMethods =
{
    PCIInit,
    PCIRelease,
    PCIProbeDevice
    
    //OSError (*start)(IODriverBase *driver  );
    //OSError (*stop)(IODriverBase *driver  );
    
};

OSError PCIDriverInit( PCIDriver* driver)
{
    OSError ret = IODriverBaseInit(&driver->base, pciName);
    
    if(ret == OSError_None)
    {
        driver->base.driverMethods = &PCIMethods;
        driver->isaNode = NULL;
    }
    
    return ret;
}


static OSError PCIInit(IODriverBase *driver  )
{
    return OSError_None;
}

static OSError PCIRelease(IODriverBase *driver  )
{
    return OSError_Some;
}

static  ssize_t _ComRead(IODevice* dev, uint8_t* toBuf,  size_t maxBufSize  );
static ssize_t _ComWrite(IODevice* dev, const uint8_t* buf , size_t bufSize  );

static IODeviceCallbacks _ComMethods =
{
    _ComRead,
    _ComWrite,
};

#ifndef SOFA_TESTS_ONLY
static ps_chardevice_t comDev = {0};
#endif
static OSError PCIProbeDevice(IODriverBase* driver , IONode* node,KernelTaskContext* ctx)
{
    PCIDriver* self = (PCIDriver*) driver;
    if( node->hid == 0x30ad041) // PNP0A03
    {
        IONode* isaNode = IONodeGetChildName(node, "ISA");
        if( isaNode)
        {
            self->isaNode = isaNode;
            isaNode->_attachedDriver = driver;
            
            struct kobject*o = NULL;
            IONodeForEach(isaNode, o)
            {
                IONode* c = (IONode*) o;
                
                if( c->hid == 0x105D041 || c->hid == 0x303d041 || c->hid == 0x130FD041) // PNP0501 COM port  PNP0303   keyboard PNP0F13  mouse
                {
                    IODevice* dev = kmalloc( sizeof(IODevice));
                    ALWAYS_ASSERT(dev);
                    
                    ALWAYS_ASSERT_NO_ERR( IODeviceInit(dev, c, c->base.obj.k_name) );
                    
                    if( c->hid == 0x105D041 )//com
                    {
#ifndef SOFA_TESTS_ONLY
                        ALWAYS_ASSERT(ps_cdev_init(PC99_SERIAL_COM1 , &ctx->opsIO ,&comDev) );
#endif
                        dev->methods = &_ComMethods;
                    }
                    ALWAYS_ASSERT_NO_ERR(DriverKitRegisterDevice(dev) );
                }
                
            }
        }

        return OSError_None;
    }
    return OSError_Some;
}


static  ssize_t _ComRead(IODevice* dev, uint8_t* toBuf,  size_t maxBufSize  )
{
#ifndef SOFA_TESTS_ONLY
    int c = ps_cdev_getchar(&comDev);
    
    while (c<=0)
    {
        c = ps_cdev_getchar(&comDev);
    }
    
    toBuf[0] = c;
    //kprintf("_ComRead on device '%s'\n" , dev->base.k_name);
    return 1;
#else
    return OSError_Some;
#endif
}

static ssize_t _ComWrite(IODevice* dev, const uint8_t* buf , size_t bufSize  )
{
    kprintf("_ComWrite on device '%s'\n" , dev->base.k_name);
    return OSError_Some;
}
