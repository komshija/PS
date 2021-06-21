#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdio.h>
#define N 50
#define threads 6 
// radi za 6, za 7 threads ne radi


__global__ void kernel(int* A, int* B, int n)
{
	extern __shared__ int sh[]; // velicina (threads + 2)^2
	int res;
	int col = threadIdx.x + blockIdx.x * blockDim.x;
	int row = threadIdx.y + blockIdx.y * blockDim.y;

	int indA = col + row * n;
	int indB = col + row * (n - 2);

	int red = threads + 2;
	int threadInd = threadIdx.x + threadIdx.y * red;

	if (col >= n && row >= n) return;

	sh[threadInd] = A[indA];
	//desno dva
	if (threadIdx.x == blockDim.x - 1 && blockIdx.x != gridDim.x - 1)
	{
		sh[threadInd + 1] = A[indA + 1];
		sh[threadInd + 2] = A[indA + 2];
	}
	if (threadIdx.y == blockDim.y - 1 && blockIdx.y != gridDim.y - 1)
	{
		sh[threadInd + red] = A[indA + n];
		sh[threadInd + red * 2] = A[indA + 2 * n];
	}
	if (threadIdx.x == blockDim.x - 1 && threadIdx.y == blockDim.y - 1
		&& blockIdx.x != gridDim.x - 1 && blockIdx.y != gridDim.y - 1)
	{
		sh[threadInd + red + 1] = A[indA + n + 1];
		sh[threadInd + red + 2] = A[indA + n + 2];
		sh[threadInd + 1 + red * 2] = A[indA + 1 + 2 * n];
		sh[threadInd + 2 + red * 2] = A[indA + 2 + 2 * n];
	}

	__syncthreads();

	if (row < n - 2 && col < n - 2) {

		res = sh[threadInd] +
			sh[threadInd + 1] + sh[threadInd + 2] +
			sh[threadInd + red] + sh[threadInd + red * 2]
			+ sh[threadInd + red + 1] + sh[threadInd + red + 2]
			+ sh[threadInd + 1 + 2 * red] + sh[threadInd + 2 + 2 * red];
		res /= 9.f;

		B[indB] = res;
	}



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
	printmat(A, N , N);

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