// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#include <boot/stage2/drivers/loader/loader.h>
#include <boot/stage2/lib/stdlib.h>
#include <boot/stage2/lib/string.h>


PE_LOAD_RESULT load_pe(uint8_t* pe_image, void* dest_base)
{

    PE_LOAD_RESULT result;

    memset(&result, 0x00, sizeof(PE_LOAD_RESULT));

    PIMAGE_DOS_HEADER dos_header = (PIMAGE_DOS_HEADER)pe_image;

    if (dos_header->e_magic != IMAGE_DOS_SIGNATURE)
        return result;

	PIMAGE_NT_HEADERS32 nt = (PIMAGE_NT_HEADERS32)(pe_image + dos_header->e_lfanew);

    if (nt->Signature != IMAGE_NT_SIGNATURE)
        return result;

    if (nt->FileHeader.Machine != IMAGE_FILE_MACHINE_I386)
        return result;
    
    if (nt->FileHeader.SizeOfOptionalHeader != sizeof(IMAGE_OPTIONAL_HEADER32)) // 224 bytes for PE32
        return result;

    if (nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC)
        return result;
    

    uint32_t size_of_image = nt->OptionalHeader.SizeOfImage;
	uint32_t image_base = nt->OptionalHeader.ImageBase;
	uint32_t entry_point_rva = nt->OptionalHeader.AddressOfEntryPoint;
	uint16_t number_of_sections = nt->FileHeader.NumberOfSections;
	uint32_t size_of_headers = nt->OptionalHeader.SizeOfHeaders;

    // copy headers
    memcpy(dest_base, pe_image, size_of_headers);

    // copy sections
	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(nt);
	for (uint32_t i = 0; i < number_of_sections; i++)
	{
		void* dest = (uint8_t*)dest_base + section->VirtualAddress;
		void* src = pe_image + section->PointerToRawData;
		uint32_t section_size = section->SizeOfRawData;

        // handle bss type sections
        if (section->SizeOfRawData == 0 && section->Misc.VirtualSize > 0)
        {
            memset(dest, 0x00, section->Misc.VirtualSize);
            section++;
            continue;
        }

		memcpy(dest, src, section_size);
		section++;
	}

    result.entry_point = (void*)(entry_point_rva + (uint8_t*)dest_base);
    result.image_base = image_base;
    result.size_of_image = size_of_image;
    return result;

    
}
