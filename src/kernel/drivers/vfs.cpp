#include <drivers/vfs.h>
#include <drivers/thread.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>

struct vfs_process_file_descriptors_struct {
    
};



struct vfs_directory_stream_struct {
    DIR* fs_directoryStream;
    fs_instance_t* instance;
};

typedef struct vfs_directory_stream_struct vfs_directory_stream_t;
typedef struct vfs_process_file_descriptors_struct vfs_process_file_descriptors_t;

vfs_process_file_descriptors_t** processFD = NULL;
vfs_process_file_descriptors_t* currentProcessFD = NULL;
vfs_mount_point_t** mountPoints;

void vfs_init() {
    processFD = (vfs_process_file_descriptors_t**)kmalloc(sizeof(vfs_process_file_descriptors_t*)*MAX_PROCESSES);
    processFD[0] = (vfs_process_file_descriptors_t*)kmalloc(sizeof(vfs_process_file_descriptors_t));
    currentProcessFD = processFD[0];
    mountPoints = (vfs_mount_point_t**)kmalloc(sizeof(vfs_mount_point_t*)*VFS_MAX_MOUNT_POINTS);
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
    vfs_mount_point_t* mountPoint = (vfs_mount_point_t*)kmalloc(sizeof(vfs_mount_point_t));
    mountPoint->logicalDevice = logicalDevice;
    mountPoint->path = (char*)kmalloc(pathLength+1);
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

vfs_mount_point_t** vfs_list_mount_points() {
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
            printf("Paths dont match %s %s\n", path, mountPoint->path);
        }
    }
    return NULL;
}

DIR* vfs_openDir(const char* path) {
    if(path == NULL) return NULL;
    vfs_mount_point_t* mountPoint = vfs_find_mount_point(path);
    if(mountPoint == NULL) {
        printf("Failed to find mount point for %s\n", path);
        return NULL;
    }
    const char* relativePath = (const char*)((size_t)(path) + mountPoint->pathLength);
    DIR* directoryStream = fs_openDir(mountPoint->fsInstance, relativePath);

    if(directoryStream == NULL) return NULL;

    vfs_directory_stream_t* vfsDirectoryStream = (vfs_directory_stream_t*)kmalloc(sizeof(vfs_directory_stream_t));
    vfsDirectoryStream->fs_directoryStream = directoryStream;
    vfsDirectoryStream->instance = mountPoint->fsInstance;

    return vfsDirectoryStream;
}

int vfs_closeDir(DIR* dirPtr) {
    if(dirPtr == NULL) return FS_RET_BAD_DIR_PTR;
    vfs_directory_stream_t* vfsDirectoryStream = (vfs_directory_stream_t*)dirPtr;
    int retResult = fs_closeDir(vfsDirectoryStream->instance, vfsDirectoryStream->fs_directoryStream);
    kfree(vfsDirectoryStream);
    return retResult;
}

fs_dir_entry_t* vfs_readDir(DIR* dirPtr) {
    if(dirPtr == NULL) return NULL;
    vfs_directory_stream_t* vfsDirectoryStream = (vfs_directory_stream_t*)dirPtr;
    return fs_readDir(vfsDirectoryStream->instance, vfsDirectoryStream->fs_directoryStream);
}

void vfs_rewindDir(DIR* dirPtr) {
    if(dirPtr == NULL) return;
    vfs_directory_stream_t* vfsDirectoryStream = (vfs_directory_stream_t*)dirPtr;
    fs_rewindDir(vfsDirectoryStream->instance, vfsDirectoryStream->fs_directoryStream);
}

void vfs_seekDir(DIR* dirPtr, uint64_t dirLocationOffset) {
    if(dirPtr == NULL) return;
    vfs_directory_stream_t* vfsDirectoryStream = (vfs_directory_stream_t*)dirPtr;
    fs_seekDir(vfsDirectoryStream->instance, vfsDirectoryStream->fs_directoryStream, dirLocationOffset);
}

uint64_t vfs_tellDir(DIR* dirPtr) {
    if(dirPtr == NULL) return FS_RET_BAD_DIR_PTR;
    vfs_directory_stream_t* vfsDirectoryStream = (vfs_directory_stream_t*)dirPtr;
    return fs_tellDir(vfsDirectoryStream->instance, vfsDirectoryStream->fs_directoryStream);
}

size_t vfs_read(int32_t fd, void* buffer, size_t count) {
    /*vfs_mount_point_t* mountPoint = vfs_find_mount_point(path);
    if(mountPoint == NULL) return;
    fs_read(mountPoint->fsInstance, (char*)((size_t)(path) + mountPoint->pathLength), size, buffer);*/
    return 0;
}

size_t vfs_write(int32_t fd, const void* buffer, size_t count) {
    /*vfs_mount_point_t* mountPoint = vfs_find_mount_point(path);
    if(mountPoint == NULL) return;
    fs_write(mountPoint->fsInstance, (char*)((size_t)(path) + mountPoint->pathLength), size, buffer);*/
    return 0;
}