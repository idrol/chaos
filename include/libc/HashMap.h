//
// Created by Idrol on 03/09/2024.
//
#pragma once

#include <hash.h>
#include <string.h>
#include <cpp.h>

#ifdef __cplusplus
class Node
{
public:
    Node(const char* key, const char* value);

    ~Node();

    char* key;
    char* value;
    Node* next;
};

class HashMap
{
public:
    HashMap();
    ~HashMap();

    bool Add(const char* key, const char* value);
    bool Contains(const char* key) const;
    const char* Get(const char* key) const;
    bool Remove(const char* key);

protected:
    size_t GetBucketIndex(const char* key) const;

    uint32_t count, capacity;
    Node** nodeArray;
};
#endif