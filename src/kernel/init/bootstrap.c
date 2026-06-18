
#include <kernel/include/common.h>
#include <kernel/include/boot.h>

void enable_paging();
void jump_to_entry();

__attribute__((section(".btstrp"))) 
void* memset(void *dest, int val, uint32_t count) 
{
    unsigned char *ptr = (unsigned char *)dest;
    
    while (count > 0) 
    {
        *ptr = (unsigned char)val;
        ptr++;
        count--;
    }

    return dest;
}


__attribute__((section(".btstrp"))) 
void bootstrap_entry(BOOT_INFO* binfo)
{
    uint32_t* page_directory = (uint32_t*)0x1000;
    uint32_t* page_table0 = (uint32_t*)0x2000;

    memset(page_directory, 0, 4096);   
    memset(page_table0, 0, 4096); 
    
    page_directory[0] = ((uint32_t)page_table0) | 3;

    // 1-1 map first 4MiB of phsical space
    // if kernel is bigger than 3MiB will break
    // must also be loaded preferably at 0x100000
    for (uint32_t i = 0; i < 0x400; i++)
        page_table0[i] = (i * 0x1000) | 3;

    uint32_t* page_table1 = (uint32_t*)0x3000;
    

    // map first 4MiB of kernels virtual base to its physical address
    
    uint16_t pd_idx = binfo->image_base >> 22;

    page_directory[pd_idx] = ((uint32_t)page_table1) | 3;
    uint16_t pt_idx = (binfo->image_base >> 12) & 0x3FF;

    for (uint32_t i = 0; i < 0x400; i++)
    {
        page_table1[pt_idx + i] = (binfo->load_address + (i * 0x1000)) | 3;
    }


    enable_paging((uint32_t)page_directory);


    __asm__ __volatile__("xchg %bx, %bx");
    jump_to_entry();


}
