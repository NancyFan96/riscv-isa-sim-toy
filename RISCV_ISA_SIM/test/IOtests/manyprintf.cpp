#include <stdio.h>

int main()
{
		int a = 3;
		short int b = 5;
		long int c = 0x123456780;
		int a1 = 4;
		int a2 = 7;
		int a3 = 9;
		int a4 = 11;
		int a5 = 12;

		printf("%d, %hd, %lx\n, %d, %d, %d, %d, %d", a, b, c, a1, a2, a3, a4, a5);

		return 0;
}
