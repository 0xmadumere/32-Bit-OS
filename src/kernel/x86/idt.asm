; Copyright (c) 2026 0xmadumere
; SPDX-License-Identifier: MIT

[cpu 486]
[bits 32]

extern idtptr

global idt_load

idt_load:
    lidt    [idtptr]