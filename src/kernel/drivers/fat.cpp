#include <drivers/fat.h>
#include <drivers/memory.h>
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
    char lfnName1[10]; // Wide char array
    uint8_t attributes; // Should always be 0x0F if this is valid LFN record
    uint8_t longEntryType;
    uint8_t shortNameChecksum;
    char lfnName2[12]; // Wide char array
    uint16_t reserved1;
    char lfnName3[4]; // Wide char array
} __attribute__((packed));

typedef struct fat_dirent_struct fat_dirent_t;

struct fat_directory_stream_struct {
    bool firstDataClusterIsRootDirectory;
    uint32_t firstDataCluster;

    bool bufferHasData;
    unsigned char* blockBuffer;
    bool currentClusterIsRootDirectory;
    uint32_t currentCluster;
    uint32_t currentBlockOffset;
    uint32_t byteOffsetInBlock;
    uint32_t dirEntryOffset;

    fs_dir_entry_t currentEntry;
    fat_dirent_t currentFatEntry;
};

typedef struct fat_lfn_ent_struct fat_lfn_ent_t;
typedef struct fat_directory_stream_struct fat_directory_stream_t;

uint8_t* buffer;
fat_dirent_t* returnDir;

void fat_init() {
    fs_register_fs_driver(FS_TYPE_FAT,
                          &fat_create_instance,
                          &fat_openDir,
                          &fat_closeDir,
                          &fat_readDir,
                          &fat_rewindDir,
                          &fat_seekDir,
                          &fat_tellDir,
                          &fat_read,
                          &fat_write);
    buffer = (uint8_t*)kmalloc(512);
    returnDir = (fat_dirent_t*)kmalloc(sizeof(fat_dirent_t));
    printf("Initialized FAT fs driver\n");
}


void* fat_create_instance(block_logical_device_t* device) {
    block_logical_device_read(device, 0, 1, buffer);
    fat_instance_t* instance = (fat_instance_t*)kmalloc(sizeof(fat_instance_t));
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
    /*printf("Bytes per block %i\n", instance->bytesPerBlock);
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
    printf("Num clusters %i\n", instance->numClusters);*/
    if(instance->bytesPerBlock == 0) {
        instance->fatType = FAT_TYPE_EX;
        kfree(instance);
        //printf("Fat type EXFAT unsupported\n");
        return NULL;
    } else if(instance->numClusters < 4085) {
        instance->fatType = FAT_TYPE_12;
        //printf("Fat type FAT12\n");
    } else if(instance->numClusters < 65525) {
        instance->fatType = FAT_TYPE_16;
        //printf("Fat type FAT16\n");
    } else {
        instance->fatType = FAT_TYPE_32;
        //printf("Fat type FAT32\n");
    }
    return (void*)instance;
}

uint32_t fat_first_block_of_cluster(fat_instance_t* fatInstance, uint32_t cluster) {
    return((cluster - 2) * fatInstance->numBlocksPerCluster) + fatInstance->firstDataBlock;
}


uint32_t fat_get_root_directory_block(fat_instance_t* fatInstance) {
    return fatInstance->firstDataBlock - fatInstance->numRootDirectoryBlocks;
}

uint32_t fat_get_fat_table_value(fs_instance_t* instance, uint32_t currentCluster) {
    fat_instance_t* fatInstance = (fat_instance_t*) instance->implPtr;
    unsigned char* FAT_table = (unsigned char*)kmalloc(fatInstance->bytesPerBlock);
    uint32_t retValue = 0xFFF8;

    if(fatInstance->fatType == FAT_TYPE_12 ||
       fatInstance->fatType == FAT_TYPE_16 ||
       fatInstance->fatType == FAT_TYPE_32 ||
       fatInstance->fatType == FAT_TYPE_EX) {

        uint32_t fat_offset = currentCluster;

        if(fatInstance->fatType == FAT_TYPE_12 || fatInstance->fatType == FAT_TYPE_16) {
            fat_offset *= 2;
        } else {
            fat_offset *= 4;
        }

        uint32_t fat_block = fatInstance->numReservedBlocks + (fat_offset / fatInstance->bytesPerBlock);
        uint32_t entry_offset = fat_offset % fatInstance->bytesPerBlock;
        block_logical_device_read(instance->logicalDevice, fat_block, 1, FAT_table);

        if(fatInstance->fatType == FAT_TYPE_12 || fatInstance->fatType == FAT_TYPE_16) {
            uint16_t table_value = *(uint16_t *) &FAT_table[entry_offset];

            if (fatInstance->fatType == FAT_TYPE_12) {
                table_value = (currentCluster & 1) ? table_value >> 4 : table_value & 0xFFFF;
            }

            retValue = table_value;
        } else {
            retValue = *(uint32_t*)&FAT_table[entry_offset];
            if(fatInstance->fatType == FAT_TYPE_32) {
                retValue &= 0x0FFFFFFF;
            }
        }
    }

    kfree(FAT_table);
    return retValue;
}

