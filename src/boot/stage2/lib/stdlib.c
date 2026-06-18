// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#include <boot/stage2/lib/stdlib.h>

void memcpy(void* dest, void* src, uint32_t size)
{
    uint8_t* d = (uint8_t*)dest;
    uint8_t* s = (uint8_t*)src;

    while (size--)
        *d++ = *s++;
}

void* memset(void *dest, int val, uint32_t count) 
{
    unsigned char *ptr = (unsigned char *)dest;
    
    while (count > 0) 
    {
        *ptr = (unsigned char)val;
        ptr++;
        count--;
    }

    return dest;
}