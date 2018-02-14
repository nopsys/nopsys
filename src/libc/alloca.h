
#ifndef _ALLOCA_H_
#define _ALLOCA_H_

void *alloca(size_t size);

#ifdef __GNUC__
#define alloca __builtin_alloca
#else
#error dont have alloca for this compiler
#endif

#endif  // _ALLOCA_H_


