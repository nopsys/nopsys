
#include "nopsys.h"

void mark(int col)
{
#ifdef DEBUG
	static int count = 1;
	short *video = (short*)0xfd000000;
	int i;

	for (i=10*count; i<1200*10-10*count; i++)
		video[i+count*1200*10] = col;

	count++;
	count++;
#endif
}

void fill_rectangle(display_info_t *display, int width, int height, int x, int y, uint color)
{
	uchar *dest            = display->address;
	int bytes_per_scanline = display->bytes_per_scanline; // the amount of bytes occupied by a line of the screen's framebuffer
	int depth              = display->depth;

	uchar *components = (uchar*)&color;
	int    first_word = bytes_per_scanline * y + BYTES_PER_PIXEL(x, depth);
	int bytes_per_row = BYTES_PER_PIXEL(width, depth);
		
	for (int line = y - height; line < y; line++, first_word -= bytes_per_scanline)
	{
		uchar *pos = dest + first_word;
		for (int i = 0; i < bytes_per_row; i++, pos++)
		{
			*pos = components[i % 4];
		}
	}
}


void bitblt_32bit_to_fb(display_info_t *display, uint32_t *bitmap, int width, int height, int x, int y, uint32_t colormask)
{
	uchar *dest            = display->address;
	int bytes_per_scanline = display->bytes_per_scanline; // the amount of bytes occupied by a line of the screen's framebuffer
	int depth              = display->depth;
		
	if (depth != 32)
		while(1); // we should have a fallback or something
		
	int first_word = bytes_per_scanline * y + BYTES_PER_PIXEL(x, depth);
	int bytes_per_row = BYTES_PER_PIXEL(width, depth);
		
	for (int line = y - height; line < y; line++, first_word -= bytes_per_scanline)
	{
		uint32_t *pos = (uint32_t*)(dest + first_word);
		for (int i = 0; i < bytes_per_row; i+=4)
		{
			*(pos++) = *(bitmap++) & colormask;
		}
	}
}

