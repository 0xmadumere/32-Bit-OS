// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#include <kernel/klib/kstdlib.h>

void memset(void* dest, int value, uint32_t count)
{
    if (count == 0)
        return;

    uint8_t* p = (uint8_t*)dest;

    while (count--)
        *p++ = value;

}

void* kmemcpy(void *dest, const void *src, size_t count)
{
    unsigned char *d = (unsigned char *)dest;
    unsigned char *s = (unsigned char *)src;

    for (size_t i = 0; i < count; i++) 
        d[i] = s[i];

    return dest;
}