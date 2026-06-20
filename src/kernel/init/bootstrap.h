// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#pragma once
#include <kernel/include/common.h>
#include <kernel/include/boot.h>

void enable_paging();
void jump_to_entry(BOOT_INFO* binfo);

typedef struct 
{
    uint32_t base;
    uint32_t next;
    uint32_t end;
} bump_t;

