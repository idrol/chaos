//
// Created by Idrol on 20/05/2023.
//
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <cdecl.h>

#define pageDirectoryIdFromAddress(address, pageNumOffset, pageSize) ((address+(pageSize*pageNumOffset))/pageSize)


struct PageDirectory {
    void Clear();
    void SetData(uint32_t data);
    uint32_t GetData();
    bool IsPresent();
    bool IsWriteEnabled();
    bool IsPhysical();
    bool IsUserSpace();
    void SetPresent(bool present);
    void SetWriteEnabled(bool writeEnabled);
    void SetPhysical(bool physical);
    void SetUserSpace(bool isUserSpace);
    uint32_t GetAddress();
    // If this is not an alligned address the os will crash. Has to be 4MiB aligned if physical is true otherwise an address to a page table
    void SetAddress(uint32_t address);
    void ClearAddress();

private:
    uint32_t data; // 32-bit bitmap
}  __attribute__((packed));

__cdecl void i386_paging_setup_boot_directory(PageDirectory* pd);
__cdecl void i386_paging_set_active_page_directory(PageDirectory* pd);
__cdecl PageDirectory* i386_paging_get_active_page_directory();
__cdecl size_t i386_paging_get_physical_address(size_t virtualAddress);
__cdecl PageDirectory* i386_paging_clone_directory(PageDirectory* directory);
__cdecl bool i386_paging_map_address_range(size_t physicalStart, size_t virtualStart, size_t size, size_t alignment);
__cdecl bool i386_paging_unmap_address_range(size_t virtualStart, size_t size, size_t alignment);