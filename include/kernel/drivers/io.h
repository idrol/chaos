//
// Created by Idrol on 21/09/2024.
//
#pragma once
#include <stdint-gcc.h>

typedef void(*arch_io_hlt)();
typedef void(*arch_io_wait)();

typedef void(*arch_io_outb)(uint16_t port, uint8_t val);
typedef uint8_t(*arch_io_inb)(uint16_t port);
typedef void(*arch_io_outw)(uint16_t port, uint16_t val);
typedef uint16_t(*arch_io_inw)(uint16_t port);
typedef void(*arch_io_outl)(uint16_t port, uint32_t val);
typedef uint32_t(*arch_io_inl)(uint16_t port);

#ifdef __cplusplus
extern "C" {
#endif
void io_init(arch_io_hlt _arch_io_hlt,
             arch_io_wait _arch_io_wait,
             arch_io_outb _arch_io_outb,
             arch_io_inb  _arch_io_inb,
             arch_io_outw _arch_io_outw,
             arch_io_inw  _arch_io_inw,
             arch_io_outl _arch_io_outl,
             arch_io_inl  _arch_io_inl);

void io_halt();
void io_wait();

void io_outb(uint16_t port, uint8_t val);
void io_outw(uint16_t port, uint16_t val);
void io_outl(uint16_t port, uint32_t val);

uint8_t  io_inb(uint16_t port);
uint16_t io_inw(uint16_t port);
uint32_t io_inl(uint16_t port);

#ifdef __cplusplus
    }
#endif