//
//  SysCalls.c
//  kernel_taskSofaV2
//
//  Created by Manuel Deneu on 19/06/2019.
//  Copyright © 2019 Manuel Deneu. All rights reserved.
//

#include <seL4/seL4.h>
#include "SysCalls.h"

int SC_usleep(seL4_CPtr cap, int microsecs)
{
    seL4_MessageInfo_t msg = seL4_MessageInfo_new(0, 0, 0, 2);
    seL4_SetMR(0,  SysCallNum_nanosleep);
    seL4_SetMR(1 , microsecs);
    msg = seL4_Call(cap , msg);
    
    ALWAYS_ASSERT( seL4_GetMR(0) == SysCallNum_nanosleep);
    
    return -seL4_GetMR(1);
}

