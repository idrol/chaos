//
// Created by Idrol on 13/05/2023.
//
#include "i386_gdt.h"

gdt_entry_t gdt_entries[6];
gdt_ptr_t gdt_ptr;

extern "C" void setGDT(uint32_t);

__cdecl void i386_gdt_set_entry(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt_entries[num].limit_low = (limit & 0xFFFF);
    gdt_entries[num].limit_high = (limit >> 16) & 0xF;
    gdt_entries[num].base_low = (base & 0xFFFFFF);
    gdt_entries[num].base_high = (base >> 24) & 0xFF;

    gdt_entries[num].accessed = access & 0x1;
    gdt_entries[num].read_write = (access >> 1) & 0x1;
    gdt_entries[num].conforming_expand_down = (access >> 2) & 0x1;
    gdt_entries[num].code = (access >> 3) & 0x1;
    gdt_entries[num].code_data_segment = (access >> 4) & 0x1;
    gdt_entries[num].DPL = (access >> 5) & 0x3;
    gdt_entries[num].present = (access >> 7) & 0x1;

    gdt_entries[num].available = gran & 0x1;
    gdt_entries[num].long_mode = (gran >> 1) & 0x1;
    gdt_entries[num].big = (gran >> 2) & 0x1;
    gdt_entries[num].gran = (gran >> 3) & 0x1;
}

__cdecl void i386_gdt_init() {
    gdt_ptr.limit = (sizeof(gdt_entry_t) * 6) - 1;
    gdt_ptr.base = (uint32_t)&gdt_entries;

    i386_gdt_set_entry(0, 0, 0, 0, 0);
    i386_gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xC);
    i386_gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xC);
    i386_gdt_set_entry(3, 0, 0xFFFFFFFF, 0xFA, 0xC);
    i386_gdt_set_entry(4, 0, 0xFFFFFFFF, 0xF2, 0xC);
    setGDT((uint32_t)&gdt_ptr);
}

__cdecl gdt_entry_t* i386_gdt_get_entry(uint8_t entry) {
    if(entry > 5) return nullptr; // TODO kernel panic
    return &gdt_entries[entry];
}

