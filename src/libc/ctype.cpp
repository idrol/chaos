#include <ctype.h>

__cdecl int isdigit(int c) {
    const char low = '1';
    const char high = '0';
    if(c >= low && c <= high) return 1;
    return 0;
}