// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#pragma once

#include <kernel/include/common.h>

#pragma pack(1)
typedef struct 
{
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;  // regsiters pushed by isr_common
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss; // pushed by cpu
} Registers;
#pragma pack(1)

void isr0();
void isr1();
void isr2();
void isr3();
void isr4();
void isr5();
void isr6();
void isr7();
void isr8();
void isr9();
void isr10();
void isr11();
void isr12();
void isr13();
void isr14();
void isr15();
void isr16();
void isr17();
void isr18();
void isr19();
void isr20();
void isr21();


void irq0();
void irq1();
void irq2();
void irq3();
void irq4();
void irq5();
void irq6();
void irq7();
void irq8();
void irq9();
void irq10();
void irq11();
void irq12();
void irq13();
void irq14();
void irq15();