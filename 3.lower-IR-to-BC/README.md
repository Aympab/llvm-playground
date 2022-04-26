# Lowering IR to LLVM bitcode

To lower from LLVM-IR / assembly code to bitcode format, run :

`llvm-as mult.ll -o mult.bc`

This will give a file full of unreadable caracters with a default IDE since it
is bitcode. To be able to "read" the file, use the `hexdump` tool :
`hexdump -C mult.bc`

First 10 lines : 
```
00000000  42 43 c0 de 35 14 00 00  05 00 00 00 62 0c 30 24  |BC..5.......b.0$|
00000010  4d 59 be 66 9d fb b4 4f  1b c8 24 44 01 32 05 00  |MY.f...O..$D.2..|
00000020  21 0c 00 00 07 01 00 00  0b 02 21 00 02 00 00 00  |!.........!.....|
00000030  16 00 00 00 07 81 23 91  41 c8 04 49 06 10 32 39  |......#.A..I..29|
00000040  92 01 84 0c 25 05 08 19  1e 04 8b 62 80 0c 45 02  |....%......b..E.|
00000050  42 92 0b 42 64 10 32 14  38 08 18 4b 0a 32 32 88  |B..Bd.2.8..K.22.|
00000060  48 70 c4 21 23 44 12 87  8c 10 41 92 02 64 c8 08  |Hp.!#D....A..d..|
00000070  b1 14 20 43 46 88 20 c9  01 32 32 84 18 2a 28 2a  |.. CF. ..22..*(*|
```