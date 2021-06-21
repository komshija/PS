#include <stdio.h>
#include <stdlib.h>
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#define N 14
#define M 7
#define threads 4

__global__ void sumMin(int* A, int* pom, int* min)
{
	__shared__ int locmin;
	__shared__ int sh[threads + M - 1];
	int sum = 0;
	int index = threadIdx.x + blockDim.x * blockIdx.x;

	if (index >= N) return;
	sh[threadIdx.x] = A[index];

	if (index >= N - M + 1) return;

	if (blockDim.x - 1 == threadIdx.x)
		for (int i = 1; i < M; i++)
			sh[threadIdx.x + i] = A[index + i];
	
	__syncthreads();
	for (int i = 0; i < M; i++)
		sum += sh[threadIdx.x + i];
	pom[index] = sum;

	if (threadIdx.x == 0 && blockIdx.x == 0)
		*min = sum;
	
	if (threadIdx.x == 0)
		locmin = sum;
	__syncthreads();

	atomicMin(&locmin, sum);
	__syncthreads();

	if (threadIdx.x == 0)
		atomicMin(min, locmin);

}

int main()
{
	int A[N], pom[N - M + 1], min;
	int* Ad, * pomd, * mind;

	printf("A\n");
	for (int i = 0; i < N; i++)
	{
		A[i] = rand() % 10;
		printf("%d ", A[i]);
	}
	printf("\n");

	cudaMalloc((void**)&Ad, sizeof(int) * N);
	cudaMalloc((void**)&pomd, sizeof(int) * (N - M + 1));
	cudaMalloc((void**)&mind, sizeof(int));

	cudaMemcpy(Ad, A, sizeof(int) * N, cudaMemcpyHostToDevice);

	sumMin << <(N - M + threads) / threads, threads >> > (Ad, pomd, mind);

	cudaMemcpy(pom, pomd, sizeof(int) * (N - M + 1), cudaMemcpyDeviceToHost);
	cudaMemcpy(&min, mind, sizeof(int), cudaMemcpyDeviceToHost);

	printf("pom\n");
	for (int i = 0; i < N - M + 1; i++)
		printf("%d ", pom[i]);
	printf("\n");
	printf("Min= %d ", min);

	return 0;
}