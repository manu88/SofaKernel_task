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
#include <pci/pci.h>
#endif

#include "PCIDriver.h"
#include "../DriverKit/IODevice.h"
#include "../Bootstrap.h"
#include "../DriverKit/DriverKit.h"
#include <ACPIDesc.h>
#include "EGADriver.h"
#include "ATADriver.h"

typedef struct
{
    IODevice base;
#ifndef SOFA_TESTS_ONLY
    ps_chardevice_t _dev;
#endif
    
} IOComDevice;



typedef struct
{
    IONode base;
    
} IOPCINode;


static OSError IOPCINodeInit(IOPCINode* node , const char* name)
{
    memset(node, 0, sizeof(IOPCINode));
    OSError ret = IONodeInit(&node->base, name);
    
    if( ret == OSError_None)
    {
        
    }
    return ret;
}

static const char vgaName[] = "VGA";
static IOPCINode vgaNode = {0};
static IODevice vgaDev = {0};

static const char pciName[] = "PCIDriver";
static const char ATAControllerName[] = "ATAController";
static const char NetControllerName[] = "NetController";

static ATADriver _ataDriver = {0};

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
#ifndef SOFA_TESTS_ONLY
        ALWAYS_ASSERT_NO_ERR(ATADriverInit(&_ataDriver));

        ALWAYS_ASSERT_NO_ERR(DriverKitRegisterDriver((IODriverBase *)&_ataDriver));
        kobject_put((struct kobject *) &_ataDriver);
#endif
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

static ssize_t _ComRead(IODevice* dev,uint64_t offset, uint8_t* toBuf,  size_t maxBufSize  );
static ssize_t _ComWrite(IODevice* dev,uint64_t offset, const uint8_t* buf , size_t bufSize  );


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

