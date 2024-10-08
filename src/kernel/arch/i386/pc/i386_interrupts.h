//
// Created by Idrol on 13/05/2023.
//
#pragma once

#include <cdecl.h>
#include <stdint.h>

#define INTERRUPT_GATE 0x8E

struct idt_ptr_t {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct idt_entry {
    uint16_t offset_low; // Bit 0..15 of the 32 bit offset address
    uint16_t selector; // 16 bit selector
    uint8_t zero; // 8 Bit Unused have to be 0x00
    // Bit 0..3 is the type
    // Bit 4    is the storage segment should be 0 with interrupt and trap gates
    // Bit 5..6 is the Descriptor privilege level
    // Bit 7    is the Present bit should be 0 if unused
    uint8_t type_attr;
    uint16_t offset_high; // Bit 16..31 of the 32 bit offset address

} __attribute__((packed));

__cdecl struct intframe_t {
    uint32_t eip;
    uint16_t cs;
    uint32_t eflags;
    // Set if privilege change
    uint32_t esp;
    uint16_t ss;
} __attribute__((packed));

__cdecl struct int_error_frame_t
{
    uint32_t error_code;
    uint32_t eip;
    uint16_t cs;
    uint32_t eflags;
    // Set if privilege change
    uint32_t esp;
    uint16_t ss;
} __attribute__((packed));

typedef struct idt_entry idt_entry_t;

__cdecl void i386_interrupts_init();

__cdecl void i386_interrupts_set_entry(int32_t num, uint32_t offset, uint16_t gdt_entry, uint8_t type);

