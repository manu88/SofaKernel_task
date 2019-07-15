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
#include "Process.h"


OSError ProcessInit( Process* process)
{
    memset(process , 0,  sizeof(Process));
    OSError err = ThreadInitBase(&process->base);
    
    if( err == OSError_None)
    {
        process->base.type = ThreadType_User;
    }
    return err;
}


OSError ProcessStart(KernelTaskContext* context, Process* process,const char* imageName )
{
    kprintf("ProcessStart : '%s'\n" , imageName);
    int error = 0;
    uint8_t prio =  0;//seL4_MaxPrio;
    sel4utils_process_config_t config = process_config_default_simple( &context->simple, imageName, prio);
    
    error = sel4utils_configure_process_custom( &process->_process , &context->vka , &context->vspace, config);
    
    if( error != 0)
    {
        kprintf("Error sel4utils_configure_process_custom : %i \n" , error);
        return OSError_Some;
    }
    ZF_LOGF_IFERR(error, "Failed to spawn a new thread.\n"
                  "\tsel4utils_configure_process expands an ELF file into our VSpace.\n"
                  "\tBe sure you've properly configured a VSpace manager using sel4utils_bootstrap_vspace_with_bootinfo.\n"
                  "\tBe sure you've passed the correct component name for the new thread!\n");
    
    
    seL4_Word argc = 1;
    char string_args[argc][WORD_STRING_SIZE];
    char* argv[argc];
    sel4utils_create_word_args(string_args, argv, argc ,10);
    
    error = sel4utils_spawn_process_v(&process->_process , &context->vka , &context->vspace , argc, (char**) &argv , 1);
    ZF_LOGF_IFERR(error, "Failed to spawn and start the new thread.\n"
                  "\tVerify: the new thread is being executed in the root thread's VSpace.\n"
                  "\tIn this case, the CSpaces are different, but the VSpaces are the same.\n"
                  "\tDouble check your vspace_t argument.\n");
    
    if( error != 0)
    {
        kprintf("Error sel4utils_spawn_process_v : %i \n" , error);
        return OSError_Some;
    }
    
    
    ThreadSetName(&process->base , imageName);
    return OSError_None;
}
