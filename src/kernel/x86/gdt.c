// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#include <kernel/x86/gdt.h>

// yup all 8192 possible entries
GDTEntry gdt[8192];
GDTDescriptor gdtptr; 

TSS kernel_tss;


void gdt_set_entry(
    int idx, uint32_t base,
	uint32_t limit, uint8_t access, uint8_t flags
)
{

    gdt[idx].limit_low = limit & 0xFFFF;
    gdt[idx].base_low = base & 0xFFFF;
    gdt[idx].base_mid = (base >> 16) & 0xFF;
    gdt[idx].access_byte = access;
    gdt[idx].limit_high_flags = (flags << 4);
    gdt[idx].limit_high_flags |= (limit >> 16) & 0x0F;
    gdt[idx].base_high = (base >> 24) & 0xFF;

}



void gdt_init()
{
    gdtptr.base = (uint32_t)gdt;
    gdtptr.limit = sizeof(gdt) - 1;

    // null descriptor  
	gdt_set_entry(0, 0, 0, 0, 0);

    // kernel code seg
    gdt_set_entry(1, 0, 0xFFFFF, 0x9A, 0xC);

    // kernel data seg
    gdt_set_entry(2, 0, 0xFFFFF, 0x92, 0xC);

    // kernel tss seg
    gdt_set_entry(4, (uint32_t)&kernel_tss, sizeof(kernel_tss) - 1, 0x89, 0x04);


    gdt_load();
    tr_load();

}