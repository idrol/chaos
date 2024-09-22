#pragma once

#include <stdint.h>
#include <drivers/device.h>
#include <stdbool.h>
#include <drivers/blockio.h>

#define ATA_DEVICE_LBA48_FLAG 0x1
#define ATA_DEVICE_SLAVE_FLAG 0x2

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Flags
 *   7 Unused
 *   6 Unused
 *   5 Unused
 *   4 Unused
 *   3 Unused
 *   2 Unused
 *   1 Does this device support LBA48
 *   0 Set if this device should select the secondary drive
 */

struct ata_device_entry_struct {
    block_device_t* device;
    uint8_t flags; // 
    uint16_t baseIoPort;
    uint16_t baseControlPort;
    uint32_t numSectorsHigh; // This is only valid if bit 1 in flags is set
    uint32_t numSectorsLow;
};

typedef struct ata_device_entry_struct ata_device_entry_t;

void ata_init_devices();
bool ata_identify_drive(uint16_t port, bool isSlave, uint16_t* buffer);
void ata_read_sectors_pio(ata_device_entry_t* device, uint32_t lba, uint8_t sectors, uint8_t* buffer);
void ata_write_sectors_pio(ata_device_entry_t* device, uint32_t lba, uint8_t sectors, uint16_t* buffer);

// Block I/O
void ata_block_read(block_device_t* device, uint32_t startBlock, uint32_t blockCount, void* buffer);
void ata_block_write(block_device_t* device, uint32_t startBlock, uint32_t blockCount, void* buffer);

#ifdef __cplusplus
}
#endif