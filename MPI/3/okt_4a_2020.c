#include <stdio.h>
#include <stdlib.h>
#include <mpi/mpi.h>
#define N 8

int main(int argc, char *argv[])
{
    int A[N][N], C[N][N];
    int rank, size;
    MPI_Datatype snd, rcv, tmp[N];

    int count = 0, blockLens[N];
    MPI_Aint displacements[N];

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            A[i][j] = i * N + j;
            C[i][j] = 0;
        }
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Type_contiguous(2, MPI_INT, &snd);
    MPI_Type_create_resized(snd, 0, sizeof(int) * 2 * N, &snd);

    MPI_Type_vector(N / 2, 1, 1, snd, &snd);
    MPI_Type_create_resized(snd, 0, sizeof(int) * 2, &snd);

    MPI_Type_vector(N / 2, 1, 1, snd, &snd);
    MPI_Type_commit(&snd);

    int temp = N * N / 2;
    for (int i = N; temp > 0; i--)
    {
        temp -= i;
        if (temp < 0)
            i += temp;

        MPI_Type_vector(i, 1, -N, MPI_INT, &tmp[count]);
        MPI_Type_create_resized(tmp[count], 0, -sizeof(int), &tmp[count]);

        blockLens[count] = 1;
        displacements[count] = sizeof(int) * -count;
        count++;
    }

    MPI_Type_struct(count, blockLens, displacements, tmp, &rcv);
    MPI_Type_commit(&rcv);

    if (rank == 0)
    {
        for (int i = 1; i < size; i++)
        {
            MPI_Send(&A[0][0], 1, snd, i, 0, MPI_COMM_WORLD);
        }
    }
    else
    {
        MPI_Recv(&C[N-1][N-1], 1, rcv, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (rank == 1)
        {
             for (int i = 0; i < N; i++)
            {
                for (int j = 0; j < N; j++)
                {
                    printf("%d ", A[i][j]);
                }
                printf("\n");
            }

            printf("\n");
            printf("\n");

            for (int i = 0; i < N; i++)
            {
                for (int j = 0; j < N; j++)
                {
                    printf("%d ", C[i][j]);
                }
                printf("\n");
            }
        }
    }

    MPI_Finalize();
}