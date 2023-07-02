//
// Created by Idrol on 18/06/2023.
//
#pragma once

#include <stddef.h>
#include <stdint.h>

#define FILE uint32_t
#define DIR uint32_t

struct dir_entry_t {
    char* name;
    uint8_t nameLength;
    uint16_t isFile : 1;
    uint16_t isDirectory : 2;
    uint16_t unused : 14;
};