void fat_extract_lfn_entry_to_buffer(char* lfnBuffer, uint16_t* currentLength, fat_lfn_ent_t* lfn_ent) {
    if(lfnBuffer == NULL) return;
    for(uint16_t c = 0; c < 5; c++) {
        lfnBuffer[*currentLength] = lfn_ent->lfnName1[c*2];
        if(lfnBuffer[*currentLength] == '\0') return;
        *currentLength += 1;
    }
    for(uint16_t c = 0; c < 6; c++) {
        lfnBuffer[*currentLength] = lfn_ent->lfnName2[c*2];
        if(lfnBuffer[*currentLength] == '\0') return;
        *currentLength += 1;
    }
    for(uint16_t c = 0; c < 2; c++) {
        lfnBuffer[*currentLength] = lfn_ent->lfnName3[c*2];
        if(lfnBuffer[*currentLength] == '\0') return;
        *currentLength += 1;
    }
}

unsigned char* fat_get_next_data_block_from_stream(fs_instance_t* instance, fat_directory_stream_t* directoryStream) {
    if(directoryStream == NULL) {
        return NULL;
    }
    fat_instance_t* fat_instance = (fat_instance_t*)instance->implPtr;

    uint32_t blockAddress = 0;
    if(directoryStream->currentClusterIsRootDirectory) {
        blockAddress = fat_get_root_directory_block(fat_instance);
    } else {
        if(directoryStream->currentCluster >= 0x0FFFFFF8 || directoryStream->currentCluster <= 0x1) {
            return NULL; // End of cluster chain
        }
        blockAddress = fat_first_block_of_cluster(fat_instance, directoryStream->currentCluster);
    }

    blockAddress += directoryStream->currentBlockOffset;

    block_logical_device_read(instance->logicalDevice, blockAddress, 1, directoryStream->blockBuffer);
    directoryStream->bufferHasData = true;

    directoryStream->currentBlockOffset++;
    directoryStream->byteOffsetInBlock = 0;

    if(directoryStream->currentClusterIsRootDirectory) {
        if(directoryStream->currentBlockOffset >= fat_instance->numRootDirectoryBlocks) {
            return NULL;
        }
    } else {
        if(directoryStream->currentBlockOffset >= fat_instance->numBlocksPerCluster) {
            directoryStream->currentBlockOffset = 0;
            directoryStream->currentCluster = fat_get_fat_table_value(instance, directoryStream->currentCluster);
            directoryStream->bufferHasData = false;
            directoryStream->currentClusterIsRootDirectory = false;
        }
    }

    return directoryStream->blockBuffer;
}

