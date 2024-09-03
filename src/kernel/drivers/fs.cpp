#include <drivers/fs.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>

fs_driver_t** fsDrivers;

void fs_init() {
    fsDrivers = (fs_driver_t**)kmalloc(sizeof(fs_driver_t*)*16);
    for(size_t i = 0; i < 16; i++) {
        fsDrivers[i] = NULL;
    }
    printf("Initialized fs driver\n");
}

fs_driver_t* fs_register_fs_driver(uint8_t fsType,
                                   fs_create_instance_t createInstanceFn,
                                   fs_openDir_fn_t openDirFn,
                                   fs_closeDir_fn_t closeDirFn,
                                   fs_readDir_fn_t readDirFn,
                                   fs_rewindDir_fn_t rewindDirFn,
                                   fs_seekDir_fn_t seekDirFn,
                                   fs_tellDir_fn_t tellDirFn,
                                   fs_read_fn_t readFn,
                                   fs_write_fn_t writeFn) {
    fs_driver_t* driver = (fs_driver_t*)kmalloc(sizeof(fs_driver_t));
    driver->fsType = fsType;
    driver->createInstanceFn = createInstanceFn;
    driver->openDirFn = openDirFn;
    driver->closeDirFn = closeDirFn;
    driver->readDirFn = readDirFn;
    driver->rewindDirFn = rewindDirFn;
    driver->seekDirFn = seekDirFn;
    driver->tellDirFn = tellDirFn;
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
    fs_instance_t* instance = (fs_instance_t*)kmalloc(sizeof(fs_instance_t));
    instance->fsType = fsType;
    instance->logicalDevice = logicalDevice;
    instance->implPtr = driver->createInstanceFn(logicalDevice);
    return instance;
}

void fs_destroy(fs_instance_t* instance) {
    kfree(instance);
}

DIR* fs_openDir(fs_instance_t* instance, const char* dirName) {
    fs_driver_t* driver = fsDrivers[instance->fsType];
    if(driver == NULL) {
        printf("ERR: Attempted filesystem operation with unregistered filesystem driver\n");
        return NULL;
    }
    return driver->openDirFn(instance, dirName);
}

int fs_closeDir(fs_instance_t* instance, DIR* dirPtr) {
    fs_driver_t* driver = fsDrivers[instance->fsType];
    if(driver == NULL) {
        printf("ERR: Attempted filesystem operation with unregistered filesystem driver\n");
        return -999;
    }
    return driver->closeDirFn(instance, dirPtr);
}

fs_dir_entry_t* fs_readDir(fs_instance_t* instance, DIR* dirPtr) {
    fs_driver_t* driver = fsDrivers[instance->fsType];
    if(driver == NULL) {
        printf("ERR: Attempted filesystem operation with unregistered filesystem driver\n");
        return NULL;
    }
    return driver->readDirFn(instance, dirPtr);
}

void fs_rewindDir(fs_instance_t* instance, DIR* dirPtr) {
    fs_driver_t* driver = fsDrivers[instance->fsType];
    if(driver == NULL) {
        printf("ERR: Attempted filesystem operation with unregistered filesystem driver\n");
        return;
    }
    driver->rewindDirFn(instance, dirPtr);
}

void fs_seekDir(fs_instance_t* instance, DIR* dirPtr, uint64_t dirLocationOffset) {
    fs_driver_t* driver = fsDrivers[instance->fsType];
    if(driver == NULL) {
        printf("ERR: Attempted filesystem operation with unregistered filesystem driver\n");
        return;
    }
    driver->seekDirFn(instance, dirPtr, dirLocationOffset);
}

uint64_t fs_tellDir(fs_instance_t* instance, DIR* dirPtr) {
    fs_driver_t* driver = fsDrivers[instance->fsType];
    if(driver == NULL) {
        printf("ERR: Attempted filesystem operation with unregistered filesystem driver\n");
        return UINT64_MAX;
    }
    return driver->tellDirFn(instance, dirPtr);
}

size_t fs_read(fs_instance_t* instance, int32_t fd, void* buffer, size_t count) {
    fs_driver_t* driver = fsDrivers[instance->fsType];
    if(driver == NULL) {
        printf("ERR: Attempted filesystem operation with unregistered filesystem driver\n");
        return 0;
    }
    //driver->readFn(instance, path, size, ptr);
    return 0;
}

size_t fs_write(fs_instance_t* instance, int32_t fd, const void* buffer, size_t count) {
    fs_driver_t* driver = fsDrivers[instance->fsType];
    if(driver == NULL) {
        printf("ERR: Attempted filesystem operation with unregistered filesystem driver\n");
        return 0;
    }
    //driver->writeFn(instance, path, size, ptr);
    return 0;
}