//
// Created by Idrol on 15/05/2023.
//
#include "i386_memory.h"
#include <panic.h>
#include <stdio.h>
#include <stddef.h>
#include <align.h>
#include <buddy_allocator.h>
#include "i386_paging.h"
#include <memory.h>

buddy_allocator_t* physicalMemoryAllocator = nullptr;
buddy_allocator_t* kernelVirtualMemoryAllocator = nullptr;
buddy_allocator_t* activeVirtualMemoryAllocator = nullptr;

const char* memory_type(multiboot_uint32_t type) {
    if(type == MULTIBOOT_MEMORY_AVAILABLE) {
        return "Free";
    }
    return "Reserved";
}

uint32_t i386_memory_find_free_memory(multiboot_info_t* mbd, uint32_t kernel_start, uint32_t kernel_end, size_t freeMemorySize, uint32_t alignment) {
    uint32_t pageAlignedKernelStart = align(4096, _physical_address(kernel_start));
    uint32_t pageAlignedKernelSize = align(4096, (kernel_end-kernel_start));
    if((kernel_end-kernel_start)%4096) pageAlignedKernelSize += 4096;


    /* Loop through the memory map and display the values */
    for(uint32_t i = 0; i < mbd->mmap_length; i += sizeof(multiboot_memory_map_t)) {
        auto mmmt = (multiboot_memory_map_t*) (_logical_address(mbd->mmap_addr) + i);

        uint32_t address = mmmt->addrL;
        uint32_t length = mmmt->lenL;
        if(mmmt->addrH > 0x0) continue;
        if(mmmt->lenH > 0x0) length = 0xFFFFFFFF;
        if((0xFFFFFFFF-address) < length) length = 0xFFFFFFFF - address + 1;

        uint32_t nextAddress = address + length;

        if(address == pageAlignedKernelStart) {
            // Grub should place the kernel at the start of a free memory block
            //printf("Start Addr: 0x%lX | Length: 0x%lX | Type: Kernel\n", pageAlignedKernelStart, pageAlignedKernelSize);
            address = pageAlignedKernelStart+pageAlignedKernelSize;
            length -= pageAlignedKernelSize;
        }
        //printf("Start Addr: 0x%lX | Length: 0x%lX | Type: %s\n",
        //       address, length, memory_type(mmmt->type));


        if(mmmt->type == MULTIBOOT_MEMORY_AVAILABLE && length >= freeMemorySize) {
            uint32_t alignedStartAddress = align(alignment, address);
            alignedStartAddress += alignment;
            uint32_t offset = alignedStartAddress - address;
            uint32_t adjustedLength = length - offset;
            if(adjustedLength >= freeMemorySize) {
                return alignedStartAddress;
            }
        }
    }
    return 0x0;
}

uint32_t i386_memory_find_total_memory_size(multiboot_info_t* mbd) {
    uint32_t memorySize = 0x0;
    uint32_t nextAddress = 0x0;

    /* Loop through the memory map and display the values */
    for(uint32_t i = 0; i < mbd->mmap_length; i += sizeof(multiboot_memory_map_t)) {
        auto mmmt = (multiboot_memory_map_t*) (_logical_address(mbd->mmap_addr) + i);


        uint32_t address = mmmt->addrL;
        uint32_t length = mmmt->lenL;

        if(mmmt->addrH > 0x0) continue;
        if(mmmt->lenH > 0x0) length = 0xFFFFFFFF;
        if((0xFFFFFFFF-address) < length) length = 0xFFFFFFFF - address + 1;

        if(nextAddress != address) {
            //printf("Start Addr: 0x%lX | Length: 0x%lX | Type: Reserved\n",
            //       nextAddress, address-nextAddress);
            if(memorySize > (memorySize + (address-nextAddress))) {
                return 0xFFFFFFFF; // We have maximum memory capacity no point in continuing scan
            } else {
                memorySize += address-nextAddress;
            }
            //printf("Size: 0x%lX | Length: 0x%lX\n", memorySize, address-nextAddress);
        }
        nextAddress = address+length;

        //printf("Start Addr: 0x%X | Length: 0x%X | Type: %s\n",
        //       mmmt->addrL, mmmt->lenL, memory_type(mmmt->type));

        if(memorySize > (memorySize + length)) {
            return 0xFFFFFFFF; // We have maximum memory capacity no point in continuing scan
        } else {
            memorySize += length;
        }
        //printf("Size: 0x%lX | Length: 0x%lX\n", memorySize, length);
    }

    return memorySize;
}

