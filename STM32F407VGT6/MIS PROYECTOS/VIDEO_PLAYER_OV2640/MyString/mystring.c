/*
  * Copyright (C) 2019, VadRov, all rights reserved.
  *
  * Procedures for working with strings
  *
  * Free distribution allowed.
  *For any method of distribution, attribution to the author is MANDATORY.
  * In case of changes and distribution of modifications, the indication of the original author is MANDATORY.
  * Distributed “as is”, that is, use is at your own peril and risk.
  * The author does not provide any guarantees.
 *
 *  https://www.youtube.com/@VadRov
 *  https://dzen.ru/vadrov
 *  https://vk.com/vadrov
 *  https://t.me/vadrov_channel
 */

#include "mystring.h"
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

/*
* Copies line 2 to line 1, wrapping a maximum of len characters,
* but not more than the length of the string str2
 */
void my_strcpy_fixed_sym(char *str1, const char *str2, int len)
{
	while (len > 0 && *str2) {
		*str1 = *str2;
		str1++;
		str2++;
		len--;
	}
	*str1 = '\0';
}

/*
* Appends line 2 to line 1, carrying a maximum of len characters,
* but not more than the length of the string str2.
 */
void my_strcat_fixed_sym (char *str1, const char *str2, int len)
{
	str1 += my_strlen(str1);
	while (len > 0 && *str2) {
		*str1 = *str2;
		str1++;
		str2++;
		len--;
	}
	*str1 = '\0';
}

/*
 * Appends line 2 to line 1.
 */
void my_strcat (char *str1, const char *str2)
{
	str1 += my_strlen(str1);
	while (*str2) {
		*str1 = *str2;
		str1++;
		str2++;
	}
	*str1 = '\0';
}

/*
* Copies line 2 to line 1
 */
void my_strcpy(char *str1, const char *str2)
{
   while(*str2) {
      *str1 = *str2;
      str1++;
      str2++;
   }
   *str1 = '\0';
}

/*
* Allocates memory for a string and copies the string to this memory area.
* Returns the starting memory address of this string.
 */
char* my_strcpy_mem(const char *str)
{
	char *mem = (char *)calloc(my_strlen(str) + 1, 1);
	if (mem) my_strcpy(mem, str);
	return mem;
}

/*
 *Returns the length of a string
 */
uint32_t my_strlen(const char *str1)
{
	uint32_t i = 0;
	while (*str1) {
		str1++;
		i++;
	}
	return i;
}

/*
 * Fills a string with a given value (0...255)
 */
void my_strset(char *str1, uint8_t code, uint32_t len)
{
	while (len--) {
		*str1 = code;
		str1++;
	}
}

/*
* Stitches multiple lines into one line with a delimiter.
* n - number of lines, separator - separator line, then there are n number of lines.
* Ignores empty lines.
*/
char* my_strsplice (int n, const char *separator, ...)
{
	va_list spisok;
	va_start(spisok, separator);
	int len = 0, i;
	i = n;
	while (i) {
		char *st = va_arg(spisok, char *);
		if (my_strlen(st)) {
			len+=my_strlen(st);
			if (i != 1) len+=my_strlen(separator);
		}
		i--;
	}
	char *text = calloc(len+1, sizeof(char));
	if (text==0) return 0;
	va_start(spisok, separator);
	i = n;
	while (i) {
		char *st = va_arg(spisok, char *);
		if (my_strlen(st)) {
			my_strcat(text, st);
			if (i != 1) my_strcat(text, separator);
		}
		i--;
	}
	return text;
}

/*
 * Search for the string str in the string buffer, buffer size size
 */
char *memstr(char *buffer, char *str, uint32_t size)
{
	char *pos;
	char lenstr = my_strlen(str);
	for (pos = buffer; pos <= (buffer - lenstr + size); pos++) {
		if (memcmp(pos, str, lenstr) == 0) return pos;
	}
	return NULL;
}
