//
// Created by Idrol on 14/05/2023.
//
#include <panic.h>
#include "i386_vga.h"
#include <string.h>
#include <stdio.h>

__cdecl void vkernel_panic(const char* errorFormat, va_list list) {
    i386_vga_set_color(i386_vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLUE));
    i386_vga_clear();
    char errorBuffer[4096];
    vsnprintf(errorBuffer, 4096, errorFormat, list);
    uint32_t stringLength = strlen(errorBuffer);
    if(stringLength <= 70) {
        i386_vga_set_cursor_pos(40-(stringLength/2), 12);
    } else {
        i386_vga_set_cursor_pos(0, 0);
    }
    i386_vga_write_string(errorBuffer);
    while(true) {
        asm volatile("hlt");
    }
}