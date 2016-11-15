#include<stdio.h>

int GG = 99;
int G;

int main()
{
	int a = 3;
	static int  b = 2;
	int c = a + b + GG;
	return 0;
}
