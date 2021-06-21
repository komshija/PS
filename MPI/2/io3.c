#include <stdio.h>
#include <mpi/mpi.h>

int main(int argc, char *argv[])
{
    int size, rank,buf[10];
    MPI_File fh;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Svi upisu random redom

    // MPI_File_open(MPI_COMM_WORLD,"io3.dat",MPI_MODE_CREATE | MPI_MODE_WRONLY,MPI_INFO_NULL,&fh);
    // MPI_File_write_shared(fh,&rank,1,MPI_INT,MPI_STATUS_IGNORE);
    // MPI_File_close(&fh);

    // upisu rednom po rankovima

    // MPI_File_open(MPI_COMM_WORLD,"io3.dat",MPI_MODE_CREATE | MPI_MODE_WRONLY,MPI_INFO_NULL,&fh);
    // MPI_File_write_ordered(fh,&rank,1,MPI_INT,MPI_STATUS_IGNORE);
    // MPI_File_close(&fh);


    MPI_Finalize();
}