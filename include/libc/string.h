#pragma once

#include <stdint.h>
#include <cdecl.h>
#include <stddef.h>
#include <stdbool.h>


__cdecl bool strequal(const char* str1, const char* str2);
__cdecl size_t strlen(const char* str);

__cdecl void* memcpy(void* dest, const void* src, size_t n); // Copy data from src to dst src and st may never overlapp
// __cdecl void* memmove(void* dest, const void* src, size_t n); // Copy data from src to dst src and dst may overlapp

__cdecl void* memset(void* str, int c, size_t n);

// dest needs to be large enough to contain both dest and source + 1
__cdecl char* strcat(char* dest, const char* source);

__cdecl char* strcpy(char* dest, const char* source);

__cdecl char* strtok(char* str1, const char* delim);

__cdecl char* strchr(const char* str, int c);

__cdecl uint32_t strcountchar(const char* str, int c);

__cdecl int32_t strncmp(const char* str1, const char* str2, size_t count);

__cdecl int isspace(char c);

__cdecl void toLower(char* str);
