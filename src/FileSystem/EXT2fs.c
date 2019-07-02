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

#include "EXT2fs.h"
#include "../DriverKit/IODevice.h"

static FSModule _ext2fs = {0};
FSModule* ext2fs = &_ext2fs;

const char name[] = "ext2fs";


static OSError ext2_init(struct _FSModule* module);
static OSError ext2_deinit(struct _FSModule* module);
static OSError ext2_probe(struct _FSModule* module, IODevice* device );
static OSError ext2_mount(struct _FSModule* module,const char *dir, int flags, void *data);

static FSModuleMethods ext2Methods =
{
    ext2_init,
    ext2_deinit,
    ext2_probe,
    ext2_mount
};

OSError EXT2fsInit()
{
    if( _ext2fs.obj.k_name != NULL)
        return OSError_None;
    
    OSError err =  FSModuleInit(&_ext2fs, name);
    
    if( err == OSError_None)
    {
        _ext2fs.methods = &ext2Methods;
    }
    return err;
}

static OSError ext2_init(struct _FSModule* module)
{
    return OSError_None;
}

static OSError ext2_deinit(struct _FSModule* module)
{
    return OSError_None;
}

static OSError ext2_mount(struct _FSModule* module,const char *dir, int flags, void *data)
{
    IODevice* dev = data;
    printf("EXT2 : mount '%s' to '%s\n" , dev->base.k_name , dir);
    return OSError_None;
}

static OSError ext2_probe(struct _FSModule* module, IODevice* device )
{
    printf("EXT2FS probing '%s'\n" , device->base.k_name);
    return OSError_None;
}
