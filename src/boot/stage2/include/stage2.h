// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#pragma once

#include <boot/stage2/include/boot.h>

#define KERNEL_BUFFER_ADDRESS   0x10000

#define KERNEL_LOAD_ADDR        0x100000
#define KERNEL_SIGNATURE        0x77FF5F80

#define MAX_MM_ENTRIES 32

typedef void (*kernel_entry_t)(BOOT_INFO* info);