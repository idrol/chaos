//
// Created by Idrol on 13/06/2023.
//
#pragma once

#include <stddef.h>
#include <stdint.h>

typedef uint32_t ino_t;
typedef uint32_t dev_t;
typedef uint32_t fd_t;

#define DT_UNKNOWN 0x0
#define DT_REG     0x1
#define DT_DIR     0x2

#define SEEK_SET 0x0
#define SEEK_CUR 0x1
#define SEEK_END 0x2

struct dirent_t {
    char* name;
    uint8_t nameLength;
    uint8_t type;
    ino_t st_ino; // File serial number (inode num)
    dev_t st_dev; // Device id (logical device)
};

__cdecl int vfs_get_dir_entry(const char* path, size_t size, dirent_t entry);

__cdecl int vfs_rename(const char* oldPath, const char* newPath);
__cdecl int vfs_mkdir(const char* dirPath, )

__cdecl fd_t vfs_open(const char* path);
__cdecl void vfs_close(fd_t fileDescriptor);
__cdecl void vfs_read(fd_t fileDescriptor, size_t size, void* buffer);
__cdecl void vfs_write(fd_t fileDescriptor, size_t size, void* buffer);
__cdecl void vff_lseek(fd_t fileDescriptor, size_t offset, uint8_t seekEnum);