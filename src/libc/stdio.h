
#ifndef _STDIO_H_
#define _STDIO_H_

typedef struct FILE {
} FILE;

extern FILE *stderr;
extern FILE *stdin;
extern FILE *stdout;

void perror(const char *s);
int  printf(const char *format, ...);
int fprintf(FILE * stream, const char * format, ...);

int close(int fildes);


FILE* fopen (const char * filename, const char * mode);
int fflush  (FILE * stream );
int fileno  (FILE *stream);

#endif  // _STDIO_H_

