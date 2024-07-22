//
// Created by Idrol on 14/01/2024.
//
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct file_descriptor_struct;
typedef file_descriptor_struct file_descriptor_t;

typedef size_t(*file_write_fn_t)(file_descriptor_t* fd, const void* buffer, size_t bufferSize);
typedef size_t(*file_read_fn_t)(file_descriptor_t* fd, const void* buffer, size_t bufferSize);
typedef off_t(*file_lseek)(file_descriptor_t* fd, off_t offset);
typedef int(*file_sync)(file_descriptor_t* fd);

struct file_descriptor_struct {
    uint32_t fd_id = UINT32_MAX;
    file_write_fn_t writeFn = nullptr;
    file_read_fn_t readFn = nullptr;
    file_lseek lseekFn = nullptr;
    file_sync fileSyncFn = nullptr;
    size_t cursor = 0;
    void* implData = nullptr;
};

int close(int fd);
size_t write(int fd, const void* buffer, size_t bufferSize);
size_t read(int fd, const void* buffer, size_t bufferSize);
off_t lseek(int fd, off_t offset);
int fsync(int fd);

file_descriptor_t* file_get_descriptor(int fd);

#ifdef __cplusplus
}
#endif