//
// Created by Idrol on 14/05/2023.
//
#include "i386_interrupts.h"
#include "i386_hal.h"

idt_entry_t  idt_entries[256];
idt_ptr_t idt_ptr;

extern "C" void setIDT(uint32_t);

__cdecl __attribute__((interrupt)) void i386_default_handler(intframe_t *iframe) {
    /*printf("Unregistered handler called!!! Dumping frame\n");

    printf("Error code %#08X\n", iframe->error_code);
    printf("EIP %#08X\n", iframe->eip);
    printf("CS %#08X\n", iframe->cs);
    printf("EFLAGS %#08X\n", iframe->eflags);*/
}

__cdecl void i386_interrupts_init() {
    idt_ptr.base = (uint32_t)&idt_entries;
    idt_ptr.limit = (uint16_t)(8 * 256) - 1;

    uint32_t address = (uint32_t)&i386_default_handler;
    for(uint16_t i = 0; i < 256; i++) {
        i386_interrupts_set_entry(i, address, 1, INTERRUPT_GATE);
    }

    setIDT((uint32_t)&idt_ptr);
}

__cdecl void i386_interrupts_set_entry(int32_t num, uint32_t offset, uint16_t gdt_entry, uint8_t type) {
    idt_entries[num].offset_low = (offset & 0xFFFF);
    idt_entries[num].offset_high = (offset >> 16 & 0xFFFF);
    idt_entries[num].selector = gdt_entry * 0x08; // Selector refers to which gdt entry and one gdt entry is 0x08 which means entry 1 is located at 0x08.
    idt_entries[num].type_attr = type;
}