//
// Created by Idrol on 20/05/2023.
//
#include <buddy_allocator.h>
#include <panic.h>
#include <string.h>
#include <stdio.h>


const uint32_t mem512K = 512 * 1024;

size_t buddy_allocator_get_allocator_size(size_t totalMemorySize) {
    uint32_t num512KBlocks = totalMemorySize / mem512K;
    if(totalMemorySize % mem512K) num512KBlocks++;
    uint32_t currentBlocks = num512KBlocks;
    size_t size = 0;

    for(uint8_t i = 8; i > 0; i--) {
        uint32_t offset = currentBlocks/8;
        size += offset;
        currentBlocks *= 2;
    }

    size += sizeof(buddy_allocator_t);
    return size;
}

buddy_allocator_t* buddy_allocator_new_allocator_from_memory(size_t startAddress, size_t size, size_t totalMemorySize) {
    uint32_t num512KBlocks = totalMemorySize / mem512K;
    if(totalMemorySize % mem512K) num512KBlocks++;

    uint32_t* localBuddies[8];
    uint32_t currentAddress = startAddress;
    uint32_t currentBlocks = num512KBlocks;
    uint32_t maxAddress = startAddress + size - 1;

    for(uint8_t i = 8; i > 0; i--) {
        localBuddies[i-1] = (uint32_t*)currentAddress;
        uint32_t offset = currentBlocks/8;
        currentAddress += offset;
        if(currentAddress > maxAddress) kernel_panic("Not enough memory to init heap memory");
        currentBlocks *= 2;
    }


    uint32_t allocatorSize = sizeof(buddy_allocator_t);
    auto allocator = (buddy_allocator_t*)currentAddress;
    currentAddress += allocatorSize;

    currentBlocks = num512KBlocks;
    for(uint8_t i = 8; i > 0; i--) {
        allocator->buddies[i-1] = localBuddies[i-1];
        memset(allocator->buddies[i-1], 0x0, currentBlocks/8); // Clear bitmap
        currentBlocks *= 2;
    }
    allocator->totalMemorySize = totalMemorySize;
    allocator->num512KBlocks = num512KBlocks;
    allocator->totalAllocatorSize = currentAddress-startAddress;
    return allocator;
}

uint32_t buddy_allocator_get_num_blocks_in_buddy(buddy_allocator_t* allocator, uint8_t buddyIndex) {
    if(buddyIndex < 7) {
        return allocator->num512KBlocks * (2 << (7-buddyIndex));
    }
    return allocator->num512KBlocks;
}

uint32_t buddy_allocator_get_buddy_block_size(uint32_t buddyIndex) {
    return 4096 << buddyIndex;
}

void buddy_allocator_set_index_allocated(buddy_allocator_t* allocator, uint8_t buddy, uint32_t bitIndex) {
    uint32_t numBlocks = buddy_allocator_get_num_blocks_in_buddy(allocator, buddy);
    uint32_t arrayIndex = bitIndex/32;
    if(arrayIndex*4 >= numBlocks) kernel_panic("Index out of range in set index allocated");
    //if(allocator->buddies[buddy][arrayIndex] >> (bitIndex%32) & 0x1) kernel_panic("Attempting to mark already allocated block as allocated");
    allocator->buddies[buddy][arrayIndex] |= 1 << (bitIndex%32);
}

void buddy_allocator_set_index_free(buddy_allocator_t* allocator, uint8_t buddy, uint32_t bitIndex) {
    uint32_t numBlocks = buddy_allocator_get_num_blocks_in_buddy(allocator, buddy);
    uint32_t arrayIndex = bitIndex/32;
    if(arrayIndex*4 >= numBlocks) kernel_panic("Index out of range in set index allocated");
    allocator->buddies[buddy][arrayIndex] &= ~(1 << (bitIndex%32));
}

void buddy_allocator_set_allocated(buddy_allocator_t* allocator, size_t startAddress, size_t size) {
    for(uint8_t buddyIndex = 0; buddyIndex < 8; buddyIndex++) {
        uint32_t startBitIndex = startAddress / (4096 << buddyIndex); // No need to align int division aligns
        uint32_t numSizeBits = size / (4096 << buddyIndex);
        if(size % (4096 << buddyIndex) != 0) numSizeBits++;
        for(uint32_t i = startBitIndex; i < (startBitIndex+numSizeBits); i++) {
            buddy_allocator_set_index_allocated(allocator, buddyIndex, i);
        }
    }
}

