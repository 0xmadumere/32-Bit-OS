// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#include <boot/stage2/include/boot.h>
#include <boot/stage2/drivers/disk/disk.h>
#include <boot/stage2/bios/bios.h>

extern uint8_t g_mode;
extern BOOT_INFO g_boot_info;


int disk_read(uint32_t lba, uint16_t count, void* address)
{
    CHS chs;
    DAP dap;

    if (g_mode)
    {
        dap.lba.size = 16; // 0x10
        dap.lba.reserved = 0x00;
        dap.lba.lba_low = lba;
        dap.lba.num_sectors = count;
        dap.lba.buffer_segment = SEG(address);
        dap.lba.lba_high = 0x00000000; // ignore lba high
        dap.lba.buffer_offset = OFF(address);

        return bios_ext_read((DAP*)&dap.lba);
    }

    uint8_t cylinder = (lba / g_boot_info.sectors_per_track) / g_boot_info.num_of_heads;

    uint8_t sector = (lba % g_boot_info.sectors_per_track) + 1;

    uint8_t head = (lba / g_boot_info.sectors_per_track) % g_boot_info.num_of_heads;

    // max gemoetry for a floppy, if chs mode enabled we assume floppy
    if (cylinder > 79 || head > 1 || sector > 36)
        return 0;

    chs.cylinder = cylinder;
    chs.sector = sector;
    chs.head = head;
    chs.offset = OFF(address);
    chs.segment = SEG(address);
    chs.count = count;

    return bios_chs_read(&chs);

}