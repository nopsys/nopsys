
extern ints_signal_master_semaphore
extern ints_signal_slave_semaphore
extern ints_init_structs
extern idt_descriptor
extern printCallStack
extern nopsys_main

extern printf_args
extern sprintf_args
extern snprintf_args


global get_CS
global get_CR2
global get_tsc
global enable_paging_using
global enable_sse
global inb
global outb
global printf
global sprintf
global snprintf

global isr_void
global ints_init
global enable_long_mode

%define ALIGN align 16 ;   as ".align 16"

error_msg : db "Exception! %d", 10, 0

%macro PUSH_ALL_REGS 0
;	push es
;	push ds
	push rax
	push rcx
	push rdx
	push rsp
	push rsi
	push rdi
	push r8
	push r9
	push r10
	push r11
%endmacro

%macro POP_ALL_REGS 0
	pop r11
	pop r10
	pop r9
	pop r8
	pop rdi
	pop rsi
	pop rsp
	pop rdx
	pop rcx
	pop rax
;	pop ds
;	pop es
%endmacro

%macro SAVE_CONTEXT 0
;	xchg bx, bx
	PUSH_ALL_REGS
%endmacro

%macro RESTORE_CONTEXT 0
	POP_ALL_REGS
%endmacro

%macro ISR_HANDLER_PROLOGUE 1 ; arg: isr name
global isr_%1_ASM

ALIGN
isr_%1_ASM:
%endmacro

%macro DEFINE_HANDLER_PASS_CONSTANT 2    ; args: number, C handler
extern %2
	ISR_HANDLER_PROLOGUE %1
	SAVE_CONTEXT
	; push %1 ; this was for 32 bits
	mov rdi, %1
	call %2
	; add rsp, 8 ; this was for 32 bits
	RESTORE_CONTEXT
	iretq	
%endmacro

%macro DEFINE_MASTER_SEMAPHORE_ISR 1
	DEFINE_HANDLER_PASS_CONSTANT %1,ints_signal_master_semaphore
%endmacro

%macro DEFINE_SLAVE_SEMAPHORE_ISR 1
	DEFINE_HANDLER_PASS_CONSTANT %1,ints_signal_slave_semaphore
%endmacro

%macro DEFINE_HANDLER_CALL_C 1 ; arg: isr name
extern isr_%1_C
	ISR_HANDLER_PROLOGUE %1
	push rax
	mov  rax, [esp+8]  ; error code
	SAVE_CONTEXT
	; push rax  ; this was for 32 bits
	mov rdi, rax
	call isr_%1_C
	
	; add rsp, 4 ; idem
	RESTORE_CONTEXT
	pop rax            ; restore eax
	add rsp, 8         ; remove error code from tos
	iretq
%endmacro


%macro DEFINE_HANDLER_NO_ERROR_CODE_STOP 1 ; arg: isr nameAddress
	ISR_HANDLER_PROLOGUE %1
	mov rax, %1

	mov rax, 0
	mov rdi, error_msg
	mov rsi, %1
	call printf

	xchg bx, bx
	jmp $
	jmp [esp]

	;add esp, 8    ; idem prev but using iret. remove error code and iret
	;iret
	jmp isr_handler_common_stop
%endmacro

%macro DEFINE_HANDLER_WITH_ERROR_CODE_STOP 1 ; arg: isr nameAddress
	ISR_HANDLER_PROLOGUE %1
	jmp [esp+8]   ;uncomment if you want the exception to repeat again and again

	;add esp, 8    ; idem prev but using iret. remove error code and iret
	;iret
	mov rax, %1

	mov rax, 0
	mov rdi, error_msg
	mov rsi, %1
	call printf

	jmp isr_handler_common_stop
%endmacro

; ==============================
; end of macros
; ==============================
section .text

isr_handler_common_stop:
	xchg bx, bx
	jmp $


extern isr_clock_C

ISR_HANDLER_PROLOGUE clock
;	xchg bx, bx
	SAVE_CONTEXT
	call isr_clock_C
	RESTORE_CONTEXT
	iretq

DEFINE_MASTER_SEMAPHORE_ISR 33  ; keyboard
DEFINE_MASTER_SEMAPHORE_ISR 34
DEFINE_MASTER_SEMAPHORE_ISR 35
DEFINE_MASTER_SEMAPHORE_ISR 36
DEFINE_MASTER_SEMAPHORE_ISR 37
DEFINE_MASTER_SEMAPHORE_ISR 38
DEFINE_MASTER_SEMAPHORE_ISR 39

