// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#include <kernel/drivers/vga/vga.h>
#include <kernel/klib/kstdio.h>
#include <kernel/klib/kstring.h>
#include <stdarg.h>


void kputchar(int c)
{
    vga_putchar(c);
}


void kputs(const char* str)
{
    while (*str)
    {
        kputchar(*str++);
    }
}


uint32_t kprintf(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    char temp_buff[23];
    float fbuff[23];

    int char_count = 0;
    uint16_t written = 0;

    if (format == NULL)
    {
        return 0;
    }
    
    while (*format)
    {
        if (*format == '%')
        {
            format++;
            int pad_width = 0;
            char pad_char = ' ';
            int left_align = 0; 

            if (*format == '-')
            {
                left_align = 1;
                format++;
            }
            if (*format == '0')
            {
                pad_char = '0';
                format++;
            }
            while (*format >= '0' && *format <= '9')
            {
                pad_width = pad_width * 10 + (*format - '0');
                format++;
            }

            switch (*format)
            {
                case 's':
                {
                    const char* str = va_arg(args, const char*);
                    if (str)
                    {
                        if (pad_width && left_align)
                        {
                            kputs(str);
                            int len = 0;
                            while (str[len]) len++;
                            while (len < pad_width)
                            {
                                kputchar(' ');
                                len++;
                            }
                        }
                        else
                        {
                            kputs(str);
                        }
                    }
                    break;
                }

                case 'c':
                    char c = (uint8_t)va_arg(args, int);
                    kputchar(c);
                    char_count += written;
                    break;

                case 'd':
                    int num = va_arg(args, int);
                    kitoa(num, temp_buff);
                    if (pad_width)
                        kpad(temp_buff, (char*)fbuff, pad_width, pad_char);
                    else
                        kstrcpy((char*)fbuff, temp_buff);
                    kputs((char*)fbuff);
                    char_count += written;
                    break;

                case 'x':
                    uint32_t unum = va_arg(args, unsigned int);
                    kutoa_hex(unum, temp_buff, 0);
                    if (pad_width)
                        kpad(temp_buff, (char*)fbuff, pad_width, pad_char);
                    else
                        kstrcpy((char*)fbuff, temp_buff);
                    kputs((char*)fbuff);
                    char_count += written;
                    break;
                    
                case 'X':
                    unum = va_arg(args, unsigned int);
                    kutoa_hex(unum, temp_buff, 1);
                    if (pad_width)
                        kpad(temp_buff, (char*)fbuff, pad_width, pad_char);
                    else
                        kstrcpy((char*)fbuff, temp_buff);
                    kputs((char*)fbuff);
                    char_count += written;
                    break;
                
                case '%':
                    kputchar('%');
                    char_count += written;
                    break;

                default:
                    break;
            }
            format++;
        }
        else
        {
            kputchar(*format);
            char_count += written;
            format++;
        }
    }

}