
#ifndef _LIMITS_H_
#define _LIMITS_H_

//For x86_64
#define INT_MAX		0x7fffffff
#define INT_MIN		(-INT_MAX - 1)

#ifndef __LP64__
   #define LONG_MAX	0x7fffffffL
   #define ULONG_MAX	0xffffffffUL
#else
   #define LONG_MAX	0x7fffffffffffffffL
   #define ULONG_MAX	0xffffffffffffffffUL
#endif
#define LONG_MIN	(-LONG_MAX - 1)

#define LLONG_MAX	0x7fffffffffffffffLL
#define LLONG_MIN	(-LLONG_MAX - 1)

#endif  // _LIMITS_H_

