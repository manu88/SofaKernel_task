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

static FSModule _ext2fs = {0};
FSModule* ext2fs = &_ext2fs;

const char name[] = "ext2fs";

OSError EXT2fsInit()
{
    if( _ext2fs.obj.k_name != NULL)
        return OSError_None;
    
    return FSModuleInit(&_ext2fs, name);
}
