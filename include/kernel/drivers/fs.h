#pragma once

#include <drivers/blockio.h>

#define FS_TYPE_UNKOWN 0x0

// First bit in attributes indicates if the node is a file or directory
// if the bit is set it is a file otherwise it is a directory
#define FS_NODE_ATTRIB_FILE 0x1

#define FS_RET_OK  0
#define FS_RET_EOD -1
#define FS_RET_DIRECTORY_NOT_FOUND -2

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

typedef struct fs_dir_entry_struct fs_dir_entry_t;
typedef struct fs_driver_struct fs_driver_t;
typedef struct fs_instance_struct fs_instance_t;

typedef void*(*fs_create_instance_t)(block_logical_device_t*);
typedef int (*fs_list_directory_fn_t)(fs_instance_t*, const char*, fs_dir_entry_t* dirEntry);
typedef int (*fs_get_dir_entry_fn_t)(fs_instance_t*, const char*, fs_dir_entry_t* dirEntry);
typedef void(*fs_read_fn_t)(fs_instance_t*, const char*, size_t, void*);
typedef void(*fs_write_fn_t)(fs_instance_t*, const char*, size_t, void*);

struct fs_dir_entry_struct {
    char* name;
    uint16_t isFile : 1;
    uint16_t unused : 15;
};

struct fs_driver_struct {
    uint8_t fsType; // The file system type
    fs_create_instance_t createInstanceFn;
    fs_list_directory_fn_t listDirectoryFn;
    fs_get_dir_entry_fn_t getDirEntryFn;
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

fs_driver_t* fs_register_fs_driver(uint8_t fsType, fs_create_instance_t createInstanceFn, fs_get_dir_entry_fn_t getDirEntryFn, fs_list_directory_fn_t lsFn, fs_read_fn_t readFn, fs_write_fn_t writeFn);

fs_instance_t* fs_create_instance(block_logical_device_t* logicalDevice);
void fs_destroy(fs_instance_t* t);

int fs_list_directory(fs_instance_t* instance, const char* path, fs_dir_entry_t *dirEnt);
int fs_get_dir_entry(fs_instance_t* instance, const char* path, fs_dir_entry_t *dirEnt);
void fs_read(fs_instance_t* instance, const char* path, size_t size, void* ptr);
void fs_write(fs_instance_t* instance, const char* path, size_t size, void* ptr);

#ifdef __cplusplus
}
#endif