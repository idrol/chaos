#include <ctype.h>
#include <string.h>

__cdecl bool strequal(const char* str1, const char* str2) {
    uint8_t i = 0;
    char c1 = 0x0;
    while((c1 = str1[i]) != 0x0) {
        if(c1 != str2[i]) return false; // Differing char not equal
        i++;
    }
    if(str1[i] != str2[i]) return false; // Unequal length not equal
    return true; // Same chars and same length must be equal
}

__cdecl size_t strlen(const char* str) {
    size_t len = 0;
    char c1 = 0x0;
    while((c1 = str[len]) != 0x0) {
        len++;
    }
    return len;
}

__cdecl void* memcpy(void* dest, const void* src, size_t n) {
    for(size_t i = 0; i < n; i++) {
        ((char*)dest)[i] = ((const char*)src)[i];
    }
    return dest;
}


__cdecl void* memset(void* str, int c, size_t n) {
    char byte = (char)c;
    for(size_t i = 0; i < n; i++) {
        ((char*)str)[i] = byte;
    }
    return str;
}

__cdecl char* strcat(char* dest, const char* source) {
    size_t sourceLen = strlen(source);
    size_t destLen = strlen(dest);

    for(size_t i = 0; i <= sourceLen; i++) {
        size_t destPos = destLen + i;
        dest[destPos] = source[i];
    }
    dest[destLen+sourceLen] = 0x0; // Add null terminator
    return dest;
}

__cdecl char* strcpy(char* dest, const char* source) {
    size_t srcLen = strlen(source);
    return (char*)memcpy((void*)dest, (const void*)source, srcLen+1); // perform memcpy but include null terminator
}

__cdecl char* strtok(char* str1, const char* delim) {
    static char* searchStr;
    if(str1 != NULL) searchStr = str1;
    char* start = searchStr;

    size_t delimLength = strlen(delim);

    size_t offset = 0;
    bool found = false;
    while(searchStr[offset] != 0x0) {
        char c = searchStr[offset];
        for(size_t i = 0; i < delimLength; i++) {
            if(c == delim[i]) {
                // found delim
                searchStr[offset] = 0x0;
                searchStr = (char*)(((size_t)searchStr) + offset + 1);
                return start;
            }
        }
        offset++;
    }
    if(start[0] != 0x0)
    {
        auto len = strlen(start);
        searchStr = &searchStr[len];
        return start;
    }
    return NULL;
}

__cdecl char* strchr(const char* str, int c) {
    size_t offset = 0;
    while(str[offset] != 0x0) {
        if(str[offset] == c) return (char*)(((size_t)str)+offset);
        offset++;
    }
    return NULL;
}

__cdecl uint32_t strcountchar(const char* str, int c) {
    uint32_t count = 0;
    uint32_t offset = 0;
    
    while(str[offset] != 0x0) {
        if(str[offset] == c) count++;
        offset++;
    }
    return count;
}

__cdecl int32_t strncmp(const char* str1, const char* str2, size_t count) {
    size_t offset = 0;
    while(str1[offset] == str2[offset] && str1[offset] != 0x0) {
        if(offset == count) return 0;
        offset++;
    }
    if(offset == count) return 0;
    return str1[offset]-str2[offset];
}

__cdecl void strtolower(char* str) {
    for(char *p = str; *p; ++p)
        if(isupper(*p)) *p = (char)tolower(*p);
}

__cdecl void strtoupper(char* str)
{
    for(char *p = str; *p; ++p)
    {
        if(islower(*p)) *p = (char)toupper(*p);
    }
}
