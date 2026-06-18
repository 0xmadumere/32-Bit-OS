[bits 32]

extern entry

global enable_paging
global jump_to_entry

section .btstrp
enable_paging:
    mov eax, [esp + 4]   ; first argument = physical address of page directory
    mov cr3, eax         ; load page directory
    mov eax, cr0
    or  eax, 0x80000000  ; set PG bit (bit 31)
    mov cr0, eax         ; paging enabled from this instruction onwards

    ret


jump_to_entry:
    mov     eax, entry
    mov     [far_jump_target], eax
    jmp     far [far_jump_target]

far_jump_target:
    dd 0
    dw 0x08