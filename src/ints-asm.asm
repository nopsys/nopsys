
extern ints_signal_master_semaphore
extern ints_signal_slave_semaphore
extern ints_init_structs

extern idt_descriptor


global get_CS
global get_CR2
global inb
global outb
global isr_void
global ints_init

%define ALIGN align 16 ;   as ".align 16"


%macro SAVE_CONTEXT 0
;	xchg bx, bx
	push es
	push ds
	pushad
%endmacro

%macro RESTORE_CONTEXT 0
	popad
	pop ds
	pop es
%endmacro

%macro DEFINE_HANDLER_PROLOGUE 1 ; arg: isr name
global isr_%1_ASM

ALIGN
isr_%1_ASM:
%endmacro

%macro DEFINE_HANDLER_SIMPLE 1   ; arg: isr name
extern isr_%1_C
	DEFINE_HANDLER_PROLOGUE %1
	SAVE_CONTEXT
	call isr_%1_C
	RESTORE_CONTEXT
	iret
%endmacro

%macro DEFINE_HANDLER_PASS_CONSTANT 2    ; args: number, C handler
extern %2
	DEFINE_HANDLER_PROLOGUE %1
	SAVE_CONTEXT
	push %1
	call %2
	add esp, 4
	RESTORE_CONTEXT
	iret	
%endmacro

%macro DEFINE_MASTER_SEMAPHORE_ISR 1
	DEFINE_HANDLER_PASS_CONSTANT %1,ints_signal_master_semaphore
%endmacro

%macro DEFINE_SLAVE_SEMAPHORE_ISR 1
	DEFINE_HANDLER_PASS_CONSTANT %1,ints_signal_slave_semaphore
%endmacro

%macro DEFINE_HANDLER_WITH_ERROR_CODE 1 ; arg: isr name
extern isr_%1_C
	DEFINE_HANDLER_PROLOGUE %1
	push eax
	mov  eax, [esp+4]  ; error code
	SAVE_CONTEXT
	push eax
	call isr_%1_C
	add esp, 4
	RESTORE_CONTEXT
	pop eax            ; restore eax
	add esp, 4         ; remove error code from tos
	iret
%endmacro



section .text

DEFINE_HANDLER_SIMPLE clock 


DEFINE_MASTER_SEMAPHORE_ISR 1
DEFINE_MASTER_SEMAPHORE_ISR 2
DEFINE_MASTER_SEMAPHORE_ISR 3
DEFINE_MASTER_SEMAPHORE_ISR 4
DEFINE_MASTER_SEMAPHORE_ISR 5
DEFINE_MASTER_SEMAPHORE_ISR 6
DEFINE_MASTER_SEMAPHORE_ISR 7

DEFINE_SLAVE_SEMAPHORE_ISR 8
DEFINE_SLAVE_SEMAPHORE_ISR 9
DEFINE_SLAVE_SEMAPHORE_ISR 10
DEFINE_SLAVE_SEMAPHORE_ISR 11
DEFINE_SLAVE_SEMAPHORE_ISR 12
DEFINE_SLAVE_SEMAPHORE_ISR 13
DEFINE_HANDLER_WITH_ERROR_CODE page_fault
DEFINE_SLAVE_SEMAPHORE_ISR 15

ALIGN
isr_void:
	inc DWORD [0xb800c]  ; show something on screen
	push eax
	mov al, 0x20         ; command code: end-of-interrupt
	out 0x20, al         ; send command to master-pic port
	pop	eax
    iret


ints_init:
	;xchg bx, bx
	clts       ; Clear Task Switch flag, just in case the bootloader left it set (OFW does)
	fninit     ; Initialize FPU, don't check for pending exceptions
	cli        ; Stop interrupts
	
	call ints_init_structs

    lidt [idt_descriptor]
	sti        ; Resume interrupts
	ret


get_CS:
	mov ax, cs
	ret

get_CR2:
	mov eax, cr2
	ret

inb:   ; uint8_t inb(uint16_t port)
	mov dx, [esp+4]    ; port
	in al, dx
	ret

outb:    ;void outb(uint16_t port, uint8_t byte)
	mov dx, [esp+4]
	mov al, [esp+8]
	out dx, al
	ret







