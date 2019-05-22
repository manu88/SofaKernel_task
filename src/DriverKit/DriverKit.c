//
//  DriverKit.c
//  kernel_taskSofaV2
//
//  Created by Manuel Deneu on 22/05/2019.
//  Copyright © 2019 Manuel Deneu. All rights reserved.
//

#include "DriverKit.h"
#include "deviceTree.h"

static IONode driverKitNode = {0};

static const char dkName[] = "DriverKit";

OSError DriverKitInit(IONode* root, const uint8_t* fromDatas, size_t bufferSize)
{
    
    ALWAYS_ASSERT(IONodeInit(&driverKitNode, IONodeType_Node, dkName) == OSError_None);
    
    ALWAYS_ASSERT(IONodeAddChild(root, &driverKitNode) == OSError_None);
    
    OSError ret = DeviceTreeContructDeviceTree(&driverKitNode, fromDatas, bufferSize);
    
    
    return ret;
}
