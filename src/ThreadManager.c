//
//  ThreadManager.c
//  kernel_taskSofaV2
//
//  Created by Manuel Deneu on 20/06/2019.
//  Copyright © 2019 Manuel Deneu. All rights reserved.
//

#include <string.h>
#include "KObject/KObject.h"
#include "ThreadManager.h"
#include "Thread.h"

static const char _threadManagerName[] = "ThreadManager";
static struct _ThreadManagerContext
{
    struct kset set;
    
    
} _threadManagerCtx;


OSError ThreadManagerInit()
{
    memset(&_threadManagerCtx, 0, sizeof(struct _ThreadManagerContext));
    
    kset_init(&_threadManagerCtx.set);
    _threadManagerCtx.set.obj.k_name = _threadManagerName;
    return OSError_None;
}

OSError ThreadManagerAddThread( Thread* thread)
{
    return kset_append(&_threadManagerCtx.set, (struct kobject *)thread);
}

OSError ThreadManagerRemoveThread( Thread* thread)
{
    return kset_remove(&_threadManagerCtx.set, (struct kobject *)thread);
}

Thread* ThreadManagerGetThreadWithID( uint32_t id )
{
    struct kobject* obj = NULL;
    kset_foreach( &_threadManagerCtx.set, obj)
    {
        Thread* thread = (Thread*) obj;
        ALWAYS_ASSERT(thread);
        
        if( thread->threadID == id)
        {
            return thread;
        }
    }
    
    return NULL;
}

struct kobject* ThreadManagerGetHandle()
{
    return &_threadManagerCtx.set.obj;
}


