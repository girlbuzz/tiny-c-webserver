# A webserver in an 820-byte ELF file
This is achieved by:
* Using the tiny C compiler
* Compiling without the standard library, instead using inline assembly syscalls
* Using ld.gold
* Stripping the binary as much as possible
* Doing plenty of other weird things

## Issues:
* Ctrl+C causes socket still open weird annoying cant bind port problem iushdfoihfgoiudfhgouihdsf but i dont really care
