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

#include <pci/pci.h>
#include "PCIDriver.h"
#include "../DriverKit/IODevice.h"
#include "../Bootstrap.h"
#include "../DriverKit/DriverKit.h"
#include <ACPIDesc.h>
#include "EGADriver.h"
#include "AtaDriver.h"

typedef struct
{
    IODevice base;
#ifndef SOFA_TESTS_ONLY
    ps_chardevice_t _dev;
#endif
    
} IOComDevice;


static const char pciNodeAttr[] = "pci";
static const char vgaName[] = "VGA";
static IONode vgaNode = {0};
static IODevice vgaDev = {0};

static const char pciName[] = "PCIDriver";


static ATADriver _ataDriver = {0};

static OSError PCIInit(IODriverBase *driver  ) NO_NULL_POINTERS;
static OSError PCIRelease(IODriverBase *driver  ) NO_NULL_POINTERS;
static OSError PCIProbeDevice(IODriverBase* driver , IONode* node,KernelTaskContext* ctx) NO_NULL_POINTERS;


static OSError InitVGA(KernelTaskContext* ctx);

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
        
        ALWAYS_ASSERT_NO_ERR(ATADriverInit(&_ataDriver));
        ALWAYS_ASSERT_NO_ERR(DriverKitRegisterDriver((IODriverBase *)&_ataDriver));
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

static ssize_t _ComRead(IODevice* dev, uint8_t* toBuf,  size_t maxBufSize  );
static ssize_t _ComWrite(IODevice* dev, const uint8_t* buf , size_t bufSize  );


static IODeviceCallbacks _ComMethods =
{
    _ComRead,
    _ComWrite,
};

static int _ScanPCIDevices(KernelTaskContext* context)
{
#ifndef SOFA_TESTS_ONLY
    if(libpci_num_devices == 0)
    {
        libpci_scan( context->ops.io_port_ops);
    }
    
    return libpci_num_devices;
#else
    return 0;
#endif
}

static OSError PCIProbeDevice(IODriverBase* driver , IONode* node,KernelTaskContext* ctx)
{
    
    PCIDriver* self = (PCIDriver*) driver;
    
    
    
    
    if( node->hid == 0x30ad041) // PNP0A03
    {
        IONode* isaNode = IONodeGetChildByName(node, "ISA");
        if( isaNode)
        {
            self->isaNode = isaNode;
            isaNode->_attachedDriver = driver;
            
            struct kobject*o = NULL;
            IONodeForEach(isaNode, o)
            {
                
                IONode* c = (IONode*) o;
                
                if( c->hid == 0x105D041 )//com
                {
                    
                    uint16_t comID = 0;
                    
                    IOAttribute* a = NULL;
                    IOAttribute* tmp = NULL;
                    
                    IOAttributeForEach( c,tmp, a)
                    {
                        ALWAYS_ASSERT(a);
                        if( a->type == SmallResourceItemsType_IOPortDescriptor)
                        {
                            const IOPortDescriptor* add = (const IOPortDescriptor*) a->data.ptr;
                            printf("%s: Got '%s' %i %hx\n" ,c->base.obj.k_name, a->id , a->type , add->rangeMinBaseAddr);
                            comID = add->rangeMinBaseAddr;
                        }
                        
                    }
                    
                    IOAttribute* idAttr = IONodeGetAttr(c, "_UID");
                    uint64_t v = idAttr->data.v;
                    
                    IOComDevice* dev = kmalloc( sizeof(IOComDevice));
                    ALWAYS_ASSERT(dev);
                    
                    ALWAYS_ASSERT_NO_ERR( IODeviceInit(&dev->base, c, c->base.obj.k_name) );
                    
                    
                    
#ifndef SOFA_TESTS_ONLY
                    ALWAYS_ASSERT(ps_cdev_init(PC99_SERIAL_COM1 , &ctx->opsIO ,&dev->_dev) );
#endif
                    dev->base.methods = &_ComMethods;
                    
                    ALWAYS_ASSERT_NO_ERR(DriverKitRegisterDevice( (IODevice*) dev) );
                    kobject_put((struct kobject*) dev);
                }
                else if(  c->hid == 0x303d041 || c->hid == 0x130FD041) // PNP0303   keyboard PNP0F13  mouse
                {
                    IODevice* dev = kmalloc( sizeof(IODevice));
                    ALWAYS_ASSERT(dev);
                    
                    ALWAYS_ASSERT_NO_ERR( IODeviceInit(dev, c, c->base.obj.k_name) );
                    ALWAYS_ASSERT_NO_ERR(DriverKitRegisterDevice(dev) );
                    kobject_put((struct kobject*)dev);
                }
                
            }
        } // end isaNode
        
        int numPciDevices = _ScanPCIDevices(ctx);
        libpci_device_t* ideDev = libpci_find_device_bdf(0 , 0x01,0x01);
        
        if( ideDev)
        {
            printf("Got an IDE device vendor id %x device id %x  '%s' '%s'\n" ,
                   ideDev->vendor_id ,
                   ideDev->device_id ,
                   ideDev->vendor_name ,
                   ideDev->device_name);
            
            
            
            IONode* ataNode = kmalloc(sizeof(IONode));
            ALWAYS_ASSERT_NO_ERR(IONodeInit(ataNode, "ATAController"));
            
            ALWAYS_ASSERT_NO_ERR(IONodeAddChild(node, ataNode));

            ALWAYS_ASSERT_NO_ERR(IONodeAddAttr(ataNode, pciNodeAttr, /*itemType*/ 0, ideDev));
        }
        // do we have an IDE storage device?
        /*
        
        
        for(int i=0;i<numPciDevices;i++)
        {
            const libpci_device_t* device = &libpci_device_list[i];
            printf("Got a PCI Device vid %x did %x bus %x dev %x fun %x \n" , device->vendor_id , device->device_id , device->bus , device->dev , device->fun);
            
        }
         */
        
        
        ALWAYS_ASSERT_NO_ERR(InitVGA(ctx));
        
        ALWAYS_ASSERT_NO_ERR(IONodeAddChild(node, &vgaNode));
        kobject_put((struct kobject *)&vgaNode);
        
        ALWAYS_ASSERT_NO_ERR(DriverKitRegisterDevice(&vgaDev));
        kobject_put((struct kobject *)&vgaDev);
        return OSError_None;
    }
    return OSError_Some;
}



static OSError InitVGA(KernelTaskContext* ctx)
{
    ALWAYS_ASSERT_NO_ERR(InitEGADriver(ctx));
    
    ALWAYS_ASSERT_NO_ERR(IONodeInit(&vgaNode, vgaName));
    
    ALWAYS_ASSERT_NO_ERR( IODeviceInit(&vgaDev, &vgaNode, vgaName));
    
    
    return OSError_None;
}



static  ssize_t _ComRead(IODevice* dev, uint8_t* toBuf,  size_t maxBufSize  )
{
    IOComDevice* self = (IOComDevice*) dev;
#ifndef SOFA_TESTS_ONLY
    int c = ps_cdev_getchar(&self->_dev);
    
    while (c<=0)
    {
        c = ps_cdev_getchar(&self->_dev);
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
    IOComDevice* self = (IOComDevice*) dev;
    
    for(size_t i=0;i<bufSize;++i)
    {
        ps_cdev_putchar(&self->_dev , (int) buf[i]);
    }
    return bufSize;
}
