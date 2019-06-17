//
//  SysCallHandler.c
//  kernel_taskSofaV2
//
//  Created by Manuel Deneu on 17/06/2019.
//  Copyright © 2019 Manuel Deneu. All rights reserved.
//

#include "SysCallHandler.h"
#include "SysCalls.h"

static void handleSleep( int numSecs,seL4_MessageInfo_t message)
{
    kprintf("Sleep %i seconds\n" , numSecs);
    
    seL4_SetMR(0,SysCallNum_Sleep);
    seL4_SetMR(1, 0 );
    seL4_Reply( message );
}

void processSysCall(KernelTaskContext* context , seL4_MessageInfo_t message, seL4_Word sender_badge)
{
    const int msgLen = seL4_MessageInfo_get_length(message);
    ALWAYS_ASSERT(msgLen > 0); // at least the syscall #
    
    const SysCallNum sysCallID = (SysCallNum) seL4_GetMR(0);
    switch (sysCallID)
    {
        case SysCallNum_Sleep:
            handleSleep(seL4_GetMR(1),message);
            break;
            
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
