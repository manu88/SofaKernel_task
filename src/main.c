
#include <string.h>
#include "Sofa.h"
#include "Bootstrap.h"

#include "DriverKit/DriverKit.h"
#include "Drivers/PCIDriver.h"


#ifndef SOFA_TESTS_ONLY
#include <platsupport/plat/acpi/acpi.h>
#include <sel4platsupport/io.h>
#else
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

static uint8_t* readAndFillBuffer(const char* fromFile , size_t* bufSize)
{
    int fd = open(fromFile, O_RDONLY);
    
    uint8_t c = 0;
    
    uint8_t* ptr = NULL;
    
    size_t size = 0;
    
    while(  read(fd, &c, 1) >0)
    {
        uint8_t* lastP = ptr;
        ptr =  realloc(ptr, ++size);
        
        if (!ptr)
        {
            free(lastP);
            *bufSize = 0;
            return NULL;
        }
        ptr[size-1] = c;
        
    }
    
    *bufSize = size;
    
    return ptr;
}

#endif //#ifndef SOFA_TESTS_ONLY

/*
 Get ACPI
 Init DriverKit
 Creates DeviceTree
 
 */
static OSError earlySystemInit(KernelTaskContext *ctx);

/*
 Drivers loading + matching/probing
 */
static OSError baseSystemInit(KernelTaskContext *ctx);


static void lateSystemInit(KernelTaskContext *ctx);


/* ****** */
static struct kset root = {0};
static const char rootNodeName[] = "root";
/* ****** */
// globals to remove at some point

PCIDriver _pciDriver;

/* ****** */
int main(void)
{
    KernelTaskContext ctx = {0};
    memset(&ctx , 0 , sizeof(KernelTaskContext) );
    
#ifndef SOFA_TESTS_ONLY
    zf_log_set_tag_prefix("kernel_task");
    ctx.info = platsupport_get_bootinfo();
    ZF_LOGF_IF(ctx.info == NULL, "Failed to get bootinfo.");
#endif
    
    int err = bootstrapSystem(&ctx);

    if( err != 0)
        return 0;
    
    kprintf("Start SOFA\n");
    ALWAYS_ASSERT_NO_ERR( earlySystemInit(&ctx) );
    ALWAYS_ASSERT_NO_ERR( baseSystemInit(&ctx)  );
    lateSystemInit(&ctx);
    
    return 0;
}


static OSError earlySystemInit(KernelTaskContext *context)
{
    uint8_t *acpiBuffer = NULL;
    size_t acpiBufferSize = 0;
#ifdef SOFA_TESTS_ONLY
    acpiBuffer = readAndFillBuffer("out.aml"/*qemu-dsdt.aml"*/, &acpiBufferSize);
#else
    ps_io_mapper_t io_mapper;
    int error =  sel4platsupport_new_io_mapper(context->vspace, context->vka, &io_mapper);
    ALWAYS_ASSERT(error == 0);
    
    acpi_t* acpi = acpi_init(io_mapper);
    
    ALWAYS_ASSERT(acpi != NULL);
    
    acpi_dsdt_t* dsdt = acpi_find_region(acpi, ACPI_DSDT);
    ALWAYS_ASSERT(dsdt != NULL);
    
    acpiBuffer = dsdt->definition_block;
    acpiBufferSize = dsdt->header.length;
#endif
    
    ALWAYS_ASSERT(acpiBuffer);
    kprintf("Start Parsing ACPI table\n");
    
    kset_init(&root);
    root.obj.k_name = rootNodeName;
    //ALWAYS_ASSERT(IONodeInit(&root, IONodeType_Node, "DeviceTree") == OSError_None);
    ALWAYS_ASSERT_NO_ERR(DriverKitInit(&root, acpiBuffer, acpiBufferSize) );
    
    
    return OSError_None;
}

static OSError baseSystemInit(KernelTaskContext *context)
{

    ALWAYS_ASSERT_NO_ERR( PCIDriverInit(&_pciDriver) );
    ALWAYS_ASSERT_NO_ERR(DriverKitRegisterDriver( (IODriverBase*)&_pciDriver) );
    kobject_put((struct kobject *)&_pciDriver);
    
    
    
    
    ALWAYS_ASSERT_NO_ERR( DriverKitDoMatching() );
    
    return OSError_None;
}

static void lateSystemInit(KernelTaskContext *ctx)
{
    kobject_printTree( (const struct kobject *) &root);
    //kobject_printTree(&root);
    //DriverKitDump();
}
