// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#pragma once
#include <kernel/include/common.h>

#define PIC1_COMMAND    0x20
#define PIC1_DATA       0x21
#define PIC2_COMMAND    0xA0
#define PIC2_DATA       0xA1
#define PIC_EOI         0x20

#define ICW1_INIT       0x11
#define ICW4_8086       0x01

void pic_remap(uint8_t offset1, uint8_t offset2);
void pic_send_eoi(uint8_t irq);
void pic_disable();