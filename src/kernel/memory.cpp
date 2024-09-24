//
// Created by Idrol on 23/05/2023.
//
#include <memory.h>
#include <MemoryManager.h>
#include <stdio.h>
#include <stdint.h>

MemoryManager kernelMemoryManager = MemoryManager();

__cdecl void* kmalloc(size_t size) {
    auto address = kernelMemoryManager.alloc(size);
    printf("Kmalloc 0x%lX 0x%lX\n", size, (size_t)address);
    return address;
}

__cdecl void kfree(void* address) {
    printf("Kfree 0x%lX\n", (uint32_t)address);
    if(address == nullptr) return;
    kernelMemoryManager.free(address);
}