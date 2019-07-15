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

#include <stdlib.h>
#ifndef SOFA_TESTS_ONLY
#include <cpio/cpio.h>
#endif
#include "CPIOfs.h"


/* The linker will link this symbol to the start address  *
 * of an archive of attached applications.                */
extern char _cpio_archive[];
extern char _cpio_archive_end[];

static FSModule _cpiofs = {0};
FSModule* cpiofs = &_cpiofs;

static const char name[] = "cpiofs";


static void CPIODumpContent(struct cpio_info *info);
OSError CPIOfsInit()
{
    struct cpio_info info;
    unsigned long len = _cpio_archive_end - _cpio_archive;
    
    if(cpio_info(_cpio_archive, len , &info) != 0)
    {
        kprintf("Error gettings CPIO archive\n");
        cpiofs = NULL;
        return OSError_DeviceNotFound;
    }
    // check if already init
    if( _cpiofs.obj.k_name != NULL)
        return OSError_None;
    
    OSError err =  FSModuleInit(&_cpiofs, name);
    
    if( err == OSError_None)
    {
        cpiofs = &_cpiofs;
        
        kprintf("CPIO Module is OK\n");
        CPIODumpContent(&info);
        //_cpiofs.methods = &cpiofsMethods;
    }
    return err;
}


static void CPIODumpContent(struct cpio_info *info)
{
    kprintf("----- CPIO Content (%zi files) --- \n" , info->file_count);
    
    uint8_t **buf = malloc( info->file_count);
    assert(buf);
    
    for(int i=0;i<info->file_count;i++)
    {
        buf[i] = malloc(info->max_path_sz );
        assert(buf[i]);
        memset(buf[i] , 0 , info->max_path_sz);
    }
    
    unsigned long len = _cpio_archive_end - _cpio_archive;
    cpio_ls(_cpio_archive ,len, buf, info->file_count);
    
    
    
    for(int i=0;i<info->file_count;i++)
    {
        kprintf("'%s'\n" , buf[i]);
        free(buf[i]);
    }
    
    free(buf);
    kprintf("----- END CPIO Content --- \n");
}
