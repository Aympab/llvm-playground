# LLVM playground
This repository aims at studying LLVM's framework to get a better understanding
of how a compiler works and what are the steps to get executable machine code
from a high-level, human-readable language.

The exercices of this repository are the oneS from **LLVM Cookbook** written by
Mayur Pandey & Suyog Sarda.

Every recipe is described in the `README.md` file in each folder.

1. [Trying several simple passes](1.basic-passes-utilisation/README.md)
2. [Lowering C source code to LLVM IR](2.lower-C-to-IR/README.md)
3. [Lowering IR to LLVM bitcode](3.lower-IR-to-BC/README.md)
4. [Lowering from bitcode to target machine assembly](4.lower-BC-to-ASM/README.md)
5. [Converting back bitcode to LLVM assembly code (LLVM IR)](5.convert-BC-to-IR/README.md)
6. [Transforming LLVM IR](6.transform-IR/README.md)
7. [Linking LLVM bitcode](7.linking-BC/README.md)
8. [Executing LLVM bitcode](8.execution/README.md)

[Building a toy language (WIP)](toylang/README.md)