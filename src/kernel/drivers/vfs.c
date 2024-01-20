#include <drivers/vfs.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>

vfs_mount_point_t** mountPoints;

void vfs_init() {
    mountPoints = kmalloc(sizeof(vfs_mount_point_t*)*VFS_MAX_MOUNT_POINTS);
    for(size_t i = 0; i < VFS_MAX_MOUNT_POINTS; i++) {
        mountPoints[i] = NULL;
    }
    printf("Initialized vfs driver\n");
}

void vfs_sort_mount_points() {
    for(size_t i = 0; i < VFS_MAX_MOUNT_POINTS; i++) {

    }
}

void vfs_mount(block_logical_device_t* logicalDevice, const char* path) {
    printf("Mounting %s\n", path);
    fs_instance_t* fsInstance = fs_create_instance(logicalDevice);
    if(fsInstance == NULL) {
        printf("Mounting error unrecognaized filesystem\n");
        return;
    }

    size_t pathLength = strlen(path);
    vfs_mount_point_t* mountPoint = kmalloc(sizeof(vfs_mount_point_t));
    mountPoint->logicalDevice = logicalDevice;
    mountPoint->path = kmalloc(pathLength+1);
    memcpy(mountPoint->path, path, pathLength);
    mountPoint->path[pathLength] = 0x0;
    mountPoint->pathLength = pathLength;
    mountPoint->fsInstance = fsInstance;
    for(size_t i = 0; i < VFS_MAX_MOUNT_POINTS; i++) {
        if(mountPoints[i] == NULL) {
            mountPoints[i] = mountPoint;
            printf("%s mounted\n", path);
            return;
        }
    }
    printf("Mounting error\n");
}

void vfs_unmount(const char* path) {
    printf("Unmounting %s\n", path);
    for(size_t i = 0; i < VFS_MAX_MOUNT_POINTS; i++) {
        if(mountPoints[i] != NULL) {
            if(strequal(path, mountPoints[i]->path)) {
                kfree(mountPoints[i]);
                mountPoints[i] = NULL;
                return;
            }
        }
    }
    printf("Unmount error\n");
}

void vfs_unmount_device(block_logical_device_t* logicalDevice) {
    for(size_t i = 0; i < VFS_MAX_MOUNT_POINTS; i++) {
        if(mountPoints[i] != NULL) {
            if(mountPoints[i]->logicalDevice == logicalDevice) {
                printf("Unmounting %s\n", mountPoints[i]->path);
                kfree(mountPoints[i]);
                mountPoints[i] = NULL;
                return;
            }
        }
    }
    printf("Unmount error\n");
}

const vfs_mount_point_t** vfs_list_mount_points() {
    return mountPoints;
}

bool vfs_has_mount_point(const char* path) {
    for(size_t i = 0; i < VFS_MAX_MOUNT_POINTS; i++) {
        if(mountPoints[i] != NULL) {
            if(strequal(path, mountPoints[i]->path)) {
                return true;
            }
        }
    }
    return false;
}

vfs_mount_point_t* vfs_find_mount_point(const char* path) {
    size_t pathLength = strlen(path);
    vfs_mount_point_t* mountPoint;
    for(size_t i = 0; i < VFS_MAX_MOUNT_POINTS; i++) {
        if(mountPoints[i] == NULL) continue;
        mountPoint = mountPoints[i];
        if(mountPoint->pathLength > pathLength) {
            printf("Longer path\n");
            continue; // Mount point has longer path
        }
        if(strncmp(mountPoint->path, path, mountPoint->pathLength) == 0) {
            // We found a matching mountpoint
            return mountPoint;
        } else {
            printf("Paths dont match %s %s", path, mountPoint->path);
        }
    }
    return NULL;
}

int vfs_list_directory(const char* path, fs_dir_entry_t* dirEnt) {
    static vfs_mount_point_t* mountPoint;
    if(path != NULL) {
        mountPoint = vfs_find_mount_point(path);
        if(mountPoint == NULL) {
            printf("Failed to find mount point for %s\n", path);
            return FS_RET_DIRECTORY_NOT_FOUND;
        }
        return fs_list_directory(mountPoint->fsInstance, (char*)((size_t)(path) + mountPoint->pathLength), dirEnt);
    }
    return fs_list_directory(mountPoint->fsInstance, NULL, dirEnt);
}

int vfs_get_dir_entry(const char* path, fs_dir_entry_t* dirEnt) {
    return -1;
}

void vfs_read(const char* path, size_t size, void* buffer) {
    vfs_mount_point_t* mountPoint = vfs_find_mount_point(path);
    if(mountPoint == NULL) return;
    fs_read(mountPoint->fsInstance, (char*)((size_t)(path) + mountPoint->pathLength), size, buffer);
}

void vfs_write(const char* path, size_t size, void* buffer) {
    vfs_mount_point_t* mountPoint = vfs_find_mount_point(path);
    if(mountPoint == NULL) return;
    fs_write(mountPoint->fsInstance, (char*)((size_t)(path) + mountPoint->pathLength), size, buffer);
}