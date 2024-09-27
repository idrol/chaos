#include "partition.h"
#include <drivers/memory.h>
#include <string.h>

struct mbr_partition_struct {
    uint8_t attrib;
    uint8_t unused1[3];
    uint8_t partitionType;
    uint8_t unused2[3];
    uint32_t lbaStart; // blockOffset
    uint32_t numSectors; // blockCount
} __attribute__((packed));

typedef struct mbr_partition_struct mbr_partition_t;

uint8_t* blockBuffer;
partition_info_t* partitionInfo;

void partition_init() {
    blockBuffer = (uint8_t*)kmalloc(512);
    partitionInfo = (partition_info_t*)kmalloc(sizeof(partition_info_t));
}

bool partition_is_gpt(block_device_t* device) {
    block_device_read(device, 1, 1, blockBuffer);
    char signature[8];
    memcpy(signature, blockBuffer, 8);
    return strequal("EFI PART", signature);
}

uint8_t partition_identify_type(block_device_t* device) {
    if(partition_is_gpt(device)) return PARTITION_TYPE_GPT;
    // If gpt is not found MBR is assumed
    block_device_read(device, 0, 1, blockBuffer);
    uint32_t partitionOffset = 0x1BE;
    mbr_partition_t partitions[4];
    mbr_partition_t partition;
    bool foundPartition = false;
    for(uint8_t i = 0; i < 4; i++) {
        memcpy(&partition, &blockBuffer[partitionOffset+(sizeof(mbr_partition_t)*i)], sizeof(mbr_partition_t));
        if(partition.numSectors != 0) {
            return PARTITION_TYPE_MBR;
        }
    }
    return PARTITION_TYPE_NONE;
}

block_device_t* lastDevice = NULL;
uint8_t lastPartition = 0;

partition_info_t* parition_get_next_partition(block_device_t* device) {
    if(lastDevice != device) {
        lastDevice = device;
        lastPartition = 0;
        block_device_read(device, 0, 1, blockBuffer);   
    }
    if(lastPartition > 3) return NULL;
    uint32_t partitionOffset = 0x1BE;
    mbr_partition_t partition;
    bool foundPartition = false;
    memcpy(&partition, &blockBuffer[partitionOffset+(sizeof(mbr_partition_t)*lastPartition)], sizeof(mbr_partition_t));
    if(partition.numSectors != 0) {
        partitionInfo->valid = true;
        partitionInfo->partitionIndex = lastPartition;
        partitionInfo->blockOffset = partition.lbaStart;
        partitionInfo->blockCount = partition.numSectors;
        partitionInfo->isBootable = partition.attrib >> 7; // Bit 7 indicated bootable partition
    } else {
        partitionInfo->valid = false;
        partitionInfo->partitionIndex = 0;
        partitionInfo->blockOffset = 0;
        partitionInfo->blockCount = 0;
        partitionInfo->isBootable = 0;
    }
    lastPartition++;
    return partitionInfo;
}