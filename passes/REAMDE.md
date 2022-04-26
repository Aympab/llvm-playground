Trying several simple passes
==============================

`a-original.ll` contains the main code.

Run : `opt -S -wanted_pass a-original.ll -o output.ll`

Differents passes are for example :
- **instcombine** : combines the instructions
- **deadargelim** : gets rid of the dead argument