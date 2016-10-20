CFLAGS=-std=c99 -Wall -Wextra -pedantic

default:
	$(CC) $(CFLAGS) hex2bin.c -o hex2bin
	$(CC) $(CFLAGS) rot13.c -o rot13
	$(CC) $(CFLAGS) xor.c -o xor

clean:
	rm -f hex2bin rot13 xor
