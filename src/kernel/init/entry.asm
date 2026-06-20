; Copyright (c) 2026 0xmadumere
; SPDX-License-Identifier: MIT

[cpu 486]
[bits 32]

extern  kmain

global kstack_start
global kstack_end
global kentry


section .text
kentry:
    cli

    ; ebx = boot_info*
    mov     esp, kstack_end ; put new kernel stack into esp

    ; push boot_info* unto new stack
    push    dword ebx

    call    kmain

stop:

    cli 

hang:

    hlt
    jmp     hang


section .bss

align 4096
kstack_start:
    resb    0xA000  ; 40 kb stack, 10 pages.
kstack_end: