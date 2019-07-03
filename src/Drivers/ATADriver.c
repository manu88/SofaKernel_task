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

#include <pci/pci.h>
#include "../Bootstrap.h"
#include "ata.h"
#include "ATADriver.h"
#include "../DriverKit/IODevice.h"
#include "../DriverKit/DriverKit.h"
#include "../FileSystem/VFS.h"

#include "ext2.h"

static const char ataName[] = "ATADriver";

static ATADrive drives[4];

static OSError ATAProbeDevice(IODriverBase* driver , IONode* node,KernelTaskContext* ctx) NO_NULL_POINTERS;


static IODriverCallbacks ATAMethods =
{
    NULL, // init
    NULL, // release
    ATAProbeDevice,
    NULL, // interrupts
    
    
};


static ssize_t ATARead(IODevice* dev,uint64_t offset, uint8_t* toBuf,  size_t maxBufSize  ) NO_NULL_POINTERS;
static ssize_t ATAWrite(IODevice* dev,uint64_t offset, const uint8_t* buf , size_t bufSize  ) NO_NULL_POINTERS;
static OSError ATAioctl(IODevice* dev, int request, void *argp);
static IODeviceCallbacks ataMethods =
{
    ATARead,
    ATAWrite,
    ATAioctl,
};



OSError ATADriverInit( ATADriver* driver)
{
    memset(drives, 0, sizeof(ATADrive) * 4);
    
    OSError ret = IODriverBaseInit(&driver->base, ataName);
    
    if(ret == OSError_None)
    {
        driver->base.driverMethods = &ATAMethods;
        //driver->base.driverMethods = &PCIMethods;
        //driver->isaNode = NULL;
    }
    
    return ret;
}



static void DriveInit(KernelTaskContext* ctx, ATADrive* drive)
{
    printf("DriveInit\n");
    ata_soft_reset(ctx, drive);
    //testRead(ctx, drive);
}

static OSError ATAProbeDevice(IODriverBase* driver , IONode* node,KernelTaskContext* ctx)
{

    
    IOData class;
    IOData subclass;
    OSError ret = IONodeGetAttribute(node, IONodeAttributePCIClass , &class);
    
    if (ret != OSError_None)
        return ret;
    ret = IONodeGetAttribute(node,  IONodeAttributePCISubClass  ,&subclass);
    
    if (ret != OSError_None)
    {
        return ret;
    }

    if( class.data.val == 0x01 && subclass.data.val == 0x01)
    {
        kprintf("Ata : try to probe node '%s' class %x subclass %x\n" , IONodeGetName(node) , class.data.val , subclass.data.val);
        
        
        const libpci_device_t* dev = (const libpci_device_t*) node->impl;
        uint32_t base, control;
        uint8_t id;
/* Primary Channel */
        id = 0;
        base = libpci_read_reg32(dev->bus, dev->dev, dev->fun, 0x10 + 4 * id);
        
        id = 1;
        control = libpci_read_reg32(dev->bus, dev->dev, dev->fun, 0x10 + 4 * id);

        if (base == 0 || base == 1)
            base = ATA_PIO_PRI_PORT_BASE;
        
        if (control == 0 || control == 1)
            control = ATA_PIO_PRI_PORT_CONTROL;
        
        printf("ata: IDE controller primary channel: (Base: 0x%x, Control: 0x%x)\n", base, control);
        
        
        drives[0].base = base;
        drives[0].control = control;
        drives[0].id = 0;
        drives[0].isSlave = 0;
        
        drives[1].base = base;
        drives[1].control = control;
        drives[1].id = 1;
        drives[1].isSlave = 1;
        
/* Secondary Channel */
        id = 2;
        
        base    = libpci_read_reg32(dev->bus, dev->dev, dev->fun, 0x10 + 4 * id);
        id = 3;
        control = libpci_read_reg32(dev->bus, dev->dev, dev->fun, 0x10 + 4 * id);
        

        if (base == 0 || base == 1)
            base = ATA_PIO_SEC_PORT_BASE;
        
        if (control == 0 || control == 1)
            control = ATA_PIO_SEC_PORT_CONTROL;
        
        printf("ata: IDE controller secondary channel: (Base: 0x%x, Control: 0x%x)\n", base, control);

        drives[2].base = base;
        drives[2].control = control;
        drives[2].id = 2;
        drives[2].isSlave = 0;

        
        drives[3].base = base;
        drives[3].control = control;
        drives[3].id = 3;
        drives[3].isSlave = 1;

        /*
        ata_soft_reset(ctx, &drives[0]);
        ata_soft_reset(ctx, &drives[1]);
        ata_soft_reset(ctx, &drives[2]);
        ata_soft_reset(ctx, &drives[3]);
        
         */
        for (int i = 0; i < 4 ; ++i)
        {
            /* disable interrupts */
            ata_disable_IRQ(ctx, &drives[i]);
            
            
            uint8_t type = ata_detect_drive(ctx , &drives[i]);
            switch (type)
            {
                case ATADEV_PATA:
                    printf("ata%d: Initializing ATA device (PIO)\n", i);
                    
                    if( drives[i].capabilities && strlen(drives[i].model))
                    {
                        IONode* hdNode = (IONode*) kmalloc(sizeof(IONode));
                        ALWAYS_ASSERT(hdNode);
                        
                        const char* hdName = (i == 0? "hda" : (i == 1? "hdb" :(i == 2? "hdc" :  "hdd" )));
                        IONodeInit(hdNode, hdName);
                        hdNode->impl = &drives[i];
                        hdNode->_attachedDriver = driver;
                        IONodeAddChild(node, hdNode);
                        kobject_put((struct kobject *)hdNode);
                        
                        IODevice* hdDev = (IODevice*) kmalloc(sizeof(IODevice));
                        if( hdDev)
                        {
                            if(IODeviceInit(hdDev, node, hdName) == OSError_None)
                            {
                                hdDev->methods = &ataMethods;
                                DriveInit(ctx, &drives[i]);
                                hdDev->nodeRef = hdNode;
                                
                                //hdNode->hid = &drives[i];
                                DriverKitRegisterDevice(hdDev);
                                kobject_put((struct kobject *)hdDev);
                            }
                            else
                            {
                                kfree(hdDev);
                            }
                        }
                    }
                    break;
                case ATADEV_SATA:
                    printf("ata%d: SATA is not supported\n", i);
                    break;
                case ATADEV_PATAPI:
                    printf("ata%d: PATAPI is not supported\n", i);
                    break;
                case ATADEV_SATAPI:
                    printf("ata%d: SATAPI is not supported\n", i);
                    break;
                case ATADEV_NOTFOUND:
                    printf("ata%d: No device found\n", i);
                    break;
                default:
                    printf("ata%d: Unkown ATA type %d\n", i, drives[i].type);
                    break;
            }
        }

        
        return OSError_None;
    }
    return OSError_NotSupportedDevice;
}


