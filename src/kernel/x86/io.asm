; Copyright (c) 2026 0xmadumere
; SPDX-License-Identifier: MIT

[cpu 486]
[bits 32]

global outb
global inb
global sti
global hcf

section .text
outb:

    mov dx, [esp + 4]
    mov al, [esp + 8]
    out dx, al
    ret


inb:

    mov dx, [esp + 4]
    xor eax, eax
    in al, dx
    ret


sti:

    sti
    ret


hcf:
    cli
.hang:
    hlt
    jmp     .hang
