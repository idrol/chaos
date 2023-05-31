//
// Created by Idrol on 20/05/2023.
//
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <cdecl.h>

#ifdef __cplusplus

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

__cdecl PageDirectory* paging_kernel_page_directory;
__cdecl PageDirectory* paging_active_page_directory;
#else
struct PageDirectory_struct;
typedef struct PageDirectory_struct PageDirectory;

extern PageDirectory* paging_kernel_page_directory;
extern PageDirectory* paging_active_page_directory;
#endif

__cdecl void paging_set_active_page_directory(PageDirectory* pd);
__cdecl PageDirectory* paging_get_active_page_directory();
__cdecl size_t paging_get_physical_address(size_t virtualAddress);
__cdecl PageDirectory* paging_clone_directory(PageDirectory* directory);