//
// Created by Idrol on 14/05/2023.
//
#include "i386_vga.h"
#include "i386_hal.h"
#include "i386_serial.h"
#include <string.h>
#include <tty.h>
#include <memory.h>

uint32_t vga_row;
uint32_t  vga_column;
uint8_t vga_color;
uint16_t* vga_buffer;
#define BUFFER_SIZE (1024 * 1024)
uint8_t* text_buffer = NULL;
size_t bufferNextPos = 0;
uint8_t charactersSinceRerender = 0;

const uint32_t VGA_WIDTH = 80;
const uint32_t VGA_HEIGHT = 25;

bool cursor_enabled = true;

__cdecl void i386_vga_init() {
    vga_row = 0;
    vga_column = 0;
    vga_color = i386_vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_buffer = (uint16_t*)0xC00B8000;

    i386_vga_clear();
}

__cdecl void i386_vga_init_memory() {
    text_buffer = (uint8_t*)kmalloc(BUFFER_SIZE);
    memset(text_buffer, 0x0, BUFFER_SIZE);
    //i386_vga_clear();
}

__cdecl void i386_vga_clear() {
    for(uint32_t y = 0; y < VGA_HEIGHT; y++) {
        for(uint32_t x = 0; x < VGA_WIDTH; x++) {
            const uint32_t index = y * VGA_WIDTH + x;
            vga_buffer[index] = i386_vga_entry(' ', vga_color);
        }
    }
    vga_row = 0;
    vga_column = 0;
    if(cursor_enabled) i386_vga_set_cursor_pos(0, 1);
}

__cdecl uint8_t i386_vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

__cdecl void i386_vga_set_color(uint8_t color) {
    vga_color = color;
}

__cdecl uint16_t i386_vga_entry(uint8_t uc, uint8_t color) {
    return (uint16_t) uc | (uint16_t) color << 8;
}

__cdecl void i386_vga_enable_cursor(uint8_t start, uint8_t end) {
    i386_hal_io_outb(0x3D4, 0x0A);
    i386_hal_io_outb(0x3D5, (i386_hal_io_inb(0x3D5) & 0xC0) | start);

    i386_hal_io_outb(0x3D4, 0x0B);
    i386_hal_io_outb(0x3D5, (i386_hal_io_inb(0x3D5) & 0xE0) | end);
    cursor_enabled = true;
}

__cdecl void i386_vga_disable_cursor() {
    i386_hal_io_outb(0x3D4, 0x0A);
    i386_hal_io_outb(0x3D5, 0x20);
    cursor_enabled = false;
}

__cdecl void i386_vga_set_cursor_pos(uint8_t x, uint8_t y) {
    if(cursor_enabled) {
        uint16_t pos = y * VGA_WIDTH + x;
        i386_hal_io_outb(0x3D4, 0x0F);
        i386_hal_io_outb(0x3D5, (uint8_t) (pos & 0xFF));
        i386_hal_io_outb(0x3D4, 0x0E);
        i386_hal_io_outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
    }
}

__cdecl void i386_vga_put_entry_at(char c, uint8_t color, uint8_t x, uint8_t y) {
    const uint32_t index = y * VGA_WIDTH + x;
    vga_buffer[index] = i386_vga_entry(c, color);
}

__cdecl size_t text_buffer_line_backtrace(uint8_t* buffer, size_t bufferSize, size_t currentPos, size_t lineLength, size_t maxLines, size_t* numCharacters, size_t* numLines) {
    *numLines = 0;
    size_t retPos = currentPos;
    size_t currentLine = 0;
    for(size_t i = 1; i < bufferSize; i++) {
        size_t index = (currentPos - i) % bufferSize;
        uint8_t byte = buffer[index];
        *numCharacters = i;
        if(byte == 0x0) {
            return retPos;
        }
        retPos = index;
        currentLine++;
        if(byte == '\n') {
            currentLine = 0;
            *numLines = *numLines + 1;
        } else if(currentLine >= lineLength) {
            currentLine = 0;
            *numLines = *numLines + 1;
        }
        if(*numLines >= maxLines) {
            if(byte == '\n') {
                retPos = (currentPos - (i - 1)) % bufferSize;
            }
            return retPos;
        }
    }
    return retPos;
}

