// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#include <kernel/include/boot.h>
#include <kernel/init/bootstrap.h>
#include <kernel/klib/kstdlib.h>

extern uint32_t* g_page_dir;


static void bump_init(bump_t* bump, uint32_t base, BOOT_INFO* binfo)
{
    // align base to page boundary
    bump->base = (base + 4095) & ~0xFFF;
    bump->next = base;

    uint32_t total_ram = 0;
    for (uint32_t i = 0; i < binfo->mmap_entries; i++)
    {
        if (binfo->mmap_array[i].type == 1)
        {
            uint32_t top = (uint32_t)(binfo->mmap_array[i].base_addr + binfo->mmap_array[i].length);
            if (top > total_ram)
                total_ram = top;
        }
    }

    bump->end = total_ram;
}



static uint32_t bump_alloc(bump_t* bump, uint32_t size)
{
    // round up to page boundary
    size = (size + 4095) & ~0xFFF;
    if (bump->next + size > bump->end)
    // out of memory
        return 0; 

    uint32_t addr = bump->next;
    bump->next += size;
    memset((void*)addr, 0x00, size);
    return addr;
}



int32_t bootstrap_entry(BOOT_INFO* binfo)
{

    bump_t bump;
    uint32_t base = binfo->load_address + binfo->size_of_image;
    bump_init(&bump, base, binfo);


    // calculate frames required for bitmap
    uint32_t total_frames = bump.end / 4096;
    uint32_t bitmap_bytes = (total_frames + 7) / 8;
    uint32_t bitmap_pages = (bitmap_bytes + 4095) / 4096;

    __asm__ __volatile__("xchg %bx, %bx");
    // allocate frames for bitmap
    uint8_t* bitmap = (uint8_t*)bump_alloc(&bump, (bitmap_pages * 0x1000)); 


    uint32_t* page_dir = (uint32_t*)bump_alloc(&bump, 0x1000);
    if (page_dir == 0)
        return 1;


    // recursive map, allows us to edit page dir and tables after paging is on
    page_dir[1023] = ((uint32_t)page_dir) | 3;  


    // create page table for 1-1 mib map
    uint32_t* page_table0 = (uint32_t*)bump_alloc(&bump, 0x1000);
    if (page_table0 == 0)
        return 1;


    // 1-1 map first 1mib, our stack is somewhere here and vga memory is here
    // if we dont 1-1 map, we are in big trouble
    for (uint32_t i = 0; i < 0x100; i++)
    {
        page_table0[i] = (i * 0x1000) | 3;
    }

    // save pde
    page_dir[0] = ((uint32_t)page_table0) | 3;


    // identity map entire kernel + bump allocs
    uint32_t phys_addr = binfo->load_address;
    uint32_t pages_mapped = 0;

    // re checks bump.next every iteration
    // so if we allocate inside the loop, that page table will be mapped as well
    while (phys_addr < bump.next)
    {
        uint32_t pd_idx = (phys_addr >> 22);
        uint32_t pt_idx = (phys_addr >> 12) & 0x3FF;
        uint32_t* pt;

        if (page_dir[pd_idx] & 1)
        {
            pt = (uint32_t*)(page_dir[pd_idx] & 0xFFFFF000);
        }
        else
        {
            pt = (uint32_t*)bump_alloc(&bump, 0x1000);  
            if (pt == 0)
                return 1;
        }

        while (pt_idx < 1024 && phys_addr < bump.next)
        {
            pt[pt_idx] = phys_addr | 3;
            phys_addr += 0x1000;
            pt_idx++;
            pages_mapped++;
        }   
        page_dir[pd_idx] = ((uint32_t)pt) | 3;
    }



    // map kernels image base(high address) + bump alloc to their physical address
    phys_addr = binfo->load_address;
    uint32_t virt_addr = binfo->image_base;
    pages_mapped = 0;

    // re checks bump.next every iteration
    // so if we allocate inside the loop, that page table will be mapped as well
    while (phys_addr < bump.next)   
    {
        uint32_t pd_idx = (virt_addr >> 22);
        uint32_t pt_idx = (virt_addr >> 12) & 0x3FF;
        uint32_t* pt;


        if (page_dir[pd_idx] & 1)
        {
            pt = (uint32_t*)(page_dir[pd_idx] & 0xFFFFF000);
        }
        else
        {
            pt = (uint32_t*)bump_alloc(&bump, 0x1000);
            if (pt == 0)
                return 1;

        }

        while (pt_idx < 1024 && phys_addr < bump.next)
        {
            pt[pt_idx] = phys_addr | 3;
            phys_addr += 0x1000;
            virt_addr += 0x1000;
            pt_idx++;
            pages_mapped++;
        }
        page_dir[pd_idx] = ((uint32_t)pt) | 3;
    }

    // total bump allocations
    binfo->bump_allocs = bump.next - bump.base;
    

    enable_paging((uint32_t)page_dir);


    // recurive page dir entry trick
    // now we dont need to know the physcial address of a page table or the page directory 
    g_page_dir = (uint32_t*)0xFFFFF000;
    

    // start executing from virtual address, not identity mapping
    // then call kernel entry fn
    jump_to_entry(binfo);

}