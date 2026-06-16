; Copyright (c) 2026 0xmadumere
; SPDX-License-Identifier: MIT

[cpu 486]
[bits 16]
[org 0x7C3E]           

%include "boot/stage1/vbr.inc" 

; some bioses load at 0x7c00:0x0000 instead of 0x0000:0x7c00, so we normalize
    jmp     0x0000:start                          


probe_values:
;   spt(sectors per track) to probe
    db  36, 18, 15, 9, 0                        


; reference: https://www.ctyme.com/intr/rb-0605.htm

; DISK - RESET DISK SYSTEM

; AH = 00h
; DL = drive (if bit 7 is set both hard disks and floppy disks reset)

; Return:
; AH = status (see #00234)
; CF clear if successful (returned AH=00h)
; CF set on error

; Note: Forces controller to recalibrate drive heads (seek to track 0). 
; For PS/2 35SX, 35LS, 40SX and L40SX, as well as many other systems, 
; both the master drive and the slave drive respond to the Reset function that is issued to either drive 


floppy_probe:

    push    dx                                  ; save drive number
    mov     si, probe_values - 1                ; load the address just before the label into si


probe_loop:

    xor     ax, ax                              ; set ax to 0
    int     0x13                                ; reset disk
    inc     si                                  ; move si to the start of the label, remember we put (label - 1) into si
    mov     cl, byte [si]                       ; fetch first probe value
    test    cl, cl                              ; is spt = 0?, if so all probes failed
    jnz     do_read

    ; floppy probe failure
    MSG     fd_probe_error_string
	jmp	    general_error

fd_probe_error_string   db "Floppy", 0x00


; reference: https://www.ctyme.com/intr/rb-0607.htm

; DISK - READ SECTOR(S) INTO MEMORY

; AH = 02h
; AL = number of sectors to read (must be nonzero)
; CH = low eight bits of cylinder number
; CL = sector number 1-63 (bits 0-5)
; high two bits of cylinder (bits 6-7, hard disk only)
; DH = head number
; DL = drive number (bit 7 set for hard disk)
; ES:BX -> data buffer

; Return:
; CF set on error
; if AH = 11h (corrected ECC error), AL = burst length
; CF clear if successful
; AH = status (see #00234)
; AL = number of sectors transferred (only valid if CF set for some
; BIOSes)



do_read:

    mov     bx, TEST_BUFFER_SEG                 ; move test buffer segment into bx
    mov     es, bx                              ; store bx into es          
    xor     bx, bx                              ; bx = 0
; bx = offset where read sectors will be written to                

    mov     al, 0x01                            ; move number of sectors to read into al
    mov     ah, 0x02                            ; function 2
    mov     ch, 0x00                            ; ch = cylinder to read from, reading from first cylinder (0 based)
    mov     dh, 0x00                            ; dh = which head to read from
    int     0x13

    jc      probe_loop                          ; if carry flag set jump back to beginning of loop and try again with a new spt value

    mov     dh, 1                               ; if probe worked set max head idx to 1
    mov     ch, 79                              ; set max cylinder idx to 79


; why do we hardcode the max head and cylinder idx to 1 and 79?
; we do that because all the floppies we actually care about, 
; have the same max cylinder and head idx

; Name of floppy                Cylinders	Heads	SPT(Sectors per track)	
; 3.5 Double Density    	    80          2	    9	
; 5.25 High Density     	    80	        2	    15	
; 3.5 High Density  	        80	        2	    18	
; 3.5 Microsoft DMF     	    80	        2	    21	
; 3.5 Extra-High Density    	80	        2	    36 


    jmp     final_init


; dap/chs info data structure overlapped
; mode             1 byte  at -1 from si, this isnt part of the dap
; disk_address_packet: ← si points here
; sectors          1 byte  at offset 0      dap size
;                  1 byte  at offset 1      dap reserved
;                  2 bytes at offset 2      dap num_sectors
; heads            2 bytes at offset 4      dap buffer_offset
;                  2 bytes at offset 6      dap buffer_segment
; cylinders        2 bytes at offset 8      dap lba_low bytes 0-1
; sector_start     1 byte  at offset 10     dap lba_low byte 2
; head_start       1 byte  at offset 11     dap lba_low byte 3
; cylinder_start   2 bytes at offset 12     dap lba_high bytes 0-1


mode:

	db	0x00

disk_address_packet:

sectors:

	dd	0x00

heads:

	dd	0x00

cylinders:

	dw	0x00

sector_start:

	db	0x00

head_start:

	db	0x00

cylinder_start:

	dd	0x00


start:

    cli                                         ; clear interrupts

; check to see if any of bits 4-6 of dl are 1, 
; if so then the drive number doesn't fall between resonable valid range of
; 0x00 -> 0x0F and 0x80 -> 0x8F
    test    dl, 0x70
    jz      .1 
    mov     dl, 0x80                            ; force drive 0x80

.1:

    mov     byte [bpb_drive_number], dl         ; store correct drive number in bpb
    xor	    ax, ax                              ; ax = 0       
	mov 	ds, ax                              ; ds = 0
	mov 	ss, ax                              ; ss = 0
    mov     sp, STACK_START                     ; setup stack
    sti

    mov     si, disk_address_packet


; reference: https://www.ctyme.com/intr/rb-0706.htm

; IBM/MS INT 13 Extensions - INSTALLATION CHECK

; AH = 41h
; BX = 55AAh
; DL = drive (80h-FFh)

; Return:
; CF set on error (extensions not supported)
; AH = 01h (invalid function)
; CF clear if successful
; BX = AA55h if installed
; AH = major version of extensions
; 01h = 1.x
; 20h = 2.0 / EDD-1.0
; 21h = 2.1 / EDD-1.1
; 30h = EDD-3.0
; AL = internal use
; CX = API subset support bitmap (see #00271)
; DH = extension version (v2.0+ ??? -- not present in 1.x)

; Note: The Phoenix Enhanced Disk Drive Specification v1.0 uses version 2.0 of the INT 13 Extensions API 


; check if extended lba is available
    push    dx                                  ; save drive number
    mov     ah, 0x41                           
    mov     bx, 0x55AA
	int	    0x13 
    pop     dx                                  ; fetch drive number
    push    dx                                  ; push it back unto the stack

; check if the call succeeded
    jc      chs_mode
    and     cx, 0x01
    jz      chs_mode
    cmp     bx, 0xAA55
    jnz     chs_mode

; if function succeeded proceed with lba mode, else fall back to chs mode


; reference: https://www.ctyme.com/intr/rb-0708.htm

; IBM/MS INT 13 Extensions - EXTENDED READ

; AH = 42h
; DL = drive number
; DS:SI -> disk address packet (see #00272)

; Return:
; CF clear if successful
; AH = 00h
; CF set on error
; AH = error code (see #00234)
; disk address packet's block count field set to number of blocks
; successfully transferred


; Format of disk address packet:

; Offset  Size    Description     (Table 00272)
; 00h    BYTE    size of packet (10h or 18h)
; 01h    BYTE    reserved (0)
; 02h    WORD    number of blocks to transfer (max 007Fh for Phoenix EDD)
; 04h    DWORD   -> transfer buffer
; 08h    QWORD   starting absolute block number
; (for non-LBA devices, compute as
; (Cylinder*NumHeads + SelectedHead) * SectorPerTrack +
; SelectedSector - 1
; 10h    QWORD   (EDD-3.0, optional) 64-bit flat address of transfer buffer;
; used if DWORD at 04h is FFFFh:FFFFh


lba_mode:

	mov 	word [si + 0x04], STAGE2_ADDRESS        ; write stage 2 offset to dap
    
    mov     ax, 0x01
; set mode to 1, which means extended lba mode is enabled
   
    mov     byte [si - 0x01], al                    ; write disk mode to dap, in this case 1

    mov     word [si + 0x02], STAGE2_SECTORS        ; sectors to read for stage2
    mov     word [si], 0x0010                       ; store size and reserved

    mov     ebx, [bpb_num_of_hidden_sectors]        ; lba of where the partition starts
    add     ebx, LBA_OF_SECTOR2                     ; lba of sector 2, start of stage2
    mov     dword [si + 0x08], ebx                  ; store lba low

    xor     ebx, ebx                                ; ebx = 0
    mov     dword [si + 0x0C], ebx                  ; write lba high = 0

    mov     word [si + 0x06], bx                    ; write buffer segment = 0

    mov 	ah, 0x42
	int	    0x13

; if read doesn't work, assume lba is not supported, jump to chs, else jump to verify stage 2 signature
	jc	    chs_mode
    jmp     verify


; reference: https://www.ctyme.com/intr/rb-0621.htm

; DISK - GET DRIVE PARAMETERS (PC,XT286,CONV,PS,ESDI,SCSI)

; AH = 08h
; DL = drive (bit 7 set for hard disk)
; ES:DI = 0000h:0000h to guard against BIOS bugs

; Return:
; CF set on error
; AH = status (07h) (see #00234)
; CF clear if successful
; AH = 00h
; AL = 00h on at least some BIOSes
; BL = drive type (AT/PS2 floppies only) (see #00242)
; CH = low eight bits of maximum cylinder number
; CL = maximum sector number (bits 5-0)
; high two bits of maximum cylinder number (bits 7-6)
; DH = maximum head number
; DL = number of drives
; ES:DI -> drive parameter table (floppies only)


chs_mode:

;   Determine the hard disk geometry from the bios
	mov 	ah, 0x08
	int	    0x13
	jnc	    final_init              

    pop     dx                                      ; fetch stored drive number from stack
    push    dx                                      ; push it back unto the stack

    test    dl, dl      
    jns     floppy_probe                            ; drive is a floppy, probe sectors manually

    ERR     hd_probe_error_string                   ; failed to query hard drive geometry


final_init:
; reached from both chs_mode/lba_mode and floppy_probe
; at this point geometry is known, so we use that to populate the dap

    movzx   eax, dh
; ax: ah = 0x00, al = max head idx (0-based)

    mov     byte [si - 0x01], ah                    ; set mode to 0, 0 = chs

    inc 	ax
; al = number of heads

    mov     dword [si + 0x04], eax                  ; store number of heads in dap

    movzx   dx, cl                                  ; CL = maximum sector idx (bits 5-0) (0-based)     
                                                    ; bits 7 and 6 = high 2 bits of cylinder number    

    shl 	dx, 0x02                                  
; shift dx left twice, which then isolates bits 7 and 6 in dh 

    mov     al, ch                                  ; store low bits of max cylinder number in al

    mov     ah, dh                                  ; move dh into al, remember dh has the high 2 bits of max cylinder idx
; now ax contains the full max cylinder idx     

    inc	    ax                                          ; cylinders are zero based, so add one to get number of cylinders

    mov     word [si + 0x08], ax                    ; store number of cylinders in dap


    movzx   ax, dl                                  ; dl contains bits 5-0 of the sectors per track count
                                                    ; but rn they are in bits 7 -> 2, because of the earlier left shift 


    shr     al, 0x02                                ; we shift ax right by 2, now bits 7 and 2 are back in 5 and 0

    mov     [si], eax                               ; save number of sectors 


; reference: https://www.ctyme.com/intr/rb-0607.htm

; DISK - READ SECTOR(S) INTO MEMORY

; AH = 02h
; AL = number of sectors to read (must be nonzero)
; CH = low eight bits of cylinder number
; CL = sector number 1-63 (bits 0-5)
; high two bits of cylinder (bits 6-7, hard disk only)
; DH = head number
; DL = drive number (bit 7 set for hard disk)
; ES:BX -> data buffer

; Return:
; CF set on error
; if AH = 11h (corrected ECC error), AL = burst length
; CF clear if successful
; AH = status (see #00234)
; AL = number of sectors transferred (only valid if CF set for some
; BIOSes)


setup_sectors:

    mov     eax, dword [bpb_num_of_hidden_sectors]  ; fetch number of hidden sectors
    add     eax, LBA_OF_SECTOR2
; eax = full lba of sector 2

    xor     edx, edx                                ; clear edx for upcoming division

    div     dword [si]                              ; divide by sectors per track!

; calculate sector
; sector = (LBA % sectors_per_track) + 1

; save sector to read in cl                         ; eax = lba / spt, edx = lba % spt
    mov     cl, dl

; calculate cylinder
; cylinder = (LBA / sectors_per_track) / heads
    xor 	dx, dx	                                ; clear edx for upcoming division
	div 	dword [si + 0x04]		                ; divide eax(which is = lba / spt) by number of heads
; eax = cylinder to read, edx = head to read

    cmp     ax, [si + 0x08]  
; eax = cylinder to read                     
; compare cylinder to read, to the number of cylinders available
; if cylinder to read is greater than or equal to number of cylinders then thats an error

	jge	    geometry_error

; normalize sector to read, sectors are 1-based 
	inc	    cl

; move low 8 bits of cylinder to read into ch
    mov     ch, al

	xor	    al, al
; clear out al, now ah = bits 8-9 of cylinder to read

; shift ax right by 2, which moves those two bits into al
    shr     ax, 0x02   
    or      cl, al  
; now cl bits 5-0 = sector to read, cl bits 6-7 = high two bits of cylinder to read   

; save head to read 
    mov     al, dl 

; restore dl
; dl had our drive number, but has been clobbered
; so we first move dl(head to read from) into al to save it
; then we restore the drive number
; then put head to read into dh
	pop 	dx

	; head to read
    mov     dh, al

    xor     bx, bx                                  ; bx = 0
    mov     es, bx                                  ; es = segment to write read sector data
    mov     bx, STAGE2_ADDRESS                      ; bx = offset


    mov     ah, 0x02                                ; function 2
    mov     al, STAGE2_SECTORS                      ; sectors to read

	int	    0x13

	jc	    read_error


verify:

    mov     eax, dword [STAGE2_ADDRESS]             ; fetch 4 byte stage 2 signature
    cmp     eax, STAGE2_SIG                         ; cmp signature to valid sig
    jne     bad_sig                                 ; print bad sig error and hlt, signature doesnt match
    mov     ax, STAGE2_ADDRESS                      ; store address of stage 2 in ax
    add     ax, LEN_OF_SIG                          ; add length of sig
    mov     ebx, disk_address_packet - 1            ; ebx = ptr to dap parameter/chs values -  1              
    jmp     ax                                      ; jump to stage 2


bad_sig:

    ERR     bad_sig_string

read_error:

    ERR     read_err_string

error_message:

	call	message

general_error:

	MSG     general_error_string

; reference: https://www.ctyme.com/intr/rb-2241.htm

; DISKLESS BOOT HOOK (START CASSETTE BASIC)

; Desc: Called when there is no bootable disk available to the system 

; tell the bios a boot failure occurred
    int	    0x18

; hlt the machine
stop:
    hlt
	jmp	    stop


; reference: https://www.ctyme.com/intr/rb-0106.htm

; VIDEO - TELETYPE OUTPUT

; AH = 0Eh
; AL = character to write
; BH = page number
; BL = foreground color (graphics modes only)

; Return:
; Nothing

; Desc: Display a character on the screen, advancing the cursor and scrolling the screen as necessary 


fall_through:

    mov     bh, 0x00
    mov     ah, 0x0E
	int	    0x10		                            ; display a byte 

message:
	lodsb
    cmp     al, 0x00                                ; is null terminator?
	jne	    fall_through	                        ; if not end of string, jmp to fall through
	ret

geometry_error:
	ERR     geom_error_string


; error strings
geom_error_string       db "Geometry", 0x00
bad_sig_string          db "Bad Sig", 0x00
read_err_string         db "Read", 0x00
general_error_string	db " Error", 0x0A, 0x0D, 0x00
hd_probe_error_string	db "Hard Disk", 0x00


; vbr code will be place in between bpb table and boot sector signature
times  510 - ($ - $$)   db 0x00