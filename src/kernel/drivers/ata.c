#include <drivers/ata.h>
#include <drivers/io.h>

#include <stdio.h>
#include <memory.h>
#include <stdbool.h>

#define ATA_PRIMARY_BUS_IO 0x1F0
#define ATA_SECONDARY_BUS_IO 0x170
#define ATA_PRIMARY_BUS_CONTROL 0x3F6
#define ATA_SECONDARY_BUS_CONTROL 0x376

#define ATA_ERROR_FEATURE_REG_OFFSET 0x1
#define ATA_SECTOR_COUNT_REG_OFFSET 0x2
#define ATA_LBA_LOW_REG_OFFSET 0x3
#define ATA_LBA_MID_REG_OFFSET 0x4
#define ATA_LBA_HIG_REG_OFFSET 0x5
#define ATA_DRIVE_HEAD_REG_OFFSET 0x6
#define ATA_STATUS_REG_OFFSET 0x7


#define ATA_STATUS_ERROR 0x01
#define ATA_STATUS IDX 0x02 // Will always be 0
#define ATA_STATUS_CORRECTED 0x04 // Will always be 0
#define ATA_STATUS_DRQ 0x08 // Drive has data to transfer / ready to accept PIO data
#define ATA_STATUS_SERVICE 0x10 // Overlapped service request
#define ATA_STATUS_DRIVE_FAULT 0x20
#define ATA_STATUS_READY 0x40
#define ATA_STATUS_BUSY 0x80

#define ATA_COMMAND_READ 0x20
#define ATA_COMMAND_WRITE 0x30
#define ATA_COMMAND_FLUSH 0xE7
#define ATA_COMMAND_IDENTIFY 0xEC

#define ATA_IDENT_MODEL        54

void ata_wait_busy() {
    while(io_inb(0x1F7)&ATA_STATUS_BUSY);
}

bool ata_wait_for_data_ready() { // POLL
    for(uint8_t i = 0; i < 4; i++) {
        io_inb(0x3F6);
    }
    while(true) {
        uint8_t status = io_inb(0x1F7);
        if(status & ATA_STATUS_DRQ) {
            //printf("%2x drq\n", status);
            return true;
        }
        if(status & ATA_STATUS_ERROR) {
            printf("ATA Read error\n");
            return false;
        }
        if(status & ATA_STATUS_DRIVE_FAULT){
            printf("ATA Drive fault\n");
            return false;
        }
    };
}

void ata_flush_cache() {
    io_outb(0x1F7, ATA_COMMAND_FLUSH);
    ata_wait_busy();
}

void ata_select_drive(ata_device_entry_t* ataDevice, uint32_t lba) {
    // Select master driver on primary bus
    uint8_t slaveBit = ataDevice->flags&ATA_DEVICE_SLAVE_FLAG;
    io_outb(ataDevice->baseIoPort + ATA_DRIVE_HEAD_REG_OFFSET, 0xE0 | (slaveBit << 4) | ((lba >> 24) & 0x0F));
}

bool ata_identify_drive(uint16_t port, bool isSlave, uint16_t* buffer) {
    uint8_t driveNum = 0xE0;
    if(isSlave) driveNum = 0xF0;
    io_outb(port + ATA_DRIVE_HEAD_REG_OFFSET, driveNum);
    io_outb(port + ATA_SECTOR_COUNT_REG_OFFSET, 0x0);
    io_outb(port + ATA_LBA_LOW_REG_OFFSET, 0x0);
    io_outb(port + ATA_LBA_MID_REG_OFFSET, 0x0);
    io_outb(port + ATA_LBA_HIG_REG_OFFSET, 0x0);
    io_outb(port + ATA_STATUS_REG_OFFSET, ATA_COMMAND_IDENTIFY);
    uint8_t status = io_inb(port + ATA_STATUS_REG_OFFSET);
    if(status) {
        while(io_inb(port + ATA_STATUS_REG_OFFSET)&ATA_STATUS_BUSY);
        while(true) {
            status = io_inb(port + ATA_STATUS_REG_OFFSET);
            if(status & ATA_STATUS_ERROR) {
                //printf("Error identifying drive\n");
                return false;
            }
            if(status & ATA_STATUS_DRQ) {
                //printf("Drive is online\n");
                break;
            }
        }
        for(uint16_t i = 0; i < 256; i++) {
            uint16_t data = io_inw(port);
            buffer[i] = data;
        }
        return true;
    }
    //printf("Identify error status was null\n");
    return false;
}

