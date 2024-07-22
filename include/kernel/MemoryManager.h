//
// Created by Idrol on 23/05/2023.
//
#pragma once

#include <stddef.h>

struct MemoryBlock {
    MemoryBlock* prev;
    MemoryBlock* next;

    size_t size;
    bool allocated;
} __attribute__((packed));

// Generic memory manager
class MemoryManager {
public:
    MemoryManager() {};
    MemoryBlock* RegisterFreeBlock(size_t startAddr, size_t size);
    // Alloc size bytes
    void* alloc(size_t size);
    // Free the block associated with ptr
    void free(void* ptr);

    // NOTE even tough active allocations can show 0 there is always a small amount of allocated memory that the memory manager has to keep track of the free memory blocks available
    size_t GetAllocatedMemory();
    size_t GetAvailableMemory();
    size_t GetFreeMemory();
    size_t GetActiveAllocations();

    void PrintStats();

private:
    MemoryBlock *firstBlock = nullptr;
    size_t allocated = 0;
    size_t available = 0;
    size_t activeAllocations = 0;
};
