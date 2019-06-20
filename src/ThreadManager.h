//
//  ThreadManager.h
//  kernel_taskSofaV2
//
//  Created by Manuel Deneu on 20/06/2019.
//  Copyright © 2019 Manuel Deneu. All rights reserved.
//

#pragma once
#include "Sofa.h"

typedef struct _Thread Thread;


OSError ThreadManagerInit(void);
OSError ThreadManagerAddThread( Thread* thread) NO_NULL_POINTERS;
