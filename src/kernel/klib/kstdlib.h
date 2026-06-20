
#pragma once
#include <kernel/include/common.h>


void memset(void* dest, int value, uint32_t count);
void* kmemcpy(void *dest, const void *src, size_t count);