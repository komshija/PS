#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define N 10

int main()
{
	const int chunk_size = 5;
	int A[N], B[N], C = 0, res[3];
	double start, stop, single_time, parallel_time, reduce_time;

	omp_set_num_threads(omp_get_num_procs());

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			A[i] = (i + 1) * (j * j);
			B[i] = (i) * ((j + 2) * (j + 3));
		}
	}

	start = omp_get_wtime();
	C = 0;
	for (int i = 0; i < N; i++)
		C += A[i] * B[i];
	stop = omp_get_wtime();
	single_time = stop - start;
	res[0] = C;

	start = omp_get_wtime();
	C = 0;
	#pragma omp parallel for schedule(dynamic, chunk_size)
	for (int i = 0; i < N; i++)
		C += A[i] * B[i];
	stop = omp_get_wtime();
	parallel_time = stop - start;
	res[1] = C;


	start = omp_get_wtime();
	C = 0;
	#pragma omp parallel for schedule(dynamic, chunk_size) reduce(+:C)
	for (int i = 0; i < N; i++)
		C += A[i] * B[i];
	stop = omp_get_wtime();
	reduce_time = stop - start;
	res[2] = C;

	printf("Single thread time: %.10lf ;; result = %d\n", single_time, res[0]);
	printf("Parallel thread time: %.10lf ;; result = %d\n", parallel_time, res[1]);
	printf("Parallel with reduction thread time: %.10lf ;; result = %d\n", reduce_time, res[2]);

}
