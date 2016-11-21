#include <stdio.h>
#include <stdlib.h>
#include <math.h>




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
	int m = 0,n = 0,t = 0;
	scanf("%d", &m);
	scanf("%d", &n);
	scanf("%d", &t);
	int **A, **B, **C;
	A = new int*[m];
	B = new int*[n];
	C = new int*[m];

	for(int i = 0; i < m; i++){
		A[i] = new int[n];
		for(int j = 0; j < n; j++)
			scanf("%d", &A[i][j]);
	}
	for(int i = 0; i < n; i++){
		B[i] = new int[t];
		for(int j = 0; j < t; j++)
			scanf("%d", &B[i][j]);
	}
	for(int i = 0; i < m; i++){
		C[i] = new int[m];
	}


	ijk(A, B, C, m, n, t);



	for(int i = 0; i < m; i++){
		for(int j = 0; j < t; j++)
			printf("%d ", C[i][j]);
		printf("\n");
	}

	return 0;
}
