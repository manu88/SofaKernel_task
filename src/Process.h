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
#include <sel4utils/process.h>
#include "Sofa.h"
#include "Thread.h"

typedef struct _Process
{
    Thread base;
    
    sel4utils_process_t _process;
} Process;


OSError ProcessInit( Process* process) NO_NULL_POINTERS;


OSError ProcessStart(KernelTaskContext* context, Process* process,const char* imageName ) NO_NULL_POINTERS;
