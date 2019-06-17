//
//  SysCallHandler.h
//  kernel_taskSofaV2
//
//  Created by Manuel Deneu on 17/06/2019.
//  Copyright © 2019 Manuel Deneu. All rights reserved.
//

#pragma once

#include "Bootstrap.h"

void processSysCall(KernelTaskContext* context , seL4_MessageInfo_t message, seL4_Word sender_badge);
