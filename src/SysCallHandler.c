//
//  SysCallHandler.c
//  kernel_taskSofaV2
//
//  Created by Manuel Deneu on 17/06/2019.
//  Copyright © 2019 Manuel Deneu. All rights reserved.
//
#include <errno.h>
#include "SysCallHandler.h"
#include "SysCalls.h"
#include "Timer.h"
#include "Thread.h"
#include "Utils.h"


// THis is gross. But only temporary
static uint32_t counter = 0;
static KernelTaskContext* _context = NULL;


static int OnTime(uintptr_t token)
{
    ALWAYS_ASSERT(_context);
    seL4_CPtr reply = (seL4_CPtr) token;
    ALWAYS_ASSERT(reply);
    
    int err = tm_deregister_cb(&_context->tm  , counter-1);
    ALWAYS_ASSERT(err == 0);
    
    err = tm_free_id(&_context->tm , counter-1);
    ALWAYS_ASSERT(err == 0);
    
    //printf("ON TIME \n");
    
    seL4_MessageInfo_t tag = seL4_MessageInfo_new(0, 0, 0, 2);
    seL4_SetMR(0, SysCallNum_nanosleep);
    seL4_SetMR(1, 0); // sucess
    
    seL4_Send(reply , tag);
    
    cnode_delete(_context,reply);
}

static void handleSleep(KernelTaskContext* context,seL4_MessageInfo_t message)
{
    int seconds = seL4_GetMR(1);
    //kprintf("Sleep %i seconds\n" , seconds);
    int error = -ENOSYS;
    
    seL4_CPtr reply = get_free_slot(context);
    if( reply == 0)
    {
        seL4_SetMR(0,SysCallNum_nanosleep);
        seL4_SetMR(1, -EINVAL );
        seL4_Reply( message );
        return;
    }
    
    error = cnode_savecaller( context, reply );
    if( error != 0)
    {
        cnode_delete(context , reply);
        seL4_SetMR(0,SysCallNum_nanosleep);
        seL4_SetMR(1, -EINVAL );
        seL4_Reply( message );
        return;
    }
    
    int err = TimerAllocAndRegisterOneShot(&context->tm , seconds*NS_IN_MS /* To nano seconds*/,  counter++, OnTime, (uintptr_t) reply );
    if( err != 0)
    {
        kprintf("TimerAllocAndRegisterOneShot err %i\n" , err);
    }
    ALWAYS_ASSERT_NO_ERR(err);
    
    
    /*
    seL4_SetMR(0,SysCallNum_Sleep);
    seL4_SetMR(1, 0 );
    seL4_Reply( message );
     */
}


static void handleKill(KernelTaskContext* context, seL4_MessageInfo_t message)
{
    long idToKill = seL4_GetMR(1);
    
    printf("Should kill ID %i \n" , idToKill);
    
    
    Thread* theThread =  ThreadManagerGetThreadWithID( (uint32_t) idToKill );
    
    int err = -EACCES;
    if( theThread)
    {
        OSError ret =  ThreadManagerRemoveThread(theThread);
        if( ret != OSError_None)
        {
            printf("ThreadManagerRemoveThread error %i\n" , ret);
            err = ESRCH;
        }
        else
        {
            ThreadRelease(theThread , &context->vka , &context->vspace);
            err = 0;
        }
    }
    seL4_SetMR(0,SysCallNum_kill);
    seL4_SetMR(1, err );
    seL4_Reply( message );
}

void processSysCall(KernelTaskContext* context , seL4_MessageInfo_t message, seL4_Word sender_badge)
{
    if( _context == NULL)
    {
        _context = context;
    }
    const int msgLen = seL4_MessageInfo_get_length(message);
    ALWAYS_ASSERT(msgLen > 0); // at least the syscall #
    
    const SysCallNum sysCallID = (SysCallNum) seL4_GetMR(0);
    switch (sysCallID)
    {
        case SysCallNum_nanosleep:
            handleSleep(context,message);
            break;
            
        case SysCallNum_kill:
            handleKill(context , message);
            
        default:
            break;
    }
    
    /*
    printf("Arg0 %li Arg1 %li\n", seL4_GetMR(0),seL4_GetMR(1) );
    
    seL4_SetMR(0,42);
    seL4_SetMR(1, 43 );
    seL4_Reply( message );
     */
}
