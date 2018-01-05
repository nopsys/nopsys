
#include "libc.h"
#include "multiboot.h"
#include "ints.h"
#include "console.h"

void computer_initialize_from(computer_t *computer, ulong magic, multiboot_info_t *mbi);
void computer_initialize_from_modules (computer_t *computer, multiboot_info_t *mbi);

void extern *OFW_callout;  /// ????
void *OFW_callout;

#include "../shared/splashscreen.c"

void nopsys_main (ulong magic, multiboot_info_t *mbi)
{
	computer_t *computer = current_computer();
	computer_initialize_from(computer, magic, mbi);
	
	//fill_rectangle(100, 50, 700, 300, 0x00ff0000);
	//fill_rectangle(100, 50, 700, 350, 0x0000ff00);
	//fill_rectangle(100, 50, 700, 400, 0x000000ff);
	
	memcpy (computer->video_info.address, splashscreen_image.pixel_data, splashscreen_image.width * splashscreen_image.height * 4);
	//bitblt_32bit_to_fb(splashscreen_image.pixel_data, splashscreen_image.width, splashscreen_image.height, 0, 0);

	ints_init();

	if (computer->image)
	{
		printf("Found image at %p\n", computer->image);
		nopsys_vm_main(computer->image, computer->image_length);
	}
	else
	{
		printf("Image not found!!!\n");
		nopsys_exit();
	}		
}

void computer_initialize_from(computer_t *computer, ulong magic, multiboot_info_t *mbi)
{
	computer->snapshot_start = NULL;
	computer->snapshot_end = NULL;
	computer->image = NULL;
	computer->in_gc = 0;
	computer->in_page_fault = 0;
	
	// set the memory map that grubs passes (grub gets it by asking the bios) to
	// a variable we can query from the image by using a primitive	
	computer->mbi = mbi;

		/* Are mods_* valid?  */
	if (magic == MULTIBOOT_BOOTLOADER_MAGIC)
	{
		if (mbi->flags && MULTIBOOT_INFO_CMDLINE)
		{
			display_initialize_from_cmd(&computer->video_info, (char*)mbi->cmdline);
		}

		console_initialize_stdout();

		if (mbi->flags && MULTIBOOT_INFO_MODS)
		{
			computer_initialize_from_modules(computer, mbi);
		}
	}
	else
	{
		OFW_callout = (void*)magic;
		display_initialize_hardcoded(&computer->video_info);
		computer->image = (void*)0x800000;
	}

}


void computer_initialize_from_modules(computer_t *computer, multiboot_info_t *mbi)
{
	module_t *mod = (module_t *) mbi->mods_addr;

	if (mbi->mods_count >= 1)
	{
		computer->image = (void*)mod->mod_start;
		computer->image_length = mod->mod_end - mod->mod_start;
	}
	
	printf ("mods_count = %u, mods_addr = %p\n", mbi->mods_count, (void*)mbi->mods_addr);
		
	for (int i = 0; i < mbi->mods_count; i++, mod++)
	{
		
		printf("mod_start = 0x%x, mod_end = 0x%x, string = %s\n",
				(unsigned) mod->mod_start,
				(unsigned) mod->mod_end,
				(char *)   mod->string);

				
		if (strcmp((char *)mod->string, "/nopsys.config") == 0)
		{
			char *conf_str = (char *)mod->mod_start;
			*((char*)mod->mod_end) = 0;
			printf("Config module found\n");
			printf("Says: %s\n", conf_str);
			console_set_debugging(conf_str[0] == '0');
			printf("End of config module\n");
		}
	}
	
}



