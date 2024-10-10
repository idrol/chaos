//
// Created by Idrol on 28/09/2024.
//
#include <drivers/gfx.h>
#include <font.h>

bool gfx_initialized = false;
framebuffer_t framebuffer = framebuffer_t();

void clear_screen(int color)
{
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    uint32_t offset = 0;
    auto fb = (uint8_t*)framebuffer.buffer;
    for(int y = 0; y < framebuffer.height; y++)
    {
        for(int x = 0; x < framebuffer.width; x++)
        {
            auto pixelOffset = offset+framebuffer.bytesPerPixel*x;
            fb[pixelOffset] = b;
            fb[pixelOffset+1] = g;
            fb[pixelOffset+2] = r;
        }
        offset += framebuffer.pitch;
    }
}

void gfx_init(size_t fbAddress, uint32_t width, uint32_t height, uint8_t bytesPerPixel, uint32_t pitch, uint8_t colorMode)
{
    gfx_initialized = true;
    framebuffer.buffer = (void*)fbAddress;
    framebuffer.width = width;
    framebuffer.height = height;
    framebuffer.pitch = pitch;
    framebuffer.bytesPerPixel = bytesPerPixel;
    framebuffer.colorMode = colorMode;
    clear_screen(0x323232);
}

void drawchar_8bpp(unsigned char c, int x, int y, int fgColor, int bgColor)
{
    if(!gfx_initialized) return;
    uint8_t* glyph = get_glyph_start(c);
    int mask[8]={1,2,4,8,16,32,64,128};
    uint32_t where = x*framebuffer.bytesPerPixel + y*framebuffer.pitch;
    uint8_t r = (fgColor >> 16) & 0xFF;
    uint8_t g = (fgColor >> 8) & 0xFF;
    uint8_t b = fgColor & 0xFF;
    uint8_t bgR = (bgColor >> 16) & 0xFF;
    uint8_t bgG = (bgColor >> 8) & 0xFF;
    uint8_t bgB = bgColor & 0xFF;
    auto fb = (uint8_t*)framebuffer.buffer;
    for(int i = 0; i < 16; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            auto pixelOffset = where+framebuffer.bytesPerPixel*j;
            if(glyph[i]&mask[7-j]) {
                fb[pixelOffset] = b;
                fb[pixelOffset+1] = g;
                fb[pixelOffset+2] = r;
            } else
            {
                fb[pixelOffset] = bgR;
                fb[pixelOffset+1] = bgG;
                fb[pixelOffset+2] = bgB;
            }
        }
        where += framebuffer.pitch;
    }
}


int x = 0, y = 0;
void scroll_screen(uint32_t bgColor)
{
    uint8_t r = (bgColor >> 16) & 0xFF;
    uint8_t g = (bgColor >> 8) & 0xFF;
    uint8_t b = bgColor & 0xFF;
    uint32_t offset = 0;
    uint32_t copyOffset = framebuffer.pitch*16;
    int numVerticalLines = (int)framebuffer.height/16;
    auto fb = (uint8_t*)framebuffer.buffer;
    for(int y = 0; y < (numVerticalLines-1)*16; y++)
    {
        for(int x = 0; x < framebuffer.width; x++)
        {
            auto pixelCopyOffset = copyOffset+framebuffer.bytesPerPixel*x;
            auto pixelOffset = offset+framebuffer.bytesPerPixel*x;
            fb[pixelOffset] = fb[pixelCopyOffset];
            fb[pixelOffset+1] = fb[pixelCopyOffset];
            fb[pixelOffset+2] = fb[pixelCopyOffset];
        }
        offset += framebuffer.pitch;
        copyOffset += framebuffer.pitch;
    }
    offset = (numVerticalLines-1)*16*framebuffer.pitch;
    for(int y = (numVerticalLines-1)*16; y < framebuffer.height; y++)
    {
        for(int x = 0; x < framebuffer.width; x++)
        {
            auto pixelOffset = offset+framebuffer.bytesPerPixel*x;
            fb[pixelOffset] = b;
            fb[pixelOffset+1] = g;
            fb[pixelOffset+2] = r;
        }
        offset += framebuffer.pitch;
    }
    y -= 16;
}

void gfx_putc(unsigned char c)
{
    if(!gfx_initialized) return;
    if(c == '\n')
    {
        x = 0;
        y += 16;
        if(y >= framebuffer.height-16)
        {
            scroll_screen(0x323232);
        }
        return;
    } else if(c == '\b')
    {
        if(x < 8) return;
        x -= 8;
        drawchar_8bpp(' ', x, y, 0xDBDBDB, 0x323232);
        return;
    }
    drawchar_8bpp(c, x, y, 0xDBDBDB, 0x323232);
    x+=8;
    if(x >= framebuffer.width)
    {
        x=0;
        y+=16;
    }
    if(y >= framebuffer.height-16)
    {
        scroll_screen(0x323232);
    }
}

