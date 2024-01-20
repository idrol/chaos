//
// Created by Idrol on 27/05/2023.
//
#pragma once

#include <panic.h>

__cdecl __attribute__((format (kernel_panic, 1, 2))) void kernel_panic(const char* errorFormat, ...) {
    va_list  list;
    va_start(list, errorFormat);
    vkernel_panic(errorFormat, list);
    va_end(list);
}