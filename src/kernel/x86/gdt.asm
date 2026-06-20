; Copyright (c) 2026 0xmadumere
; SPDX-License-Identifier: MIT

[cpu 486]
[bits 32]

extern gdtptr

global tr_load
global gdt_load
global gdt_reload
global tr_reload

section .text
gdt_load:

	lgdt    [gdtptr]
    jmp     0x08:.1

.1:

    ; load data segs
    mov     ax, 0x10
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    mov     ss, ax

	ret


tr_load:
    ; load tr with kernel tss gdt selector, which is 0x20
	mov     ax, 0x20
	ltr     ax
    ret


gdt_reload:

	lgdt    [gdtptr]
	ret

tr_reload:
    ; reload tr
    mov     ax, 0x20
	ltr     ax
    ret


