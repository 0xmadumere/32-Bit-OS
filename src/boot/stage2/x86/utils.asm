; Copyright (c) 2026 0xmadumere
; SPDX-License-Identifier: MIT

[cpu 486]
[bits 16]

%include "boot/stage2/common.inc"

global stop
global error_message
global message

[bits 16]
error_message:

	call	message
    

[bits 16]
general_error:

	MSG     general_error_string


    ; reference: https://www.ctyme.com/intr/rb-2241.htm

    ; DISKLESS BOOT HOOK (START CASSETTE BASIC)

    ; Desc: Called when there is no bootable disk available to the system 

    ; tell the bios a boot failure occurred
    int	0x18

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

general_error_string	db " Error", CRLF, 0x00