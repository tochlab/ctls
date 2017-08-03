#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
	srand(time(NULL));   // should only be called once
	int right = 0;
	int wrong = 0;
	int count = 1;
	while(1)
	{
		int x = rand() % 100;
		int y = rand() % 100;
		int z = x + y;
		int answer = -1;
		printf("%d: %d+%d=", count, x, y);

		char answer_str[128];
		char *res = fgets(&answer_str[0], 128, stdin);

		if( res == NULL ) {
			printf("Wrong input! Exit...\n");
			return 1;
		}

		int scanf_res = sscanf(answer_str, "%d", &answer);
		if( scanf_res != 1) {
			printf("Wrong input! Exit...\n");
			return 1;
		}

		if(answer == z) {
			printf("Right!\n");
			right++;
		}
		else {
			printf("Wrong %d!\n",z);
			wrong++;
		}
		if( count % 10 == 0) {
			printf("=== Wrong %d Right %d (%0.2f %%)\n", wrong, right, ((float ) right / count) * 100.f );
		}

		count++;
	}
	return 0;
}
