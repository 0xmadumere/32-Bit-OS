; Copyright (c) 2026 0xmadumere
; SPDX-License-Identifier: MIT

%define CRLF 0x0A, 0x0D


%macro MSG 1
    mov     si, %1
    call    message
%endmacro  


%macro ERR 1
    mov     si, %1
    jmp     error_message
%endmacro


%macro REAL_MODE 0
[bits 32]
    jmp     word 0x18:%%pm16_mode       
    ; far jmp to change cs


[bits 16]
%%pm16_mode:
    mov     eax, cr0
    and     al, ~0x01
    mov     cr0, eax
    
    jmp     word 0x00:%%real_mode
    ; back to real mode

[bits 16]
%%real_mode:
    xor     ax, ax
    mov     ds, ax
    mov     ss, ax

    sti

%endmacro

%macro PROTECTED_MODE 0
[bits 16] 
    cli

    mov     eax, cr0
    or      al, 1
    mov     cr0, eax

    jmp     dword 0x08:%%pm32_mode


[bits 32]
%%pm32_mode:

    mov     ax, 0x10
    mov     ds, ax
    mov     ss, ax

%endmacro
