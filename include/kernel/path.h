#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

// Sanitizes the given path ex. "   /boot/memes////test.txt     " will be sanitized to "/boot/memes/test.txt"
// This function modifies the exisiting path and returns a pointer inside the current path
char* path_sanitize(char* path);
char* path_split(char* path);


#ifdef __cplusplus
}
#endif