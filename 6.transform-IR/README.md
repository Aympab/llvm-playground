# Transforming LLVM IR

In this part we will see how we can trasnform IR from one form to another using
the LLVM optimized tool `opt`.

First, we convert out `multiply.c` file from part `2.lower-C-to-IR` and lower it
to LLVM IR as we've seen with :
```
$ clang -emit-llvm -S multiply.c -o multiply.ll -Xclang -disable-O0-optnone
```

Now, we use the `opt` tool to transform it into a form where memory is promoted
to register :
```
$ opt -mem2reg -S multiply.ll -o multiply1.ll
```

This will simplify our IR for the mult function from 
```llvm
define dso_local i32 @mult() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 5, ptr %1, align 4
  store i32 3, ptr %2, align 4
  %4 = load i32, ptr %1, align 4
  %5 = load i32, ptr %2, align 4
  %6 = mul nsw i32 %4, %5
  store i32 %6, ptr %3, align 4
  %7 = load i32, ptr %3, align 4
  ret i32 %7
}
```
To :
```llvm
define dso_local i32 @mult() #0 {
  %1 = mul nsw i32 5, 3
  ret i32 %1
}
```


# Remarks 
Note that you need to pass the options `-Xclang -disable-O0-optnone` to be able
to run LLVM's optimizer in order to see changes. By default, when
compiling with `O0`, clang prevents further optimizations.