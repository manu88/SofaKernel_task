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


static struct kset * _root = NULL;
static int startsWith(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre),
    lenstr = strlen(str);
    
    return lenstr < lenpre ? 0 : strncmp(pre, str, lenpre) == 0;
}

static int execCmd( const char* cmd);

OSError ShellRun( IODevice* comDev ,struct kset *root)
{
    _root = root;
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


static int execCmd( const char* cmd)
{
    ALWAYS_ASSERT(_root);
    if( startsWith("ls", cmd))
    {
        const char* arg = cmd + strlen("ls ");
        
        if (!arg)
        {
            kobject_printTree((struct kobject*) _root);
        }
    }
    else
    {
        printf("unknown command '%s'\n" , cmd);
        return -EINVAL;
    }
    return 0;
}
