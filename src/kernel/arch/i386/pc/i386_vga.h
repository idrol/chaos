//
// Created by Idrol on 14/05/2023.
//
#pragma once

#include <cdecl.h>
#include <stdint.h>

enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
};

__cdecl void i386_vga_init();
__cdecl void i386_vga_init_memory();
__cdecl void i386_vga_clear();
__cdecl uint8_t i386_vga_entry_color(enum vga_color fg, enum vga_color bg);
__cdecl void i386_vga_set_color(uint8_t color);
__cdecl uint16_t i386_vga_entry(uint8_t uc, uint8_t color);
__cdecl void i386_vga_enable_cursor(uint8_t start, uint8_t end);
__cdecl void i386_vga_disable_cursor();
__cdecl void i386_vga_set_cursor_pos(uint8_t x, uint8_t y);
__cdecl void i386_vga_put_entry_at(char c, uint8_t color, uint8_t x, uint8_t y);
__cdecl void i386_vga_put_char(char c);
__cdecl void i386_vga_write(const char* data, uint32_t size);
__cdecl void i386_vga_write_string(const char* data);
__cdecl void i386_vga_new_line();
__cdecl void i386_vga_write_line(const char* data);
