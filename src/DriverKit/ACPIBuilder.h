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
#include <AMLDecompiler.h>

#include "IODevice.h"

#define MAX_STACK 32
typedef struct
{
    IONode* sta[MAX_STACK];
    int pos;
} Stack;

int       StackInit(Stack* stack);
IONode*   StackPop(Stack* stack);
void      StackPush(Stack* stack , IONode*dev);

IONode* StackTop(const Stack* stack);

typedef struct
{
    IONode *rootDevRef;
    
    Stack devStack;
    
    char expectedName[5];
    
    
} DeviceTreeContext;

AMLDecompilerCallbacks* getCallbacks(void);
