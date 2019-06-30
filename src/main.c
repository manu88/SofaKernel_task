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

#include <string.h>
#include "Sofa.h"
#include "Bootstrap.h"
#include "FileSystem/FileSystem.h"

#include "DriverKit/DriverKit.h"

#include "Timer.h"
#include "Thread.h"
#include "ThreadManager.h"
#include "Shell.h"

#include "Drivers/PCIDriver.h"


#define IRQ_EP_BADGE       BIT(seL4_BadgeBits - 1)
#define IRQ_BADGE_TIMER    (1 << 0)
#define IRQ_BADGE_NETWORK  (1 << 1)
#define IRQ_BADGE_KEYBOARD (1 << 2)

#ifndef SOFA_TESTS_ONLY
#include <platsupport/plat/acpi/acpi.h>
#include <sel4platsupport/io.h>
#else
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "SysCalls.h"
#include "SysCallHandler.h"

static uint8_t* readAndFillBuffer(const char* fromFile , size_t* bufSize)
{
    int fd = open(fromFile, O_RDONLY);
    
    uint8_t c = 0;
    
    uint8_t* ptr = NULL;
    
    size_t size = 0;
    
    while(  read(fd, &c, 1) >0)
    {
        uint8_t* lastP = ptr;
        ptr =  realloc(ptr, ++size);
        
        if (!ptr)
        {
            free(lastP);
            *bufSize = 0;
            return NULL;
        }
        ptr[size-1] = c;
        
    }
    
    *bufSize = size;
    
    return ptr;
}

#endif //#ifndef SOFA_TESTS_ONLY

/*
 Get ACPI
 Init DriverKit
 Creates DeviceTree
 
 */
static OSError earlySystemInit(KernelTaskContext *ctx);

/*
 FileSystem Init
 Drivers loading + matching/probing
 */
static OSError baseSystemInit(KernelTaskContext *ctx);


static void lateSystemInit(KernelTaskContext *ctx);


/* ****** */
static struct kset root = {0};
static const char rootNodeName[] = "root";




/* ****** */
// globals to remove at some point

static Thread shellThread;

PCIDriver _pciDriver;


/* ****** */
int main(void)
{
    KernelTaskContext ctx = {0};
    memset(&ctx , 0 , sizeof(KernelTaskContext) );
    
#ifndef SOFA_TESTS_ONLY
    zf_log_set_tag_prefix("kernel_task");
    ctx.info = platsupport_get_bootinfo();
    ZF_LOGF_IF(ctx.info == NULL, "Failed to get bootinfo.");
#endif
    
    int err = bootstrapSystem(&ctx);

    if( err != 0)
        return 0;
    
    kprintf("Start SOFA\n");
    ALWAYS_ASSERT_NO_ERR( earlySystemInit(&ctx) );
    ALWAYS_ASSERT_NO_ERR( baseSystemInit(&ctx)  );
    lateSystemInit(&ctx);
    
    return 0;
}


static OSError earlySystemInit(KernelTaskContext *context)
{
    kprintf("Early System Init\n");
    uint8_t *acpiBuffer = NULL;
    size_t acpiBufferSize = 0;
#ifdef SOFA_TESTS_ONLY
    acpiBuffer = readAndFillBuffer("out.aml"/*qemu-dsdt.aml"*/, &acpiBufferSize);
#else
    ps_io_mapper_t io_mapper;
    int error =  sel4platsupport_new_io_mapper(context->vspace, context->vka, &io_mapper);
    ALWAYS_ASSERT(error == 0);
    
    acpi_t* acpi = acpi_init(io_mapper);
    
    ALWAYS_ASSERT(acpi != NULL);
    
    acpi_dsdt_t* dsdt = acpi_find_region(acpi, ACPI_DSDT);
    ALWAYS_ASSERT(dsdt != NULL);
    
    acpiBuffer = dsdt->definition_block;
    acpiBufferSize = dsdt->header.length;
#endif
    
    ALWAYS_ASSERT(acpiBuffer);
    kprintf("Start Parsing ACPI table\n");
    
    kset_init(&root);
    root.obj.k_name = rootNodeName;
    //ALWAYS_ASSERT(IONodeInit(&root, IONodeType_Node, "DeviceTree") == OSError_None);
    ALWAYS_ASSERT_NO_ERR(DriverKitInit(&root, acpiBuffer, acpiBufferSize) );
    
#ifndef SOFA_TESTS_ONLY
/* create an endpoint. */
    error = vka_alloc_endpoint(&context->vka, &context->rootTaskEP);
    ZF_LOGF_IFERR(error, "Failed to allocate new endpoint object.\n");
    
    vka_cspace_make_path(&context->vka, context->rootTaskEP.cptr, &context->ep_cap_path);
    
    error = vka_alloc_notification(&context->vka, &context->ntfn_object);
    ALWAYS_ASSERT(error == 0);
    
    error = seL4_TCB_BindNotification(seL4_CapInitThreadTCB, context->ntfn_object.cptr);
    ZF_LOGF_IFERR(error, "Unable to BindNotification.\n");
    
    cspacepath_t notification_path;
    vka_cspace_make_path( &context->vka, context->ntfn_object.cptr, &notification_path);
    
    /* System Timer */
    
    error = TimerInit(context , notification_path.capPtr);
    ALWAYS_ASSERT( error == 0);
#endif
    return OSError_None;
}

