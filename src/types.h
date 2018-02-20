#ifndef _NOS_TYPES_H_
#define _NOS_TYPES_H_

#include "stdbool.h"
#include "stddef.h"
#include "stdint.h"
#include "multiboot.h"

#ifndef NULL
#define NULL ((void *)0)
#endif

typedef unsigned char uchar;
typedef unsigned long ulong;
typedef unsigned int  uint;

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

