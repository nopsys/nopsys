global setjmp
global _setjmp
global longjmp
global _longjmp

; HEAVILY INSPIRED IN LIBMUSL

; int setjmp(jmp_buf env);
; env -> [esp+4]
_setjmp:
setjmp:
	mov eax, [esp+4] ; load the jmp_buf buffer into eax

    ; now store caller saved regs into the buffer

	mov [eax+0], ebx   
	mov [eax+4], esi
	mov [eax+8], edi
	mov [eax+12], ebp

	mov ecx, [esp]     ; take the return address from the stack
	mov [eax+20], ecx

	; save the top of the stack to restore in the longjmp
	lea ecx, [esp+4] 
	mov [eax+16], ecx

	xor eax, eax
	ret


; void longjmp(jmp_buf env, int val);
; env -> [esp+4]
; val -> [esp+8]
_longjmp:
longjmp:
	mov   edx, [esp+4]
	mov   eax, [esp+8]
	test  eax, eax
	jnz next  ; this function never returns 0 (see docs)
	inc   eax

next:
	; restore cdecl registers

	mov   ebx, [edx+0]
	mov   esi, [edx+4]
	mov   edi, [edx+8]
	mov   ebp, [edx+12]

	; restore stack pointer

	mov   ecx, [edx+16]
	mov   esp, ecx

	; return
	mov   ecx, [edx+20]
	jmp ecx


