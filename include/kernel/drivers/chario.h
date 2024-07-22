//
// Created by Idrol on 27/01/2024.
//
#pragma once

#include <drivers/device.h>

#ifdef __cplusplus
extern "C" {
#endif

    struct char_device_struct;
    typedef struct char_device_struct char_device_t;

    typedef void(*char_device_read_t)(char_device_t*, size_t, size_t, char*);
    typedef void(*char_device_write_t)(char_device_t*, size_t, size_t, const char*);

    struct char_device_struct {
        device_entry_t* device;
        size_t hwDevicePtr;
        char_device_read_t charDeviceReadImpl;
        char_device_write_t charDeviceWriteImpl;
    };


    //char_device_t* char_device_create(size_t hwDevicePtr, )

#ifdef __cplusplus
}
#endif
