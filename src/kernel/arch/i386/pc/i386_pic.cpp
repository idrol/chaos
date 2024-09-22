//
// Created by Idrol on 12/05/2023.
//
#include "i386_pic.h"
#include "i386_hal.h"
#include "i386_interrupts.h"
#include <stdio.h>

bool picActive = false;
uint32_t picOffsetStart = 0;

__attribute__((no_caller_saved_registers)) void i386_pic_send_eoi(uint8_t irq) {
    i386_hal_io_outb(PIC1, PIC_EOI);
    if(irq >= 8) {
        i386_hal_io_outb(PIC2, PIC_EOI);
    }
}

uint8_t i386_pic_get_mask1() {
    return i386_hal_io_inb(PIC1_DATA);
}

uint8_t i386_pic_get_mask2() {
    return i386_hal_io_inb(PIC2_DATA);
}

void i386_pic_enable_irq(uint8_t irqLine) {
    uint16_t port;
    uint8_t val;

    if(irqLine < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irqLine -= 8;
    }
    val = i386_hal_io_inb(port) & ~(1 << irqLine);
    i386_hal_io_outb(port, val);
}

void i386_pic_disable_irq(uint8_t irqLine) {
    uint16_t port;
    uint8_t val;

    if(irqLine < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irqLine -= 8;
    }
    val = i386_hal_io_inb(port) | (1 << irqLine);

    i386_hal_io_outb(port, val);
}

void i386_pic_enable_all_irqs() {
    for(int i = 0; i < 16; i++) {
        i386_pic_enable_irq(i);
    }
}

uint16_t i386_pic_get_irq_register(int32_t ocw3) {
    i386_hal_io_outb(PIC1, ocw3);
    i386_hal_io_outb(PIC2, ocw3);
    return (i386_hal_io_inb(PIC2) << 8) | i386_hal_io_inb(PIC1);
}

__attribute__((no_caller_saved_registers)) uint16_t i386_pic_get_irr() {
    return i386_pic_get_irq_register(PIC_READ_IRR);
}

__attribute__((no_caller_saved_registers)) uint16_t i386_pic_get_isr() {
    return i386_pic_get_irq_register(PIC_READ_ISR);
}

__attribute__((interrupt)) void i386_pic_irq_master_handler(intframe_t *iframe) {
    uint16_t isr = i386_pic_get_isr();
    printf("PIC master called %i\n", isr);

    if(isr & 0x0001) {
        //schedule();
    } else if(isr & 0x0002) {
        unsigned char scan_code = i386_hal_io_inb(0x60);

        printf("Keyboard %c\n", scan_code);
        //keyboard_irq();
    } else {
        //printf("Unkown irq\n");
    }
    i386_pic_send_eoi(0);
}

__attribute__((interrupt)) void i386_pic_irq_slave_handler(intframe_t *iframe) {
    uint16_t isr = i386_pic_get_isr();
    printf("PIC Slave called %i\n", isr);
    i386_pic_send_eoi(8);
}

void i386_pic_init(uint16_t offset) {
    picActive = true;
    picOffsetStart = offset;
    uint8_t temp1, temp2;

    temp1 = i386_hal_io_inb(PIC1_MASK);
    temp2 = i386_hal_io_inb(PIC2_MASK);

    i386_hal_io_outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    i386_hal_io_wait();
    i386_hal_io_outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    i386_hal_io_wait();
    i386_hal_io_outb(PIC1_MASK, offset);
    i386_hal_io_wait();
    i386_hal_io_outb(PIC2_MASK, offset+8);
    i386_hal_io_wait();
    i386_hal_io_outb(PIC1_MASK, 4);
    i386_hal_io_wait();
    i386_hal_io_outb(PIC2_MASK, 2);
    i386_hal_io_wait();

    i386_hal_io_outb(PIC1_MASK, ICW4_8086);
    i386_hal_io_wait();
    i386_hal_io_outb(PIC2_MASK, ICW4_8086);
    i386_hal_io_wait();

    i386_hal_io_outb(PIC1_MASK, temp1);
    i386_hal_io_outb(PIC2_MASK, temp2);

    /*for(uint16_t irq = 0; irq < 16; irq++) {
        uint32_t address;
        if(irq < 8) {
            address = (uint32_t)&i386_pic_irq_master_handler;
            //i386_interrupts_set_entry(irq+masterOffset, address, 1, INTERRUPT_GATE);
        } else {
            address = (uint32_t)&i386_pic_irq_slave_handler;
            //i386_interrupts_set_entry(irq+(slaveOffset-8), address, 1, INTERRUPT_GATE);
        }
    }*/

    i386_hal_io_outb(PIC1_DATA, 0xFB);
    i386_hal_io_outb(PIC2_DATA, 0xFF);
}

bool i386_pic_is_active()
{
    return picActive;
}

bool i386_pic_is_pic_irq(uint32_t irq)
{
    return picOffsetStart <= irq < picOffsetStart + 16;
}

uint8_t i386_pic_translate_irq(uint32_t irq)
{
    return (uint8_t)(irq - picOffsetStart);
}

