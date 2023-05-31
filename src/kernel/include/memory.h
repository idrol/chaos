//
// Created by Idrol on 20/05/2023.
//
#pragma once

#include <cdecl.h>
#include <stddef.h>

#define ALIGN_1KIB 1024
#define ALIGN_2KIB (ALIGN_1KIB*2)
#define ALIGN_4KIB (ALIGN_1KIB*4)

#define ALIGN_1MIB (ALIGN_1KIB*1024)
#define ALIGN_2MIB (ALIGN_1MIB*2)
#define ALIGN_4MIB (ALIGN_1MIB*4)
#define ALIGN_32MIB (ALIGN_1MIB*32)

#define BIT_SET(bitMap, bitNum) (bitMap |= 1UL << bitNum)
#define BIT_CLEAR(bitMap, bitNum) (bitMap &= ~(1UL << bitNum))

__cdecl void* mmap(void* addressHint, size_t size);
__cdecl int   munmap(void* startAddress, size_t size);

__cdecl void* kmalloc(size_t size);
__cdecl void  kfree(void* address);