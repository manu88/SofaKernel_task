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


#include "VFS.h"
#include "EXT2fs.h"

static struct
{
    struct kset base;

} _FSNode = {0};

const char fsNodeName[] = "VirtualFileSystem";

struct kobject* VFSInit( )
{
    kset_initWithName(&_FSNode.base, fsNodeName);
    
    return (struct kobject*) &_FSNode.base;
}

OSError VFSRegisterFSModule( FSModule* module)
{
    if( kset_contains(&_FSNode.base, (struct kobject *)module))
    {
        return OSError_AlreadyInSet;
    }
    
    if( !module->isLoaded)
    {
        printf("Load module '%s'\n" , module->obj.k_name);
        OSError ret = module->methods->init(module);
        if(  ret != OSError_None)
        {
            return ret;
        }
        module->isLoaded = 1;
    }
    
    OSError ret = kset_append(&_FSNode.base, (struct kobject *)module);
    
    if( ret == OSError_None)
    {
        
    }
    return ret;
}


OSError VFSRegisterEXT2Module()
{
    EXT2fsInit();
    return VFSRegisterFSModule(ext2fs);
}
