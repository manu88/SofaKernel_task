//
//  ACPIBuilder.h
//  kernel_taskSofaV2
//
//  Created by Manuel Deneu on 08/05/2019.
//  Copyright © 2019 Manuel Deneu. All rights reserved.
//

#pragma once
#include <AMLDecompiler.h>

#include "IODevice.h"

#define MAX_STACK 32
typedef struct
{
    IONode* sta[MAX_STACK];
    int pos;
} Stack;

int       StackInit(Stack* stack);
IONode* StackPop(Stack* stack);
void      StackPush(Stack* stack , IONode*dev);

IONode* StackTop(const Stack* stack);

typedef struct
{
    IONode *rootDevRef;
    
    Stack devStack;
    
    char expectedName[5];
    
    
} DeviceTreeContext;

AMLDecompilerCallbacks* getCallbacks(void);
