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

void* memcpy(void *dst, const void *src, size_t n);
void* memset(void *s, int c, size_t n);
int memcmp (const void *s1, const void *s2, size_t len);

int strlen(const char *s);
char* strcpy (char *dest, const char *src);
int strcmp (const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, register size_t n);
char* strstr(const char *s1, const char *s2);

void perror(const char *s);

#endif /// _NOS_LIBC_H_
