#include <ctype.h>

__cdecl int isdigit(int c) {
    return c >= '0' && c <= '9';
}

__cdecl int islower(int c)
{
    return c >= 'a' && c <= 'z';
}

__cdecl int isupper(int c)
{
    return c >= 'A' && c <= 'Z';
}


__cdecl int isalpha(int c)
{
    return islower(c) || isupper(c);
}

__cdecl int isalnum(int c)
{
    return isalpha(c) || isdigit(c);
}

__cdecl int isxdigit(int c)
{
    return isdigit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

__cdecl int ispunct(int c)
{
    return (c >= ' ' && c <= '/') ||
           (c >= ':' && c <= '@') ||
           (c >= '[' && c <= '`') ||
           (c >= '{' && c <= '~');
}

__cdecl int isprint(int c)
{
    return isalnum(c) || ispunct(c);
}

__cdecl int isspace(int c)
{
    return (c == ' ') ||
           (c >= '\t' && c <= '\r');
}

__cdecl int toupper(int c)
{
    if(!islower(c)) return c;
    return c-32;
}

__cdecl int tolower(int c)
{
    if(!isupper(c)) return c;
    return c+32;
}