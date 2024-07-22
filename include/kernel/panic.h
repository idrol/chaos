//
// Created by Idrol on 20/05/2023.
//
#pragma once
#include <cdecl.h>
#include <stdarg.h>

__cdecl void vkernel_panic(const char* errorFormat, va_list list);
__cdecl __attribute__((format (kernel_panic, 1, 2))) void kernel_panic(const char* errorFormat, ...);
