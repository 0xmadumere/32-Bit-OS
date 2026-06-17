// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#include <boot/stage2/lib/string.h>

uint16_t strlen(const char* str)
{
	const char* s = str;

	while (*str)
		str++;

	return str - s;
}

char* strcpy(char* dest, const char* src)
{
    char* d = dest;

	if (!dest || !src)
		return dest;

	while (*src)
		*dest++ = *src++;

	*dest = '\0';

	return d;
}


char* strncpy(char* dest, const char* src, size_t count)
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


int tolower(int c)
{
	if (c >= 'A' && c <= 'Z')
		c += 32;

	return c;
}



int strncimp(const char* str1, const char* str2, size_t count)
{
	if (!str1 || !str2 || !count)
		return -1;


    while (--count && (*str1 && *str2))
    {
        int c1 = tolower(*str1++);
        int c2 = tolower(*str2++);

        if (c1 != c2)
            return c1 - c2;

    }

    return tolower(*str1) - tolower(*str2);
}


char* strrev(char* str)
{
    uint32_t len, i;

	if (!str)
		return str;

	len = strlen(str);

	for (i = 0; i < (len / 2); i++)
	{
		char temp = str[i];
		str[i] = str[len - 1 - i];
		str[len - 1 - i] = temp;
	}

	return str;
}


char* itoa(uint32_t value, char* buff)
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


	strrev(digits);
	strcpy(buff, digits);
	return buff;
}