DEFINE_SLAVE_SEMAPHORE_ISR 40
DEFINE_SLAVE_SEMAPHORE_ISR 41
DEFINE_SLAVE_SEMAPHORE_ISR 42
DEFINE_SLAVE_SEMAPHORE_ISR 43
DEFINE_SLAVE_SEMAPHORE_ISR 44
DEFINE_SLAVE_SEMAPHORE_ISR 45
DEFINE_SLAVE_SEMAPHORE_ISR 46
DEFINE_SLAVE_SEMAPHORE_ISR 47

DEFINE_HANDLER_NO_ERROR_CODE_STOP 0
DEFINE_HANDLER_NO_ERROR_CODE_STOP 1
DEFINE_HANDLER_NO_ERROR_CODE_STOP 2
DEFINE_HANDLER_NO_ERROR_CODE_STOP 3
DEFINE_HANDLER_NO_ERROR_CODE_STOP 4
DEFINE_HANDLER_NO_ERROR_CODE_STOP 5
DEFINE_HANDLER_NO_ERROR_CODE_STOP 6
DEFINE_HANDLER_NO_ERROR_CODE_STOP 7
DEFINE_HANDLER_WITH_ERROR_CODE_STOP 8
DEFINE_HANDLER_NO_ERROR_CODE_STOP 9
DEFINE_HANDLER_WITH_ERROR_CODE_STOP 10
DEFINE_HANDLER_WITH_ERROR_CODE_STOP 11
DEFINE_HANDLER_WITH_ERROR_CODE_STOP 12
DEFINE_HANDLER_WITH_ERROR_CODE_STOP 13

DEFINE_HANDLER_WITH_ERROR_CODE_STOP 15

DEFINE_HANDLER_CALL_C page_fault

ALIGN
isr_void:
	inc byte [0xb800c]  ; show something on screen
	push rax
	mov al, 0x20         ; command code: end-of-interrupt
	out 0x20, al         ; send command to master-pic port
	pop	rax
    iretq


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
	mov rax, cr2
	ret

get_tsc:
	xor rax, rax
	rdtsc
	shl rdx, 32
	add rax, rdx
	ret

enable_sse:
	mov rax, cr0
	and rax, ~0x4		; clear coprocessor emulation CR0.EM
	or  rax, 0x2		; set coprocessor monitoring  CR0.MP
	mov cr0, rax
	mov rax, cr4
	or  rax, 3 << 9		; set CR4.OSFXSR and CR4.OSXMMEXCPT at the same time
	mov cr4, rax
	ret


global measure_tsc_per_pit_interrupt
extern nopsys_ticks

;uint64_t measure_tsc_per_pit_interrupt
measure_tsc_per_pit_interrupt:
	push rbp
	mov rbp, rsp
	
	xor rax, rax
	mov rcx, [nopsys_ticks]

.wait_loop1:
	cmp rcx, [nopsys_ticks]  ;do nothing until the tick counter is increased
	je .wait_loop1	

	rdtsc
	shl rdx, 32
	add rax, rdx
	mov r11, rax
	
	xor rax, rax
	mov rcx, [nopsys_ticks]

.wait_loop2:
	cmp rcx, [nopsys_ticks]  ;do nothing until the tick counter is increased
	je .wait_loop2	

	rdtsc
	shl rdx, 32
	add rax, rdx
	
	sub rax, r11
	
	pop rbp
	ret



inb:   ; uint8_t inb(uint16_t port)
	mov dx, di    ; port
	in al, dx
	ret

outb:    ;void outb(uint16_t port, uint8_t byte)
	mov dx, di
	mov al, sil
	out dx, al
	ret

inb32:   ; uint8_t inb(uint16_t port)
	mov dx, [esp+4]    ; port
	in al, dx
	ret

outb32:    ;void outb(uint16_t port, uint8_t byte)
	mov dx, [esp+4]
	mov al, [esp+8]
	out dx, al
	ret



;int printf (const char *format, ...)
;{
;	void **arg = (void **) &format;
;	return printf_args(format, arg + 1);
;}

; FIXME: this implementation supports at most 12 arguments. Works by moving the stack up
; 80 bytes. Does not support floats or doubles!!!

printf:
	push rbp
	mov rbp, rsp
	sub rsp, 0x60
                         ;do not touch rdi because it is already expected in rdi by printf_args
	mov [rsp+0x08], rsi
	mov [rsp+0x10], rdx
	mov [rsp+0x18], rcx
	mov [rsp+0x20], r8
	mov [rsp+0x28], r9
	mov r11, [rbp+0x10]  ; first pushed argument
	mov [rsp+0x30], r11
	mov r11, [rbp+0x18]  ; first pushed argument
	mov [rsp+0x38], r11
	mov r11, [rbp+0x20]  ; first pushed argument
	mov [rsp+0x40], r11
	mov r11, [rbp+0x28]  ; first pushed argument
	mov [rsp+0x48], r11
	mov r11, [rbp+0x30]  ; first pushed argument
	mov [rsp+0x50], r11
	mov r11, [rbp+0x38]  ; first pushed argument
	mov [rsp+0x58], r11
	mov r11, [rbp+0x40]  ; first pushed argument
	mov [rsp+0x60], r11

	lea rsi, [rsp+8]
	call printf_args
	add rsp, 0x60

	pop rbp
	ret

