//
//  SysCalls.h
//  kernel_taskSofaV2
//
//  Created by Manuel Deneu on 17/06/2019.
//  Copyright © 2019 Manuel Deneu. All rights reserved.
//

#pragma once

#include "Sofa.h"
#include <seL4/types.h> // seL4_CPtr

typedef enum
{
    SysCallNum_Unknown = 0,
    
    SysCallNum_nanosleep = 1,
    SysCallNum_spawn     = 2,
} SysCallNum;


// Syscalls from inside kerneltask ONLY
int SC_usleep(seL4_CPtr cap, int microsecs);
