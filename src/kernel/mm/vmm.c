// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#include <kernel/mm/vmm.h>
#include <kernel/mm/pmm.h>

extern uint32_t* g_page_dir;


void* vmm_alloc()
{
    // scan kernel space for a free virtual page
    for (uint32_t pd_idx = (KVIRT_START >> 22); pd_idx < 1023; pd_idx++)
    {
        if (!(g_page_dir[pd_idx] & 1))
        {
            // if entire page table is absent, use first page
            uint32_t virt = pd_idx << 22;

            //allocate new physical frame to back up virtual page
            uint32_t phys = (uint32_t)pmm_alloc();
            if (phys == NULL)
                return NULL;

            // vmm_map handles the creation of the pt dw
            if (!vmm_map(virt, phys, 3))
                return NULL;
                
            return (void*)virt;
        }


        // if page table exists, scan for free entries
        uint32_t* pt = (uint32_t*)(0xFFC00000 + (pd_idx * 0x1000));
        for (uint32_t pt_idx = 0; pt_idx < 1024; pt_idx++)
        {
            if (!(pt[pt_idx] & 1))
            {
                // reconstruct virt addr
                uint32_t virt = (pd_idx << 22) | (pt_idx << 12);

                //allocate new physical frame to back up virtual page
                uint32_t phys = (uint32_t)pmm_alloc();
                if (phys == NULL)
                    return NULL;

                // map the virt page to phys fram
                // vmm_map handles pt creation if not already created
                if (!vmm_map(virt, phys, 3))
                    return NULL;

                return (void*)virt;
            }
        }


    }
    return NULL;
}


void* vmm_reserve(void)
{
    // check all kernel space addresses
    for (uint32_t pd_idx = (KVIRT_START >> 22); pd_idx < 1023; pd_idx++)
    {
        // check if entire pde is absent
        if (!(g_page_dir[pd_idx] & 1))
        {
            // if so allocate a new pt
            uint32_t* new_pt = (uint32_t*)pmm_alloc();
            if (!new_pt)
                return NULL;

            // create pde
            g_page_dir[pd_idx] = ((uint32_t)new_pt) | 3;

            // flush tlb
            __asm__ __volatile__("invlpg (%0)" :: "r"(0xFFC00000 + (pd_idx * 0x1000)) : "memory");

            // zero out pt
            uint32_t* pt = (uint32_t*)(0xFFC00000 + (pd_idx * 0x1000));
            for (uint32_t i = 0; i < 1024; i++)
                pt[i] = 0;

            // reconstruct virt addr
            uint32_t virt = pd_idx << 22;

            // set present bit to 1, so this page cant be given out by vmm_reserve or vmm_alloc again
            // no physical frame backing yet
            pt[0] = 1;

            // return virtual page
            return (void*)virt;
        }

        // if pde was valid for current iter, loop through all pte
        uint32_t* pt = (uint32_t*)(0xFFC00000 + (pd_idx * 0x1000));
        for (uint32_t pt_idx = 0; pt_idx < 1024; pt_idx++)
        {
            // if present bit is 0 in a pte
            // then claim it and set present bit to 1
            if (!(pt[pt_idx] & 1))
            {
                // mark claimed
                pt[pt_idx] = 1;
                
                // reconstruct virtual addr and return it
                uint32_t virt = (pd_idx << 22) | (pt_idx << 12);
                return (void*)virt;
            }
        }
    }

    // no free space
    return NULL;
}


void vmm_free(void* virt)
{
    uint32_t v = (uint32_t)virt;
    uint32_t pd_idx = v >> 22;
    uint32_t pt_idx = (v >> 12) & 0x3FF;

    uint32_t* pd = (uint32_t*)0xFFFFF000;
    if (!(pd[pd_idx] & 1))
    // page table doesnt exist, nothing to free
        return;

    uint32_t* pt = (uint32_t*)(0xFFC00000 + (pd_idx * 0x1000));
    if (!(pt[pt_idx] & 1))
    // present bit is already zero
        return; 

    uint32_t phys = pt[pt_idx] & 0xFFFFF000;
    // give the physical frame back to the pmm
    pmm_free((void*)phys);   

     // clear the pt entry
    pt[pt_idx] = 0;       
    __asm__ __volatile__("invlpg (%0)" :: "r"(v) : "memory");
}



int32_t vmm_map(void* virt, void* phys, uint32_t flags)
{
    uint32_t v = (uint32_t)virt, p = (uint32_t)phys;
    uint32_t pd_idx = v >> 22;
    uint32_t pt_idx = v >> 12 & 0x3FF;
    uint32_t* pt;

    if (!(g_page_dir[pd_idx] & 1))
    {
        // page table doesnt exist, allocate a new one
        uint32_t* new_pt = (uint32_t*)pmm_alloc();
        if (new_pt == NULL)
            return 0;

        g_page_dir[pd_idx] = ((uint32_t)new_pt) | 3;

        // flush tlb for the new page table
        __asm__ __volatile__("invlpg (%0)" :: "r"(0xFFC00000 + (pd_idx * 0x1000)) : "memory");
        
        // zero it out through the recursive window
        pt = (uint32_t*)(0xFFC00000 + (pd_idx * 0x1000));
        for (uint32_t i = 0; i < 1024; i++)
            pt[i] = 0;
    }


    pt = (uint32_t*)(0xFFC00000 + (pd_idx * 0x1000));
    pt[pt_idx] = p | flags;

    __asm__ __volatile__("invlpg (%0)" :: "r"(virt) : "memory");

    return 1;
}