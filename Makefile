CFLAGS=-std=c99 -Wall -Wextra -pedantic -g -lpthread

default:
	$(CC) $(CFLAGS) hex2bin.c -o hex2bin
	$(CC) $(CFLAGS) rot13.c -o rot13
	$(CC) $(CFLAGS) xor.c -o xor
	$(CC) $(CFLAGS) lib/listdir.c listdir.c -o listdir
	$(CC) $(CFLAGS) lib/rawfs.c rawtest.c -o rawtest

clean:
	rm -f hex2bin rot13 xor listdir rawtest
