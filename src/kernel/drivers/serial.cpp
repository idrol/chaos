//
// Created by Idrol on 24/09/2024.
//
#include <drivers/serial.h>

serial_write_handler write_handler = nullptr;

void serial_init(serial_write_handler handler)
{
    write_handler = handler;
}

void serial_write(char c)
{
    if(write_handler == nullptr) return
    write_handler(c);
}

