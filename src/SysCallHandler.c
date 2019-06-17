//
//  SysCallHandler.c
//  kernel_taskSofaV2
//
//  Created by Manuel Deneu on 17/06/2019.
//  Copyright © 2019 Manuel Deneu. All rights reserved.
//

#include "SysCallHandler.h"

void processSysCall(KernelTaskContext* context , seL4_MessageInfo_t message, seL4_Word sender_badge)
{
    const int msgLen = seL4_MessageInfo_get_length(message);
    printf("[kernTask] Syscall from %li (%i args) \n" ,sender_badge, msgLen);
    
    printf("Arg0 %li Arg1 %li\n", seL4_GetMR(0),seL4_GetMR(1) );
    
    seL4_SetMR(0,42);
    seL4_SetMR(1, 43 );
    seL4_Reply( message );
}
