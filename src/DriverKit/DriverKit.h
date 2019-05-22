//
//  DriverKit.h
//  kernel_taskSofaV2
//
//  Created by Manuel Deneu on 22/05/2019.
//  Copyright © 2019 Manuel Deneu. All rights reserved.
//

#pragma once
#include "../Sofa.h"
#include "IODevice.h"

OSError DriverKitInit(IODevice* root, const uint8_t* fromDatas, size_t bufferSize) NO_NULL_ARGS(1, 1);
