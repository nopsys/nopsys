#ifndef _NOPSYS_INTS_H_
#define _NOPSYS_INTS_H_

#include "types.h"

#define PIT_FREQ_HZ_INT   4772727/4      // the hardware native freq 
#define PIT_FREQ_HZ_FLOAT 4772727/4.0

// Math:
// PIT_FREQ_HZ / aDivisor = desired_freq. => PIT_FREQ / desired_freq = aDivisor 

#define TIMER_DESIRED_INT_FREQ_HZ	2000  // 0.5 msecs
#define TIMER_DIVISOR	((int)(PIT_FREQ_HZ_INT   / TIMER_DESIRED_INT_FREQ_HZ+1))
#define TIMER_FREQ_HZ	      (PIT_FREQ_HZ_FLOAT / TIMER_DIVISOR)

#define IRQ_NONE	    0x00
#define IRQ_TIMER	    0x01
#define IRQ_KEYBOARD    0x02
#define IRQ_CASCADE     0x04
#define IRQ_3		    0x08
#define IRQ_4		    0x10
#define IRQ_5		    0x20
#define IRQ_6		    0x40
#define IRQ_7		    0x80
#define IRQ_PS2		    0x10

typedef struct idt_descriptor_t {
        uint16_t limit;
        void *addr;
} __attribute__((packed)) idt_descriptor_t;

#ifdef __x86_64__
typedef struct idt_entry_t {
    uint16_t offset_0_15;
    uint16_t segsel;
    uint16_t attr;
    uint16_t offset_16_31;
	uint32_t offset_32_63;
	uint32_t reserved;
} __attribute__((__packed__, aligned (16))) idt_entry_t;
#else
#error(unsupported)
typedef struct idt_entry_t {
    uint16_t offset_0_15;
    uint16_t segsel;
    uint16_t attr;
    uint16_t offset_16_31;
} __attribute__((__packed__, aligned (8))) idt_entry_t;
#endif

typedef int irq_semaphores_t[16];

extern irq_semaphores_t irq_semaphores;

extern volatile uint64_t nopsys_ticks;

extern uint64_t nopsys_tsc_freq;

uint64_t get_tsc();

void isr_void();
void set_idt(uint32_t index, void *handler);

void ints_master_pic_int_ended();
void ints_slave_pic_int_ended();

// exceptions
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

// external interrupts (PIC)
void isr_clock_ASM();
void isr_33_ASM();
void isr_34_ASM();
void isr_35_ASM();
void isr_36_ASM();
void isr_37_ASM();
void isr_38_ASM();
void isr_39_ASM();
void isr_40_ASM();
void isr_41_ASM();
void isr_42_ASM();
void isr_43_ASM();
void isr_44_ASM();
void isr_45_ASM();
void isr_46_ASM();
void isr_47_ASM();


#endif /* _NOPSYS_INTS_H_ */

