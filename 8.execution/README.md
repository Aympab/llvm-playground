# Executing LLVM bitcode
Now that we know how to lower our code to LLVM bitcode, or even ASM, we want to
execute it.

To do this, we need to invoke the `lli` command. If we do this on our
`output.bc` file from `7.linking-BC`, we will get the output on console.

```
$ lli output.bc

Number is : 10
```
