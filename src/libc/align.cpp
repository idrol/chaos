//
// Created by Idrol on 20/05/2023.
//
#include <align.h>

__cdecl size_t align(size_t alignment, size_t address) {
    return address-(address%alignment);
}