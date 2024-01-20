#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <tty.h>

__cdecl int putchar(const int character) {
    //if(!Terminal::IsInitialized()) return -1; // Check that terminal has been initialized
    tty_put_char(character);
    return character;
}


__cdecl int puts(const char* str) {
    //if(!Terminal::IsInitialized()) return -1;
    size_t len = strlen(str);
    for(size_t i = 0; i <= len; i++) {
        putchar(str[i]);
    }
    putchar('\n');
    return 0;
}
 
__cdecl __attribute__ ((format (printf, 1, 2))) int printf (const char* format, ...) {
    va_list list;
    va_start (list, format);
    int i = vprintf (format, list);
    va_end (list);
    return i;
}

__cdecl __attribute__ ((format (sprintf, 1, 2))) int sprintf(char* buffer, const char* format, ...) {
    va_list list;
    va_start (list, format);
    int i = vsprintf (buffer, format, list);
    va_end (list);
    return i;
}

__cdecl __attribute__ ((format (spnrintf, 1, 2))) int snprintf(char* buffer, size_t bufferSize, const char* format, ...) {
    va_list list;
    va_start (list, format);
    int i = vsnprintf (buffer, bufferSize, format, list);
    va_end (list);
    return i;
}