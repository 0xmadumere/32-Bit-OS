// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#pragma once
#include <boot/stage2/include/boot.h>


void bios_putc(int c);
void bios_puts(const char* str);
int bios_ext_read(DAP* dap);
int bios_chs_read(CHS* chs);
uint8_t bios_get_mmap(MemoryMapEntry* mmap, uint8_t max_entries);