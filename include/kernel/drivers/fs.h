#pragma once

#include <drivers/blockio.h>

#define FS_TYPE_UNKOWN 0x0

// File types
#define FS_FILE_TYPE_UNKNOWN 0x0
#define FS_FILE_TYPE_FILE 0x1
#define FS_FILE_TYPE_DIRECTORY 0x2

#define FS_RET_OK  0
#define FS_RET_EOD -1
#define FS_RET_DIRECTORY_NOT_FOUND -2
#define FS_RET_BAD_DIR_PTR -3

#define NAME_MAX 256
#define FS_NAME_MAX NAME_MAX
#define FS_INITIAL_BUFFER_SIZE 2048
#define FS_DESCRIPTOR_BUFFER_SIZE 1024

#define O_RDONLY 0x1
#define O_WRONLY 0x1 << 1
#define O_RDWR 0x3
#define O_APPEND 0x1 << 2
#define O_CREATE 0x1 << 3



#ifdef __cplusplus
extern "C" {
#endif

/*struct fs_node_struct {
    block_logical_device_t* logicalDevice;
    uint8_t fsType; // The file system type
    uint16_t nodeAttributes;
    uint8_t name[255];
    void* implPtr;
};

typedef struct fs_node_struct fs_node_t;

fs_node_t* fs_open(const char* path);
void fs_close(fs_node_t* node);*/


struct fs_dir_entry_struct;
struct fs_driver_struct;
struct fs_instance_struct;
struct fs_file_descriptor_struct;

typedef struct fs_dir_entry_struct fs_dir_entry_t;
typedef struct fs_driver_struct fs_driver_t;
typedef struct fs_instance_struct fs_instance_t;
typedef struct fs_file_descriptor_struct fs_file_descriptor_t;
typedef void DIR;

typedef void*(*fs_create_instance_t)(block_logical_device_t*);

typedef DIR* (*fs_openDir_fn_t)(fs_instance_t*, const char*);
typedef int (*fs_closeDir_fn_t)(fs_instance_t*, DIR*);
typedef fs_dir_entry_t* (*fs_readDir_fn_t)(fs_instance_t*, DIR* dirPtr);
typedef void (*fs_rewindDir_fn_t)(fs_instance_t*, DIR*);
typedef void (*fs_seekDir_fn_t)(fs_instance_t*, DIR*, uint64_t);
typedef uint64_t (*fs_tellDir_fn_t)(fs_instance_t*, DIR*);

typedef void(*fs_read_fn_t)(fs_instance_t*, const char*, size_t, void*);
typedef void(*fs_write_fn_t)(fs_instance_t*, const char*, size_t, void*);

struct fs_dir_entry_struct {
    char name[FS_NAME_MAX];
    uint8_t fileType;
    uint16_t nameLen;
};

struct fs_file_descriptor_struct {
    char fileName [FS_NAME_MAX];
    uint16_t fileNameLength;

    uint32_t openFlags;
    size_t fileSize;
    // Cursors for underlying file
    size_t readCursor;
    size_t writeCursor;

    uint8_t writeBuffer[FS_DESCRIPTOR_BUFFER_SIZE];
    size_t writeBufferCursor;

    void* implPtr;
};

struct fs_driver_struct {
    uint8_t fsType; // The file system type
    fs_create_instance_t createInstanceFn;

    fs_openDir_fn_t openDirFn;
    fs_closeDir_fn_t closeDirFn;
    fs_readDir_fn_t readDirFn;
    fs_rewindDir_fn_t rewindDirFn;
    fs_seekDir_fn_t seekDirFn;
    fs_tellDir_fn_t tellDirFn;

    fs_read_fn_t readFn;
    fs_write_fn_t writeFn;
};

struct fs_instance_struct {
    block_logical_device_t* logicalDevice;
    uint8_t fsType; // The file system type
    void* implPtr;
};

// Init driver
void fs_init();

fs_driver_t* fs_register_fs_driver(uint8_t fsType,
                                   fs_create_instance_t createInstanceFn,
                                   fs_openDir_fn_t openDirFn,
                                   fs_closeDir_fn_t closeDirFn,
                                   fs_readDir_fn_t readDirFn,
                                   fs_rewindDir_fn_t rewindDirFn,
                                   fs_seekDir_fn_t seekDirFn,
                                   fs_tellDir_fn_t tellDirFn,
                                   fs_read_fn_t readFn,
                                   fs_write_fn_t writeFn);

fs_instance_t* fs_create_instance(block_logical_device_t* logicalDevice);
void fs_destroy(fs_instance_t* t);

DIR* fs_openDir(fs_instance_t* instance, const char* dirName);
int fs_closeDir(fs_instance_t* instance, DIR* dirPtr);
fs_dir_entry_t* fs_readDir(fs_instance_t* instance, DIR* dirPtr);
void fs_rewindDir(fs_instance_t* instance, DIR* dirPtr);
// dirLocationOffset should be a value returned from fs_tellDir
void fs_seekDir(fs_instance_t* instance, DIR* dirPtr, uint64_t dirLocationOffset);
uint64_t fs_tellDir(fs_instance_t* instance, DIR* dirPtr);

int32_t fs_open(fs_instance_t* instance, const char* path, int flags);
int32_t fs_close(fs_instance_t* instance, int32_t fd);

size_t fs_read(fs_instance_t* instance, int32_t fd, void* buffer, size_t count);
size_t fs_write(fs_instance_t* instance, int32_t fd, const void* buffer, size_t count);

int32_t fs_lseek(fs_instance_t* instance, int32_t fd, int32_t offset, int whence);

#ifdef __cplusplus
}
#endif