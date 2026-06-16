// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#pragma once

#include <boot/stage2/common.h>

#pragma pack(1)

typedef struct 
{
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint8_t num_of_fat_tables;
    uint16_t sectors_per_fat;
    uint16_t num_of_root_entries;
    
    uint32_t lba_of_fat1; 
    uint32_t lba_of_fat2;   
    uint32_t lba_of_root; 
    uint32_t lba_of_data; 
    
    uint32_t root_dir_size;    // in sectors
    uint32_t total_clusters; 
    

} FAT16_INFO;

#pragma pack()


#pragma pack(1)
typedef struct _FILE_INFO
{
    char filename[12];      
    uint32_t start_cluster; // first cluster idx
    uint32_t size;  
    uint8_t attributes;   
    uint16_t write_date;
    uint16_t write_time;
} FILE_INFO;
#pragma pack()


void fat16_init(BOOT_INFO* binfo);