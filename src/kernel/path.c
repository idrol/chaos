#include "path.h"
#include <stdio.h>
#include <memory.h>

char* path_sanitize(char* path) {
    size_t len = strlen(path);
    size_t offset = 0;
    size_t start = 0;
    bool foundFirstChar = false;
    uint8_t lastChar = 0x0;
    if(len < 2) {
        if(len < 1) return NULL;
        if(path[0] == '/') return path;
        return NULL;
    }
    while(path[offset] != 0x0) {
        if(!foundFirstChar) {
            uint8_t c = path[offset];
            if(!isspace(c)) {
                foundFirstChar = true;
                continue;
            } else {
                offset++;
                start++;
                continue;
            }
        }
        if(lastChar == '/' && path[offset] == '/') {
            size_t copyOffset = offset;
            while(path[copyOffset] != 0x0) {
                path[copyOffset-1] = path[copyOffset];
                copyOffset++;
            }
            path[copyOffset] = 0x0;
            continue;
        } else {
            lastChar = path[offset];
        }
        offset++;
    }
    offset--;
    while(isspace(path[offset]) || path[offset] == '/') {
        offset--;
    }
    path[offset+1] = 0x0;
    return &path[start];
}

char* path_split(char* path) {
    static bool lastReturnWasNull;
    static size_t end;
    static char* lastToken;
    if(lastReturnWasNull) return NULL;
    if(path != NULL) {
        path = path_sanitize(path);
        if(path[0] == '/') path = &path[1];
        lastReturnWasNull = false;
        lastToken = NULL;
    }
    char* token = strtok(path, "/");
    if(token == NULL) {
        lastReturnWasNull = true;
        if(lastToken == NULL) {
            // Only one directory level
            return path;
        }
        size_t tokenLen = strlen(lastToken);
        return &lastToken[tokenLen+1];
    }
    lastToken = token;
    return token;
}