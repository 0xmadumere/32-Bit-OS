// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#pragma once
#include <kernel/include/common.h>
#include <kernel/include/boot.h>


#define PAGE_SIZE       4096
#define MMAP_TYPE_FREE  1


void pmm_init(MemoryMapEntry* mmap, uint32_t entries);
void pmm_print_stats(void);
uint32_t pmm_total_frames(void);
uint32_t pmm_used_frames(void);
void* pmm_alloc(void);
void pmm_free(void* addr);
void* pmm_alloc_region(void* base, uint32_t length);
void pmm_free_region(void* base, uint32_t length);