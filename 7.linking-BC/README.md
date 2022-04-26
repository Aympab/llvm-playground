# Linking LLVM bitcode
In this section, we will link generated `.bc` files to get one single bitcode
containing all the references.

We have 2 files of code, where one makes a references to the other.

First, we convert the C code to `.bc` file by lowering it twice :
```
clang -emit-llvm -S test1.c -o test1.ll
clang -emit-llvm -S test2.c -o test2.ll
```
```
llvm-as test1.ll -o test1.bc
llvm-as test2.ll -o test2.bc
```
Now we have our two `.bc` files ready. With `test2.bc` making a referenre to
the `func` syntax from the `test1.bc` file.

Finally, we need to invoke the `llvm-link` command in order to link our two LLVM
bitcode files :

```
llvm-link test1.bc test2.bc -o output.bc
```

If we upper our code back to LLVM IR, we get the `output.ll` file, which
contains the definition of the two functions : `main` and `func`.