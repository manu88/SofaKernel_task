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

#include "Shell.h"
#include <errno.h>
#include "Thread.h"
#include "SysCalls.h"

static struct kset * _root = NULL;
static Thread* _thread = NULL;

static int startsWith(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre),
    lenstr = strlen(str);
    
    return lenstr < lenpre ? 0 : strncmp(pre, str, lenpre) == 0;
}

static int execCmd( const char* cmd);

OSError ShellRun(Thread* thread, IODevice* comDev ,struct kset *root)
{
    _root = root;
    _thread = thread;
    char cmd[128] = "";
    int cmdPos = 0;
    while(1)
    {
        uint8_t b = 0;
        ssize_t ret =  IODeviceRead(comDev, 0,&b, 1);
        
        if( b == '\n' || b == '\r')
        {
            printf("\n");
            int ret = execCmd(cmd);
            if( ret != 0)
            {
                printf("'%s' returned %i\n" , cmd,ret);
            }
            
            //clean
            memset(cmd, 0, 128);
            cmdPos = 0;
        }
        else
        {
            IODeviceWrite(comDev,0, &b, 1);
            cmd[cmdPos++] = b;
            
            
        }
        
        
    }
    
    return OSError_None;
}

static int DoRead( const char* args)
{
    
    
    
    return 0;
}

static int DoWrite( const char* args)
{
    char deviceName[128] = "";
    char data[128]       = "";
    
    if( sscanf(args , "%s %s" , deviceName , data) == 2)
    {
        IODevice* dev = kobjectResolve(deviceName, _root);
        
        if( dev && kobjectIsKindOf(dev, IODeviceClass))
        {
            ssize_t r =  IODeviceWrite(dev,0, (const uint8_t*) &data, strlen(data));
            if( r < 0)
            {
                return (int) r;
            }
            return 0;
        }
        return -ENODEV;
    }
    
    return -EINVAL;
}

static int DoAttr( const char* args)
{
    
    char deviceName[128] = "";
    char attrName[128]       = "";
    
    if( sscanf(args , "%s %s" , deviceName , attrName) == 2)
    {
        struct kobject * obj = kobjectResolve(deviceName, _root);
        
        if( obj && kobjectIsKindOf(obj, IONodeClass))
        {
            const IONode* node = (const IONode*)obj;
            
            IOData data = {0};
            OSError ret = IONodeGetAttribute(node, attrName, &data);
            if( ret == OSError_None)
            {
                switch (data.type)
                {
                    case IODataType_Invalid:
                        printf("Invalid attribute '%s'\n" , attrName);
                        break;
                    case IODataType_Numeric:
                        printf("attribute '%s' val : %llx\n" , attrName , data.data.val);
                        break;
                    case IODataType_String:
                        printf("attribute '%s' str : '%s'\n" , attrName , data.data.str);
                        break;
                    case IODataType_Pointer:
                        printf("attribute '%s'  ptr : %p\n" , attrName , data.data.ptr);
                        break;
                        
                        
                }
                return 0;
            }
            printf("IONodeGetAttribute returned %i\n" , ret);
            return -EINVAL;
            
        }
        
        return -ENODEV;
        
    }
    
    return -EINVAL;

}

static int DoMount( const char* args)
{
    char deviceName[128] = "";
    char path[128]       = "";
    
    if( sscanf(args , "%s %s" , deviceName , path) == 2)
    {
        return SC_mount(_thread->ipc_ep_cap);
    }
    return -EINVAL;
}

static int execCmd( const char* cmd)
{
    ALWAYS_ASSERT(_root);
    if( startsWith("help", cmd))
    {
        printf("Sofa shell help\n");
        printf("Available commands : \n");
        printf("\tps\n");
        printf("\tsys\n");
        printf("\tkill\n");
        printf("\tattr\n");
        printf("\twrite\n");
        printf("\tread\n");
        printf("\tsleep\n");
        printf("\n");
        
    }
    else if( startsWith("sys ", cmd))
    {
        const char* arg = cmd + strlen("sys ");
        
        if ( strlen(cmd) > 3 &&  arg)
        {
            struct kobject * obj = kobjectResolve(arg, _root);
            if( obj)
            {
                kobject_printTree(obj);
            }
            else
            {
                printf("no such object '%s'\n" , arg);
            }
        }
        else
        {
            kobject_printTree((struct kobject*) _root);
        }
    }
    else if( startsWith("mount ", cmd))
    {
        const char* arg = cmd + strlen("attr ");
        
        if( arg)
        {
            return DoMount(arg);
        }
        return -EINVAL;
    }
    else if( startsWith("ps", cmd))
    {
        struct kobject * threadManager = kobjectResolve("/ThreadManager", _root);
        
        kobject_printTree(threadManager);
        
    }
    else if( startsWith("kill ", cmd))
    {
        const char* arg = cmd + strlen("kill ");
        int which = atoi(arg);
        
        if( which > 0)
        {
            return SC_kill(_thread->ipc_ep_cap, which);
        }
        return -EINVAL;
    }
    else if (startsWith("attr ", cmd))
    {
        const char* arg = cmd + strlen("attr ");
        
        if( arg)
        {
            return DoAttr(arg);
        }
        
        
    }
    else if( startsWith("spawn", cmd))
    {
        return SC_spawn(_thread->ipc_ep_cap);
    }
    else if (startsWith("write ", cmd))
    {
        const char* arg = cmd + strlen("write ");
        if (arg)
        {
            return DoWrite(arg);
        }
    }
    else if (startsWith("read ", cmd))
    {
        const char* arg = cmd + strlen("read ");
        if (arg)
        {
            return DoRead(arg);
        }
    }
    else if( startsWith("vga ", cmd))
    {
        const char* arg = cmd + strlen("vga ");
        
        char buf[128] = "";
        
        int x = -1;
        int y = -1;
        sscanf(arg, "%i %i %s" , &x , &y , buf);
        
        for (int i=0;i<strlen(buf);i++)
        {
            terminal_putentryat(buf[i], 1/*VGA_COLOR_BLUE*/, x+i, y);
        }
    }
    else if( startsWith("sleep ", cmd))
    {
        const char* arg = cmd + strlen("sleep ");
        if( !arg || strlen(arg ) == 0)
        {
            return -EINVAL;
        }
        
        int secs = atoi(arg);
        
        if( secs <= 0)
        {
            return -EINVAL;
        }
        
        //seL4_DebugDumpScheduler();
        
        printf("[Shell] asks to sleep for %i seconds\n" , secs);
        return SC_usleep(_thread->ipc_ep_cap, secs);


    }
    else if( startsWith("fault", cmd))
    {
        float* lol = NULL;
        *lol = 1.0f;
        return 0;
    }
    else
    {
        printf("unknown command '%s'\n" , cmd);
        return -EINVAL;
    }
    return 0;
}
