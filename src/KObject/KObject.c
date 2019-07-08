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

#include "KObject.h"
#include <string.h>
#include <stdlib.h>

static void KobjectDefaultRelease(struct kobject *);

static KObjectMethods _defaultOBJMethods =
{
    KobjectDefaultRelease,
};

static void KOBJgetInfos( const struct kobject *obj , char outDesc[MAX_DESC_SIZE] );
static void KSETgetInfos( const struct kobject *obj , char outDesc[MAX_DESC_SIZE] );

static const KClass objClass =
KClassMake(
           "KObject",
           KOBJgetInfos,
           NULL // release
           );

const KClass* KObjectClass = &objClass;


static const KClass setClass =
KClassMake(
           "KSet",
           KSETgetInfos,
           NULL // release
           );

const KClass* KSetClass = &setClass;

void kref_init(struct kref* k)
{
    k->refcount = 1;
}

void kref_get(struct kref* k)
{
    atomic_fetch_add(&k->refcount, 1);
}

void kref_put(struct kref* k)
{
    atomic_fetch_sub(&k->refcount, 1);

}




void kobject_init(struct kobject* object)
{
    memset(object, 0, sizeof(struct kobject) );
    kref_init(&object->kref);
    object->_class = &objClass;
    object->methods = _defaultOBJMethods;
}

void kobject_initWithName(struct kobject* object, const char*name)
{
    kobject_init(object);
    object->k_name = name;
}


struct kobject *kobject_get(struct kobject *ko)
{
    kref_get(&ko->kref);
    return ko;
}
void kobject_put(struct kobject *ko)
{
    kref_put(&ko->kref);
    
    if (ko->kref.refcount == 0)
    {
        ko->methods.release(ko);
        
        /*
        if( ko->class && ko->class->release)
        {
            ko->class->release(ko);
        }
        */
    }
}

static void KOBJgetInfos(const struct kobject *obj , char outDesc[MAX_DESC_SIZE] )
{
    //snprintf(outDesc, MAX_DESC_SIZE, "(refc %i)" ,  obj->kref.refcount );
}

static void KSETgetInfos(const struct kobject *obj , char outDesc[MAX_DESC_SIZE] )
{
    struct kset *self = (struct kset *) obj;
    snprintf(outDesc, MAX_DESC_SIZE, "%zi children ",   kset_count(self)  );
}


void kset_init(struct kset* set)
{
    kobject_init(&set->obj);
    set->obj._class = &setClass;
    set->obj.isSet = 1;
    set->_listHead = NULL;
}

void kset_initWithName(struct kset* set , const char* name)
{
    kset_init(set);
    set->obj.k_name = name;
}

OSError kset_append(struct kset*set , struct kobject* obj)
{
    DL_APPEND(set->_listHead, obj);
    kobject_get(obj); // inc ref count
    obj->_parent = &set->obj;

    return OSError_None;
}

static int PtrCmp(struct kobject *obj1 , struct kobject *obj2)
{
    return !(obj1 == obj2);
}

int kset_contains(struct kset* set , struct kobject *obj)
{
    // (head,out,elt,cmp)
    struct kobject* out = NULL;
    DL_SEARCH(set->_listHead , out , out , PtrCmp);
    return out != NULL;
}

OSError kset_remove(struct kset*set , struct kobject* obj)
{
    DL_DELETE(set->_listHead, obj);
    obj->_parent = NULL;
    kobject_put(obj); // dec ref count
    return OSError_None;
}

size_t  kset_count(const struct kset* set)
{
    size_t c = 0;
    struct kobject* iter = NULL;
    DL_COUNT(set->_listHead, iter, c);
    
    
    
    return c;
}


struct kobject* kset_getChildByName( const struct kset* set , const char* name )
{
    struct kobject* obj = NULL;
    kset_foreach( set, obj)
    {
        if ( obj &&  strcmp( obj->k_name  , name) == 0)
        {
            return obj;
        }
    }
    
    return NULL;
}



static void _printOBJ( const struct kobject* obj , int indent)
{
    ALWAYS_ASSERT(obj);
    for(int i =0;i<indent;i++)
        kprintf("|\t");
    
    char desc[MAX_DESC_SIZE] = "";
    
    ALWAYS_ASSERT(obj->_class);
    obj->_class->getInfos(obj , desc);
    
    kprintf("'%s' %s %p refc %i %s\n" , obj->k_name, obj->_class->name, (const void*)obj, obj->kref.refcount, desc);
    
    if (kobject_isSet(obj))
    {
        const struct kset* set = (const struct kset* ) obj;
        
        struct kobject* child = NULL;
        kset_foreach(set, child)
        {
            _printOBJ(child, indent + 1);
        }
    }
}


void kobject_printTree( const struct kobject* obj)
{
    _printOBJ(obj, 0);
}


struct kobject* kobjectResolve( const char* path_ , struct kset* startNode )
{
    if (strlen(path_) == 0)
        return NULL;
    
    
    char* path = strdup(path_);
    ALWAYS_ASSERT(path);
    static const char delim[] = "/";
    
    char* token = strtok(path, delim);
    
    struct kset *ret = startNode;
    
    while ( token != NULL )
    {
        if (kobject_isSet((const struct kobject *)ret))
            ret = (struct kset *) kset_getChildByName(ret, token);// InodeGetChildByName(ret ,token);
        else
            ret = NULL;

        if(!ret)
        {
            free(path);
            return NULL;
        }
        token = strtok(NULL, delim);
        
    }
    /*
     char *last = strrchr(path, '/');
     if (last)
     {
     printf("Got a last '%s' \n" , last);
     }
     */
    
    free(path);
    return  (struct kobject*)ret;
}


static void KobjectDefaultRelease(struct kobject *obj)
{
    UNUSED_PARAMETER(obj);
}
