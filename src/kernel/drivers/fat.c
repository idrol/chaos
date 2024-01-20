#include <drivers/fat.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include <path.h>

struct fat_dirent_struct {
    char fileName[11];
    uint8_t attributes;
    uint8_t reserved1;
    uint8_t timeTenthsOfASecond;
    uint16_t creationTime;
    uint16_t creationDate;
    uint16_t lastAccessedData;
    uint16_t dataClusterNumHigh;
    uint16_t lastModifiedTime;
    uint16_t lastModifiedDate;
    uint16_t dataClusterNumLow;
    uint32_t fileSize;
} __attribute__((packed));

struct fat_lfn_ent_struct {
    uint8_t lfnOrder;
    char lfnName1[10];
    uint8_t attributes; // Should always be 0x0F if this is valid LFN record
    uint8_t longEntryType;
    uint8_t shortNameChecksum;
    char lfnName2[12];
    uint16_t reserved1;
    char lfnName3[4];
} __attribute__((packed));

typedef struct fat_dirent_struct fat_dirent_t;
typedef struct fat_lfn_ent_struct fat_lfn_ent_t;

uint8_t* buffer;
fat_dirent_t* returnDir;

void fat_init() {
    fs_register_fs_driver(FS_TYPE_FAT,
                          &fat_create_instance,
                          &fat_get_directory_entry,
                          &fat_list_directory,
                          &fat_read,
                          &fat_write);
    buffer = kmalloc(512);
    returnDir = kmalloc(sizeof(fat_dirent_t));
    printf("Initialized FAT fs driver\n");
}


void* fat_create_instance(block_logical_device_t* device) {
    block_logical_device_read(device, 0, 1, buffer);
    fat_instance_t* instance = kmalloc(sizeof(fat_instance_t));
    instance->bytesPerBlock = *((uint16_t*)&buffer[0x0B]);
    instance->numBlocksPerCluster = buffer[0x0D];
    instance->numReservedBlocks = *((uint16_t*)&buffer[0x0E]);
    instance->numFats = buffer[0x10];
    instance->numRootDirectoryEntries = *((uint16_t*)&buffer[0x11]);
    instance->numRootDirectoryBlocks = ((instance->numRootDirectoryEntries * 32) + (instance->bytesPerBlock - 1)) / instance->bytesPerBlock;
    instance->numBlocks = (uint32_t)*((uint16_t*)&buffer[0x13]);
    if(instance->numBlocks == 0) {
        instance->numBlocks = *((uint32_t*)&buffer[0x20]);
    }
    instance->numBlocksPerFAT = *((uint16_t*)&buffer[0x16]);
    instance->numHiddenBlocks = *((uint16_t*)&buffer[0x1C]);
    instance->firstDataBlock = instance->numReservedBlocks + (instance->numFats * instance->numBlocksPerFAT) + instance->numRootDirectoryBlocks;
    instance->numDataBlocks = instance->numBlocks - (instance->numReservedBlocks + (instance->numFats * instance->numBlocksPerFAT) + instance->numRootDirectoryBlocks);
    instance->numClusters = instance->numDataBlocks / instance->numBlocksPerCluster;
    
    //tty_clear();
    printf("Bytes per block %i\n", instance->bytesPerBlock);
    printf("Num block per cluster: %i\n", instance->numBlocksPerCluster);
    printf("Num reserved block %i\n", instance->numReservedBlocks);
    printf("Num fats %i\n", instance->numFats);
    printf("Num root directory entries %i\n", instance->numRootDirectoryEntries);
    printf("Num root directory blocks %i\n", instance->numRootDirectoryBlocks);
    printf("Num blocks %i\n", instance->numBlocks);
    printf("Num blocks per fat %i\n", instance->numBlocksPerFAT);
    printf("Num hidden blocks %i\n", instance->numHiddenBlocks);
    printf("First data block %i\n", instance->firstDataBlock);
    printf("Num data blocks %i\n", instance->numDataBlocks);
    printf("Num clusters %i\n", instance->numClusters);
    if(instance->bytesPerBlock == 0) {
        instance->fatType = FAT_TYPE_EX;
        kfree(instance);
        printf("Fat type EXFAT unsupported\n");
        return NULL;
    } else if(instance->numClusters < 4085) {
        instance->fatType = FAT_TYPE_12;
        printf("Fat type FAT12\n");
    } else if(instance->numClusters < 65525) {
        instance->fatType = FAT_TYPE_16;
        printf("Fat type FAT16\n");
    } else {
        instance->fatType = FAT_TYPE_32;
        printf("Fat type FAT32\n");
    }
    return (void*)instance;
}

uint32_t fat_first_block_of_cluster(fs_instance_t* instance, uint32_t cluster) {
    fat_instance_t* fatInstance = (fat_instance_t*)instance->implPtr;
    return((cluster - 2) * fatInstance->numBlocksPerCluster) + fatInstance->firstDataBlock;
}


uint32_t fat_get_root_directory_lba(fat_instance_t* fatInstance) {
    return fatInstance->numReservedBlocks + (fatInstance->numBlocksPerFAT * fatInstance->numFats);
    
}

fat_dirent_t* fat_find_directory_entry(fs_instance_t* instance, uint32_t directoryStart, char* directoryName) {
    fat_instance_t* fatInstance = (fat_instance_t*)instance->implPtr;
    fat_dirent_t* dirEnt;
    block_logical_device_read(instance->logicalDevice, directoryStart, 1, buffer);
    char nameBuffer[255];
    for(size_t offset = 0; offset < fatInstance->bytesPerBlock; offset+=sizeof(fat_dirent_t)) {
        if(buffer[offset] == 0x0 || offset >= 512) break; // No more entries
        if(buffer[offset] == 0xE5) continue; // Unused entry
        uint8_t* bufferOffset = &buffer[offset];
        memcpy(dirEnt, &buffer[offset], sizeof(fat_dirent_t));
        bool isVolumeID = dirEnt->attributes & 0x08;
        if(isVolumeID) continue;
        if(dirEnt->attributes == 0x0F) continue; // LFN entry
        if(strequal(directoryName, dirEnt->fileName)) return dirEnt;
    }
    return NULL;
}

int fat_list_directory(fs_instance_t* instance, const char* path, fs_dir_entry_t* dirEnt) {
    return FS_RET_EOD;
}

fat_dirent_t* fat_get_fat_directory_entry(fs_instance_t* instance, const char* path) {
    fat_instance_t* fatInstance = (fat_instance_t*)instance->implPtr;
    uint32_t directoryLBA = fat_get_root_directory_lba(fatInstance);
    size_t pathLength = strlen(path);
    char* copyPath = kmalloc(pathLength);
    memcpy(copyPath, path, pathLength+1); // Copy NULL terminator
    char* directory = path_split(copyPath);
    fat_dirent_t* dirEnt;
    while(directory != NULL) {
        dirEnt = fat_find_directory_entry(instance, directoryLBA, directory);
        if(dirEnt == NULL) {
            printf("Could not find directory %s\n", directory);
            kfree(copyPath);
            return NULL;
        }
        directory = path_split(NULL);
    }
    return dirEnt;
}

int fat_get_directory_entry(fs_instance_t* instance, const char* path, fs_dir_entry_t* dirEnt) {
    fat_dirent_t* fat_get_fat_directory_entry(instance, path);
    return FS_RET_DIRECTORY_NOT_FOUND;
}

void fat_read(fs_instance_t* instance, const char* path, size_t size, void* ptr) {

}

void fat_write(fs_instance_t* instance, const char* path, size_t size, void* ptr) {

}