void i386_memory_mark_allocated_pages(multiboot_info_t* mbd, uint32_t kernel_start, uint32_t kernel_end, buddy_allocator_t* allocator, uint32_t initialPhysicalMemoryStart, uint32_t initialPhysicalMemorySize) {
    uint32_t pageAlignedKernelStart = align(ALIGN_4KIB, _physical_address(kernel_start));
    uint32_t pageAlignedKernelSize = align(ALIGN_4KIB, (kernel_end-kernel_start));
    if((kernel_end-kernel_start)%ALIGN_4KIB) pageAlignedKernelSize += ALIGN_4KIB;

    /* Loop through the memory map and display the values */
    for(uint32_t i = 0; i < mbd->mmap_length; i += sizeof(multiboot_memory_map_t)) {
        auto mmmt = (multiboot_memory_map_t*) (_logical_address(mbd->mmap_addr) + i);

        uint32_t address = mmmt->addrL;
        uint32_t length = mmmt->lenL;
        if(mmmt->addrH > 0x0) continue;
        if(mmmt->lenH > 0x0) length = 0xFFFFFFFF;
        if((0xFFFFFFFF-address) < length) length = 0xFFFFFFFF - address + 1;

        uint32_t nextAddress = address + length;

        if(address == pageAlignedKernelStart) {
            // Grub should place the kernel at the start of a free memory block
            buddy_allocator_set_allocated(allocator, pageAlignedKernelStart, pageAlignedKernelSize);
            address = pageAlignedKernelStart+pageAlignedKernelSize;
            length -= pageAlignedKernelSize;
        }

        if(mmmt->type != MULTIBOOT_MEMORY_AVAILABLE) {
            buddy_allocator_set_allocated(allocator, address, length);
        }
    }
    buddy_allocator_set_allocated(allocator, initialPhysicalMemoryStart, initialPhysicalMemorySize);
}

__cdecl void i386_memory_init(multiboot_info_t* mbd, uint32_t kernel_start, uint32_t kernel_end, uint32_t* boot_page_directory) {
    if(!(mbd->flags >> 6 & 0x1)) {
        kernel_panic("Bootloader has not provided memory map");
    }

    paging_kernel_page_directory = (PageDirectory*)boot_page_directory;
    paging_active_page_directory = (PageDirectory*)boot_page_directory;

    printf("Page 768 %d 0x%lX\n", paging_kernel_page_directory[768].IsPresent(), paging_kernel_page_directory[768].GetAddress());
    printf("Page 769 %d 0x%lX\n", paging_kernel_page_directory[769].IsPresent(), paging_kernel_page_directory[769].GetAddress());
    printf("Page 770 %d 0x%lX\n", paging_kernel_page_directory[770].IsPresent(), paging_kernel_page_directory[770].GetAddress());

    auto totalMemory = i386_memory_find_total_memory_size(mbd);
    printf("Total memory size is 0x%lX\n", totalMemory);

    const uint32_t MEM_8MB = 0x800000;
    auto neededMemory = (uint32_t)buddy_allocator_get_allocator_size(totalMemory);
    auto freeMemoryStart = i386_memory_find_free_memory(mbd, kernel_start, kernel_end, neededMemory*2, ALIGN_4MIB);
    printf("Found free 8MB at 0x%lX\n", freeMemoryStart);

    auto numPages = (neededMemory*2)/ALIGN_4MIB;
    if((neededMemory*2)%ALIGN_4MIB>0) numPages++;

    for(uint32_t i = 0; i < numPages; i++) {
        auto pd = &paging_get_active_page_directory()[769+i];
        pd->SetPresent(true);
        pd->SetPhysical(true);
        pd->SetWriteEnabled(true);
        pd->SetAddress(freeMemoryStart+(ALIGN_4MIB*i));
    }

    physicalMemoryAllocator = buddy_allocator_new_allocator_from_memory(_logical_address(freeMemoryStart), neededMemory, totalMemory);

    i386_memory_mark_allocated_pages(mbd, kernel_start, kernel_end, physicalMemoryAllocator, freeMemoryStart, neededMemory*2);

    kernelVirtualMemoryAllocator = buddy_allocator_new_allocator_from_memory(_logical_address(freeMemoryStart+neededMemory), neededMemory, totalMemory);
    activeVirtualMemoryAllocator = kernelVirtualMemoryAllocator;

    buddy_allocator_set_allocated(activeVirtualMemoryAllocator, _logical_address(0x0), ALIGN_4MIB*(1+numPages));
    buddy_allocator_set_allocated(activeVirtualMemoryAllocator, 0x0, _logical_address(0x0)); // Mark first 3.5Gib unusable
}

