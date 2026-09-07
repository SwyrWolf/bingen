.code

PUBLIC square

square PROC
	; Windows x86_64 calling convention: the first int argument arrives in ecx.
	; Integer return values are passed back in eax.
	mov eax, ecx   ; Copy the input into the return-value register.
	imul eax, ecx  ; eax = input * input.
	ret            ; Return with the result in eax.
square ENDP

END
