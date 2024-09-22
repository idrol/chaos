#include <drivers/blockio.h>
#include <stdio.h>
#include <memory.h>
#include <partition.h>
#include <drivers/vfs.h>

void block_init() {
    partition_init();
    printf("Block driver initialized\n");
}

block_device_t* block_device_create(size_t hwDevicePtr, uint32_t blockSize, block_device_read_t readBlockImpl, block_device_write_t writeBlockImpl) {
    block_device_t* blockDevice = (block_device_t*)kmalloc(sizeof(block_device_t));
    blockDevice->hwDevicePtr = hwDevicePtr;
    blockDevice->blockSize = blockSize;
    blockDevice->readBlockImpl = readBlockImpl;
    blockDevice->writeBlockImpl = writeBlockImpl;
    blockDevice->device = device_register_device(DEVICE_TYPE_BLOCK, (size_t)&blockDevice);
    blockDevice->partitionFormat = partition_identify_type(blockDevice);
    if(blockDevice->partitionFormat == PARTITION_TYPE_GPT) {
        printf("New physical block device with GPT partition discovered\n");
    } else if(blockDevice->partitionFormat == PARTITION_TYPE_MBR) {
        printf("New physical block device with MBR partition discovered\n");
    } else {
        printf("New physical block device with Unknown partition discovered\n");
    }
    //asm volatile("hlt");
    partition_info_t* partitionInfo = parition_get_next_partition(blockDevice);;
    while(partitionInfo != NULL) {
        if(partitionInfo->valid) {
            block_logical_device_t* logicalDevice = (block_logical_device_t*)kmalloc(sizeof(block_logical_device_t));
            logicalDevice->physicalDevice = blockDevice;
            logicalDevice->partitionNum = partitionInfo->partitionIndex;
            logicalDevice->blockOffset = partitionInfo->blockOffset;
            logicalDevice->blockCount = partitionInfo->blockCount;
            blockDevice->logicalDevices[logicalDevice->partitionNum];
            blockDevice->logicalDevices[logicalDevice->partitionNum] = logicalDevice; 
            printf("New logical device %i discovered\n", logicalDevice->partitionNum);
            if(partitionInfo->isBootable) {
                logicalDevice->isBootable = true;
                blockDevice->bootLogicalDeviceIndex = logicalDevice->partitionNum;
                if(!vfs_has_mount_point("/")) {
                    vfs_mount(logicalDevice, "/");
                }
            } else {
                logicalDevice->isBootable = false;
            }
        }
        partitionInfo = parition_get_next_partition(blockDevice);
    }
    return blockDevice;
}

void block_device_read(block_device_t* device, uint32_t blockOffset, uint32_t blockCount, void* buffer) {
    device->readBlockImpl(device, blockOffset, blockCount, buffer);
}

void block_device_write(block_device_t* device, uint32_t blockOffset, uint32_t blockCount, void* buffer) {
    device->writeBlockImpl(device, blockOffset, blockCount, buffer);
}

void block_logical_device_read(block_logical_device_t* device, uint32_t blockOffset, uint32_t blockCount, void* buffer) {
    uint32_t partitionStart = device->blockOffset;
    uint32_t partitionEnd = partitionStart+device->blockCount;

    uint32_t start = partitionStart+blockOffset;
    uint32_t end = start+blockCount;
    if(start > partitionEnd || start < partitionStart)
    {
        printf("Error: Attempted to read outside of logicalDevice 0x%X->0x%X Start: 0x%X\n", partitionStart, partitionEnd, start);
        return;
    }
    if(end > partitionEnd || end < partitionStart) {
        printf("Error: Attempted to read outside of logicalDevice 0x%X->0x%X End: 0x%X\n", partitionStart, partitionEnd, end);
        return;
    }
    device->physicalDevice->readBlockImpl(device->physicalDevice, start, blockCount, buffer);
}

void block_logical_device_write(block_logical_device_t* device, uint32_t blockOffset, uint32_t blockCount, void* buffer) {
    uint32_t partitionStart = device->blockOffset;
    uint32_t partitionEnd = partitionStart+device->blockCount;

    uint32_t start = partitionStart+blockOffset;
    uint32_t end = partitionStart+blockCount;
    if(end > partitionEnd) {
        size_t skipped = end - partitionEnd;
        printf("Error: Attempted to write outside of logicalDevice skipped %i blocks\n", skipped);
        end = partitionEnd;
    }
    device->physicalDevice->writeBlockImpl(device->physicalDevice, start, end, buffer);
}