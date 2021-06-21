#include <stdio.h>
#include <mpi/mpi.h>
#define DIMBUF 10

/// KONZISTENTNOST

int main(int argc, char *argv[])
{
    MPI_File fh;
    int buf[DIMBUF];
    int rdbuf[DIMBUF];
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    for (int i = 0; i < DIMBUF; i++)
        buf[i] = i + DIMBUF * rank;

    MPI_File_open(MPI_COMM_WORLD, "io4.dat", MPI_MODE_CREATE | MPI_MODE_RDWR, MPI_INFO_NULL, &fh);

    // ****************** PRVI NACIN ******************
    // MPI_File_set_atomicity(fh, 1);
    // for (int i = 0; i < size; i++)
    // {
    //     if (rank == i)
    //         MPI_File_write_at(fh, sizeof(int) * DIMBUF * rank, buf, 10, MPI_INT, MPI_STATUS_IGNORE);
    //     MPI_Barrier(MPI_COMM_WORLD);
    //     if (rank != i)
    //     {
    //         MPI_File_read_at(fh, sizeof(int) * i * DIMBUF, rdbuf, 10, MPI_INT, MPI_STATUS_IGNORE);
    //         for (int i = 0; i < DIMBUF; i++)
    //             printf("%d ", rdbuf[i]);
    //         printf("\n");
    //     }

    //     MPI_Barrier(MPI_COMM_WORLD);
    // }

    
    // ****************** DRUGI NACIN ******************
    for (int i = 0; i < size; i++)
    {
        if (rank == i)
            MPI_File_write_at(fh, sizeof(int) * DIMBUF * rank, buf, 10, MPI_INT, MPI_STATUS_IGNORE);
        MPI_File_close(&fh);
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_File_open(MPI_COMM_WORLD, "io4.dat", MPI_MODE_CREATE | MPI_MODE_RDWR, MPI_INFO_NULL, &fh);
        if (rank != i)
        {
            MPI_File_read_at(fh, sizeof(int) * i * DIMBUF, rdbuf, 10, MPI_INT, MPI_STATUS_IGNORE);
            for (int i = 0; i < DIMBUF; i++)
                printf("%d ", rdbuf[i]);
            printf("\n");
        }
        MPI_Barrier(MPI_COMM_WORLD);

    }



    // ****************** TRECI NACIN ******************

    // for (int i = 0; i < size; i++)
    // {
    //     if (rank == i)
    //         MPI_File_write_at(fh, sizeof(int) * DIMBUF * rank, buf, 10, MPI_INT, MPI_STATUS_IGNORE);

    //     MPI_File_sync(fh);
    //     MPI_Barrier(MPI_COMM_WORLD);
    //     if (rank != i)
    //     {
    //         MPI_File_read_at(fh, sizeof(int) * i * DIMBUF, rdbuf, 10, MPI_INT, MPI_STATUS_IGNORE);
    //         for (int i = 0; i < DIMBUF; i++)
    //             printf("%d ", rdbuf[i]);
    //         printf("\n");
    //     }

    //     MPI_File_sync(fh);
    //     MPI_Barrier(MPI_COMM_WORLD);
    // }

    MPI_File_close(&fh);

    MPI_Finalize();
}