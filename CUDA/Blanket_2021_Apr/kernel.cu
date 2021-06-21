#include <stdio.h>
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdlib.h>

#define N 7
#define threads 2

__global__ void avgKernel(int* A, int* B)
{
	__shared__ int sh[(threads + 2) * (threads + 2)];
	int row = threadIdx.y + blockIdx.y * blockDim.y;
	int col = threadIdx.x + blockIdx.x * blockDim.x;
	int indexA = row * N + col;
	int indexB = row * (N - 2) + col;
	int red = threads + 2;
	int indexThread = threadIdx.x + threadIdx.y * red;
	int sum = 0;
	if (row >= N || col >= N) return;


	sh[indexThread] = A[indexA];
	if (threadIdx.x == blockDim.x - 1 && blockIdx.x != gridDim.x - 1)
	{
		sh[indexThread + 1] = A[indexA + 1];
		sh[indexThread + 2] = A[indexA + 2];
	}
	if (threadIdx.y == blockDim.y - 1 && blockIdx.y != gridDim.y - 1)
	{
		sh[indexThread + red] = A[indexA + N];
		sh[indexThread + 2 * red] = A[indexA + 2 * N];
	}
	if (threadIdx.x == blockDim.x - 1 && blockIdx.x != gridDim.x - 1
		&& threadIdx.y == blockDim.y - 1 && blockIdx.y != gridDim.y - 1)
	{
		sh[indexThread + 1 + red] = A[indexA + 1 + N];
		sh[indexThread + 2 + red] = A[indexA + 2 + N];
		sh[indexThread + 1 + 2 * red] = A[indexA + 1 + 2 * N];
		sh[indexThread + 2 + 2 * red] = A[indexA + 2 + 2 * N];
	}

	__syncthreads();

	if (col >= N - 2 || row >= N - 2) return;

	sum = sh[indexThread] + sh[indexThread + 2] + sh[indexThread + 2 * red] + sh[indexThread + 2 * red + 2];
	sum /= 4.f;
	B[indexB] = sum;

}

void avgHost(int* A, int* B)
{
	for (int i = 0; i < N - 2; i++)
		for (int j = 0; j < N - 2; j++)
			B[i * (N - 2) + j] = (A[i * N + j] + A[i * N + j + 2] + A[i * N + j + 2*N] + A[i * N + 2 + j + 2 * N]) / 4.f;
}

void Print(int* mat, int n, int m)
{
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; j++)
		{
			printf("%d ", mat[i * m + j]);
		}
		printf("\n");
	}
}

int main()
{
	int A[N * N], B[(N - 2) * (N - 2)], Bh[(N - 2) * (N - 2)];
	int* Ad, * Bd;

	srand(0);
	for (int i = 0; i < N * N; i++)
		A[i] = i;
	printf("A\n");
	Print(A, N, N );

	cudaMalloc((void**)&Ad, sizeof(int) * N * N);
	cudaMalloc((void**)&Bd, sizeof(int) * (N - 2) * (N - 2));
	cudaMemcpy(Ad, A, sizeof(int) * N * N, cudaMemcpyHostToDevice);

	dim3 gridSize((N - 1 + threads) / threads, (N - 1 + threads) / threads);
	dim3 blockSize(threads, threads);

	avgKernel << <gridSize, blockSize >> > (Ad, Bd);

	cudaMemcpy(B, Bd, sizeof(int) * (N - 2) * (N - 2), cudaMemcpyDeviceToHost);
	avgHost(A, Bh);

	printf("HOST\n");
	Print(Bh, N - 2, N - 2);
	printf("DEVICE\n");
	Print(B, N - 2, N - 2);

	cudaFree(Ad);
	cudaFree(Bd);

	return 0;
}