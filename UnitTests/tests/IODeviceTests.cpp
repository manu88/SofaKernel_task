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
}
