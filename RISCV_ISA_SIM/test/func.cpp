int func(int a, int b){
	return a+b;
}

int main()
{
	int a1, a2, a3, a4, a5;
	a1 = 1;
	a2 = 2;
	a3 = 3;
	a4 = 4;
 	a5 = 5;
	a1 = func(a2, a3);
	a4 = func(a1, a4);

	return 0;
}
