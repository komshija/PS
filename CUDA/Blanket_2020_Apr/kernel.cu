#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdio.h>
#include <stdlib.h>
#define N 50
#define M 50
#define threads 32

__global__ void kernel(int* mat, int* k, int* kmat)
{
	__shared__ int sh[(threads + 1) * (threads + 1)];
	__shared__ int ksh[9];
	__shared__ int ksum;
	ksum = 0;
	int res;
	int col = threadIdx.x + blockIdx.x * blockDim.x;
	int row = threadIdx.y + blockIdx.y * blockDim.y;
	int index = col + row * M;
	int threadIndex = threadIdx.x + threadIdx.y * (blockDim.x + 1);

	if (blockIdx.x > 0)
		threadIndex++;
	if (blockIdx.y > 0)
		threadIndex += blockDim.x + 1;

	if (threadIdx.x < 3 && threadIdx.y < 3)
	{
		ksh[threadIdx.x + threadIdx.y * 3] = k[threadIdx.x + threadIdx.y * 3];
		atomicAdd(&ksum, ksh[threadIdx.x + threadIdx.y * 3]);
	}

	if (row >= N || col >= M) return;

	sh[threadIndex] = mat[index];

	//desno
	if (threadIdx.x == blockDim.x - 1 && blockIdx.x != gridDim.x - 1)
		sh[threadIndex + 1] = mat[index + 1];
	//dole
	if (threadIdx.y == blockDim.y - 1 && blockIdx.y != gridDim.y - 1)
		sh[threadIndex + blockDim.x + 1] = mat[index + M];
	//gore
	if (blockIdx.y > 0 && threadIdx.y == 0)
		sh[threadIndex - blockDim.x - 1] = mat[index - M];
	//levo
	if (blockIdx.x > 0 && threadIdx.x == 0)
		sh[threadIndex - 1] = mat[index - 1];

	//gore levo
	if (blockIdx.y > 0 && threadIdx.y == 0 && blockIdx.x != gridDim.x - 1 && blockIdx.x > 0 && threadIdx.x == 0)
		sh[threadIndex - 1 - blockDim.x - 1] = mat[index - 1 - M];
	//dole desno
	if (threadIdx.x == blockDim.x - 1 && blockIdx.y != gridDim.y - 1 && threadIdx.y == blockDim.y - 1)
		sh[threadIndex + blockDim.x + 2] = mat[index + M + 1];
	//gore desno
	if (threadIdx.x == blockDim.x - 1 && blockIdx.x != gridDim.x - 1 && blockIdx.y > 0 && threadIdx.y == 0)
		sh[threadIndex - blockDim.x] = mat[index + 1 - M];
	//dole levo
	if (threadIdx.y == blockDim.y - 1 && blockIdx.y != gridDim.y - 1 && blockIdx.x > 0 && threadIdx.x == 0)
		sh[threadIndex + blockDim.x] = mat[index + M - 1];

	__syncthreads();

	if (index % M == 0 || index < M || (index + 1) % M == 0 || index >= (N - 1) * M)
		res = sh[threadIndex];
	else {
		res = 0;
		int startIndex = threadIndex - 1 - (blockDim.x + 1);
		for (int i = 0; i < 3; i++) {
			for (int k = 0; k < 3; k++) {
				res += sh[startIndex + k] * ksh[i * 3 + k];
			}
			startIndex += blockDim.x + 1;
		}
		res /= (float)ksum;
	}

	kmat[index] = res;
}

void hostkernel(int* A, int* K, int* AR)
{
	int ksum = 0;
	for (int i = 0; i < 9; i++)
		ksum += K[i];

	for (int i = 0; i < N; i++) {
		for (int k = 0; k < M; k++) {
			if (i == 0 || k == 0 || i == N - 1 || k == M - 1)
				AR[i * M + k] = A[i * M + k];
			else {
				AR[i * M + k] = 0;
				for (int p = -1; p < 2; p++)
					for (int q = -1; q < 2; q++)
						AR[i * M + k] += A[(i + p) * M + (k + q)] * K[(1 + p) * 3 + (1 + q)];
				AR[i * M + k] /= ksum;
			}
		}
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

bool compare(int* A, int* B, int max)
{
	for (int i = 0; i < max; i++)
	{
		if (A[i] != B[i])
			return false;
	}
	return true;
}

int main()
{
	//Za identicno resenje kao na blanketu pozvati kernel za tri matrice tj R, G i B
	int A[N * M], K[9], AR[N * M], ARh[N * M];
	int* Ad, * Kd, * ARd;

	cudaMalloc((void**)&Ad, sizeof(int) * N * M);
	cudaMalloc((void**)&ARd, sizeof(int) * N * M);
	cudaMalloc((void**)&Kd, sizeof(int) * 9);

	for (int i = 0; i < N; i++)
		for (int j = 0; j < M; j++)
			A[i * M + j] = rand() % 15;
	for (int i = 0; i < 9; i++)
		K[i] = rand() % 10;

	printf("===== A =====\n");
	printmat(A, N, M);
	/*
	printf("===== K =====\n");
	printmat(K,3,3);
	printf("===== = =====\n\n");*/

	cudaMemcpy(Ad, A, sizeof(int) * N * M, cudaMemcpyHostToDevice);
	cudaMemcpy(Kd, K, sizeof(int) * 9, cudaMemcpyHostToDevice);

	dim3 gridSize((N + threads - 1) / threads, (M + threads - 1) / threads);
	dim3 blockSize(threads, threads);

	kernel << <gridSize, blockSize >> > (Ad, Kd, ARd);

	cudaMemcpy(AR, ARd, sizeof(int) * N * M, cudaMemcpyDeviceToHost);

	hostkernel(A, K, ARh);

	printf("===== AR =====\n");
	printmat(AR, N, M);
	printf("===== ARh =====\n");
	printmat(ARh, N, M);
	printf("===== = =====\n");

	bool res = compare(AR, ARh, N * M);
	if (res)
		printf("Isti");

	cudaFree(Ad);
	cudaFree(Kd);
	cudaFree(ARd);

	return 0;
}
