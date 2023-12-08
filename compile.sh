#!/bin/bash
for file in *.c; do
    filename=$(basename "$file" .c)
    tcc "$filename.c" -nostdlib -c -vv
    ld.gold "$filename.o" -o "$filename"
    strip --strip-all "$filename"
    objcopy --strip-section-headers "$filename" "$filename"
    echo -n "Compiled $filename, executable size (bytes): "
    stat --format=%s $filename
done