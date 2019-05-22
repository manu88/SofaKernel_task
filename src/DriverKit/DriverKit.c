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

    ALWAYS_ASSERT(IONodeInit(&_dkContext.driverKitNode, IONodeType_Node, driverKitNodeName) == OSError_None);
    ALWAYS_ASSERT(IONodeInit(&_dkContext.deviceTree   , IONodeType_Node, deviceTreeName)    == OSError_None);
    ALWAYS_ASSERT(IONodeInit(&_dkContext.driversNode  , IONodeType_Node, driversNodeName)   == OSError_None);
    
    ALWAYS_ASSERT(kset_append(root, (struct kobject*) &_dkContext.driverKitNode) == OSError_None);
    
    ALWAYS_ASSERT(IONodeAddChild(&_dkContext.driverKitNode, &_dkContext.deviceTree) == OSError_None);
    ALWAYS_ASSERT(IONodeAddChild(&_dkContext.driverKitNode, &_dkContext.driversNode) == OSError_None);
    
    OSError ret = DeviceTreeContructDeviceTree(&_dkContext.deviceTree, fromDatas, bufferSize);
    
    
    return ret;
}


static void _printObject(const IONode* object , int indent)
{
    
    for(int i =0;i<indent;i++)
        printf("|\t");
    
    printf("|'%s' Type %i \n" , object->base.obj.k_name , object->type  );//, child->type == INodeType_Folder? "Folder":"File");
    
    
    
    if( object->type != IONodeType_Node)
    {
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
    return IONodeAddChild(&_dkContext.driversNode, (IONode*) driver);
    
}
