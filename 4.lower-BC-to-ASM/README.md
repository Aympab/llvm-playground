# Lowering from bitcode to target machine assembly

Now we are going to use the LLVM static compiler `llc` to lower the code once
more.

Run :
```
llc mult.bc -o mult.s
```

Or using Clang API :
```
clang -S mult.bc -o mult.s -fomit-frame-pointer
```

We use the `-fomit-frame-pointer` option because by default Clang does not elimate the frame pointer whereas `llc` does it by default.


## Remarks

Here we did not mention the target architecture, by default the assembly code is
generated for the host's architecture. One can toggle different options
depending on where the code is going to be run.
- **`-march=architecture`** : target a specific hardware
- **`-mcpu=cpu`** : specify a CPU whitin the architecture
- **`-regalloc=basic/greedy/fast/pbqp`** : type of register allocation