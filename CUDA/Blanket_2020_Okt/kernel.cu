#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdio.h>
#define N 40

__global__ void kernel(int* A, int* X, int* B, int num)
{
	__shared__ int sum[32];
	int proizvod;
	int k = threadIdx.x + blockIdx.x * blockDim.x;
	int i = threadIdx.y + blockIdx.y * blockDim.y;
	int indA = i * num + k;
	sum[threadIdx.y] = 0;

	__syncthreads();
	if (i < num && k < num)
		proizvod = A[indA] * X[k];
	else
		proizvod = 0;

	atomicAdd(&sum[threadIdx.y], proizvod);

	__syncthreads();

	if (threadIdx.x == 0)
		atomicAdd(&B[i],sum[threadIdx.y]);

}

void Hostkenrel(int* A, int* X, int* B)
{
	for (int i = 0; i < N; i++)
	{
		B[i] = 0;
		for (int k = 0; k < N; k++)
		{
			B[i] += A[i * N + k] * X[k];
		}
	}
}

void PrintVec(int* vec) {
	for (int i = 0; i < N; i++)
		printf("%d ", vec[i]);
	printf("\n");
}

int main()
{
	int A[N * N], X[N], B[N], Bh[N];
	int* Ad, * Xd, * Bd;
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			A[i * N + j] = j + j;
			printf("%d ", A[i * N + j]);
		}
		X[i] = i;
		B[i] = 0;
		printf("\n");
	}
	printf("\n");

	PrintVec(X);

	cudaMalloc((void**)&Ad, sizeof(int) * N * N);
	cudaMalloc((void**)&Xd, sizeof(int) * N);
	cudaMalloc((void**)&Bd, sizeof(int) * N);

	cudaMemcpy(Ad, A, sizeof(int) * N * N, cudaMemcpyHostToDevice);
	cudaMemcpy(Xd, X, sizeof(int) * N, cudaMemcpyHostToDevice);
	cudaMemcpy(Bd, B, sizeof(int) * N, cudaMemcpyHostToDevice);

	dim3 gridSize((N + 31) / 32, (N + 31) / 32);
	dim3 blockSize(32, 32);

	kernel << <gridSize, blockSize >> > (Ad, Xd, Bd, N);

	cudaMemcpy(B, Bd, sizeof(int) * N, cudaMemcpyDeviceToHost);
	Hostkenrel(A, X, Bh);

	PrintVec(B);
	printf("\n");
	PrintVec(Bh);

	cudaFree(Ad);
	cudaFree(Bd);
	cudaFree(Xd);

	return 0;
}