#!/bin/sh

for COMPILER in gcc clang tcc; do
	CC=$COMPILER make clean webserver.o >/dev/null 2>&1

	for LINKER in gcc clang tcc ld.bfd ld.gold ld.lld mold; do
		OUT="$COMPILER-nostinc-$LINKER-webserver"

		CC=$COMPILER LD=$LINKER LDFLAGS="-nostdinc -nostartfiles -s" OUT=$OUT make >/dev/null 2>&1

		if [ $? -ne 0 ]; then
			printf '\033[0;31mFailed\033[0m to compile %s\n' $OUT
		else
			SIZE=$(stat --format=%s $OUT)
			printf 'Size of %s: %u bytes\n' "$OUT" "$SIZE"
		fi
	done
done
