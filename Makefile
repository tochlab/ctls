CFLAGS=-std=c99 -Wall -Wextra -pedantic -g -lpthread

default:
	$(CC) $(CFLAGS) hex2bin.c -o hex2bin
	$(CC) $(CFLAGS) rot13.c -o rot13
	$(CC) $(CFLAGS) xor.c -o xor
	$(CC) $(CFLAGS) lib/listdir.c listdir.c -o listdir
	#$(CC) $(CFLAGS) lib/rawfs.c lib/queue.c rawtest.c -o rawtest
	$(CC) $(CFLAGS) randomq.c -o randomq
	$(CC) $(CFLAGS) cbuf.c lib/cbuf.c -o cbuf

clean:
	rm -f hex2bin rot13 xor listdir randomq cbuf
