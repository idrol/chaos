//
// Created by Idrol on 28/09/2024.
//
#pragma once

#include <stdint.h>

struct font_size_t
{
    uint32_t bytesPerGlyph;
    uint8_t glyphWidth;
    uint8_t glyphHeight;
};

void init_font();
font_size_t get_font_size();
uint8_t* get_glyph_start(uint8_t index);
