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

FSModuleMethods defaultMethods =
{
    DEFAULT_init,
    DEFAULT_deinit
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
    return OSError_None;
}

static OSError DEFAULT_deinit(struct _FSModule* module)
{
    return OSError_None;
}
