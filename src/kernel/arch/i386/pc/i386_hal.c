//
// Created by Idrol on 12/05/2023.
//
#include "i386_hal.h"

void i386_io_wait() {
    asm volatile("jmp 1f\n\t"
                 "1:jmp 2f\n\t"
                 "2:");
}

void i386_hal_enable_interrupts() {
    asm volatile("sti");
}

void i386_hal_disable_interrupts() {
    asm volatile("cli");
}

void i386_hal_io_outb(uint16_t port, uint8_t val) {
    asm volatile("outb %%al, %%dx": : "a"(val), "d"(port));
}

uint8_t i386_hal_io_inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %%dx, %%al" : "=a"(ret) : "d"(port));
    return ret;
}


void i386_hal_io_outw(uint16_t port, uint16_t val) {
    asm volatile("outw %%ax, %%dx": : "a"(val), "d"(port));
}

uint16_t i386_hal_io_inw(uint16_t port) {
    uint16_t ret;
    asm volatile("inw %%dx, %%ax" : "=a"(ret) : "d"(port));
    return ret;
}

void i386_hal_io_outl(uint16_t port, uint32_t val) {
    asm volatile("outl %0, %1": : "a"(val), "Nd"(port));
}

uint32_t i386_hal_io_inl(uint16_t port) {
    uint32_t ret;
    asm volatile("inl %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void i386_hal_io_wait() {
    i386_io_wait();
}

void i386_hal_io_halt() {
    asm volatile("hlt");
}