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

#include <assert.h>
#include "../Sofa.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../Bootstrap.h"
#include <EISAID.h>
#include "ACPIBuilder.h"
#include "deviceTree.h"

int StackInit(Stack* stack)
{
    ALWAYS_ASSERT(stack);
    memset(stack, 0, sizeof(Stack));
    stack->pos = -1;
    return 1;
}

IONode* StackTop(const Stack* stack)
{
    ALWAYS_ASSERT(stack);
    
    if (stack->pos >= (MAX_STACK-1) || stack->pos == -1)
    {
        ALWAYS_ASSERT(0);
        return NULL;
    }
    return stack->sta[stack->pos];
}


IONode* StackPop(Stack* stack)
{
    ALWAYS_ASSERT(stack);
    
    IONode* cur = stack->sta[stack->pos--];

    return cur;
}

void StackPush(Stack* stack, IONode* dev)
{
    stack->sta[++stack->pos] = dev;
    ALWAYS_ASSERT(stack);
}


static IONode* resolveNodeRelativeTo(const AMLName* name , IONode* current, IONode* root   )
{
    ALWAYS_ASSERT(name);
    
    ALWAYS_ASSERT(root);
    
    
    if (AMLNameIsRoot(name))
        return root;
    
    for(int i=0;i<AMLNameCountParents(name);++i)
    {
        current =  (IONode*) kobject_getParent(&current->base.obj);// current->base.obj.parent;
    }
    
    for (int i=0;i<AMLNameCountSegments(name);++i)
    {
        char toBuffer[5] = "";
        if(AMLNameGetSegment(name, i, toBuffer))
        {
            IONode* newN =(IONode*) kset_getChildByName((const struct kset*) current, toBuffer);
            if(newN == NULL)
            {
                //printf("Create '%s' node\n" , toBuffer);
                newN = kmalloc(sizeof(IONode));
                ALWAYS_ASSERT(newN);
                ALWAYS_ASSERT(IONodeInit(newN, toBuffer) == OSError_None);
                ALWAYS_ASSERT(IONodeAddChild(current, newN) == OSError_None);
                
                kobject_put((struct kobject *)newN);
                
            }
            current = newN;
        }
        //kset_getChildByName
    }
    /*
    IODevice* newScope = malloc(sizeof(IODevice));
    
    if(IODeviceInit(newScope, "NAME") != OSError_None)
    {
        free(newScope);
        
        return NULL;//AMLParserError_ElementAllocError;
    }
    
    
    if(IODeviceAddChild(current, newScope) != OSError_None)
    {
        free(newScope);
        return NULL;//AMLParserError_UnexpectedValue;
    }
    
    return newScope;
     */
    return current;
}


static int _DeviceTree_startResourceTemplate(AMLDecompiler* decomp, const ParserContext* context , size_t numItems )
{
    DeviceTreeContext* treeCtx =(DeviceTreeContext*) decomp->userData;
    ALWAYS_ASSERT(treeCtx);
    
    ALWAYS_ASSERT(treeCtx->expectedName[0] != 0);
    
    
    return 0;
}
static int _DeviceTree_endResourceTemplate(AMLDecompiler* decomp, const ParserContext* context , size_t numItemsParsed, AMLParserError err)
{
    DeviceTreeContext* treeCtx =(DeviceTreeContext*) decomp->userData;
    ALWAYS_ASSERT(treeCtx);
    
    
    ALWAYS_ASSERT(treeCtx->expectedName[0] != 0);
    treeCtx->expectedName[0] = 0;
    
    return 0;
}
static int _DeviceTree_onLargeItem(AMLDecompiler* decomp,const ParserContext* context, LargeResourceItemsType itemType, const uint8_t* buffer , size_t bufferSize)
{
    DeviceTreeContext* treeCtx =(DeviceTreeContext*) decomp->userData;
    ALWAYS_ASSERT(treeCtx);
    
    
    
    return 0;
}
static int _DeviceTree_onSmallItem(AMLDecompiler* decomp,const ParserContext* context, SmallResourceItemsType itemType, const uint8_t* buffer , size_t bufferSize)
{
    DeviceTreeContext* treeCtx =(DeviceTreeContext*) decomp->userData;
    ALWAYS_ASSERT(treeCtx);
    
    IONode* current = StackTop(&treeCtx->devStack);
    
    ALWAYS_ASSERT(current);
    
    uint8_t* dataCpy = kmalloc(bufferSize);
    ALWAYS_ASSERT(dataCpy);
    memcpy(dataCpy, buffer, bufferSize);
    
    IONodeAddAttr(current, treeCtx->expectedName, itemType, dataCpy);

    return 0;
}

