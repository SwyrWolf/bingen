format MS64 COFF

section '.text' code readable executable

public strcmp

strcmp:
	;	Windows x64 calling convention:
	;		1st arg -> rcx (const char* lhs)
	;		2nd arg -> rdx (const char* rhs)
	;		return value -> eax (int)

compare_loop:
	movzx eax, byte [rcx]
	movzx r8d, byte [rdx]

	cmp eax, r8d
	jne compare_done

	test eax, eax
	je compare_equal

	inc rcx
	inc rdx
	jmp compare_loop

compare_done:
	sub eax, r8d
	ret

compare_equal:
	xor eax, eax
	ret
