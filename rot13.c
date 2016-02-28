#include <stdio.h>
#include <ctype.h>

char rotchar(int c, int count, int start) {
	return (((c - start) + count) % (count * 2) + start);
}

int main() {
	int c = 0;
	while( c != EOF ) {
		c = fgetc(stdin);
		if( !isalpha(c) ) { 
			if( isprint(c) ) {
				printf("%c", c);
			}
		}
		else {
			if( isupper(c) ) {
				printf( "%c", rotchar( c, 13, 'A'));
			}
			else if(islower(c) ) {
				printf( "%c", rotchar( c, 13, 'a'));
			}
		}
	}
	return 0;
}
