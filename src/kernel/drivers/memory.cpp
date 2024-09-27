//
// Created by Idrol on 23/05/2023.
//
#include <drivers/memory.h>
#include <drivers/paging.h>
#include <MemoryManager.h>
#include <stdio.h>
#include <stdint.h>

MemoryManager kernelMemoryManager = MemoryManager();
buddy_allocator_t* physicalAllocator = nullptr;
buddy_allocator_t* kernelVirtualAllocator = nullptr;
buddy_allocator_t* activeVirtualAllocator = nullptr;

__cdecl void memory_init(buddy_allocator_t* _physicalAllocator, buddy_allocator_t* _kernelVirtualAllocator)
{
    physicalAllocator = _physicalAllocator;
    kernelVirtualAllocator = _kernelVirtualAllocator;
    activeVirtualAllocator = kernelVirtualAllocator;
}

__cdecl void* mmap(void* addressHint, size_t size, int prot, int flags) {
    size_t alignment = ALIGN_4KIB;

    if(flags & MAP_HUGETLB) {
        alignment = ALIGN_4MIB;
    } else {
        printf("mmap does not support omitting MAP_HUGETLB");
        return nullptr;
    }

    size_t alignedSize = size - (size % alignment);
    if(alignedSize != size) alignedSize += alignment;
    if(addressHint != NULL) {
        printf("Address hint for mmap not supported");
        return nullptr;
    }
    // Allocate continous virtual pages
    auto virtualBlock = buddy_allocator_alloc_block(activeVirtualAllocator, alignedSize, alignment);
    auto startVirtualAddress = virtualBlock.startAddress;

    auto physicalBlock = buddy_allocator_alloc_block(physicalAllocator, alignedSize, alignment);
    auto startPhysicalAddress = physicalBlock.startAddress;

    if(startVirtualAddress % alignment) {
        printf("Allocation error allocated page is not aligned to page boundary");
        return nullptr;
    }

    if(!paging_map_address_range(startPhysicalAddress, startVirtualAddress, alignedSize, alignment))
    {
        printf("Page mapping error physical: %Xl virtual: %Xl size: %Xl alignment: %Xl\n", startPhysicalAddress, startVirtualAddress, alignedSize, alignment);
        return nullptr;
    }

    //printf("Mapped address: 0x%lX\n", startVirtualAddress);
    return (void *)startVirtualAddress;
}

__cdecl int munmap(void* startAddress, size_t size) {
    size_t alignment = ALIGN_4MIB;
    auto address = (uint32_t)startAddress;

    if(address % alignment != 0) {
        alignment = ALIGN_4KIB;
        if(address % alignment != 0) {
            return -1;
        }
    }

    if(alignment == ALIGN_4KIB) {
        printf("4KIB alignment not supporter in paging");
        return -1;
    }

    size_t alignedSize = size - (size % alignment);
    if(alignedSize != size) alignedSize += alignment;

    if(!paging_unmap_address_range(address, alignedSize, alignment))
    {
        return -1;
    }
    return 0;
}

__cdecl void* kmalloc(size_t size) {
    auto address = kernelMemoryManager.alloc(size);
    //printf("Kmalloc 0x%lX 0x%lX\n", size, (size_t)address);
    return address;
}

__cdecl void kfree(void* address) {
    //printf("Kfree 0x%lX\n", (uint32_t)address);
    if(address == nullptr) return;
    kernelMemoryManager.free(address);
}