OSError IONodeGetAttrPCI(const IONode* node, const char*attrName , IOData *data)
{
#ifndef SOFA_TESTS_ONLY
    const libpci_device_t* implDev = (const libpci_device_t*) node->impl;
    ALWAYS_ASSERT(implDev);
    
    if( strcmp(attrName, IONodeAttributePCIClass) == 0)
    {
        data->type = IODataType_Numeric;
        data->data.val = implDev->dev;
        return OSError_None;
    }
    else if( strcmp(attrName, IONodeAttributePCISubClass) == 0)
    {
        data->type = IODataType_Numeric;
        data->data.val = implDev->fun;
        return OSError_None;
    }
    
    else if( strcmp(attrName, IONodeAttributeVendorID) == 0)
    {
        data->type = IODataType_Numeric;
        data->data.val = implDev->vendor_id;
        return OSError_None;
    }
    else if( strcmp(attrName, IONodeAttributeDeviceID) == 0)
    {
        data->type = IODataType_Numeric;
        data->data.val = implDev->device_id;
        return OSError_None;
    }
    else if( strcmp(attrName, IONodeAttributeDeviceName) == 0)
    {
        data->type = IODataType_String;
        data->data.str = implDev->device_name;
        return OSError_None;
    }
    else if( strcmp(attrName, IONodeAttributeVendorName) == 0)
    {
        data->type = IODataType_String;
        data->data.str = implDev->vendor_name;
        return OSError_None;
    }
#endif
    return OSError_ArgError;
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
                    
                    IONodeACPI* acpiNode = (IONodeACPI*) c;
                    
                    ACPINamedResource outRes = {0};
                    ACPIScopeGetNamedResource(&acpiNode->dev, "_CRS", &outRes);
                    
                    if( outRes.type == ACPIObject_Type_SmallItem && outRes.size == sizeof(IOPortDescriptor))
                    {
                        const IOPortDescriptor* ioDesc = (const IOPortDescriptor*) outRes.data;
                        //printf(" rangeMinBaseAddr %x\n" , ioDesc->rangeMinBaseAddr);
                        //printf(" rangeMaxBaseAddr %x\n" , ioDesc->rangeMaxBaseAddr);
                        //printf(" baseAlign %x\n" , ioDesc->baseAlign);
                        //printf(" rangeLen %x\n" , ioDesc->rangeLen);
                        //printf(" isDecoder %x\n" , ioDesc->isDecoder);
                    }
                    /*
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
                     */
                    
                    
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

#ifndef SOFA_TESTS_ONLY
        int numPciDevices = _ScanPCIDevices(ctx);
        libpci_device_t* ideDev = libpci_find_device_bdf(0 , 0x01,0x01);
        
        if( ideDev)
        {
            printf("Got an IDE device vendor id %x device id %x  '%s' '%s'\n" ,
                   ideDev->vendor_id ,
                   ideDev->device_id ,
                   ideDev->vendor_name ,
                   ideDev->device_name);
            
            IOPCINode* ataNode = kmalloc(sizeof(IONode));
            
            ALWAYS_ASSERT_NO_ERR(IOPCINodeInit(ataNode, ATAControllerName));
            ataNode->base.GetAttr = IONodeGetAttrPCI;
            ataNode->base.impl = ideDev;
            ALWAYS_ASSERT_NO_ERR(IONodeAddChild(node, ataNode));

            //ALWAYS_ASSERT_NO_ERR(IONodeAddAttr(ataNode, IONodeAttributePCI, /*itemType*/ 0, ideDev));
        }
        
        // vga
        libpci_device_t* vgaDevice = libpci_find_device_bdf(0 , 0x03,0x00);
        
        if (vgaDevice)
        {
            ALWAYS_ASSERT_NO_ERR(InitEGADriver(ctx));
            
            ALWAYS_ASSERT_NO_ERR(IOPCINodeInit(&vgaNode, vgaName));
            
            ALWAYS_ASSERT_NO_ERR( IODeviceInit(&vgaDev, &vgaNode, vgaName));
            
            vgaNode.base.impl = vgaDevice;
            
            ALWAYS_ASSERT_NO_ERR(IONodeAddChild(node, &vgaNode));
            kobject_put((struct kobject *)&vgaNode);
            
            ALWAYS_ASSERT_NO_ERR(DriverKitRegisterDevice(&vgaDev));
            kobject_put((struct kobject *)&vgaDev);
        }
        
        // net
        libpci_device_t* netDev = libpci_find_device_bdf(0 , 0x02,0x00);
        if( netDev)
        {
            IOPCINode* netNode = kmalloc(sizeof(IONode));
            
            ALWAYS_ASSERT_NO_ERR(IOPCINodeInit(netNode, NetControllerName));
            netNode->base.GetAttr = IONodeGetAttrPCI;
            netNode->base.impl = netDev;
            ALWAYS_ASSERT_NO_ERR(IONodeAddChild(node, netNode));
        }
        
#endif
        // do we have an IDE storage device?
        /*
        
        
        for(int i=0;i<numPciDevices;i++)
        {
            const libpci_device_t* device = &libpci_device_list[i];
            printf("Got a PCI Device vid %x did %x bus %x dev %x fun %x \n" , device->vendor_id , device->device_id , device->bus , device->dev , device->fun);
            
        }
         */

        return OSError_None;
    }
    return OSError_Some;
}







static  ssize_t _ComRead(IODevice* dev,uint64_t offset, uint8_t* toBuf,  size_t maxBufSize  )
{
    UNUSED_PARAMETER(offset);
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

static ssize_t _ComWrite(IODevice* dev,uint64_t offset, const uint8_t* buf , size_t bufSize  )
{
    UNUSED_PARAMETER(offset);
    IOComDevice* self = (IOComDevice*) dev;
#ifndef SOFA_TESTS_ONLY
    for(size_t i=0;i<bufSize;++i)
    {
        ps_cdev_putchar(&self->_dev , (int) buf[i]);
    }
    return bufSize;
#else
    return OSError_Some;
#endif
}
