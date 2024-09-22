//
// Created by Idrol on 20/07/2024.
//
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <HashMap.h>

#define MAX_PROCESSES 65536

struct Thread
{
    uint32_t stack_top;
    uint32_t page_directory_address;

    uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, eip, eflags;

    uint32_t envSize = 0;
    char** env = nullptr;

    Thread* nextThread = nullptr;
};

void thread_init();
Thread *thread_get_active_thread();
const char* getenv(const char* variable);
void setenv(const char* variable, const char* value);
void unsetenv(const char* variable);