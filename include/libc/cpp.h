//
// Created by Idrol on 03/09/2024.
//
#pragma once

#include <drivers/memory.h>

#ifdef __cplusplus
void *operator new(size_t size);
void *operator new[](size_t size);
void operator delete(void *p);
void operator delete(void *p, size_t size);
void operator delete[](void *p);
#endif