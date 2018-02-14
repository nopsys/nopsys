
#include "nopsys.h"
#include "ints.h"
#include "libc.h"

#define PIC1_PORT 0x20
#define PIC2_PORT 0xA0

volatile ulong timer = 0;

irq_semaphores_t irq_semaphores;

static idt_entry_t IDT[0x100] = { 0 };

idt_descriptor_t idt_descriptor = { 0 };

void isr_0_ASM();
void isr_1_ASM();
void isr_2_ASM();
void isr_3_ASM();
void isr_4_ASM();
void isr_5_ASM();
void isr_6_ASM();
void isr_7_ASM();
void isr_8_ASM();
void isr_9_ASM();
void isr_10_ASM();
void isr_11_ASM();
void isr_12_ASM();
void isr_13_ASM();
void isr_page_fault_ASM();
void isr_15_ASM();

void isr_clock_ASM();


void reset_pic() {
    outb(PIC1_PORT+0, 0x11); /* IRQs edge triggered, cascade, 8086/88 mode */
    outb(PIC1_PORT+1, 0x20); /* int number (start) */
    outb(PIC1_PORT+1, 0x04); /* PIC1 Master, Slave enters Int through IRQ2 */
    outb(PIC1_PORT+1, 0x01); /* Mode 8086 */
    outb(PIC1_PORT+1, 0xFF); /* Mask all! */

    outb(PIC2_PORT+0, 0x11); /* IRQs edge triggered, cascade, 8086/88 mode */
    outb(PIC2_PORT+1, 0x28); /* int number (start) */
    outb(PIC2_PORT+1, 0x02); /* PIC2 Slave, enters Int through IRQ2 */
    outb(PIC2_PORT+1, 0x01); /* Mode 8086 */
    outb(PIC2_PORT+1, 0xFF); /* Mask all! */
}

void enable_pic() {
    outb(PIC1_PORT+1, 0x00);
    outb(PIC2_PORT+1, 0x00);
}

void ints_init_structs()
{
	for (int i = 0; i < sizeof(irq_semaphores)/sizeof(irq_semaphores[0]); i++)
		irq_semaphores[i]=0;

	// setup void interrupt handlers as default
	for (int i = 0x0; i < 0x100; i++)
		set_idt(i, isr_void);
	
	set_idt(0xe, isr_page_fault_ASM);
	set_idt(0x20, isr_clock_ASM);

	// set exception handlers
	set_idt(0x0, isr_0_ASM);
	set_idt(0x1, isr_1_ASM);
	set_idt(0x2, isr_2_ASM);
	set_idt(0x3, isr_3_ASM);
	set_idt(0x4, isr_4_ASM);
	set_idt(0x5, isr_5_ASM);
	set_idt(0x6, isr_6_ASM);
	set_idt(0x7, isr_7_ASM);
	set_idt(0x8, isr_8_ASM);
	set_idt(0x9, isr_9_ASM);
	set_idt(0xa, isr_10_ASM);
	set_idt(0xb, isr_11_ASM);
	set_idt(0xc, isr_12_ASM);
	set_idt(0xd, isr_13_ASM);
	set_idt(0xf, isr_15_ASM);

	reset_pic();

	// set timer frequency
	outb(0x43, 0x34);	// timer 0, mode binary, write 16 bits count
	outb(0x40, TIMER_DIVISOR & 0xff);
	outb(0x40, (TIMER_DIVISOR >> 8) & 0xff);
	outb(0x21, ~(IRQ_TIMER));
	
	enable_pic();

    idt_descriptor.addr = IDT;
    idt_descriptor.limit = sizeof(IDT) - 1;
}


void set_idt(unsigned int index, void *handler)
{
	IDT[index].segsel = get_CS();
	IDT[index].offset_0_15  = ((uint32_t)handler) & 0xffff;
	IDT[index].attr = 0x8E00; // interrupt, dpl 0
	IDT[index].offset_16_31 = ((uint32_t)handler) >> 16;
}


void ints_master_pic_int_ended()
{
	uchar code = 0x20;  // end-of-interrupt
	uchar port = 0x20;  // master pic
	outb(port, code);
}

void ints_slave_pic_int_ended()
{
	uchar code = 0x20;  // end-of-interrupt
	uchar port = 0xA0;  // slave pic
	outb(port, code);
}

// when a pic interrupt arrives, signal the semaphore. 
// If already signaled tell it ended... FIXME: why only in that case? think and explain

void ints_signal_master_semaphore(int number)
{
	//if (irq_semaphores[number] != 0)
		semaphore_signal_with_index(irq_semaphores[number]);
	//else
	ints_master_pic_int_ended();
}


void ints_signal_slave_semaphore(int number)
{
	if (irq_semaphores[number] != 0)
		 semaphore_signal_with_index(irq_semaphores[number]);
	else
	{
		//ints_slave_pic_int_ended();
		//ints_master_pic_int_ended();
	}
}

void isr_clock_C() 
{
	timer++;
	// *(long*)(0xfd000000+timer)=0;
	ints_signal_master_semaphore(0);
	//ints_master_pic_int_ended();
}

#define sti()        	    __asm("sti")

void isr_page_fault_C(uint32_t error_code)
{
/*
	computer_t *computer = current_computer();
	computer->in_page_fault++;
	computer->total_page_faults++;

	void *virtual_address_failure = get_CR2();

	printf_tab("PageFaultISR: Entre\n");
	printf_inc_tab();
	//printf_tab("PageFaultISR: Esta en la rootTable: %d\n",is_inside_root_table(virtual_address_failure));
	computer->page_fault_address = virtual_address_failure;
	
	sti();      //////////// why?
	//paging_handle_fault(error_code);

	printf_dec_tab();
	computer->in_page_fault--;
	printf_tab("PageFaultISR: Sali\n");
*/
}



