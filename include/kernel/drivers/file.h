//
// Created by Idrol on 14/01/2024.
//
#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct file_descriptor_struct {
     uint32_t fd_id = UINT32_MAX;

};

#ifdef __cplusplus
}
#endif