#include <stdio.h>
#include <mpi/mpi.h>
#define N 4
#define M 2

int main(int argc, char *argv[])
{
    int buf[N][M];
    int rank, size;
    MPI_File fh;
    MPI_Datatype dt;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    for (int i = 0; i < N; i++)
        for (int j = 0; j < M; j++)
            buf[i][j] = rank;

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
            printf("%d ", buf[i][j]);
        printf("\n");
    }

    MPI_Type_vector(M, M, N, MPI_INT, &dt);
    MPI_Type_commit(&dt);

    MPI_File_open(MPI_COMM_WORLD, "test.dat", MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fh);
    MPI_File_set_view(fh, (rank % M * M + rank / M * M * N) * sizeof(int), MPI_INT, dt, "native", MPI_INFO_NULL);
    MPI_File_write_all(fh, &buf[0][0], 2 * M, MPI_INT, MPI_STATUS_IGNORE);
    MPI_File_close(&fh);

    MPI_Finalize();
}