__cdecl size_t i386_memory_alloc_page(size_t pageAlignment, size_t numPages) {
    auto size = pageAlignment * numPages;
    auto virtualAddressBlock = buddy_allocator_alloc_block(activeVirtualMemoryAllocator, size, ALIGN_4MIB);
    //printf("Virtual address alloc 0x%lX alloc size: 0x%lX size: 0x%lX align: 0x%lX\n", virtualAddressBlock.startAddress, virtualAddressBlock.allocatedSize, size, ALIGN_4MIB);
    size_t pageStartNum = virtualAddressBlock.startAddress / ALIGN_4MIB;
    auto physicalAddressBlock = buddy_allocator_alloc_block(physicalMemoryAllocator, size, ALIGN_4MIB);
    //printf("Physical address alloc 0x%lX alloc size: 0x%lX size: 0x%lX align: 0x%lX\n", physicalAddressBlock.startAddress, physicalAddressBlock.allocatedSize, size, ALIGN_4MIB);
    //printf("I386 alloc page called with %zu num pages\n", numPages);
    //printf("Size of size_t %zu\n", sizeof(size_t));
    for(size_t i = 0; i < numPages; i++) {
        //printf("%lu %zu | ", i, numPages);

        auto pageNum = pageStartNum + i;
        if(pageNum > 1023) {
            printf("page num out of range 1024 numPages: %zu index: %lu pageStartNum: %i\n", numPages, i, 760);
            while (true) {
                asm volatile("hlt");
            }
        }
        size_t physicalAddress = physicalAddressBlock.startAddress + (i * ALIGN_4MIB);
        //printf("Mapping page %zu to physical address 0x%lX\n", pageNum, physicalAddress);
        auto pd = &paging_get_active_page_directory()[pageNum];
        if(pd->IsPresent()) {
            printf("Page is already mapped error in buddy allocator page: %i\n", pageNum);
            while(true) {
                asm volatile("hlt");
            }
        }
        pd->SetPresent(true);
        pd->SetPhysical(true);
        pd->SetWriteEnabled(true);
        pd->SetAddress(physicalAddress);
    }
    return virtualAddressBlock.startAddress;
}

__cdecl void i386_memory_unmap_page(size_t startPage, size_t numPages) {
    for(size_t pageNum = startPage; pageNum < startPage+numPages; pageNum++) {
        auto pd = &paging_get_active_page_directory()[pageNum];
        auto physicalAddress = pd->GetAddress();
        pd->Clear();
        buddy_allocator_free_block(physicalMemoryAllocator, physicalAddress, ALIGN_4MIB);
    }
    auto virtualStartAddress = startPage * ALIGN_4MIB;
    buddy_allocator_free_block(activeVirtualMemoryAllocator, virtualStartAddress, numPages*ALIGN_4MIB);
    printf("I386 free pages called with start page %zu and %zu num pages\n", startPage, numPages);
}

__cdecl void* mmap(void* addressHint, size_t size) {
    size_t alignedSize = size - (size % ALIGN_4MIB);
    if(alignedSize != size) alignedSize += ALIGN_4MIB;
    auto startAddress = i386_memory_alloc_page(ALIGN_4MIB, alignedSize/ALIGN_4MIB);
    printf("Mapped address: 0x%lX\n", startAddress);
    return (void *)startAddress;
}

__cdecl int munmap(void* startAddress, size_t size) {
    size_t alignedSize = size - (size % ALIGN_4MIB);
    if(alignedSize != size) alignedSize += ALIGN_4MIB;
    auto startPage = ((size_t)startAddress) / ALIGN_4MIB;
    i386_memory_unmap_page(startPage, alignedSize / ALIGN_4MIB);
    return 0;
}