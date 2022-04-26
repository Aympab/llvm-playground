# Lowering C source code to LLVM assembly (with clang)

To emit LLVM IR from a c file, run :

```
clang -emit-llvm -S myFile.c -o myIR.ll
```

## How does it works ?
1. The *lexer* breaks C code into a token stream (token example : identifier,
literal, operator, ...)
2. Feed the token stream to the parser, which builds the Absract Syntax Tree
(**AST**) with the help of Context Free Grammar (**CFG**) for the language
3. Semantic analysis
4. IR generation