// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#include <boot/stage2/include/common.h>
#include <boot/stage2/include/boot.h>
#include <boot/stage2/drivers/fat/fat.h>
#include <boot/stage2/bios/bios.h>
#include <boot/stage2/include/stage2.h>
#include <boot/stage2/lib/stdlib.h>
#include <boot/stage2/drivers/loader/loader.h>

BOOT_INFO g_boot_info;
uint8_t g_mode;


void stage2_entry(DAP* dap, BPB* bpb)
{

    BOOT_INFO boot_info;

    boot_info.mode = dap->mode;
    g_mode = dap->mode;

    boot_info.sectors_per_track = dap->chs.sectors;
    boot_info.num_of_heads = dap->chs.heads;
    boot_info.num_of_cylinders = dap->chs.cylinders;

    boot_info.total_sectors = bpb->total_sectors_16 == 0 ? bpb->total_sectors_32 : bpb->total_sectors_16;

    boot_info.partition_lba_start = bpb->hidden_sectors;

    boot_info.bytes_per_sector = bpb->bytes_per_sector;

    boot_info.reserved_sectors = bpb->reserved_sectors;

    boot_info.sectors_per_cluster = bpb->sectors_per_cluster;

    boot_info.num_of_fat_tables = bpb->fat_count;

    boot_info.max_root_entries = bpb->root_entry_count;

    boot_info.sectors_per_fat = bpb->sectors_per_fat;

    boot_info.lba_of_fat1 = boot_info.partition_lba_start + boot_info.reserved_sectors;

    boot_info.lba_of_fat2 = boot_info.lba_of_fat1 + boot_info.sectors_per_fat;

    boot_info.lba_of_root = boot_info.lba_of_fat2 + boot_info.sectors_per_fat;

    boot_info.num_of_root_sectors = ((
        (bpb->root_entry_count << 5) + boot_info.bytes_per_sector) - 1) / boot_info.bytes_per_sector;

    boot_info.lba_of_data_region = boot_info.num_of_root_sectors + boot_info.lba_of_root;

    g_boot_info = boot_info;


    fat16_init(&boot_info);


    FILE_INFO finfo;

    if (!fat16_find_file("kernel.bin", &finfo))
    {
        bios_puts("Failed to locate kernel!\r\n");
        return;
    }


    uint32_t clusters_read = fat_read_file((void*)KERNEL_BUFFER_ADDRESS, &finfo);

    uint32_t bytes_per_cluster = boot_info.sectors_per_cluster * boot_info.bytes_per_sector;
    
    uint32_t bytes_written = clusters_read * bytes_per_cluster;

    uint32_t expected_clusters = (finfo.size + bytes_per_cluster - 1) / bytes_per_cluster;

    if (clusters_read != expected_clusters)
    {
        bios_puts("Failed read kernel from disk!\r\n");
        return;
    }


    MemoryMapEntry mmap[MAX_MM_ENTRIES];

    uint8_t entries = bios_get_mmap(mmap, MAX_MM_ENTRIES);

    if (entries == 0)
    {
        bios_puts("Failed to get memory map!\r\n");
        return;
    }


    boot_info.mmap_array = mmap;
    boot_info.mmap_entries = entries;

    bios_puts("Loading kernel...\r\n");

    PE_LOAD_RESULT result = load_pe((uint8_t*)KERNEL_BUFFER_ADDRESS, (void*)KERNEL_LOAD_ADDR);
    if (result.entry_point == NULL)
    {
        bios_puts("Failed to load kernel!\r\n");
        return;
    }

    kernel_entry_t kmain = (kernel_entry_t)result.entry_point;

    boot_info.load_address = KERNEL_LOAD_ADDR;
    boot_info.image_base = result.image_base;
    boot_info.size_of_image = result.size_of_image;

    bios_puts("Loaded kernel\r\n");

    kmain(&boot_info);


    return;

}