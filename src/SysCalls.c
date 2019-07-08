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

int SC_kill  (seL4_CPtr cap, int id)
{
    seL4_MessageInfo_t msg = seL4_MessageInfo_new(0, 0, 0, 2);
    seL4_SetMR(0,  SysCallNum_kill);
    seL4_SetMR(1 , id);
    msg = seL4_Call(cap , msg);
    
    ALWAYS_ASSERT( seL4_GetMR(0) == SysCallNum_kill);
    
    return -seL4_GetMR(1);
}

int SC_spawn (seL4_CPtr cap)
{
    seL4_MessageInfo_t msg = seL4_MessageInfo_new(0, 0, 0, 2);
    seL4_SetMR(0,  SysCallNum_spawn);
    //seL4_SetMR(1 , id);
    msg = seL4_Call(cap , msg);
    
    ALWAYS_ASSERT( seL4_GetMR(0) == SysCallNum_spawn);
    
    return -seL4_GetMR(1);
}

int SC_mount(seL4_CPtr cap)
{
    seL4_MessageInfo_t msg = seL4_MessageInfo_new(0, 0, 0, 2);
    seL4_SetMR(0,  SysCallNum_mount);
    //seL4_SetMR(1 , id);
    msg = seL4_Call(cap , msg);
    
    ALWAYS_ASSERT( seL4_GetMR(0) == SysCallNum_mount);
    
    return -seL4_GetMR(1);
}


int SC_ps(seL4_CPtr cap)
{
    seL4_MessageInfo_t msg = seL4_MessageInfo_new(0, 0, 0, 2);
    seL4_SetMR(0,  SysCallNum_debug);
    seL4_SetMR(1 , SysCallDebugID_ps);
    seL4_Send(cap , msg);
    
    ALWAYS_ASSERT( seL4_GetMR(0) == SysCallNum_debug);
    
    return 0;
}
