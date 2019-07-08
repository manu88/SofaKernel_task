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

#ifndef SOFA_TESTS_ONLY
#include <sel4platsupport/bootinfo.h>
#include <allocman/vka.h>
#include <simple/simple.h>
#include <sel4platsupport/timer.h>
#include <sel4platsupport/io.h>

#include <platsupport/time_manager.h>
#include <platsupport/chardev.h>
#endif

#include "KObject/KObject.h"

typedef struct _KernelTaskContext
{
    
#ifndef SOFA_TESTS_ONLY
    seL4_BootInfo *info;

    cspacepath_t ep_cap_path;

    simple_t       simple;
    vka_t          vka;
    allocman_t *   allocman;
    vspace_t       vspace;

    time_manager_t tm;
    seL4_timer_t   timer;
    vka_object_t   ntfn_object;
    ps_io_ops_t ops;

    struct ps_io_ops    opsIO;
    
    vka_object_t rootTaskEP;
    
#else
    void* unused;
#endif
} KernelTaskContext;

extern struct kset* RootObject; // created in main.c -> earlySystemInit
int bootstrapSystem(KernelTaskContext* context) NO_NULL_POINTERS;


void* kmalloc(size_t size) NO_NULL_POINTERS WARN_UNUSED_RESULT;
void kfree(void *);


typedef struct _Thread Thread;
void spawnTest(KernelTaskContext *ctx , Thread* parent);
