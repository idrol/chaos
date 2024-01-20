//
// Created by Idrol on 27/05/2023.
//
#include <stdio.h>

extern "C" void kernel_main() {
    for(int i = 0; i < 10; i++) {
        printf("Num %i\n", i);
    }
}