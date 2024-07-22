//
// Created by Idrol on 20/07/2024.
//
#pragma once

#include <stdint.h>
#include <stddef.h>

#define MAX_PROCESSES 65536

#ifdef __cplusplus
extern "C" {
#endif

struct thread_control_block_struct {
    uint32_t stack_top;
    uint32_t page_directory_address;
    thread_control_block_struct* next;
};

typedef struct thread_control_block_struct thread_control_block_t;

#ifdef __cplusplus
}
#endif