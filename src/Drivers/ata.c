//
//  ata.c
//  kernel_taskSofaV2
//
//  Created by Manuel Deneu on 30/06/2019.
//  Copyright © 2019 Manuel Deneu. All rights reserved.
//

// implementation borrowed from the great Mohamed Anwar :  https://github.com/mohamed-anwar/Aquila

#include <string.h>
#include "ata.h"
#include "../Bootstrap.h"

#define io_in8(ops , adr, res)   ps_io_port_in(ops , adr , 1 ,(uint32_t*) res)
#define io_in16(ops , adr, res)   ps_io_port_in(ops , adr , 2 ,(uint32_t*) res)

#define io_out8(ops , adr , val) ps_io_port_out(ops , adr , 1 , val)

static const char *ata_error_string(uint8_t err)
{
    switch (err)
    {
        case ATA_ERROR_BBK:   return "Bad block";
        case ATA_ERROR_UNC:   return "Uncorrectable data";
        case ATA_ERROR_MC:    return "Media changed";
        case ATA_ERROR_IDNF:  return "ID mark not found";
        case ATA_ERROR_MCR:   return "Media change request";
        case ATA_ERROR_ABRT:  return "Command aborted";
        case ATA_ERROR_TK0NF: return "Track 0 not found";
        case ATA_ERROR_AMNF:  return "No address mark";
        default:              return "Unknown";
    }
}

static void ata_wait(KernelTaskContext* ctx, ATADrive *drive)
{
    //printf("ata_wait \n");
    for (int i = 0; i < 4; ++i)
    {
        uint32_t result = 0;
        int err = io_in8(&ctx->opsIO.io_port_ops, drive->base + ATA_REG_ALTSTATUS,  &result);
        
        if( err != 0)
        {
            printf("Erreur ps_io_port_in %i\n" , err);
        }
        //io_in8(&drive->base, ATA_REG_ALTSTATUS);
    }
}


void ata_soft_reset( KernelTaskContext* ctx, ATADrive *drive)
{
    //printf("Start Soft reset \n");
    io_out8(&ctx->opsIO.io_port_ops, drive->control,  ATA_CMD_RESET);
    
    //io_out8(&drive->ctrl, 0, ATA_CMD_RESET);
    ata_wait(ctx,drive);
    io_out8(&ctx->opsIO.io_port_ops, drive->control, 0);
    
    //printf("End Soft reset \n");
}

static int ata_poll(KernelTaskContext* ctx, ATADrive *drive, int advanced_check)
{
    //printf("start ata_poll \n");
    ata_wait(ctx , drive);
    
    uint32_t s;
    
    do
    {
        int ret = io_in8(&ctx->opsIO.io_port_ops, drive->base + ATA_REG_STATUS, &s);
        assert(!ret);
    } while (s & ATA_STATUS_BSY);
    
    if (advanced_check)
    {
        if ((s & ATA_STATUS_ERR) || (s & ATA_STATUS_DF))
        {
            ata_wait(ctx, drive);
            uint32_t err;
            int ret = io_in8(&ctx->opsIO.io_port_ops, drive->base + ATA_REG_ERROR , &err);
            assert(!ret);
            printf("ata: drive error: %s\n", ata_error_string(err));
            return -err;
        }
        
        if (!(s & ATA_STATUS_DRQ))
        {
            printf("ata: drive error: DRQ not set\n");
            return -1;
        }
    }
    
    return 0;
}

