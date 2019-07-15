//
//  CPIOfs.c
//  kernel_taskSofaV2
//
//  Created by Manuel Deneu on 15/07/2019.
//  Copyright © 2019 Manuel Deneu. All rights reserved.
//

#include "CPIOfs.h"


static FSModule _cpiofs = {0};
FSModule* cpiofs = &_cpiofs;

static const char name[] = "cpiofs";

OSError CPIOfsInit()
{
    // check if already init
    if( _cpiofs.obj.k_name != NULL)
        return OSError_None;
    
    OSError err =  FSModuleInit(&_cpiofs, name);
    
    if( err == OSError_None)
    {
        //_cpiofs.methods = &cpiofsMethods;
    }
    return err;
}
