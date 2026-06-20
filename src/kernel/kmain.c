// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#include <kernel/drivers/vga/vga.h>
#include <kernel/drivers/pic/pic.h>
#include <kernel/drivers/pit/pit.h>
#include <kernel/x86/gdt.h>
#include <kernel/x86/idt.h>
#include <kernel/x86/io.h>
#include <kernel/klib/kstdio.h>
#include <kernel/klib/kstdlib.h>
#include <kernel/include/boot.h>
#include <kernel/mm/pmm.h>
#include <kernel/mm/vmm.h>


uint8_t* _kstart;
uint8_t* _kend;
BOOT_INFO g_binfo;
uint32_t* g_page_dir;
extern volatile uint16_t* g_Screen_buff;


static inline void flush_tlb(void)
{
    uint32_t cr3;
    __asm__ __volatile__("mov %%cr3, %0" : "=r"(cr3));
    __asm__ __volatile__("mov %0, %%cr3" :: "r"(cr3));
}


void kmain(BOOT_INFO* binfo)
{
    vga_init();

    // DO NOT DEPEND ON STAGE2'S STACK
    // COPY TO KERNEL SPACE AND ACCESS FROM THERE
    kmemcpy(&g_binfo, binfo, sizeof(BOOT_INFO));


    _kstart = (uint8_t*)g_binfo.image_base;
    _kend = (uint8_t*)(g_binfo.image_base + g_binfo.size_of_image);


    // cpu uses the first 0 - 21 (22- 31 are reserved) for faults and traps
    // so we map irqs to vector 32 - 47
    pic_remap(0x20, 0x28);

    gdt_init();
    idt_init();

    pit_init();
    pit_set_freq(40);
    sti();

    kprintf("Initializing...\n"); 
    
    pmm_init(g_binfo.mmap_array, g_binfo.mmap_entries);



    uint32_t* vga_mem = vmm_reserve(); // doesnt peg it to physical mem, just returns a free virtual page
    if (!vga_mem)
    {
        kprintf("Failed to allocate vga memory!\n");
        hcf();
    }

    // map vga memory to kernel virtual address
    vmm_map(vga_mem, 0xB8000, 3);

    // set vga buff address to new virtual address
    g_Screen_buff = vga_mem;


    // undo 1-1 1mib map
    if (g_page_dir[0] & 1)
    { 
        uint32_t* page_table0 = (uint32_t*)(0xFFC00000 + (0/*idx 0*/ * 0x1000));
        for (uint32_t i = 0; i < 0x100; i++)
        {
            page_table0[i] = 0;
        }
    }
    flush_tlb();


    // undo identity map
    uint32_t phys_addr = g_binfo.load_address;
    uint32_t pages_mapped = 0;
    uint32_t total_size = g_binfo.load_address + g_binfo.bump_allocs + g_binfo.size_of_image;
    while (phys_addr < total_size)
    {
        uint32_t pd_idx = (phys_addr >> 22);
        uint32_t pt_idx = (phys_addr >> 12) & 0x3FF;

        if (!(g_page_dir[pd_idx] & 1))
        {
            // pde dont exist, skip entire 4mib region
            phys_addr = (pd_idx + 1) << 22;
            continue;
        }

        uint32_t* pt = (uint32_t*)(0xFFC00000 + (pd_idx * 0x1000));

        while (pt_idx < 1024 && phys_addr < total_size)
        {
            pt[pt_idx] = 0;
            phys_addr += 0x1000;
            pt_idx++;
            pages_mapped++;
        }   
    }
    flush_tlb();

    // print memory stats
    pmm_print_stats();

    kprintf(
    "\n"
    "              Welcome!\n"
    "       .---.         .-----------\n"
    "      //     \\  __  /    ------\n"
    "     // /     \\(  )/    -----\n"  
    "    ///////   ' \\/ `   ---\n"
    "   ///// / // :    : ---\n"
    "  /// /   /  /`    '--\n"
    " ///          //..\\\n"
    "         ====UU====UU====\n"
    "             '//||\\`\n"
    "               ''``\n"       
    );

    for (;;)
    {
        
    }
}