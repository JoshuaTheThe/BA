# A very very Tiny language
the goal was to make one of the smallest compilers possible, yet still being useful.<br>
i will eventually port this to DekOS.<br>

FINAL* SIZE IS 5238 BYTES<br>
use sstrip to strip to final size<br>

only works in i386 32 bit mode<br>

to build normally, just use
cc src/ba.c src/crt.S -m32 -o bin/ba
