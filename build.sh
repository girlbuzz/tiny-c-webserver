#!/bin/sh

#clean
rm -f *.o webserver *-webserver

#compile
for i in tcc gcc clang; do
	$i -c webserver.c -o $i-nostdinc-webserver.o
done

#link
for i in tcc gcc clang; do
	gcc -static -fwhole-program -nostartfiles -nostdlib $i-nostdinc-webserver.o -o $i-nostdinc-gcc-webserver
	ld.bfd $i-nostdinc-webserver.o -o $i-nostdinc-bfd-webserver
	ld.gold $i-nostdinc-webserver.o -o $i-nostdinc-gold-webserver
	ld.lld $i-nostdinc-webserver.o -o $i-nostdinc-lld-webserver
	mold $i-nostdinc-webserver.o -o $i-nostdinc-mold-webserver
	tcc -static -nostdlib $i-nostdinc-webserver.o -o $i-nostdinc-tcc-webserver
done

#smaller
for i in tcc gcc clang; do
	for ii in gcc bfd gold lld mold tcc; do
		strip --strip-all $i-nostdinc-$ii-webserver
		objcopy --strip-section-headers $i-nostdinc-$ii-webserver
	done
done

#size
for i in tcc gcc clang; do
	for ii in gcc bfd gold lld mold tcc; do
		printf '%s %s\n' `stat --format=%s $i-nostdinc-$ii-webserver` nostdinc_${i}_${ii}
	done
done
