#include <stdio.h>
#include <mpi/mpi.h>
#include <stdlib.h>
#include <math.h>

#define N 8

int main(int argc, char *argv[])
{
    struct
    {
        int value;
        int rank;
    } maxlocal, maxglobal;

    int A[N][N], B[N], C[N], temp[N];
    int rank, size, maxrank;
    MPI_Datatype matType, vecType;
    MPI_Comm comCol, comRow;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int q = (int)sqrt(size);
    const int w = N / q;
    int mat[w][w], vec[w];

    MPI_Type_vector(w, 1, q, MPI_INT, &matType);
    MPI_Type_create_resized(matType, 0, sizeof(int) * N, &matType);
    MPI_Type_vector(w, 1, q, matType, &matType);
    MPI_Type_create_resized(matType, 0, sizeof(int), &matType);
    MPI_Type_commit(&matType);

    MPI_Type_vector(w, 1, q, MPI_INT, &vecType);
    MPI_Type_create_resized(vecType, 0, sizeof(int), &vecType);
    MPI_Type_commit(&vecType);

    int numCol = rank % q;
    int numRow = rank / q;
    MPI_Comm_split(MPI_COMM_WORLD, numCol, numRow, &comCol);
    MPI_Comm_split(MPI_COMM_WORLD, numRow, numCol, &comRow);

    if (rank == 0)
    {
        printf("A\n");
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                A[i][j] = i * N + j;
                printf("%d ", A[i][j]);
            }
            B[i] = i;
            printf("\n");
        }
        
        printf("B\n");
        for (int i = 0; i < N; i++)
            printf("%d ", B[i]);
        printf("\n");
        
        for (int i = 0; i < size; i++)
        {
            if (i == 0)
            {
                for (int i = 0; i < w; i++)
                {
                    for (int j = 0; j < w; j++)
                    {
                        mat[i][j] = A[i * q][j * q];
                    }
                    vec[i] = B[i * q];
                }
            }
            else
            {
                MPI_Send(&A[i / q][i % q], 1, matType, i, 0, MPI_COMM_WORLD);
                // MPI_Send(&B[i % q], 1, vecType, i, 0, MPI_COMM_WORLD);
            }
        }
    }
    else
    {
        MPI_Recv(&mat[0][0], w * w, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // MPI_Recv(&vec[0], w, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    if (numRow == 0)
        MPI_Scatter(&B[0], 1, vecType, &vec[0], w, MPI_INT, 0, comRow);
    MPI_Bcast(&vec[0], w, MPI_INT, 0, comCol);

    for (int i = 0; i < N; i++)
        temp[i] = 0;

    for (int i = 0; i < w; i++)
        for (int j = 0; j < w; j++)
            temp[numRow + q * i] += mat[i][j] * vec[j];

    maxlocal.rank = rank;
    maxlocal.value = mat[0][0];
    for (int i = 0; i < w; i++)
        for (int j = 0; j < w; j++)
            if (mat[i][j] > maxlocal.value)
                maxlocal.value = mat[i][j];

    MPI_Reduce(&maxlocal, &maxglobal, 1, MPI_2INT, MPI_MAXLOC, 0, MPI_COMM_WORLD);
    MPI_Bcast(&maxglobal, 1, MPI_2INT, 0, MPI_COMM_WORLD);
    MPI_Reduce(&temp[0], &C[0], N, MPI_INT, MPI_SUM, maxglobal.rank, MPI_COMM_WORLD);

    if (maxglobal.rank == rank)
    {
        printf("Res\n");

        for (int i = 0; i < N; i++)
            printf("%d ", C[i]);
    }

    MPI_Finalize();
}