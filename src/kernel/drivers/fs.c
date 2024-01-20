#include <drivers/fs.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>

fs_driver_t** fsDrivers;

void fs_init() {
    fsDrivers = kmalloc(sizeof(fs_driver_t*)*16);
    for(size_t i = 0; i < 16; i++) {
        fsDrivers[i] = NULL;
    }
    printf("Initialized fs driver\n");
}

fs_driver_t* fs_register_fs_driver(uint8_t fsType, fs_create_instance_t createInstanceFn, fs_get_dir_entry_fn_t getDirEntryFn, fs_list_directory_fn_t lsFn, fs_read_fn_t readFn, fs_write_fn_t writeFn) {
    fs_driver_t* driver = kmalloc(sizeof(fs_driver_t));
    driver->fsType = fsType;
    driver->createInstanceFn = createInstanceFn;
    driver->getDirEntryFn = getDirEntryFn;
    driver->listDirectoryFn = lsFn;
    driver->readFn = readFn;
    driver->writeFn = writeFn;
    fsDrivers[fsType] = driver;
    printf("Registered new filesystem type\n");
    return driver;
}

uint8_t fs_identify(block_logical_device_t* logicalDevice) {
    // TODO identify, hard coded to FAT16
    return 0x1;//FS_TYPE_UNKOWN;
}

fs_instance_t* fs_create_instance(block_logical_device_t* logicalDevice) {
    uint8_t fsType = fs_identify(logicalDevice);
    fs_driver_t* driver = fsDrivers[fsType];
    if(driver == NULL) return NULL;
    fs_instance_t* instance = kmalloc(sizeof(fs_instance_t));
    instance->fsType = fsType;
    instance->logicalDevice = logicalDevice;
    instance->implPtr = driver->createInstanceFn(logicalDevice);
    return instance;
}

void fs_destroy(fs_instance_t* instance) {
    kfree(instance);
}

int fs_list_directory(fs_instance_t* instance, const char* path, fs_dir_entry_t* dirEnt) {
    fs_driver_t* driver = fsDrivers[instance->fsType];
    if(driver == NULL) {
        printf("ERR: Attempted filesystem operation with unregistered filesystem driver\n");
    }
    return driver->listDirectoryFn(instance, path, dirEnt);
}

int fs_get_dir_entry(fs_instance_t* instance, const char* path, fs_dir_entry_t* dirEnt) {
    fs_driver_t* driver = fsDrivers[instance->fsType];
    if(driver == NULL) {
        printf("ERR: Attempted filesystem operation with unregistered filesystem driver\n");
    }
    return driver->getDirEntryFn(instance, path, dirEnt);
}

void fs_read(fs_instance_t* instance, const char* path, size_t size, void* ptr) {
    fs_driver_t* driver = fsDrivers[instance->fsType];
    if(driver == NULL) {
        printf("ERR: Attempted filesystem operation with unregistered filesystem driver\n");
    }
    driver->readFn(instance, path, size, ptr);
}

void fs_write(fs_instance_t* instance, const char* path, size_t size, void* ptr) {
    fs_driver_t* driver = fsDrivers[instance->fsType];
    if(driver == NULL) {
        printf("ERR: Attempted filesystem operation with unregistered filesystem driver\n");
    }
    driver->writeFn(instance, path, size, ptr);
}