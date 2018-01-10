
#include "types.h"

#define breakpoint_bochs() __asm("xchg %bx, %bx");

int  nopsys_vm_main(void *image, uint image_length);
__attribute__ ((noreturn)) void nopsys_exit();

uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t byte);

void ints_init();


uint16_t get_CS();
void*    get_CR2();

computer_t* current_computer();
uint64_t current_microseconds(void);
uint64_t current_seconds(void);

void display_initialize_hardcoded(display_info_t *video_info);
void display_initialize_from_cmd(display_info_t *video_info, char *video_config_line);

void fill_rectangle    (display_info_t *display, int width, int height, int x, int y, uint color);
void bitblt_32bit_to_fb(display_info_t *display, uint32_t *bitmap, int width, int height, int x, int y, uint32_t colormask);


void console_initialize_stdout();
void console_set_debugging(bool debugging);
void console_std_put_string(char string[]);
void console_std_put_char(char c);


__attribute__ ((noreturn)) 
void serial_enter_debug_mode();
void serial_write(uchar a);

void semaphore_signal_with_index(int i);

void debug_print_call_stack();

#define BYTES_PER_LINE(width, depth)	((((width) + 31) >> 5 << 2) * (depth))
#define BYTES_PER_LINE_RD(width, depth)	((((width) >> 5) << 2) * (depth))
#define BYTES_PER_PIXEL(width, depth)	(width * (depth>>3))

