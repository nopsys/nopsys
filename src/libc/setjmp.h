
#ifndef _SETJMP_H_
#define _SETJMP_H_

// #ifdef x86  // we don't care for now, just support x86

typedef unsigned long __jmp_buf[6];

// #elif x64  ...
// #endif


// taken from musl

typedef struct __jmp_buf_tag {
	__jmp_buf __jb;
	unsigned long __fl;
	unsigned long __ss[128/sizeof(long)];
} jmp_buf[1];

typedef jmp_buf sigjmp_buf;
int sigsetjmp (sigjmp_buf, int);
_Noreturn void siglongjmp (sigjmp_buf, int);


int setjmp (jmp_buf);
_Noreturn void longjmp (jmp_buf, int);

//#define setjmp setjmp
//#define longjmp longjmp

#endif  // _SETJMP_H_

