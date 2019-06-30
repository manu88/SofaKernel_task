//
//  IONodeAttributes.h
//  kernel_taskSofaV2
//
//  Created by Manuel Deneu on 30/06/2019.
//  Copyright © 2019 Manuel Deneu. All rights reserved.
//

#pragma once
#include "../KObject/uthash.h"

extern const char IONodeAttributePCI[];
extern const char IONodeAttributePCIClass[];
extern const char IONodeAttributePCISubClass[];
extern const char IONodeAttributeHID[];

extern const char IONodeAttributeVendorID[];
extern const char IONodeAttributeDeviceID[];

extern const char IONodeAttributeVendorName[];
extern const char IONodeAttributeDeviceName[];


typedef struct
{
    char id[10];
    int type;
    
    union
    {
        void* ptr;
        uint64_t v;
    } data;
    
    UT_hash_handle hh; /* makes this structure hashable */
    
} IOAttribute;

typedef enum
{
    IODataType_Invalid,
    IODataType_Numeric,
    IODataType_String,
    IODataType_Pointer,
} IODataType;

typedef struct
{
    
    IODataType type;
    
    union
    {
        void* ptr;
        uint64_t val;
        const char* str;
    } data;
    
} IOData;