/**
 * @brief Buffer must be of size (sizeof(uint16_t)*256*sectors)
 * 
 * @param lba 
 * @param sectors 
 * @param buffer 
 */
void ata_read_sectors_pio(ata_device_entry_t* device, uint32_t lba, uint8_t sectors, uint8_t* buffer) {
    //printf("Reading lba %#08X %#02X\n", lba, sectors);
	/*ata_wait_busy();
    io_outb(0x3F6, 0x04); // Software reset device
    io_wait();
	ata_wait_busy();
    io_outb(0x3F6, 0x02); // Disable interrupts*/
    ata_wait_busy();
    uint8_t isSlave = 0x0; 
    io_outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
    io_outb(0x1F1, 0x00); // This should be needed but wastes alot of cpu and according to https://wiki.osdev.org/ATA_PIO_Mode#Master.2FSlave_Drives it is ignored
    io_outb(0x1F2, sectors);
    io_outb(0x1F3, (uint8_t)lba);
    io_outb(0x1F4, (uint8_t)(lba >> 8));
    io_outb(0x1F5, (uint8_t)(lba >> 16));
    io_outb(0x1F7, ATA_COMMAND_READ);
    //printf("%i\n", sizeof(uint16_t));
    for(uint8_t sector = 0; sector < sectors;sector++) {
        uint32_t sectorOffset = sector * 256;
        ata_wait_busy();
        if(!ata_wait_for_data_ready()) return;
        for(uint16_t word = 0; word < 256; word++) {
            uint16_t index = word+sectorOffset;
            //printf("%i,", index);
            uint16_t data = io_inw(0x1F0);
            /*printf("%02X", (uint8_t)data);
            printf("%02X ",  (uint8_t)(data>>8));*/
            buffer[index*2] = (uint8_t)(data);
            buffer[index*2+1] = (uint8_t)(data>>8);
            //buffer[index] = ((data&0xFF)<<8)+(data>>8);
            //printf("%08X ", (uint32_t)&buffer[index*2]);
            //data = port_word_in(0x1F0);
        }
    }
    //printf("\n");
    //printf("%08X\n", (uint32_t)buffer);
}

void ata_write_sectors_pio(ata_device_entry_t* device, uint32_t lba, uint8_t sectors, uint16_t* buffer) {
    ata_wait_busy();
    uint8_t isSlave = 0x0;
    io_outb(0x1F6, 0xE0 | (isSlave << 4) | ((lba >> 24) & 0x0F));
    //io_outb(0x1F1, 0x00); // This should be needed but wastes alot of cpu and according to https://wiki.osdev.org/ATA_PIO_Mode#Master.2FSlave_Drives it is ignored
    io_outb(0x1F2, sectors);
    io_outb(0x1F3, lba & 0xFF);
    io_outb(0x1F4, (lba >> 8) & 0xFF);
    io_outb(0x1F5, (lba >> 16) & 0xFF);
    io_outb(0x1F7, ATA_COMMAND_WRITE);

     for(uint8_t sector = 0; sector < sectors;sector++) {
        uint32_t sectorOffset = sector * 256;
        ata_wait_busy();
        if(!ata_wait_for_data_ready()) return;
        for(uint16_t word = 0; word < 256; word++) {
            io_outw(0x1F0, buffer[word+sectorOffset]);
            //ata_flush_cache();
        }
    }
}