;int sprintf (char *formatted, const char *format, ...)
;{
;	void **arg = (void **) &format;
;	return sprintf_args(formatted, format, arg + 1);
;}

sprintf:
	push rbp
	mov rbp, rsp
	sub rsp, 0x60
                         ;do not touch rdi because it is already expected in rdi by sprintf_args
                         ;do not touch rsi because it is already expected in rsi by sprintf_args
	mov [rsp+0x10], rdx
	mov [rsp+0x18], rcx
	mov [rsp+0x20], r8
	mov [rsp+0x28], r9
	mov r11, [rbp+0x10]  ; first pushed argument
	mov [rsp+0x30], r11
	mov r11, [rbp+0x18]  ; first pushed argument
	mov [rsp+0x38], r11
	mov r11, [rbp+0x20]  ; first pushed argument
	mov [rsp+0x40], r11
	mov r11, [rbp+0x28]  ; first pushed argument
	mov [rsp+0x48], r11
	mov r11, [rbp+0x30]  ; first pushed argument
	mov [rsp+0x50], r11
	mov r11, [rbp+0x38]  ; first pushed argument
	mov [rsp+0x58], r11
	mov r11, [rbp+0x40]  ; first pushed argument
	mov [rsp+0x60], r11

	lea rdx, [rsp+0x10]
	call sprintf_args
	add rsp, 0x60

	pop rbp
	ret

;int snprintf (char *formatted, size_t max_size, const char *format, ...)
;{
;	void **arg = (void **) &format;
;	return sprintf_args(formatted, format, arg + 1);
;}

snprintf:
	push rbp
	mov rbp, rsp
	sub rsp, 0x60
                         ;do not touch rdi because it is already expected in rdi by snprintf_args
                         ;do not touch rsi because it is already expected in rsi by snprintf_args
	;mov [rsp+0x10], rdx ;do not touch rsi because it is already expected in rsi by snprintf_args
	mov [rsp+0x18], rcx
	mov [rsp+0x20], r8
	mov [rsp+0x28], r9
	mov r11, [rbp+0x10]  ; first pushed argument
	mov [rsp+0x30], r11
	mov r11, [rbp+0x18]  ; first pushed argument
	mov [rsp+0x38], r11
	mov r11, [rbp+0x20]  ; first pushed argument
	mov [rsp+0x40], r11
	mov r11, [rbp+0x28]  ; first pushed argument
	mov [rsp+0x48], r11
	mov r11, [rbp+0x30]  ; first pushed argument
	mov [rsp+0x50], r11
	mov r11, [rbp+0x38]  ; first pushed argument
	mov [rsp+0x58], r11
	mov r11, [rbp+0x40]  ; first pushed argument
	mov [rsp+0x60], r11

	lea rcx, [rsp+0x18]
	call snprintf_args
	add rsp, 0x60

	pop rbp
	ret

; enable_paging_using(void* page_dir)
; page_dir -> rdi
enable_paging_using:
	mov cr3, rdi ; write the page_dir to cr3

	mov rax, cr0
	mov edx, 0x80000000
	or rax, rdx ; enable paging bit
	mov cr0, rax

	ret


; Global Descriptor Table (64-bit).

GDT64:
    dq 0                 ; The null descriptor.
GDT64_CODE:              ; The code descriptor.
    dq 0                 
GDT64_DATA:              ; The data descriptor.
    dq 0
GDT64_DESC:              ; The GDT-pointer.
    dw $ - GDT64 - 1     ; limit
    dq GDT64             ; address


; ==============================================================================
; this code is executed in 32 bits protected mode
; ==============================================================================
bits 32

%define OFFSET_ATTR1 5
%define OFFSET_ATTR2 6
%define OFFSET_LIMIT_0_15 0

%define GDT_SEL_CODE 8
%define CR4_PAE 1<<5
%define CR0_PG  1<<31
%define PAGE_TABLES_AMOUNT 2048 ; 4096bytes per entry * 512 entries per table * 2048 = 4gb addressable
%define PRESENT_READ_WRITE 3

PAGE_DIRS_AMOUNT equ PAGE_TABLES_AMOUNT / 512  ; 512 PT per PD
DWORD_ENTRIES_AMOUNT equ 1024 + 1024 + 1024 + (1024 * PAGE_TABLES_AMOUNT)

