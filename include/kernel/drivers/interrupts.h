//
// Created by Idrol on 21/09/2024.
//
#pragma once

#include <stdint.h>
#include <drivers/device.h>

typedef void(*interrupt_handler)(uint32_t irq, device_entry_t* device);
typedef void(*arch_interrupt_mask_handler)(uint32_t irq, bool enabled);
typedef void(*arch_interrupt_activation_handler)(bool enabled);

struct interrupt_handler_entry_t
{
    interrupt_handler handler = nullptr;
    device_entry_t* device = nullptr;
};


void interrupt_register_handler(uint32_t irq, interrupt_handler handler, device_entry_t* device);
void interrupt_clear_handler(uint32_t irq);
void interrupt_enable();
void interrupt_disable();
void interrupt_enable_line(uint32_t irq);
void interrupt_disable_line(uint32_t irq);

void interrupt_init(arch_interrupt_activation_handler _activation_handler, arch_interrupt_mask_handler _mask_handler);
void interrupt_call_handler(uint32_t irq);