static int _DeviceTree_onString(AMLDecompiler* decomp,const ParserContext* context, const char* string)
{
    DeviceTreeContext* treeCtx =(DeviceTreeContext*) decomp->userData;
    ALWAYS_ASSERT(treeCtx);
    
    
    
    ALWAYS_ASSERT(treeCtx->expectedName[0] != 0);
    treeCtx->expectedName[0] = 0;
    
    return 0;
}
static int _DeviceTree_onDefinitionBlock(AMLDecompiler* decomp,const ParserContext* context, const ACPIDefinitionBlock* block)
{
    return 0;
}
static int _DeviceTree_onOperationRegion(AMLDecompiler* decomp,const ParserContext* context, const ACPIOperationRegion* reg)
{
    DeviceTreeContext* treeCtx =(DeviceTreeContext*) decomp->userData;
    ALWAYS_ASSERT(treeCtx);
    

    
    return 0;
}
static int _DeviceTree_startField(AMLDecompiler* decomp,const ParserContext* context, const ACPIField* field)
{
    DeviceTreeContext* treeCtx =(DeviceTreeContext*) decomp->userData;
    ALWAYS_ASSERT(treeCtx);
    
    
    
    return 0;
}
static int _DeviceTree_onFieldElement(AMLDecompiler* decomp,const ParserContext* context, const ACPIFieldElement* fieldElement)
{
    return 0;
}
static int _DeviceTree_endField(AMLDecompiler* decomp,const ParserContext* context, const ACPIField* field)
{
    return 0;
}

static int _DeviceTree_onCreateField(AMLDecompiler* decomp, const ParserContext* context , const ACPICreateFieldBase *field)
{
    return 0;
}

static int _DeviceTree_onBuffer(AMLDecompiler* decomp, const ParserContext* context , size_t bufferSize , const uint8_t* buffer)
{
    DeviceTreeContext* treeCtx =(DeviceTreeContext*) decomp->userData;
    ALWAYS_ASSERT(treeCtx);
    

    
    
    ALWAYS_ASSERT(treeCtx->expectedName[0] != 0);
    treeCtx->expectedName[0] = 0;
    
    return 0;
}



static int _DeviceTree_startScope(AMLDecompiler* decomp,const ParserContext* context, const ACPIScope*scpe )
{
    DeviceTreeContext* treeCtx =(DeviceTreeContext*) decomp->userData;
    ALWAYS_ASSERT(treeCtx);
    
    IONode* newScope = resolveNodeRelativeTo(&scpe->name, StackTop(&treeCtx->devStack), treeCtx->rootDevRef);
    ALWAYS_ASSERT(newScope);
    
    StackPush(&treeCtx->devStack, newScope);

    return 0;
    
}
static int _DeviceTree_endScope(AMLDecompiler* decomp,const ParserContext* context, const ACPIScope*scpe)
{
    
    DeviceTreeContext* treeCtx =(DeviceTreeContext*) decomp->userData;
    ALWAYS_ASSERT(treeCtx);

    ALWAYS_ASSERT(StackTop(&treeCtx->devStack));
    StackPop(&treeCtx->devStack);
    
    return 0;
}
static int _DeviceTree_startDevice(AMLDecompiler* decomp,const ParserContext* context, const ACPIDevice* device)
{
    DeviceTreeContext* treeCtx =(DeviceTreeContext*) decomp->userData;
    ALWAYS_ASSERT(treeCtx);
    
    IONode* newScope = resolveNodeRelativeTo(&device->name, StackTop(&treeCtx->devStack), treeCtx->rootDevRef );
    ALWAYS_ASSERT(newScope);
    
    StackPush(&treeCtx->devStack, newScope);
    return 0;
}
static int _DeviceTree_endDevice(AMLDecompiler* decomp,const ParserContext* context, const ACPIDevice* name)
{
    DeviceTreeContext* treeCtx =(DeviceTreeContext*) decomp->userData;
    ALWAYS_ASSERT(treeCtx);
    ALWAYS_ASSERT(StackTop(&treeCtx->devStack));
    StackPop(&treeCtx->devStack);
    
    return 0;
}


