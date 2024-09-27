//
// Created by Idrol on 27/09/2024.
//
#include <stdio.h>
#include <drivers/paging.h>

paging_set_active_pd_fn setActiveImpl = nullptr;
paging_get_active_pd_fn getActiveImpl = nullptr;
paging_get_physical_address_fn getPhysicalAddressImpl = nullptr;
paging_clone_pd_fn clonePdImpl = nullptr;
paging_map_address_range_fn mapAddressRangeImpl = nullptr;
paging_unmap_address_range_fn unmapAddressRangeImpl = nullptr;

void paging_init(paging_set_active_pd_fn _setActivePDFn,
                 paging_get_active_pd_fn _getActivePDFn,
                 paging_get_physical_address_fn _getPhysicalAddressFn,
                 paging_clone_pd_fn _clonePDFn,
                 paging_map_address_range_fn _mapAddressRangeFn,
                 paging_unmap_address_range_fn _unmapAddressRangeFn)
{
    setActiveImpl = _setActivePDFn;
    getActiveImpl = _getActivePDFn;
    getPhysicalAddressImpl = _getPhysicalAddressFn;
    clonePdImpl = _clonePDFn;
    mapAddressRangeImpl = _mapAddressRangeFn;
    unmapAddressRangeImpl = _unmapAddressRangeFn;
}

void paging_set_active_pd(PD pd)
{
    if(setActiveImpl == nullptr)
    {
        printf("Paging not initialized!!!\n");
        return;
    }
    setActiveImpl(pd);
}

PD paging_get_active_pd()
{
    if(getActiveImpl == nullptr)
    {
        printf("Paging not initialized!!!\n");
        return nullptr;
    }
    return getActiveImpl();
}

size_t paging_get_physical_address(size_t virtualAddress)
{
    if(getPhysicalAddressImpl == nullptr)
    {
        printf("Paging not initialized!!!\n");
        return 0x0;
    }
    return getPhysicalAddressImpl(virtualAddress);
}

PD paging_clone_pd(PD pd)
{
    if(clonePdImpl == nullptr)
    {
        printf("Paging not initialized!!!\n");
        return nullptr;
    }
    return clonePdImpl(pd);
}

bool paging_map_address_range(size_t physicalStart, size_t virtualStart, size_t size, size_t alignment)
{
    if(mapAddressRangeImpl == nullptr)
    {
        printf("Paging not initialized!!!\n");
        return false;
    }
    return mapAddressRangeImpl(physicalStart, virtualStart, size, alignment);
}

bool paging_unmap_address_range(size_t virtualStart, size_t size, size_t alignment)
{
    if(unmapAddressRangeImpl == nullptr)
    {
        printf("Paging not initialized!!!\n");
        return false;
    }
    return unmapAddressRangeImpl(virtualStart, size, alignment);
}




