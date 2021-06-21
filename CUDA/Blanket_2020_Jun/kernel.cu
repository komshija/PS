#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdio.h>
#include <stdlib.h>
#define N 250
#define threads 32

__global__ void kernel(int *mat, int *vec)
{
    //__shared__ int sh[threads*threads];
    int value;
    __shared__ int Qsh[threads], Psh[threads];
    int P = 0, Q = 0;
    int row = threadIdx.y + blockIdx.y * blockDim.y;
    int col = threadIdx.x + blockIdx.x * blockDim.x;
    int indexMat = col + row * N;
    int threadIndex = threadIdx.x + threadIdx.y * blockDim.x;

    if (col >= N || row >= N) return;

    Qsh[threadIdx.y] = 0;
    Psh[threadIdx.y] = 0;

    value = mat[indexMat];
    if (value > 0) {
        P = 1;
    } else {
        Q = 1;
    }

    atomicAdd(&Psh[threadIdx.y], P);
    atomicAdd(&Qsh[threadIdx.y], Q);

    __syncthreads();

    if (threadIdx.x == 0)
        atomicAdd(&vec[row], Psh[threadIdx.y]);
   
    __syncthreads();
   
    if (threadIdx.x == 0)
        atomicSub(&vec[row], Qsh[threadIdx.y]);

    __syncthreads();

    if (value < 0) {
        value = vec[row];
        mat[indexMat] = value;
    }

}

void PrintMat(int* mat,int n,int m) {
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
            printf("%d ", mat[i * N + j]);

        printf("\n");
    }
}

void hostkernel(int* A,int* B)
{
    for(int i = 0; i < N; i++) {
        int pos = 0, neg = 0;
        for (int j = 0; j < N; j++)
            if (A[i * N + j] > 0) pos++; else neg++;
        
        B[i] = pos - neg;

        for (int j = 0; j < N; j++)
            if (A[i * N + j] < 0) A[i * N + j] = B[i];
    }
}

void compare(int* A, int* B)
{
    for (int i = 0; i < N; i++)
        if (A[i] != B[i])
            printf("nisu isti");
}

int main()
{
    int A[N * N], B[N], Bh[N],A2[N*N];
    int* Ad, * Bd;

    cudaMalloc((void**)&Ad, sizeof(int) * N * N);
    cudaMalloc((void**)&Bd, sizeof(int) * N);
    
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            int val = rand() % 10;
            A[i * N + j] = val % 3 == 0 ? -val : val;
        }
    for (int i = 0; i < N; i++)
        B[i] = 0;

    printf("===== A =====\n");
    //PrintMat(A, N, N);
    printf("=====  =====\n");

    cudaMemcpy(Ad, A, sizeof(int) * N * N, cudaMemcpyHostToDevice);
    cudaMemcpy(Bd, B, sizeof(int) * N, cudaMemcpyHostToDevice);
    
    dim3 gridSize((N + threads - 1) / threads, (N + threads - 1) / threads);
    dim3 blockSize(threads, threads);

    kernel << <gridSize, blockSize >> > (Ad, Bd);

    cudaMemcpy(A2, Ad, sizeof(int) * N * N, cudaMemcpyDeviceToHost);
    cudaMemcpy(B, Bd, sizeof(int) * N, cudaMemcpyDeviceToHost);
    hostkernel(A, Bh);


   /* printf("===== DEVICE =====\n");
    PrintMat(B,1,N);
    printf("=====  =====\n");
    PrintMat(A2, N, N);
    printf("===== HOST =====\n");
    PrintMat(Bh, 1, N);
    printf("=====  =====\n");
    PrintMat(A, N, N);*/

    compare(B, Bh);
    
    return 0;
}