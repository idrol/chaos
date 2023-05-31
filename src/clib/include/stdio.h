#pragma once

#include <cdecl.h>
#include <stdarg.h>
#include <stddef.h>

__cdecl int putchar(const int character);
__cdecl int puts(const char* str);
__cdecl int vprintf(const char* format, va_list list);
__cdecl __attribute__((format(printf, 1, 2))) int printf(const char* format, ...);
__cdecl int vsprintf(char* buffer, const char* format, va_list list);
__cdecl __attribute__((format(sprintf, 1, 2))) int sprintf(char* buffer, const char* format, ...);
__cdecl int vsnprintf(char* buffer, size_t bufferSize, const char* format, va_list list);
__cdecl __attribute__((format(snprintf, 1, 2))) int snprintf(char* buffer, size_t bufferSize, const char* format, ...);