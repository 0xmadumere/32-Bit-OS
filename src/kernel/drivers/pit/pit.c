// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#include <kernel/drivers/pit/pit.h>
#include <kernel/x86/idt.h>
#include <kernel/x86/io.h>

uint32_t system_ticks = 0;
static uint32_t current_pit_freq = 18;   // default before we reprogram it

uint32_t pit_get_freq(void)
{
    return current_pit_freq;
}


void pit_init() 
{
    irq_register_handler(0, pit_handler);
}

void pit_set_freq(uint32_t frequency)
{
    if (frequency < 18 || frequency > 1193182)
        frequency = 18;

    uint16_t divisor = 1193182 / frequency;
    
    // send comamnd byte
    outb(0x43, 0x36);
    
    // send the divisor low byte
    outb(0x40, divisor & 0xFF);
    
    // send the divisor high byte
    outb(0x40, (divisor >> 8) & 0xFF);

    current_pit_freq = 1193182 / divisor;
}


void pit_handler()
{
    system_ticks++;
}

uint32_t pit_get_tick()
{
    return system_ticks;
}