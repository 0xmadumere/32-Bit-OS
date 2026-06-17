; Copyright (c) 2026 0xmadumere
; SPDX-License-Identifier: MIT

[cpu 486]

%include "boot/stage2/include/macros.inc"


extern  stage2_entry    
extern  enable_a20

global  g_boot_drive


section .entry

dd 0x5DAA7B54                               ; 4 byte signature


[bits 16]
entry:

    ; dl contains drive number
    ; ebx = pointer to dap
    ; save ptr to dap and boot_drive
    mov     dword [dap_ptr], ebx
    mov     byte [g_boot_drive], dl

    ; normalize segment registers and setup stack
    xor     ax, ax      
    mov     fs, ax
    mov     gs, ax
    mov     es, ax
    mov     ds, ax
    mov     ss, ax
    mov     sp, 0x7E00

    ; enable a20 line
    call    enable_a20
    test    al, al
    jz      a20_failed

    ; print confirmation message
    MSG     a20_activated_string


[bits 16]
enable_pm:

    cli                                         ; disable interrupts
    push    ds                                  ; save ds
    lgdt    [gdt_descriptor]                    ; load GDT
    mov     eax, cr0
    or      eax, 1                              ; set PM bit
    mov     cr0, eax                            ; store back in cr0
    jmp     0x08:pm                             ; far jump to set cs to pm gdt code selector, 0x08 = 32-bit code segment selector


[bits 32]
pm:

    ; setup protected mode data selector
    mov     ax, 0x10                
    mov     ds, ax
    mov     gs, ax
    mov     es, ax
    mov     fs, ax

    ; push arguments unto stack
    push    0x00007C00
    mov     eax, dword [dap_ptr]
    push    eax

    ; stage2_entry(DAP* dap, BPB* bpb)
    call    stage2_entry

    jmp     stop                                ; execution should never reach here


[bits 16]
a20_failed:

    ERR     a20_failed_string




[bits 16]
error_message:

	call	message
    

[bits 16]
general_error:

	MSG     general_error_string

    ; halt the machine
[bits 16]
stop:

    cli

hang:

    hlt     
    jmp     hang


[bits 16]
fall_through:

    mov     bx, 0x0001
    mov     ah, 0x0E
	int	    0x10		                            ; display a byte 

[bits 16]
message:
	lodsb
    cmp     al, 0x00                                ; is null terminator?
	jne	    fall_through	                        ; if not end of string, jmp to fall through
	ret


