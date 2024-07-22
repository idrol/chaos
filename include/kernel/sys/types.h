//
// Created by Idrol on 21/01/2024.
//
#pragma once

#if defined __x86_64__
typedef long long int off_t;
#else
typedef long int off_t;
#endif