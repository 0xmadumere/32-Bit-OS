// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#pragma once

#include <boot/stage2/include/common.h>

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



#pragma pack(1)  // FAT dir entries are exactly 32 bytes, no padding
typedef struct 
{
    char name[8];           // File name (8 chars, uppercase, space-padded)
    char ext[3];            // Extension (3 chars, uppercase, space-padded)
    uint8_t attributes;     // 0x08=volume, 0x10=dir, 0x20=file, 0x06=hidden+sys, etc.
    uint8_t reserved;       // Reserved (0 for FAT16)
    uint8_t create_time_tenths;
    uint16_t create_time;   // Creation time (DOS format)
    uint16_t create_date;   // Creation date (DOS format)
    uint16_t access_date;   // Last access date
    uint16_t high_start;    // High 16 bits of start cluster (FAT32 only, 0 for FAT16)
    uint16_t write_time;    // Last write time
    uint16_t write_date;    // Last write date
    uint16_t low_start;     // Low 16 bits of start cluster (FAT16)
    uint32_t size;          // File size in bytes
} DIR_ENTRY;
#pragma pack()


#pragma pack(1)
typedef struct {
    uint8_t  order;
    uint16_t name1[5];
    uint8_t  attributes;   // always 0x0F
    uint8_t  type;         // always 0
    uint8_t  checksum;
    uint16_t name2[6];
    uint16_t first_cluster; // always 0
    uint16_t name3[2];
} LFN_ENTRY;
#pragma pack()


#define ATTR_LFN            0x0F
#define ATTR_DIRECTORY      0x10
#define ATTR_VOLUME_ID      0x08
#define LFN_DELETED         0xE5
#define LFN_MAX_CHARS       256
#define END_OF_DIRECTORY    0x00
#define DELETED_ENTRY       0xE5



bool fat16_find_file(const char* filename, FILE_INFO* finfo);
uint32_t fat_read_file(void* address, FILE_INFO* finfo); 
void fat16_init(BOOT_INFO* binfo);