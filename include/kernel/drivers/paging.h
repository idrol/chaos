//
// Created by Idrol on 27/09/2024.
//
#pragma once

#include <stddef.h>

typedef void* PD;
typedef void(*paging_set_active_pd_fn)(PD);
typedef PD(*paging_get_active_pd_fn)();
typedef size_t(*paging_get_physical_address_fn)(size_t virtualAddress);
typedef PD(*paging_clone_pd_fn)(PD);
typedef bool(*paging_map_address_range_fn)(size_t physicalStart, size_t virtualStart, size_t size, size_t alignment);
typedef bool(*paging_unmap_address_range_fn)(size_t virtualStart, size_t size, size_t alignment);

void paging_init(paging_set_active_pd_fn _setActivePDFn,
                 paging_get_active_pd_fn _getActivePDFn,
                 paging_get_physical_address_fn _getPhysicalAddressFn,
                 paging_clone_pd_fn _clonePDFn,
                 paging_map_address_range_fn _mapAddressRangeFn,
                 paging_unmap_address_range_fn _unmapAddressRangeFn);

void paging_set_active_pd(PD pd);
PD paging_get_active_pd();
size_t paging_get_physical_address(size_t virtualAddress);
PD paging_clone_pd(PD pd);
bool paging_map_address_range(size_t physicalStart, size_t virtualStart, size_t size, size_t alignment);
bool paging_unmap_address_range(size_t virtualStart, size_t size, size_t alignment);
