; Copyright (c) 2026 0xmadumere
; SPDX-License-Identifier: MIT

[cpu 486]
[bits 32]

%include "kernel/x86/isr.inc"

extern isr_handler

global isr_common

section .text

; cpu exceptions
ISR_NOERRCODE 0   ; divide by zero
ISR_NOERRCODE 1   ; debug
ISR_NOERRCODE 2   ; nmi
ISR_NOERRCODE 3   ; breakpoint
ISR_NOERRCODE 4   ; overflow
ISR_NOERRCODE 5   ; bound range exceeded
ISR_NOERRCODE 6   ; invalid opcode
ISR_NOERRCODE 7   ; device not available
ISR_ERRCODE   8   ; double fault
ISR_NOERRCODE 9   ; coprocessor segment overrun
ISR_ERRCODE   10  ; invalid TSS
ISR_ERRCODE   11  ; segment not present
ISR_ERRCODE   12  ; stack fault
ISR_ERRCODE   13  ; general protection fault
ISR_ERRCODE   14  ; page fault
ISR_NOERRCODE 15  ; reserved
ISR_NOERRCODE 16  ; x87 floating point
ISR_ERRCODE   17  ; alignment check
ISR_NOERRCODE 18  ; machine check
ISR_NOERRCODE 19  ; simd floating point
ISR_NOERRCODE 20 ; virtualization exception
ISR_ERRCODE  21  ; control protection exception

; irqs
IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39
IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47


isr_common:
    pusha                   ; push all gprs
    xor     eax, eax
    mov     ax, ds
    push    eax                ; save data segment

    mov     ax, 0x10            ; load kernel data segment
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax

    call    isr_handler

    pop     eax                 ; restore data segment
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax

    popa                    ; restore registers
    add     esp, 8             ; clean up error code and interrupt number
    sti
    iret

