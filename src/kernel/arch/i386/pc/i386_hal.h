//
// Created by Idrol on 12/05/2023.
//
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void    i386_hal_enable_interrupts();
void    i386_hal_disable_interrupts();

void    i386_hal_io_outb(uint16_t port, uint8_t val);
uint8_t i386_hal_io_inb(uint16_t port);
void    i386_hal_io_outw(uint16_t port, uint16_t val);
uint16_t i386_hal_io_inw(uint16_t port);
void    i386_hal_io_outl(uint16_t port, uint32_t val);
uint32_t i386_hal_io_inl(uint16_t port);
void    i386_hal_io_wait();
void    i386_hal_io_halt();

#ifdef __cplusplus
};
#endif