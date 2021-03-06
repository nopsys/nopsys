
#ifndef _STDINT_H_
#define _STDINT_H_

typedef char      int8_t;
typedef short     int16_t;
typedef int       int32_t;
typedef long long int64_t;

typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

typedef unsigned int       size_t;

#ifdef __x86_64__
typedef long long intptr_t;
typedef unsigned long long uintptr_t;
#else
typedef long intptr_t;
typedef unsigned long uintptr_t;
#endif

#endif  // _STDINT_H_

