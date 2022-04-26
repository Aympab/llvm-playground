Trying several simple passes
==============================

`a-original.ll` contains the basic, unpotimized code.

Run :
```
$ opt -S -wanted_pass a-original.ll -o output.ll
```

Differents passes are for example :
- **instcombine** : combines the instructions
- **deadargelim** : gets rid of the dead argument
- **loweratomic** :  lowers atomic intrinsics to non-atomic form
- **mem2reg** : promotes memoty to register
- **memcpyopt** : MemCpy optimization
- ...

One can write a specific pass. [Here](https://llvm.org/docs/Passes.html) is the list of all available pass for LLVM.