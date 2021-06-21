
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdio.h>
#define N 6
#define threads 2

__global__ void kernel(int* A, int* B)
{
	const int red = threads + 2;
	__shared__ int sh[red * red];
	int col = threadIdx.x + blockIdx.x * blockDim.x;
	int row = threadIdx.y + blockIdx.y * blockDim.y;
	int index = col + row * N;
	int indexB = col + row * (N - 2);
	int threadIndex = threadIdx.x + threadIdx.y * red;
	float res = 0;

	if (col >= N || row >= N) return;

	sh[threadIndex] = A[index];
	//dva desno
	if (threadIdx.x == blockDim.x - 1 && blockIdx.x != gridDim.x - 1)
	{
		sh[threadIndex + 1] = A[index + 1];
		sh[threadIndex + 2] = A[index + 2];
	}
	if (threadIdx.y == blockDim.y - 1 && blockIdx.y != gridDim.y - 1)
	{
		sh[threadIndex + red] = A[index + N];
		sh[threadIndex + 2 * red] = A[index + 2 * N];
	}
	if (threadIdx.x == blockDim.x - 1 && threadIdx.y == blockDim.y - 1)
	{
		sh[threadIndex + red + 1] = A[index + N + 1];
		sh[threadIndex + 2 * red + 1] = A[index + 2 * N + 1];
		sh[threadIndex + red + 2] = A[index + N + 2];
		sh[threadIndex + 2 * red + 2] = A[index + 2 * N + 2];
	}

	__syncthreads();
	if (col >= N - 2 || row >= N - 2) return;

	res = sh[threadIndex]
		+ sh[threadIndex + 1] + sh[threadIndex + 2]
		+ sh[threadIndex + red] + sh[threadIndex + 2 * red]
		+ sh[threadIndex + red + 1] + sh[threadIndex + red + 2]
		+ sh[threadIndex + 2 * red + 1] + sh[threadIndex + 2 * red + 2];
	res /= 9.f;
	B[indexB] = res;

}

void hostkernel(int* A, int* B)
{
	for (int i = 0; i < N - 2; i++) {
		for (int j = 0; j < N - 2; j++) {
			B[i * (N - 2) + j] =
				A[(i)*N + (j)]
				+ A[(i + 1) * N + (j)] + A[(i + 2) * N + (j)]
				+ A[(i)*N + (j + 1)] + A[(i)*N + (j + 2)]
				+ A[(i + 1) * N + (j + 1)] + A[(i + 1) * N + (j + 2)]
				+ A[(i + 2) * N + (j + 1)] + A[(i + 2) * N + (j + 2)];
			B[i * (N - 2) + j] /= 9.f;

		}
	}
}

void PrintMat(int* B) {
	for (int i = 0; i < N - 2; i++) {
		for (int j = 0; j < N - 2; j++) {
			printf("%d ", B[i * (N - 2) + j]);
		}
		printf("\n");
	}
}

void printmat(int* A, int n, int m) {
	for (int i = 0; i < n; i++) {
		for (int k = 0; k < m; k++) {
			printf("%d ", A[i * m + k]);
		}
		printf("\n");
	}
}

int main()
{
	int A[N * N], B[(N - 2) * (N - 2)], Bh[(N - 2) * (N - 2)];
	int* Ad, * Bd;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			A[i * N + j] = 10;
		}
	}
	printmat(A, N, N);

	cudaMalloc((void**)&Ad, sizeof(int) * N * N);
	cudaMalloc((void**)&Bd, sizeof(int) * (N - 2) * (N - 2));

	cudaMemcpy(Ad, A, sizeof(int) * N * N, cudaMemcpyHostToDevice);

	dim3 gridSize((N + threads - 1) / threads, (N + threads - 1) / threads);
	dim3 blockSize(threads, threads);
	int memSize = (threads + 2) * (threads + 2);

	kernel << <gridSize, blockSize, memSize >> > (Ad, Bd, N);
	cudaMemcpy(B, Bd, sizeof(int) * (N - 2) * (N - 2), cudaMemcpyDeviceToHost);

	hostkernel(A, Bh);

	printf("\nHost\n");
	PrintMat(Bh);
	printf("\nDevice\n");
	PrintMat(B);

	cudaFree(Ad);
	cudaFree(Bd);

	return 0;
}