# Converting back bitcode to LLVM assembly code (LLVM IR)

Here we need the *dissasembler* tool : `llvm-dis`.

Run (with the right paths and filename) :
```
$ llvm-dis mult.bc -o dismult.ll
```

In our case, the output is the same as the one we created in
`../3.lower-IR-to-BC`.