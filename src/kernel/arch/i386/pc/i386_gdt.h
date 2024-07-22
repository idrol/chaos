//
// Created by Idrol on 13/05/2023.
//
#pragma once

#include <stdint.h>
#include <cdecl.h>

struct gdt_ptr_t {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct gdt_entry {
    unsigned int limit_low              : 16;
    unsigned int base_low               : 24;
    unsigned int accessed               :  1;
    unsigned int read_write             :  1; // readable for code, writable for data
    unsigned int conforming_expand_down :  1; // conforming for code, expand down for data
    unsigned int code                   :  1; // 1 for code, 0 for data
    unsigned int code_data_segment      :  1; // should be 1 for everything but TSS and LDT
    unsigned int DPL                    :  2; // privilege level
    unsigned int present                :  1;
    unsigned int limit_high             :  4;
    unsigned int available              :  1; // only used in software; has no effect on hardware
    unsigned int long_mode              :  1;
    unsigned int big                    :  1; // 32-bit opcodes for code, uint32_t stack for data
    unsigned int gran                   :  1; // 1 to use 4k page addressing, 0 for byte addressing
    unsigned int base_high              :  8;
} __attribute__((packed));

typedef struct gdt_entry gdt_entry_t;

__cdecl void i386_gdt_init();
__cdecl gdt_entry_t* i386_gdt_get_entry(uint8_t entry);