//
//  PCIDriver.c
//  kernel_taskSofaV2
//
//  Created by Manuel Deneu on 22/05/2019.
//  Copyright © 2019 Manuel Deneu. All rights reserved.
//

#include "PCIDriver.h"

static const char pciName[] = "PCIDriver";

OSError PCIDriverInit( PCIDriver* driver)
{
    return IODriverBaseInit(&driver->base, pciName);
}
