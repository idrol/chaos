#pragma once

#include <drivers/fs.h>
#include <drivers/blockio.h>

#define FS_TYPE_FAT 0x1

#define FAT_TYPE_12 0x0
#define FAT_TYPE_16 0x1
#define FAT_TYPE_32 0x2
#define FAT_TYPE_EX 0x3

#define FAT_STATUS_OK 0
#define FAT_STATUS_NOT_VALID -1 // The logical device does not contain a valid FAT file system 

#ifdef __cplusplus
extern "C" {
#endif

struct fat_instance_struct {
    uint8_t fatType;
    uint16_t bytesPerBlock;
    uint8_t numBlocksPerCluster;
    uint16_t numReservedBlocks;
    uint8_t numFats;
    uint16_t numRootDirectoryEntries;
    uint16_t numRootDirectoryBlocks;
    uint32_t numBlocks;
    uint16_t numBlocksPerFAT;
    uint32_t numHiddenBlocks;
    char volumeLabel[11];
    char systemIdentifier[8];
    uint32_t firstDataBlock;
    uint32_t numDataBlocks;
    uint32_t numClusters;
};

typedef struct fat_instance_struct fat_instance_t;

void fat_init();

void* fat_create_instance(block_logical_device_t* device);

DIR* fat_openDir(fs_instance_t* instance, const char* dirName);
int fat_closeDir(fs_instance_t* instance, DIR* dirPtr);
fs_dir_entry_t* fat_readDir(fs_instance_t* instance, DIR* dirPtr);
void fat_rewindDir(fs_instance_t* instance, DIR* dirPtr);
void fat_seekDir(fs_instance_t* instance, DIR* dirPtr, uint64_t dirLocationOffset);
uint64_t fat_tellDir(fs_instance_t* instance, DIR* dirPtr);

void fat_read(fs_instance_t* instance, const char* path, size_t size, void* ptr);
void fat_write(fs_instance_t* instance, const char* path, size_t size, void* ptr);

#ifdef __cplusplus
}
#endif