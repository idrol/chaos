//
// Created by Idrol on 03/09/2024.
//
#include <hash.h>

size_t hash_string(const char* str)
{
    size_t hash = 5381;
    char c;
    while((c = *str++))
    {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}