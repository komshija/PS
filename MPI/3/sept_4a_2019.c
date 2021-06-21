#include <stdio.h>
#include <mpi/mpi.h>
#define N 4

int main(int argc, char *argv[])
{
    int A[N][N], C[N][N];
    int rank, size;
    MPI_Datatype snd, rcv, temp[N];
    int lens[N], displ[N];
    MPI_Aint displ2[N];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    for (int i = 0; i < N; i++)
    {
        lens[i] = i + 1;
        displ[i] = i * N;
    }

    MPI_Type_indexed(N, lens, displ, MPI_INT, &snd);
    MPI_Type_commit(&snd);

    for (int i = 0; i < N; i++)
    {
        MPI_Type_vector(N - i, 1, -1, MPI_INT, &temp[i]);
        lens[i] = 1;
        displ2[i] = i * sizeof(int) * N;
    }

    MPI_Type_struct(N, lens, displ2, temp, &rcv);
    MPI_Type_commit(&rcv);


    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            C[i][j] = 0;

    if (rank == 0)
    {
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                A[i][j] = i * N + j;
        
         for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
                printf("%d ", A[i][j]);
            printf("\n");
        }

        for (int i = 1; i < size; i++)
        {
            MPI_Send(&A[0][0], 1, snd, i, 0, MPI_COMM_WORLD);
        }
    }
    else
    {
        MPI_Recv(&C[0][N - 1], 1, rcv, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        printf("\n");
        printf("C \n");
        printf("\n");
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
                printf("%d ", C[i][j]);
            printf("\n");
        }
    }

    MPI_Finalize();
}