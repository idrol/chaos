#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "drivers/blockio.h"

// First bit in attributes indicates if the node is a file or directory
// if the bit is set it is a file otherwise it is a directory
#define FS_NODE_ATTRIB_FILE 0x1

#define PARTITION_TYPE_UNKNOWN 0x0
#define PARTITION_TYPE_NONE 0x1
#define PARTITION_TYPE_MBR 0x2
#define PARTITION_TYPE_GPT 0x3

#ifdef __cplusplus
extern "C" {
#endif

struct partition_info_struct {
    bool valid;
    uint8_t partitionIndex;
    uint32_t blockOffset;
    uint32_t blockCount;
    bool isBootable;
};

typedef struct partition_info_struct partition_info_t;

void partition_init();

uint8_t partition_identify_type(block_device_t* device);

// The value returned is valid until the next call to this function
partition_info_t* parition_get_next_partition(block_device_t* device);


#ifdef __cplusplus
}
#endif