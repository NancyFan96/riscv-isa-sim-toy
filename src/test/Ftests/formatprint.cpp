#include <stdio.h>
int main()
{
	int number = 10000;
	float fnumber = (float)number;

	long longnum = 0xf;
	float flongnum = (float)longnum;

	printf("convert from int to float: number = %d, fnumber = %6.1f\n", number, fnumber);
	printf("convert from long to float: longnum = %ld, flongnum = %6.1f\n", longnum, flongnum);


	return 0;
}
