#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdio.h>
#define N 100
#define THREADS 32

__global__ void scalaradd(int* A, int* B, int* C)
{
    __shared__ int a[THREADS];
    __shared__ int b[THREADS];
    __shared__ int sum;
    sum = 0;
    int proizvod;
    int ind = threadIdx.x + blockDim.x * blockIdx.x;

    if (ind < N) {
        a[threadIdx.x] = A[ind];
        b[threadIdx.x] = B[ind];
    }

    if (ind < N)
        proizvod = a[threadIdx.x] * b[threadIdx.x];
    else
        proizvod = 0;

    atomicAdd(&sum, proizvod);
    __syncthreads();

    if (threadIdx.x == 0)
        atomicAdd(C, sum);

}


int skalar(int* A, int* B) {
    int sum = 0;
    for (int i = 0; i < N; i++) {
        sum += A[i] * B[i];
    }
    return sum;
}

int main()
{
    int A[N], B[N], C, Ch;
    int* Ad, * Bd, * Cd;
    int gridDim = (N + THREADS - 1) / THREADS;
    C = 0;
    cudaMalloc((void**)&Ad, sizeof(int) * N);
    cudaMalloc((void**)&Bd, sizeof(int) * N);
    cudaMalloc((void**)&Cd, sizeof(int));

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N / 5; j++) {
            A[i] = (j * i);
            B[i] = (j * i);
        }
        A[i] /= i % 4 == 0 ? 2 : 5;
        B[i] /= 2;
        B[i] *= i % 4 == 0 ? -1 : 2;
    }


    cudaMemcpy(Ad, A, sizeof(int) * N, cudaMemcpyHostToDevice);
    cudaMemcpy(Bd, B, sizeof(int) * N, cudaMemcpyHostToDevice);
    cudaMemcpy(Cd, &C, sizeof(int), cudaMemcpyHostToDevice);

    dim3 gridSize(gridDim);
    dim3 blockSize(THREADS);
    int memSize = 100;
    scalaradd << <gridSize, blockSize ,memSize >> > (Ad, Bd, Cd);

    cudaMemcpy(&C, Cd, sizeof(int), cudaMemcpyDeviceToHost);
    Ch = skalar(A, B);

    if (C == Ch) {

        printf("Isti rezultat. device = %d host = %d\n",C, Ch);
    }
    else {
        printf("Nije isti rezultat. device = %d host = %d\n",C, Ch);
    }
    
    cudaFree(Ad);
    cudaFree(Bd);
    cudaFree(Cd);

    return 0;
}