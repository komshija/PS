#include <stdio.h>
#include <stdlib.h>
#include <mpi/mpi.h>
#define M 5
#define N 6

int main(int argc, char *argv[])
{
    int rank, size, colNum;
    int A[M][N], B[N], C[N],temp[N];
    int *colBuf, *vecBuf;
    MPI_Datatype colType, vecType;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    colNum = N / size;
    colBuf = (int *)malloc(sizeof(int) * M * colNum);
    vecBuf = (int *)malloc(sizeof(int) * colNum);
    MPI_Type_vector(M, 1, N, MPI_INT, &colType);
    MPI_Type_create_resized(colType, 0, sizeof(int) * size, &colType);
    MPI_Type_vector(colNum, 1, 1, colType, &colType);
    MPI_Type_create_resized(colType, 0, sizeof(int), &colType);
    MPI_Type_commit(&colType);

    MPI_Type_vector(colNum, 1, size, MPI_INT, &vecType);
    MPI_Type_create_resized(vecType, 0, sizeof(int), &vecType);
    MPI_Type_commit(&vecType);

    if (rank == 0)
    {
        srand(0);
        int k = 0;
        for (int i = 0; i < M; i++)
        {
            for (int j = 0; j < N; j++)
            {
                A[i][j] = rand() % 10;
                printf("%d ", A[i][j]);
                if (i == 0)
                {
                    B[j] = rand() % 10;
                }
            }
            printf("\n");
        }
        printf("\n");
        printf("\n");

        for (int j = 0; j < N; j++)
            printf("%d ", B[j]);

        printf("\n");
        printf("\n");
    }

    MPI_Scatter(&A[0][0], 1, colType, colBuf, M * colNum, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(&B[0], 1, vecType, vecBuf, colNum, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < M; i++)
    {
        temp[i] = 0;
        for (int j = 0; j < colNum; j++)
        {
            temp[i] += vecBuf[j] * colBuf[i + M*j];
            
        }
    }

    MPI_Reduce(&temp[0],&C[0],N,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);

    if (rank == 0)
    {

        for (int j = 0; j < N; j++)
            printf("%d ", C[j]);
        printf("\n");
    }

    free(colBuf);
    free(vecBuf);

    MPI_Finalize();
}