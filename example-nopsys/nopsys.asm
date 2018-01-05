
extern putc_debug

global nopsys_vm_main
global semaphore_signal_with_index
global debug_print_call_stack

section .data

clock_str: db "|/-\"
clock_idx: db 0

section .text

nopsys_vm_main:
	inc eax
	jmp nopsys_vm_main


semaphore_signal_with_index:
	;xchg bx, bx
	xor eax, eax
	mov al, [clock_idx]
	mov eax, [clock_str+eax]
	inc byte [clock_idx]
	cmp byte [clock_idx], 4
	jb .not_end
	mov byte [clock_idx], 0
.not_end:
	push 40
	push 40
	push eax
	call putc_debug
	add esp, 12
	ret


debug_print_call_stack:
	ret



