#include <stdio.h>
#include <mpi/mpi.h>

#define N 9
#define M 12

int main(int argc, char *argv[])
{
    //procesa 9
    int A[N][M], a[N / 3][M / 3];
    int rank, size, offset;
    MPI_File fh;
    MPI_Datatype arrblocktype;
    int gsizes[2] = {N, M};
    int distribs[2] = {MPI_DISTRIBUTE_BLOCK, MPI_DISTRIBUTE_BLOCK};
    int dargs[2] = {MPI_DISTRIBUTE_DFLT_DARG, MPI_DISTRIBUTE_DFLT_DARG};
    int psizes[2] = {3, 3};

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Type_create_darray(size, rank, 2, gsizes, distribs, dargs, psizes, MPI_ORDER_C, MPI_INT, &arrblocktype);
    MPI_Type_commit(&arrblocktype);

    for (int i = 0; i < N / 3; i++)
        for (int j = 0; j < M / 3; j++)
            a[i][j] = i*M/3+j + rank;

    MPI_File_open(MPI_COMM_WORLD,"io2.dat",MPI_MODE_CREATE | MPI_MODE_WRONLY,MPI_INFO_NULL,&fh);
    MPI_File_set_view(fh,0,MPI_INT,arrblocktype,"native",MPI_INFO_NULL);
    int matsize = N/3*M/3;
    MPI_File_write_all(fh,&a[0][0],matsize,MPI_INT,MPI_STATUS_IGNORE);
    MPI_File_close(&fh);

    MPI_Finalize();
}