gdt_start:
    ; null desriptor
    dq 0


    ; 32-bit code segment covers all 4GiB, ring 0 executable
    ; selector = 0x08
    dw      0xFFFF                                   ; limit low
    dw      0x0000                                   ; base low
    db      0x00                                     ; base middle

    ; acess byte
    ; bit 7      bit 6-5    bit 4    bit 3    bit2    bit 1    bit 0
    ; P          DPL        S        E        DC      RW       A
    db      10011010b
    ; P (bit 7) - Present. 1 = segment exists and is valid
    ; DPL (bits 6-5) - Descriptor Privilege Level. 00 = ring 0
    ; S (bit 4) - Descriptor type. 1 = code/data segment 
    ; E (bit 3) — Executable. 1 = code segment
    ; DC (bit 2) - Direction/Conforming. 0 = grows up
    ; RW (bit 1) - Readable/Writable. For code segments: 1 = readable
    ; A (bit 0) - Accessed. CPU sets this to 1 when the segment is accessed


    ; flags byte
    ; bit 7      bit 6    bit 5    bit 4    bit 3-0
    ; G          DB       L        0        limit 19-16 
    db      11001111b
    ; G (bit 7) - Granularity. 1 = limit in 4KiB pages
    ; DB (bit 6) - Default/Big. For code segments: 1 = 32-bit defaults
    ; L (bit 5) - Long mode. 0 = not using long mode
    ; 0 (bit 4) - reserved. Always 0
    ; limit 19-16 - bits 3-0 - upper 4 bits of the 20 bit limit value

    db      0x00                                    ; base high


    ; 32-bit data segment covers all 4GiB, ring 0 writable
    ; selector = 0x10
    dw      0xFFFF                                   ; limit low
    dw      0x0000                                   ; base low
    db      0x00                                     ; base middle

    ; acess byte
    ; bit 7      bit 6-5    bit 4    bit 3    bit2    bit 1    bit 0
    ; P          DPL        S        E        DC      RW       A
    db      10010010b
    ; P (bit 7) - Present. 1 = segment exists and is valid
    ; DPL (bits 6-5) - Descriptor Privilege Level. 00 = ring 0
    ; S (bit 4) - Descriptor type. 1 = code/data segment 
    ; E (bit 3) - Executable. 0 = data segment
    ; DC (bit 2) - Direction/Conforming. 0 = grows up
    ; RW (bit 1) - Readable/Writable. For data segments: 1 = writable
    ; A (bit 0) - Accessed. CPU sets this to 1 when the segment is accessed


    ; flags byte
    ; bit 7      bit 6    bit 5    bit 4    bit 3-0
    ; G          DB       L        0        limit 19-16 
    db      11001111b
    ; G (bit 7) - Granularity. 1 = limit in 4KiB pages
    ; DB (bit 6) - Default/Big. For code/data segments: 1 = 32-bit defaults
    ; L (bit 5) - Long mode. 0 = not using long mode
    ; 0 (bit 4) - reserved. Always 0
    ; limit 19-16 - bits 3-0 - upper 4 bits of the 20 bit limit value

    db      0x00                                     ; base high


    ; 16-bit code segment covers all 1MiB, ring 0 executable
    ; selector = 0x18
    dw      0xFFFF                                   ; limit low
    dw      0x0000                                   ; base low
    db      0x00                                     ; base middle

    ; acess byte
    ; bit 7      bit 6-5    bit 4    bit 3    bit2    bit 1    bit 0
    ; P          DPL        S        E        DC      RW       A
    db 10011010b
    ; P (bit 7) - Present. 1 = segment exists and is valid
    ; DPL (bits 6-5) - Descriptor Privilege Level. 00 = ring 0
    ; S (bit 4) - Descriptor type. 1 = code/data segment 
    ; E (bit 3) — Executable. 1 = code segment
    ; DC (bit 2) - Direction/Conforming. 0 = grows up
    ; RW (bit 1) - Readable/Writable. For code segments: 1 = readable
    ; A (bit 0) - Accessed. CPU sets this to 1 when the segment is accessed

    ; flags byte
    ; bit 7      bit 6    bit 5    bit 4    bit 3-0
    ; G          DB       L        0        limit 19-16 
    db 00001111b 
    ; G (bit 7) - Granularity. 0 = limit in 1 byte granularity
    ; DB (bit 6) - Default/Big. For code segments: 0 = 16-bit defaults
    ; L (bit 5) - Long mode. 0 = not using long mode
    ; 0 (bit 4) - reserved. Always 0
    ; limit 19-16 - bits 3-0 - upper 4 bits of the 20 bit limit value

    db      0x00                                     ; base high


    ; 16-bit data segment covers all 1MiB, ring 0 writable
    ; selector = 0x20
    dw      0xFFFF                                   ; limit low
    dw      0x0000                                   ; base low
    db      0x00                                     ; base middle

    ; acess byte
    ; bit 7      bit 6-5    bit 4    bit 3    bit2    bit 1    bit 0
    ; P          DPL        S        E        DC      RW       A
    db      10010010b
    ; P (bit 7) - Present. 1 = segment exists and is valid
    ; DPL (bits 6-5) - Descriptor Privilege Level. 00 = ring 0
    ; S (bit 4) - Descriptor type. 1 = code/data segment 
    ; E (bit 3) - Executable. 0 = data segment
    ; DC (bit 2) - Direction/Conforming. 0 = grows up
    ; RW (bit 1) - Readable/Writable. For data segments: 1 = writable
    ; A (bit 0) - Accessed. CPU sets this to 1 when the segment is accessed

    ; flags byte
    ; bit 7      bit 6    bit 5    bit 4    bit 3-0
    ; G          DB       L        0        limit 19-16 
    db      00001111b
    ; G (bit 7) - Granularity. 0 = limit in 1 byte granularity
    ; DB (bit 6) - Default/Big. For code/data segments: 0 = 16-bit defaults
    ; L (bit 5) - Long mode. 0 = not using long mode
    ; 0 (bit 4) - reserved. Always 0
    ; limit 19-16 - bits 3-0 - upper 4 bits of the 20 bit limit value

    db      0x00

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start


dap_ptr:

    dd 0x00000000

g_boot_drive:

    db 0x00



general_error_string        db " Error", CRLF, 0x00 
a20_activated_string        db "A20 line activated", CRLF, 0x00
a20_failed_string           db "A20 activation", 0x00