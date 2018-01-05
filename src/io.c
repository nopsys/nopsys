
#include "nopsys.h"
#include "utils.h"
#include "framebuffer.h"
#include <string.h>



void display_render(uchar   *image_src, uint32_t width, uint32_t height, uint32_t depth,
                    uint32_t affectedL, uint32_t affectedR, uint32_t affectedT, uint32_t affectedB)
{
	uint last_word, first_word, line, count_per_line;

	display_info_t *display = &current_computer()->video_info;
	uchar *image_dst = display->address;
	uint scan_line = display->bytes_per_scanline;

	first_word = scan_line * affectedT + BYTES_PER_LINE_RD(affectedL, depth);
	last_word  = scan_line * affectedT + BYTES_PER_LINE(affectedR, depth);
	count_per_line = last_word - first_word;

	switch (display->depth)
	{
	case 16:
		for (line= affectedT; line < affectedB; line++, first_word += scan_line)
		{
			uint16_t *from = (uint16_t*)(image_src + first_word);
			uint16_t *to   = (uint16_t*)(image_dst + first_word);
			for (; to < (uint16_t*)(image_dst + first_word + count_per_line); to += 2, from += 2)
			{
				to[0] = repack(from[1]);
				to[1] = repack(from[0]);
			}
		}
		break;
	case 8:
		for (line = affectedT; line < affectedB; line++, first_word += scan_line)
		{
			uint32_t *from = (uint32_t*)(image_src + first_word);
			uint32_t *to   = (uint32_t*)(image_dst + first_word);
			for (;to < (uint32_t*)(image_dst + first_word + count_per_line); to += 1, from += 1) 
		// to[0] = from[0];
				to[0] = swap32(from[0]);
		}
		break;
	default:
		for (line= affectedT; line < affectedB; line++, first_word += scan_line)
			memcpy(image_dst + first_word, image_src + first_word, count_per_line);
		break;
	}
}

void display_initialize_hardcoded(display_info_t *video_info)
{
	video_info->width = 1200;
	video_info->height = 900;
	video_info->depth = 16;
	video_info->address = (uchar*)0xfd000000;
	video_info->bytes_per_scanline = 2400;
}


/**
 * video_config_line must be something like video=1024x768x32@0xf0000000,4096
 * where 0xf0000000 is the base address and 4096 is the number of bytes per scanline
 * if the number of byte per scanline is not present, it's going to be guessed (X/8)
**/
void display_initialize_from_cmd(display_info_t *video_info, char *video_config_line)
{
	/* video_config_line is {width}x{height}x{depth}@{address},{bytesPerScanLine}\0 */
	char sep_tokens[] = { /*width*/ 'x', /*height*/'x', /*depth*/ '@', /*address*/ ',', /*bytesPerScanLine*/ '\0'};
	
	video_config_line = strstr(video_config_line, "video=") + 6;

	video_config_line = parse_string(video_config_line, &video_info->width,   sep_tokens[0]);
	video_config_line = parse_string(video_config_line, &video_info->height,  sep_tokens[1]);
	video_config_line = parse_string(video_config_line, &video_info->depth,   sep_tokens[2]);
	video_config_line = parse_string(video_config_line, (int*)&video_info->address, sep_tokens[3]);
	
	if (video_config_line)
		parse_string(video_config_line, &video_info->bytes_per_scanline, sep_tokens[4]);
	else
		video_info->bytes_per_scanline = BYTES_PER_LINE(video_info->width, video_info->depth);
}

void connect_to_APM()
{
	//connect to an APM interface
	__asm("mov 0x53,%ah"); //this is an APM command
    __asm("mov 3,%al"); // connecting to real mode
    __asm("xor %bx,%bx"); // and device 0 (0 = APM BIOS)
    __asm("int $0x15");     // call the BIOS function through interrupt 15h
	//__asm("jc APM_error") // if the carry flag is set there was an error
}	

void enable_power_management()
{
	__asm("mov 0x53,%ah"); //this is an APM command
	__asm("mov 0x8,%al");  // Change the state of power management...
	__asm("mov 1, %bx"); // ...on all devices to...
	__asm("mov 1, %cx"); //...power management on.
	__asm("int $0x15"); // call the BIOS function through interrupt 15h
}

void computer_shutdown()
{	
	connect_to_APM();
	enable_power_management();
	__asm("mov 0x53, %ah"); // this is an APM command
	__asm("mov 0x7, %al"); // Set the power state...
	__asm("mov 1, %bx"); //...on all devices to...
	__asm("mov 0x3, %cx"); // power mode: shutdown
	__asm("int $0x15"); // call the BIOS function throu
}



