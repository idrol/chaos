//
// Created by Idrol on 23/05/2023.
//
#include <MemoryManager.h>
#include <stdio.h>
#include <stdint.h>
#include <drivers/memory.h>

MemoryBlock* MemoryManager::RegisterFreeBlock(size_t startAddr, size_t size) {
    //printf("Memory manager registering new block 0x%lX size 0x%lX\n", startAddr, size);
    auto block = (MemoryBlock*)(startAddr);
    block->allocated = false;
    block->size = size - sizeof(MemoryBlock);
    //allocated += sizeof(MemoryBlock);
    available += size;

    if(firstBlock == nullptr) {
        firstBlock = block;
        firstBlock->prev = nullptr;
        firstBlock->next = nullptr;
    } else {
        MemoryBlock* last = firstBlock;
        // Find the last block in the memory chain
        while(last->next != nullptr) {
            last = last->next;
        }
        // Link the new memory block
        last->next = block;
        block->prev = last;
    }
    return block;
}

void* MemoryManager::alloc(size_t size) {
    MemoryBlock* allocatedBlock = nullptr;

    // Find a large anough contigous block of memory for the alloc operation
    for(MemoryBlock* block = firstBlock; block != nullptr && allocatedBlock == nullptr; block = block->next) {
        if(!block->allocated && block->size >= size) {
            // We found a block that is large enough
            allocatedBlock = block;
        }
    }

    if(allocatedBlock == nullptr) {
        // We don't have a block that is large enough in virtual memory
        // Request more memory from kernel
        size_t neededSize = size + sizeof(MemoryBlock); // We also need to account for the storage for the memory block info then requesting more memory from the OS
        size_t alignedSize = neededSize - (neededSize % ALIGN_4MIB);
        if(alignedSize != neededSize) alignedSize += ALIGN_4MIB; // Mmap follows page boundries so the size have to be aligned to page boundries
        auto startAddress = mmap(nullptr, alignedSize, PROT_WRITE | PROT_READ, MAP_HUGETLB);
        if(startAddress == nullptr) return nullptr;
        allocatedBlock = this->RegisterFreeBlock((size_t)startAddress, alignedSize);
    }

    if(allocatedBlock->size == size) {
        // If we were to split the block into the allocated part and and unallocated block the unallocated block could not fit the block header and a single byte of memory and because of that we just mark the entire block as allocated
        allocated += allocatedBlock->size + sizeof(MemoryBlock);
    } else {
        // We can split the unallocated memory into it's own block to allow something else to allocate it
        /*size_t allocatedBlockAddress = (size_t)allocatedBlock;
        size_t memBlockSize = sizeof(MemoryBlock);
        size_t unallocatedBlockAddress = allocatedBlockAddress + memBlockSize + size;
        MemoryBlock* unallocatedBlock = (MemoryBlock*)unallocatedBlockAddress;*/
        auto nonAllocatedSize = allocatedBlock->size - size;
        if(nonAllocatedSize >= sizeof(MemoryBlock) + 0x20) {
            MemoryBlock* unallocatedBlock = (MemoryBlock*)((size_t)allocatedBlock + sizeof(MemoryBlock) + size);
            unallocatedBlock->allocated = false;
            unallocatedBlock->prev = allocatedBlock;
            unallocatedBlock->next = allocatedBlock->next;
            if(unallocatedBlock->next != nullptr) {
                unallocatedBlock->next->prev = unallocatedBlock;
            }
            // We also remove sizeof MemoryBlock here because the new block size should not account
            // for the new blocks memory block header
            unallocatedBlock->size = allocatedBlock->size - size - sizeof(MemoryBlock);

            allocatedBlock->next = unallocatedBlock;
            allocatedBlock->size = size;
        }

        allocated += allocatedBlock->size + sizeof(MemoryBlock);
    }

    allocatedBlock->allocated = true;
    activeAllocations++;
    return (void*)((size_t)allocatedBlock + sizeof(MemoryBlock));
}

void MemoryManager::free(void* ptr) {
    MemoryBlock* block = (MemoryBlock*)((size_t)ptr - sizeof(MemoryBlock));
    block->allocated = false;

    allocated -= block->size;

    if(block->next != nullptr && !block->next->allocated) {
        // Merge the next block with this block as both are unallocated
        MemoryBlock* rightBlock = block->next;

        block->size = block->size + sizeof(MemoryBlock) + rightBlock->size;
        block->next = rightBlock->next;
        if(rightBlock->next != nullptr) {
            rightBlock->next->prev = block;
        }

        allocated -= sizeof(MemoryBlock); // Because we merged 2 memory blocks we removed 1 block header so that memory became unallocated
    } else if(block->prev != nullptr && !block->prev->allocated)
    {
        MemoryBlock* leftBlock = block->prev;

        leftBlock->size = leftBlock->size + sizeof(MemoryBlock) + block->size;
        leftBlock->next = block->next;
        if(block->next != nullptr)
        {
            block->next->prev = leftBlock;
        }

        allocated -= sizeof(MemoryBlock);
    }
    activeAllocations--;
}

size_t MemoryManager::GetAllocatedMemory() {
    return allocated;
}

size_t MemoryManager::GetAvailableMemory() {
    return available;
}

size_t MemoryManager::GetFreeMemory() {
    return available - allocated;
}

size_t MemoryManager::GetActiveAllocations() {
    return activeAllocations;
}

void MemoryManager::PrintStats() {
    uint32_t roundingError;
    uint32_t availableMemoryUnit = 0;
    uint32_t allocatedMemoryUnit = 0;
    //size_t availableMemory = shortenDataUnit(GetAvailableMemory(), availableMemoryUnit, roundingError);
    //size_t allocatedMemory = shortenDataUnit(GetAllocatedMemory(), allocatedMemoryUnit, roundingError);

    printf("Memory:\n");

    printf("    Active allocations: %zi\n", GetActiveAllocations());
    //printf("    Available memory: %zi %s\n", availableMemory, getDataUnitString(availableMemoryUnit));
    //printf("    Allocated memory: %zi %s\n", allocatedMemory, getDataUnitString(allocatedMemoryUnit));
}