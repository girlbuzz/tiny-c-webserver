#!/bin/bash
tcc webserver.c -nostdlib -c -vv -o ./bin/webserver.o
ld.gold ./bin/webserver.o -o ./bin/webserver
strip --strip-all ./bin/webserver
objcopy --strip-section-headers ./bin/webserver ./bin/webserver
echo -n "Compiled webserver, executable size (bytes): "
stat --format=%s ./bin/webserver