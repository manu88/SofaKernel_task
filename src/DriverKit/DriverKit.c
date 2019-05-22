//
//  DriverKit.c
//  kernel_taskSofaV2
//
//  Created by Manuel Deneu on 22/05/2019.
//  Copyright © 2019 Manuel Deneu. All rights reserved.
//

#include "DriverKit.h"
#include "deviceTree.h"

OSError DriverKitInit(IONode* root, const uint8_t* fromDatas, size_t bufferSize)
{
    
    OSError ret = DeviceTreeContructDeviceTree(root, fromDatas, bufferSize);
    
    
    return ret;
}
