//
// Created by Idrol on 20/07/2024.
//
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <HashMap.h>

#define MAX_PROCESSES 65536

struct thread_registers_struct
{
    uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, eip, eflags;
};

typedef struct thread_registers_struct thread_registers_t;

struct thread_control_block_struct;
typedef struct thread_control_block_struct thread_control_block_t;

struct thread_control_block_struct {
    uint32_t stack_top;
    uint32_t page_directory_address;
    thread_registers_t registers;
    thread_control_block_t* next;
};

class Thread
{

};