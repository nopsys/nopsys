#ifndef _NOS_TYPES_H_
#define _NOS_TYPES_H_

#include "multiboot.h"
#include <stdbool.h>
#include <stddef.h>
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

typedef struct readonly_page_t
{
	unsigned long virtualAddress;
	unsigned long physicalAddress;
	unsigned char contents[4096];
} readonly_page_t;

typedef struct snapshot_info_t
{
	unsigned long pagesSaved;
	unsigned long pagesToSave;
	readonly_page_t *pages;
} snapshot_info_t;

typedef struct computer_t
{
	snapshot_info_t snapshot;
	display_info_t video_info;
	multiboot_info_t *mbi;
	void *image;
	uint32_t image_length;
	void *snapshot_start, *snapshot_end;
	void (*page_fault_handler)(void*);
	bool in_gc;
	int  in_page_fault;
	void *page_fault_address;
	uint32_t total_page_faults;
} computer_t;


#endif  /* _NOS_TYPES_H_ */

