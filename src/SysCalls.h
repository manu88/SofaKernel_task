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
    SysCallDebugID_sched,
} SysCallDebugIDs;




// Syscalls from inside kerneltask ONLY
int SC_usleep(seL4_CPtr cap, int microsecs);
int SC_kill  (seL4_CPtr cap, int id);
int SC_spawn (seL4_CPtr cap);

int SC_mount (seL4_CPtr cap);

int SC_ps(seL4_CPtr cap);
int SC_sched(seL4_CPtr cap);
