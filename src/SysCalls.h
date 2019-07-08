//
//  SysCalls.h
//  kernel_taskSofaV2
//
//  Created by Manuel Deneu on 17/06/2019.
//  Copyright © 2019 Manuel Deneu. All rights reserved.
//

#pragma once

#include "Sofa.h"

#ifndef SOFA_TESTS_ONLY
#include <seL4/types.h> // seL4_CPtr
#endif

typedef enum
{
    SysCallNum_Unknown   = 0,
    
    SysCallNum_debug     = 1,
    SysCallNum_nanosleep = 2,
    SysCallNum_spawn     = 3,
    SysCallNum_kill      = 4,
    SysCallNum_mount     ,
} SysCallNum;


typedef enum
{
    SysCallDebugID_ps,
} SysCallDebugIDs;


// Syscalls from inside kerneltask ONLY
int SC_usleep(seL4_CPtr cap, int microsecs);
int SC_kill  (seL4_CPtr cap, int id);
int SC_spawn (seL4_CPtr cap);

int SC_mount (seL4_CPtr cap);

int SC_ps(seL4_CPtr cap);
