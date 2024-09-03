//
// Created by Idrol on 03/09/2024.
//
#include <HashMap.h>

Node::Node(const char* key, const char* value)
{
    size_t keyLen = strlen(key)+1;
    this->key = (char*)kmalloc(keyLen);
    memcpy(this->key, key, keyLen);

    size_t valLen = strlen(value)+1;
    this->value = (char*)kmalloc(valLen);
    memcpy(this->value, value, valLen);
    this->next = NULL;
}

Node::~Node()
{
    kfree(this->key);
    kfree(this->value);
}

HashMap::HashMap()
{
    this->capacity = 100;
    this->count = 0;
    this->nodeArray = (Node**)kmalloc(sizeof(Node*)*this->capacity);
}

HashMap::~HashMap()
{
    for(size_t i = 0; i<this->capacity;i++)
    {
        auto currentNode = this->nodeArray[i];
        while(currentNode != NULL)
        {
            auto tmpNode = currentNode;
            currentNode = tmpNode->next;
            delete tmpNode;
        }
    }
    kfree(this->nodeArray);
}

size_t HashMap::GetBucketIndex(const char* key) const {
    size_t hash = hash_string(key);
    return hash%this->capacity;
}

bool HashMap::Contains(const char* key) const
{
    auto bucketIndex = this->GetBucketIndex(key);
    if(this->nodeArray[bucketIndex] == NULL) return false;
    auto currentNode = this->nodeArray[bucketIndex];
    size_t keyLen = strlen(key);
    while(currentNode != NULL)
    {
        if(strncmp(key, currentNode->key, keyLen) == 0) return true;
        currentNode = currentNode->next;
    }
    return false;
}

bool HashMap::Add(const char* key, const char* value)
{
    if(Contains(key)) return false;
    auto bucketIndex = this->GetBucketIndex(key);
    if(this->nodeArray[bucketIndex] == NULL)
    {
        this->nodeArray[bucketIndex] = new Node(key, value);
    } else
    {
        auto currentNode = this->nodeArray[bucketIndex];
        while(currentNode->next != NULL)
        {
            currentNode = currentNode->next;
        }
        currentNode->next = new Node(key, value);
    }
    this->count++;
    return true;
}

const char* HashMap::Get(const char* key) const
{
    auto bucketIndex = this->GetBucketIndex(key);
    if(this->nodeArray[bucketIndex] == NULL) return NULL;
    auto currentNode = this->nodeArray[bucketIndex];
    size_t keyLen = strlen(key);
    while(currentNode != NULL)
    {
        if(strncmp(key, currentNode->key, keyLen) == 0) return currentNode->value;
        currentNode = currentNode->next;
    }
    return NULL;
}

bool HashMap::Remove(const char* key)
{
    auto bucketIndex = this->GetBucketIndex(key);
    if(this->nodeArray[bucketIndex] == NULL) return false;
    Node* lastNode = NULL;
    auto currentNode = this->nodeArray[bucketIndex];
    size_t keyLen = strlen(key);
    while(currentNode != NULL)
    {
        if(strncmp(key, currentNode->key, keyLen) == 0)
        {
            if(lastNode != NULL)
            {
                lastNode->next = currentNode->next;
            } else
            {
                // First element
                this->nodeArray[bucketIndex] = currentNode->next;
            }
            delete currentNode;
            this->count++;
            return true;
        }
        lastNode = currentNode;
        currentNode = currentNode->next;
    }
    return false;
}





