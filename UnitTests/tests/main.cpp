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

    REQUIRE( kobjectIsKindOf(&obj, KObjectClass));
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
    
    REQUIRE( kobjectIsKindOf(&set.obj, KSetClass));
    REQUIRE(set.obj._class == KSetClass);
    REQUIRE(set.obj._parent == NULL);
    REQUIRE(set.obj.isSet   == 1);
    REQUIRE(set.obj.k_name == NULL);
    REQUIRE(set.obj.methods.release != NULL);
    
    REQUIRE( kset_getChildByName(&set, "LOL") == NULL);
    REQUIRE( kset_count(&set) == 0);

    struct kobject obj;
    kobject_init(&obj);
    obj.k_name = "MyName";
    
    REQUIRE( kset_append(&set, &obj) == OSError_None);
    REQUIRE( kset_count(&set) == 1);
    REQUIRE( obj.kref.refcount == 2);
    REQUIRE( obj._parent == (struct kobject*) &set);
    
    REQUIRE( kset_getChildByName(&set, "MyName") == &obj);
    
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

TEST_CASE("KSet obj in multiple sets")
{
    struct kobject child1;
    kobject_init(&child1);
    
    REQUIRE( child1.kref.refcount == 1);
    
    struct kset set1;
    kset_init(&set1);
    REQUIRE( set1.obj.kref.refcount == 1);
    
    struct kset set2;
    kset_init(&set2);
    REQUIRE( set2.obj.kref.refcount == 1);
    
    REQUIRE(kset_append(&set1, &child1) == OSError_None);
    REQUIRE( child1.kref.refcount == 2);
    
    REQUIRE(kset_append(&set2, &child1) == OSError_None);
    REQUIRE( child1.kref.refcount == 3);
    
    REQUIRE(kset_contains(&set1, &child1));
    REQUIRE(kset_contains(&set2, &child1));
    
    
    kset_remove(&set1, &child1);
    REQUIRE( child1.kref.refcount == 2);
    REQUIRE(kset_contains(&set1, &child1) == 0);
    REQUIRE(kset_contains(&set2, &child1));
    
    kobject_put(&child1);
    REQUIRE(kset_contains(&set2, &child1));
    REQUIRE( child1.kref.refcount == 1);
    
    kset_remove(&set2, &child1);
    REQUIRE( child1.kref.refcount == 0);
}
