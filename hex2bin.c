#include <stdlib.h>
#include <stdio.h>

static char *hex2bin(char *hexstring, int len8) {
	int resultlen8 = len8 / 2;
	char *result = (char *) calloc(resultlen8, sizeof(char));
	if( result == NULL ) {
	    return NULL;
	}
	char *src = hexstring;
	char *dst = result;
	for( int i = 0;i<resultlen8;i++) {
		int tmp_char = 0;
		sscanf(src, "%2x",&tmp_char);
		*dst = tmp_char;
		dst++;
		src+=2;
	}
	return result;
}

int main() {
	#define MAX_BLOCK_LEN8 1024
	char read_buf[ MAX_BLOCK_LEN8 ];
	int readed = 0;
	for(;;) {
		readed = fread(&read_buf[0], sizeof(char), MAX_BLOCK_LEN8, stdin);
		if(readed < 1) {
		    break;
		}
		char *binstring = hex2bin(read_buf, readed);
		fwrite(binstring, sizeof(char), readed / 2, stdout);
		free(binstring);
	}
	return 0;
}
