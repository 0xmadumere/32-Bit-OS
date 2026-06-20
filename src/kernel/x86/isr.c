// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#include <kernel/x86/isr.h>
#include <kernel/x86/idt.h>
#include <kernel/x86/io.h>
#include <kernel/drivers/pic/pic.h>
#include <kernel/klib/kstdio.h>

extern irq_handler_t irq_handlers[16];


void isr_handler(Registers regs)
{
    if (regs.int_no < 32)
    {
         kprintf("caught exception: #%d\n", regs.int_no);

        kprintf("eax=0x%08X ebx=0x%08X ecx=0x%08X edx=0x%08X\n", regs.eax, regs.ebx, regs.ecx, regs.edx);
        kprintf("esi=0x%08X edi=0x%08X ebp=0x%08X esp=0x%08X\n", regs.esi, regs.edi, regs.ebp, regs.esp);
        kprintf("eip=0x%08X cs=0x%08X eflags=0x%08X\n", regs.eip, regs.cs, regs.eflags);
        kprintf("useresp=0x%08X ss=0x%08X ds=0x%08X\n", regs.useresp, regs.ss, regs.ds);
        kprintf("int_no=%d err_code=0x%08X\n", regs.int_no, regs.err_code);
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