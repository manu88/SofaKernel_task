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

static struct _ThreadManagerContext
{
    struct kset set;
    
    
} _threadManagerCtx;


OSError ThreadManagerInit()
{
    memset(&_threadManagerCtx, 0, sizeof(struct _ThreadManagerContext));
}

OSError ThreadManagerAddThread( Thread* thread)
{
    return OSError_None;
}