__cdecl void i386_vga_rerender_text_buffer() {
    i386_vga_clear();
    charactersSinceRerender = 0;
    size_t numLines = 0;
    size_t length = 0;
    size_t bufferStart = text_buffer_line_backtrace(text_buffer, BUFFER_SIZE, bufferNextPos-1, VGA_WIDTH, VGA_HEIGHT-1, &length, &numLines);
    uint32_t y = 0;
    uint32_t x = 0;
    for(size_t i = 0; i < length; i++) {
        uint8_t c = text_buffer[(bufferStart+i)%BUFFER_SIZE];
        if(c == '\n') {
            x = 0;
            y++;
            continue;
        }
        i386_vga_put_entry_at(c, VGA_COLOR_LIGHT_GREY, x, y);
        x++;
        if(x >= VGA_WIDTH) {
            x = 0;
            y++;
        }
    }
    vga_column = x;
    vga_row = y;
    i386_vga_set_cursor_pos(x, y);
    /*if(cursor_enabled) {
        i386_vga_put_entry_at('>', VGA_COLOR_LIGHT_GREY, 0, VGA_HEIGHT-1);
        i386_vga_put_entry_at(' ', VGA_COLOR_LIGHT_GREY, 1, VGA_HEIGHT-1);
        i386_vga_set_cursor_pos(2, VGA_HEIGHT-1);
    }*/
}

__cdecl void i386_vga_put_char(char c) {
    if(text_buffer != NULL) {
        if(c == '\b')
        {
            if(bufferNextPos == 0)
            {
                bufferNextPos = BUFFER_SIZE-1;
            } else
            {
                bufferNextPos--;
            }
            text_buffer[bufferNextPos] = 0x0;
            i386_vga_put_entry_at(' ', VGA_COLOR_LIGHT_GREY, vga_column, vga_row);
            if(--vga_column == 0)
            {
                vga_column = VGA_WIDTH-1;
                if(--vga_row == 0)
                {
                    vga_row = 0;
                }
            }
        } else {
            text_buffer[bufferNextPos] = (uint8_t)c;
            bufferNextPos++;
            bufferNextPos = bufferNextPos % BUFFER_SIZE;
            if(c == '\n') {
                i386_vga_rerender_text_buffer();
                return;
            }
            i386_vga_put_entry_at(c, VGA_COLOR_LIGHT_GREY, vga_column, vga_row);
            if(++vga_column == VGA_WIDTH) {
                vga_column = 0;
                if(++vga_row == VGA_HEIGHT) {
                    vga_row = 0;
                    //i386_vga_clear();
                }
            }
        }
        i386_vga_set_cursor_pos(vga_column, vga_row);
        return;
    }
    if(c == '\n')  {
        vga_column = 0;
        if(++vga_row == VGA_HEIGHT) {
            vga_row = 0;
            //i386_vga_clear();
        }
    } else {
        i386_vga_put_entry_at(c, vga_color, vga_column, vga_row);
        if(++vga_column == VGA_WIDTH) {
            vga_column = 0;
            if(++vga_row == VGA_HEIGHT) {
                vga_row = 0;
                //i386_vga_clear();
            }
        }
    }
    i386_vga_set_cursor_pos(vga_column, vga_row);
}

__cdecl void i386_vga_write(const char* data, uint32_t size) {
    for(uint32_t i = 0; i < size; i++) {
        i386_vga_put_char(data[i]);
    }
}

__cdecl void i386_vga_write_string(const char* data) {
    i386_vga_write(data, strlen(data));
}

__cdecl void i386_vga_new_line() {
    i386_vga_put_char('\n');
}

__cdecl void i386_vga_write_line(const char* data) {
    i386_vga_write_string(data);
    i386_vga_new_line();
}

__cdecl void tty_put_char(const int character) {
    i386_vga_put_char((const char)character);
    i386_write_serial((char)character);
}