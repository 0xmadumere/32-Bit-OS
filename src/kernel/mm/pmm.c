// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#include <kernel/mm/pmm.h>
#include <kernel/klib/kstdio.h>
#include <kernel/include/boot.h>

extern uint8_t* _kstart;
extern uint8_t* _kend;
extern BOOT_INFO g_binfo;

uint8_t* g_bitmap;
uint32_t g_total_ram = 0;
uint32_t g_total_frames = 0;
uint32_t g_used_frames = 0;


static inline void bitmap_set(uint32_t frame)
{
    g_bitmap[frame / 8] |= (1 << (frame % 8));
}

 
static inline void bitmap_clear(uint32_t frame)
{
    g_bitmap[frame / 8] &= ~(1 << (frame % 8));
}

 
static inline uint8_t bitmap_test(uint32_t frame)
{
    return g_bitmap[frame / 8] & (1 << (frame % 8));
}


// mark a byte range of physical memory as used
static void pmm_mark_used(void* base, uint32_t length)
{
    // frames are 0 indexed
    // round down to the nearest frame idx, so if base = 0x1050, 0x1050 / 0x1000 = 1, so first frame is = 1
    uint32_t first_frame = ((uint32_t)base) / PAGE_SIZE;

    // this round up the the nearest full page length
    // e.g if length is 0xFFF, 0xFFF + 0x1000 = 0x1FFF - 1 = 0x1FFE / 0x1000 = 1
    // so therefore num of frames is 1  
    uint32_t num_frames  = (length + PAGE_SIZE - 1) / PAGE_SIZE;
 

    for (uint32_t i = 0; i < num_frames; i++)
    {
        uint32_t frame = first_frame + i;

        // if we have 1 frame, max frame index is = total frames - 1, which is 0
        // so we make sure the frame is valid and that it is actually free
        if (frame < g_total_frames && !bitmap_test(frame))
        {
            bitmap_set(frame);
            g_used_frames++;
        }
    }
}


static void pmm_mark_free(void* base, uint32_t length)
{
    // frames are 0 indexed
    // round down to the nearest frame idx, so if base = 0x1050, 0x1050 / 0x1000 = 1, so first frame is = 1
    uint32_t first_frame = ((uint32_t)base) / PAGE_SIZE;

    
    // this round up the the nearest full page length
    // e.g if length is 0xFFF, 0xFFF + 0x1000 = 0x1FFF - 1 = 0x1FFE / 0x1000 = 1
    // so therefore num of frames is 1  
    uint32_t num_frames  = (length + PAGE_SIZE - 1) / PAGE_SIZE;
 
    for (uint32_t i = 0; i < num_frames; i++)
    {
        uint32_t frame = first_frame + i;

        // if we have 1 frame, max frame index is = total frames - 1, which is 0
        // so we make sure the frame is valid and that it is actually taken
        if (frame < g_total_frames && bitmap_test(frame))
        {
            bitmap_clear(frame);
            g_used_frames--;
        }
    }
}


void* pmm_alloc_region(void* base, uint32_t length)
{
    uint32_t first_frame = ((uint32_t)base) / PAGE_SIZE;

    uint32_t num_frames  = (length + PAGE_SIZE - 1) / PAGE_SIZE;

    // make sure all frames in region are valid and not already taken
    for (uint32_t i = 0; i < num_frames; i++)
    {
        uint32_t frame = first_frame + i;
        if (frame >= g_total_frames || bitmap_test(frame))
            return NULL;
    }

    pmm_mark_used(base, length);

    return (void*)base;
}


// free a block/region of frames
void pmm_free_region(void* base, uint32_t length)
{
    pmm_mark_free(base, length);
}


// allocate a fram
void* pmm_alloc(void)
{
    // loop through free frames
    for (uint32_t frame = 0; frame < g_total_frames; frame++)
    {
        // if frame isnt taken, claim it and mark as taken
        if (!bitmap_test(frame))
        {
            bitmap_set(frame);
            g_used_frames++;

            return (uint32_t*)(frame * PAGE_SIZE);
        }
    }
    return NULL;  // out of memory 
}



