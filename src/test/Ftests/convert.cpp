#include <stdio.h>
int main()
{
	int number = 10000;
	float fnumber = (float)number;

	long longnum = 0xf;
	float flongnum = (float)longnum;

	printf("convert from int to float: number = %d, fnumber = %f\n", number, fnumber);
	printf("convert from long to float: longnum = %ld, flongnum = %f\n", longnum, flongnum);


	return 0;
}
