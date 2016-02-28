#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *xorblock(char *block, int blocklen, char *key, int keylen) {
	char *result = calloc(blocklen, sizeof(char));
	int i = 0;
	
	for( i = 0;i<blocklen;i++ ) {
		result[ i ] = block[ i ] ^ key[ i % keylen ];
	}
	return result;
}

char testfraze[] = { "abcdefghijklmnopqrstuvwxyzZ" };
char testkey[] = { "key" };

int main() {
	int i = 0;
	int keylen = strlen(testkey);
	int frazelen = strlen(testfraze);
	char *xored_block = xorblock(testfraze, frazelen, testkey, keylen);
	char *unxored_block = xorblock(xored_block, frazelen, testkey, keylen);
	for(i = 0;i<frazelen;i++) {
		printf("%c", unxored_block[i]);
	}
	return 0;
}
