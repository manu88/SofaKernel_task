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

// implementation borrowed from the great Mohamed Anwar :  https://github.com/mohamed-anwar/Aquila

#pragma once
#include <stdint.h>
#include "../Sofa.h"

/* definitions used in atadev driver */
#define ATADEV_UNKOWN               0x000
#define ATADEV_NOTFOUND             0x001
#define ATADEV_PATA                 0x002
#define ATADEV_SATA                 0x003
#define ATADEV_PATAPI               0x004
#define ATADEV_SATAPI               0x005

/* default ports */
#define ATA_PIO_PRI_PORT_BASE       0x1F0
#define ATA_PIO_PRI_PORT_CONTROL    0x3F6
#define ATA_PIO_SEC_PORT_BASE       0x170
#define ATA_PIO_SEC_PORT_CONTROL    0x376

/* ATA registers */
#define ATA_REG_DATA                0x00
#define ATA_REG_ERROR               0x01
#define ATA_REG_FEATURES            0x01
#define ATA_REG_SECCOUNT0           0x02
#define ATA_REG_LBA0                0x03
#define ATA_REG_LBA1                0x04
#define ATA_REG_LBA2                0x05
#define ATA_REG_HDDEVSEL            0x06
#define ATA_REG_CMD                 0x07
#define ATA_REG_STATUS              0x07
#define ATA_REG_SECCOUNT1           0x08
#define ATA_REG_LBA3                0x09
#define ATA_REG_LBA4                0x0A
#define ATA_REG_LBA5                0x0B
#define ATA_REG_CONTROL             0x0C
#define ATA_REG_ALTSTATUS           0x0C
#define ATA_REG_DEVADDRESS          0x0D

/* ATA commands */
#define ATA_CMD_RESET               0x08
#define ATA_CMD_READ_SECTORS        0x20
#define ATA_CMD_READ_SECTORS_EXT    0x24
#define ATA_CMD_WRITE_SECTORS       0x30
#define ATA_CMD_WRITE_SECTORS_EXT   0x34
#define ATA_CMD_READ_DMA            0xC8
#define ATA_CMD_READ_DMA_EXT        0x25
#define ATA_CMD_WRITE_DMA           0xCA
#define ATA_CMD_WRITE_DMA_EXT       0x35
#define ATA_CMD_CACHE_FLUSH         0xE7
#define ATA_CMD_CACHE_FLUSH_EXT     0xEA
#define ATA_CMD_PACKET              0xA0
#define ATA_CMD_IDENTIFY_PACKET     0xA1
#define ATA_CMD_IDENTIFY            0xEC

#define ATA_DRIVE_LBA48             0x40

#define ATA_STATUS_BSY              0x80    /* Busy */
#define ATA_STATUS_DRDY             0x40    /* Drive ready */
#define ATA_STATUS_DF               0x20    /* Drive write fault */
#define ATA_STATUS_DSC              0x10    /* Drive seek complete */
#define ATA_STATUS_DRQ              0x08    /* Data request ready */
#define ATA_STATUS_CORR             0x04    /* Corrected data */
#define ATA_STATUS_IDX              0x02    /* Inlex */
#define ATA_STATUS_ERR              0x01    /* Error */

#define ATA_ERROR_BBK               0x080   /* Bad block */
#define ATA_ERROR_UNC               0x040   /* Uncorrectable data */
#define ATA_ERROR_MC                0x020   /* Media changed */
#define ATA_ERROR_IDNF              0x010   /* ID mark not found */
#define ATA_ERROR_MCR               0x008   /* Media change request */
#define ATA_ERROR_ABRT              0x004   /* Command aborted */
#define ATA_ERROR_TK0NF             0x002   /* Track 0 not found */
#define ATA_ERROR_AMNF              0x001   /* No address mark */


#define ATA_IDENT_DEVICETYPE        0
#define ATA_IDENT_CYLINDERS         2
#define ATA_IDENT_HEADS             6
#define ATA_IDENT_SECTORS           12
#define ATA_IDENT_SERIAL            20
#define ATA_IDENT_MODEL             54
#define ATA_IDENT_CAPABILITIES      98
#define ATA_IDENT_FIELDVALID        106
#define ATA_IDENT_MAX_LBA           120
#define ATA_IDENT_COMMANDSETS       164
#define ATA_IDENT_MAX_LBA_EXT       200

#define ATA_CAP_LBA                 0x200

#define ATA_MODE_CHS                0x10
#define ATA_MODE_LBA28              0x20
#define ATA_MODE_LBA48              0x40


typedef struct _KernelTaskContext KernelTaskContext;

typedef struct
{
    uint32_t base;
    uint32_t control;
    uint16_t type;
    uint8_t  isSlave;
    uint8_t  id;
    
    uint8_t  mode;
    uint16_t signature;
    uint16_t capabilities;
    uint32_t command_sets;
    uint64_t max_lba;
    char     model[41];
} ATADrive;

void ata_disable_IRQ(KernelTaskContext* ctx, ATADrive *drive) NO_NULL_POINTERS;
void ata_soft_reset( KernelTaskContext* ctx, ATADrive *drive) NO_NULL_POINTERS;
uint8_t ata_detect_drive(KernelTaskContext* ctx, ATADrive *drive) NO_NULL_POINTERS;
void ata_select_drive(KernelTaskContext* ctx, ATADrive *drive, uint32_t mode);


ssize_t ata_read(KernelTaskContext* ctx, ATADrive *drive, uint64_t lba, size_t count, void *buf);
ssize_t ata_write(KernelTaskContext* ctx, ATADrive *drive, uint64_t lba, size_t count, const void *buf);
