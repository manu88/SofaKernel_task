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


static void _printObject(const IONode* object , int indent)
{
    
    for(int i =0;i<indent;i++)
        printf("|\t");
    
    printf("|'%s' Type %i \n" , object->base.obj.k_name , object->type  );//, child->type == INodeType_Folder? "Folder":"File");
    
    
    
    if( object->type != IONodeType_Node)
    {
        for(int i =0;i<indent+1;i++)
            printf("|\t");
        
        if (isEisaId(object->hid))
        {
            char eisaID[8] = "";
            if(getEisaidString( object->hid , eisaID))
            {
                printf("- HID '%s' \n" ,  eisaID);
            }
        }
        else
        {
            printf("- HID 0x%llx \n" ,  object->hid);
        }
    }
    
    struct kobject *child = NULL;
    kset_foreach( ((struct kset*)object), child)
    {
        _printObject((IONode*)child, indent +1);
        /*
         if (child->type == INodeType_Folder)
         {
         _printNode(child , indent + 1);
         }
         */
    }
}

void DriverKitDump(IONode* root)
{
    printf("--- DriverKit Tree ---\n");
    
    _printObject( root , 0);
    
    printf("--- DriverKit Tree ---\n");
}

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
    
    DriverKitDump(root);
    
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
