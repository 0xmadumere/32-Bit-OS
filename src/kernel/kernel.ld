OUTPUT_FORMAT(binary)

SECTIONS {
    . = 0x100000;

    _kstart = .;

    .text : {
        *(.entry)
        *(.text)
    }

    .data : {
        *(.data)
        *(.rodata)
    }

    .bss : {
        _bss_start = .;
        *(.bss)
        *(COMMON)
        _bss_end = .;
    }

    _kend = .;

    /DISCARD/ : {
        *(.eh_frame)
        *(.comment)
        *(.note.GNU-stack)
    }
}