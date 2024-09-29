//
// Created by Idrol on 28/09/2024.
//
#include <drivers/gfx.h>
#include <font.h>

bool gfx_initialized = false;
framebuffer_t framebuffer = framebuffer_t();

void gfx_init(size_t fbAddress, uint32_t width, uint32_t height, uint8_t bytesPerPixel, uint32_t pitch, uint8_t colorMode)
{
    gfx_initialized = true;
    framebuffer.buffer = (void*)fbAddress;
    framebuffer.width = width;
    framebuffer.height = height;
    framebuffer.pitch = pitch;
    framebuffer.bytesPerPixel = bytesPerPixel;
    framebuffer.colorMode = colorMode;
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
    auto fb = (uint8_t*)framebuffer.buffer;
    for(int i = 0; i < 16; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            if(glyph[i]&mask[7-j]) {
                auto pixelOffset = where+framebuffer.bytesPerPixel*j;
                fb[pixelOffset] = b;
                fb[pixelOffset+1] = g;
                fb[pixelOffset+2] = r;
            }
        }
        where += framebuffer.pitch;
    }
}

void drawstring(const char* str, int x, int y, int fgColor, int bgColor)
{

}

int x = 0, y = 0;
void gfx_putc(unsigned char c)
{
    if(!gfx_initialized) return;
    if(c == '\n')
    {
        x = 0;
        y += 16;
        return;
    }
    drawchar_8bpp(c, x, y, 0xDBDBDB, 0x323232);
    x+=8;
    if(x >= framebuffer.width)
    {
        x=0;
        y+=16;
    }
    if(y >= framebuffer.height)
    {
        y = 0;
    }
}

