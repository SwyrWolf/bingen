format MS64 COFF

; char helpMsg[] = "Input: <integer>"; the final 0 is '\0'.
; db means define bytes: this stores the characters, like char[], not a pointer.
section '.data' data readable
	helpMsg db "Input: <integer>", 0
	invalidMsg db "Error: enter a whole decimal integer in the signed 64-bit range.", 0

section '.text' code readable executable
	public main
	public print_decimal
	extrn puts
	extrn strtoll
	extrn _errno

main:
	; Entry RSP is 8 bytes off a 16-byte boundary. Subtracting 56 aligns calls.
	; Bytes 0..31 are shadow space; 32..55 hold three local pointers.
	sub rsp, 56
	cmp ecx, 2
	jne .usage

	mov rcx, [rdx + 8] ; argv[1]: pointer to the input string.
	mov [rsp + 32], rcx ; Save it because function calls may overwrite RCX.
	call _errno ; Windows CRT: returns a pointer to this thread's errno.
	mov [rsp + 48], rax ; Keep the errno pointer across strtoll.
	mov dword [rax], 0 ; Clear old errors so we detect only this conversion's error.

	mov rcx, [rsp + 32]
	lea rdx, [rsp + 40] ; &endptr: strtoll writes where parsing stopped.
	mov r8d, 10 ; base = 10
	call strtoll

	mov rdx, [rsp + 40]
	cmp rdx, [rsp + 32] ; endptr == input means no digits were consumed.
	je .invalid
	cmp byte [rdx], 0 ; Require the entire string: reject suffixes like "123abc".
	jne .invalid
	mov r10, [rsp + 48]
	cmp dword [r10], 0 ; Overflow sets errno (ERANGE), even though RAX is clamped.
	jne .invalid
	mov rcx, rax
	call print_decimal

	xor eax, eax
	add rsp, 56
	ret

.usage:
	lea rcx, [helpMsg]
	jmp .error

.invalid:
	lea rcx, [invalidMsg]
.error:
	call puts
	mov eax, 1
	add rsp, 56 ; Restore the same amount reserved on entry, on every path.
	ret

; void print_decimal(int64_t number)
; rcx holds the signed integer. puts adds a newline after the digits.
print_decimal:
	sub rsp, 56 ; 32 bytes of shadow space + 24 bytes for the string.
	mov rax, rcx
	mov r9, rcx ; Keep the original sign.
	test rax, rax
	jns .magnitude_ready
	neg rax ; INT64_MIN becomes 2^63, which unsigned div handles correctly.

.magnitude_ready:
	lea r8, [rsp + 55]
	mov byte [r8], 0 ; Null terminator; build the digits backwards.
	mov r10d, 10

.digit_loop:
	xor edx, edx ; div uses the unsigned 128-bit dividend rdx:rax.
	div r10 ; rax = quotient, rdx = remainder (0 through 9).
	add dl, '0' ; Convert the remainder to an ASCII digit.
	dec r8
	mov [r8], dl
	test rax, rax
	jnz .digit_loop ; Runs at least once so zero prints as "0".

	test r9, r9
	jns .print
	dec r8
	mov byte [r8], '-'

.print:
	mov rcx, r8
	call puts
	add rsp, 56
	ret

; fasm src/asm/decimal.asm build/decimal/d.obj
; clang build/decimal/d.obj -o build/decimal.exe
; ./build/decimal.exe 123456