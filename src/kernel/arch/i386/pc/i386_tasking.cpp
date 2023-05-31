//
// Created by Idrol on 13/05/2023.
//
#include "i386_tasking.h"
#include "i386_gdt.h"
#include <string.h>

tss_entry_t tss_entry;
__cdecl tss_entry_t* tss0 = nullptr;

__cdecl void flushTSS(void);

__cdecl void i386_tasking_write_tss(gdt_entry_t* gdtEntry) {
    auto base = (uint32_t)&tss_entry;
    auto limit = sizeof(tss_entry);

    gdtEntry->limit_low = limit;
    gdtEntry->limit_high = (limit >> 16) & 0xF;
    gdtEntry->base_low = base;
    gdtEntry->base_high = (base >> 24);
    gdtEntry->accessed = 0x1;
    gdtEntry->read_write = 0x0;
    gdtEntry->conforming_expand_down = 0;
    gdtEntry->code = 1;
    gdtEntry->code_data_segment = 0;
    gdtEntry->DPL = 0;
    gdtEntry->present = 1;
    gdtEntry->available = 0;
    gdtEntry->long_mode = 0;
    gdtEntry->big = 0;
    gdtEntry->gran = 0;

    memset(&tss_entry, 0x0, sizeof(tss_entry));
    tss_entry.ss0 = 0x10;
    tss_entry.esp0 = 0x0;

    tss0 = &tss_entry;
}

__cdecl void i386_tasking_init() {
    i386_tasking_write_tss(i386_gdt_get_entry(5));
    flushTSS();
}

__cdecl void i386_tasking_set_stack(uint32_t stack) {
    tss_entry.esp0 = stack;
}