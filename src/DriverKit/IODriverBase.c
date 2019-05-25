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

#include "IODriverBase.h"


static OSError _Default_init(IODriverBase *driver  ) NO_NULL_POINTERS;
static OSError _Default_release(IODriverBase *driver  ) NO_NULL_POINTERS;
static OSError _Default_probeDevice(IODriverBase* driver , IONode* node) NO_NULL_POINTERS;

static IODriverCallbacks _defaultMethods =
{
    _Default_init,
    _Default_release,
    _Default_probeDevice,
    NULL // interupt
    
    //OSError (*start)(IODriverBase *driver  );
    //OSError (*stop)(IODriverBase *driver  );
    
};

OSError IODriverBaseInit(IODriverBase* base, const char* name)
{
    kobject_init(&base->base);
    base->base.k_name = name;
    base->driverMethods = &_defaultMethods;
    return OSError_None;
}




static OSError _Default_init(IODriverBase *driver  )
{
    return OSError_None;
}

static OSError _Default_release(IODriverBase *driver  )
{
    return OSError_None;
}

static OSError _Default_probeDevice(IODriverBase* driver , IONode* node)
{
    return OSError_None;
}
