#include <stdio.h>

float g = 9;

int main(){
	float a = 1.5;
	float b = 2.1;
	static float c = 3.3;
	float d;
	
	d = a+b;	
	printf("a=%f, b=%f, d=a+b=%f\n", a, b, d);

	d = c*g;
	printf("c=%f, g=%f, d=c*g=%f\n", c, g, d);

	

	return 0;
}

