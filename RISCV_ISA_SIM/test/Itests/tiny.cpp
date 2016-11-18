int global = 9;

int main(){
	int a = 1;
	int b = 2;
	static int c = 3;
	a = b+99;
	a ++;
	a = a+global;
	c = b+global;

	return 0;
}

