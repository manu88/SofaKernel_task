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

#include <EISAID.h>
#include "DriverKit.h"
#include "deviceTree.h"
#include "../KObject/KObject.h"
#include "IODriverBase.h"

static struct _DKContext
{
    struct kset driverKitNode;
    IONode deviceTree;
    struct kset driversNode;
    
    struct kset exportedDevicesTree;
    
} _dkContext = {0};

static const char driverKitNodeName[] = "DriverKit";
static const char deviceTreeName[]    = "DeviceTree";
static const char driversNodeName[]   = "Drivers";

static const char exportedDevicesTreeName[]   = "Devices";



OSError DriverKitInit(struct kset* root, const uint8_t* fromDatas, size_t bufferSize)
{
    kset_initWithName(&_dkContext.exportedDevicesTree , exportedDevicesTreeName);
    ALWAYS_ASSERT(kset_append(root, (struct kobject*) &_dkContext.exportedDevicesTree) == OSError_None);
    
    kset_initWithName(&_dkContext.driverKitNode , driverKitNodeName);
    
    ALWAYS_ASSERT_NO_ERR( IONodeInit(&_dkContext.deviceTree, deviceTreeName));
    
    kset_initWithName(&_dkContext.driversNode, driversNodeName);
    
    ALWAYS_ASSERT(kset_append(root, (struct kobject*) &_dkContext.driverKitNode) == OSError_None);
    kobject_put((struct kobject *)&_dkContext.driverKitNode);
    
    ALWAYS_ASSERT_NO_ERR(kset_append(&_dkContext.driverKitNode, (struct kobject *)&_dkContext.deviceTree) );
    kobject_put((struct kobject *)&_dkContext.deviceTree);
    
    ALWAYS_ASSERT_NO_ERR(kset_append(&_dkContext.driverKitNode, (struct kobject *)&_dkContext.driversNode) );
    kobject_put((struct kobject *)&_dkContext.driversNode);
    
    OSError ret = DeviceTreeContructDeviceTree(&_dkContext.deviceTree, fromDatas, bufferSize);
   
    return ret;
}




static void _printObject(const IONode* object , int indent)
{
    
    for(int i =0;i<indent;i++)
        printf("|\t");
    
    printf("|'%s'  \n" , object->base.obj.k_name   );//, child->type == INodeType_Folder? "Folder":"File");
    
    
    
    
    for(int i =0;i<indent+1;i++)
        printf("|\t");
    
    if (isEisaId(object->hid))
    {
        char eisaID[8] = "";
        if(getEisaidString( object->hid , eisaID))
        {
            printf("- HID '%s' \n" ,  eisaID);
        }
    }
    else
    {
        printf("- HID 0x%llx \n" ,  object->hid);
    }

    
    struct kobject *child = NULL;
    kset_foreach( ((struct kset*)object), child)
    {
        _printObject((IONode*)child, indent +1);
        /*
         if (child->type == INodeType_Folder)
         {
         _printNode(child , indent + 1);
         }
         */
    }
}

void DriverKitDump()
{
    printf("--- DriverKit Tree ---\n");
    
    _printObject((const IONode*) &_dkContext.driverKitNode , 0);
    
    printf("--- DriverKit Tree ---\n");
}


static OSError _CallDriverInit(IODriverBase* driver)
{
    if( driver->driverMethods->init)
    {
        return  driver->driverMethods->init(driver);
    }
    
    return OSError_None;
}

static OSError _CallDriverRelease(IODriverBase* driver)
{
    if( driver->driverMethods->release)
    {
        return  driver->driverMethods->release(driver);
    }
    
    return OSError_None;
}

OSError DriverKitRegisterDriver( IODriverBase* driver)
{
    OSError ret = kset_append(&_dkContext.driversNode, (struct kobject *)driver);
    //OSError ret = IONodeAddChild(&_dkContext.driversNode, (IONode*) driver);
    
    if (ret == OSError_None)
    {
        kprintf("DriverKit : register Driver '%s'\n" , driver->base.k_name);
        if(driver->isInit == 0)
        {
            ret = _CallDriverInit(driver);
        }
    }
    return ret;
    
}

OSError DriverKitRemoveDriver( IODriverBase* driver)
{
    OSError ret = kset_remove(&_dkContext.driversNode, (struct kobject *)driver);
    //OSError ret = IONodeRemoveChild(&_dkContext.driversNode,(IONode*) driver);
    
    if (ret == OSError_None)
    {
        kprintf("DriverKit : remove Driver '%s'\n" , driver->base.k_name);
        ret = _CallDriverRelease(driver);
        driver->isInit = 0;
    }
    return ret;
}



static NO_NULL_POINTERS  OSError _DriverKitTryProbeNode( IONode* node , KernelTaskContext* context )
{
    
    struct kobject* drv = NULL;
    kset_foreach((&_dkContext.driversNode), drv)
    {
        IODriverBase* driver = (IODriverBase*) drv;
        
        OSError retProbe =  driver->driverMethods->probeDevice(driver , node , context);
        
        if( retProbe == OSError_None)
        {
            kprintf("Attaching driver '%s' to device '%s'\n" , driver->base.k_name , node->base.obj.k_name);
            
            node->_attachedDriver = driver;
        }
    }
    
    return OSError_Some;
}

static NO_NULL_POINTERS  OSError _DriverKitTryProbeNodeAndChildren( IONode* node, KernelTaskContext* context )
{
    
    
    OSError ret = _DriverKitTryProbeNode(node , context);
    if (ret == OSError_None)
    {
        return ret;
    }
        
    struct kobject* obj = NULL;
    kset_foreach((&node->base), obj)
    {
        IONode* child = (IONode* ) obj;
        
        _DriverKitTryProbeNodeAndChildren(child , context);
    }
    
    return OSError_None;
}


OSError DriverKitDoMatching(KernelTaskContext* context)
{
    kprintf("DriverKit : Start matching process \n" );
    OSError ret = OSError_None;
    
    
    ret = _DriverKitTryProbeNodeAndChildren(&_dkContext.deviceTree , context);
    
    kprintf("DriverKit : End matching process \n" );
    return ret;
}

OSError DriverKitRegisterInterupt(IODriverBase* base, uint32_t intNum)
{
    
    return OSError_None;
}


OSError DriverKitRegisterDevice(IODevice* device )
{
    return kset_append(&_dkContext.exportedDevicesTree, (struct kobject *)device);
}

