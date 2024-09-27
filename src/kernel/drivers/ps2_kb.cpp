//
// Created by Idrol on 21/09/2024.
//
#include <drivers/ps2_kb.h>
#include <drivers/interrupts.h>
#include <drivers/io.h>
#include <stdio.h>
#include <keys.h>
#include <drivers/memory.h>
#include <string.h>
#include <tty.h>

ps2_keyboard_device_t* kb_device = nullptr;
char getLineBuffer[4096];

uint32_t CodeSet1Buffer[128] = {
    0x0, KEY_ESCAPE, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5,
    KEY_6, KEY_7, KEY_8, KEY_9, KEY_0, KEY_MINUS, KEY_EQUAL, KEY_BACKSPACE, KEY_TAB,
    KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P, KEY_LEFT_BRACKET, KEY_RIGHT_BRACKET,
    KEY_ENTER, KEY_LEFT_CONTROL, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J, KEY_K, KEY_L, KEY_SEMICOLON,
    0x0 /* Single quote */, 0x0 /* Back tick */, KEY_LEFT_SHIFT, KEY_BACKSLASH, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B,
    KEY_N, KEY_M, KEY_COMMA, KEY_PERIOD, KEY_SLASH, KEY_RIGHT_SHIFT, KEY_KP_MULTIPLY, KEY_LEFT_ALT, KEY_SPACE, KEY_CAPS_LOCK,
    KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_NUM_LOCK, KEY_SCROLL_LOCK, KEY_KP_7,
    KEY_KP_8, KEY_KP_9, KEY_KP_SUBTRACT, KEY_KP_4, KEY_KP_5, KEY_KP_6, KEY_KP_ADD, KEY_KP_1, KEY_KP_2, KEY_KP_3, KEY_KP_0, KEY_KP_DECIMAL,
    0x0, 0x0, 0x0, KEY_F11, KEY_F12
};

void ScancodeSet1::ClearBuffer()
{
    for(auto i = 0; i < 4; i++)
    {
        buffer[i] = 0x0;
    }
    bufferPos = 0;
}


bool ScancodeSet1::AddInput(uint8_t scanCode)
{
    if(bufferPos == 3)
    {
        if(buffer[2] == 0xE0)
        {
            if(scanCode == 0x37)
            {
                ClearBuffer();
                lastKey = KEY_PRINT_SCREEN;
                action = ACTION_PRESS;
                return true;
            } else if(scanCode == 0xAA)
            {
                ClearBuffer();
                lastKey = KEY_PRINT_SCREEN;
                action = ACTION_RELEASE;
                return true;
            }
        }
    } else if(bufferPos == 2)
    {
        if(scanCode == 0xE0 && (buffer[1] == 0x2A || buffer[1] == 0xB7))
        {
            buffer[2] = scanCode;
            bufferPos++;
            return false;
        } else if(buffer[1] == 0x1D && scanCode == 0x45)
        {
            buffer[2] = scanCode;
            bufferPos++;
            return false;
        }
    } else if(bufferPos == 1)
    {
        if(buffer[0] == 0xE0)
        {
            if(scanCode == 0x2A || scanCode == 0xB7)
            {
                buffer[1] = scanCode;
                bufferPos++;
                return false;
            }
        } else if(buffer[0] == 0xE1 && scanCode == 0x1D)
        {
            buffer[1] = scanCode;
            bufferPos++;
            return false;
        }
    } else if(bufferPos == 0)
    {
        if(scanCode == 0xE0 || scanCode == 0xE1) {
            buffer[0] = scanCode;
            bufferPos++;
            return false;
        } else
        {
            uint32_t reducedKey = scanCode;
            if(scanCode >= 0x80 && scanCode <= 0xD8)
            {
                reducedKey = scanCode-0x80;
            }
            if(CodeSet1Buffer[reducedKey] != 0x0)
            {
                lastKey = CodeSet1Buffer[reducedKey];
                if(scanCode < 0x81)
                {
                    action = ACTION_PRESS;
                } else
                {
                    action = ACTION_RELEASE;
                }
                return true;
            }
        }
    }
}

