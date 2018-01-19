#include "nopsys.h"
#include "libc.h"

/**
 *  This is a C driver for Serial port. We use it for debugging and
 *  comunicating with the outside world (when running in VMware or
 *  like).
 *  It's pretty basic, if you want to improve it, you are welcome.
**/ 

#define PORT 0x3F8   /* COM1 */

void serial_init()
{
   outb(PORT + 1, 0x00);    // Disable all interrupts
   outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
   //outb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
   outb(PORT + 0, 0x02);    // Set divisor to 2 (lo byte) 57600 baud   
   outb(PORT + 1, 0x00);    //                  (hi byte)
   outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
//   outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14 (16?)-byte threshold
   outb(PORT + 2, 0x07);    // Enable FIFO, clear them, with 1-byte threshold
//   outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
//   outb(PORT + 4, 0x00);    // IRQs disabled completely
}


void serial_read_all_registers()
{
	for (int i = 0; i < 8; i++) {
		/*uchar reg =*/ inb(PORT + i);
		//printf("Reg %d value: %x\n", i, (unsigned int)reg);
	}
}


int serial_received()
{
	// Don't ask me why, but adding this fixes everything
	// and I don't wan't to bother what the real problem is.
	static int i = 0;
	i++;
	if (i == 10000)
		serial_read_all_registers();
		
	// until here
   return inb(PORT + 5) & 1;
}


uchar serial_read()
{
   while (serial_received() == 0) {};
 
   return inb(PORT);
}


int serial_is_transmit_empty()
{
	static int count = 0;
	int res = inb(PORT + 5) & 0x20;
	if (res == 0)
		count++;
	else
		count = 0;
	
	if (count == 10000)
	{
		serial_read_all_registers();
		count = 0;
		res = 1;
	}
	return res;
}

 
void serial_write(uchar a)
{
   // separate in two 4 bit sends because it doesn't seem to be able to send 8 bits at once
   while (serial_is_transmit_empty() == 0);
   outb(PORT, a & 0x0F);
   
   while (serial_is_transmit_empty() == 0);
   outb(PORT, (a>>4) & 0x0F);

}


void serial_write_string(char *string)
{
	while (*string != 0)
	{
		serial_write(*string);
		string++;
	}
}


void serial_enter_debug_mode()
{
	printf("Waiting for debug commands via serial port.\n");
	printf("Write an address and I'll tell you its value.");
	printf("Do\n$ grep var_name nopsys.sym in bash to get var_name address in memory of any VM var\n");
	
	serial_init();
	
	
	while (1)
	{
		uchar next_address[4];
		printf("Waiting... ");
		next_address[0] = serial_read();
		printf("Received something: %x\n", next_address[0]);
		next_address[1] = serial_read();
		next_address[2] = serial_read();
		next_address[3] = serial_read();
		
		unsigned int *array_address = (unsigned int*)next_address;
		unsigned int *value = (unsigned int*)*array_address;

		printf("Address is %p. uint32 value: 0x%x (%d dec)\n", value, *value, *value);
	}
}


