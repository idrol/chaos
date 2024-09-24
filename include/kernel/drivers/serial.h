//
// Created by Idrol on 24/09/2024.
//
#pragma once

typedef void(*serial_write_handler)(char c);

void serial_init(serial_write_handler handler);
void serial_write(char c);