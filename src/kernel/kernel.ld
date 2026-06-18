OUTPUT_FORMAT(pei-i386)
ENTRY(bootstrap_entry)

SECTIONS
{
  . = SIZEOF_HEADERS;
  . = ALIGN(__section_alignment__);

  .text __image_base__ + ( __section_alignment__ < 0x1000 ? . : __section_alignment__ ) :
  {
    *(.text)
  }

  .btstrp BLOCK(__section_alignment__) :
  {
    *(.btstrp)
  }

  .data BLOCK(__section_alignment__) :
  {
    *(.data)
  }

  .bss BLOCK(__section_alignment__) :
  {
    *(.bss)
    *(COMMON)
  }

  /DISCARD/ :
  {
    *(.comment)
    *(.idata*)
    *(.note*)
    *(.debug*)
    *(.drectve)
  }
}