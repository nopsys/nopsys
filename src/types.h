#ifndef _NOS_TYPES_H_
#define _NOS_TYPES_H_

#include "multiboot.h"
#include "stdbool.h"
#include "stddef.h"
#include "stdint.h"

#ifndef NULL
#define NULL ((void *)0)
#endif

typedef unsigned char uchar;
typedef unsigned long ulong;
typedef unsigned int  uint;
typedef void* time_t;

typedef struct display_info_t
{
	int width,
	    height,
        depth,
        bytes_per_scanline;
    uchar *address;

} display_info_t;


typedef struct computer_t
{
	display_info_t video_info;
	multiboot_info_t *mbi;
	void *image;
	uint32_t image_length;
} computer_t;


#endif  /* _NOS_TYPES_H_ */

