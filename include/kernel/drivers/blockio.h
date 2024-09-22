#pragma once

#include <stdbool.h>
#include <drivers/device.h>
#include <stdint.h>
#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif

struct block_device_struct;
typedef struct block_device_struct block_device_t;

struct block_logical_device_struct;
typedef struct block_logical_device_struct block_logical_device_t;

typedef void(*block_device_read_t)(block_device_t*, uint32_t, uint32_t, void*);
typedef void(*block_device_write_t)(block_device_t*, uint32_t, uint32_t, void*);

struct block_device_struct {
    device_entry_t* device;
    size_t hwDevicePtr; // Pointer to the underlying device
    uint32_t blockSize; // Size of a single block in bytes
    block_device_read_t readBlockImpl;
    block_device_write_t writeBlockImpl;
    uint8_t partitionFormat;
    // NOTE if the partitions for a block device changes then all logical devices are invalidated
    block_logical_device_t* logicalDevices[4]; // An array of logical device pointers
    int8_t bootLogicalDeviceIndex; // if this is >= 0 then this device has a logical device that is designated as boot
};

struct block_logical_device_struct { // Represents a partition of block_device_t
    block_device_t* physicalDevice;
    uint32_t blockOffset;
    uint32_t blockCount;
    uint32_t partitionNum;
    bool isBootable;    
};

void block_init();

block_device_t* block_device_create(size_t hwDevicePtr, uint32_t blockSize, block_device_read_t readBlockImpl, block_device_write_t writeBlockImpl);
void block_device_read(block_device_t* device, uint32_t blockOffset, uint32_t blockCount, void* buffer);
void block_device_write(block_device_t* device, uint32_t blockOffset, uint32_t blockCount, void* buffer);
void block_logical_device_read(block_logical_device_t* device, uint32_t blockOffset, uint32_t blockCount, void* buffer);
void block_logical_device_write(block_logical_device_t* device, uint32_t blockOffset, uint32_t blockCount, void* buffer);

#ifdef __cplusplus
}
#endif