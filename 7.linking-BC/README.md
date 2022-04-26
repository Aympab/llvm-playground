# Linking LLVM bitcode
In this section, we will link generated `.bc` files to get one single bitcode
containing all the references.

We have 2 files of code, where one makes a references to the other.

First, we convert the C code to `.bc` file by lowering it twice :
```
$ clang -emit-llvm -S test1.c -o test1.ll
$ clang -emit-llvm -S test2.c -o test2.ll
```
```
$ llvm-as test1.ll -o test1.bc
$ llvm-as test2.ll -o test2.bc
```
Now we have our two `.bc` files ready. With `test2.bc` making a referenre to
the `func` syntax from the `test1.bc` file.

Finally, we need to invoke the `llvm-link` command in order to link our two LLVM
bitcode files :

```
$ llvm-link test1.bc test2.bc -o output.bc
```

If we upper our code back to LLVM IR using the `llvm-dis` tool, we get the
`output.ll` file, which contains the definition of the two functions : `main`
and `func`.

```
$ llvm-dis output.bc -o output.ll
```

Gives us one file containing the two function definitions :
```llvm
...
; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @func(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  %3 = load i32, ptr %2, align 4
  %4 = mul nsw i32 %3, 2
  store i32 %4, ptr %2, align 4
  %5 = load i32, ptr %2, align 4
  ret i32 %5
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  store i32 5, ptr %2, align 4
  %3 = load i32, ptr %2, align 4
  %4 = call i32 @func(i32 noundef %3)
  store i32 %4, ptr %2, align 4
  %5 = load i32, ptr %2, align 4
  %6 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %5)
  %7 = load i32, ptr %2, align 4
  ret i32 %7
}
...
```