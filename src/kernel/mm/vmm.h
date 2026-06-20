// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#pragma once
#include <kernel/include/common.h>

// start of kernel address space
#define KVIRT_START 0xC0000000

void* vmm_alloc();
int32_t vmm_map(void* virt, void* phys, uint32_t flags);
void vmm_free(void* virt);
void* vmm_reserve(void);