uint32_t ScancodeSet1::GetLastMappedKey()
{
    return lastKey;
}

uint8_t ScancodeSet1::GetLastKeyAction()
{
    return action;
}

void ps2_kb_irq_handler(uint32_t irq, device_entry_t* device)
{
    //auto kb_device = (ps2_keyboard_device_t*)(device->deviceAddr);
    uint8_t scanCode = io_inb(0x60);
    if(kb_device->scancodeSet->AddInput(scanCode))
    {
        auto key = kb_device->scancodeSet->GetLastMappedKey();
        auto action = kb_device->scancodeSet->GetLastKeyAction();
        auto currentAction = kb_device->keyStates[key];
        if((currentAction == ACTION_PRESS || currentAction == ACTION_REPEAT))
        {
            if(action == ACTION_PRESS)
            {
                action = ACTION_REPEAT;
            }
        }
        if(action == ACTION_PRESS || action == ACTION_REPEAT)
        {
            uint8_t letter = key;
            if(key == KEY_BACKSPACE)
            {
                if(kb_device->bufferPos == 0)
                {
                    kb_device->bufferPos = 4095;
                } else {
                    kb_device->bufferPos--;
                }
                kb_device->buffer[kb_device->bufferPos] = 0x0;
                if(kb_device->getLineActive)
                {

                    if(kb_device->getLineLength > 0)
                    {
                        kb_device->getLineLength--;
                    }

                }
            } else
            {
                if(key == KEY_ENTER || key == KEY_KP_ENTER)
                {
                    letter = '\n';
                }

                kb_device->buffer[kb_device->bufferPos] = letter;
                if(kb_device->getLineActive)
                {

                    kb_device->getLineLength++;
                }
                kb_device->bufferPos++;
                if(kb_device->bufferPos >= 4096) kb_device->bufferPos = 0;
                if(letter == '\n')
                {
                    kb_device->getLineActive = false; // disable getline
                }
            }

        }

        kb_device->keyStates[key] = action;
    }
    //printf("Keyboard 0x%X\n", scanCode);
}

void ps2_kb_init()
{
    memset(getLineBuffer, 0x0, 4096);
    kb_device = new ps2_keyboard_device_t();
    kb_device->device = device_register_device(DEVICE_TYPE_CHAR, (size_t)&kb_device);
    kb_device->scancodeSet = new ScancodeSet1();
    kb_device->bufferPos = 0;
    kb_device->buffer = (uint8_t*)kmalloc(4096);
    memset(kb_device->buffer, 0x0, 4096);
    interrupt_register_handler(33, &ps2_kb_irq_handler, kb_device->device);
    interrupt_enable_line(33);
}

const char* getline()
{
    tty_put_char('>');
    tty_put_char(' ');
    kb_device->getLineActive = true;
    kb_device->getLineStart = kb_device->bufferPos;
    kb_device->getLineLength = 0;
    uint32_t lastLength = 0;
    while(kb_device->getLineActive)
    {
        io_halt();// wait until getline deactivates
        if(kb_device->getLineLength > lastLength)
        {
            // Add char
            tty_put_char(kb_device->buffer[kb_device->getLineStart+(kb_device->getLineLength-1)]);
        } else if(kb_device->getLineLength < lastLength)
        {
            tty_put_char('\b');
            // Remove char
        }
        lastLength = kb_device->getLineLength;
    }
    memset(getLineBuffer, 0x0, 4096);
    int lineBufferOffset = 0;
    for(int i = kb_device->getLineStart; i < kb_device->getLineStart+kb_device->getLineLength; i++)
    {
        int actualIndex = i%4096;
        getLineBuffer[lineBufferOffset] = kb_device->buffer[actualIndex];

        lineBufferOffset++;
    }
    getLineBuffer[kb_device->getLineLength-1] = 0x0;
    return getLineBuffer;
}
