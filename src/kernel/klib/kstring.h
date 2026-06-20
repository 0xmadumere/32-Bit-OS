// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#pragma once
#include <kernel/include/common.h>

uint16_t kstrlen(const char* str);
char* kstrcpy(char* dest, const char* src);
char* kstrncpy(char* dest, const char* src, size_t count);
int ktolower(int c);
int kstrncimp(const char* str1, const char* str2, size_t count);
char* kstrrev(char* str);
char* kitoa(uint32_t value, char* buff);
void kpad(const char* src, char* dst, int width, char pad_char);
void kutoa_hex(uint32_t val, char* buf, uint32_t uppercase);
char ktoupper(char c);