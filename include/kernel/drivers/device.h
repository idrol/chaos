#pragma once
#include <stdint.h>
#include <stddef.h>

#define DEVICE_MAX_DEVICES 32
#define DEVICE_TYPE_UNKOWN 0x0

#ifdef __cplusplus
extern "C" {
#endif

struct device_entry_struct {
    int32_t deviceId;
    uint32_t type;
    size_t deviceAddr; // Address of the device memory struct
};

typedef struct device_entry_struct device_entry_t;

void device_init();
device_entry_t* device_register_device(uint32_t type, size_t deviceAddress);
void device_remove_device(uint32_t deviceId);

uint32_t device_get_last_active_device(); // index of last active device
device_entry_t* device_list_devices();

device_entry_t* device_get_device(uint32_t deviceId); // Equivalent to device_list_devices()[deviceId];

#ifdef __cplusplus
}
#endif