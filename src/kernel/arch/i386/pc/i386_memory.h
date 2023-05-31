//
// Created by Idrol on 14/05/2023.
//
#pragma once

#include <cdecl.h>
#include <stdint.h>
#include <stddef.h>
#include <grub/multiboot.h>

#define KERNEL_SPACE 0xC0000000 // Kernel space starts at 1 GIB in i386
// Macros for converting to and from logical adress
#define _physical_address(logical_address) (logical_address - KERNEL_SPACE)
#define _logical_address(physical_address) (physical_address + KERNEL_SPACE)

__cdecl void i386_memory_init(multiboot_info_t* mbd, uint32_t kernel_start, uint32_t kernel_end, uint32_t* boot_page_directory);
__cdecl size_t i386_memory_alloc_page(size_t pageAlignment, size_t numPages);