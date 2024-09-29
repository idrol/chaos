//
// Created by Idrol on 28/09/2024.
//
#pragma once

#include <stddef.h>
#include <stdint.h>

#define COLOR_MODE_RGBA 1
#define COLOR_MODE_BGRA 2

struct framebuffer_t
{
    void* buffer = nullptr;
    uint32_t width, height, pitch;
    uint8_t bytesPerPixel, colorMode;
};

void gfx_init(size_t fbAddress, uint32_t width, uint32_t height, uint8_t bytesPerPixel, uint32_t pitch, uint8_t colorMode);
void drawstring(const char* str, int x, int y, int fgColor, int bgColor);
void drawchar_8bpp(unsigned char c, int x, int y, int fgColor, int bgColor);
void gfx_putc(unsigned char c);