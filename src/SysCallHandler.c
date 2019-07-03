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
#include "ThreadManager.h"
#include "Utils.h"
#include "FileSystem/VFS.h"
#include "DriverKit/DriverKit.h"

// THis is gross. But only temporary
//static uint32_t counter = 0;
static KernelTaskContext* _context = NULL;


static int OnTime(uintptr_t token)
{

    ALWAYS_ASSERT(_context);
    
    Thread* callingThread = (Thread*) token;
    //seL4_CPtr reply = (seL4_CPtr) token;
    ALWAYS_ASSERT(callingThread->reply);
    
    int err = tm_deregister_cb(&_context->tm  , callingThread->timerID);
    ALWAYS_ASSERT(err == 0);
    
    //printf("ON TIME \n");
    
    seL4_MessageInfo_t tag = seL4_MessageInfo_new(0, 0, 0, 2);
    seL4_SetMR(0, SysCallNum_nanosleep);
    seL4_SetMR(1, 0); // sucess
    
    seL4_Send(callingThread->reply , tag);
    
    cnode_delete(_context,callingThread->reply);
    callingThread->reply = NULL;
}

static void handleSleep(KernelTaskContext* context,Thread* callingThread,seL4_MessageInfo_t message)
{
    
    if( callingThread->timerID == 0)
    {
        
        int err = tm_alloc_id(&context->tm , &callingThread->timerID);
        if( err != 0)
        {
            seL4_SetMR(0,SysCallNum_nanosleep);
            seL4_SetMR(1, -EPERM );
            seL4_Reply( message );
            
        }
    }
    
    ALWAYS_ASSERT(callingThread->timerID > 0);
    ALWAYS_ASSERT( callingThread->reply == NULL);
    int seconds = seL4_GetMR(1);
    //kprintf("Sleep %i seconds\n" , seconds);
    int error = -ENOSYS;
    
    callingThread->reply = get_free_slot(context);
    if( callingThread->reply == NULL)
    {
        seL4_SetMR(0,SysCallNum_nanosleep);
        seL4_SetMR(1, -EINVAL );
        seL4_Reply( message );
        return;
    }
    
    error = cnode_savecaller( context, callingThread->reply );
    if( error != 0)
    {
        cnode_delete(context , callingThread->reply);
        callingThread->reply = NULL;
        seL4_SetMR(0,SysCallNum_nanosleep);
        seL4_SetMR(1, -EINVAL );
        seL4_Reply( message );
        return;
    }
    
    
    
    int err = tm_register_rel_cb( &context->tm , seconds*NS_IN_MS , callingThread->timerID , OnTime , callingThread);
    //TimerAllocAndRegisterOneShot(&context->tm , seconds*NS_IN_MS /* To nano seconds*/,  counter++, OnTime, (uintptr_t) reply );
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


static void handleKill(KernelTaskContext* context,Thread* callingThread, seL4_MessageInfo_t message)
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

static void handleSpawn(KernelTaskContext* context, Thread* callingThread,seL4_MessageInfo_t message)
{
    printf("Spawn\n");
    int err = -EACCES;
    
    seL4_SetMR(0,SysCallNum_spawn);
    seL4_SetMR(1, err );
    
    spawnTest(context);
    seL4_Reply( message );
}

static void handleMount(KernelTaskContext* context, Thread* callingThread,seL4_MessageInfo_t message)
{
    int err = -EINVAL;
    seL4_SetMR(0,SysCallNum_mount);
    seL4_SetMR(1, err );
    
    IODevice* dev = DriverKitGetDevice("hda");
    if( dev)
    {
        VFSMount(dev, "SomePath");
    }
    
    seL4_Reply( message );
}

void processSysCall(KernelTaskContext* context , Thread* callingThread,seL4_MessageInfo_t message, seL4_Word sender_badge)
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
            handleSleep(context,callingThread,message);
            break;
            
        case SysCallNum_kill:
            handleKill(context , callingThread,message);
            break;
            
        case SysCallNum_spawn:
            handleSpawn(context ,callingThread, message);
            break;
            
        case SysCallNum_mount:
            handleMount(context,callingThread, message);
            
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
