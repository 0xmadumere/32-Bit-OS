// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#include <kernel/x86/isr.h>
#include <kernel/x86/idt.h>
#include <kernel/x86/io.h>
#include <kernel/drivers/pic/pic.h>

extern irq_handler_t irq_handlers[16];


void isr_handler(Registers regs)
{
    if (regs.int_no < 32)
    {
        hcf();
    }
    else
    {
        uint8_t irq = regs.int_no - 32;
 
        if (irq_handlers[irq])
            irq_handlers[irq](&regs);
 
        pic_send_eoi(irq);

    }

}