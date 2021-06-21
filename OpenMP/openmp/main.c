#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#define N 30

int main(int argc, char* argv[])
{
	//1. zadatak
	int i, j, k;
	int A[N][N], B[N][N], Csingle[N][N], Cparallel[N][N];;
	int maxCores = 0;
	const int chunks = 4;


	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			A[i][j] = (i + 1) * (j * j);
			B[i][j] = (i) * ((j+2) * (j+3));
		}
	}

	double start_single = omp_get_wtime();
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			Csingle[i][j] = 0;
			for (k = 0; k < N; k++)
			{
				Csingle[i][j] += A[i][k] * B[k][i];
			}
		}
	}
	double stop_single = omp_get_wtime();
	

	maxCores = omp_get_num_procs();
	printf("Max cores %d\n", maxCores);
	omp_set_num_threads(maxCores);

	double start_parallel = omp_get_wtime();
	#pragma omp parallel for schedule(dynamic,chunks)
	for (i = 0; i < N; i++)
	{
		for ( j = 0; j < N; j++) 
		{
			Cparallel[i][j] = 0;
			for ( k = 0; k < N; k++)
			{
				Cparallel[i][j] += A[i][k] * B[k][i];
			}
		}
	}
	double stop_parallel = omp_get_wtime();

	double time_single = stop_single - stop_single;
	double time_parallel = stop_parallel - stop_parallel;
	printf("Time on signle thread %lf\nTime on parallel threads %lf\n", time_single, time_parallel);

	int tacno = 1;
	for ( i = 0; i < N && tacno; i++)
		for (j = 0; j < N && tacno; j++)
			tacno = Csingle[i][j] == Cparallel[i][j];
			
		

	if (tacno == 0) {
		for (i = 0; i < N && tacno; i++)
		{
			for (j = 0; j < N && tacno; j++)
				printf("%d ", Csingle[i][j]);
			printf("\n");
		}
		
		printf("\n");
		printf("\n");

		for (i = 0; i < N && tacno; i++)
		{
			for (j = 0; j < N && tacno; j++)
				printf("%d ", Cparallel[i][j]);
			printf("\n");
		}
	}
	return 0;
}