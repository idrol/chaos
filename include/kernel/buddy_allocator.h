//
// Created by Idrol on 15/05/2023.
//
#pragma once

#include <stdint.h>
#include <stddef.h>

struct buddy_allocator_t {
    uint32_t* buddies[8];
    uint32_t totalMemorySize;
    uint32_t num512KBlocks;
    uint32_t totalAllocatorSize;
};

struct buddy_allocator_alloc_info_t {
    size_t startAddress; // Where in physical memory was the block allocated
    size_t allocatedSize; // The actual size allocated
};

size_t                       buddy_allocator_get_allocator_size(size_t totalMemorySize);

buddy_allocator_t*           buddy_allocator_new_allocator_from_memory(size_t startAddress, size_t size, size_t totalMemorySize);
// Allocated new block of memory that is at least requestBlockSize
buddy_allocator_alloc_info_t buddy_allocator_alloc_block(buddy_allocator_t* allocator, size_t requestedBlockSize, size_t alignment);

void                         buddy_allocator_free_block(buddy_allocator_t* allocator, size_t startAddress, size_t size);

// Used when setting up pre-allocated memory should not be used of general allocation
void                         buddy_allocator_set_allocated(buddy_allocator_t* allocator, size_t startAddress, size_t size);