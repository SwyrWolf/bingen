# COFF file encoding
COFF **(Common Object File Format)** is a format for executable object code.
Primarily used for Windows object files before extended further into PE **(Portable Executable)**
files which are `.exe` or `.dll`

## Table of Contents
- [Header](#header)
  - [Machine Architecture Options](#machine-architecture-options)
- [Section Header](#section-header)
  - [Section Header Name Options](#section-header-name-options)
  - [Section Header Characteristics Flags](#section-header-characteristics-flags)
- [Section Data](#section-data)
- [Symbol Table](#symbol-table)
- [String Table](#string-table)

## Header

| Field | Value | Description |
| --- | --- | --- |
| Machine | `0x8664` | Targeted CPU architecture <Example: x86-64>. |
| Sections Quantity | `0x0001` | How many sections are in the file <Example: 1>. |
| Timestamp | `0x00000000` | Unix epoch (can be zero). |
| Pointer to symbol table | `0x00000042` | File offset of the COFF symbol table. |
| Quantity of symbols | `0x00000001` | Indicates that the symbol table contains one entry. |
| Optional header size | `0x0000` | Optional (zero). |
| Characteristics | `0x0000` | No file-level characteristic flags are set. |

### Machine Architecture Options

| Architecture | Value | Description |
| --- | --- | --- |
| Unknown | `0x0000` | The target architecture is unspecified. |
| Intel i386 | `0x014C` | 32-bit Intel-compatible architecture. |
| MIPS little-endian | `0x0166` | MIPS R4000 architecture. |
| Hitachi SH3 | `0x01A2` | Hitachi SuperH 3 architecture. |
| Hitachi SH4 | `0x01A6` | Hitachi SuperH 4 architecture. |
| ARM | `0x01C0` | 32-bit ARM architecture. |
| ARM Thumb | `0x01C2` | ARM Thumb instruction set. |
| ARM Thumb-2 | `0x01C4` | ARMv7 Thumb-2 instruction set. |
| IBM PowerPC | `0x01F0` | 32-bit PowerPC architecture. |
| Intel Itanium | `0x0200` | Intel IA-64 architecture. |
| RISC-V 32-bit | `0x5032` | 32-bit RISC-V architecture. |
| RISC-V 64-bit | `0x5064` | 64-bit RISC-V architecture. |
| RISC-V 128-bit | `0x5128` | 128-bit RISC-V architecture. |
| LoongArch 32-bit | `0x6232` | 32-bit LoongArch architecture. |
| LoongArch 64-bit | `0x6264` | 64-bit LoongArch architecture. |
| AMD64 | `0x8664` | 64-bit x86 architecture used by this object file. |
| ARM64 | `0xAA64` | 64-bit ARM architecture. |
| EFI bytecode | `0x0EBC` | Platform-independent EFI bytecode. |

---
---
---

## Section Header
### Section Header binary structure

| Field | Value | Description |
| --- | --- | --- |
| Name | `.text\0\0\0` | Eight-byte, null-padded name of the code section. |
| Virtual size | `0x00000000` | Unused for a COFF object file. |
| Virtual address | `0x00000000` | Assigned later by the linker. |
| Size of raw data | `0x00000006` | The section contains six bytes of machine code. |
| Pointer to raw data | `0x0000003C` | File offset at which the section's bytes begin. |
| Pointer to relocations | `0x00000000` | No relocation table is present for this section. |
| Pointer to line numbers | `0x00000000` | No deprecated COFF line-number table is present. |
| Number of relocations | `0x0000` | The code has no addresses that the linker must adjust. |
| Number of line numbers | `0x0000` | No COFF line-number records are present. |
| Characteristics | `0x60500020` | Combined flags defining the section as aligned, executable, readable code. |

### Section Header Name Options

- `.text` — Executable machine code.
- `.data` — Initialized, writable data.
- `.rdata` — Initialized, read-only data such as constants and string literals.
- `.bss` — Uninitialized, writable data that occupies no space in the object file.
- `.pdata` — Function table entries used for exception handling on x64.
- `.xdata` — Unwind and exception-handling information referenced by `.pdata`.
- `.drectve` — Linker directives embedded by the compiler.
- `.debug$*` — Debugging information, commonly stored in CodeView format.

> *These are standard section names used by Windows PE/COFF tools. Some names have
special meaning to the Microsoft linker or Windows runtime, while others mainly
describe the section's intended contents. The section's `Characteristics` flags,
rather than its name alone, determine its memory permissions and basic behavior.*

### Section Header Characteristics Flags
> The characteristics value is a bitmask composed of the following flags:

| Flag | Value | Description |
| --- | --- | --- |
| Contains code | `0x00000020` | Marks the section as executable instructions rather than data. |
| 16-byte alignment | `0x00500000` | Requests that the linker align the section on a 16-byte boundary. |
| Executable | `0x20000000` | Allows the section's memory pages to be executed. |
| Readable | `0x40000000` | Allows the section's memory pages to be read. |

## Section Data

The `.text` section implements `square(int value)`. Under the Windows x64
calling convention, the first integer argument arrives in `ecx` and the return
value is placed in `eax`.

```asm
mov  eax, ecx  ; 89 C8
imul eax, ecx  ; 0F AF C1
ret            ; C3
```

| Instruction | Description |
| --- | --- |
| `mov eax, ecx` | Copies the argument into the return-value register. |
| `imul eax, ecx` | Multiplies the value by itself and stores the low 32 bits in `eax`. |
| `ret` | Returns control to the caller with the result in `eax`. |

## Symbol Table

The symbol table exposes `square` to the linker so other object files can refer
to the function.

| Field | Value | Description |
| --- | --- | --- |
| Name | `square\0\0` | Eight-byte, null-padded symbol name. |
| Value | `0x00000000` | Offset of the symbol from the beginning of its section. |
| Section number | `0x0001` | Associates the symbol with the first section, `.text`. |
| Type | `0x0020` | Identifies the symbol as a function. |
| Storage class | `0x02` | Marks the symbol as external and visible to the linker. |
| Number of auxiliary records | `0x00` | No additional symbol records follow this entry. |

## String Table

The string table stores names that do not fit in the fixed eight-byte name
fields used by section and symbol records. Because all names in this file fit
inline, the table contains only its required four-byte size field.

| Field | Value | Description |
| --- | --- | --- |
| Total size | `0x00000004` | Four-byte size field with no additional string data. |
