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

#include "EXT2fs.h"
#include "../DriverKit/IODevice.h"
#include "../Drivers/ext2.h"

static FSModule _ext2fs = {0};
FSModule* ext2fs = &_ext2fs;

const char name[] = "ext2fs";


static OSError ext2_init(struct _FSModule* module);
static OSError ext2_deinit(struct _FSModule* module);
static OSError ext2_probe(struct _FSModule* module, IODevice* device );
static OSError ext2_mount(struct _FSModule* module,const char *dir, int flags, void *data);

static FSModuleMethods ext2Methods =
{
    ext2_init,
    ext2_deinit,
    ext2_probe,
    ext2_mount
};

OSError EXT2fsInit()
{
    if( _ext2fs.obj.k_name != NULL)
        return OSError_None;
    
    OSError err =  FSModuleInit(&_ext2fs, name);
    
    if( err == OSError_None)
    {
        _ext2fs.methods = &ext2Methods;
    }
    return err;
}

static OSError ext2_init(struct _FSModule* module)
{
    return OSError_None;
}

static OSError ext2_deinit(struct _FSModule* module)
{
    return OSError_None;
}

static OSError ext2_mount(struct _FSModule* module,const char *dir, int flags, void *data)
{
    IODevice* dev = data;
    printf("EXT2 : mount '%s' to '%s\n" , dev->base.k_name , dir);
    return OSError_None;
}

static OSError ext2_probe(struct _FSModule* module, IODevice* device )
{
    printf("EXT2FS probing '%s'\n" , device->base.k_name);
 
    IODeviceCtl(device, IOCTL_Reset, NULL);
    struct ext2_superblock sb = {0};
    

    // The first 1024 bytes of the disk, the "boot block", are reserved
    // for the partition boot sectors and are unused by the Ext2 filesystem
    
    ssize_t ret = IODeviceRead(device, 2, &sb, sizeof(struct ext2_superblock));
    
    printf("ext2_probe  : IODeviceRead returns %zi\n" , ret);
    if( ret < 0)
        return (OSError)ret;
    
    
    
    /* Valid Ext2? */
    if (sb.ext2_signature != EXT2_SIGNATURE)
    {
        printf("Invalid signature 0X%X!\n" , sb.ext2_signature);
        
        return OSError_NotSupportedDevice;
    }
    
    printf("ext2_probe : Signature is matching on drive '%s'\n" , device->base.k_name);
    
    
    printf("inodes_count : %i\n",sb.inodes_count);
    printf("blocks_count : %i\n",sb.blocks_count);
    printf("su_blocks_count : %i\n",sb.su_blocks_count);
    printf("unallocated_blocks_count : %i\n",sb.unallocated_blocks_count);
    printf("unallocated_inodes_count : %i\n",sb.unallocated_inodes_count);
    printf("su_block_number : %i\n",sb.su_block_number);
    printf("block_size : %i\n",sb.block_size);
    printf("fragment_size : %i\n",sb.fragment_size);
    printf("blocks_per_block_group : %i\n",sb.blocks_per_block_group);
    printf("fragments_per_block_group : %i\n",sb.fragments_per_block_group);
    printf("inodes_per_block_group : %i\n",sb.inodes_per_block_group);
    printf("last_mount_time : %i\n",sb.last_mount_time);
    printf("last_written_time : %i\n",sb.last_written_time);
    printf("times_mounted : %i\n",sb.times_mounted);  // after last fsck
    printf("allowed_times_mounted : %i\n",sb.allowed_times_mounted);
    printf("ext2_signature : %i\n",sb.ext2_signature);
    printf("filesystem_state : %i\n",sb.filesystem_state);
    printf("on_error : %i\n",sb.on_error);
    printf("minor_version : %i\n",sb.minor_version);
    printf("last_fsck : %i\n",sb.last_fsck);
    printf("time_between_fsck : %i\n",sb.time_between_fsck);  // Time allowed between fsck(s)
    printf("os_id : %i\n",sb.os_id);
    printf("major_version : %i\n",sb.major_version);
    printf("uid : %i\n",sb.uid);
    printf("gid : %i\n",sb.gid);
    
    
    printf("fsid: ");//
    for(int i=0;i<16;i++)
        printf("%X" , sb.fsid[i]);
    printf("\n");
    //'%s' \n",sb.fsid);
    printf("name: '%s' \n",(const char*)sb.name);
    printf("path_last_mounted_to: '%s' \n",sb.path_last_mounted_to);
    
    size_t bs = 1024UL << sb.block_size;
    printf("\n Block Size : %zi \n" , bs);
    
    struct ext2_block_group_descriptor blockGroupDesc = {0};
    
    ret = IODeviceRead(device, 3 /* * lba(512)*/,&blockGroupDesc,sizeof(struct ext2_block_group_descriptor) );
    
    printf("-- ext2_block_group_descriptor\n ");
    printf("block_usage_bitmap : %u\n",blockGroupDesc.block_usage_bitmap);
    printf("inode_usage_bitmap : %u\n",blockGroupDesc.inode_usage_bitmap);
    printf("inode_table : %u\n",blockGroupDesc.inode_table);
    printf("unallocated_blocks_count : %u\n",blockGroupDesc.unallocated_blocks_count);
    printf("unallocated_inodes_count : %u\n",blockGroupDesc.unallocated_inodes_count);
    printf("directories_count : %u\n",blockGroupDesc.directories_count);
    
    return OSError_None;
}
