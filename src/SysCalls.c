/*
 * This file is part of the Sofa project
 * Copyright (c) 2018 Manuel Deneu.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

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

int SC_sched(seL4_CPtr cap)
{
    seL4_MessageInfo_t msg = seL4_MessageInfo_new(0, 0, 0, 2);
    seL4_SetMR(0,  SysCallNum_debug);
    seL4_SetMR(1 , SysCallDebugID_sched);
    seL4_Send(cap , msg);
    
    ALWAYS_ASSERT( seL4_GetMR(0) == SysCallNum_debug);
    
    return 0;
}
