// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#pragma once
#include <kernel/include/common.h>


uint32_t kprintf(const char* format, ...);
void kputchar(int c);
void kputs(const char* str);