#include "nopsys.h"
#include "stdio.h"
#include "string.h"
#include "time.h"
//#include <signal.h>


/** 
 * libc functions that we implement for nos platform
**/

#define EOF -1

// For malloc and friends

#define HEAP_SIZE               1024*1024*10

#define PAGESIZE 4096

// Console especific code. Some screen stuff.

#define COLUMNS                 80       // The number of columns
#define LINES                   24       // The number of lines
#define ATTRIBUTE               7        // The attribute of an character
#define VIDEO                   0xB8000  // The video memory address


/* These are globals declared as extern in some place of libc headers. We
   have to define them to avoid link errors. */
__thread int errno;
__thread int __libc_errno;
/* Needed in some libm.a implementations */
int __errno_location;

__attribute__ ((noreturn)) 
void exit(int a) 
{
	printf("exit()\n");
	nopsys_exit();
} 



void* malloc(unsigned int size)
{
	static char heap[HEAP_SIZE];
	static char *heap_end = &heap[HEAP_SIZE];
	static char *heap_new = heap;

	unsigned long long total;
	total = size;

	while ((uintptr_t)heap_new % 4 != 0) // align to 4 bytes
	{
		heap_new++;
	}	

	if (heap_new + total < heap_end)
	{
		char *answer = heap_new;
		
		heap_new += total;
		return answer;
	}
	printf("malloc got out of space. You asked for (%d) bytes.\n", size);
	nopsys_exit();
	return 0;
}

void *realloc(void * ptr, unsigned int size)
{
	printf("Someone called unimplemented realloc. Exiting.");
	nopsys_exit();
	return 0;
}


// this asumes that malloc will get each block in a contiguous always forward way
void* valloc(size_t size)
{
	uintptr_t result = (uintptr_t)malloc(4); // get one byte to see where we are placed.
	
	// now look where is the next aligned position (could be exactly result
	// or something near it).
	uintptr_t pagesize      = PAGESIZE;
	uintptr_t first_aligned = (result + pagesize - 1) & ~(pagesize - 1);
	uintptr_t wasted        = first_aligned - result; // calc how many bytes are wasted due to alignment

	// malloc the needed amount
	if (malloc(pagesize + wasted - 4) != (void*)result+4)
	{
		//this should never happen. If it happens it's a big mistake.
		perror("ERROR in valloc: malloc not allocating contiguous positions\n");
	}

	return (void*)first_aligned;
}


void *calloc(unsigned int count, unsigned int size)
{
	return malloc(count*size);
}

void free(void *p)
{
	printf("free(%p)\n",p);
	nopsys_exit();
}




void uitoa64 (uint64_t value, char *str_out, int base)
{
	char *p = str_out;

	/* Algorithm: Divide value by base until it is zero, acummulating remainders. 
                  String is stored backwards.  */
	do
	{
		int remainder = value % base;

		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
	}
	while (value /= base);

	// Terminate str_out.
	*p = 0;

	// Reverse str_out.

	char *p1 = str_out;
	char *p2 = p - 1;

	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}
}

void itoa64 (int64_t value, char *str_out, int base)
{
	/* Special case for base 10: consider it signed, put `-' if needed */
	if (base == 10 && value < 0)
	{
		*str_out++ = '-';
		value = -value;
	}

	uitoa64((uint64_t)value, str_out, base);
}

/**
 * Parses the C string str interpreting its content as an integral number, which is returned as an int value.
**/
int _atoi(char *str) {
  unsigned int answer=0;
  int base = 10;

  if (*str == '0' && str[1] == 'x') {
          base = 0x10;
          str += 2;
  }

  while (*str) {
    answer *= base;
    if (*str > '9') 
      answer += (*str | 0x20) - 'a' + 0xa;
    else 
      answer += *str - '0';
    str++;
  }
  return answer;
}


/* Put the character C on the screen.  */
int putchar (int c)
{
	console_std_put_char(c);
	serial_write(c);
	return c;
}


void putstring(const char *str)
{
	console_std_put_string(str);
	while (*str)
		serial_write(*str++);

}

static 
void copy_buf(char **pformatted, const char *buf)
{
	char *formatted = *pformatted;
	int len = strlen(buf);
	memcpy(formatted, buf, len);
	*pformatted = formatted + len;
}

/* Format a string and print it on the screen, just like the libc
   function printf.  */
