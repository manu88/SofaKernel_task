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


struct kset* ThreadManagerGetHandle()
{
    return &_threadManagerCtx.set;
}
