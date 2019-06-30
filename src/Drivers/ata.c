//
//  ata.c
//  kernel_taskSofaV2
//
//  Created by Manuel Deneu on 30/06/2019.
//  Copyright © 2019 Manuel Deneu. All rights reserved.
//

#include "ata.h"
#include "../Bootstrap.h"

#define io_in8(ops , adr, res)   ps_io_port_in(ops , adr , 1 ,(uint32_t*) res)
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
        default:              return "Unkown";
    }
}

static void ata_wait(KernelTaskContext* ctx, ATADrive *drive)
{
    printf("ata_wait \n");
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
    printf("Start Soft reset \n");
    io_out8(&ctx->opsIO.io_port_ops, drive->control,  ATA_CMD_RESET);
    
    //io_out8(&drive->ctrl, 0, ATA_CMD_RESET);
    ata_wait(ctx,drive);
    io_out8(&ctx->opsIO.io_port_ops, drive->control, 0);
    
    printf("End Soft reset \n");
}

static int ata_poll(KernelTaskContext* ctx, ATADrive *drive, int advanced_check)
{
    printf("start ata_poll \n");
    ata_wait(ctx , drive);
    
    uint32_t s;
    
    do {
        io_in8(&ctx->opsIO.io_port_ops, drive->base + ATA_REG_STATUS, &s);
    } while (s & ATA_STATUS_BSY);
    
    if (advanced_check)
    {
        if ((s & ATA_STATUS_ERR) || (s & ATA_STATUS_DF))
        {
            ata_wait(ctx, drive);
            uint8_t err;
            io_in8(&ctx->opsIO.io_port_ops, drive->base + ATA_REG_ERROR , &err);
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
    printf("start ata_detect_drive \n");
    ata_select_drive(ctx , drive, 0);
    
    ata_poll(ctx,drive, 0);
    uint32_t status;
    io_in8(&ctx->opsIO.io_port_ops,drive->base +  ATA_REG_STATUS  , &status);
    
    if (!status)
    {
        /* No Device, bail */
        drive->type = ATADEV_NOTFOUND;
        printf("ata_detect_drive : No device found\n");
        //goto done;
    }
    
    return drive->type;
}

void ata_disable_IRQ(KernelTaskContext* ctx, ATADrive *drive)
{
    printf("start ata_disable_IRQ \n");
    io_out8(&ctx->opsIO.io_port_ops, drive->base + ATA_REG_CONTROL, 0x2);
}

void ata_select_drive(KernelTaskContext* ctx, ATADrive *drive, uint32_t mode)
{
    printf("start ata_select_drive \n");
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
