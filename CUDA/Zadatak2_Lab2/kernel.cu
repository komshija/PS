#include "cuda_runtime.h"
#include "cuda.h"
#include "device_launch_parameters.h"
#include "device_functions.h"
#include <stdio.h>

#define N 4
#define M 5
#define THREADS 32

__global__ void kernel(int* mat, int* max, int shsize)
{
    extern __shared__ int kolone[];
    int row = blockDim.y * blockIdx.y + threadIdx.y;
    int col = blockDim.x * blockIdx.x + threadIdx.x;
    int index = col + row * M; // row * total cols = prvi element u nekom redu, a + col pomeraj u tom redu 
    int localIndex = threadIdx.x + THREADS * threadIdx.y;
    
    if (row >= N || col >= M) return;

    //shared memoriju mapiramo na vise promenjivih
    int* colsh = kolone; // velicina shsize * shsize
    int* maxsh = &kolone[shsize*shsize]; // velicina shsize

    // ucitaju svi u svoju shared i inicijalizuje se max
    colsh[localIndex] = mat[index];
    maxsh[threadIdx.x] = 0;
    
    __syncthreads();

    // kroz atomicne operacije u shared nadje se max za svaki tile
    maxsh[threadIdx.y] = atomicMax(&maxsh[threadIdx.y], colsh[localIndex]);
    __syncthreads();


    max[blockIdx.y * (blockDim.x + gridDim.x)  + col] = maxsh[threadIdx.x];
}

void host(int *A, int* max) {

}

int main()
{
    int A[N][M], max[M], temp[N][M], maxh[M];
    int* Ad, *maxd;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            A[i][j] = i % 4 == 0 ? i * i - j : j * i;
        }
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            printf("%d ", A[i][j]);
        }
        printf("\n");
    }
    
    cudaMalloc((void**)&Ad, sizeof(int) * N * M);
    cudaMalloc((void**)&maxd, sizeof(int) * M * ((M + THREADS - 1) / THREADS));
    cudaMemcpy(Ad, A, sizeof(int) * N * M, cudaMemcpyHostToDevice);

    dim3 gridSize((N+THREADS-1)/THREADS,(M+THREADS-1)/THREADS);
    dim3 blockSize(THREADS,THREADS);
    int memSize = THREADS*(THREADS + 1);

    kernel << <gridSize, blockSize, memSize * sizeof(int) >> > (Ad, maxd, THREADS);
    
    cudaMemcpy(temp, maxd, sizeof(int) * M * ((N + THREADS - 1) / THREADS), cudaMemcpyDeviceToHost);

   // kernel << <1, blockSize, memSize * sizeof(int) >> > (maxd, maxd, THREADS);



    cudaFree(Ad);
    cudaFree(maxd);


    return 0;
}