// free a previously allocated frame
void pmm_free(void* addr)
{
    uint32_t frame = (uint32_t)addr / PAGE_SIZE;

    // make the the frame is a valid frame idx and was actually taken
    if (frame < g_total_frames && bitmap_test(frame))
    {
        bitmap_clear(frame);
        g_used_frames--;
    }
}


void pmm_init(MemoryMapEntry* mmap, uint32_t entries)
{
    
    // compute total frames from highest usable e820 address
    uint32_t total_ram = 0;
    for (uint8_t i = 0; i < entries; i++)
    {
        if (mmap[i].type == 1)
        {
            uint32_t top = (uint32_t)(mmap[i].base_addr + mmap[i].length);
            if (top > total_ram)
                total_ram = top;
        }
    }
    g_total_ram = total_ram;

    // round down, so if total ram = 0x2050, that mean 2 frames
    // this ensures we always use full 4096 byte frames
    uint32_t total_frames = total_ram / 4096;   
    g_total_frames = total_frames;   


    // adding 7 to the total frames, before dividing ensures effiency and no wasted bits
    // e.g total frames = 15, 15 + 7 = 22, 22 / 8 = 2(round down), meaning bitmap would be 2 bytes
    // which is enough to track 15 frames
    uint32_t bitmap_bytes = (total_frames + 7) / 8;


    // bit map starts at end of kernel
    uint8_t* bitmap = (uint8_t*)_kend;
    g_bitmap = bitmap;


    // mark all bytes/bits in bitmap as used, initially
    for (uint32_t i = 0; i < bitmap_bytes; i++)
    {   
        g_bitmap[i] = 0xFF;
    }
    g_used_frames = total_frames;


    // for each usable e820 region, mark as free 
    for (uint8_t i = 0; i < entries; i++)
    {
        if (mmap[i].type == MMAP_TYPE_FREE)
        {
            // only low 32 bits (no PAE) 
            uint32_t base   = (uint32_t)(mmap[i].base_addr & 0xFFFFFFFF);
            uint32_t length = (uint32_t)(mmap[i].length    & 0xFFFFFFFF);
            pmm_mark_free(base, length);
        }
    }


    // calculate used region
    uint32_t used_region = g_binfo.size_of_image + g_binfo.bump_allocs;

    // mark entire kernel + bump alloc frames as used
    pmm_mark_used(g_binfo.load_address, used_region);

    // mark the first segment as used
    pmm_alloc_region(0, 0x10000);
    
}


void pmm_print_stats(void)
{

    uint32_t used_bytes   = g_used_frames * PAGE_SIZE;
    uint32_t total_mb     = g_total_ram >> 20;

    uint32_t percent_used = (g_used_frames * 100) / g_total_frames;

    // format total ram
    char total_str[32];
    if (total_mb >= 1024)
    {
        uint32_t total_gb  = total_mb / 1024;
        uint32_t total_dec = ((total_mb % 1024) * 100) / 1024;
        ksprintf(total_str, "%d.%02d GiB", total_gb, total_dec);
    }
    else
    {
        ksprintf(total_str, "%d MiB", total_mb);
    }

    // format used ram
    char used_str[32];
    uint32_t used_kb = used_bytes / 1024;
    uint32_t used_mb = used_kb / 1024;
    uint32_t used_gb  = used_mb / 1024;
    uint32_t used_dec;

    if (used_kb < 1024)  // less than 1 mib
    {
        used_dec = ((used_bytes % 1024) * 100) / 1024;
        ksprintf(used_str, "%d.%02d KiB", used_kb, used_dec);
    }
    else if (used_mb < 11)  // less than 11 mib 
    {
        used_dec = ((used_kb % 1024) * 100) / 1024;
        ksprintf(used_str, "%d.%02d MiB", used_mb, used_dec);
    }
    else // greater than or eq to 1 gib
    {
        used_dec = ((used_mb % 1024) * 100) / 1024;
        ksprintf(used_str, "%d.%02d GiB", used_gb, used_dec);
    }

    kprintf("%s / %s (%d%%)\n", used_str, total_str, percent_used);
}


uint32_t pmm_total_frames(void) { return g_total_frames; }
uint32_t pmm_used_frames(void)  { return g_used_frames;  }