static OSError baseSystemInit(KernelTaskContext *context)
{
    kprintf("Base System Init\n");
    
    struct kobject* fsNode = FileSystemInit();
    ALWAYS_ASSERT( fsNode);
    
    ALWAYS_ASSERT_NO_ERR(kset_append(&root, fsNode));
    kobject_put(fsNode);
    
    ALWAYS_ASSERT_NO_ERR( PCIDriverInit(&_pciDriver) );
    ALWAYS_ASSERT_NO_ERR(DriverKitRegisterDriver( (IODriverBase*)&_pciDriver) );
    kobject_put((struct kobject *)&_pciDriver);
    
    
    
    
    ALWAYS_ASSERT_NO_ERR( DriverKitDoMatching( context) );
    
    return OSError_None;
}

static void processLoop(KernelTaskContext* context, seL4_CPtr epPtr  );


static int OnTime(uintptr_t token)
{
    printf("ON TIME\n");
    
    return 0;
}

static void ThreadTest(Thread *self, void *arg, void *ipc_buf)
{
    printf("Thread %i test Started\n" , self->threadID);
    
    while (1)
    {
        SC_usleep(self->ipc_ep_cap, 10000);
    }
}

static void ThreadTest2(Thread *self, void *arg, void *ipc_buf)
{
    printf("Thread %i test Started\n" , self->threadID);
    
    while (1)
    {
        SC_usleep(self->ipc_ep_cap, 200000);
    }
}

static void TestThreadIsReleased( struct kobject *obj)
{
    Thread* thread = (Thread*) obj;
    
    printf("TEST THREAD %i IS RELEASED \n" , thread->threadID);
    
    kfree(obj);
    
    
}
static void ThreadShell(Thread *self, void *arg, void *ipc_buf)
{
    IODevice* comDev =(IODevice*) kset_getChildByName(kset_getChildByName(&root, "Devices") , "COM1");
    ALWAYS_ASSERT(comDev);
    
    ShellRun( self,comDev ,&root);

}

static Thread* ThreadPrepare(KernelTaskContext *ctx, Thread* thread , ThreadEntryPoint entryPoint)
{
    OSError err = ThreadInit(thread);
    if( err != OSError_None)
        return NULL;
    
    ALWAYS_ASSERT( thread->threadID >= 1);
    
    seL4_Word threadBadge = thread->threadID;
    
    err = ThreadConfigureWithFaultEndPoint(ctx, thread, &ctx->vka, &ctx->vspace, ctx->rootTaskEP, threadBadge);
    
    
    if( err != OSError_None)
        return NULL;
    
    thread->entryPoint = entryPoint;
    
    /* create a IPC endpoint */
    
    /* allocate a cspace slot for the IPC endpoint */
    err = vka_cspace_alloc(
                           &ctx->vka,
                           &thread->ipc_ep_cap);
    ZF_LOGF_IF(err != 0, "Failed to allocate thread IPC endpoint");
    
    
    /* create a badged IPC endpoint for the thread */
    err = seL4_CNode_Mint(
                          simple_get_cnode(&ctx->simple),
                          thread->ipc_ep_cap,
                          seL4_WordBits,
                          seL4_CapInitThreadCNode,
                          ctx->rootTaskEP.cptr,
                          seL4_WordBits,
                          seL4_AllRights,
                          threadBadge);
    ZF_LOGF_IF(err != 0, "Failed to mint badged IPC endpoint for thread");
    
    /**/
    
    return thread;
}

void spawnTest(KernelTaskContext *ctx)
{
    Thread* testThread = kmalloc(sizeof(Thread));
    
    ALWAYS_ASSERT( ThreadPrepare(ctx, testThread, ThreadTest2) == testThread);
    
    testThread->obj.methods.release = TestThreadIsReleased;
    OSError ret = ThreadManagerAddThread(testThread);
    
    ALWAYS_ASSERT_NO_ERR(ret);
    
    kobject_put((struct kobject *)testThread);
    ThreadSetParent(testThread, NULL);
    ALWAYS_ASSERT_NO_ERR(ThreadStart(testThread , NULL , 1) );
}

