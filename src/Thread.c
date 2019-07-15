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

#include "Thread.h"
#include "Timer.h"
#include <assert.h>
#include <stdio.h>

static const char threadDefaultName[] = "Thread";
static  uint32_t _idCounter = 1; // 0 is kernel_task's root thread

static void ThreadGetInfos( const struct kobject *obj , char outDesc[MAX_DESC_SIZE] );


static const KClass threadClass = KClassMake("Thread", ThreadGetInfos,NULL /*Release*/);
//const KClass *ThreadClass = &threadClass;

static void _KernelThreadStart(void *arg0, void *arg1, void *ipc_buf)
{
	KernelThread* self = (KernelThread*) arg0;
	assert(self);

	self->entryPoint(self , arg1 , ipc_buf);
}

OSError ThreadInitBase( Thread* thread)
{
    ALWAYS_ASSERT(thread);
    
    kobject_init(&thread->obj);
    thread->obj.k_name = threadDefaultName;
    thread->obj._class = &threadClass;
    thread->threadID = _idCounter++;
    
    return OSError_None;
}

OSError KernelThreadInit(KernelThread* thread)
{
    memset(thread , 0 , sizeof(KernelThread));
    
    OSError err = ThreadInitBase(&thread->base);
    
    if( err == OSError_None)
    {
        thread->base.type = ThreadType_Kernel;
    }
    return err;
}


OSError ThreadSetName(Thread* thread , const char* name)
{
    thread->obj.k_name = name;
    return OSError_None;
}

const char* ThreadGetName( const Thread* thread)
{
    return thread->obj.k_name;
}

void ThreadSetParent( Thread* thread , Thread* parent)
{
    thread->parent = parent;
}

void ThreadRelease(Thread* thread,vka_t *vka, vspace_t *alloc)
{
    TimerCancelFromThread(thread); // cancel any pending timer
    
    if( thread->type == ThreadType_Kernel)
    {
        KernelThread* self = (KernelThread*) thread;
        sel4utils_clean_up_thread(vka,alloc, &self->thread);
    }
    else if( thread->type == ThreadType_User)
    {
        kprintf("ThreadRelease Todo : add cleanup code for process\n");
    }
    
    if(thread->ipc_ep_cap )
    {
        // delete the cap
        int err = seL4_CNode_Delete(
                                seL4_CapInitThreadCNode,
                                thread->ipc_ep_cap,
                                seL4_WordBits);
    
        assert(err == 0);
    }
    kobject_put(&thread->obj);
    
}

OSError KernelThreadConfigure(KernelThread* thread , vka_t *vka, vspace_t *parent, sel4utils_thread_config_t fromConfig)
{
	return sel4utils_configure_thread_config(vka , parent , /*alloc*/parent , fromConfig , &thread->thread);
}

OSError KernelThreadConfigureWithFaultEndPoint(KernelTaskContext *ctx,Thread* thread ,
                                    vka_t *vka,
                                    vspace_t *parent,
                                    vka_object_t rootEndpoint,
                                    seL4_Word ipc_badge)
{
    sel4utils_thread_config_t threadConf = thread_config_new(&ctx->simple);
    
    int err = 0;
    
    /* create a FAULT endpoint */
    /* allocate a cspace slot for the fault endpoint */
    seL4_CPtr fault_ep = 0;
    err = vka_cspace_alloc(
                           &ctx->vka,
                           &fault_ep);
    ZF_LOGF_IF(err != 0, "Failed to allocate thread fault endpoint");
    
    /* create a badged fault endpoint for the thread */
    err = seL4_CNode_Mint(
                          simple_get_cnode(&ctx->simple),
                          fault_ep,
                          seL4_WordBits,
                          seL4_CapInitThreadCNode,
                          rootEndpoint.cptr,
                          seL4_WordBits,
                          seL4_AllRights,
                          ipc_badge
                          );
    ZF_LOGF_IF(err != 0, "Failed to mint badged fault endpoint for thread");
    /**/
    
    threadConf = thread_config_fault_endpoint(threadConf , fault_ep);
    
    return KernelThreadConfigure(thread , &ctx->vka, &ctx->vspace, threadConf);
}


OSError KernelThreadSetPriority(KernelThread* thread , uint8_t priority)
{
    
#ifndef SOFA_TESTS_ONLY
    return seL4_TCB_SetPriority(thread->thread.tcb.cptr, seL4_CapInitThreadTCB ,  priority);
#else
    return OSError_Unimplemented;
#endif
}

OSError KernelThreadStart(KernelThread* thread , void* arg,   int resume)
{
	return sel4utils_start_thread(&thread->thread , _KernelThreadStart , thread , arg , resume);
}


static void ThreadGetInfos( const struct kobject *obj , char outDesc[MAX_DESC_SIZE] )
{
    const Thread* self = (Thread*) obj;
    
    snprintf(outDesc, MAX_DESC_SIZE, "Thread ID %i Parent %p" , self->threadID , self->parent);
}
