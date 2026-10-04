A raw initializer uses a two-digit hex, byte count from `00` to `FF` (255 bytes).
Hexadecimal values are case insensitive. 
Whitespace is ignored.

encoded as unsigned numeric as native-endian:
`SW` (Single Word : 2 bytes)
`DW` (Double Word : 4 bytes)
`QW` (Quad Word   : 8 bytes)

```text
08{".text" 00 00 00} // .text000
DW{00 00 00 00}      // Virtual Size = 0
DW{00 00 00 00}      // Virtual Address = 0
DW{00 00 00 06}      // Size of Data = 6
DW{00 00 00 3C}      // Pointer to Raw data = 0x3C = 60
DW{00 00 00 00}      // Pointer to Relocations = 0
DW{00 00 00 00}      // Pointer to Line Numbers = 0
SW{00 00}            // Number of Line Numbers = 0
DW{60 50 00 20}      // Characteristics = 0x60500020
```