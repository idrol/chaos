//
// Created by Idrol on 14/05/2023.
//
#include "i386_interrupts.h"
#include "i386_hal.h"
#include <stdio.h>
#include <drivers/interrupts.h>

#include "i386_pic.h"

idt_entry_t  idt_entries[256];
idt_ptr_t idt_ptr;

extern "C" void setIDT(uint32_t);

extern "C" uint32_t default_handlers[256];

__cdecl void i386_irq_handler_common(uint32_t irq, intframe_t* interrupt_frame)
{
    interrupt_call_handler(irq);
    if(i386_pic_is_pic_irq(irq))
    {
        i386_pic_send_eoi(i386_pic_translate_irq(irq));
    }
}

void i386_interrupt_activation_handler(bool enabled)
{
    if(enabled)
    {
        i386_hal_enable_interrupts();
    } else
    {
        i386_hal_disable_interrupts();
    }
}

void i386_set_irq_mask(uint32_t irq, bool enabled)
{
    if(i386_pic_is_active() && i386_pic_is_pic_irq(irq))
    {
        uint8_t picIrq = i386_pic_translate_irq(irq);
        if(enabled)
        {
            i386_pic_enable_irq(picIrq);
        } else
        {
            i386_pic_disable_irq(picIrq);
        }
    }
}

__cdecl void i386_interrupts_init() {
    idt_ptr.base = (uint32_t)&idt_entries;
    idt_ptr.limit = (uint16_t)(8 * 256) - 1;

    for(uint16_t i = 0; i < 256; i++) {
        i386_interrupts_set_entry(i, default_handlers[i], 1, INTERRUPT_GATE);
    }

    setIDT((uint32_t)&idt_ptr);

    interrupt_init(&i386_interrupt_activation_handler, &i386_set_irq_mask);
}

__cdecl void i386_interrupts_set_entry(int32_t num, uint32_t offset, uint16_t gdt_entry, uint8_t type) {
    idt_entries[num].offset_low = (offset & 0xFFFF);
    idt_entries[num].offset_high = (offset >> 16 & 0xFFFF);
    idt_entries[num].selector = gdt_entry * 0x08; // Selector refers to which gdt entry and one gdt entry is 0x08 which means entry 1 is located at 0x08.
    idt_entries[num].type_attr = type;
}