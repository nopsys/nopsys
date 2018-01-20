#ifndef _NOS_LIBC_H_
#define _NOS_LIBC_H_

#include "nopsys.h"

int printf(const char *format, ...);
int printf_tab(const char *format, ...);
int printf_tab_with(int tabs_local, const char *format, ...);

int printf_inc_tab();
int printf_dec_tab();

int puts(const char *msg);

int _atoi(char *str);
void itoa (int value, char *str_out, int base);
void uitoa (uint32_t value, char *str_out, int base);


#endif /// _NOS_LIBC_H_
