//
// Created by Idrol on 21/09/2024.
//
#pragma once

#include <keys.h>
#include <stdint-gcc.h>

#include <drivers/device.h>

class ScancodeSet
{
public:
    virtual ~ScancodeSet() = default;
    virtual bool AddInput(uint8_t scanCode) = 0;
    virtual uint32_t GetLastMappedKey() = 0;
    virtual uint8_t GetLastKeyAction() = 0;
};

class ScancodeSet1: public ScancodeSet
{
public:
    bool AddInput(uint8_t scanCode) override;
    uint32_t GetLastMappedKey() override;
    uint8_t GetLastKeyAction() override;
    void ClearBuffer();

private:
    size_t bufferPos = 0;
    uint8_t buffer[4] = {0x0, 0x0, 0x0, 0x0};
    uint32_t lastKey = 0x0;
    uint8_t action = 0;
};

struct ps2_keyboard_device_t
{
    device_entry_t* device;
    uint8_t keyStates[KEY_LAST+1];
    ScancodeSet* scancodeSet = nullptr;
    uint32_t bufferPos = 0;
    uint8_t* buffer;
    bool getLineActive = false;
    uint32_t getLineStart = 0;
    uint32_t getLineLength = 0;
};

void ps2_kb_init();
const char* getline();