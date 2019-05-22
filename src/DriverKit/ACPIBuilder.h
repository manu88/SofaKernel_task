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
    IODevice* sta[MAX_STACK];
    int pos;
} Stack;

int       StackInit(Stack* stack);
IODevice* StackPop(Stack* stack);
void      StackPush(Stack* stack , IODevice*dev);

IODevice* StackTop(const Stack* stack);

typedef struct
{
    IODevice *rootDevRef;
    
    Stack devStack;
    
    char expectingName[5];
    
    
} DeviceTreeContext;

AMLDecompilerCallbacks* getCallbacks(void);