block_device_t* ata_create_device_object(uint16_t port, bool isSlave, uint16_t* buffer) {
    if(ata_identify_drive(port, isSlave, buffer)) {
        bool hasLBA48 = buffer[83] & 0x400;
        uint32_t maxSectorsLow;
        uint32_t maxSectorsHigh = 0x0;
        if(hasLBA48) {
            maxSectorsHigh = buffer[102];
            maxSectorsHigh |= buffer[103] << 16;
            maxSectorsLow = buffer[100];
            maxSectorsLow |= buffer[101] << 16;
        } else {
            maxSectorsLow = buffer[60];
            maxSectorsLow |= buffer[61] << 16;
        }
        ata_device_entry_t* ataDevice = (ata_device_entry_t*)kmalloc(sizeof(ata_device_entry_t));
        ataDevice->baseIoPort = port;
        if(port == ATA_PRIMARY_BUS_IO) {
            ataDevice->baseControlPort = ATA_PRIMARY_BUS_CONTROL;
        } else {
            ataDevice->baseControlPort = ATA_SECONDARY_BUS_CONTROL;
        }
        ataDevice->numSectorsHigh = maxSectorsHigh;
        ataDevice->numSectorsLow = maxSectorsLow;
        char* mode = "LBA28";
        if(hasLBA48) mode = "LBA48";
        printf("Found ATA Device Mode %s Max Sectors %lX%lX\n", mode, maxSectorsHigh, maxSectorsLow);
        ataDevice->device = block_device_create((size_t)ataDevice, 512, &ata_block_read, &ata_block_write);
        //ataDevice->device = device_register_device(DEVICE_TYPE_ATA, (size_t)&ataDevice);
        return ataDevice->device;
    }
    return NULL;
}

void ata_irq_handler(uint32_t irq, device_entry_t* device)
{
    printf("Ata irq handler called %i\n", irq);
}

void ata_init_devices() {
    // Used as byte buffer 
    uint8_t* identifyBuffer = (uint8_t*)kmalloc(512);

    /*interrupt_register_handler(46, &ata_irq_handler, nullptr);
    interrupt_register_handler(47, &ata_irq_handler, nullptr);
    interrupt_enable_line(46);
    interrupt_enable_line(47);*/

    block_device_t* primaryMaster = ata_create_device_object(ATA_PRIMARY_BUS_IO, false, (uint16_t*)identifyBuffer);
    if(primaryMaster) {
        printf("Found primary master ATA drive\n");
    }
    block_device_t* primarySlave = ata_create_device_object(ATA_PRIMARY_BUS_IO, true, (uint16_t*)identifyBuffer);
    if(primarySlave) {
        printf("Found primary slave ATA drive\n");
    }
    block_device_t* secondaryMaster = ata_create_device_object(ATA_SECONDARY_BUS_IO, false, (uint16_t*)identifyBuffer);
    if(secondaryMaster) {
        printf("Found secondary master ATA drive\n");
    }
    block_device_t* secondarySlave =ata_create_device_object(ATA_SECONDARY_BUS_IO, true, (uint16_t*)identifyBuffer);
    if(secondarySlave) {
        printf("Found secondary slave ATA drive\n");
    }
    printf("Initialized ATA driver\n");
}

void ata_block_read(block_device_t* device, uint32_t blockOffset, uint32_t blockCount, void* buffer) {
    ata_device_entry_t* ataDevice = (ata_device_entry_t*)device->hwDevicePtr;
    ata_read_sectors_pio(ataDevice, blockOffset, blockCount, (uint8_t*)buffer);
}

void ata_block_write(block_device_t* device, uint32_t blockOffset, uint32_t blockCount, void* buffer) {
    ata_device_entry_t* ataDevice = (ata_device_entry_t*)device->hwDevicePtr;
    ata_write_sectors_pio(ataDevice, blockOffset, blockCount, (uint16_t*)buffer);
}