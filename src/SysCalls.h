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
    SysCallNum_Unknown   = 0,
    
    SysCallNum_nanosleep = 1,
    SysCallNum_spawn     = 2,
    SysCallNum_kill      = 3,
    SysCallNum_mount     ,
} SysCallNum;


// Syscalls from inside kerneltask ONLY
int SC_usleep(seL4_CPtr cap, int microsecs);
int SC_kill  (seL4_CPtr cap, int id);
int SC_spawn (seL4_CPtr cap);

int SC_mount (seL4_CPtr cap);
