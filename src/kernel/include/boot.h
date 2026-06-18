// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#include <kernel/include/common.h>


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
    uint32_t image_base;
    uint32_t size_of_image;
    uint32_t load_address;
} BOOT_INFO;
#pragma pack()