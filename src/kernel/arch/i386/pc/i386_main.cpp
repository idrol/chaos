#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "grub/multiboot.h"
#include "i386_hal.h"
#include "i386_gdt.h"
#include "i386_interrupts.h"
#include "i386_pic.h"
#include "i386_exception.h"
#include "i386_vga.h"
#include "i386_memory.h"
#include "i386_tasking.h"
#include <memory.h>
#include <stdio.h>
#include <panic.h>
#include <drivers/blockio.h>
#include <drivers/device.h>
#include <drivers/fat.h>
#include <drivers/fs.h>
#include <drivers/interrupts.h>
#include <drivers/vfs.h>
#include <drivers/io.h>
#include <drivers/ata.h>
#include <drivers/ps2_kb.h>

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "Attempting to compile i386 architecture without a ix86-efl compatible compiled"
#endif

extern "C" uint32_t kernel_start;
extern "C" uint32_t kernel_end;

extern "C" void kernel_main();

extern "C" void kernel_i386_main(multiboot_info_t *mbd, uint32_t magic, uint32_t* boot_page_directory, uint32_t stack_top, uint32_t stack_bottom)
{
    i386_vga_init();

    printf("Kernel start 0x%lX\nKernel end 0x%lX\n", (uint32_t)&kernel_start, (uint32_t)&kernel_end);

    io_init(
        &i386_hal_io_halt,
        &i386_hal_io_wait,
        &i386_hal_io_outb,
        &i386_hal_io_inb,
        &i386_hal_io_outw,
        &i386_hal_io_inw,
        &i386_hal_io_outl,
        &i386_hal_io_inl);

    i386_hal_disable_interrupts();

    i386_gdt_init();
    i386_interrupts_init();
    i386_pic_init(32);
    i386_exception_init();

    i386_hal_enable_interrupts();

    i386_memory_init(mbd, (uint32_t)&kernel_start, (uint32_t)&kernel_end, boot_page_directory);
    i386_tasking_init();

    i386_vga_init_memory();

    thread_init();
    block_init();
    device_init();
    fs_init();
    fat_init();
    vfs_init();
    interrupt_enable_line(46);
    interrupt_enable_line(47);
    ata_init_devices();

    ps2_kb_init();

    printf("Kernel init done\n");
    kernel_main();
    printf("Kernel end\n");
}