; void clear_page_map()

; It zeroes the maps memory. There are 512 entries per table, but
; we have 32 bit regs to clean them

clear_page_map:
	cld                            ; clear direction flag (just in case grub left garbage)
	xor eax, eax
    mov edi, page_map_l4
	mov ecx, DWORD_ENTRIES_AMOUNT
	rep stosd
	ret


; void initialize_page_map()

; It fills the fields of the map (remember that all map addresses have to be below 4gb,
; so we use just 32 bit pointers

; void initialize_dir(page_dir, page_tables, dir_idx)
;page_dir    -> esp+4
;page_tables -> esp+8
;dir_idx     -> esp+12
initialize_dir:
	push ebp
	mov ebp, esp
	push edi
	push ebx

	mov eax, [ebp+8]
	mov edi, [ebp+12]
	mov ebx, [ebp+16]
	shl ebx, 30; 4096 bpt entry * 512 entries * 512 tables per dir = 1gb

	xor ecx, ecx            ; dir index

.fill_dir:
	cmp ecx, 512
	je .directory_initialized

	mov [eax], edi
	or [eax], dword PRESENT_READ_WRITE

	xor edx, edx

.fill_table:
	cmp edx, 512
	je .table_initialized

	mov [edi], ebx
	or [edi], dword PRESENT_READ_WRITE

	add ebx, 4096
	add edi, 8 ; move to the next 64-bit entry in PT
	inc edx
	jmp .fill_table

.table_initialized:
	add eax, 8 ; move to the next 64-bit entry in PD
	inc ecx
	jmp .fill_dir

.directory_initialized:
	pop ebx
	pop edi
	mov esp, ebp
	pop ebp
	ret


initialize_page_map:
	mov dword [page_map_l4], page_dir_pt 
	or  dword [page_map_l4], PRESENT_READ_WRITE
	
	mov ebx, page_dir_pt
	mov edi, page_dir
	mov esi, page_tables
	xor ecx, ecx


.fill_dir:
	cmp ecx, PAGE_DIRS_AMOUNT
	je .done

	mov dword [ebx], edi
	or  dword [ebx], PRESENT_READ_WRITE

	push ecx
	push esi
	push edi
	call initialize_dir
	add esp, 8
	pop ecx

	add ebx, 8             ; move to next pdpt entry
	add edi, 512 * 8       ; move to next page_dir
	add esi, 512 * 512 * 8 ; we filled a complete dir
	inc ecx
	jmp .fill_dir

.done:
	ret

;void enable_long_mode(int64_t multiboot magic number, int64_t *multiboot data structure)
; warning, adhoc abi
; multiboot magic number -> rsp
; multiboot data structure -> rsp+8
enable_long_mode:

; To enable paging we will need to setup the paging map

	call clear_page_map
	call initialize_page_map	

; Enable physical-address extensions by setting CR4_PAE = 1
	mov eax, cr4
    or eax, CR4_PAE
    mov cr4, eax


; Enable IA-32e mode by setting IA32_EFER_LME = 1
	mov ecx, 0xC0000080          ; Set the C-register to 0xC0000080, which is the EFER MSR.
    rdmsr                        ; Read from the model-specific register.
    or eax, 1 << 8               ; Set the LM-bit which is the 9th bit (bit 8).
    wrmsr                        ; Write to the model-specific register.

; Enable paging
	mov eax, page_map_l4
	mov cr3, eax

    mov eax, cr0
    or eax, CR0_PG

    mov cr0, eax

; Fill the gdt and jump

    mov byte [GDT64_CODE+OFFSET_ATTR1], 10011010b ; Present, Not a TSS (S=1), Access exec/read.
    mov byte [GDT64_CODE+OFFSET_ATTR2], 10100000b ; 64-bit code segment (L=1), G=1.
	mov word [GDT64_CODE+OFFSET_LIMIT_0_15], 0xFFFF;
    mov byte [GDT64_DATA+OFFSET_ATTR1], 10010010b ; Present, Not a TSS (S=1), Access read/write.

	lgdt [GDT64_DESC]         ; Load the 64-bit global descriptor table.
    jmp GDT_SEL_CODE:long_mode

bits 64
long_mode:
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	pop   rdi   ; magic number
	pop   rsi   ; multiboot struct
    call  nopsys_main            ; call kernel proper
    hlt                          ; halt machine should kernel return


section .bss

align 4096
page_map_l4: resq 512
page_dir_pt: resq 512
page_dir:    resq 512 * PAGE_DIRS_AMOUNT
page_tables: resq 512 * PAGE_TABLES_AMOUNT



