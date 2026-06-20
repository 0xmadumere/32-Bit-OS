; Copyright (c) 2026 0xmadumere
; SPDX-License-Identifier: MIT

; non error code exceptions
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    cli
    push dword 0     ; dummy err code
    push dword %1    ; interrupt no
    jmp isr_common
%endmacro

; error code exceptions
%macro ISR_ERRCODE 1
global isr%1
isr%1:
    cli
    push dword %1    ; interrupt no, err code pushed by cpu
    jmp isr_common
%endmacro

; irqs
%macro IRQ 2
global irq%1
irq%1:
    cli
    push dword 0     ; dummy error code
    push dword %2    ; interrupt number
    jmp isr_common
%endmacro



