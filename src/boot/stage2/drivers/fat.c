// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#include <boot/stage2/boot.h>
#include <boot/stage2/drivers/fat.h>


static FAT16_INFO fat;


void fat16_init(BOOT_INFO* binfo)
{

    fat.bytes_per_sector = binfo->bytes_per_sector;
    fat.sectors_per_cluster = binfo->sectors_per_cluster;
    fat.num_of_fat_tables = binfo->num_of_fat_tables;
    fat.sectors_per_fat = binfo->sectors_per_fat;
    fat.num_of_root_entries = binfo->max_root_entries;
    
    fat.lba_of_fat1 = binfo->lba_of_fat1;
    fat.lba_of_fat2 = binfo->lba_of_fat2;
    fat.lba_of_root = binfo->lba_of_root;
    fat.lba_of_data = binfo->lba_of_data_region;
    

    fat.root_dir_size = binfo->num_of_root_sectors;
    

    uint32_t data_sectors = binfo->total_sectors
                        - binfo->reserved_sectors      
                        - binfo->sectors_per_fat * binfo->num_of_fat_tables
                        - fat.root_dir_size;


    fat.total_clusters = data_sectors / binfo->sectors_per_cluster;

}