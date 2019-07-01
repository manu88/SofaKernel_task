//
//  IODeviceTests.cpp
//  UnitTests
//
//  Created by admingie on 01/07/2019.
//  Copyright © 2019 admingie. All rights reserved.
//

#include "catch.hpp"
#include "DriverKit/IODevice.h"

TEST_CASE("IODevice Base")
{
    IODevice dev;
    IONode node;
    
    REQUIRE(IONodeInit(&node, "TheNode") == OSError_None);
    
    REQUIRE(IODeviceInit(&dev, &node, "TheDevice") == OSError_None);
    
    REQUIRE( kobjectIsKindOf((const struct kobject *)&dev, IODeviceClass));
    
    uint8_t buf[128];
    REQUIRE(IODeviceRead(&dev , buf, 128) == OSError_Unimplemented);
    REQUIRE(IODeviceWrite(&dev, buf, 128) == OSError_Unimplemented);
    
    IODeviceCallbacks methods;
    methods.read = [](IODevice* dev, uint8_t* toBuf,  size_t maxBufSize  ) -> ssize_t
    {
        toBuf[0] = 100;
        toBuf[maxBufSize-1] = 100;
        return maxBufSize;
    };
    
    methods.write = [](IODevice* dev, const uint8_t* buf , size_t bufSize  ) -> ssize_t
    {
        return bufSize;
    };
    
    dev.methods = &methods;
    memset(buf, 0, 128);
    REQUIRE(IODeviceRead(&dev , buf, 128) == 128);
    REQUIRE( buf[0] == 100 );
    REQUIRE( buf[1] == 0 );
    REQUIRE( buf[126] == 0 );
    REQUIRE( buf[127] == 100 );
    REQUIRE(IODeviceWrite(&dev, buf, 128) == 128);
    
}
