#include <stdio.h>
#include <stdlib.h>
//#include <time.h>
#include <math.h>

//#define DEBUG


void ijk(int **A, int **B, int **C, int m, int n, int t){
	for(int i = 0; i < m; i++){
		for(int j = 0; j < t; j++){
			int tmp = 0;
			for(int k = 0; k < n; k++){
				tmp += A[i][k]*B[k][j];
			}
			C[i][j] = tmp;
		}
	}
}

int main()
{
	int N = 16;

	int **A, **B, **C;
	N = sqrt(N/2);
	A = new int*[N];
	B = new int*[N];
	C = new int*[N];

	for(int i = 0; i < N; i++){
		A[i] = new int[N];
		for(int j = 0; j < N; j++)
			//scanf("%d", &A[i][j]);
			A[i][j] = i+j;
	}
	for(int i = 0; i < N; i++){
		B[i] = new int[N];
		for(int j = 0; j < N; j++)
			//scanf("%d", &B[i][j]);
			B[i][j] = i-j;
	}
	for(int i = 0; i < N; i++){
		C[i] = new int[N];
	}


	ijk(A, B, C, N, N, N);

//#ifdef DEBUG
	/*
	for(int i = 0; i < m; i++){
		for(int j = 0; j < t; j++)
			printf("%d ", C[i][j])
		printf("\n");
	}
	*/
//#endif

	return 0;
}
