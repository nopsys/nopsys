
#include "types.h"

#define breakpoint() __asm("xchg %bx, %bx");
//#define breakpoint() __asm volatile("xchg %%bx, %%bx" ::: "ebx");  // or use this? which is best?

int  nopsys_vm_main(void *image, uint image_length);
__attribute__ ((noreturn)) void nopsys_exit();

uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t byte);

void ints_init();


uint16_t get_CS();
void*    get_CR2();
void enable_sse();
void enable_paging_using(void* page_dir);

computer_t* current_computer();

uint computer_first_free_address(computer_t *computer);
uint64_t current_microseconds(void);
uint64_t current_seconds(void);

void display_initialize_hardcoded(display_info_t *video_info);
void display_initialize_from_mbi(display_info_t *video_info, multiboot_info_t *mbi);

void fill_rectangle    (display_info_t *display, int width, int height, int x, int y, uint color);
void bitblt_32bit_to_fb(display_info_t *display, uint32_t *bitmap, int width, int height, int x, int y, uint32_t colormask);


void console_initialize_stdout();
void console_set_debugging(bool debugging);
void console_std_put_string(const char string[]);
void console_std_put_char(char c);


__attribute__ ((noreturn)) 
void serial_enter_debug_mode();
void serial_write(uchar a);

void semaphore_signal_with_index(int i);

void debug_print_call_stack();

#define BYTES_PER_LINE_PADDED(width, depth)	((((width) + 31) >> 5 << 2) * (depth))
#define BYTES_PER_LINE_FLOOR(width, depth)	((((width) >> 5) << 2) * (depth))
#define BYTES_PER_PIXEL(width, depth)	(width * (depth>>3))


#define COLOR_GREEN 0x0E70


