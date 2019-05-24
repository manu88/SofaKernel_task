//
//  DriverKit.c
//  kernel_taskSofaV2
//
//  Created by Manuel Deneu on 22/05/2019.
//  Copyright © 2019 Manuel Deneu. All rights reserved.
//

#include <EISAID.h>
#include "DriverKit.h"
#include "deviceTree.h"
#include "../KObject/KObject.h"
#include "IODriverBase.h"

static struct _DKContext
{
    IONode driverKitNode;
    IONode deviceTree;
    IONode driversNode;
    
} _dkContext = {0};

static const char driverKitNodeName[] = "DriverKit";
static const char deviceTreeName[]    = "DeviceTree";
static const char driversNodeName[]   = "Drivers";



OSError DriverKitInit(struct kset* root, const uint8_t* fromDatas, size_t bufferSize)
{

    ALWAYS_ASSERT(IONodeInit(&_dkContext.driverKitNode,  driverKitNodeName) == OSError_None);

    
    ALWAYS_ASSERT(IONodeInit(&_dkContext.deviceTree   ,  deviceTreeName)    == OSError_None);
    ALWAYS_ASSERT(IONodeInit(&_dkContext.driversNode  ,  driversNodeName)   == OSError_None);
    
    ALWAYS_ASSERT(kset_append(root, (struct kobject*) &_dkContext.driverKitNode) == OSError_None);
    kobject_put((struct kobject *)&_dkContext.driverKitNode);
    
    ALWAYS_ASSERT(IONodeAddChild(&_dkContext.driverKitNode, &_dkContext.deviceTree) == OSError_None);
    kobject_put((struct kobject *)&_dkContext.deviceTree);
    ALWAYS_ASSERT(IONodeAddChild(&_dkContext.driverKitNode, &_dkContext.driversNode) == OSError_None);
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
    
    _printObject( &_dkContext.driverKitNode , 0);
    
    printf("--- DriverKit Tree ---\n");
}


OSError DriverKitRegisterDriver( IODriverBase* driver)
{
    OSError ret = IONodeAddChild(&_dkContext.driversNode, (IONode*) driver);
    
    if (ret == OSError_None)
    {
        kprintf("DriverKit : register Driver '%s'\n" , driver->base.k_name);
        if(driver->isInit == 0)
        {
            ret = driver->driverMethods->init(driver);
        }
    }
    return ret;
    
}

OSError DriverKitRemoveDriver( IODriverBase* driver)
{
    OSError ret = IONodeRemoveChild(&_dkContext.driversNode,(IONode*) driver);
    if (ret == OSError_None)
    {
        kprintf("DriverKit : remove Driver '%s'\n" , driver->base.k_name);
        ret = driver->driverMethods->release(driver);
        driver->isInit = 0;
    }
    return ret;
}



static NO_NULL_POINTERS  OSError _DriverKitTryProbeNode( IONode* node )
{
    
    struct kobject* drv = NULL;
    kset_foreach((&_dkContext.driversNode.base), drv)
    {
        IODriverBase* driver = (IODriverBase*) drv;
        
        OSError retProbe =  driver->driverMethods->probeDevice(driver , node);
        
        if( retProbe == OSError_None)
        {
            kprintf("Attaching driver '%s' to device '%s'\n" , driver->base.k_name , node->base.obj.k_name);
            
            node->_attachedDriver = driver;
        }
    }
    
    return OSError_Some;
}

static NO_NULL_POINTERS  OSError _DriverKitTryProbeNodeAndChildren( IONode* node )
{
    
    
    OSError ret = _DriverKitTryProbeNode(node);
    if (ret == OSError_None)
    {
        return ret;
    }
        
    struct kobject* obj = NULL;
    kset_foreach((&node->base), obj)
    {
        IONode* child = (IONode* ) obj;
        
        _DriverKitTryProbeNodeAndChildren(child);
    }
    
    return OSError_None;
}


OSError DriverKitDoMatching()
{
    kprintf("DriverKit : Start matching process \n" );
    OSError ret = OSError_None;
    
    
    ret = _DriverKitTryProbeNodeAndChildren(&_dkContext.deviceTree);
    
    kprintf("DriverKit : End matching process \n" );
    return ret;
}
