#include <stdio.h>

int main()
{
	int a = 3;
	short int b = 5;
	long int c = 0x123456780;

	printf("%d, %hd, %lx\n", a, b, c);

	return 0;
}

