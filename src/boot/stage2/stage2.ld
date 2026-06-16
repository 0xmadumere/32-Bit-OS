OUTPUT_FORMAT(binary)

SECTIONS {
    . = 0x7E00;

    .text : {
        *(.entry)
        *(.text)
    }

    .data : {
        *(.data)
        *(.rodata)
    }

    .bss : {
        *(.bss)
        *(COMMON)
    }

    /DISCARD/ : {
        *(.eh_frame)
        *(.comment)
        *(.note.GNU-stack)
    }
}