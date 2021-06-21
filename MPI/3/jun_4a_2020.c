#include <stdio.h>
#include <mpi/mpi.h>
#define N 3
#define M 4

int main(int argc, char *argv[])
{
    int rank, size;
    MPI_Comm cartcom;
    int dims[2] = {M, N};
    int period[2] = {1, 0};
    int coords[2];
    int levi, desni;
    int x;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, period, 0, &cartcom);
    MPI_Cart_coords(cartcom, rank, 2, coords);
    MPI_Cart_shift(cartcom, 0, coords[1], &levi, &desni);
    x = coords[0] + coords[1];

    printf("PRE: [%d,%d] = %d\n", coords[1], coords[0], x);
    MPI_Sendrecv_replace(&x, 1, MPI_INT, desni, 0, levi, 0, cartcom, MPI_STATUS_IGNORE);
    printf("POSLE: [%d,%d] = %d\n", coords[1], coords[0], x);

    MPI_Finalize();
}