static ssize_t ATARead(IODevice* dev,uint64_t offset, uint8_t* toBuf,  size_t maxBufSize  )
{
    IONode* hdNode = dev->nodeRef;
    ALWAYS_ASSERT(hdNode);
    ATADrive* drive = (ATADrive*) hdNode->impl;
    
    ALWAYS_ASSERT(drive);
    return ata_read(dev->ctx, drive, offset, maxBufSize, toBuf);
}

static ssize_t ATAWrite(IODevice* dev,uint64_t offset, const uint8_t* buf , size_t bufSize  )
{
    IONode* hdNode = dev->nodeRef;
    ALWAYS_ASSERT(hdNode);
    ATADrive* drive = (ATADrive*) hdNode->impl;
    ALWAYS_ASSERT(drive);
    
    return ata_write(dev->ctx, drive, offset, bufSize, buf);
}

static OSError ATAioctl(IODevice* dev, int request, void *argp)
{
    printf("ATAioctl request %i\n" , request);
    IONode* hdNode = dev->nodeRef;
    ALWAYS_ASSERT(hdNode);
    ATADrive* drive = (ATADrive*) hdNode->impl;
    ALWAYS_ASSERT(drive);
    
    switch (request)
    {
        case IOCTL_Reset:
            ata_soft_reset(dev->ctx, drive);
            ata_disable_IRQ(dev->ctx, drive);
            return OSError_None;
        
        default:
            return OSError_ArgError;
    }
    
}
/*
static void testRead(KernelTaskContext* ctx, ATADrive* drive)
{
    printf("Test read \n");
    ata_soft_reset(ctx, drive);
    
    struct ext2_superblock sb = {0};
    
    printf("size of buf is %zi max lba %i sig is at offset %zi\n" ,sizeof(struct ext2_superblock) , drive->max_lba ,
           offsetof(struct ext2_superblock, ext2_signature) );
    
    // The first 1024 bytes of the disk, the "boot block", are reserved
    // for the partition boot sectors and are unused by the Ext2 filesystem
    
    ssize_t ret = ata_read(ctx, drive, 2 ,sizeof(struct ext2_superblock), &sb);
    
    printf("ata_read returns %i\n" , ret);
    
 
    if (sb.ext2_signature != EXT2_SIGNATURE)
    {
        printf("Invalid signature 0X%X!\n" , sb.ext2_signature);
        
        return ;
    }

    
    // register ext2 fs, if not already added
    OSError err =  VFSRegisterEXT2Module();
    
    ALWAYS_ASSERT( err == OSError_None || err == OSError_AlreadyInSet);
    
    
    
}
*/
