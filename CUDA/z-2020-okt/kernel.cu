
#include <stdio.h>
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#define N 9
#define threads 3

__global__ void mulKernel(int* A, int* X, int* B)
{
    __shared__ int a[threads * threads];
    __shared__ int x[threads];
    __shared__ int parcial[threads];
    int res = 0;

    int row = threadIdx.y + blockIdx.y * blockDim.y;
    int col = threadIdx.x + blockIdx.x * blockDim.x;
    int iA = col + row * N;

    int tidA = threadIdx.x + threads * threadIdx.y;
    int tidX = threadIdx.y;

    if (row >= N || col >= N) return;

    a[tidA] = A[iA];
    if (threadIdx.x == 0)
        x[tidX] = B[row];

    __syncthreads();

    res = a[tidA] * x[tidX];
    atomicAdd(&parcial[tidX], res);
    
    __syncthreads();

    if (threadIdx.x == 0)
        atomicAdd(&B[col], parcial[tidX]);

}

//lab2-6
__global__ void kernel6(int* A, int* posmin)
{
    //__shared__ int sh[threads];
    __shared__ int locPosMin;
    int loc;
    int index = threadIdx.x + blockIdx.x * blockDim.x;

    loc = A[index];

    if (threadIdx.x == 0)
        locPosMin = INT_MAX;
    __syncthreads();
    
    if(loc > 0)
        atomicMin(&locPosMin, loc);
    __syncthreads();
    
    if (threadIdx.x == 0)
        atomicMin(posmin, locPosMin);
    __syncthreads();
    
    if (threadIdx.x == 0)
        locPosMin = *posmin;
    __syncthreads();

    if (loc <= 0)
        loc = locPosMin;
    A[index] = loc;
}

int main()
{
    

    return 0;
}