fs_dir_entry_t* fat_get_next_directory_entry_from_stream(fs_instance_t* instance, fat_directory_stream_t* directoryStream) {
    if(directoryStream == NULL) {
        return NULL;
    }

    fat_instance_t* fat_instance = (fat_instance_t*)instance->implPtr;
    char* lfnBuffer = (char*)kmalloc(FS_NAME_MAX);
    uint16_t lfnLength = 0;

    bool foundDirectoryEntry = false;

    while(!foundDirectoryEntry) {
        if(!directoryStream->bufferHasData) {
            fat_get_next_data_block_from_stream(instance, directoryStream);
        }
        unsigned char* directoryEntryBuffer = directoryStream->blockBuffer;
        uint32_t currentOffset = directoryStream->byteOffsetInBlock;

        directoryStream->byteOffsetInBlock += sizeof(fat_dirent_t);
        if(directoryStream->byteOffsetInBlock >= fat_instance->bytesPerBlock) {
            directoryStream->byteOffsetInBlock = 0;
            fat_get_next_data_block_from_stream(instance, directoryStream);
        }

        if(directoryEntryBuffer[currentOffset] == 0x0) {
            // End of directory
            break;
        } else if(directoryEntryBuffer[currentOffset] == 0xE5) {
            // Unused entry skip
            continue;
        } else if(directoryEntryBuffer[currentOffset+0x0B] == 0x0F) {
            // LFN entry
            fat_lfn_ent_t* lfnEnt = (fat_lfn_ent_t*)&directoryEntryBuffer[currentOffset];
            fat_extract_lfn_entry_to_buffer(lfnBuffer, &lfnLength, lfnEnt);
        } else {
            fat_dirent_t* dirEnt = (fat_dirent_t*)&directoryEntryBuffer[currentOffset];

            if(dirEnt->attributes == 0x08) {
                continue;
            }

            if(dirEnt->attributes != 0x10) {
                directoryStream->currentEntry.fileType = FS_FILE_TYPE_FILE;
            } else {
                directoryStream->currentEntry.fileType = FS_FILE_TYPE_DIRECTORY;
            }
            memcpy(&directoryStream->currentFatEntry, dirEnt, sizeof(fat_dirent_t));
            memset(directoryStream->currentEntry.name, 0x0, FS_NAME_MAX);
            if(lfnLength > 0) {
                // Copy lfn name
                strcpy(directoryStream->currentEntry.name, lfnBuffer);
                directoryStream->currentEntry.nameLen = strlen(lfnBuffer);
                //printf("Long name: %s\n", directoryStream->currentEntry.name);
            } else {
                // Copy short name last 3 bytes are file extension so fullname is 8 bytes + . + last 3 bytes and null terminator
                uint8_t nameLength = 0;
                for(uint8_t i = 0; i < 11; i++) {
                    uint8_t offset = i;
                    if(i > 7) offset += 1;
                    if(dirEnt->fileName[i] == 0x20) {
                        directoryStream->currentEntry.name[offset] = 0x0;
                        break;
                    }
                    directoryStream->currentEntry.name[offset] = dirEnt->fileName[i];
                    nameLength = offset+1;
                }
                directoryStream->currentEntry.name[nameLength] = '\0';
                nameLength++;
                directoryStream->currentEntry.nameLen = nameLength;
                strtolower(directoryStream->currentEntry.name);
                //printf("Short name: %s\n", directoryStream->currentEntry.name);

                /*memcpy(directoryStream->currentEntry.name, dirEnt->fileName, 8);
                directoryStream->currentEntry.nameLen = 8;
                if(dirEnt->fileName[8] != '\0') {
                    directoryStream->currentEntry.name[8] = '.';
                    directoryStream->currentEntry.name[9] = dirEnt->fileName[8];
                    if(dirEnt->fileName[9] != '\0') {
                        directoryStream->currentEntry.name[10] = dirEnt->fileName[9];
                        if(dirEnt->fileName[10] != '\0') {
                            directoryStream->currentEntry.name[11] = dirEnt->fileName[10];
                        } else {
                            directoryStream->currentEntry.name[11] = '\0';
                        }
                    } else {
                        directoryStream->currentEntry.name[10] = '\0';
                    }
                } else {
                    directoryStream->currentEntry.name[8] = '\0';
                }*/
            }
            foundDirectoryEntry = true;
            directoryStream->dirEntryOffset++;
        }
    }

    kfree(lfnBuffer);
    if(foundDirectoryEntry) return &directoryStream->currentEntry;
    return NULL;
}

fs_dir_entry_t* fat_find_directory_entry_from_stream(fs_instance_t* instance, fat_directory_stream_t* directoryStream, const char* name, bool isDirectory) {
    fs_dir_entry_t* dirEntry = fat_get_next_directory_entry_from_stream(instance, directoryStream);
    while(dirEntry != NULL) {
        if(isDirectory && dirEntry->fileType == FS_FILE_TYPE_DIRECTORY && strequal(name, dirEntry->name))
            return dirEntry;

        dirEntry = fat_get_next_directory_entry_from_stream(instance, directoryStream);
    }

    return NULL;
}

