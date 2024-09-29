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
#include "i386_serial.h"
#include <drivers/memory.h>
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
#include <drivers/gfx.h>
#include <drivers/ps2_kb.h>
#include <drivers/serial.h>
#include <drivers/paging.h>
#include <font.h>

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

void* framebuffer_addr = nullptr;

void framebuffer_draw()
{

}

void fillrect(unsigned char *vram,
              unsigned char r, unsigned char g, unsigned char b,
              int x, int y, int w, int h,
              int pixelWidth, int pitch) {
    unsigned char *where = vram;
    int i, j;

    where += y*pitch + x*pixelWidth;
    for (i = 0; i < w; i++) {
        for (j = 0; j < h; j++) {
            //putpixel(vram, 64 + j, 64 + i, (r << 16) + (g << 8) + b);
            where[j*pixelWidth + 2] = r;
            where[j*pixelWidth + 1] = g;
            where[j*pixelWidth] = b;
        }
        where+=pitch;
    }
}

extern "C" void kernel_i386_main(multiboot_info_t *mbd, uint32_t magic, uint32_t* boot_page_directory, uint32_t stack_top, uint32_t stack_bottom)
{
    if(mbd->flags & 0x1<<12)
    {
        if(mbd->framebuffer_type == 2)
        {
            i386_vga_init();
        } else
        {

        }
        //framebuffer_addr = mbd->framebuffer_addr;
    } else
    {
        i386_vga_init();
    }


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

    i386_init_serial();
    serial_init(&i386_write_serial);

    i386_hal_disable_interrupts();

    i386_gdt_init();
    i386_interrupts_init();
    i386_pic_init(32);
    i386_exception_init();

    i386_hal_enable_interrupts();

    i386_memory_init(mbd, (uint32_t)&kernel_start, (uint32_t)&kernel_end, boot_page_directory);
    i386_tasking_init();

    i386_vga_init_memory();

    init_font();
    if(mbd->flags & 0x1<<12)
    {
        if(mbd->framebuffer_type != 2)
        {
            auto fbSize = mbd->framebuffer_height*mbd->framebuffer_pitch;
            auto alignedFbSize = fbSize - (fbSize % ALIGN_4MIB);
            auto bytesPerPixel = mbd->framebuffer_bpp/8;
            if(mbd->framebuffer_bpp%8 != 0)
            {
                bytesPerPixel+=1;
            }
            if(alignedFbSize != fbSize)
            {
                alignedFbSize += ALIGN_4MIB;
            }
            auto framebuffer = map_physical_address_space((size_t)mbd->framebuffer_addr, alignedFbSize, ALIGN_4MIB);
            uint8_t colorMode = 0;
            if(mbd->framebuffer_red_field_position == 0)
            {
                colorMode = COLOR_MODE_RGBA;
            } else if(mbd->framebuffer_blue_field_position == 0)
            {
                colorMode = COLOR_MODE_BGRA;
            }
            gfx_init((size_t)framebuffer, mbd->framebuffer_width, mbd->framebuffer_height, bytesPerPixel, mbd->framebuffer_pitch, colorMode);
            //fillrect((unsigned char*)framebuffer, 0xEC, 0x55, 0x35, 20, 20, 50, 50, bytesPerPixel, mbd->framebuffer_pitch);
        }
    }

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