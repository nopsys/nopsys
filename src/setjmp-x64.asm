global setjmp
global _setjmp
global longjmp
global _longjmp

; HEAVILY INSPIRED IN LIBMUSL

; int setjmp(jmp_buf env);
; env -> rdi
_setjmp:
setjmp:
    ; store caller saved regs into the buffer

	mov [rdi+0], rbx   
	mov [rdi+8], rbp
	mov [rdi+16], r12
	mov [rdi+24], r13
	mov [rdi+32], r14
	mov [rdi+40], r15

	mov rcx, [rsp]     ; take the return address from the stack
	mov [rdi+48], rcx

	; save the top of the stack to restore in the longjmp
	lea rcx, [rsp+8] 
	mov [rdi+56], rcx

	xor rax, rax
	ret


; void longjmp(jmp_buf env, int val);
; env -> rdi
; val -> rsi
_longjmp:
longjmp:
	mov rax, rsi
	test  rax, rax
	jnz next  ; this function never returns 0 (see docs)
	inc   rax

next:
	; restore cdecl registers

	mov   rbx, [rdi+0]
	mov   rbp, [rdi+8]
	mov   r12, [rdi+16]
	mov   r13, [rdi+24]
	mov   r14, [rdi+32]
	mov   r15, [rdi+40]

	; restore stack pointer

	mov   rcx, [rdi+56]
	mov   rsp, rcx

	; return
	mov   rcx, [rdi+48]
	jmp rcx


