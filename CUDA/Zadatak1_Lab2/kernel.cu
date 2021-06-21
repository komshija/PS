#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdio.h>
#define THREADS 32

__global__ void func(int* A, int* B, int N)
{
	extern __shared__ int sh[];
	int tid = threadIdx.x + blockIdx.x * blockDim.x;
	if (tid >= N) return;

	int* Ash = sh;
	int* res = &sh[N + 2];

	Ash[threadIdx.x] = A[tid];
	// Fora je u tome da ako je nit poslednja u bloku mora uzme jos 2 preko
	// Ali ako je globalni indeks niti poslednji u nizu, takodje mora da uzme jos 2 preko
	if (threadIdx.x == blockDim.x - 1 || tid == N - 1) {
		Ash[threadIdx.x + 1] = A[tid + 1];
		Ash[threadIdx.x + 2] = A[tid + 2];
	}
	__syncthreads();

	res[threadIdx.x] = (3 * Ash[threadIdx.x] + 10 * Ash[threadIdx.x + 1] + 7 * Ash[threadIdx.x + 2]) / 20.f;
	__syncthreads();

	B[tid] = res[threadIdx.x];

}
void Host_func(int* A, int* B, int N) {
	for (int i = 0; i < N; i++) {
		B[i] = (3 * A[i] + 10 * A[i + 1] + 7 * A[i + 2]) / 20.f;
	}
}

int CompareArrays(int* A, int* B,int N) {
	int res = 0;
	for (int i = 0; i < N; i++) {
		if (A[i] != B[i]) res++;
	}
	return res;
}

void Print(int* A, int N) {
	for (int i = 0; i < N; i++)
		printf("%d ", A[i]);
	printf("\n");
}

int main()
{
	int* A, * B, N, * Ad, * Bd, * Bh;
	scanf("%d", &N);
	A = new int[N + 2];
	B = new int[N];
	Bh = new int[N];
	cudaMalloc((void**)&Ad, sizeof(int) * (N+2));
	cudaMalloc((void**)&Bd, sizeof(int) * N);

	for (int i = 0; i < N+2; i++) {
		A[i] = i;
	}
	cudaMemcpy(Ad, A, sizeof(int) * (N+2), cudaMemcpyHostToDevice);

	dim3 gridSize((N + (THREADS - 1)) / THREADS);
	dim3 blockSize(THREADS);

	func << <gridSize, blockSize, (N + 2 + N) * sizeof(int) >> > (Ad, Bd, N);

	Host_func(A, Bh, N);
	cudaMemcpy(B, Bd, sizeof(int) * N, cudaMemcpyDeviceToHost);
	int diff = CompareArrays(B, Bh, N);
	printf("difference %d\n", diff);
	Print(B, N);
	Print(Bh, N);

	cudaFree(Ad);
	cudaFree(Bd);

	return 0;
}
