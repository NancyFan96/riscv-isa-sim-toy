#include <stdio.h>

double g = 9;

int main(){
	double a = 1.5;
	double b = 2.1;
	static double c = 3.3;
	double d;
	
	d = a+b;	
	printf("a=%lf, b=%lf, d=a+b=%lf\n", a, b, d);

	d = c*g;
	printf("c=%lf, g=%lf, d=c*g=%lf\n", c, g, d);

	

	return 0;
}

