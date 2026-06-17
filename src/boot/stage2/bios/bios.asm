; Copyright (c) 2026 0xmadumere
; SPDX-License-Identifier: MIT

[cpu 486]

%include "boot/stage2/include/macros.inc"
%include "boot/stage2/bios/bios.inc"

extern g_boot_drive

global bios_chs_read
global bios_ext_read
global bios_putc
global bios_puts
global bios_get_mmap


[bits 32]
bios_putc:
    push    ebp
    mov     ebp, esp

    REAL_MODE

    mov     ah, 0x0E
    mov     al, [bp + 8]
    int     0x10

    PROTECTED_MODE

.done:
    mov     esp, ebp
    pop     ebp
    ret


[bits 32]
bios_puts:
    push    ebx
    push    esi
    push    ebp
    mov     ebp, esp

    REAL_MODE

    mov     si, [bp + 16]


.loop:
    mov     ah, 0x0E
    lodsb
    cmp     al, 0x00
    jz      .done
    mov     bh, 0x00
    int     0x10
    jmp     .loop

.done:
    PROTECTED_MODE
    mov     esp, ebp
    pop     ebp
    pop     esi
    pop     ebx
    ret


[bits 32]
bios_chs_read:
    push    ebp
    push    ebx
    push    es
    mov     ebp, esp

    REAL_MODE

    mov     bx, [bp + 16]            ; CHS pointer

    mov     dl, [g_boot_drive]      ; fetch drive number
    mov     al, [bx + CHS.count]    ; num of sectors to read

    mov     ch, [bx + CHS.cylinder] ; cylinder to read from
    mov     cl, [bx + CHS.sector]   ; sector to read from

    mov     dh, [bx + CHS.head]     ; head to read from

    mov     ah, 0x02    
    mov     cx, [bx + CHS.segment]  ; segment to write to
    mov     es, cx

    mov     cx, [bx + CHS.offset]   ; offset to write to

    mov     bx, cx

    int     0x13

    mov     eax, 1
    sbb     eax, 0

    push    eax

    PROTECTED_MODE

    pop     eax

.done: 
    mov     esp, ebp
    pop     es
    pop     ebx
    pop     ebp
    ret


[bits 32]
bios_ext_read:
    push    ebp
    mov     ebp, esp

    REAL_MODE

    mov     si, [bp + 8]    ; dap pointer
    mov     ah, 0x42
    mov     dl, [g_boot_drive]
    int     0x13

    mov     eax, 1
    sbb     eax, 0

    push    eax

    PROTECTED_MODE

    pop     eax

.done:
    mov     esp, ebp
    pop     ebp
    ret


[bits 32]

bios_get_mmap:
    push    ebp
    push    edi
    push    esi
    push    ebx
    mov     eax, es
    push    eax
    mov     ebp, esp
    sub     esp, 0x08

    ; bp - 4 = num of entries
    ; bp + 24 = address of mmap array
    ; bp + 28 = max_entries
    mov     dword [ebp - 4], 0x00   ; init to zero
    mov     eax, dword [ebp + 28]   ; fetch max entries
    mov     dword [ebp - 8], eax    ; store in ebp - 8

    mov     eax, [ebp + 24]         ; fetch address of mmap array
    mov     ecx, eax                ; copy address
    shr     eax, 4                  ; eax = addr >> 4  -> this is the segment
    and     eax, 0xFFFF
    and     ecx, 0x0F               ; ecx = addr & 0xF -> this is the offset

    push    ecx                     ; save offset
    push    eax                     ; save segment

    REAL_MODE
    [bits 16]

    pop    eax                      ; fetch segment
    mov    es, ax
    pop    ecx                      ; fetch offset
    mov    di, cx 
    xor    ebx, ebx

    ; jump to end of loop check
    ; i normally see compilers do this
    jmp     .next_iter

.loop:

    mov     eax, 0xE820
    mov     ecx, 24             ; size of each buffer
    mov     edx, 0x534D4150     ; magic "SMAP"
    int     0x15
    jc      .done               ; carry set = end of list or error, will return current entires so far or zero, if err on first run
    cmp     eax, 0x534D4150     ; check bios returned "SMAP"
    jne     .done
    add     di, 24              ; move to next entry slot
    inc     dword [ebp - 4]
    test    ebx, ebx            ; ebx = 0 means end of list
    jz      .done


.next_iter:

    mov    eax, dword [ebp - 4] ; fetch current entries so far
    cmp    eax, dword [ebp - 8] ; compare with max entries
    jl     .loop



.done:

    PROTECTED_MODE
    [bits 32]

    mov     eax, dword [ebp - 4]
    mov     esp, ebp
    pop     ecx
    mov     es, cx
    pop     ebx
    pop     esi
    pop     edi
    pop     ebp
    ; eax = number of entries
    ret 