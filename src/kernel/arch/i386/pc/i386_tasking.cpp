//
// Created by Idrol on 13/05/2023.
//
#include "i386_tasking.h"
#include "i386_gdt.h"
#include <string.h>

struct tss_entry_struct {
    uint32_t prev_tss; // The previous TSS - with hardware task switching these form a kind of backward linked list.
    uint32_t esp0;     // The stack pointer to load when changing to kernel mode.
    uint32_t ss0;      // The stack segment to load when changing to kernel mode.
    // Everything below here is unused.
    uint32_t esp1; // esp and ss 1 and 2 would be used when switching to rings 1 or 2.
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;
    uint16_t trap;
    uint16_t iomap_base;
} __attribute__((packed));

typedef struct tss_entry_struct tss_entry_t;

tss_entry_t tss_entry;

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
}

__cdecl void i386_tasking_init() {
    i386_tasking_write_tss(i386_gdt_get_entry(5));
    flushTSS();
}