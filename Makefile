CFLAGS=-std=c99 -Wall -Wextra

default:
	$(CC) $(CFLAGS) hex2bin.c -o hex2bin

clean:
	rm -f hex2bin