static void startShell(KernelTaskContext* ctx)
{
    Thread* t = ThreadPrepare(ctx , &shellThread , ThreadShell);
    ALWAYS_ASSERT(t);
    
    ThreadSetName(&shellThread ,"Shell");
    ThreadSetParent(&shellThread, NULL);
    
    OSError ret = ThreadManagerAddThread(&shellThread);
    ALWAYS_ASSERT_NO_ERR(ret);
    
    kobject_put((struct kobject *)&shellThread);
    
    ALWAYS_ASSERT_NO_ERR(ThreadStart(&shellThread , NULL , 1) );

}
static void lateSystemInit(KernelTaskContext *ctx)
{
    kprintf("Late System Init\n");
    //kobject_printTree( (const struct kobject *) &root);
    
    OSError ret = ThreadManagerInit();
    ALWAYS_ASSERT_NO_ERR(ret);
    
    kset_append(&root, ThreadManagerGetHandle() );
    
    startShell(ctx);
    int err = 0;
    
    
/* ---- ---- ---- ---- ---- ---- ---- ---- ---- */
    
    Thread* testThread = kmalloc(sizeof(Thread));
    ALWAYS_ASSERT( ThreadPrepare(ctx, testThread, ThreadTest) == testThread);
    
    testThread->obj.methods.release = TestThreadIsReleased;
    ret = ThreadManagerAddThread(testThread);
    
    ALWAYS_ASSERT_NO_ERR(ret);
    
    kobject_put((struct kobject *)testThread);
    ThreadSetParent(testThread, NULL);
    ALWAYS_ASSERT_NO_ERR(ThreadStart(testThread , NULL , 1) );
    
    /*
    for (int i=0;i<4;i++)
    {
        spawnTest(ctx);
    }
     */
    
    
/* ---- ---- ---- ---- ---- ---- ---- ---- ---- */

#ifndef SOFA_TESTS_ONLY
    //int err = TimerAllocAndRegister(&ctx->tm , 1000*NS_IN_MS, 0, 0, OnTime, 0);
    //ALWAYS_ASSERT_NO_ERR(err);
    
    kprintf("Start Looping\n");
    processLoop(ctx, ctx->rootTaskEP.cptr);
#endif
    //kobject_printTree(&root);
    //DriverKitDump();
}

#ifndef SOFA_TESTS_ONLY
static void processTimer(KernelTaskContext* context,seL4_Word sender_badge)
{
    //kprintf("Process Timer \n");
    sel4platsupport_handle_timer_irq(&context->timer, sender_badge);
    int err = tm_update(&context->tm);
}



static void processLoop(KernelTaskContext* context, seL4_CPtr epPtr  )
{
    kprintf("Start Main Process Loop\n");
    
    int error = 0;
    while(1)
    {
        
        seL4_Word sender_badge = 0;
        seL4_MessageInfo_t message;
        seL4_Word label;
        
        //kprintf("[kernTask] Listening...\n");
        message = seL4_Recv(epPtr, &sender_badge);
        //kprintf("[kernTask] Got a message\n");

        label = seL4_MessageInfo_get_label(message);
        
        if(sender_badge & IRQ_EP_BADGE)
        {
            
            if (sender_badge & IRQ_BADGE_TIMER)
            {
                //         processTimer(context ,sender_badge);
            }
            else
            {
                /*
                IOBaseDevice *dev = DriverKitGetDeviceForBadge( sender_badge - IRQ_EP_BADGE );
                
                if (dev)
                {
                    dev->HandleIRQ(dev , -1);
                    //            continue;
                }
                 */
                /*            else
                 {
                 printf("NOT FOUND device for badge %lx\n" , sender_badge - IRQ_EP_BADGE);
                 
                 }
                 */
            }
            
            processTimer(context ,sender_badge);
        }
        else if (label == seL4_VMFault)
        {
            
            
            Thread* callingThread = ThreadManagerGetThreadWithID( sender_badge);
            
            ALWAYS_ASSERT( callingThread);
            
            printf("[kernTask] VM Fault from %li Thread '%s' %i \n",sender_badge ,ThreadGetName(callingThread ) , callingThread->threadID );
            
            
            OSError err = ThreadManagerRemoveThread(callingThread);
            ALWAYS_ASSERT_NO_ERR(err);
            
            ThreadRelease(&shellThread , &context->vka, &context->vspace);
            
            /*
            if( callingThread == &shellThread)
            {
                startShell(context);
            }
             */
            printf("Remaining ref count from thread : %i\n" , callingThread->threadID);
            
            struct kobject * threadManager = kobjectResolve("/ThreadManager", &root);
            
            kobject_printTree(threadManager);
            
            
        }
        else if( label == seL4_CapFault)
        {
            printf("[kernTask] CAP Fault \n");
        }
        
        else if (label == seL4_NoFault)
        {
            //printf("[kernTask] Syscall from %li (%i args) \n" ,sender_badge, seL4_MessageInfo_get_length(message));
            processSysCall(context , message , sender_badge);
            
            /*
            Process* senderProcess =  ProcessTableGetByPID( sender_badge);
            
            if(!senderProcess)
            {
                printf("kernel_task : no sender process for badge %li\n", sender_badge);
                assert(0);
                continue;
            }
            
            processSyscall(context,senderProcess , message , sender_badge );
             */
        }
         
        else
        {
            printf("kernel_task.ProcessLoop : other msg \n");
        }
    } // end while(1)
}


#endif
