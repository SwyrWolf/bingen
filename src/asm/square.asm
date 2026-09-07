format MS64 COFF

section '.text' code readable executable

public square

square:
	mov eax, ecx   ; Copy the input into the return-value register.
	imul eax, ecx  ; eax = input * input.
	ret            ; Return with the result in eax.

; Notes about this assembly file:
; Windows x64 calling convention: the first int argument arrives in ecx.
; Integer return values are passed back in eax.