// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#pragma once
#include <kernel/include/common.h>
#include <kernel/x86/isr.h>


#pragma pack(1)
typedef struct
{
    uint16_t offset_low;   // bits 0-15 of handler address
    uint16_t selector;     // gdt selector to load into cs/tss selector to select for task gates
    uint8_t reserved;     // 0
    uint8_t flag;         // flags and type
    uint16_t offset_high;  // bits 16-31 of handler address
} IDTEntry;
#pragma pack()


#pragma pack(1)
typedef struct
{
    uint16_t limit;
    uint32_t base; 
} IDTDescriptor;
#pragma pack()



typedef void (*irq_handler_t)(Registers* regs);

void irq_register_handler(uint8_t irq, irq_handler_t handler);
void idt_load();
void idt_init();