int
sprintf_args (char *formatted, const char *format, void *arg[])
{
	int c;
	char buf[20];

	while ((c = *format++) != 0)
	{
		if (c != '%')
		{
			*formatted++ = c;
		}
		else
		{
			char *p;
			void *value = *arg;

			arg++;

			c = *format++;
			switch (c)
			{
			case 'p':
				*formatted++ = '0';
				*formatted++ = 'x';
				uitoa64 ((uintptr_t)value, buf, 16);
				copy_buf(&formatted, buf);
				break;

			case 'x':
				uitoa64 ((uintptr_t)value, buf, 16);
				copy_buf(&formatted, buf);
				break;

			case 'u':
				uitoa64 ((uintptr_t)value, buf, 10);
				copy_buf(&formatted, buf);
				break;

			case 'd':
				itoa64 ((uintptr_t)value, buf, 10);
				copy_buf(&formatted, buf);
				break;

			case 's':
				p = (char*)value;
				if (!p)
					p = "(null)";

				copy_buf(&formatted, p);
				break;

			case 'l': //operand size unsupported
				break; 
					

			default:
				copy_buf(&formatted, "<unkonwn format %");
				buf [0] = c; buf[1] = '>'; buf[2] = 0;
				copy_buf(&formatted, buf);
				break;
			}
		}
	}

	*formatted = 0;
	return 0; // should return num of chars written
}

int printf_args(const char *format, void *arg[])
{
	char formatted[10000];

	sprintf_args(formatted, format, arg);
	putstring(formatted);
	return 0;
}

/* in 64 bits args are passed in registers, check the asm
int printf (const char *format, ...)
{
	void **arg = (void **) &format;
	return printf_args(format, arg + 1);
}

int sprintf (char *formatted, const char *format, ...)
{
	void **arg = (void **) &format;
	return sprintf_args(formatted, format, arg + 1);
}
*/

int printf_fixed_size(const char *string, const long size)
{
	int i = 0;
	for (i = 0; i < size; i++)
	{
		putchar (string[i]);
	}
	return 0;
}

static int tabs = 0;

int printf_inc_tab() { return ++tabs; }
int printf_dec_tab() { return --tabs; }

int printf_tab(const char *format, ...)
{
	//printf("entering printf_tab %d", tab);
	for(int i = 0; i < tabs; i++){
		printf("\t");
	}

	void **arg = (void **) &format;
	return printf_args(format, arg +1);
}

int printf_tab_with (int tabs_local, const char *format, ...)
{
	//printf("entering printf_tab_with %d", tab);
	for(int i = 0; i < tabs_local; i++){
		printf("\t");
	}
	void **arg = (void **) &format;
	return printf_args(format, arg + 1);
}


int puts(const char *msg)
{
	putstring(msg); 
	putchar('\n');
	return 0;
}

/* Linux implementations use close while others use fclose */
int close(int fildes)
{
	perror("close unimplemented");
    return 0;
}
 
int fclose(FILE* f)
{
	return close(fileno(f));
}

FILE *stdin  = (FILE *) 0;
FILE *stdout = (FILE *)-1;
FILE *stderr = (FILE *)-2;

FILE* fopen (const char * filename, const char * mode)
{
	perror("opening files is not unsupported");
	return NULL;
}

int fprintf(FILE *file, const char *format, ...)
{
	void **arg = (void **) &format;

	if (file == stderr) 
		return printf_args(format, arg+1);
	
	printf("fprintf called with file %p.\n", file);
	return EOF;
}

int fputs(const char *msg, FILE *file)
{
	if (file == stderr || file == stdout)
	{ 
		printf(msg);
		return 0;
	}
	else 
	{
		printf("fputs called with file %p.\n", file);
		return EOF;
	}

}

size_t fwrite(const void *buf, size_t size, size_t nmemb, FILE *file)
{
	char *msg = (char*)buf;
	int total = size*nmemb;
	if (file == stderr)
		while (total--) putchar(*msg++);
	else printf("fwrite called with file %p.\n", file);

	return size;
}

int fflush (FILE *f)
{
	return 0;
}

int fileno(FILE *f)
{
	return (int)(intptr_t)f;
}

int getchar(void)
{
	return 0;
}

time_t time(time_t *t)
{
	time_t answer;

	answer = (time_t)current_seconds();
	if (t) *t = answer;
	return answer;
}

