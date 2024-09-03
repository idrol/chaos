//
// Created by Idrol on 03/09/2024.
//
#include <cpp.h>

void* operator new(size_t size) {
    return kmalloc(size);
}

void* operator new[](size_t size) {
    return kmalloc(size);
}

void operator delete(void* p) {
    kfree(p);
}

void operator delete(void* p, size_t size) {
    kfree(p);
}

void operator delete[](void* p) {
    kfree(p);
}