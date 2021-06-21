#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N 50
#define THREADS 32

__global__ void kernel(int* A, int* B)
{
	__shared__ int sh[(THREADS + 2) * (THREADS + 2)];
	int res,minimum,maximum;
	int row = threadIdx.y + blockDim.y * blockIdx.y;
	int col = threadIdx.x + blockDim.x * blockIdx.x;
	int indexA = row * (N + 2) + col;
	int indexB = (row-1) * N + (col-1);

	int threadIndex = threadIdx.x + threadIdx.y * (THREADS + 2) + (THREADS + 2) + 1;
	if (row >= N + 2 || col >= N + 2) return;

	// ucitavanje podataka
	sh[threadIndex] = A[indexA];

	// desno
	if (threadIdx.x == blockDim.x - 1 && col != (N+1) ) 
		sh[threadIndex + 1] = A[indexA + 1];

	// dole
	if (threadIdx.y == blockDim.y - 1 && row != (N+1)) 
		sh[threadIndex + (THREADS + 2)] = A[indexA + N + 2]; 

	// levo
	if (col != 0 && threadIdx.x == 0 && blockIdx.x > 0)  
		sh[threadIndex - 1] = A[indexA - 1]; 

	// gore
	if (blockIdx.y > 0 && threadIdx.y == 0 && indexA % (N+2) != 0 )
		sh[threadIndex - (THREADS + 2)] = A[indexA - N - 2];
	

	__syncthreads();
	if (col < 1 || row < 1 || col > N || row > N) return;
	//izracunavanje

	minimum = min(sh[threadIndex - 1], sh[threadIndex + 1]);
	maximum = max(sh[threadIndex + (THREADS+2)], sh[threadIndex - (THREADS+2)]);
	res = maximum - minimum;

	B[indexB] = res;

}


void HostKernel(int* A, int* B)
{
	int k = 0;
	for (int i = 1; i < N + 1 ; i++) {
		for (int j = 1; j < N + 1; j++) {
			int max1 = A[(i - 1) * (N + 2) + j];
			int max2 = A[(i + 1) * (N + 2) + j];
			int min1 = A[i * (N + 2) + j - 1];
			int min2 = A[i * (N + 2) + j + 1];

			B[k] = max(max1, max2) - min(min1,min2);
			k++;
		}
	}
}

bool compare(int* A, int* B) {
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			if (A[i * N + j] != B[i * N + j]) return false;
	return true;
}

int main()
{
	int A[(N + 2) * (N + 2)], B[N * N], Bh[N * N];
	int* Ad, * Bd;

	cudaMalloc((void**)&Ad, sizeof(int) * (N + 2) * (N + 2));
	cudaMalloc((void**)&Bd, sizeof(int) * N * N);

	for (int i = 0; i < N + 2; i++) {
		for (int j = 0; j < N + 2; j++) {
			A[i * (N+2) + j] = rand() % 10;
			printf("%d ", A[i * (N+2) + j]);
		}
		printf("\n");
	}

	cudaMemcpy(Ad, A, sizeof(int) * (N + 2) * (N + 2), cudaMemcpyHostToDevice);

	dim3 gridSize(((N+2) + THREADS - 1) / THREADS, ((N + 2) + THREADS - 1) / THREADS);
	dim3 blockSize(THREADS, THREADS);

	kernel << <gridSize, blockSize >> > (Ad, Bd);

	cudaMemcpy(B, Bd, sizeof(int) * N * N, cudaMemcpyDeviceToHost);
	HostKernel(A, Bh);

	printf("HOST\n");
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++)
			printf("%d ", Bh[i * N + j]);
		printf("\n");
	}

	printf("DEVICE\n");
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++)
			printf("%d ", B[i * N + j]);
		printf("\n");
	}


	if (compare(B, Bh))
		printf("isti\n");
	else
		printf("razliciti\n");


	cudaFree(Ad);
	cudaFree(Bd);

	return 0;
}
