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

#include "PCIDriver.h"
#include "../DriverKit/IODevice.h"
#include "../Bootstrap.h"
#include "../DriverKit/DriverKit.h"

static const char pciName[] = "PCIDriver";


static OSError PCIInit(IODriverBase *driver  ) NO_NULL_POINTERS;
static OSError PCIRelease(IODriverBase *driver  ) NO_NULL_POINTERS;
static OSError PCIProbeDevice(IODriverBase* driver , IONode* node) NO_NULL_POINTERS;

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

static OSError PCIProbeDevice(IODriverBase* driver , IONode* node)
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
                
                if( c->hid == 0x105D041) // PNP0501 COM port
                {
                    kprintf("Got a com port \n");
                    
                    IODevice* comDev = kmalloc( sizeof(IODevice));
                    ALWAYS_ASSERT(comDev);
                    
                    ALWAYS_ASSERT_NO_ERR( IODeviceInit(comDev, c, c->base.obj.k_name) );
                    ALWAYS_ASSERT_NO_ERR(DriverKitRegisterDevice(comDev) );
                }
            }
        }

        return OSError_None;
    }
    return OSError_Some;
}
