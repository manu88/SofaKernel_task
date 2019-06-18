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
#include <sel4utils/thread.h>
#include <sel4utils/thread_config.h>
#endif
#include "Bootstrap.h"

struct _Thread;

typedef void (*ThreadEntryPoint)(struct _Thread *self, void *arg, void *ipc_buf);


typedef struct _Thread
{
    sel4utils_thread_t thread;
    ThreadEntryPoint entryPoint;
    
    uint32_t threadID;
    seL4_CPtr ipc_ep_cap;
    
} Thread;

OSError ThreadInit(Thread* thread , vka_t *vka, vspace_t *parent, sel4utils_thread_config_t fromConfig);

// init a thread with an endpoint. This endpoint will be minted with the provided badge.
OSError ThreadInitWithFaultEndPoint(KernelTaskContext *ctx,Thread* thread ,
                                    vka_t *vka,
                                    vspace_t *parent,
                                    vka_object_t rootEndpoint,
                                    seL4_Word ipc_badge);

static inline OSError ThreadSetPriority(Thread* thread , uint8_t priority)
{
	return seL4_TCB_SetPriority(thread->thread.tcb.cptr, seL4_CapInitThreadTCB ,  priority);
}


OSError ThreadStart(Thread* thread , void* arg,  int resume);