static int _DeviceTree_startName(AMLDecompiler* decomp,const ParserContext* context, const char* name)
{
    DeviceTreeContext* treeCtx =(DeviceTreeContext*) decomp->userData;
    ALWAYS_ASSERT(treeCtx);
    IONode* currentNode = StackTop(&treeCtx->devStack);
    ALWAYS_ASSERT(currentNode);
    
    ALWAYS_ASSERT(treeCtx->expectedName[0] == 0);
    memcpy(treeCtx->expectedName, name, 5);
    
    return 0;
}

static int _DeviceTree_onValue(AMLDecompiler* decomp,const ParserContext* context, uint64_t value)
{
    DeviceTreeContext* treeCtx =(DeviceTreeContext*) decomp->userData;
    ALWAYS_ASSERT(treeCtx);
    IONode* currentNode = StackTop(&treeCtx->devStack);
    ALWAYS_ASSERT(currentNode);
    
    
    //ALWAYS_ASSERT(treeCtx->expectedName[0] != 0);
    
    
    if (strcmp(treeCtx->expectedName, "_HID") == 0)
    {
        currentNode->hid = value;
    }
    else
    {
        IONodeAddAttr(currentNode, treeCtx->expectedName, 0,(void*) value);
    }
    
    treeCtx->expectedName[0] = 0;
    return 0;
}

static int _DeviceTree_onMethod(AMLDecompiler* decomp,const ParserContext* context, const ACPIMethod* method)
{
    DeviceTreeContext* treeCtx =(DeviceTreeContext*) decomp->userData;
    ALWAYS_ASSERT(treeCtx);
    

    
    return 0;
}
static int _DeviceTree_startPackage(AMLDecompiler* decomp,const ParserContext* context, const ACPIPackage* package)
{
    DeviceTreeContext* treeCtx =(DeviceTreeContext*) decomp->userData;
    ALWAYS_ASSERT(treeCtx);
    
    
    
    ALWAYS_ASSERT(treeCtx->expectedName[0] != 0);
    treeCtx->expectedName[0] = 0;
    
    return 0;
}

static int _DeviceTree_onPackageElement(AMLDecompiler* decomp,const ParserContext* context, const ACPIPackageElement* element)
{
    
    
    return 0;
}
static int _DeviceTree_endPackage(AMLDecompiler* decomp,const ParserContext* context, const ACPIPackage* package)
{
    return 0;
}

static int _DeviceTree_startIndexField(AMLDecompiler* decomp,const ParserContext* context, const ACPIIndexField* field)
{
    DeviceTreeContext* treeCtx =(DeviceTreeContext*) decomp->userData;
    ALWAYS_ASSERT(treeCtx);
    

    
    return 0;
}
static int _DeviceTree_onIndexFieldElement(AMLDecompiler* decomp,const ParserContext* context, const ACPIIndexFieldElement* fieldElement)
{
    return 0;
}

static int _DeviceTree_endIndexField(AMLDecompiler* decomp,const ParserContext* context, const ACPIIndexField* field)
{
    return 0;
}

static AMLDecompilerCallbacks _DeviceTreeCallbacks =
{
    
    _DeviceTree_startResourceTemplate,
    _DeviceTree_endResourceTemplate,
    _DeviceTree_onLargeItem,
    _DeviceTree_onSmallItem,
    _DeviceTree_onValue,
    _DeviceTree_onString,
    _DeviceTree_onDefinitionBlock,
    _DeviceTree_onOperationRegion,
    _DeviceTree_startField,
    _DeviceTree_onFieldElement,
    _DeviceTree_endField,
    _DeviceTree_startIndexField,
    _DeviceTree_onIndexFieldElement,
    _DeviceTree_endIndexField,
    _DeviceTree_onCreateField,
    _DeviceTree_onBuffer,
    _DeviceTree_startScope,
    _DeviceTree_endScope,
    _DeviceTree_startDevice,
    _DeviceTree_endDevice,
    _DeviceTree_startName,
    _DeviceTree_onMethod,
    _DeviceTree_startPackage,
    _DeviceTree_onPackageElement,
    _DeviceTree_endPackage
    
};


AMLDecompilerCallbacks* getCallbacks()
{
    return &_DeviceTreeCallbacks;
}
