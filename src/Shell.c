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
        ssize_t ret =  IODeviceRead(comDev, &b, 1);
        
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
            IODeviceWrite(comDev, &b, 1);
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
            ssize_t r =  IODeviceWrite(dev, (const uint8_t*) &data, strlen(data));
            if( r < 0)
            {
                return (int) r;
            }
            return 0;
        }
        return -ENODEV;
    }
    
    return 0;
}

static int execCmd( const char* cmd)
{
    ALWAYS_ASSERT(_root);
    if( startsWith("ls ", cmd))
    {
        const char* arg = cmd + strlen("ls ");
        
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
    else if (startsWith("attr ", cmd))
    {
        const char* arg = cmd + strlen("attr ");
        
        struct kobject * obj = kobjectResolve(arg, _root);
        
        if( obj && kobjectIsKindOf(obj, IONodeClass))
        {
            const IONode* node = (const IONode*)obj;
            
            IOAttribute* tmp = NULL;
            IOAttribute* attr = NULL;
            
            printf("IONode '%s' has %zi attributes:\n", node->base.obj.k_name, IONodeGetAttrCount(node) );
            IOAttributeForEach(node, tmp, attr)
            {
                
                printf("Attribute '%s' Type %i value %llu\n" , attr->id , attr->type , attr->data.v);
            }
        }
        else
        {
            return -EINVAL;
        }
        
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
