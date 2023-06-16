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
#include <memory.h>
#include <stdio.h>
#include <panic.h>

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

    printf("Kernel start 0x%lX kernel end 0x%lX\n", (uint32_t)&kernel_start, (uint32_t)&kernel_end);

    i386_hal_disable_interrupts();

    i386_gdt_init();
    i386_interrupts_init();
    i386_pic_init(32, 40);
    i386_exception_init();

    i386_hal_enable_interrupts();

    i386_memory_init(mbd, (uint32_t)&kernel_start, (uint32_t)&kernel_end, boot_page_directory);

    i386_vga_clear();

    kernel_main();
}