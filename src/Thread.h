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
#else
typedef int sel4utils_thread_t;
typedef int vka_t;
typedef int vspace_t;
typedef int sel4utils_thread_config_t;
typedef int seL4_Word;
typedef int vka_object_t;
#endif
#include "Bootstrap.h"
#include "KObject/KObject.h"

struct _Thread;

typedef void (*ThreadEntryPoint)(struct _Thread *self, void *arg, void *ipc_buf);

typedef enum
{
    ThreadType_Kernel,
    ThreadType_User,
    
} ThreadType;

typedef enum
{
        ThreadState_Running,
        ThreadState_Suspended,
} ThreadState;


typedef struct _Thread
{
    struct kobject obj; // must remains first
    ThreadType type;
    
    struct _Thread* parent; // Null means attached to the rootTask
    ThreadState state;
    uint32_t threadID;
    seL4_CPtr ipc_ep_cap; // The minted cap for the thread to communicate with the root task
    uint32_t timerID; // > 0 if allocated. Used to sleep
    seL4_CPtr reply;
    
} Thread;

typedef struct _KernelThread
{
    Thread base;
    
    sel4utils_thread_t thread;
    ThreadEntryPoint entryPoint;
    
    
    
} KernelThread;

// -------- Thread methods

// called by ProcessInit & KernelThreadInit
OSError ThreadInitBase( Thread* thread , ThreadType type) NO_NULL_ARGS(1, 1);
void ThreadRelease(Thread* thread ,vka_t *vka, vspace_t *alloc) NO_NULL_POINTERS;

void ThreadSetParent( Thread* thread , Thread* parent) NO_NULL_ARGS(1, 1);
OSError ThreadSetName(Thread* thread , const char* name) NO_NULL_POINTERS;
const char* ThreadGetName( const Thread* thread) NO_NULL_POINTERS;





// --------  KernelThread methods

/* Will init everything except the Sel4 Thread */
OSError KernelThreadInit(KernelThread* thread) NO_NULL_POINTERS;
OSError KernelThreadSetPriority(KernelThread* thread , uint8_t priority);

OSError KernelThreadConfigure(KernelThread* thread , vka_t *vka, vspace_t *parent, sel4utils_thread_config_t fromConfig) NO_NULL_ARGS(1 ,3);

// init a thread with an endpoint. This endpoint will be minted with the provided badge.
OSError KernelThreadConfigureWithFaultEndPoint(KernelTaskContext *ctx,Thread* thread ,
                                    vka_t *vka,
                                    vspace_t *parent,
                                    vka_object_t rootEndpoint,
                                    seL4_Word ipc_badge) NO_NULL_ARGS(1 ,4);

OSError KernelThreadStart(KernelThread* thread , void* arg,  int resume) ;


