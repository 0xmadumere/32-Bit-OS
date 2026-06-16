; Copyright (c) 2026 0xmadumere
; SPDX-License-Identifier: MIT

; ========== Constants ==========
%define STACK_START     0x7C00
%define STAGE2_ADDRESS  0x7E00
%define STAGE2_SECTORS  0x0F        ; 15 sectors
%define TEST_BUFFER_SEG 0x7000
%define LBA_OF_SECTOR2  0x01
%define STAGE2_SIG      0x5DAA7B54
%define LEN_OF_SIG      0x04


; ========== Macros ==========
%macro MSG 1
    mov     si, %1
    call    message
%endmacro  


%macro ERR 1
    mov     si, %1
    jmp     error_message
%endmacro


; ============= BPB ============= 
bpb_num_of_hidden_sectors       equ 0x7C1C      ; 4 bytes
bpb_drive_number                equ 0x7C24      ; 1 byte