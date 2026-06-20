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


int32_t ksprintf(char* buf, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    char temp[32];
    char* out = buf;

    while (*fmt)
    {
        if (*fmt == '%')
        {
            fmt++;
            int pad_width = 0;
            char pad_char = ' ';

            if (*fmt == '0')
            {
                pad_char = '0';
                fmt++;
            }
            while (*fmt >= '0' && *fmt <= '9')
            {
                pad_width = pad_width * 10 + (*fmt - '0');
                fmt++;
            }

            switch (*fmt)
            {
                case 'd':
                {
                    int num = va_arg(args, int);
                    kitoa((uint32_t)num, temp);
                    if (pad_width)
                    {
                        char padded[32];
                        kpad(temp, padded, pad_width, pad_char);
                        kstrcpy(out, padded);
                        out += kstrlen(padded);
                    }
                    else
                    {
                        kstrcpy(out, temp);
                        out += kstrlen(temp);
                    }
                    break;
                }
                case 'x':
                {
                    uint32_t num = va_arg(args, uint32_t);
                    kutoa_hex(num, temp, 0);
                    if (pad_width)
                    {
                        char padded[32];
                        kpad(temp, padded, pad_width, pad_char);
                        kstrcpy(out, padded);
                        out += kstrlen(padded);
                    }
                    else
                    {
                        kstrcpy(out, temp);
                        out += kstrlen(temp);
                    }
                    break;
                }
                case 'X':
                {
                    uint32_t num = va_arg(args, uint32_t);
                    kutoa_hex(num, temp, 1);
                    if (pad_width)
                    {
                        char padded[32];
                        kpad(temp, padded, pad_width, pad_char);
                        kstrcpy(out, padded);
                        out += kstrlen(padded);
                    }
                    else
                    {
                        kstrcpy(out, temp);
                        out += kstrlen(temp);
                    }
                    break;
                }
                case 's':
                {
                    const char* str = va_arg(args, const char*);
                    if (str)
                    {
                        kstrcpy(out, str);
                        out += kstrlen(str);
                    }
                    break;
                }
                case 'c':
                {
                    char c = (char)va_arg(args, int);
                    *out++ = c;
                    break;
                }
                case '%':
                {
                    *out++ = '%';
                    break;
                }
                default:
                    break;
            }
            fmt++;
        }
        else
        {
            *out++ = *fmt++;
        }
    }

    *out = '\0';
    va_end(args);
    return (int)(out - buf);
}


int32_t kprintf(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    char temp_buff[23];
    char fbuff[23];

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
                        kpad(temp_buff, fbuff, pad_width, pad_char);
                    else
                        kstrcpy(fbuff, temp_buff);
                    kputs(fbuff);
                    char_count += written;
                    break;

                case 'x':
                    uint32_t unum = va_arg(args, unsigned int);
                    kutoa_hex(unum, temp_buff, 0);
                    if (pad_width)
                        kpad(temp_buff, fbuff, pad_width, pad_char);
                    else
                        kstrcpy(fbuff, temp_buff);
                    kputs(fbuff);
                    char_count += written;
                    break;
                    
                case 'X':
                    unum = va_arg(args, unsigned int);
                    kutoa_hex(unum, temp_buff, 1);
                    if (pad_width)
                        kpad(temp_buff, fbuff, pad_width, pad_char);
                    else
                        kstrcpy(fbuff, temp_buff);
                    kputs(fbuff);
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