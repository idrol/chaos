//
// Created by Idrol on 20/05/2023.
//
#include "i386_paging.h"
#include <panic.h>
#include <string.h>
#include <drivers/memory.h>
#include <stdio.h>
#include <drivers/paging.h>

PageDirectory* paging_kernel_page_directory = nullptr;
PageDirectory* paging_active_page_directory = nullptr;

extern "C" void loadPageDirectory(uint32_t*);
extern "C" void enablePaging();

__cdecl void i386_paging_setup_boot_directory(PageDirectory* pd)
{
    paging_kernel_page_directory = pd;
    paging_active_page_directory = pd;
    paging_init((paging_set_active_pd_fn)&i386_paging_set_active_page_directory,
                (paging_get_active_pd_fn)&i386_paging_get_active_page_directory,
                &i386_paging_get_physical_address,
                (paging_clone_pd_fn)&i386_paging_clone_directory,
                &i386_paging_map_address_range,
                &i386_paging_unmap_address_range);
}


__cdecl void i386_paging_set_active_page_directory(PageDirectory* pd) {
    size_t virtualAddress = (size_t)pd;
    if(virtualAddress%ALIGN_4KIB != 0) {
        kernel_panic("Using page directory that is not 4K aligned 0x%lX\n", virtualAddress);
        //printf("%#08X\n", virtualAddress);
    }
    paging_active_page_directory = pd;
    uint32_t physicallAddress = i386_paging_get_physical_address((size_t)pd);
    loadPageDirectory(&physicallAddress);
}

__cdecl size_t i386_paging_get_physical_address(size_t virtualAddress) {
    PageDirectory* pde = &paging_active_page_directory[virtualAddress >> 22];
    size_t offset =  virtualAddress & 0x3FFFFF;
    return pde->GetAddress() | offset;
}

__cdecl PageDirectory* i386_paging_get_active_page_directory() {
    return paging_active_page_directory;
}

__cdecl PageDirectory* i386_paging_clone_directory(PageDirectory* directory) {
    return nullptr;
    /*PageDirectory* pd = (PageDirectory*)kmalloca(sizeof(PageDirectory)*1024, MEM_ALIGN_4K);
    // TODO assumes no page tables
    memset(pd, 0x0, sizeof(PageDirectory)*1024);
    memcpy(pd, ((PageDirectory*)directory+sizeof(PageDirectory)*768), sizeof(PageDirectory)*128);
    return pd;*/
}

__cdecl bool i386_paging_map_address_range(size_t physicalStart, size_t virtualStart, size_t size, size_t alignment)
{
    if(size % alignment != 0)
    {
        printf("Attempting to map with non aligned size %Xl\n", size);
        return false;
    }
    if(physicalStart % alignment != 0)
    {
        printf("Attempting to map with non aligned physical address %Xl\n", physicalStart);
        return false;
    }
    if(virtualStart % alignment != 0)
    {
        printf("Attempting to map with non aligned virtual address %Xl\n", virtualStart);
        return false;
    }

    auto pageDirectory = i386_paging_get_active_page_directory();
    size_t numDirectories = size / alignment;
    for(size_t i = 0; i < numDirectories; i++)
    {
        auto pd = &pageDirectory[pageDirectoryIdFromAddress(virtualStart, i, alignment)];

        if(pd->IsPresent()) {
            printf("Attempting to map physical memory to already mapped page");
            return false;
        }

        pd->SetPresent(true);
        pd->SetPhysical(true);
        pd->SetWriteEnabled(true);

        pd->SetAddress(physicalStart + (alignment * i));
    }

    return true;
}

__cdecl bool i386_paging_unmap_address_range(size_t virtualStart, size_t size, size_t alignment)
{
    if(size % alignment != 0)
    {
        printf("Attempting to map with non aligned size %Xl\n", size);
        return false;
    }
    if(virtualStart % alignment != 0)
    {
        printf("Attempting to map with non aligned virtual address %Xl\n", virtualStart);
        return false;
    }

    auto pageDirectory = i386_paging_get_active_page_directory();
    auto numPages = size / alignment;
    for(size_t i = 0; i < numPages; i++) {
        auto pd = &pageDirectory[pageDirectoryIdFromAddress(virtualStart, i, alignment)];
        if(pd->IsPresent()) {
            pd->Clear();
        } else {
            return false;
        }
    }
    return true;
}



bool PageDirectory::IsPresent() {
    return (bool)(data & 0x1);
}

bool PageDirectory::IsWriteEnabled() {
    return (bool)(data & 0x1 << 1);
}

bool PageDirectory::IsPhysical() {
    return !(bool)(data & 0x1 << 7);
}

bool PageDirectory::IsUserSpace() {
    return (bool)(data & 0x1 << 2);
}

uint32_t PageDirectory::GetAddress() {
    if(IsPhysical()) {
        return data & 0xFFC00000;
    }
    return data & 0xFFFFF000;
}

void PageDirectory::ClearAddress() {
    data &= 0xFFF; // Keeps bits 11->0 thus clearing the bits reserved for address
}

void PageDirectory::SetAddress(uint32_t address) {
    uint32_t maskedAddress;
    if(IsPhysical()) {
        maskedAddress = address & 0xFFC00000;
    } else {
        maskedAddress = address & 0xFFFFF000;
    }

    if(maskedAddress != address) {
        kernel_panic("Page directory address provided was not correctly aligned\n");
    }
    ClearAddress();
    data |= address;
}

void PageDirectory::SetPresent(bool present) {
    if(IsPresent() && present) {
        kernel_panic("Attempting to set present bit of already present page directory entry!\n");
    }

    if(present) {
        BIT_SET(data, 0);
    } else {
        BIT_CLEAR(data, 0);
    }
}

void PageDirectory::SetWriteEnabled(bool writeEnabled) {
    if(writeEnabled) {
        BIT_SET(data, 1);
    } else {
        BIT_CLEAR(data, 1);
    }
}

void PageDirectory::SetUserSpace(bool isUserSpace) {
    if(isUserSpace) {
        BIT_SET(data, 2);
    } else {
        BIT_CLEAR(data, 2);
    }
}

void PageDirectory::SetPhysical(bool isPhysical) {
    // If this is set to false the address points to a page table
    if(isPhysical) {
        BIT_SET(data, 7);
    } else {
        BIT_CLEAR(data, 7);
    }
}

void PageDirectory::Clear() {
    data = 0x0;
}

void PageDirectory::SetData(uint32_t data) {
    this->data = data;
}

uint32_t PageDirectory::GetData() {
    return data;
}