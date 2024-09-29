//
// Created by Idrol on 28/09/2024.
//
#include <font.h>

#define PSF1_FONT_MAGIC 0x0436

typedef struct {
    uint16_t magic; // Magic bytes for identification.
    uint8_t fontMode; // PSF font mode.
    uint8_t characterSize; // PSF character size.
} PSF1_Header;


#define PSF_FONT_MAGIC 0x864ab572

typedef struct {
    uint32_t magic;         /* magic bytes to identify PSF */
    uint32_t version;       /* zero */
    uint32_t headersize;    /* offset of bitmaps in file, 32 */
    uint32_t flags;         /* 0 if there's no unicode table */
    uint32_t numglyph;      /* number of glyphs */
    uint32_t bytesperglyph; /* size of each glyph */
    uint32_t height;        /* height in pixels */
    uint32_t width;         /* width in pixels */
} PSF_font;

extern "C" char _binary_zap_light16_psf_start;
extern "C" char _binary_zap_light16_psf_end;

uint8_t* glyphs = nullptr;
font_size_t fontSize = font_size_t();

void init_font()
{
    PSF1_Header* header = (PSF1_Header*)&_binary_zap_light16_psf_start;
    if(header->magic == PSF1_FONT_MAGIC)
    {
        glyphs = (uint8_t*)(&_binary_zap_light16_psf_start + sizeof(PSF1_Header));
        fontSize.glyphHeight = header->characterSize;
        fontSize.glyphWidth = 8;
        if(fontSize.glyphHeight == 20 || fontSize.glyphHeight == 24)
        {
            fontSize.glyphWidth = 10;
        } else if(fontSize.glyphHeight == 32)
        {
            fontSize.glyphWidth = 16;
        }
        fontSize.bytesPerGlyph = (fontSize.glyphWidth*fontSize.glyphHeight)/8;
    } else
    {
        PSF_font* psfFont = (PSF_font*)&_binary_zap_light16_psf_start;
        if(psfFont->magic == PSF_FONT_MAGIC)
        {
            glyphs = (uint8_t*)(&_binary_zap_light16_psf_start + sizeof(PSF_font));
            fontSize.glyphWidth = psfFont->width;
            fontSize.glyphHeight = psfFont->height;
            fontSize.bytesPerGlyph = (fontSize.glyphWidth*fontSize.glyphHeight)/2;
        }
    }
}

font_size_t get_font_size()
{
    return fontSize;
}

uint8_t* get_glyph_start(uint8_t index)
{
    return &glyphs[fontSize.bytesPerGlyph*index];
}
