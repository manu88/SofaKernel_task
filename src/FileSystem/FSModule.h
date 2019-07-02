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


#include "../Sofa.h"
#include "../KObject/KObject.h"

struct _FSModule;
typedef struct _IODevice IODevice;
typedef struct
{
    OSError (*init)(struct _FSModule* module);
    OSError (*deinit)(struct _FSModule* module);
    
    OSError (*probe)(struct _FSModule* module, IODevice* device );
    OSError (*mount)(struct _FSModule* module,const char *dir, int flags, void *data);
    
} FSModuleMethods;

typedef struct _FSModule
{
    struct kobject obj;
    
    FSModuleMethods *methods;
    uint8_t isLoaded:1;
} FSModule;


OSError FSModuleInit(FSModule* module , const char* name) NO_NULL_POINTERS;
