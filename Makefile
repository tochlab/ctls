CFLAGS=-std=c99 -Wall -Wextra

default:
	$(CC) $(CFLAGS) hex2bin.c -o hex2bin
	$(CC) $(CFLAGS) rot13.c -o rot13

clean:
	rm -f hex2bin rot13
