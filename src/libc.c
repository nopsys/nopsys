
#include "nopsys.h"
#include "libc.h"
#include <time.h>  // needed?
#include <signal.h>


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


#define FILE uint32_t  //ugh


// These two are globals declared as extern in some place of libc headers. We
// have to define them to avoid link errors.

int errno;
int *__errno_location = &errno;

__thread int __libc_errno;

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

	while ((int)heap_new % 4 != 0) // align to 4 bytes
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
	unsigned int result = (unsigned int)malloc(4); // get one byte to see where we are placed.
	
	// now look where is the next aligned position (could be exactly result
	// or something near it).
	unsigned int pagesize      = PAGESIZE;
	unsigned int first_aligned = (result + pagesize - 1) & ~(pagesize - 1);
	unsigned int wasted        = first_aligned - result; // calc how many bytes are wasted due to alignment

	// malloc the needed amount
	if (malloc(pagesize + wasted - 4) != (void*)result+4)
	{
		//this should never happen. If it happens it's a big mistake.
		printf("ERROR in valloc: malloc not allocating contiguous positions\n");
		nopsys_exit();
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




void
uitoa (uint32_t value, char *str_out, int base)
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

void itoa (int value, char *str_out, int base)
{
	/* Special case for base 10: consider it signed, put `-' if needed */
	if (base == 10 && value < 0)
	{
		*str_out++ = '-';
		value = -value;
	}

	uitoa((uint32_t)value, str_out, base);
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


void bprintf_string(const char *str)
{
	while (*str)
		putchar (*str++);
}

/* Format a string and print it on the screen, just like the libc
   function printf.  */
int
bprintf (const char *format, void *arg[])
{
	int c;
	char buf[20];

	while ((c = *format++) != 0)
	{
		if (c != '%')
		{
			putchar (c);
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
				putchar('0');
				putchar('x');
				uitoa ((int)value, buf, 16);
				bprintf_string(buf);
				break;

			case 'x':
				uitoa ((int)value, buf, 16);
				bprintf_string(buf);
				break;

			case 'u':
				uitoa ((int)value, buf, 10);
				bprintf_string(buf);
				break;

			case 'd':
				itoa ((int)value, buf, 10);
				bprintf_string(buf);
				break;

			case 's':
				p = (char*)value;
				if (!p)
					p = "(null)";

				bprintf_string(p);
				break;
					

			default:
				bprintf_string("<unkonwn format>");
				putchar (*((int *) arg++));
				break;
			}
		}
	}

	return 0; // should return num of chars written
}


int printf (const char *format, ...)
{
	void **arg = (void **) &format;
	return bprintf(format, arg + 1);
}

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
	return bprintf(format, arg +1);
}

int printf_tab_with (int tabs_local, const char *format, ...)
{
	//printf("entering printf_tab_with %d", tab);
	for(int i = 0; i < tabs_local; i++){
		printf("\t");
	}
	void **arg = (void **) &format;
	return bprintf(format, arg +1);
}


int puts(const char *msg)
{
	bprintf("%s\n", (void**)&msg);
	return 0;
}

FILE *stderr = (FILE *)-2;

int fprintf(FILE *file, const char *format, ...)
{
	void **arg = (void **) &format;

	if (file == stderr) 
		return bprintf(format, arg+1);
	
	printf("fprintf called with file %p.\n", file);
	return EOF;
}

int fputs(const char *msg, FILE *file)
{
	if (file == stderr)
	{ 
		printf("%s", msg);
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
}
*/

int mprotect(void *addr, size_t len, int prot)
{
	return 0; // FIXME: STUB
}

void perror(const char *s)
{
	// FIXME: Should print also errno and a corresponding message
	if (s)
		printf("%s", s);
}


