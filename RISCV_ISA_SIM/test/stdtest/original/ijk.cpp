#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define CHECK_TIME
//#define DEBUG

#ifdef CHECK_TIME
clock_t Start_Time;
clock_t End_Time;
#endif

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
	int N;
	int **A, **B, **C;
	scanf("%d", &N);
	N = sqrt(N/2);
	A = new int*[N];
	B = new int*[N];
	C = new int*[N];

	for(int i = 0; i < N; i++){
		A[i] = new int[N];
		for(int j = 0; j < N; j++)
			scanf("%d", &A[i][j]);
	}
	for(int i = 0; i < N; i++){
		B[i] = new int[N];
		for(int j = 0; j < N; j++)
			scanf("%d", &B[i][j]);
	}
	for(int i = 0; i < N; i++){
		C[i] = new int[N];
	}

#ifdef CHECK_TIME
    Start_Time = clock();
	ijk(A, B, C, N, N, N);
    End_Time = clock();
    printf("Duration(s): %lf\n", (double)(End_Time - Start_Time)/CLOCKS_PER_SEC);    

#endif
#ifndef CHECK_TIME
	ijk(A, B, C, N, N, N);
#endif

#ifdef DEBUG
	for(int i = 0; i < m; i++){
		for(int j = 0; j < t; j++)
			printf("%d ", C[i][j])
		printf("\n");
	}
#endif

	return 0;
}