fat_directory_stream_t* fat_find_directory_entry(fs_instance_t* instance, const char* directory) {
    fat_instance_t* fat_instance = (fat_instance_t*)instance->implPtr;
    fat_directory_stream_t* currentDirectory = NULL;
    currentDirectory = (fat_directory_stream_t*)kmalloc(sizeof(fat_directory_stream_t));
    currentDirectory->firstDataClusterIsRootDirectory = true;
    currentDirectory->firstDataCluster = 0;
    currentDirectory->bufferHasData = false;
    currentDirectory->blockBuffer = (unsigned char*)kmalloc(fat_instance->bytesPerBlock);
    currentDirectory->currentClusterIsRootDirectory = true;
    currentDirectory->currentCluster = currentDirectory->firstDataCluster;
    currentDirectory->currentBlockOffset = 0;
    currentDirectory->dirEntryOffset = 0;

    if(strlen(directory) == 0) {
        return currentDirectory;
    }

    char* tmpStr = (char*)kmalloc(strlen(directory)+1);
    strcpy(tmpStr, directory);
    tmpStr = path_sanitize(tmpStr);

    char* directoryElem = path_split(tmpStr);
    while(directoryElem != NULL) {
        fs_dir_entry_t* dirEntry = fat_find_directory_entry_from_stream(instance, currentDirectory, directoryElem, true);
        if(dirEntry != NULL) {
            currentDirectory->firstDataClusterIsRootDirectory = false;
            currentDirectory->currentClusterIsRootDirectory = false;
            currentDirectory->firstDataCluster = currentDirectory->currentFatEntry.dataClusterNumLow;
            currentDirectory->bufferHasData = false;
            currentDirectory->currentCluster = currentDirectory->firstDataCluster;
            currentDirectory->currentBlockOffset = 0;
            currentDirectory->dirEntryOffset = 0;
        }

        directoryElem = path_split(NULL);
        if(directoryElem == NULL) {
            kfree(tmpStr);
            return currentDirectory;
        }
    }

    kfree(tmpStr);
    return NULL;
}

DIR* fat_openDir(fs_instance_t* instance, const char* dirName) {
    return fat_find_directory_entry(instance, dirName);
}

int fat_closeDir(fs_instance_t* instance, DIR* dirPtr) {
    fat_instance_t* fat_instance = (fat_instance_t*)instance->implPtr;
    fat_directory_stream_t* directoryStream = (fat_directory_stream_t*)dirPtr;
    memset(directoryStream->blockBuffer, 0x0, fat_instance->bytesPerBlock); // Clear memory before free
    kfree(directoryStream->blockBuffer);
    kfree(dirPtr);
    return FS_RET_OK;
}

fs_dir_entry_t* fat_readDir(fs_instance_t* instance, DIR* dirPtr) {
    return fat_get_next_directory_entry_from_stream(instance, (fat_directory_stream_t*)dirPtr);
}

void fat_rewindDir(fs_instance_t* instance, DIR* dirPtr) {
    fat_instance_t* fat_instance = (fat_instance_t*)instance->implPtr;
    fat_directory_stream_t* directoryStream = (fat_directory_stream_t*)dirPtr;
    memset(directoryStream->blockBuffer, 0x0, fat_instance->bytesPerBlock);
    directoryStream->currentClusterIsRootDirectory = directoryStream->firstDataClusterIsRootDirectory;
    directoryStream->currentCluster = directoryStream->firstDataCluster;
    directoryStream->currentBlockOffset = 0;
    directoryStream->dirEntryOffset = 0;
}

void fat_seekDir(fs_instance_t* instance, DIR* dirPtr, uint64_t dirLocationOffset) {
    fat_rewindDir(instance, dirPtr);
    fat_directory_stream_t* directoryStream = (fat_directory_stream_t*)dirPtr;
    while(directoryStream->dirEntryOffset < dirLocationOffset) {
        fs_dir_entry_t* dirEntry = fat_readDir(instance, dirPtr);
        if(dirEntry == NULL) return;
    }
}

uint64_t fat_tellDir(fs_instance_t* instance, DIR* dirPtr) {
    fat_instance_t* fat_instance = (fat_instance_t*)instance->implPtr;
    fat_directory_stream_t* directoryStream = (fat_directory_stream_t*)dirPtr;
    return directoryStream->dirEntryOffset;
}

/*int fat_list_directory(fs_instance_t* instance, const char* path, fs_dir_entry_t* dirEnt) {
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
}*/

void fat_read(fs_instance_t* instance, const char* path, size_t size, void* ptr) {

}

void fat_write(fs_instance_t* instance, const char* path, size_t size, void* ptr) {

}