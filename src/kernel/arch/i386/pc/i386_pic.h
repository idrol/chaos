//
// Created by Idrol on 12/05/2023.
//
#pragma once

#include <stdint.h>

#define PIC1 0x20
#define PIC1_DATA PIC1 + 1
#define PIC2 0xA0
#define PIC2_DATA PIC2 + 1
#define PIC_EOI 0x20
#define PIC_READ_IRR 0x0a
#define PIC_READ_ISR 0x0b
#define PIC1_COMMAND PIC1
#define PIC1_MASK (PIC1+1)
#define PIC2_COMMAND PIC2
#define PIC2_MASK (PIC2+1)

#define ICW1_ICW4 0x01 // ICW4 not needed
#define ICW1_SINGLE 0x02 // Single cascade mode
#define ICW1_INTERVAL4 0x04 // Call address interval 4
#define ICW1_LEVEL 0x08 // Level triggered
#define ICW1_INIT 0x10 // Initialization required

#define ICW4_8086 0x01
#define ICW4_AUTO 0x02
#define ICW4_BUF_SLAVE 0x08
#define ICW4_BUF_MASTER 0x0C
#define ICW4_SFNM 0x10

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((no_caller_saved_registers)) void i386_pic_send_eoi(uint8_t irq);
uint8_t  i386_pic_get_mask1();
uint8_t  i386_pic_get_mask2();
void     i386_pic_enable_irq(uint8_t irqLine);
void     i386_pic_disable_irq(uint8_t irqLine);
void     i386_pic_enable_all_irqs();
uint16_t i386_pic_get_irq_register(int32_t ocw3);
__attribute__((no_caller_saved_registers)) uint16_t i386_pic_get_irr();
__attribute__((no_caller_saved_registers)) uint16_t i386_pic_get_isr();

void i386_pic_init(uint16_t offset);
bool i386_pic_is_active();
bool i386_pic_is_pic_irq(uint32_t irq);
uint8_t i386_pic_translate_irq(uint32_t irq);


#ifdef __cplusplus
};
#endif