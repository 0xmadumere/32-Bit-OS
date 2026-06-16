// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#include <boot/stage2/common.h>
#include <boot/stage2/boot.h>
#include <boot/stage2/drivers/fat.h>


void stage2_entry(DAP* dap, BPB* bpb)
{

    BOOT_INFO boot_info;

    boot_info.mode = dap->mode;

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


    fat16_init(&boot_info);


}