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
#include "FSModule.h"


static OSError DEFAULT_init(struct _FSModule* module);
static OSError DEFAULT_deinit(struct _FSModule* module);
static OSError DEFAULT_probe(struct _FSModule* module, IODevice* device );
static OSError DEFAULT_mount(struct _FSModule* module,const char *dir, int flags, void *data);

static FSModuleMethods defaultMethods =
{
    DEFAULT_init,
    DEFAULT_deinit,
    DEFAULT_probe,
    DEFAULT_mount
};

OSError FSModuleInit(FSModule* module, const char* name)
{
    memset(module, 0, sizeof( FSModule));
    kobject_init(&module->obj);
    module->obj.k_name = name;
    module->methods = &defaultMethods;
    return OSError_None;
}


static OSError DEFAULT_init(struct _FSModule* module)
{
    UNUSED_PARAMETER(module);
    return OSError_None;
}

static OSError DEFAULT_deinit(struct _FSModule* module)
{
    UNUSED_PARAMETER(module);
    return OSError_None;
}
static OSError DEFAULT_probe(struct _FSModule* module, IODevice* device )
{
    return OSError_Unimplemented;
}


static OSError DEFAULT_mount(struct _FSModule* module,const char *dir, int flags, void *data)
{
    UNUSED_PARAMETER(module);
    UNUSED_PARAMETER(dir);
    UNUSED_PARAMETER(flags);
    UNUSED_PARAMETER(data);
    return OSError_Unimplemented;
}
