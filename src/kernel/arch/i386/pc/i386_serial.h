//
// Created by Idrol on 24/09/2024.
//
#pragma once

#define COM1_PORT 0x3f8

void i386_init_serial();
int i386_is_serial_transmit_empty();
void i386_write_serial(char a);