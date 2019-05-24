//
//  IODriverBase.c
//  kernel_taskSofaV2
//
//  Created by Manuel Deneu on 22/05/2019.
//  Copyright © 2019 Manuel Deneu. All rights reserved.
//

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
