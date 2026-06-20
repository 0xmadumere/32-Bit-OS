// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#pragma once
#include <kernel/include/common.h>


int32_t ksprintf(char* buf, const char* fmt, ...);
int32_t kprintf(const char* format, ...);
void kputchar(int c);
void kputs(const char* str);