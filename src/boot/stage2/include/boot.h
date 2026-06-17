// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#pragma once
#include <boot/stage2/include/common.h>


#pragma pack(1)
typedef struct
{
    uint8_t     mode;               // -1 from dap, this is not part of dap or chs!

    union 
    {
        /* lba mode */
        struct 
        {
            uint8_t size;               // offset 0 dap size
            uint8_t reserved;           // offset 1 reserved
            uint16_t num_sectors;        // offset 2 number of sector
            uint16_t buffer_offset;      // offset 4 buff offset
            uint16_t buffer_segment;     // offset 6 buff segment
            uint32_t lba_low;            // offset 8 lba low
            uint32_t lba_high;           // offset 12 lba high
        } lba;

        /* chs mode */
        struct 
        {
            uint32_t sectors;            // offset 0 spt
            uint32_t heads;              // offset 4 num of heads
            uint16_t cylinders;          // offset 8 num of cylinder
            uint8_t sector_start;       // offset 10 sector to read
            uint8_t head_start;         // offset 11 head to read
            uint32_t cylinder_start;     // offset 12 cylinder to read
        } chs;
    };
} DAP;
#pragma pack()



#pragma pack(1)
typedef struct 
{
    uint8_t jump[3];                
    char oem_id[8];             
    uint16_t bytes_per_sector;     
    uint8_t sectors_per_cluster; 
    uint16_t reserved_sectors;    
    uint8_t fat_count;       
    uint16_t root_entry_count;   
    uint16_t total_sectors_16;    
    uint8_t media_descriptor;    
    uint16_t sectors_per_fat;    
    uint16_t sectors_per_track; 
    uint16_t head_count;  
    uint32_t hidden_sectors; 
    uint32_t total_sectors_32;  

    uint8_t drive_number;     
    uint8_t reserved;          
    uint8_t signature;     
    uint32_t volume_id;           
    char volume_label[11];    
    char fs_type[8];         

    uint8_t boot_code[448];  
    uint16_t boot_sig;  
} BPB;

#pragma pack()



#pragma pack(1)
typedef struct
{
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;       // 1 usable , 2 reserved, 3 acpi, 4 ?, 5 bad memory
    uint32_t acpi_attr;  // we ignore this
} MemoryMapEntry;
#pragma pack()


#pragma pack(1)
typedef struct
{
    uint8_t mode;
    uint8_t boot_drive;
    uint8_t num_of_drives;
    uint16_t sectors_per_track;
    uint16_t num_of_heads;
    uint16_t num_of_cylinders;
    uint32_t total_sectors;
    uint32_t partition_lba_start;
    uint16_t bytes_per_sector;
    uint16_t reserved_sectors;
    uint8_t sectors_per_cluster;
    uint8_t num_of_fat_tables;
    uint16_t max_root_entries;
    uint16_t sectors_per_fat;
    uint32_t lba_of_root;
    uint32_t lba_of_fat1;
    uint32_t lba_of_fat2;
    uint32_t lba_of_data_region;
    uint16_t num_of_root_sectors;
    MemoryMapEntry* mmap_array;
    uint16_t    mmap_entries;

} BOOT_INFO;
#pragma pack()


#pragma pack(1)
typedef struct 
{
    uint8_t cylinder;
    uint8_t head;
    uint8_t sector;
    uint8_t count;
    uint16_t segment;
    uint16_t offset;
} CHS;
#pragma pack()