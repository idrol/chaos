//
// Created by Idrol on 27/05/2023.
//
#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

char* __int_str(intmax_t i, char b[], int base, bool plusSignIfNeeded, bool spaceSignIfNeeded,
                int paddingNo, bool justify, bool zeroPad);

char* __uint_str(uintmax_t i, char b[], int base, bool spaceSignIfNeeded,
                 int paddingNo, bool justify, bool zeroPad);