#include <stdio.h>
#include <stdlib.h>
#include <mpi/mpi.h>
#define N 9
#define k 3

int main(int argc, char *argv[])
{
    int A[N][N], B[N][N], C[N][N], vrste[k][N], kolone[N][k], temp[N][N];
    int rank, size;
    MPI_Datatype blockCols, blockRows;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Type_vector(k * N, 1, 1, MPI_INT, &blockRows);
    MPI_Type_commit(&blockRows);

    MPI_Type_vector(N, k, N, MPI_INT, &blockCols);
    MPI_Type_commit(&blockCols);

    if (rank == 0) // master
    {
        srand(0);
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                A[i][j] = rand() % 10;
                B[i][j] = rand() % 10;
                temp[i][j] = 0;
            }
        }
        printf("----- A -----\n");
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                printf("%d ", A[i][j]);
            }
            printf("\n");
        }

        printf("----- B -----\n");
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                printf("%d ", B[i][j]);
            }
            printf("\n");
        }

        for (int i = 0; i < (N / k); i++)
        {
            for (int j = 0; j < (N / k); j++)
            {
                MPI_Send(&A[i * k][0], 1, blockRows, i * (N / k) + j + 1, 0, MPI_COMM_WORLD);
                MPI_Send(&B[0][j * k], 1, blockCols, i * (N / k) + j + 1, 0, MPI_COMM_WORLD);
            }
        }
    }
    else // workers
    {
        // provereno i tacno
        MPI_Recv(&vrste[0][0], N * k, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&kolone[0][0], N * k, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                temp[i][j] = 0;

        for (int i = 0; i < k; i++)
            for (int j = 0; j < k; j++)
                for (int p = 0; p < N; p++)
                    temp[((rank - 1) / k) * k + i][((rank - 1) % k) * k + j] += vrste[i][p] * kolone[p][j];
        
        
    }

    MPI_Reduce(&temp[0][0], &C[0][0], N * N, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("----- C -----\n");
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
                printf("%d ", C[i][j]);
            printf("\n");
        }
    }

    MPI_Finalize();
}