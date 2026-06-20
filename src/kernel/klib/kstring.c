// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#include <kernel/klib/kstring.h>


uint16_t kstrlen(const char* str)
{
	const char* s = str;

	while (*str)
		str++;

	return str - s;
}

char* kstrcpy(char* dest, const char* src)
{
    char* d = dest;

	if (!dest || !src)
		return dest;

	while (*src)
		*dest++ = *src++;

	*dest = '\0';

	return d;
}


char* kstrncpy(char* dest, const char* src, size_t count)
{
	if (dest == NULL || src == NULL || count == 0)
		return dest;

	char* d = dest;

	while (count--)
	{
		if (*src)
			*dest++ = *src++;
		else
			*dest++ = '\0';
	}

	return d;
}


int ktolower(int c)
{
	if (c >= 'A' && c <= 'Z')
		c += 32;

	return c;
}


char ktoupper(char c) 
{
    // check if the char is lowercase
    if (c >= 'a' && c <= 'z')
        return c - 32;
    return c;
}



int kstrncimp(const char* str1, const char* str2, size_t count)
{
	if (!str1 || !str2 || !count)
		return -1;


    while (--count && (*str1 && *str2))
    {
        int c1 = ktolower(*str1++);
        int c2 = ktolower(*str2++);

        if (c1 != c2)
            return c1 - c2;

    }

    return ktolower(*str1) - ktolower(*str2);
}


char* kstrrev(char* str)
{
    uint32_t len, i;

	if (!str)
		return str;

	len = kstrlen(str);

	for (i = 0; i < (len / 2); i++)
	{
		char temp = str[i];
		str[i] = str[len - 1 - i];
		str[len - 1 - i] = temp;
	}

	return str;
}




char* kitoa(uint32_t value, char* buff)
{
    char digits[20] = { 0 };
	int index = 0;
    int negative = 0;

	if (value == 0)
	{
		buff[0] = '0';
		buff[1] = '\0';
		return buff;
	}

	if (value < 0)
	{
		negative = 1;
		value = -value; //change number to positive, the - operator flips the sign of a number
	}


	while (value > 0)
	{
		digits[index] = (value % 10) + '0';
		// any number modulous 10 gives you the last digit of that number
		// you then add that number to the ascii value of 0 which will give you the ascii value of that number
		// e.g '0' = 48
		// if our number is 5. 5 + 48 = 53 = '5'

		value /= 10;
		//we then divide our value by 10, this is integer division, so the decimal part gets chopped off
		// e,g 123 / 10 - 12.3
		// remove the decimal and you get 12.
		index++;
	}


	if (negative)
	{
		digits[index] = '-';
		index++;
	}


	digits[index] = '\0'; //null terminate always


	kstrrev(digits);
	kstrcpy(buff, digits);
	return buff;
}



void kpad(const char* src, char* dst, int width, char pad_char)
{
    int len = kstrlen(src);
    int i;

    if (len >= width) 
    { 
        kstrcpy(dst, src); 
        return; 
    }
    for (i = 0; i < width - len; i++)
        dst[i] = pad_char;

    kstrcpy(dst + i, src);
}


void kutoa_hex(uint32_t val, char* buf, uint32_t uppercase)
{
    const char* digits_lower = "0123456789abcdef";
    const char* digits_upper = "0123456789ABCDEF";
    const char* digits;
    char tmp[9];
    int i = 0;
    int j = 0;
    digits = uppercase ? digits_upper : digits_lower;

    if (val == 0) 
    { 
        buf[0] = '0'; 
        buf[1] = '\0'; 
        return; 
    }

    while (val)
    {
        tmp[i++] = digits[val & 0xF];
        val >>= 4;
    }

    while (i > 0)
        buf[j++] = tmp[--i];

    buf[j] = '\0';
}