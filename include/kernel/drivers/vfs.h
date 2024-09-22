#pragma once

#include <drivers/blockio.h>
#include <drivers/fs.h>
#include <drivers/thread.h>
#include <stdbool.h>

#define VFS_MAX_MOUNT_POINTS 16
#define VFS_MAX_FILE_DESCRIPTORS_PER_PROCESS 1024
#define VFS_STD_STREAM_BUFFER_SIZE 65536

#ifdef __cplusplus
extern "C" {
#endif

struct vfs_mount_point_t {
    block_logical_device_t* logicalDevice;
    char* path;
    size_t pathLength;
    fs_instance_t* fsInstance;
};

void vfs_init();

void vfs_mount(block_logical_device_t* logicalDevice, const char* path);
void vfs_unmount(const char* path);
void vfs_unmount_device(block_logical_device_t* logicalDevice);
bool vfs_has_mount_point(const char* path);
vfs_mount_point_t** vfs_list_mount_points();

bool vfs_new_process_descriptors(uint16_t pid);
bool vfs_delete_process_descriptors(uint16_t pid);

DIR* vfs_openDir(const char* dirName);
int vfs_closeDir(DIR* dirPtr);
fs_dir_entry_t* vfs_readDir(DIR* dirPtr);
void vfs_rewindDir(DIR* dirPtr);
void vfs_seekDir(DIR* dirPtr, uint64_t dirLocationOffset);
uint64_t vfs_tellDir(DIR* dirPtr);

int32_t vfs_open(const char* path, int flags);
int32_t vfs_close(int32_t fd);

size_t vfs_read(int32_t fd, void* buffer, size_t count);
size_t vfs_write(int32_t fd, const void* buffer, size_t count);

int32_t vfs_lseek(int32_t fd, int32_t offset, int whence);

#ifdef __cplusplus
}
#endif