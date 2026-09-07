format MS64 COFF

; db means "define byte"  : it creates bytes in the output at assembly time.
; dw means "define word"  : it creates words in the output at assembly time.
; dd means "define double": it creates double words in the output at assembly time.
; dq means "define quad"  : it creates quad words in the output at assembly time.

section '.data' data readable
	message db 'Hello, world!', 0

section '.text' code readable executable
	public main
	extrn puts

main:
	sub rsp, 40 ; Reserve 32 bytes (4 paramter convention) + and 8 bytes (return address).
	lea rcx, [message]
	call puts
	xor eax, eax
	add rsp, 40
	ret

; The Windows linker resolves the external puts symbol from the C runtime library.
; At runtime, puts writes the null-terminated message to standard output and adds a newline.
