// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#include <kernel/include/boot.h>
#include <kernel/drivers/vga/vga.h>
#include <kernel/drivers/pic/pic.h>
#include <kernel/drivers/pit/pit.h>
#include <kernel/x86/gdt.h>
#include <kernel/x86/idt.h>
#include <kernel/x86/io.h>
#include <kernel/klib/kstdio.h>

extern uint8_t _kstart[];
extern uint8_t _kend[];


void kmain(BOOT_INFO* binfo)
{
    vga_init();

    // cpu uses the first 0 - 21 (22- 31 are reserved) for faults and traps
    // so we map irqs to vector 32 - 47
    pic_remap(0x20, 0x28);

    gdt_init();
    idt_init();

    pit_init();
    pit_set_freq(40);
    sti();

    kprintf("Intializing....\n");

    kprintf(
    "\n"
    "              Welcome!\n"
    "       .---.         .-----------\n"
    "      //     \\  __  /    ------\n"
    "     // /     \\(  )/    -----\n"  
    "    ///////   ' \\/ `   ---\n"
    "   ///// / // :    : ---\n"
    "  /// /   /  /`    '--\n"
    " ///          //..\\\n"
    "         ====UU====UU====\n"
    "             '//||\\`\n"
    "               ''``\n"       
    );


    for (;;)
    {

    }    
}