
#ifndef _STRING_H_
#define _STRING_H_

#include "../types.h"

void* memcpy(void *dst, const void *src, size_t n);
void* memset(void *s, int c, size_t n);
int memcmp (const void *s1, const void *s2, size_t len);

int   strlen (const char *s);
char* strcpy (char *dest, const char *src);
char* strncpy( char * dest, const char * src, size_t n );
int   strcmp (const char *s1, const char *s2);
int   strncmp(const char *s1, const char *s2, register size_t n);
char* strstr (const char *s1, const char *s2);

#endif  // _STRING_H_

