//
//  IONodeTests.cpp
//  UnitTests
//
//  Created by admingie on 01/07/2019.
//  Copyright © 2019 admingie. All rights reserved.
//


#include "catch.hpp"
#include "DriverKit/IONode.h"

TEST_CASE("IONode Base")
{
    IONode node;
    const char name[] = "Hello";
    REQUIRE(IONodeInit(&node, name) == OSError_None);
 
    REQUIRE( kobjectIsKindOf((const struct kobject *)&node, IONodeClass));
    REQUIRE( IONodeGetType(&node) == IONodeType_Unknown);
    REQUIRE( strcmp( IONodeGetName(&node), name) == 0);
    
    // no child , no for each
    struct kobject *o = nullptr;
    IONodeForEach( &node , o)
    {
        REQUIRE(false);
    }
}

static const char someDats[] = "someDats";
TEST_CASE("IONode Attributes")
{
    IONode node;
    const char name[] = "Hello";
    REQUIRE(IONodeInit(&node, name) == OSError_None);
    
    // we got OSError_Unimplemented because an IONode has no default handler for attributes
    IOData data;
    REQUIRE(IONodeGetAttribute(&node, "LOLZ", &data) == OSError_Unimplemented);
    
    // now we set a proper handler
    node.GetAttr = [](const struct _IONode* , const char*name , IOData *data) -> OSError
    {
        if (strcmp(name, "BAR") == 0)
        {
            data->type = IODataType_Numeric;
            data->data.val  = 0xAB42;
            return OSError_None;
        }
        else if (strcmp(name, "FOO") == 0)
        {
            data->type = IODataType_String;
            data->data.str = someDats;
            return OSError_None;
        }
        
        return OSError_ArgError;
    };
    
    
    REQUIRE(IONodeGetAttribute(&node, "LOLZ", &data) == OSError_ArgError);
    
    REQUIRE(IONodeGetAttribute(&node, "BAR", &data) == OSError_None);
    REQUIRE(data.type == IODataType_Numeric);
    REQUIRE(data.data.val == 0xAB42);
    
    
    REQUIRE(IONodeGetAttribute(&node, "FOO", &data) == OSError_None);
    REQUIRE(data.type == IODataType_String);
    REQUIRE( strcmp( data.data.str  , someDats) == 0);
}
