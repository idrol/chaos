//
// Created by Idrol on 27/05/2023.
//
#include <stdio.h>
#include <drivers/vfs.h>
#include <string.h>
#include <drivers/memory.h>
#include <path.h>
#include <sh.h>
#include <drivers/ps2_kb.h>

extern "C" void kernel_main() {
    /*DIR* dirStream = vfs_openDir("/boot/grub/");
    if(dirStream != NULL) {
        fs_dir_entry_t* dirEntry = vfs_readDir(dirStream);
        printf("Listing entries in /boot/grub/\n");
        while(dirEntry != NULL) {
            if(dirEntry->fileType == FS_FILE_TYPE_DIRECTORY) {
                printf("Directory: %s \n", dirEntry->name);
            } else {
                printf("File: %s \n", dirEntry->name);
            }

            dirEntry = vfs_readDir(dirStream);
        }
        vfs_closeDir(dirStream);
    } else {
        printf("Failed to open directory stream for /boot/grub/\n");
    }*/
    kernel_sh();
}
