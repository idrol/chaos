//
// Created by Idrol on 24/09/2024.
//
#include "i386_serial.h"
#include <stdio.h>
#include <drivers/io.h>

bool isSerialInitialized = false;

void i386_init_serial()
{
    io_outb(COM1_PORT + 1, 0x00);
    io_outb(COM1_PORT + 3, 0x80);
    io_outb(COM1_PORT,     0x03);
    io_outb(COM1_PORT + 1, 0x00);
    io_outb(COM1_PORT + 3, 0x03);
    io_outb(COM1_PORT + 2, 0xC7);
    io_outb(COM1_PORT + 4, 0x0B);
    io_outb(COM1_PORT + 4, 0x1E);
    io_outb(COM1_PORT,     0xAE);

    if(io_inb(COM1_PORT) != 0xAE)
    {
        printf("Serial failed initialization\n");
        return;
    }

    isSerialInitialized = true;
    io_outb(COM1_PORT + 4, 0x0F);
}

int i386_is_serial_transmit_empty()
{
    return io_inb(COM1_PORT + 5) & 0x20;
}

void i386_write_serial(char a)
{
    if(!isSerialInitialized) return;
    while(i386_is_serial_transmit_empty() == 0)
    {

    }
    io_outb(COM1_PORT, a);
}


