// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#pragma once
#include <kernel/include/common.h>

#pragma pack(1)
typedef struct 
{
    uint16_t limit_low;        // bits 0-15 of limit
    uint16_t base_low;         // bits 0-15 of base address
    uint8_t base_mid;         // bits 16-23 of base address
    uint8_t access_byte;        // P DPL S type
    uint8_t limit_high_flags; // G D/B L AVL and bits 16-19 of limit
    uint8_t base_high;        // bits 24-31 of base address
} GDTEntry;
#pragma pack()


#pragma pack(1)
typedef struct 
{
    uint16_t limit;
    uint32_t base;
} GDTDescriptor;
#pragma pack()


#pragma pack(1)
typedef struct 
{
    uint16_t link;
    uint16_t reserved1;
    uint32_t esp0;
    uint16_t ss0;
    uint16_t reserved2;
    uint32_t esp1;
    uint16_t ss1;
    uint16_t reserved3;
    uint32_t esp2;
    uint16_t ss2;
    uint16_t reserved4;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint16_t es;
    uint16_t reserved5;
    uint16_t cs;
    uint16_t reserved6;
    uint16_t ss;
    uint16_t reserved7;
    uint16_t ds;
    uint16_t reserved8;
    uint16_t fs;
    uint16_t reserved9;
    uint16_t gs;
    uint16_t reserved10;
    uint16_t ldtr;
    uint32_t reserved11;
    uint16_t iopb;
    uint32_t ssp;
} TSS;
#pragma pack()



void gdt_init();
void gdt_load();
void tr_load();
void gdt_set_entry(
    int idx, uint32_t base,
	uint32_t limit, uint8_t access, uint8_t flags
);
void gdt_reload();
void tr_reload();
