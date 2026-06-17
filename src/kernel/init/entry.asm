; Copyright (c) 2026 0xmadumere
; SPDX-License-Identifier: MIT

[cpu 486]
[bits 32]
   
section .entry

extern  kmain
extern _bss_start
extern _bss_end

global kstack_start
global kstack_end
global entry


dd  0x77FF5F80


entry:

    cli

    mov     esi, [esp + 4]  ; boot_info*, save into esi

    mov     esp, kstack_end ; put new kernel stack into esp

    ; clean bss
    mov     edi, _bss_start
    mov     ecx, _bss_end
    sub     ecx, edi
    xor     eax, eax
    rep     stosb

    ; push back boot_info* unto new stack
    push    esi

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