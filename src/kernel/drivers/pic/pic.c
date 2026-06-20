// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#include <kernel/drivers/pic/pic.h>
#include <kernel/x86/io.h>



void pic_remap(uint8_t offset1, uint8_t offset2)
{
    
    // save masks
    uint8_t mask1 = inb(PIC1_DATA);
    uint8_t mask2 = inb(PIC2_DATA);

    // start init sequence
    outb(PIC1_COMMAND, ICW1_INIT);
    outb(PIC2_COMMAND, ICW1_INIT);

    // set vector offsets
    outb(PIC1_DATA, offset1);   // big bro pic vector offset
    outb(PIC2_DATA, offset2);   // lil bro pic vector offset

    // tell them about each other
    outb(PIC1_DATA, 0x04);      // lil bro at irq2
    outb(PIC2_DATA, 0x02);

    // 8086 mode
    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);

    // restore masks
    outb(PIC1_DATA, mask1);
    outb(PIC2_DATA, mask2);

}


void pic_send_eoi(uint8_t irq)
{
    if (irq >= 8)
        // send to lil bro
        outb(PIC2_COMMAND, PIC_EOI);

    // send to big bro
    outb(PIC1_COMMAND, PIC_EOI);
}


void pic_disable()
{
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}