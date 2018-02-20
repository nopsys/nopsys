#ifndef _NOPSYS_INTS_H_
#define _NOPSYS_INTS_H_

#include "types.h"


#define REAL_TIMER_FREQUENCY	2000
#define TIMER_DIVISOR	((int)(4772727/4/REAL_TIMER_FREQUENCY+1))
#define TIMER_FREQUENCY	(4772727.0/4/TIMER_DIVISOR)

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

void isr_void();
void set_idt(uint32_t index, void *handler);

void ints_master_pic_int_ended();
void ints_slave_pic_int_ended();

#endif /* _NOPSYS_INTS_H_ */

