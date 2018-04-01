
#ifndef _STDLIB_H_
#define _STDLIB_H_

#include "types.h"

void* realloc(void *ptr, size_t new_size );
void* malloc (size_t size);
void* calloc (size_t num, size_t size);
void  free   (void* ptr);

long long int llabs (long long int n);

void exit (int status);

#endif  // _STDLIB_H_

