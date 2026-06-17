; Copyright (c) 2026 0xmadumere
; SPDX-License-Identifier: MIT

[cpu 486]
[bits 32]
   
extern  kmain
section .entry
extern _bss_start
extern _bss_end
extern current_stack_floor

dd  0x77FF5F80

[bits 32]
global entry
entry:
    cli

    mov     esi, [esp + 4]  ; boot_info*

    mov     esp, kstack_end

    ; clean bss
    mov     edi, _bss_start
    mov     ecx, _bss_end
    sub     ecx, edi
    xor     eax, eax
    rep     stosb

    ; push back boot_info*
    push    esi

    call    kmain

stop:
    cli 

hang:
    hlt
    jmp     hang


section .bss
global kstack_start
global kstack_end

align 4096
kstack_start:
    resb    0xA000  ; 40 kb stack, 10 pages.
kstack_end: