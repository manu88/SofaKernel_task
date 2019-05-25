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

#include <stdio.h>
#include <string.h>
#include <AMLDecompiler.h>
#include <EISAID.h>
#include "deviceTree.h"
#include "ACPIBuilder.h"




OSError DeviceTreeContructDeviceTree(IONode* root, const uint8_t* fromDatas, size_t bufferSize)
{
    AMLDecompiler decomp;
    DeviceTreeContext ctx = {0};
    
    StackInit(&ctx.devStack);
    
    ctx.rootDevRef = root;
    
    memset(ctx.expectedName,0,5);
    StackPush(&ctx.devStack, root);
    //ctx.currentDevice = root;
    
    if (AMLDecompilerInit(&decomp) == 0)
    {
        return OSError_InitError;
    }
    decomp.callbacks = *getCallbacks();
    decomp.userData = &ctx;
    
    AMLParserError err = AMLDecompilerStart(&decomp, fromDatas, bufferSize);
    
    
    printf("AMLParserError returned %i\n" , err);
    
    if (err != AMLParserError_None)
    {
        return OSError_Some;
    }
    

    
    return OSError_None;
}

IONode* DeviceTreeGetDeviceWithPath(IONode* root, const char* path)
{
    if (strlen(path) == 0)
        return NULL;
    
    char p[512];
    strncpy(p, path, 512);
    
    char *seg = NULL;
    seg = strtok(p, ".");
    
    IONode* currentRoot = root;
    
    while (seg != NULL)
    {
        //struct kobject *child = NULL;
        
        if (kset_count( ((struct kset*)currentRoot) ) == 0)
        {
            return NULL;
        }
        
        currentRoot = (IONode*) kset_getChildByName( (struct kset*) currentRoot, seg);
        /*
         kset_foreach( ((struct kset*)currentRoot), child)
         {
         if ( strcmp( child->k_name  , seg) == 0)
         {
         currentRoot = (IODevice*) child;
         break;
         }
         }
         */
        
        seg = strtok(NULL, ".");
    }
    
    
    return (IONode*) currentRoot;
}
