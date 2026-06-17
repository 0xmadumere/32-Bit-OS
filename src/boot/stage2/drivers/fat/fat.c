// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#include <boot/stage2/include/boot.h>
#include <boot/stage2/drivers/fat/fat.h>
#include <boot/stage2/lib/string.h>
#include <boot/stage2/drivers/disk/disk.h>


static FAT16_INFO fat;


static void entry_to_filename(DIR_ENTRY* entry, char* out)
{
    int pos = 0;

    // stop at trailing space
    for (int i = 0; i < 8; i++)
    {
        if (entry->name[i] == ' ')
            break;
            
        // shift-jis 0x05 to 0xE5, a weird edge with a japanese letter
        // chances are low never but never zero
        if (i == 0 && (uint8_t)entry->name[0] == 0x05) 
        {
            out[pos++] = (char)0xE5;
        } 
        else 
        {
            out[pos++] = entry->name[i];
        }
    }


    // copy extension if not empty
    if (entry->ext[0] != ' ')
    {
        out[pos++] = '.';
        for (int i = 0; i < 3; i++)
        {
            if (entry->ext[i] == ' ')
                break;
            out[pos++] = entry->ext[i];
        }
    }

    out[pos] = '\0';
}



static uint8_t fat_short_name_checksum(const uint8_t* short_name)
{
    uint8_t sum = 0;
    for (int i = 0; i < 11; i++)
    {
        sum = ((sum & 1) ? 0x80 : 0) + (sum >> 1) + short_name[i];
    }   
    return sum;
}



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


bool fat16_find_file(const char* filename, FILE_INFO* finfo)
{
    uint8_t sector_buffer[512];
    uint32_t root_sectors = fat.root_dir_size;

    char lfn_buffer[LFN_MAX_CHARS];
    int lfn_valid = 0;
    uint8_t lfn_checksum_expected = 0;


    for (uint32_t i = 0; i < root_sectors; i++)
    {   
        if (!disk_read(fat.lba_of_root + i, 1, sector_buffer))
            return false;

        for (uint32_t j = 0; j < fat.bytes_per_sector / 32; j++)
        {
            void* bytes = &sector_buffer[j * 32];
            DIR_ENTRY* current_dir = (DIR_ENTRY*)bytes;

            if ((uint8_t)current_dir->name[0] == END_OF_DIRECTORY)
                break; // empty slot = no other entries, rest of root_dir is empty


            if ((uint8_t)current_dir->name[0] == DELETED_ENTRY)
            {
                lfn_valid = 0;
                continue;
            }

            // volume label
            if (current_dir->attributes & ATTR_VOLUME_ID)
            {
                lfn_valid = 0;
                continue;
            }

            // directories 
            if (current_dir->attributes & ATTR_DIRECTORY)
            {
                lfn_valid = 0;
                continue;
            }

            // lfn
            if (current_dir->attributes == ATTR_LFN)
            {
                LFN_ENTRY* lfn = (LFN_ENTRY*)bytes;

                uint8_t order = lfn->order & 0x1F;
                if (order == 0 || order > 20)
                {
                    lfn_valid = 0;
                    continue;
                }

                // this equations finds the position in the lfn buffer where this partitcular chunk is to be placed
                int pos = (order - 1) * 13;

                uint16_t chars[13];

                // reaassemble the full 13 char chunk
                for (int c = 0; c < 5;  c++) 
                    chars[c]      = lfn->name1[c];

                for (int c = 0; c < 6;  c++) 
                    chars[5 + c]  = lfn->name2[c];

                for (int c = 0; c < 2;  c++) 
                    chars[11 + c] = lfn->name3[c];

                
                for (int c = 0; c < 13 && (pos + c) < (LFN_MAX_CHARS - 1); c++)
                {
                    uint16_t ch = chars[c];
                    if (ch == 0x0000 || ch == 0xFFFF)
                        lfn_buffer[pos + c] = '\0';
                    else
                        lfn_buffer[pos + c] = (char)(ch & 0xFF); // assume ascii, if it wasnt, oh well
                }

                lfn_checksum_expected = lfn->checksum;
                lfn_valid = 1;
                continue;
            }


            // sne
            char dos_name[13];
            entry_to_filename(current_dir, dos_name);

            int have_lfn = 0;
            if (lfn_valid)
            {
                uint8_t actual = fat_short_name_checksum((uint8_t*)current_dir->name);
                if (actual == lfn_checksum_expected)
                {
                    lfn_buffer[LFN_MAX_CHARS - 1] = '\0';
                    have_lfn = 1;
                }
            }
            lfn_valid = 0; 

            int matched = 0;
            if (strncimp(dos_name, filename, 12) == 0)
                matched = 1;
            else if (have_lfn && strncimp(lfn_buffer, filename, LFN_MAX_CHARS) == 0)
                matched = 1;
                
            if (matched)
            {
                strncpy(finfo->filename, dos_name, 12);
                finfo->size          = current_dir->size;
                finfo->start_cluster = current_dir->low_start;
                finfo->attributes    = current_dir->attributes;
                finfo->write_date    = current_dir->write_date;
                finfo->write_time    = current_dir->write_time;
                return true;
            }
        }
    }

    return false;
}


uint32_t fat_read_file(void* address, FILE_INFO* finfo) 
{
    uint32_t lba_of_fat = fat.lba_of_fat1;
    uint16_t cluster_idx = finfo->start_cluster;
    uint16_t bytes_per_sector = fat.bytes_per_sector;
    uint32_t lba_of_cluster = ((cluster_idx - 2) * fat.sectors_per_cluster) + fat.lba_of_data;

    uint32_t fat_sector_index = (cluster_idx * 2) / bytes_per_sector;
    uint32_t entry_offset = (cluster_idx * 2) % bytes_per_sector;
    uint32_t lba_to_read = lba_of_fat + fat_sector_index;

    uint32_t clusters_read = 0;

    uint32_t cached_sector = lba_to_read; // track what's currently in sector_buffer
    uint8_t sector_buffer[512];

    if (!disk_read(lba_to_read, 1, sector_buffer))
        return 0;


    while (true)
    {
        if (!disk_read(lba_of_cluster, fat.sectors_per_cluster, address))
            return 0;

        
        clusters_read++;
        cluster_idx = *(uint16_t*)&sector_buffer[entry_offset];

        if (cluster_idx >= 0xFFF8)
            return clusters_read; 

        
        fat_sector_index = (cluster_idx * 2) / bytes_per_sector;
        entry_offset = (cluster_idx * 2) % bytes_per_sector;
        lba_to_read = lba_of_fat + fat_sector_index;

        if (lba_to_read != cached_sector)
        {
            if (!disk_read(lba_to_read, 1, sector_buffer))
                return 0;
            
            cached_sector = lba_to_read;
        }

        lba_of_cluster = ((cluster_idx - 2) * fat.sectors_per_cluster) + fat.lba_of_data;

        // advance destination pointer for next cluster's data
        address = (uint8_t*)address + (fat.sectors_per_cluster * bytes_per_sector);
        
    }

}