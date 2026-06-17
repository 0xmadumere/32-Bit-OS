// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#pragma once
#include <boot/stage2/include/common.h>



uint16_t strlen(const char* str);
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t count);
int tolower(int c);
int strncimp(const char* str1, const char* str2, size_t count);
char* strrev(char* str);
char* itoa(uint32_t value, char* buff);