bool buddy_allocator_get_allocation_status(buddy_allocator_t* allocator, uint8_t buddyIndex, uint32_t bitIndex) {
    uint32_t numBlocks = buddy_allocator_get_num_blocks_in_buddy(allocator, buddyIndex);
    uint32_t arrayIndex = bitIndex/32;
    if(arrayIndex*4 >= numBlocks) kernel_panic("Index out of range in get allocation status");

    return (allocator->buddies[buddyIndex][arrayIndex] >> (bitIndex%32) & 0x1);
}

bool buddy_allocator_get_parent_allocation_status(buddy_allocator_t* allocator, uint8_t buddyIndex, uint32_t bitIndex) {
    if(buddyIndex < 1) return true; // Assume allocated in case we get invalid buddy index
    uint32_t parentBitIndex = bitIndex*2;
    return buddy_allocator_get_allocation_status(allocator, buddyIndex-1, parentBitIndex) ||
            buddy_allocator_get_allocation_status(allocator, buddyIndex-1, parentBitIndex+1);
}

void buddy_allocator_free_block(buddy_allocator_t* allocator, size_t startAddress, size_t size) {
    for(uint8_t buddyIndex = 0; buddyIndex < 8; buddyIndex++) {
        uint32_t startBitIndex = startAddress / (4096 << buddyIndex); // No need to align int division aligns
        uint32_t numSizeBits = size / (4096 << buddyIndex);
        if(size % (4096 << buddyIndex) != 0) numSizeBits++;
        for(uint32_t i = startBitIndex; i < (startBitIndex+numSizeBits); i++) {
            if(buddyIndex < 1 ||
              !buddy_allocator_get_parent_allocation_status(allocator, buddyIndex, i)) {
                buddy_allocator_set_index_free(allocator, buddyIndex, i);
            }
        }
    }
}

buddy_allocator_alloc_info_t buddy_allocator_alloc_block(buddy_allocator_t* allocator, size_t requestedBlockSize, size_t alignment) {
    uint32_t blockSize = 4096;
    uint8_t i;
    for(i = 0; i < 8; i++) {
        if(requestedBlockSize < (blockSize << i)) break;
    }
    if(i > 7) i = 7;
    auto currentBlockSize = (blockSize << i);
    uint32_t neededBlocks = requestedBlockSize / (blockSize << i);
    if(requestedBlockSize % (blockSize << i) > 0) neededBlocks++;
    auto bitmap = allocator->buddies[i];
    uint32_t maxIndex = (allocator->num512KBlocks * (8 - i)) / 32;
    uint32_t startIndex = 0;
    uint32_t currentFreeBlocks = 0;
    //printf("Buddy alloc blockSize: %i maxIndex: %i\n", currentBlockSize, maxIndex);
    for(uint32_t index = 0; index < maxIndex; index++) {
        uint32_t bitmapEntry = bitmap[index];
        for(uint32_t bit = 0; bit < 32; bit++) {
            bool allocated = (bitmapEntry >> bit & 0x1);
            if(allocated) {
                startIndex = 0;
                currentFreeBlocks = 0;
            } else {
                auto blockIndex = index * 32 + bit;
                auto blockAddress = blockIndex * (blockSize << i);
                if(startIndex == 0) {
                    if(blockAddress % alignment == 0) {
                        startIndex = index * 32 + bit;
                        currentFreeBlocks++;
                    }
                } else {
                    currentFreeBlocks++;
                }
            }

            if(currentFreeBlocks == neededBlocks) {
                break;
            }
        }

        if(currentFreeBlocks == neededBlocks) {
            break;
        }
    }

    if(currentFreeBlocks < neededBlocks) {
        printf("Error buddy alloc currentFreeBlocks: %i needed: %i\n", currentFreeBlocks, neededBlocks);
        return {0x0, 0x0};
    }

    //printf("Buddy alloc allocated startIndex: %i freeBlocks: %i needed blocks %i\n", startIndex, currentFreeBlocks, neededBlocks);
    size_t startAddress = currentBlockSize*startIndex;
    size_t size = currentFreeBlocks*currentBlockSize;
    buddy_allocator_set_allocated(allocator, startAddress, size);
    return {startAddress, size};
}