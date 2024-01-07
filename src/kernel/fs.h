//
// Created by Idrol on 14/06/2023.
//
#pragma once

#include <file.h>
#include <cdecl.h>

#define FS_TYPE_UNKNOWN 0x0

struct fs_instance_t;

typedef void*(*fs_create_instance_t)(void*);
typedef DIR (*fs_opendir_t)(fs_instance_t*, const char*);
typedef void (*fs_closedir_t)(fs_instance_t*, DIR dirHandle);
typedef dir_entry_t (*fs_readdir_t)(fs_instance_t*, DIR dirHandle);
typedef  int (*fs_mkdir_t)(fs_instance_t*, const char*);

typedef void(*fs_read_fn_t)(fs_instance_t*, const char*, size_t, void*);
typedef void(*fs_write_fn_t)(fs_instance_t*, const char*, size_t, void*);

struct fs_instance_t {

};

struct fs_driver_t {
    uint8_t  fsType;
    fs_create_instance_t createInstanceFn;
    fs_opendir_t opendirFn;
    fs_closedir_t closedirFn;
    fs_readdir_t readdirFn;
    fs_mkdir_t mkdirFn;
};

__cdecl DIR fs_opendir(const char* pathName);
__cdecl void fs_closedir(DIR dirHandle);
__cdecl dir_entry_t fs_readdir(DIR dirHandle);
__cdecl dir_entry_t fs_listdir(const char* pathName);
__cdecl int fs_mkdir(const char* path);

/*__cdecl FILE fs_open(const char* pathName);
__cdecl void fs_close(const char* pathName);*/

