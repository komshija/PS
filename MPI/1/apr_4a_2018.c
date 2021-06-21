#include <stdio.h>
#include <mpi/mpi.h>
#include <math.h>

#define N 8
// nesto brljavi zadatak, ima neka sitna greska, trenutno je ne vidim


// P2P
/*
int main(int argc, char *argv[])
{
    int rank, size;
    int res, temp, sndrcv;
    int count = log2(N);
    int start = 1;

    int x;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (rank == 0)
        printf("count %d\n", count);

    x = rank;
    temp = rank;
    res = rank;

    for (int i = 0; i < count; i++)
    {
        sndrcv = rank ^ start;
        MPI_Sendrecv(&x, 1, MPI_INT, sndrcv, 0, &x, 1, MPI_INT, sndrcv, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        temp += x;
        if (sndrcv < rank)
        {
            res += x;
        }
        
        printf("i = %d :: rank = %d :: res = %d :: x = %d temp = %d\n", i, rank, res, x, temp);

        x = temp;
        start = start << 1;
    }

    printf("Suma do %d je %d\n", rank, res);

    MPI_Finalize();
}*/
 
// grupne

int main(int argc, char *argv[])
{
    int rank, size;
    int res, x;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    res = rank;

    for (int i = 0; i < size; i++)
    {
        x = i == rank ? rank : 0;
        MPI_Bcast(&x, 1 , MPI_INT, i,MPI_COMM_WORLD);
        if(i < rank)
            res += x;
    }

    printf("Suma do %d je %d\n", rank, res);

    MPI_Finalize();
}