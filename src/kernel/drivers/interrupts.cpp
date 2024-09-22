//
// Created by Idrol on 21/09/2024.
//
#include <drivers/interrupts.h>

interrupt_handler_entry_t interrupt_handlers[256];

arch_interrupt_mask_handler mask_handler = nullptr;
arch_interrupt_activation_handler activation_handler = nullptr;

void interrupt_init(arch_interrupt_activation_handler _activation_handler,arch_interrupt_mask_handler _mask_handler)
{
    activation_handler = _activation_handler;
    mask_handler = _mask_handler;
    for(auto i = 0; i < 256; i++) {
        // Make sure interrupts are cleared
        interrupt_clear_handler(i);
    }
}

void interrupt_call_handler(uint32_t irq)
{
    if(irq < 32 || irq >= 256) return; // Do not raise 0->31 these are exceptions
    auto handler_entry = &interrupt_handlers[irq];
    if(handler_entry->handler != nullptr)
    {
        handler_entry->handler(irq, handler_entry->device);
    }
}

void interrupt_enable()
{
    if(activation_handler == nullptr) return;
    activation_handler(true);
}

void interrupt_disable()
{
    if(activation_handler == nullptr) return;
    activation_handler(false);
}

void interrupt_enable_line(uint32_t irq)
{
    if(mask_handler == nullptr) return;
    mask_handler(irq, true);
}

void interrupt_disable_line(uint32_t irq)
{
    if(mask_handler == nullptr) return;
    mask_handler(irq, false);
}

void interrupt_register_handler(uint32_t irq, interrupt_handler handler, device_entry_t* device)
{
    if(irq < 32 || irq >= 256) return;
    interrupt_handlers[irq].handler = handler;
    interrupt_handlers[irq].device = device;
}

void interrupt_clear_handler(uint32_t irq)
{
    if(irq >= 256) return;
    interrupt_handlers[irq].handler = nullptr;
    interrupt_handlers[irq].device = nullptr;
}
