#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdio.h>
#define N 32
#define THREADS 4

__global__ void Expression(int* A, int x, int* B, int* C)
{
	extern __shared__ int niz[];
	__shared__ int sx;
	int tid = blockIdx.x * blockDim.x + threadIdx.x;

	if (tid >= N) return;

	sx = x;
	int* sa = niz;
	int* sb = &niz[N];
	int* sc = &niz[N];

	sa[tid] = A[tid];
	sb[tid] = B[tid];

	sc[tid] = (sa[tid] * sx) + sb[tid];

	C[tid] = sc[tid];
}


void Host_Expression(int* A, int x, int* B, int* C) {
	for (int i = 0; i < N; i++) {
		C[i] = A[i] * x + B[i];
	}
}

int CompareArrays(int* A, int* B) {
	int res = 0;
	for (int i = 0; i < N; i++) {
		if (A[i] != B[i]) res++;
	}
	return res;
}

int main()
{
	int A[N], B[N], C[N], Ch[N], x;
	int* dA, * dB, * dC;
	x = 3;
	for (int i = 0; i < N; i++)
	{
		A[i] = i * i;
		B[i] = i + 2;
	}
	cudaMalloc((void**)&dA, sizeof(int) * N);
	cudaMalloc((void**)&dB, sizeof(int) * N);
	cudaMalloc((void**)&dC, sizeof(int) * N);
	cudaMemcpy(dA, A, sizeof(int) * N, cudaMemcpyHostToDevice);
	cudaMemcpy(dB, B, sizeof(int) * N, cudaMemcpyHostToDevice);

	dim3 gridSize((N + (THREADS - 1)) / THREADS);
	dim3 blockSize(THREADS);


	Expression << < gridSize, blockSize, (3 * N) * sizeof(int) >> > (dA, x, dB, dC);
	Host_Expression(A, x, B, Ch);
	cudaMemcpy(C, dC, sizeof(int) * N, cudaMemcpyDeviceToHost);

	int razlika = CompareArrays(C, Ch);
	printf("Razlika %d\n", razlika);


	cudaFree(dA);
	cudaFree(dB);
	cudaFree(dC);

	return 0;
}
