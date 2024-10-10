#pragma once

#include <cdecl.h>

__cdecl int isdigit(int c);
__cdecl int isalpha(int c);
__cdecl int isalnum(int c);
__cdecl int islower(int c);
__cdecl int isupper(int c);
__cdecl int isxdigit(int c);
__cdecl int ispunct(int c);
__cdecl int isprint(int c);
__cdecl int isspace(int c);

__cdecl int toupper(int c);
__cdecl int tolower(int c);