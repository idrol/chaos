#include <drivers/device.h>
#include <memory.h>
#include <stdio.h>

device_entry_t* devices;

void device_init() {
    devices = (device_entry_t*)kmalloc(sizeof(device_entry_t)*DEVICE_MAX_DEVICES);
    for(uint32_t id = 0; id < DEVICE_MAX_DEVICES; id++) {
        devices[id].deviceId = -1; // Set device id invalid to indicate device entry is not in use
    }
    printf("Device driver initalized\n");
}

uint32_t device_find_next_unused_device_id(){
    for(uint32_t id = 0; id < DEVICE_MAX_DEVICES; id++) {
        if(devices[id].deviceId == -1) return id;
    } 
    return DEVICE_MAX_DEVICES;
}

device_entry_t* device_register_device(uint32_t type, size_t deviceAddress) {
    uint32_t deviceId = device_find_next_unused_device_id();
    if(deviceId == DEVICE_MAX_DEVICES) return NULL;
    device_entry_t* device = &devices[deviceId];
    device->deviceId = deviceId;
    device->deviceAddr = deviceAddress;
    device->type = type;
    return device;
}

void device_remove_device(uint32_t deviceId) {
    if(deviceId >= DEVICE_MAX_DEVICES) return;
    device_entry_t* device = &devices[deviceId];
    device->deviceId = -1;
    device->type = DEVICE_TYPE_UNKOWN;
    device->deviceAddr = 0x0;
}

uint32_t device_get_last_active_device() {
    for(uint32_t id = DEVICE_MAX_DEVICES-1; id >= 0; id--) {
        if(devices[id].deviceId != -1) return id;    
    }
    return DEVICE_MAX_DEVICES; // No active devices
}

device_entry_t* device_list_devices() {
    return devices;
}

device_entry_t* device_get_device(uint32_t deviceId) {
    if(deviceId < 0 || deviceId >= DEVICE_MAX_DEVICES) return NULL;
    return &devices[deviceId];
}