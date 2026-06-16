; Copyright (c) 2026 0xmadumere
; SPDX-License-Identifier: MIT

[cpu 486]
[bits 16]

%include "boot/stage2/common.inc"

global  check_a20_status
global  enable_a20  


[bits 16]
check_a20_status:

    ; save registers
    push    ds
    push    es
    push    di
    push    si

    xor     ax, ax              ; ax = 0
    mov     es, ax

    not     ax                  ; ax = 0xFFFF
    mov     ds, ax

    mov     di, 0x0500
    mov     si, 0x0510

    mov al, byte [es:di]        ; [0x0000:0x0500] = [0x500]
    push ax                     ; save prev value

    mov al, byte [ds:si]        ; [0xFFFF:0x510] = [0x100500]
    push ax                     ; save prev value

    mov byte [es:di], 0x00      ; [0x500] = 0x00

    ; if the a20 line is disables the address 0x100500 will wrap around to 0x500
    mov byte [ds:si], 0xFF      ; [0x100500] = 0xFF
    ; so if the a20 line is disabled, this write will overwrite the 0x00 we wrote with 0xFF
    
    cmp byte [es:di], 0xFF      ; check to see if the write to 0x100500 wrapped around and overwrote our value with 0xFF

    ; restore old values
    pop ax
    mov byte [ds:si], al

    pop ax
    mov byte [es:di], al

    ; return 0 if byte got overwritten meaning a20 is disabled
    mov ax, 0
    je .done

    ; return 1 on if byte didnt get overwritten, meaning a20 is enabled
    mov ax, 1

.done:

    pop si
    pop di
    pop es
    pop ds

    ret


[bits 16]
enable_a20:

    ; save registers
    push    di
    push    si
    push    bx
    push    es
    push    ds
       
    ; is a20 already enabled, then why where we called?   
    call    check_a20_status
    test    al, al
    jnz     .done

    ; try fastest method first ofc
.fast_gate:

    ; fetch current state of that system control register
	in      al, 0x92

    ; is a20 line already enabled?
	test    al, 2
	jnz     .done

    ; set bit 1 to 1, write back to port
	or      al, 2
	out     0x92, al

    ; did it work?
	call    check_a20_status
	test    al, al
	jnz     .done

    ; ask the bios for help this time
.bios:

    mov     ax, 0x2401              ; activate A20 gate
    int     0x15
    jc      .keyboard_controller    ; couldn't activate the gate
    test    ah, ah
    jnz     .keyboard_controller    ; gate failed to activate

    call    check_a20_status
    test    al, al
    jnz     .done

    ; final chance to get out sh*t together
.keyboard_controller:

    ; disable interrupts
    cli     

    ; wait for for input buffer to be empty
    call    .wait_io1

    ; disable controllers keyboard interface
    mov     al, 0xAD
	out     0x64, al

    call    .wait_io1
	mov     al, 0xD0                ; this tells the controller to write its mode register to 0x60
	out     0x64, al

    call    .wait_io2               ; wait for output buffer to be full
	in      al, 0x60
	push    eax                     ; save controllers mode register to stack

    call    .wait_io1               ; wait again
	mov     al, 0xD1                ; this tells the controller to write the data in 0x60 to its output port                          
	out     0x64, al

    call    .wait_io1
	pop     eax                     ; fetch saved mode
	or      al, 2                   ; set bit 1 to 1, this enables a20 line
	out     0x60, al                ; write it back

    call    .wait_io1
	mov     al, 0xAE                ; re enable the keyboard interface
	out     0x64, al


    call    .wait_io1               ; wait for controller to process command, so we wait for input buffer to be clear
	sti                             ; re enable interrupts

    call    check_a20_status
    test    al, al
    jnz     .done

    ; all three didnt work? give up
    jmp     .failed

; this function waits for the input buffer to be empty
.wait_io1:

	in      al, 0x64
	test    al, 2
	jnz     .wait_io1
	ret


; this functions wait for the output buffer to be full
.wait_io2:

	in      al, 0x64
	test    al, 1
	jz      .wait_io2
	ret

.failed:

    mov     ax, 0
    pop     ds
    pop     es
    pop     bx
    pop     si
    pop     di
    ret


.done:

    mov     ax, 1
    pop     ds
    pop     es
    pop     bx
    pop     si
    pop     di
    ret