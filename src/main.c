
#include "stdio.h"
#include "string.h"
#include "multiboot.h"
#include "ints.h"
#include "console.h"
#include "nopsys.h"

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
	for (int i = 0; i < 100; i++)	
		memcpy (computer->video_info.address, splashscreen_image.pixel_data, splashscreen_image.width * splashscreen_image.height * 4);
	//bitblt_32bit_to_fb(splashscreen_image.pixel_data, splashscreen_image.width, splashscreen_image.height, 0, 0);

	breakpoint();
	ints_init();
	enable_sse();

	//if (computer->image)
	{
		printf("Found image at %p, length %d\n", computer->image, computer->image_length);
		nopsys_vm_main(computer->image, computer->image_length);
	}
	/*else
	{
		printf("Image not found!!!\n");
		nopsys_exit();
	}
	*/		
}

void computer_initialize_from(computer_t *computer, ulong magic, multiboot_info_t *mbi)
{
	breakpoint();
	computer->image = NULL;
	
	// set the memory map that grubs passes (grub gets it by asking the bios) to
	// a variable we can query from the image by using a primitive	
	computer->mbi = mbi;

		/* Are mods_* valid?  */
	if (magic == MULTIBOOT_BOOTLOADER_MAGIC)
	{
		if (mbi->flags & MULTIBOOT_INFO_CMDLINE)
		{
			display_initialize_from_mbi(&computer->video_info, mbi);
		}

		console_initialize_stdout();
		printf("loading nopsys...\n");

		if (mbi->flags & MULTIBOOT_INFO_MODS)
		{
			computer_initialize_from_modules(computer, mbi);
		}
	}
	else
	{
		breakpoint();
	}

}


void computer_initialize_from_modules(computer_t *computer, multiboot_info_t *mbi)
{
	module_t *mod = (module_t *)(uintptr_t)mbi->mods_addr;
	printf ("mbi %p, mod = %p\n", mbi, mod);

	if (mbi->mods_count >= 1)
	{
		computer->image = (void*)(uintptr_t)mod->mod_start;
		computer->image_length = mod->mod_end - mod->mod_start;
	}
	
	printf ("mods_count = %u\n", mbi->mods_count);
		
	for (int i = 0; i < mbi->mods_count; i++, mod++)
	{
		
		printf("mod_start = 0x%x, mod_end = 0x%x, string = %s\n",
				(unsigned) mod->mod_start,
				(unsigned) mod->mod_end,
				(char *)(uintptr_t)mod->string);

				
		if (strcmp((char *)(uintptr_t)mod->string, "/nopsys.config") == 0)
		{
			char *conf_str = (char *)(uintptr_t)mod->mod_start;
			*((char*)(uintptr_t)mod->mod_end) = 0;
			printf("Config module found\n");
			printf("Says: %s\n", conf_str);
			console_set_debugging(conf_str[0] == '0');
			printf("End of config module\n");
		}
	}
	
}

uintptr_t computer_first_free_address(computer_t *computer)
{
	uintptr_t result = 0;
	multiboot_info_t *mbi = computer->mbi;
	module_t *mod = (module_t *)(uintptr_t)mbi->mods_addr;

	if (mbi->mods_count == 0)
		perror("cant get first free address if no modules");
 
	for (int i = 0; i < mbi->mods_count; i++, mod++)
	{
		if (mod->mod_end > result)
			result = mod->mod_end;
	}
	return result;
}

