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
#include <assert.h>
#include <stdio.h>

static const char threadDefaultName[] = "Thread";
static  uint32_t _idCounter = 1; // 0 is kernel_task's root thread

static void ThreadGetInfos( const struct kobject *obj , char outDesc[MAX_DESC_SIZE] );


static const KClass threadClass = KClassMake("Thread", ThreadGetInfos,NULL /*Release*/);
//const KClass *ThreadClass = &threadClass;

static void _ThreadStart(void *arg0, void *arg1, void *ipc_buf)
{
	Thread* self = (Thread*) arg0;
	assert(self);


	self->entryPoint(self , arg1 , ipc_buf);
}

OSError ThreadInit(Thread* thread)
{
    memset(thread , 0 , sizeof(Thread));
    
    kobject_init(&thread->obj);
    thread->obj.k_name = threadDefaultName;
    thread->obj._class = &threadClass;
    thread->threadID = _idCounter++;
    
    return OSError_None;
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
    sel4utils_clean_up_thread(vka,alloc, &thread->thread);
    
    kobject_put(&thread->obj);
}

OSError ThreadConfigure(Thread* thread , vka_t *vka, vspace_t *parent, sel4utils_thread_config_t fromConfig)
{
	return sel4utils_configure_thread_config(vka , parent , /*alloc*/parent , fromConfig , &thread->thread);
}

OSError ThreadConfigureWithFaultEndPoint(KernelTaskContext *ctx,Thread* thread ,
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
                          //IPC_FAULT_ENDPOINT_BADGE(ipc_badge)
                          );
    ZF_LOGF_IF(err != 0, "Failed to mint badged fault endpoint for thread");
    /**/
    
    threadConf = thread_config_fault_endpoint(threadConf , fault_ep);
    
    return ThreadConfigure(thread , &ctx->vka, &ctx->vspace, threadConf);
}




OSError ThreadStart(Thread* thread , void* arg,   int resume)
{
	return sel4utils_start_thread(&thread->thread , _ThreadStart , thread , arg , resume);
}


static void ThreadGetInfos( const struct kobject *obj , char outDesc[MAX_DESC_SIZE] )
{
    const Thread* self = (Thread*) obj;
    
    snprintf(outDesc, MAX_DESC_SIZE, "Thread ID %i Parent %p" , self->threadID , self->parent);
}
