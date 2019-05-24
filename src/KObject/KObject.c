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

static void KOBJgetInfos( const struct kobject *obj , char outDesc[MAX_DESC_SIZE] );
static void KSETgetInfos( const struct kobject *obj , char outDesc[MAX_DESC_SIZE] );

static const KClass objClass = KClassMake("KObject", KOBJgetInfos);
static const KClass setClass = KClassMake("KSet", KSETgetInfos);

void kref_init(struct kref* k)
{
    k->refcount = 1;
}

void kref_get(struct kref* k)
{
    atomic_fetch_add(&k->refcount, 1);
}

void kref_put(struct kref* k,void (*release)(struct kref *k))
{
    if (atomic_fetch_sub(&k->refcount, 1) == 1)
    {
        if (release)
        {
            release(k);
        }
    }
}




void kobject_init(struct kobject* object)
{
    memset(object, 0, sizeof(struct kobject) );
    kref_init(&object->kref);
    object->class = &objClass;
}


struct kobject *kobject_get(struct kobject *ko)
{
    kref_get(&ko->kref);
    return ko;
}
void kobject_put(struct kobject *ko)
{
    kref_put(&ko->kref, NULL);
    
    if (ko->kref.refcount == 0 && ko->class && ko->class->release)
    {
        ko->class->release(ko);
    }
}

static void KOBJgetInfos(const struct kobject *obj , char outDesc[MAX_DESC_SIZE] )
{
    snprintf(outDesc, MAX_DESC_SIZE, "'%s' Kobject %p (refc %i)" ,obj->k_name, (void*) obj , obj->kref.refcount );
}

static void KSETgetInfos(const struct kobject *obj , char outDesc[MAX_DESC_SIZE] )
{
    struct kset *self = (struct kset *) obj;
    snprintf(outDesc, MAX_DESC_SIZE, "'%s' Kset %p %zi children (refc %i)" ,obj->k_name, (void*) self, kset_count(self) , obj->kref.refcount );
}


void kset_init(struct kset* set)
{
    kobject_init(&set->obj);
    set->obj.class = &setClass;
    set->obj.isSet = 1;
    set->_listHead = NULL;
}

OSError kset_append(struct kset*set , struct kobject* obj)
{

    DL_APPEND(set->_listHead, kobject_get(obj));
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
    kobject_put(obj);
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
        if ( strcmp( obj->k_name  , name) == 0)
        {
            return obj;
        }
    }
    
    return NULL;
}



static void _printOBJ( const struct kobject* obj , int indent)
{
    for(int i =0;i<indent;i++)
        kprintf("|\t");
    
    char desc[MAX_DESC_SIZE] = "";
    
    obj->class->getInfos(obj , desc);
    
    kprintf("%s\n" , desc);
    
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
