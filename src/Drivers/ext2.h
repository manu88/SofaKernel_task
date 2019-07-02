//
//  ext2.h
//  kernel_taskSofaV2
//
//  Created by Manuel Deneu on 01/07/2019.
//  Copyright © 2019 Manuel Deneu. All rights reserved.
//

#pragma once

#include "../Sofa.h"

#define EXT2_SIGNATURE  (uint16_t) 0xEF53

struct ext2_optional_features_flags {
    uint8_t preallocate_for_directories : 1;
    uint8_t afs_server_inodes : 1;
    uint8_t has_journaling : 1;     // for EXT 3
    uint8_t inodes_have_extended_attributes : 1;
    uint8_t can_be_resized : 1;
    uint8_t directories_use_hash_index : 1;
} __packed;

struct ext2_required_features_flags {
    uint8_t compressed : 1;
    uint8_t directories_has_types : 1;
    uint8_t needs_to_replay_journal : 1;
    uint8_t uses_journal_device : 1;
} __packed;

struct ext2_read_only_features_flags {
    uint8_t sparse_sb_and_gdt : 1;
    uint8_t uses_64bit_file_size : 1;
    uint8_t directories_contents_are_binary_tree : 1;
} __packed;

// comments from https://wiki.osdev.org/Ext2#Superblock
struct ext2_superblock
{
    uint32_t    inodes_count; // Total number of inodes in file system
    uint32_t    blocks_count; // Total number of blocks in file system
    uint32_t    su_blocks_count; // Number of blocks reserved for superuser (see offset 80)
    uint32_t    unallocated_blocks_count; // Total number of unallocated blocks
    uint32_t    unallocated_inodes_count; // Total number of unallocated inodes
    uint32_t    su_block_number; // Block number of the block containing the superblock
    uint32_t    block_size; // log2 (block size) - 10. (In other words, the number to shift 1,024 to the left by to obtain the block size)
    uint32_t    fragment_size; // log2 (fragment size) - 10. (In other words, the number to shift 1,024 to the left by to obtain the fragment size)
    uint32_t    blocks_per_block_group; // Number of blocks in each block group
    uint32_t    fragments_per_block_group; // Number of fragments in each block group
    uint32_t    inodes_per_block_group; // Number of inodes in each block group
    uint32_t    last_mount_time; // Last mount time (in POSIX time)
    uint32_t    last_written_time; // Last written time (in POSIX time)
    uint16_t    times_mounted; // Number of times the volume has been mounted since its last consistency check (fsck)
    uint16_t    allowed_times_mounted; // Number of mounts allowed before a consistency check (fsck) must be done
    uint16_t    ext2_signature; // Ext2 signature (0xef53), used to help confirm the presence of Ext2 on a volume
    uint16_t    filesystem_state; // File system state : 1 clean - 2 has errors
    uint16_t    on_error; // What to do when an error is detected : 1 ignore and continue - 2 remount as read-only - 3 kernel panic
    uint16_t    minor_version; // Minor portion of version (combine with Major portion below to construct full version field)
    uint32_t    last_fsck; // POSIX time of last consistency check (fsck)
    uint32_t    time_between_fsck;  // Interval (in POSIX time) between forced consistency checks (fsck)
    
    uint32_t    os_id; // Operating system ID from which the filesystem on this volume was created
    /*
     OS IDs:
     0 Linux
     1 GNU Hurd
     2 MASIX
     3 FreeBSD
     4 Other "Lites"
     */
    
    uint32_t    major_version; // Major portion of version (combine with Minor portion above to construct full version field)
    uint16_t    uid; // User ID that can use reserved blocks
    uint16_t    gid; //Group ID that can use reserved blocks
    
    // For extended superblock fields ... which is always assumed
    // These fields are only present if Major version (specified in the base superblock fields), is greater than or equal to 1.
    
    uint32_t    first_non_reserved_inode; // First non-reserved inode in file system. (In versions < 1.0, this is fixed as 11)
    uint16_t    inode_size; // Size of each inode structure in bytes. (In versions < 1.0, this is fixed as 128)
    uint16_t    sb_block_group; // Block group that this superblock is part of (if backup copy)
    uint32_t/*struct ext2_optional_features_flags*/   optional_features; // Optional features present (features that are not required to read or write, but usually result in a performance increase)
    uint32_t/*struct ext2_required_features_flags*/   required_features; //Required features present (features that are required to be supported to read or write)
    uint32_t/*struct ext2_read_only_features_flags*/  read_only_features; //Features that if not supported, the volume must be mounted read-only)
    uint8_t     fsid[16]; // File system ID (what is output by blkid)
    uint8_t     name[16]; // Volume name (C-style string: characters terminated by a 0 byte)
    uint8_t     path_last_mounted_to[64]; // Path volume was last mounted to (C-style string: characters terminated by a 0 byte)
    uint32_t    compression_algorithms; // Compression algorithms used (see Required features above)
    uint8_t     blocks_to_preallocate_for_files; // Number of blocks to preallocate for files
    uint8_t     blocks_to_preallocate_for_directories; // Number of blocks to preallocate for directories
    uint16_t    __unused__;
    uint8_t     jid[16];    // Journal ID (same style as the File system ID above)
    uint32_t    jinode;     // Journal inode
    uint32_t    jdevice;    // Journal device
    uint32_t    head_of_orphan_inode_list;  // Head of orphan inode list
} __packed;


struct ext2_block_group_descriptor
{
    uint32_t    block_usage_bitmap;
    uint32_t    inode_usage_bitmap;
    uint32_t    inode_table;
    uint16_t    unallocated_blocks_count;
    uint16_t    unallocated_inodes_count;
    uint16_t    directories_count;
    uint8_t     __unused__[14];
} __packed;



struct ext2
{
    //struct inode *supernode;
    struct ext2_superblock superblock;
    struct ext2_block_group_descriptor bgd_table;
    size_t bgds_count;
    size_t bs;
    //struct icache *icache;
};