void
__assert_fail(const char *assertion,
              const char *file,
              unsigned int line,
              const char *function)
{
	printf("ASSERTION (%s) FAILED in \"%s:%d\" in function \"%s\".\n",
	             assertion, file, line, function);
	exit(0);
}



void* memcpy(void *dst, const void *src, size_t n)
{
	void *ret = dst;
	for (; n > 0; n--)
		*(uchar*)dst++ = *(uchar*)src++;

	return ret;
}

void* memset(void *s, int c, size_t n)
{
	uchar cc = (uchar) c;
	for (; n > 0; n--)
		*(uchar*)s++ = cc;

	return s;
}

int memcmp (const void *s1, const void *s2, size_t len)
{
	const uchar *sp1, *sp2;

	sp1 = s1;
	sp2 = s2;

	while (len != 0 && *sp1 == *sp2)
		sp1++, sp2++, len--;

	if (len == 0)
		return 0;

	return *sp1 - *sp2;
}

#define WT size_t
#define WS (sizeof(WT))

// TAKEN FROM LIBMUSL
void *memmove(void *dest, const void *src, size_t n)
{
	char *d = dest;
	const char *s = src;

	if (d==s) return d;
	if (s+n <= d || d+n <= s) return memcpy(d, s, n);

	if (d<s) {
		if ((uintptr_t)s % WS == (uintptr_t)d % WS) {
			while ((uintptr_t)d % WS) {
				if (!n--) return dest;
				*d++ = *s++;
			}
			for (; n>=WS; n-=WS, d+=WS, s+=WS) *(WT *)d = *(WT *)s;
		}
		for (; n; n--) *d++ = *s++;
	} else {
		if ((uintptr_t)s % WS == (uintptr_t)d % WS) {
			while ((uintptr_t)(d+n) % WS) {
				if (!n--) return dest;
				d[n] = s[n];
			}
			while (n>=WS) n-=WS, *(WT *)(d+n) = *(WT *)(s+n);
		}
		while (n) n--, d[n] = s[n];
	}

	return dest;
}

int strlen(const char *s)
{
	int len = 0;
	while (s[len] != 0)
	{
		len++;
	}

	return len;
}


char* strcpy (char *dest, const char *src)
{
	char *p = dest;
	while (*src != 0)
	{
		*p++ = *src++;
	}

	*p = 0;

	return dest;
}

// FIXME: untested
char* strncpy (char *dest, const char *src, size_t n)
{
	char *p = dest;
	while (*src != 0 && n > 0)
	{
		*p++ = *src++;
		n--;
	}
	
	while (n > 0)
	{
		*p = 0;
		n--;
	}

	return dest;
}

int strcmp (const char *s1, const char *s2)
{
	while (*s1 != 0 && *s1 == *s2)
		s1++, s2++;

	if (*s1 == 0 || *s2 == 0)
		return (uchar) *s1 - (uchar) *s2;

	return *s1 - *s2;
}


int strncmp(const char *s1, const char *s2, register size_t n)
{
	register uchar u1, u2;

	while (n-- > 0)
	{
		u1 = (uchar) *s1++;
		u2 = (uchar) *s2++;
		
		if (u1 != u2)
			return u1 - u2;
		if (u1 == '\0')
			return 0;
	}
	
	return 0;
}


char* strstr(const char *s1, const char *s2)
{
  const char *p, *q;

	for (; *s1; s1++)
	{
		p = s1, q = s2;
		while (*q && *p)
		{
			if (*q != *p)
				break;
				
			p++, q++;
		}
		
		if (*q == 0)
			return (char *)s1;
	}
	
	return 0;
}

/// System call. Nobody should use system calls, but we need it to avoid
/// libc compile errors
/*int sigprocmask(int how, const __sigset_t *set, __sigset_t *oset)
{
	return 0; // everythink OK (?)
}

int __sigprocmask (int how, const __sigset_t *set, __sigset_t *oset)
{
	return sigprocmask (how, set, oset);
}*/

int mprotect(void *addr, size_t len, int prot)
{
	return 0; // FIXME: STUB
}



struct tm * localtime (const time_t * timer)
{
	perror("localtime unimplemented");
	return NULL;
}

char* asctime (const struct tm * timeptr)
{
	perror("asctime unimplemented");
	return NULL;
}

char* getenv (const char* name)
{
	return 0;
}





