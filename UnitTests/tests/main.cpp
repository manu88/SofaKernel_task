//
//  main.cpp
//  UnitTests
//
//  Created by admingie on 01/07/2019.
//  Copyright © 2019 admingie. All rights reserved.
//

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "KObject/KObject.h"



TEST_CASE("KObject Base")
{
    struct kobject obj;

    kobject_init(&obj);

    REQUIRE(obj._class == KObjectClass);
    REQUIRE(obj._parent == NULL);
    REQUIRE(obj.isSet   == 0);
    REQUIRE(obj.k_name == NULL);
    REQUIRE(obj.methods.release != NULL);
    
    obj.methods.release = [](struct kobject * o)
    {
        // we MUST be called
        REQUIRE( true);
    };
    
    
    REQUIRE( obj.kref.refcount == 1);
    REQUIRE(kobject_get(&obj) == &obj);
    REQUIRE( obj.kref.refcount == 2);
    
    kobject_put(&obj);
    REQUIRE( obj.kref.refcount == 1);
    
    kobject_put(&obj);
    REQUIRE( obj.kref.refcount == 0);
}

TEST_CASE("KSet Base")
{
    struct kset set;
    
    kset_init(&set);
    
    REQUIRE(set.obj._class == KSetClass);
    REQUIRE(set.obj._parent == NULL);
    REQUIRE(set.obj.isSet   == 1);
    REQUIRE(set.obj.k_name == NULL);
    REQUIRE(set.obj.methods.release != NULL);
    
    set.obj.methods.release = [](struct kobject * o)
    {
        // we MUST be called
        REQUIRE( true);
    };
    
    
    REQUIRE( set.obj.kref.refcount == 1);
    REQUIRE(kobject_get(&set.obj) == (struct kobject*) &set);
    REQUIRE( set.obj.kref.refcount == 2);
    
    kobject_put(&set.obj);
    REQUIRE( set.obj.kref.refcount == 1);
    
    kobject_put(&set.obj);
    REQUIRE( set.obj.kref.refcount == 0);
}

