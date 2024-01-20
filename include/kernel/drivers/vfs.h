#pragma once

#pragma once

#include <drivers/blockio.h>
#include <drivers/fs.h>
#include <stdbool.h>

#define VFS_MAX_MOUNT_POINTS 16

#ifdef __cplusplus
extern "C" {
#endif

struct vfs_mount_point_struct {
    block_logical_device_t* logicalDevice;
    char* path;
    size_t pathLength;
    fs_instance_t* fsInstance;
};

typedef struct vfs_mount_point_struct vfs_mount_point_t;

void vfs_init();

void vfs_mount(block_logical_device_t* logicalDevice, const char* path);
void vfs_unmount(const char* path);
void vfs_unmount_device(block_logical_device_t* logicalDevice);
bool vfs_has_mount_point(const char* path);
const vfs_mount_point_t** vfs_list_mount_points();

// Returns point the dir entry struct that is only garuanteed to be valid until the next call of vfs_ls, NULL is returned if there are no more directory entries
int vfs_list_directory(const char* path, fs_dir_entry_t* dirEnt);
int vfs_get_dir_entry(const char* path, fs_dir_entry_t* dirEnt);
void vfs_read(const char* path, size_t size, void* buffer);
void vfs_write(const char* path, size_t size, void* buffer);

#ifdef __cplusplus
}
#endif