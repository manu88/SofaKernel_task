//
//  deviceTree.c
//  kernel_taskSofaV2
//
//  Created by Manuel Deneu on 08/05/2019.
//  Copyright © 2019 Manuel Deneu. All rights reserved.
//

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
