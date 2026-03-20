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

#ifndef MYSTRING_H_
#define MYSTRING_H_

#include "main.h"

//fills the string with the specified value (0...255)
void my_strset(char *str, uint8_t code, uint32_t len);

//returns the length of the string
uint32_t my_strlen(const char *str);

//copies line 2 to line 1
void my_strcpy(char *str1, const char *str2);

//copies line 2 to line 1, carrying a maximum of len characters, but no more than the length of the string str2
void my_strcpy_fixed_sym(char *str1, const char *str2, int len);

//allocates memory for a string and copies the string to this memory area
//returns a pointer to the memory area to which the string was copied
char* my_strcpy_mem(const char *str);

// appends line 2 to line 1
void my_strcat(char *str1, const char *str2);

//appends line 2 to line 1, carrying a maximum of len characters, but no more than the length of the string str2
void my_strcat_fixed_sym(char *str1, const char *str2, int len);

//search strings str in buffer buffer
//buffer size size
char *memstr(char *buffer, char *str, uint32_t size);

//stitches several lines into one line with a delimiter
//n - number of lines, separator - separator line, then there are n lines
char* my_strsplice (int n, const char *separator, ...);

#endif /* MYSTRING_H_ */