uint8_t ata_detect_drive(KernelTaskContext* ctx, ATADrive *drive)
{
    //printf("start ata_detect_drive \n");
    ata_select_drive(ctx , drive, 0);
    
    ata_poll(ctx,drive, 0);
    uint32_t status;
    int ret = io_in8(&ctx->opsIO.io_port_ops,drive->base +  ATA_REG_STATUS  , &status);
    assert(!ret);
    
    if (!status)
    {
        /* No Device, bail */
        drive->type = ATADEV_NOTFOUND;
        //printf("ata_detect_drive : No device found\n");
        //goto done;
    }
    
    uint32_t l = 127;
    ret = io_in8(&ctx->opsIO.io_port_ops,drive->base +ATA_REG_LBA1, &l);
    assert(!ret);
    
    uint32_t h = 127;
    ret = io_in8(&ctx->opsIO.io_port_ops,drive->base +ATA_REG_LBA2, &h);
    assert(!ret);
    
    uint8_t  type_low  = (uint8_t)l;//  io_in8(&drive->base, ATA_REG_LBA1);
    uint8_t  type_high = (uint8_t)h;//io_in8(&drive->base, ATA_REG_LBA2);
    uint16_t type      = (type_high << 8) | type_low;
    
    switch (type)
    {
        case 0x0000:
            type = ATADEV_PATA;
            break;
        case 0xEB14:
            type = ATADEV_PATAPI;
            break;
        case 0x9669:
            type = ATADEV_SATAPI;
            break;
        case 0xC33C:
            type = ATADEV_SATA;
            break;
        default:
            type = ATADEV_UNKOWN;
            break;
    }
    
    printf("ata_detect_drive : Drive %i is %x\n" , drive->id , type);
    
    drive->type = type;
    if (type == ATADEV_UNKOWN)
    {
        return ATADEV_UNKOWN;
    }
    
    if (type == ATADEV_PATAPI || type == ATADEV_SATAPI)
    {
        /* TODO */
        assert(0 && "ATADEV_PATAPI ATADEV_SATAPI to implement :)");
    }
    else
    {
        char ide_ident[512];
        memset(ide_ident, 0, sizeof(ide_ident));
        
        io_out8(&ctx->opsIO.io_port_ops,drive->base + ATA_REG_CMD, ATA_CMD_IDENTIFY);
        ata_poll(ctx , drive, 1);
        
        for (int i = 0; i < 512; i += 2)
        {
            uint16_t x = 0;
            ret =  io_in16(&ctx->opsIO.io_port_ops,drive->base+ ATA_REG_DATA , &x);
            ide_ident[i+0] = x & 0xFFFF;
            ide_ident[i+1] = (x >> 8) & 0xFFFF;
        }
        
        drive->signature    = *(uint16_t *) (ide_ident + ATA_IDENT_DEVICETYPE);
        drive->capabilities = *(uint16_t *) (ide_ident + ATA_IDENT_CAPABILITIES);
        drive->command_sets = *(uint32_t *) (ide_ident + ATA_IDENT_COMMANDSETS);
        
        if (drive->command_sets & (1 << 26)) {
            /* 48-bit LBA */
            uint32_t high  = *(uint32_t *) (ide_ident + ATA_IDENT_MAX_LBA);
            uint32_t low   = *(uint32_t *) (ide_ident + ATA_IDENT_MAX_LBA_EXT);
            
            drive->max_lba = ((uint64_t) high << 32) | low;
            drive->mode    = ATA_MODE_LBA48;
        } else {
            /* 28-bit LBA */
            drive->max_lba = *(uint32_t *) (ide_ident + ATA_IDENT_MAX_LBA);
            drive->mode    = ATA_MODE_LBA28;
        }
        
        for (int i = 0; i < 40; i += 2) {
            drive->model[i + 0] = ide_ident[ATA_IDENT_MODEL + i + 1];
            drive->model[i + 1] = ide_ident[ATA_IDENT_MODEL + i + 0];
        }
        drive->model[40] = 0;
        
        printf("-----------\n");
        printf("ata%d: Signature: 0x%x\n", drive->id, drive->signature);
        printf("ata%d: Capabilities: 0x%x\n", drive->id, drive->capabilities);
        printf("ata%d: Command Sets: 0x%x\n", drive->id, drive->command_sets);
        printf("ata%d: Max LBA: 0x%x\n", drive->id, drive->max_lba);
        printf("ata%d: Model: %s\n", drive->id, drive->model);
        printf("-----------\n");
    }
    
    return drive->type;
}

void ata_disable_IRQ(KernelTaskContext* ctx, ATADrive *drive)
{
    //printf("start ata_disable_IRQ \n");
    io_out8(&ctx->opsIO.io_port_ops, drive->base + ATA_REG_CONTROL, 0x2);
}

void ata_select_drive(KernelTaskContext* ctx, ATADrive *drive, uint32_t mode)
{
    //printf("start ata_select_drive \n");
    static ATADrive *last_selected_drive = NULL;
    static uint32_t last_selected_mode = -1;
    
    if (drive != last_selected_drive || mode != last_selected_mode)
    {
        if (mode & ATA_MODE_LBA48)
        {
            //io_out8(&drive->base, ATA_REG_HDDEVSEL, ATA_DRIVE_LBA48 | (drive->slave << 4) | (mode & 0xF));
            io_out8(&ctx->opsIO.io_port_ops,drive->base + ATA_REG_HDDEVSEL, 0xE0 | (drive->isSlave << 4) | (mode & 0xF));
        }
        else
        {
            io_out8(&ctx->opsIO.io_port_ops,drive->base + ATA_REG_HDDEVSEL, 0xA0 | (drive->isSlave << 4) | (mode & 0xF));
        }
        
        ata_wait(ctx,drive);
        ata_wait(ctx,drive);
        ata_wait(ctx,drive);
        ata_wait(ctx,drive);
        ata_wait(ctx,drive);
        
        last_selected_drive = drive;
        last_selected_mode  = mode;
    }
}
