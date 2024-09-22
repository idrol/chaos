//
// Created by Idrol on 21/09/2024.
//
#include <drivers/io.h>

namespace {
    arch_io_hlt hlt_handler = nullptr;
    arch_io_wait wait_handler = nullptr;
    arch_io_inb inb_handler = nullptr;
    arch_io_inw inw_handler = nullptr;
    arch_io_inl inl_handler = nullptr;
    arch_io_outb outb_handler = nullptr;
    arch_io_outw outw_handler = nullptr;
    arch_io_outl outl_handler = nullptr;
}

extern "C" {
void io_init(arch_io_hlt _arch_io_hlt,
             arch_io_wait _arch_io_wait,
             arch_io_outb _arch_io_outb,
             arch_io_inb _arch_io_inb,
             arch_io_outw _arch_io_outw,
             arch_io_inw _arch_io_inw,
             arch_io_outl _arch_io_outl,
             arch_io_inl _arch_io_inl)
{
    hlt_handler = _arch_io_hlt;
    wait_handler = _arch_io_wait;
    inb_handler = _arch_io_inb;
    inw_handler = _arch_io_inw;
    inl_handler = _arch_io_inl;
    outb_handler = _arch_io_outb;
    outw_handler = _arch_io_outw;
    outl_handler = _arch_io_outl;
}

void io_halt()
{
    hlt_handler();
}

void io_wait()
{
    wait_handler();
}

uint8_t io_inb(uint16_t port)
{
    return inb_handler(port);
}

uint16_t io_inw(uint16_t port)
{
    return inw_handler(port);
}

uint32_t io_inl(uint16_t port)
{
    return inl_handler(port);
}

void io_outb(uint16_t port, uint8_t val)
{
    outb_handler(port, val);
}

void io_outw(uint16_t port, uint16_t val)
{
    outw_handler(port, val);
}

void io_outl(uint16_t port, uint32_t val)
{
    outl_handler(port, val);
}

}